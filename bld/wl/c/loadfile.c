/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Utilities for processing creation of load files.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "walloca.h"
#include "linkstd.h"
#if !defined( __UNIX__ ) || defined(__WATCOMC__)
#include <process.h>
#endif
#include "ring.h"
#include "pcobj.h"
#include "newmem.h"
#include "msg.h"
#include "alloc.h"
#include "specials.h"
#include "wlnkmsg.h"
#include "virtmem.h"
#include "fileio.h"
#include "mapio.h"
#include "omfreloc.h"
#include "objcalc.h"
#include "dbgall.h"
#include "loadpe.h"
#include "loados2.h"
#include "loaddos.h"
#include "loadphar.h"
#include "loadnov.h"
#include "load16m.h"
#include "loadqnx.h"
#include "loadelf.h"
#include "loadzdos.h"
#include "loadrdv.h"
#include "loadraw.h"
#include "loadfile.h"
#include "objstrip.h"
#include "impexp.h"
#include "objnode.h"
#include "strtab.h"
#include "permdata.h"
#include "ideentry.h"
#include "overlays.h"

#include "clibext.h"


#define IMPLIB_BUFSIZE  _4KB

typedef struct {
    f_handle    handle;
    char        *fname;
    char        *buffer;
    size_t      bufsize;
    char        *module_name;
    size_t      module_name_len;
    boolbit     didone  : 1;
} implibinfo;

typedef struct  {
    unsigned_32 grp_start;
    unsigned_32 seg_start;
    group_entry *lastgrp;       // used only for copy classes
    boolbit     repos   : 1;
    boolbit     copy    : 1;
} grpwriteinfo;

typedef void *writebuffer_fn(void *, const void *, size_t);

seg_leader      *StackSegPtr;
startinfo       StartInfo;

static implibinfo       ImpLib;

#define TEMPFNAME       "IMP02112.xx`"          // "'" will be an "a" when processed.
#define TEMPFNAME_SIZE  13

static char *makeTempName( char *name )
/*************************************/
{
    memcpy( name, TEMPFNAME, sizeof( TEMPFNAME ) ); // includes nullchar
    return( name + sizeof( TEMPFNAME ) - 2 );       // pointer to "a"
}

static f_handle openTempFile( char **fname )
/******************************************/
{
    const char  *ptr;
    size_t      tlen;
    char        *tptr;
    f_handle    fhdl;

    ptr = GetEnvString( "TMP" );
    if( ptr == NULL )
        ptr = GetEnvString( "TMPDIR" );
    if( ptr == NULL ) {
        _ChkAlloc( tptr, TEMPFNAME_SIZE );
        *fname = tptr;
    } else {
        tlen = strlen( ptr );
        _ChkAlloc( tptr, tlen + 1 + TEMPFNAME_SIZE );
        memcpy( tptr, ptr, tlen );
        *fname = tptr;
        tptr += tlen;
        if( !IS_PATH_SEP( tptr[-1] ) ) {
            *tptr++ = DIR_SEP;
        }
    }
    tptr = makeTempName( tptr );
    tlen = 0;
    for( ;; ) {
        if( tlen >= 26 ) {
            LnkMsg( FTL+MSG_TMP_ALREADY_EXISTS, NULL );
        }
        *tptr += 1;                     // change temp file extension
        fhdl = TempFileOpen( *fname );
        if( fhdl == NIL_FHANDLE )
            break;
        QClose( fhdl, *fname );
        ++tlen;
    }
    return( QOpenRW( *fname ) );
}

static void SetupImpLib( void )
/*****************************/
{
    const char  *fname;
    size_t      namelen;

    ImpLib.bufsize = 0;
    ImpLib.handle = NIL_FHANDLE;
    ImpLib.buffer = NULL;
    ImpLib.module_name = NULL;
    ImpLib.didone = false;
    if( FmtData.make_implib ) {
        _ChkAlloc( ImpLib.buffer, IMPLIB_BUFSIZE );
        if( FmtData.make_impfile ) {
            ImpLib.fname = ChkStrDup( FmtData.implibname );
            ImpLib.handle = QOpenRW( ImpLib.fname );
        } else {
            ImpLib.handle = openTempFile( &ImpLib.fname );
        }
        /* GetBaseName results in the filename only   *
         * it trims both the path, and the extension */
        fname = GetBaseName( Root->outfile->fname, 0, &namelen );
        ImpLib.module_name_len = namelen;
        /*
         * increase length to restore full extension if not OS2
         * sometimes the extension of the output name is important
         */
        if( (FmtData.type & MK_OS2_16BIT) == 0 )
            ImpLib.module_name_len += strlen( fname + namelen );
        _ChkAlloc( ImpLib.module_name, ImpLib.module_name_len );
        memcpy( ImpLib.module_name, fname, ImpLib.module_name_len );
    }
}

#if defined( __UNIX__ )
#define CVPACK_EXE "cvpack"
#else
#define CVPACK_EXE "cvpack.exe"
#endif

static void DoCVPack( void )
/**************************/
{
#if !defined( __UNIX__ ) || defined(__WATCOMC__)
    int         retval;
    char        *name;

    if( (LinkFlags & LF_CVPACK_FLAG) && (LinkState & LS_LINK_ERROR) == 0 ) {
        if( SymFileName != NULL ) {
            name = SymFileName;
        } else {
            name = Root->outfile->fname;
        }
        retval = (int)spawnlp( P_WAIT, CVPACK_EXE, CVPACK_EXE, "/nologo",
                          name, NULL );
        if( retval == -1 ) {
            PrintIOError( ERR+MSG_CANT_EXECUTE, "12", CVPACK_EXE );
        }
    }
#endif
}

