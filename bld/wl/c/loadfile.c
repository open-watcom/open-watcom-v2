/****************************************************************************
*
*                            Open Watcom Project
*
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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*
   LOADFILE : utilities for processing creation of load files

*/

#include <string.h>
#include <process.h>
#include <malloc.h>
#include <stdlib.h>
#include "linkstd.h"
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
#include "spillio.h"
#include "omfreloc.h"
#include "objcalc.h"
#include "dbgall.h"
#include "loadpe.h"
#include "loados2.h"
#include "loaddos.h"
#include "pharlap.h"
#include "loadnov.h"
#include "loadqnx.h"
#include "loadelf.h"
#include "loadfile.h"
#include "objstrip.h"
#include "impexp.h"
#include "objnode.h"
#include "strtab.h"
#include "permdata.h"

seg_leader *    StackSegPtr;
startinfo       StartInfo;

#define IMPLIB_BUFSIZE 4096

typedef struct {
    f_handle    handle;
    char *      fname;
    char *      buffer;
    unsigned    bufsize;
    char *      dllname;
    size_t      dlllen;
    unsigned    didone : 1;
} implibinfo;

static implibinfo       ImpLib;

extern void ResetLoadFile( void )
/*******************************/
{
    ClearStartAddr();
}

extern void CleanLoadFile( void )
/*******************************/
{
}

extern void InitLoadFile( void )
/******************************/
/* open the file, and write out header info */
{
    DEBUG(( DBG_OLD, "InitLoadFile()" ));
    LnkMsg( INF+MSG_CREATE_EXE, "f" );
}

extern void FiniLoadFile( void )
/******************************/
/* terminate writing of load file */
{
    CurrSect = Root;
    FreeSavedRelocs();
    OpenOutFiles();
    SetupImpLib();
    if( FmtData.type & MK_REAL_MODE ) {
        FiniDOSLoadFile();
#ifdef _OS2
    } else if( IS_PPC_OS2 ) {
        // development temporarly on hold:
        // FiniELFLoadFile();
    } else if( FmtData.type & MK_OS2_FLAT ) {
        FiniOS2FlatLoadFile();
    } else if( FmtData.type & MK_PE ) {
        FiniPELoadFile();
    } else if( FmtData.type & MK_OS2_16BIT ) {
        FiniOS2LoadFile();
#endif
#ifdef _PHARLAP
    } else if( FmtData.type & MK_PHAR_LAP ) {
        FiniPharLapLoadFile();
#endif
#ifdef _NOVELL
    } else if( FmtData.type & MK_NOVELL ) {
        FiniNovellLoadFile();
#endif
#ifdef _QNXLOAD
    } else if( FmtData.type & MK_QNX ) {
        FiniQNXLoadFile();
#endif
#ifdef _ELF
    } else if( FmtData.type & MK_ELF ) {
        FiniELFLoadFile();
#endif
    }
    MapSizes();
    CloseOutFiles();
    DoCVPack();
}

static void DoCVPack( void )
/**************************/
{
    int         retval;
    char *      name;

    if( LinkFlags & CVPACK_FLAG && !(LinkState & LINK_ERROR) ) {
        if( SymFileName != NULL ) {
            name = SymFileName;
        } else {
            name = Root->outfile->fname;
        }
        retval = spawnlp( P_WAIT, "cvpack.exe", "cvpack.exe", "/nologo",
                          name, NULL );
        if( retval == -1 ) {
            PrintIOError( ERR+MSG_CANT_EXECUTE, "12", "cvpack.exe" );
        }
    }
}

static seg_leader * FindStack( class_entry *class )
/*************************************************/
{
    while( class != NULL ) {
        if( class->flags & CLASS_STACK ) {
            return RingFirst( class->segs );
        }
        class = class->next_class;
    }
    return( NULL );
}

static seg_leader *StackSegment( void )
/*************************************/
/* Find stack segment. */
{
    seg_leader  *seg;

    seg = FindStack( Root->classlist );
    if( seg == NULL ) {
        if( FmtData.type & MK_OVERLAYS ) {
            seg = FindStack( NonSect->classlist );
        }
    }
    return( seg );
}