static void OpenOutFiles( void )
/******************************/
{
    outfilelist   *fnode;

    for( fnode = OutFiles; fnode != NULL; fnode = fnode->next ) {
        OpenBuffFile( fnode );
    }
}

static void CloseOutFiles( void )
/*******************************/
{
    outfilelist     *fnode;

    for( fnode = OutFiles; fnode != NULL; fnode = fnode->next ) {
        if( fnode->handle != NIL_FHANDLE ) {
            CloseBuffFile( fnode );
        }
    }
}

void ResetLoadFile( void )
/************************/
{
    ClearStartAddr();
}

void CleanLoadFile( void )
/************************/
{
}

void InitLoadFile( void )
/***********************/
/* open the file, and write out header info */
{
    DEBUG(( DBG_OLD, "InitLoadFile()" ));
    LnkMsg( INF+MSG_CREATE_EXE, "f" );
}

static void finiLoad( void )
/**************************/
{
    /*******************************************************************
     * This must be first of all
     */
#ifdef _RAW
    if( FmtData.output_raw ) {
        BinOutput();    // apply to all formats and override native output
        return;
    } else if( FmtData.output_hex ) {
        HexOutput();    // apply to all formats and override native output
        return;
    } else if( FmtData.type & MK_RAW ) {
        FiniRawLoadFile();
        return;
    }
#endif
    /*******************************************************************/
#ifdef _EXE
    if( FmtData.type & MK_DOS ) {
        FiniDOSLoadFile();
        return;
    }
#endif
#ifdef _OS2
  #if 0
    if( (FmtData.type & MK_OS2) && (LinkState & LS_HAVE_PPC_CODE) ) {
        // development temporarly on hold:
        FiniELFLoadFile();
        return;
    }
  #endif
    if( FmtData.type & MK_OS2_FLAT ) {
        FiniOS2FlatLoadFile();
        return;
    } else if( FmtData.type & MK_PE ) {
        FiniPELoadFile();
        return;
    } else if( FmtData.type & MK_OS2_16BIT ) {
        FiniOS2LoadFile();
        return;
    }
#endif
#ifdef _PHARLAP
    if( FmtData.type & MK_PHAR_LAP ) {
        FiniPharLapLoadFile();
        return;
    }
#endif
#ifdef _NOVELL
    if( FmtData.type & MK_NOVELL ) {
        FiniNovellLoadFile();
        return;
    }
#endif
#ifdef _DOS16M
    if( FmtData.type & MK_DOS16M ) {
        Fini16MLoadFile();
        return;
    }
#endif
#ifdef _QNX
    if( FmtData.type & MK_QNX ) {
        FiniQNXLoadFile();
        return;
    }
#endif
#ifdef _ELF
    if( FmtData.type & MK_ELF ) {
        FiniELFLoadFile();
        return;
    }
#endif
#ifdef _ZDOS
    if( FmtData.type & MK_ZDOS ) {
        FiniZdosLoadFile();
        return;
    }
#endif
#ifdef _RDOS
    if( FmtData.type & MK_RDOS ) {
        FiniRdosLoadFile();
        return;
    }
#endif
}

void FiniLoadFile( void )
/***********************/
/* terminate writing of load file */
{
    CurrSect = Root;
    FreeSavedRelocs();
    OpenOutFiles();
    SetupImpLib();
    finiLoad();
    MapSizes();
    CloseOutFiles();
    DoCVPack();
}

static seg_leader *FindStack( section *sect )
/*******************************************/
{
    class_entry *class;
    seg_leader  *seg;

    for( class = sect->classlist; class != NULL; class = class->next_class ) {
        if( class->flags & CLASS_STACK ) {
            return( RingFirst( class->segs ) );
        }
    }
    if( (LinkState & LS_DOSSEG_FLAG) == 0 ) {
        seg = NULL;
        for( class = sect->classlist; class != NULL; class = class->next_class ) {
            while( (seg = RingStep( class->segs, seg )) != NULL ) {
                if( seg->combine == COMBINE_STACK ) {
                    return( seg );
                }
            }
        }
    }
    return( NULL );
}

static seg_leader *StackSegment( void )
/*************************************/
/* Find stack segment. */
{
    seg_leader  *seg;

    seg = FindStack( Root );
    if( seg == NULL ) {
#ifdef _EXE
        if( FmtData.type & MK_OVERLAYS ) {
            seg = FindStack( NonSect );
        }
#endif
    }
    return( seg );
}

void GetStkAddr( void )
/*********************/
/* Find the address of the stack */
{
    if( (FmtData.type & MK_NOVELL) == 0 && !FmtData.dll ) {
        if( StackSegPtr != NULL ) {
            StackAddr.seg = StackSegPtr->seg_addr.seg;
            StackAddr.off = StackSegPtr->seg_addr.off + StackSegPtr->size;
        } else {
#ifdef _OS2
            if( (FmtData.type & MK_WINDOWS) && (LinkFlags & LF_STK_SIZE_FLAG) ) {
                PhoneyStack();
            } else {
#endif
                if( (FmtData.type & (MK_COM | MK_PE | MK_QNX | MK_ELF | MK_RDOS)) == 0 ) {
                    LnkMsg( WRN+MSG_STACK_NOT_FOUND, NULL );
                    StackAddr.seg = 0;
                    StackAddr.off = 0;
                }
#ifdef _OS2
            }
#endif
        }
    }
}

static class_entry *LocateBSSClass( void )
/****************************************/
{
    class_entry *class;
    section     *sect;

    sect = (Root->areas == NULL) ? Root : NonSect;
    for( class = sect->classlist; class != NULL; class = class->next_class ) {
        if( stricmp( class->name.u.ptr, BSSClassName ) == 0 ) {
            return( class );
        }
    }
    return( NULL );
}

static void DefABSSSym( const char *name )
/****************************************/
{
   symbol          *sym;

    sym = RefISymbol( name );
    if( (sym->info & SYM_DEFINED) == 0 || (sym->info & SYM_LINK_GEN) ) {
        sym->info |= SYM_DEFINED | SYM_LINK_GEN;
#ifdef _EXE
        if( FmtData.type & MK_OVERLAYS ) {
            sym->u.d.ovlstate |= OVL_NO_VECTOR | OVL_FORCE;
        }
#endif
        SET_ADDR_UNDEFINED( sym->addr );
    }
 }

void DefBSSSyms( void )
/*********************/
{
    DefABSSSym( BSSStartSym );
    DefABSSSym( BSS_StartSym );
    DefABSSSym( BSSEndSym );
    DefABSSSym( BSS_EndSym );
}

static bool CompSymPtr( void *sym, void *chk )
/********************************************/
{
    return( chk == sym );
}

static void CheckBSSInStart( symbol *sym, const char *name )
/***********************************************************
 * It's OK to define _edata if:
 *      1) the DOSSEG flag is not set
 *              or
 *      2) the definition occurs in the module containing the
 *          start addresses
 */
{
    symbol      *chk;

    chk = NULL;
    if( StartInfo.mod != NULL ) {
        chk = Ring2Lookup( StartInfo.mod->publist, CompSymPtr, sym );
    }
    if( chk == NULL ) {
        LnkMsg( ERR+MSG_RESERVED_SYM_DEFINED, "s", name );
    }
}

static void DefBSSStartSize( const char *name, class_entry *class )
/*****************************************************************/
/* set the value of an start symbol, and see if it has been defined */
{
    symbol      *sym;
    seg_leader  *seg;

    sym = FindISymbol( name );
    if( IS_ADDR_UNDEFINED( sym->addr ) ) {
        /* if the symbol was defined internally */
        seg = (seg_leader *)RingFirst( class->segs );
        sym->p.seg = (segdata *)RingFirst( seg->pieces );
        sym->addr = seg->seg_addr;
        ConvertToFrame( &sym->addr, seg->group->grp_addr.seg, ( (seg->info & USE_32) == 0 ) );
    } else if( LinkState & LS_DOSSEG_FLAG ) {
        CheckBSSInStart( sym, name );
    }
}

static void DefBSSEndSize( const char *name, class_entry *class )
/***************************************************************/
/* set the value of an end symbol, and see if it has been defined */
{
    symbol      *sym;
    seg_leader  *seg;

    sym = FindISymbol( name );
    if( IS_ADDR_UNDEFINED( sym->addr ) ) {
        /* if the symbol was defined internally */
        /* find last segment in BSS class */
        seg = (seg_leader *)RingLast( class->segs );
        /* set end of BSS class */
        sym->p.seg = (segdata *)RingLast( seg->pieces );
        SET_SYM_ADDR( sym, seg->seg_addr.off + seg->size, seg->seg_addr.seg );
        ConvertToFrame( &sym->addr, seg->group->grp_addr.seg, ( (seg->info & USE_32) == 0 ) );
    } else if( LinkState & LS_DOSSEG_FLAG ) {
        CheckBSSInStart( sym, name );
    }
}

void GetBSSSize( void )
/*********************/
/* Find size of BSS segment, and set the special symbols */
{
    class_entry *class;

    class = LocateBSSClass();
    if( class != NULL ) {
        /* set start of BSS class */
        DefBSSStartSize( BSSStartSym, class );
        DefBSSStartSize( BSS_StartSym, class );
        DefBSSEndSize( BSSEndSym, class );
        DefBSSEndSize( BSS_EndSym, class );
    }
}

void SetStkSize( void )
/**********************
 * Stack size calculation:
 * - DLLs have no stack
 * - for executables, warn if stack size is tiny
 * - if stack size was given, use it directly unless target is Novell
 * - else use the actual stack segment size if it is > 512 bytes
 * - otherwise use the default stack size
 * The default stack size is 4096 bytes, but for DOS programs the
 * stack segment size in the clib is smaller, hence the complex logic.
 */
{
    StackSegPtr = StackSegment();
    if( FmtData.dll ) {
        StackSize = 0;  // DLLs don't have their own stack
    } else {
        if( LinkFlags & LF_STK_SIZE_FLAG ) {
            if( StackSize < 0x200 ) {
                LnkMsg( WRN+MSG_STACK_SMALL, "d", 0x200 );
            }
#ifdef _OS2
        } else {
            if( FmtData.type & MK_PE ) {
                StackSize = DefStackSizePE();
                LinkFlags |= LF_STK_SIZE_FLAG;
            }
#endif
        }
    }
    if( StackSegPtr != NULL ) {
        if( LinkFlags & LF_STK_SIZE_FLAG ) {
            if( (FmtData.type & MK_NOVELL) == 0 ) {
                StackSegPtr->size = StackSize;
            }
        } else {
            if( !FmtData.dll && StackSegPtr->size >= 0x200 ) {
                StackSize = StackSegPtr->size;
            } else {
                StackSegPtr->size = StackSize;
            }
        }
    }
}