extern void GetStkAddr( void )
/****************************/
/* Find the address of the stack */
{
    if( !(FmtData.type & MK_NOVELL) && !FmtData.dll ) {
        if( StackSegPtr != NULL ) {
            StackAddr.seg = StackSegPtr->seg_addr.seg;
            StackAddr.off = StackSegPtr->seg_addr.off + StackSegPtr->size;
        } else {
#ifdef _OS2
            if( FmtData.type & MK_WINDOWS && LinkFlags & STK_SIZE_FLAG ) {
                PhoneyStack();
            } else
#endif
                    if( !(FmtData.type & (MK_COM|MK_PE|MK_QNX|MK_ELF)) ) {
                LnkMsg( WRN+MSG_STACK_NOT_FOUND, NULL );
                StackAddr.seg = 0;
                StackAddr.off = 0;
            }
        }
    }
}

static class_entry * LocateBSSClass( void )
/*****************************************/
{
    class_entry *currclass;

    currclass = ((Root->areas == NULL) ? Root : NonSect)->classlist;
    for(;;) {
        if( currclass == NULL ) return( NULL );
        if( stricmp( currclass->name, BSSClassName ) == 0 ) return( currclass );
        currclass = currclass->next_class;
    }
}

static void DefABSSSym( char *name )
/**********************************/
{
   symbol          *sym;

    sym = RefISymbol( name );
    if( !(sym->info & SYM_DEFINED) || sym->info & SYM_LINK_GEN ) {
        sym->info |= SYM_DEFINED | SYM_LINK_GEN;
        if( FmtData.type & MK_OVERLAYS ) {
            sym->u.d.ovlstate |= OVL_NO_VECTOR | OVL_FORCE;
        }
        sym->addr.seg = UNDEFINED;
        sym->addr.off = 0;
    }
 }

extern void DefBSSSyms( void )
/****************************/
{
    DefABSSSym( BSSStartSym );
    DefABSSSym( BSS_StartSym );
    DefABSSSym( BSSEndSym );
    DefABSSSym( BSS_EndSym );
}

static bool CompSymPtr( symbol *sym, symbol *chk )
/************************************************/
{
    return chk == sym;
}

static void CheckBSSInStart( symbol * sym, char * name )
/******************************************************/
/* It's OK to define _edata if:
        1) the DOSSEG flag is not set
                or
        2) the definition occurs in the module containing the
            start addresses */
{
    symbol *    chk;

    chk = NULL;
    if( StartInfo.mod != NULL ) {
        chk = Ring2Lookup( StartInfo.mod->publist, CompSymPtr, sym );
    }
    if( chk == NULL ) {
        LnkMsg( ERR+MSG_RESERVED_SYM_DEFINED, "s", name );
    }
}

static void DefBSSStartSize( char * name, class_entry * class )
/*************************************************************/
/* set the value of an start symbol, and see if it has been defined */
{
    symbol *    sym;
    seg_leader *seg;

    sym = FindISymbol( name );
    if( sym->addr.seg == UNDEFINED ) {
        /* if the symbol was defined internally */
        seg = (seg_leader *) RingFirst( class->segs );
        sym->p.seg = (segdata *) RingFirst( seg->pieces );
        sym->addr = seg->seg_addr;
        ConvertToFrame( &sym->addr, seg->group->grp_addr.seg );
    } else if( LinkState & DOSSEG_FLAG ) {
        CheckBSSInStart( sym, name );
    }
}

static void DefBSSEndSize( char * name, class_entry * class )
/***********************************************************/
/* set the value of an end symbol, and see if it has been defined */
{
    symbol *    sym;
    seg_leader *seg;

    sym = FindISymbol( name );
    if( sym->addr.seg == UNDEFINED ) {
        /* if the symbol was defined internally */
        /* find last segment in BSS class */
        seg = (seg_leader *) RingLast( class->segs );
        /* set end of BSS class */
        sym->p.seg = (segdata *) RingLast( seg->pieces );
        sym->addr.seg = seg->seg_addr.seg;
        sym->addr.off = seg->seg_addr.off + seg->size;
        ConvertToFrame( &sym->addr, seg->group->grp_addr.seg );
    } else if( LinkFlags & DOSSEG_FLAG ) {
        CheckBSSInStart( sym, name );
    }
}