void ClearStartAddr( void )
/*************************/
{
    memset( &StartInfo, 0, sizeof( startinfo ) );
}

void SetStartSym( const char *name )
/**********************************/
{
    size_t      namelen;

    if( StartInfo.type != START_UNDEFED ) {
        if( StartInfo.type == START_IS_SYM ) {
            namelen = strlen( name );
            if( namelen != strlen( StartInfo.targ.sym->name.u.ptr ) || CmpRtn( StartInfo.targ.sym->name.u.ptr, name, namelen ) != 0 ) {
                LnkMsg( LOC+MILD_ERR+MSG_MULT_START_ADDRS_BY, "12", StartInfo.targ.sym->name.u.ptr, name );
            }
        } else {
            LnkMsg( LOC+MILD_ERR+MSG_MULT_START_ADDRS, "12", StartInfo.mod->f.source->infile->name, StartInfo.mod->name );
        }
    } else {
        StartInfo.targ.sym = RefISymbol( name );
        StartInfo.targ.sym->info |= SYM_DCE_REF;
        StartInfo.type = START_IS_SYM;
        StartInfo.mod = CurrMod;
        if( LinkFlags & LF_STRIP_CODE ) {
            DataRef( StartInfo.targ.sym );
        }
    }
}

void GetStartAddr( void )
/***********************/
{
    bool        addoff;
    int         deltaseg;

#ifdef _NOVELL
    if( FmtData.type & MK_NOVELL ) {
        return;
    }
#endif
    addoff = true;
    switch( StartInfo.type ) {
    case START_UNDEFED:         // NOTE: the possible fall through
        addoff = false;
        if( !FmtData.dll ) {
            if( Groups == NULL || (FmtData.type & MK_ELF) ) {
                StartInfo.addr.seg = 0;
                StartInfo.addr.off = 0;
            } else {
                StartInfo.addr = Groups->grp_addr;
            }
            LnkMsg( WRN+MSG_NO_START_ADDR, "a", &StartInfo.addr );
        }
        break;
    case START_IS_SDATA:
        StartInfo.addr = StartInfo.targ.sdata->u.leader->seg_addr;
        StartInfo.addr.off += StartInfo.targ.sdata->a.delta;
        /* if startaddr is not in first segment and segment is part of */
        /* a group, adjust seg + off relative to start of group        */
        /* instead of start of seg. This allows far call optimization to work*/
        if( (StartInfo.targ.sdata->u.leader->seg_addr.seg > 0) &&
            (StartInfo.targ.sdata->u.leader->group != NULL) ) {

            deltaseg = StartInfo.targ.sdata->u.leader->seg_addr.seg
              - StartInfo.targ.sdata->u.leader->group->grp_addr.seg;
            if( (deltaseg > 0) && (deltaseg <= StartInfo.targ.sdata->u.leader->seg_addr.seg) ) {
                StartInfo.addr.seg -= deltaseg;
                StartInfo.addr.off += 16 * deltaseg - StartInfo.targ.sdata->u.leader->group->grp_addr.off;
            }
        }
        break;
    case START_IS_SYM:
        StartInfo.addr = StartInfo.targ.sym->addr;
        break;
    }
    if( addoff ) {
        StartInfo.addr.off += StartInfo.off;
    }
}

offset CalcGroupSize( group_entry *group )
/****************************************/
/* calculate the total memory size of a potentially split group */
{
    offset size;

    if(( group == DataGroup ) && ( FmtData.dgroupsplitseg != NULL )) {
        size = FmtData.dgroupsplitseg->seg_addr.off - group->grp_addr.off - FmtData.bsspad;
        DbgAssert( size >= group->size );
    } else {
        size = group->totalsize;
    }
    return( size );
}

offset CalcSplitSize( void )
/**************************/
/* calculate the size of the uninitialized portion of a group */
{
    offset size;

    if( FmtData.dgroupsplitseg == NULL ) {
        return( 0 );
    } else {
        size = DataGroup->totalsize - (FmtData.dgroupsplitseg->seg_addr.off - DataGroup->grp_addr.off);
        if( StackSegPtr != NULL ) {
            size -= StackSize;
        }
        return( size );
    }
}

bool CompareDosSegments( targ_addr *left, targ_addr *right )
/**********************************************************/
{
    return( LESS_THAN_ADDR( *left, *right ) );
}

bool CompareOffsets( targ_addr *left, targ_addr *right )
/******************************************************/
{
    return( left->off < right->off );
}

bool CompareProtSegments( targ_addr *left, targ_addr *right )
/***********************************************************/
{
    if( left->seg == right->seg ) {
        return( left->off < right->off );
    }
    return( left->seg < right->seg );
}

void OrderGroups( bool (*lessthan)(targ_addr *, targ_addr *) )
/************************************************************/
{
    group_entry     *group, *low_group, *firstgroup, **lastgroup;
    targ_addr       *low_addr;
    targ_addr       *grp_addr;

    firstgroup = Groups;
    lastgroup = &Groups;
    while( firstgroup != NULL ) {
        low_addr = &firstgroup->grp_addr;
        low_group = NULL;
        for( group = firstgroup; group->next_group != NULL; group = group->next_group ) {
            grp_addr =  &group->next_group->grp_addr;
            if( lessthan( grp_addr, low_addr ) ) {
                low_addr = grp_addr;
                low_group = group;
            }
        }
        if( low_group == NULL ) {
            *lastgroup = firstgroup;
            lastgroup = &(firstgroup->next_group);
            firstgroup = *lastgroup;
        } else {
            *lastgroup = low_group->next_group;
            lastgroup = &(low_group->next_group->next_group);
            low_group->next_group = *lastgroup;
        }
    }
}

bool WriteGroup( group_entry *group )
/***********************************/
/* write the data for group to the loadfile */
/* returns true if the file should be repositioned */
{
    unsigned long       file_loc;
    section             *sect;
    bool                repos;
    outfilelist         *finfo;

    repos = false;
    if( group->size != 0 ) {
        sect = group->section;
        CurrSect = sect;
        finfo = sect->outfile;
        file_loc = GROUP_FILE_LOC( group );
        if( file_loc > finfo->file_loc ) {
            PadLoad( file_loc - finfo->file_loc );
        } else if( file_loc != finfo->file_loc ) {
            SeekLoad( file_loc );
            repos = true;
        }
#ifdef _EXE
        if( FmtData.type & MK_OVERLAYS ) {
            OvlSetTableLoc( group, file_loc );
        }
#endif
        DEBUG((DBG_LOADDOS, "group %a section %d to %l in %s",
                &group->grp_addr, sect->ovlref, file_loc, finfo->fname ));
        file_loc += WriteGroupLoad( group, repos );
        if( file_loc > finfo->file_loc ) {
            finfo->file_loc = file_loc;
        }
    }
    return( repos );
}

unsigned_32 MemorySize( void )
/****************************/
/* Compute size of image when loaded into memory. */
{
#ifdef _EXE
    unsigned_32         start;
    unsigned_32         end;
    unsigned_32         curr;
    section             *sect;
    ovl_area            *ovl;

    if( FmtData.type & MK_REAL_MODE ) {
        start = MK_REAL_ADDR( Root->sect_addr.seg, Root->sect_addr.off );
        end = start + Root->size;
        for( ovl = Root->areas; ovl != NULL; ovl = ovl->next_area ) {
            for( sect = ovl->sections; sect != NULL; sect = sect->next_sect ) {
                curr = MK_REAL_ADDR( sect->sect_addr.seg, sect->sect_addr.off ) + sect->size;
                if( curr > end ) {
                    end = curr;
                }
            }
        }
        return( end - start );
    }
#endif
    return( Root->size );
}

unsigned_32 AppendToLoadFile( const char *name )
/**********************************************/
{
    f_handle        handle;
    unsigned_32     wrote;

    wrote = 0;
    if( name != NULL ) {
        handle = QOpenR( name );
        wrote = CopyToLoad( handle, name );
        QClose( handle, name );
    }
    return( wrote );
}

#ifdef BOOTSTRAP
#define WLIB_NAME   "bwlib"
#else
#define WLIB_NAME   "wlib"
#endif
#if defined( DLLS_IMPLEMENTED )
#define WLIB_EXE WLIB_NAME "d.dll"
#elif defined( __UNIX__ )
#define WLIB_EXE WLIB_NAME
#else
#define WLIB_EXE WLIB_NAME ".exe"
#endif

static void ExecWlib( void )
/**************************/
{
#if defined( DLLS_IMPLEMENTED )
    char        *cmdline;
    char        *temp;
    size_t      namelen;
    size_t      impnamelen;

    namelen = strlen( ImpLib.fname );
    impnamelen = strlen( FmtData.implibname );
    /*
     * in the following: +19 for options, +2 for spaces, +1 for @, +4 for quotes
     *                  and +1 for nullchar
     */
    _ChkAlloc( cmdline, namelen + impnamelen +19 +2 +1 +4 +1 );
    memcpy( cmdline, "-c -b -n -q -pa -ii \"", 19 + 2 );
    temp = cmdline + 19 - 1;
    if( LinkState & LS_HAVE_ALPHA_CODE ) {
        *temp = 'a';
    } else if( LinkState & LS_HAVE_PPC_CODE ) {
        *temp = 'p';
    } else if( LinkState & LS_HAVE_MIPS_CODE ) {
        *temp = 'm';
    } else if( LinkState & LS_HAVE_X64_CODE ) {
        *temp = '6';
    }
    temp += 3;
    memcpy( temp, FmtData.implibname, impnamelen );
    temp += impnamelen;
    *temp++ = '"';
    *temp++ = ' ';
    *temp++ = '@';
    *temp++ = '"';
    memcpy( temp, ImpLib.fname, namelen );
    temp += namelen;
    *temp++ = '"';
    *temp = '\0';
    if( ExecDLLPgm( WLIB_EXE, cmdline ) ) {
        PrintIOError( ERR+MSG_CANT_EXECUTE, "12", WLIB_EXE );
    }
    _LnkFree( cmdline );
#else
    char        *atfname;
    size_t      namelen;
    int         retval;
    char        *libtype;

    namelen = strlen( ImpLib.fname ) + 1;
    _ChkAlloc( atfname, namelen + 1 );  // +1 for the @
    *atfname = '@';
    memcpy( atfname + 1, ImpLib.fname, namelen );
    if( LinkState & LS_HAVE_ALPHA_CODE ) {
        libtype = "-ia";
    } else if( LinkState & LS_HAVE_PPC_CODE ) {
        libtype = "-ip";
    } else if( LinkState & LS_HAVE_MIPS_CODE ) {
        libtype = "-im";
    } else if( LinkState & LS_HAVE_X64_CODE ) {
        libtype = "-i6";
    } else {
        libtype = "-ii";
    }
    retval = (int)spawnlp( P_WAIT, WLIB_EXE, WLIB_EXE, "-c", "-b", "-n", "-q", "-pa",
                  libtype, FmtData.implibname, atfname, NULL );
    if( retval == -1 ) {
        PrintIOError( ERR+MSG_CANT_EXECUTE, "12", WLIB_EXE );
    }
    _LnkFree( atfname );
#endif
}