extern void GetBSSSize( void )
/****************************/
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

extern void SetStkSize( void )
/****************************/
{
    StackSegPtr = StackSegment();
    if( StackSize < 0x200 ) StackSize = 0x200;
    if( StackSegPtr != NULL ) {
        if( LinkFlags & STK_SIZE_FLAG ) {
            if( !(FmtData.type & MK_NOVELL) ) {
                StackSegPtr->size = StackSize;
            }
        } else if( StackSegPtr->size >= 0x200 ) {
            StackSize = StackSegPtr->size;
        } else {
            StackSegPtr->size = StackSize;
        }
    }
}

extern void ClearStartAddr( void )
/********************************/
{
    memset( &StartInfo, 0, sizeof(startinfo) );
}

extern void SetStartSym( char *name )
/***********************************/
{
    size_t      namelen;

    if( StartInfo.type != START_UNDEFED ) {
        if( StartInfo.type == START_IS_SYM ) {
            namelen = strlen( name );
            if( namelen != strlen(StartInfo.targ.sym->name)
                || CmpRtn( StartInfo.targ.sym->name, name, namelen ) != 0 ) {
                LnkMsg( LOC+ERR+MSG_MULT_START_ADDRS_BY, "12",
                            StartInfo.targ.sym->name, name );
            }
        } else {
            LnkMsg( LOC+ERR+MSG_MULT_START_ADDRS, NULL );
        }
    } else {
        StartInfo.targ.sym = RefISymbol( name );
        StartInfo.targ.sym->info |= SYM_DCE_REF;
        StartInfo.type = START_IS_SYM;
        StartInfo.mod = CurrMod;
        if( LinkFlags & STRIP_CODE ) {
            DataRef( StartInfo.targ.sym );
        }
    }
}