static void FlushImpBuffer( void )
/********************************/
{
    QWrite( ImpLib.handle, ImpLib.buffer, ImpLib.bufsize, ImpLib.fname );
}

void BuildImpLib( void )
/*****************************/
{
    if( (LinkState & LS_LINK_ERROR) || ImpLib.handle == NIL_FHANDLE || !FmtData.make_implib )
        return;
    if( ImpLib.bufsize > 0 ) {
        FlushImpBuffer();
    }
    QClose( ImpLib.handle, ImpLib.fname );
    if( !FmtData.make_impfile ) {
        if( ImpLib.didone ) {
            ExecWlib();
        }
        QDelete( ImpLib.fname );
    }
    _LnkFree( FmtData.implibname );
    _LnkFree( ImpLib.fname );
    _LnkFree( ImpLib.buffer );
    _LnkFree( ImpLib.module_name );
}

static void BufImpWrite( const char *buffer, size_t len )
/*******************************************************/
{
    size_t      diff;

    if( ImpLib.bufsize + len >= IMPLIB_BUFSIZE ) {
        diff = ImpLib.bufsize + len - IMPLIB_BUFSIZE;
        memcpy( ImpLib.buffer + ImpLib.bufsize , buffer, IMPLIB_BUFSIZE - ImpLib.bufsize );
        ImpLib.bufsize = IMPLIB_BUFSIZE;
        FlushImpBuffer();
        ImpLib.bufsize = diff;
        if( diff > 0 ) {
            memcpy( ImpLib.buffer, buffer + len - diff, diff );
        }
    } else {
        memcpy( ImpLib.buffer + ImpLib.bufsize, buffer, len );
        ImpLib.bufsize += len;
    }
}

void AddImpLibEntry( const char *intname, const char *extname, ordinal_t ordinal )
/********************************************************************************/
{
    size_t      intlen;
    size_t      otherlen;
    char        *buff;
    char        *currpos;

    if( ImpLib.handle == NIL_FHANDLE )
        return;
    ImpLib.didone = true;
    intlen = strlen( intname );
    if( ordinal == NOT_IMP_BY_ORDINAL ) {
        otherlen = strlen( extname );
    } else {
        otherlen = 10;          // max length of a 32-bit int.
    }
    buff = alloca( intlen + otherlen + ImpLib.module_name_len + 13 );
    buff[0] = '+';
    buff[1] = '+';
    buff[2] = '\'';
    currpos = buff + 3;
    memcpy( currpos, intname, intlen );
    currpos += intlen;
    *currpos++ = '\'';
    *currpos++ = '.';
    *currpos++ = '\'';
    memcpy( currpos, ImpLib.module_name, ImpLib.module_name_len );
    currpos += ImpLib.module_name_len;
    *currpos++ = '\'';
    *currpos++ = '.';
    if( ordinal == NOT_IMP_BY_ORDINAL ) {
        *currpos++ = '.';
        *currpos++ = '\'';
        memcpy( currpos, extname, otherlen );
        currpos += otherlen;
        *currpos++ = '\'';
    } else {
        ultoa( ordinal, currpos, 10 );
        currpos += strlen( currpos );
    }
#if !defined( __UNIX__ )
    *currpos++ = '\r';
#endif
    *currpos = '\n';
    BufImpWrite( buff, currpos - buff + 1 );
}

void WriteLoad3( void *dummy, const char *buff, size_t size )
/***********************************************************/
/* write a buffer out to the load file (useful as a callback) */
{
    /* unused parameters */ (void)dummy;

    WriteLoad( buff, size );
}

unsigned_32 CopyToLoad( f_handle handle, const char *name )
/*********************************************************/
{
    size_t          amt_read;
    unsigned_32     wrote;

    wrote = 0;
    amt_read = QRead( handle, TokBuff, TokSize, name );
    while( amt_read != 0 && amt_read != IOERROR ) {
        WriteLoad( TokBuff, amt_read );
        wrote += amt_read;
        amt_read = QRead( handle, TokBuff, TokSize, name );
    }
    return( wrote );
}

unsigned long NullAlign( unsigned align )
/***************************************/
/* align loadfile -- assumed power of two alignment */
{
    unsigned long   off;
    size_t          pad;

    off = PosLoad();
    pad = ROUND_UP( off, align ) - off;
    PadLoad( pad );
    return( off + pad );
}

unsigned long OffsetAlign( unsigned long off, unsigned long align )
/*****************************************************************/
/* align loadfile -- assumed power of two alignment */
{
    size_t          pad;

    pad = ROUND_UP( off, align ) - off;
    PadLoad( pad );
    return( off + pad );
}

static bool WriteSegData( void *_sdata, void *_info )
/***************************************************/
{
    segdata         *sdata = _sdata;
    grpwriteinfo    *info = _info;
    unsigned long   newpos;
    unsigned long   oldpos;

    if( !sdata->isuninit && !sdata->isdead && ( sdata->length > 0 ) ) {
        newpos = info->seg_start + sdata->a.delta;
        if( info->repos ) {
            SeekLoad( newpos );
        } else {
            oldpos = PosLoad();
            DbgAssert( newpos >= oldpos );
            PadLoad( newpos - oldpos );
        }
        if( sdata->vm_data == 0 ) {
            sdata->vm_data = sdata->u1.vm_ptr;
        }
        if( !info->copy ) {
            sdata->u1.vm_offs = newpos;   // for incremental linking
        }
        WriteInfoLoad( sdata->vm_data, sdata->length );
    }
    return( false );
}

static void DoWriteLeader( seg_leader *seg, grpwriteinfo *info )
/**************************************************************/
{
    RingLookup( seg->pieces, WriteSegData, info );
}

void WriteLeaderLoad( void *seg )
/*******************************/
{
    grpwriteinfo    info;

    info.repos = false;
    info.copy = false;
    info.seg_start = PosLoad();
    DoWriteLeader( seg, &info );
}

static bool DoGroupLeader( void *_seg, void *_info )
/**************************************************/
{
    seg_leader      *seg = _seg;
    grpwriteinfo    *info = _info;

    // If class or sector should not be output, skip it
    if( EMIT_CLASS( seg->class ) && EMIT_SEG( seg ) ) {
        info->seg_start = info->grp_start + SEG_GROUP_DELTA( seg );
        DoWriteLeader( seg, info );
    }
    return( false );
}

static bool DoDupGroupLeader( void *seg, void *_info )
/****************************************************/
// Substitute groups generally are sourced from NO_EMIT classes,
// As copies, they need to be output, so ignore their MOEMIT flag here
{
    grpwriteinfo    *info = _info;

    info->seg_start = info->grp_start + SEG_GROUP_DELTA( (seg_leader *)seg );
    DoWriteLeader( seg, info );
    return( false );
}

static bool WriteCopyGroups( void *_seg, void *_info )
/****************************************************/
// This is called by the outer level iteration looking for classes
//  that have more than one group in them
{
    seg_leader      *seg = _seg;
    grpwriteinfo    *info = _info;

    if( info->lastgrp != seg->group ) {   // Only interate new groups
        info->lastgrp = seg->group;
        // Check each initialized segment in group
        Ring2Lookup( seg->group->leaders, DoDupGroupLeader, info );
        info->grp_start += seg->group->totalsize;
    }
    return( false );
}

offset  WriteGroupLoad( group_entry *group, bool repos )
/******************************************************/
{
    grpwriteinfo     info;
    class_entry      *class;
    unsigned_32      grp_start;

    grp_start = PosLoad();
    info.repos = repos;
    info.grp_start = grp_start;
    // If group is a copy group, substitute source group(s) here
    class = group->leaders->class;
    if( class->flags & CLASS_COPY ) {
        info.copy = true;
        info.lastgrp = NULL; // so it will use the first group
        RingLookup( class->DupClass->segs->group->leaders, WriteCopyGroups, &info );
    } else {
        info.copy = false;
        Ring2Lookup( group->leaders, DoGroupLeader, &info );
    }
    return( PosLoad() - grp_start );
}

void FreeOutFiles( void )
/***********************/
{
    outfilelist     *fnode;

    CloseOutFiles();
    for( fnode = OutFiles; fnode != NULL; fnode = OutFiles ) {
        if( LinkState & LS_LINK_ERROR ) {
            QDelete( fnode->fname );
        }
        _LnkFree( fnode->fname );
        OutFiles = fnode->next;
        _LnkFree( fnode );
    }
}

static void *SetToFillChar( void *dest, const void *dummy, size_t size )
/**********************************************************************/
{
    memset( dest, FmtData.FillChar, size );
    return( (void *)dummy );
}

#define BUFF_BLOCK_SIZE _16KB

static void WriteBuffer( const char *data, size_t len, outfilelist *outfile, writebuffer_fn *rtn )
/************************************************************************************************/
{
    size_t   modpos;
    size_t   adjust;

    modpos = outfile->bufpos % BUFF_BLOCK_SIZE;
    outfile->bufpos += len;
    while( modpos + len >= BUFF_BLOCK_SIZE ) {
        adjust = BUFF_BLOCK_SIZE - modpos;
        rtn( outfile->buffer + modpos, data, adjust );
        QWrite( outfile->handle, outfile->buffer, BUFF_BLOCK_SIZE, outfile->fname );
        data += adjust;
        len -= adjust;
        modpos = 0;
    }
    if( len > 0 ) {
        rtn( outfile->buffer + modpos, data, len );
    }
}