extern void GetStartAddr( void )
/******************************/
{
    bool        addoff;

    if( FmtData.type & MK_NOVELL ) return;
    addoff = TRUE;
    switch( StartInfo.type ) {
    case START_UNDEFED:         // NOTE: the possible fall through
        addoff = FALSE;
        if( !FmtData.dll ) {
            if( Groups == NULL || FmtData.type & MK_ELF ) {
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
        break;
    case START_IS_SYM:
        StartInfo.addr = StartInfo.targ.sym->addr;
        break;
    }
    if( addoff ) {
        StartInfo.addr.off += StartInfo.off;
    }
}

extern offset CalcGroupSize( group_entry *group )
/***********************************************/
/* calculate the total memory size of a potentially split group */
{
    offset size;

    if( group == DataGroup && FmtData.dgroupsplitseg != NULL ) {
        size = FmtData.dgroupsplitseg->seg_addr.off - group->grp_addr.off
                                                    - FmtData.bsspad;
        DbgAssert( size >= group->size );
    } else {
        size = group->totalsize;
    }
    return size;
}

extern offset CalcSplitSize( void )
/*********************************/
/* calculate the size of the uninitialized portion of a group */
{
    offset size;

    size = DataGroup->totalsize + PE_BSS_SHIFT
           - (FmtData.dgroupsplitseg->seg_addr.off - DataGroup->grp_addr.off);
    if( StackSegPtr != NULL ) {
        size -= StackSize;
    }
    return size;
}

extern bool CompareDosSegments( targ_addr *left, targ_addr *right )
/*****************************************************************/
{
    return LESS_THAN_ADDR( *left, *right );
}

extern bool CompareOffsets( targ_addr *left, targ_addr *right )
/*****************************************************************/
{
    return left->off < right->off;
}

extern bool CompareProtSegments( targ_addr *left, targ_addr *right )
/*****************************************************************/
{
    if( left->seg == right->seg ) {
        return left->off < right->off;
    }
    return left->seg < right->seg;
}

extern void OrderGroups( bool (*lessthan)(targ_addr *, targ_addr *) )
/*******************************************************************/
{
    group_entry     *group, *low_group, *firstgroup, **lastgroup;
    targ_addr *     low_addr;
    targ_addr *     grp_addr;

    firstgroup = Groups;
    lastgroup = &Groups;
    while( firstgroup != NULL ) {
        low_addr = &firstgroup->grp_addr;
        low_group = NULL;
        for( group = firstgroup; group->next_group != NULL;
                                 group = group->next_group ) {
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

extern bool WriteDOSGroup( group_entry *group )
/*********************************************/
/* write the data for group to the loadfile */
/* returns TRUE if the file should be repositioned */
{
    unsigned long       loc;
    signed  long        diff;
    section *           sect;
    bool                repos;
    outfilelist *       finfo;

    repos = FALSE;
    if( group->size != 0 ) {
        sect = group->section;
        CurrSect = sect;
        finfo = sect->outfile;
        loc = SUB_ADDR( group->grp_addr, sect->sect_addr ) + sect->u.file_loc;
        diff = loc - finfo->file_loc;
        if( diff > 0 ) {
            PadLoad( diff );
        } else if( diff != 0 ) {
            SeekLoad( loc );
            repos = TRUE;
        }
        if( group == OvlGroup ) {
            OvlTabOffset = loc;
        }
        DEBUG((DBG_LOADDOS, "group %a section %d to %l in %s",
                &group->grp_addr, sect->ovl_num, loc, finfo->fname ));
        WriteGroupLoad( group );
        loc += group->size;
        if( loc > finfo->file_loc ) {
            finfo->file_loc = loc;
        }
    }
    return( repos );
}

extern unsigned_32 MemorySize( void )
/***********************************/
/* Compute size of image when loaded into memory. */
{
    unsigned_32         start;
    unsigned_32         end;
    unsigned_32         curr;
    section             *sect;
    ovl_area            *ovl;

    if( !(FmtData.type & MK_REAL_MODE) ) {
        return( Root->size );
    } else {
        start = MK_REAL_ADDR( Root->sect_addr.seg, Root->sect_addr.off );
        end = start + Root->size;
        for( ovl = Root->areas; ovl != NULL; ovl = ovl->next_area ) {
            for( sect = ovl->sections; sect != NULL; sect = sect->next_sect ) {
                curr = MK_REAL_ADDR( sect->sect_addr.seg,
                         sect->sect_addr.off ) + sect->size;
                if( curr > end ) end = curr;
            }
        }
        return( end - start );
    }
}

extern unsigned_32 AppendToLoadFile( char * name )
/************************************************/
{
    f_handle        handle;
    unsigned_32     wrote;

    if( name != NULL ) {
        handle = QOpenR( name );
        wrote = CopyToLoad( handle, name );
    } else {
        wrote = 0;
    }
    return( wrote );
}

static void SetupImpLib( void )
/*****************************/
{
    char *      fname;
    int         namelen;

    ImpLib.bufsize = 0;
    ImpLib.handle = NIL_HANDLE;
    ImpLib.buffer = NULL;
    ImpLib.dllname = NULL;
    ImpLib.didone = FALSE;
    if( FmtData.make_implib ) {
        _ChkAlloc( ImpLib.buffer, IMPLIB_BUFSIZE );
        if( FmtData.make_impfile ) {
            ImpLib.fname = ChkStrDup( FmtData.implibname );
            ImpLib.handle = QOpenRW( ImpLib.fname );
        } else {
            ImpLib.handle = OpenTempFile( &ImpLib.fname );
        }
        fname = RemovePath( Root->outfile->fname, &namelen );
        ImpLib.dlllen = namelen;
        if( !(FmtData.type & MK_OS2_FLAT) ) {
            ImpLib.dlllen += 4;
        }
        _ChkAlloc( ImpLib.dllname, ImpLib.dlllen );
        memcpy( ImpLib.dllname, fname, namelen );
        if( !(FmtData.type & MK_OS2_FLAT) ) {
            memcpy( ImpLib.dllname + namelen, ".dll", 4 );
        }
    }
}

extern void BuildImpLib( void )
/*****************************/
{
    if( LinkState & LINK_ERROR || ImpLib.handle == NIL_HANDLE
                                || !FmtData.make_implib ) return;
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
    _LnkFree( ImpLib.dllname );
}

#if _LINKER ==  _DLLHOST
extern bool ExecWlibDLL( char * );

static void ExecWlib( void )
/**************************/
{
    char *      cmdline;
    char *      temp;
    size_t      namelen;
    size_t      impnamelen;

    namelen = strlen(ImpLib.fname);
    impnamelen = strlen(FmtData.implibname);
/*
 * in the following: +12 for options, +2 for spaces, +1 for @, +4 for quotes
 *                  and +1 for nullchar
*/
    _ChkAlloc( cmdline, namelen + impnamelen +12 +2 +1 +4 +1 );
    memcpy( cmdline, "-b -n -q -ii \"", 14 );
    temp = cmdline + 11;
    if( LinkState & HAVE_ALPHA_CODE ) {
        *temp = 'a';
    } else if( LinkState & HAVE_PPC_CODE ) {
        *temp = 'p';
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
    if( ExecWlibDLL( cmdline ) ) {
        PrintIOError( ERR+MSG_CANT_EXECUTE, "12", "wlib.exe" );
    }
    _LnkFree( cmdline );
}
#else
static void ExecWlib( void )
/**************************/
{
    char *      atfname;
    size_t      namelen;
    int         retval;
    char *      libtype;

    namelen = strlen(ImpLib.fname) + 1;
    _ChkAlloc( atfname, namelen + 1 );  // +1 for the @
    *atfname = '@';
    memcpy( atfname + 1, ImpLib.fname, namelen );
    if( LinkState & HAVE_ALPHA_CODE ) {
        libtype = "-ia";
    } else if( LinkState & HAVE_PPC_CODE ) {
        libtype = "-ip";
    } else {
        libtype = "-ii";
    }
    retval = spawnlp( P_WAIT, "wlib.exe", "wlib.exe", "-b", "-n", "-q",
                      libtype, FmtData.implibname, atfname, NULL );
    if( retval == -1 ) {
        PrintIOError( ERR+MSG_CANT_EXECUTE, "12", "wlib.exe" );
    }
    _LnkFree( atfname );
}
#endif

extern void AddImpLibEntry( char *intname, char *extname, unsigned ordinal )
/**************************************************************************/
{
    size_t      intlen;
    size_t      otherlen;
    char *      buff;
    char *      currpos;

    if( ImpLib.handle == NIL_HANDLE ) return;
    ImpLib.didone = TRUE;
    intlen = strlen( intname );
    if( ordinal == NOT_IMP_BY_ORDINAL ) {
        otherlen = strlen(extname);
    } else {
        otherlen = 10;          // max length of a 32-bit int.
    }
    buff = alloca( intlen + otherlen + ImpLib.dlllen + 13 );
    buff[0] = '+';
    buff[1] = '+';
    buff[2] = '\'';
    currpos = buff + 3;
    memcpy( currpos, intname, intlen );
    currpos += intlen;
    *currpos++ = '\'';
    *currpos++ = '.';
    *currpos++ = '\'';
    memcpy( currpos, ImpLib.dllname, ImpLib.dlllen );
    currpos += ImpLib.dlllen;
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
#if _OS != _QNX
    *currpos++ = '\r';
#endif
    *currpos = '\n';
    BufImpWrite( buff, currpos - buff + 1 );
}

static void FlushImpBuffer( void )
/********************************/
{
    QWrite( ImpLib.handle, ImpLib.buffer, ImpLib.bufsize, ImpLib.fname );
}

static void BufImpWrite( char *buffer, int len )
/**********************************************/
{
    int     diff;

    diff = ImpLib.bufsize + len - IMPLIB_BUFSIZE;
    if( diff >= 0 ) {
        memcpy( ImpLib.buffer + ImpLib.bufsize , buffer, len - diff );
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

extern void WriteLoad3( void* dummy, void *buff, unsigned size )
/**************************************************************/
/* write a buffer out to the load file (useful as a callback) */
{
    dummy = dummy;
    WriteLoad( buff, size );
}

extern unsigned_32 CopyToLoad( f_handle handle, char * name )
/***********************************************************/
{
    unsigned_32     amt_read;
    unsigned_32     wrote;

    wrote = 0;
    for(;;) {
        amt_read = QRead( handle, TokBuff, TokSize, name );
        if( amt_read <= 0 ) break;
        WriteLoad( TokBuff, amt_read );
        wrote += amt_read;
    }
    QClose( handle, name );
    return( wrote );
}

extern unsigned long NullAlign( unsigned align )
/**********************************************/
/* align loadfile -- assumed power of two alignment */
{
    unsigned long       off;
    unsigned            pad;

    off = PosLoad();
    align--;
    pad = ( (off+align) & ~(unsigned long)align ) - off;
    PadLoad( pad );
    return( off + pad );
}

extern unsigned long OffsetAlign( unsigned long off, unsigned long align )
/************************************************************************/
/* align loadfile -- assumed power of two alignment */
{
    unsigned long       pad;

    align--;
    pad = ( (off+align) & ~align ) - off;
    PadLoad( pad );
    return( off + pad );
}

static bool WriteSegData( segdata *sdata, unsigned long *start )
/**************************************************************/
{
    unsigned long newpos;
    signed long pad;

    if( !sdata->isuninit && !sdata->isdead && sdata->length > 0 ) {
        newpos = *start + sdata->a.delta;
        pad = newpos - PosLoad();
        DbgAssert( pad >= 0 );
        PadLoad( pad );
        WriteInfo( sdata->data, sdata->length );
        sdata->data = newpos;   // for incremental linking
    }
    return FALSE;
}

static void DoWriteLeader( seg_leader *seg, unsigned long start )
/***************************************************************/
{
    RingLookup( seg->pieces, WriteSegData, &start );
}

extern void WriteLeaderLoad( seg_leader *seg )
/********************************************/
{
    DoWriteLeader( seg, PosLoad() );
}

static bool DoGroupLeader( seg_leader *seg, unsigned long *start )
/****************************************************************/
{
    DoWriteLeader( seg, *start + GetLeaderDelta( seg ) );
    return FALSE;
}

extern void WriteGroupLoad( group_entry *group )
/**********************************************/
{
    unsigned long       pos;

    pos = PosLoad();
    Ring2Lookup( group->leaders, DoGroupLeader, &pos );
}

static void OpenOutFiles( void )
/******************************/
{
    outfilelist * fnode;

    fnode = OutFiles;   // skip the root
    while( fnode != NULL ) {
        OpenBuffFile( fnode );
        fnode = fnode->next;
    }
}

static void CloseOutFiles( void )
/*******************************/
{
    outfilelist *   fnode;

    fnode = OutFiles;
    while( fnode != NULL ) {
        if( fnode->handle != NIL_HANDLE ) {
            CloseBuffFile( fnode );
        }
        fnode = fnode->next;
    }
}

extern void FreeOutFiles( void )
/******************************/
{
    outfilelist *   fnode;

    CloseOutFiles();
    fnode = OutFiles;
    while( fnode != NULL ) {
        if( LinkState & LINK_ERROR ) {
            QDelete( fnode->fname );
        }
        _LnkFree( fnode->fname );
        OutFiles = fnode->next;
        _LnkFree( fnode );
        fnode = OutFiles;
    }
}

static void * SetToZero( char *dest, const char *dummy, unsigned size )
/*********************************************************************/
{
    memset( dest, 0, size );
    return (void *) dummy;
}

extern void PadLoad( unsigned long size )
/***************************************/
/* pad out load file with zeros */
{
    outfilelist *       outfile;

    if( size == 0 ) return;
    outfile = CurrSect->outfile;
    if( outfile->buffer != NULL ) {
        WriteBuffer( NULL, size, outfile, SetToZero );
    } else {
        WriteNulls( outfile->handle, size, outfile->fname );
    }
}

extern void WriteLoad( void *buff, unsigned long size )
/*****************************************************/
/* write a buffer out to the load file */
{
    outfilelist *       outfile;

    outfile = CurrSect->outfile;
    if( outfile->buffer != NULL ) {
        WriteBuffer( buff, size, outfile, memcpy );
    } else {
        QWrite( outfile->handle, buff, size, outfile->fname );
    }
}

static void * NullBuffFunc( char *dest, const char *dummy, unsigned size )
/************************************************************************/
{
    dummy = dummy;
    size = size;
    return dest;
}

extern void SeekLoad( unsigned long offset )
/******************************************/
{
    outfilelist *       outfile;

    outfile = CurrSect->outfile;
    if( outfile->buffer != NULL && offset < outfile->bufpos ) {
        FlushBuffFile( outfile );
    }
    if( outfile->buffer == NULL ) {
        QSeek( outfile->handle, offset, outfile->fname );
    } else {
        WriteBuffer( NULL, offset - outfile->bufpos, outfile, NullBuffFunc );
    }
}

extern void SeekEndLoad( unsigned long offset )
/*********************************************/
{
    outfilelist *       outfile;

    outfile = CurrSect->outfile;
    if( outfile->buffer != NULL && offset > 0 ) {
        FlushBuffFile( outfile );
    }
    if( outfile->buffer == NULL ) {
        QLSeek( outfile->handle, offset, LSEEK_END, outfile->fname );
    }
}

extern unsigned long PosLoad( void )
/**********************************/
{
    if( CurrSect->outfile->buffer != NULL ) {
        return CurrSect->outfile->bufpos;
    } else {
        return QPos( CurrSect->outfile->handle );
    }
}

#define BUFF_BLOCK_SIZE (16*1024)

extern void InitBuffFile( outfilelist *outfile, char *filename )
/**************************************************************/
{
    outfile->fname = filename;
    outfile->handle = NIL_HANDLE;
    outfile->file_loc = 0;
    outfile->bufpos = 0;
    outfile->buffer = NULL;
}

extern void OpenBuffFile( outfilelist *outfile )
/**********************************************/
{
    outfile->handle = ExeCreate( outfile->fname );
    if( outfile->handle == NIL_HANDLE ) {
        PrintIOError( FTL+MSG_CANT_OPEN_NO_REASON, "s", outfile->fname );
    }
    _ChkAlloc( outfile->buffer, BUFF_BLOCK_SIZE );
}

static void FlushBuffFile( outfilelist *outfile )
/***********************************************/
{
    unsigned    modpos;

    modpos = outfile->bufpos % BUFF_BLOCK_SIZE;
    if( modpos != 0 ) {
        QWrite( outfile->handle, outfile->buffer, modpos, outfile->fname );
    }
    _LnkFree( outfile->buffer );
    outfile->buffer = NULL;
}

extern void CloseBuffFile( outfilelist *outfile )
/***********************************************/
{
    if( outfile->buffer != NULL ) {
        FlushBuffFile( outfile );
    }
    QClose( outfile->handle, outfile->fname );
    outfile->handle = NIL_HANDLE;
}

static void WriteBuffer( char *info, unsigned long len, outfilelist *outfile,
                         void * (*rtn)(char *, const char *, unsigned) )
/***************************************************************************/
{
    unsigned modpos;
    unsigned adjust;

    modpos = outfile->bufpos % BUFF_BLOCK_SIZE;
    outfile->bufpos += len;
    while( modpos + len >= BUFF_BLOCK_SIZE ) {
        adjust = BUFF_BLOCK_SIZE - modpos;
        rtn( outfile->buffer + modpos, info, adjust );
        QWrite( outfile->handle, outfile->buffer, BUFF_BLOCK_SIZE,
                outfile->fname );
        info += adjust;
        len -= adjust;
        modpos = 0;
    }
    if( len > 0 ) {
        rtn( outfile->buffer + modpos, info, len );
    }
}