static void SeekBuffer( unsigned long len, outfilelist *outfile, writebuffer_fn *rtn )
/************************************************************************************/
{
    size_t   modpos;
    size_t   adjust;

    modpos = outfile->bufpos % BUFF_BLOCK_SIZE;
    outfile->bufpos += len;
    while( modpos + len >= BUFF_BLOCK_SIZE ) {
        adjust = BUFF_BLOCK_SIZE - modpos;
        rtn( outfile->buffer + modpos, NULL, adjust );
        QWrite( outfile->handle, outfile->buffer, BUFF_BLOCK_SIZE, outfile->fname );
        len -= adjust;
        modpos = 0;
    }
    if( len > 0 ) {
        rtn( outfile->buffer + modpos, NULL, len );
    }
}

void PadBuffFile( outfilelist *outfile, size_t size )
/***************************************************/
/* pad out load file with zeros */
{
    if( size == 0 )
        return;
    if( outfile->buffer != NULL ) {
        WriteBuffer( NULL, size, outfile, SetToFillChar );
    } else {
        WriteNulls( outfile->handle, size, outfile->fname );
    }
}

void PadLoad( size_t size )
/*************************/
/* pad out load file with zeros */
{
    PadBuffFile( CurrSect->outfile, size );
}

void WriteLoad( const void *buff, size_t size )
/*********************************************/
/* write a buffer out to the load file */
{
    outfilelist         *outfile;

    outfile = CurrSect->outfile;
    if( outfile->buffer != NULL ) {
        WriteBuffer( buff, size, outfile, memcpy );
    } else {
        QWrite( outfile->handle, buff, size, outfile->fname );
    }
}

void WriteLoadU8( unsigned_8 data )
/*********************************/
{
    WriteLoad( &data, sizeof( data ) );
}

void WriteLoadU16( unsigned_16 data )
/***********************************/
{
    WriteLoad( &data, sizeof( data ) );
}

void WriteLoadU32( unsigned_32 data )
/***********************************/
{
    WriteLoad( &data, sizeof( data ) );
}

size_t WriteLoadU8Name( const char *data, size_t len, bool ucase )
/****************************************************************/
{
    char            buff[255];
    size_t          i;

    if( len > 255 )
        len = 255;
    WriteLoadU8( len );
    if( ucase ) {
        for( i = 0; i < len; ++i ) {
            buff[i] = toupper( data[i] );
        }
        data = buff;
    }
    WriteLoad( data, len );
    return( len + 1 );
}

static void FlushBuffFile( outfilelist *outfile )
/***********************************************/
{
    size_t  modpos;

    modpos = outfile->bufpos % BUFF_BLOCK_SIZE;
    if( modpos != 0 ) {
        QWrite( outfile->handle, outfile->buffer, modpos, outfile->fname );
    }
    _LnkFree( outfile->buffer );
    outfile->buffer = NULL;
}

#if 0
static void *NullBuffFunc( void *dest, const void *dummy, size_t size )
/*********************************************************************/
{
    /* unused parameters */ (void)dummy; (void)size;

    return( dest );
}
#endif

void SeekLoad( unsigned long offset )
/***********************************/
{
    outfilelist         *outfile;

    outfile = CurrSect->outfile;
    if( outfile->buffer != NULL && ( outfile->origin + offset ) < outfile->bufpos ) {
        FlushBuffFile( outfile );
    }
    if( outfile->buffer == NULL ) {
        QSeek( outfile->handle, outfile->origin + offset, outfile->fname );
    } else {
//        SeekBuffer( outfile->origin + offset - outfile->bufpos, outfile, NullBuffFunc );
        SeekBuffer( outfile->origin + offset - outfile->bufpos, outfile, SetToFillChar );
    }
}

void SeekEndLoad( unsigned long offset )
/*********************************************/
{
    outfilelist         *outfile;

    outfile = CurrSect->outfile;
    if( outfile->buffer != NULL && offset > 0 ) {
        FlushBuffFile( outfile );
    }
    if( outfile->buffer == NULL ) {
        QLSeek( outfile->handle, offset, LSEEK_END, outfile->fname );
    }
}

unsigned long PosLoad( void )
/**********************************/
{
    if( CurrSect->outfile->buffer != NULL ) {
        return( CurrSect->outfile->bufpos - CurrSect->outfile->origin );
    } else {
        return( QPos( CurrSect->outfile->handle ) - CurrSect->outfile->origin );
    }
}

void InitBuffFile( outfilelist *outfile, char *filename, bool executable )
/*******************************************************************************/
{
    outfile->fname    = filename;
    outfile->handle   = NIL_FHANDLE;
    outfile->file_loc = 0;
    outfile->bufpos   = 0;
    outfile->buffer   = NULL;
    outfile->ovlfnoff = 0;
    outfile->is_exe   = executable;
    outfile->origin   = 0;
}

void SetOriginLoad( unsigned long origin )
/****************************************/
{
    CurrSect->outfile->origin = origin;
}

void OpenBuffFile( outfilelist *outfile )
/**********************************************/
{
    if( outfile->is_exe ) {
        outfile->handle = ExeCreate( outfile->fname );
    } else {
        outfile->handle = QOpenRW( outfile->fname );
    }
    if( outfile->handle == NIL_FHANDLE ) {
        PrintIOError( FTL+MSG_CANT_OPEN_NO_REASON, "s", outfile->fname );
    }
    _ChkAlloc( outfile->buffer, BUFF_BLOCK_SIZE );
}

void CloseBuffFile( outfilelist *outfile )
/***********************************************/
{
    if( outfile->buffer != NULL ) {
        FlushBuffFile( outfile );
    }
    QClose( outfile->handle, outfile->fname );
    outfile->handle = NIL_FHANDLE;
}
