/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  General routines for producing debugging information in
*               load file.
*
****************************************************************************/


#include <string.h>
#include "linkstd.h"
#include "specials.h"
#include "fileio.h"
#include "virtmem.h"
#include "exeelf.h"
#include "objcalc.h"
#include "loadfile.h"
#include "objio.h"
#include "dbgcomm.h"
#include "dbgall.h"
#include "dbgcv.h"
#include "objfree.h"
#include "overlays.h"
#include "dbgdwarf.h"
#include "dbgwat.h"
#include "ring.h"
#include "loadnov.h"

#include "clibext.h"


typedef struct {
    line_walk_fn *cbfn;
} line_walk_data;

char            *SymFileName;
group_entry     *DBIGroups;

void ResetDBI( void )
/*******************/
{
    SymFileName = NULL;
    DBIGroups = NULL;
}

void DBIInit( void )
/******************/
// called just after command file parsing
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIInit( Root );
    } else if( LinkFlags & LF_CV_DBI_FLAG ) {
        CVInit();
    } else if( LinkFlags & LF_DWARF_DBI_FLAG ) {
        DwarfInit();
    }
}

void DBISectInit( section *sect )
/*******************************/
// called when a section created in command file parsing
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIInit( sect );
    }
}

void DBIInitModule( mod_entry *obj )
/**********************************/
// called before pass 1 is done on the module
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIInitModule( obj );
    } else if( LinkFlags & LF_DWARF_DBI_FLAG ) {
        DwarfInitModule( obj );
    } else if( LinkFlags & LF_CV_DBI_FLAG ) {
        CVInitModule( obj );
    }
}

void DBIP1Source( const byte *buff, const byte *endbuff )
/*******************************************************/
{
    byte        major;
    byte        minor;

    major = *buff++;
    minor = *buff++;
    if( endbuff <= buff ) {
        BadObject();
        return;
    }
    ObjFormat |= FMT_DEBUG_COMENT;
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIP1Source( major, minor, (const char *)buff, endbuff - buff );
    }
}

section *DBIGetSect( const char *clname )
/***************************************/
{
    if( ( stricmp( clname, _MSTypeClass ) == 0 ) || ( stricmp( clname, _MSLocalClass ) == 0 ) ) {
        return( CurrSect );
    } else if( stricmp( clname, _DwarfClass ) == 0 ) {
        return( Root );
    }
    return( NULL );
}

void DBIColClass( class_entry *class )
/************************************/
{
    if( stricmp( class->name.u.ptr, _DwarfClass ) == 0 ) {
        class->flags |= CLASS_DWARF;
    } else if( stricmp( class->name.u.ptr, _MSTypeClass ) == 0 ) {
        class->flags |= CLASS_MS_TYPE;
    } else if( stricmp( class->name.u.ptr, _MSLocalClass ) == 0 ) {
        class->flags |= CLASS_MS_LOCAL;
    }
}

unsigned_16 DBIColSeg( class_entry *class )
/*****************************************/
{
    switch( class->flags & CLASS_DEBUG_INFO ) {
    case CLASS_DWARF:
        if( CurrMod->modinfo & DBI_TYPE ) {
            CurrMod->modinfo |= MOD_DBI_SEEN;
        }
        return( DWARF_DEBUG_OTHER );       // assume other until later.
    case CLASS_MS_TYPE:
        return( MS_TYPE );
    case CLASS_MS_LOCAL:
        return( MS_LOCAL );
    }
    return( NOT_DEBUGGING_INFO );
}

void DBIP1ModuleScanned( void )
/*****************************/
// called in pass 1 when finished looking at a module
// if some segdefs have been delayed due to distributing libraries, this
// will be called twice (once when regular pass 1 is done, once when all
// segdefs processed
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIP1ModuleScanned();
    } else if( LinkFlags & LF_DWARF_DBI_FLAG ) {
        DwarfP1ModuleScanned();
    } else if( LinkFlags & LF_CV_DBI_FLAG ) {
        CVP1ModuleScanned();
    }
}

static bool MSSkip( void )
/************************/
{
    bool        iscv;
    bool        seencmt;

    if( (ObjFormat & FMT_OMF) == 0 ) {
        return( LinkFlags & LF_DWARF_DBI_FLAG );
    } else {
        iscv = ( (LinkFlags & LF_CV_DBI_FLAG) != 0 );
        seencmt = ( (ObjFormat & FMT_DEBUG_COMENT) != 0 );
        return( (iscv ^ seencmt) == 0 || (LinkFlags & LF_DWARF_DBI_FLAG) );
    }
}

bool DBISkip( seg_leader *seg )
/*****************************/
// returns true we should skip processing this segment because we are
// ignoring debugging information
{
    switch( seg->dbgtype ) {
    case MS_TYPE:
        return( (CurrMod->modinfo & DBI_TYPE) == 0 || MSSkip() );
    case MS_LOCAL:
        return( (CurrMod->modinfo & DBI_LOCAL) == 0 || MSSkip() );
    case NOT_DEBUGGING_INFO:
        return( false );
    default:
        return( (CurrMod->modinfo & DBI_TYPE) == 0 || (LinkFlags & LF_DWARF_DBI_FLAG) == 0 );
    }
}

bool DBINoReloc( seg_leader *seg )
/********************************/
// called to see if we should handle a relocation specially.
{
    return( IS_DBG_INFO( seg ) );
}

static void AddNovGlobals( mod_entry *mod )
/*****************************************/
{
#ifdef _NOVELL
    Ring2Walk( mod->publist, NovDBIAddGlobal );
#else
    /* unused parameters */ (void)mod;
#endif
}

void DBIPreAddrCalc( void )
/*************************/
{
    void (*modptr)( mod_entry * );
    void (*segptr)( seg_leader * );

    if( LinkFlags & LF_NOVELL_DBI_FLAG ) {
        WalkMods( AddNovGlobals );
    }
    if( (LinkFlags & LF_ANY_DBI_FLAG) == 0 )
        return;
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        modptr = ODBIP1ModuleFinished;
        segptr = ODBIAddAddrInfo;
    } else if( LinkFlags & LF_DWARF_DBI_FLAG ) {
        modptr = DwarfP1ModuleFinished;
        segptr = DwarfAddAddrInfo;
    } else {
        modptr = CVP1ModuleFinished;
        segptr = CVAddAddrInfo;
    }
    WalkMods( modptr );
    WalkLeaders( segptr );
    if( LinkFlags & LF_DWARF_DBI_FLAG ) {
        WalkMods( DwarfStoreAddrInfo );
    }
}

void DBIAddrInfoScan( seg_leader *seg,
                         void (*initfn)( segdata *, void * ),
                         void (*addfn)( segdata *, offset, offset, void *, bool ),
                         void *cookie )
/********************************************************************************/
{
    segdata     *prev;
    segdata     *curr;
    offset      size;
    bool        isnewmod;

    if( IS_DBG_INFO( seg ) )
        return;
    if( FmtData.dll || (FmtData.type & MK_PE) ) {
        if( seg->class->flags & (CLASS_STACK | CLASS_IDATA) )
            return;
        if( seg->combine == COMBINE_STACK ) {
            if( (LinkState & LS_DOSSEG_FLAG) == 0 ) {
                return;
            }
        }
    }
    prev = RingStep( seg->pieces, NULL );
    for( ;; ) {
        if( prev == NULL )
            return;
        if( !prev->isdead )
            break;
        prev = RingStep( seg->pieces, prev );
    }
    initfn( prev, cookie );
    size = 0;
    for( curr = prev; (curr = RingStep( seg->pieces, curr )) != NULL; ) {
        if( !curr->isdead ) {
            size += curr->a.delta - prev->a.delta;
            isnewmod = ( ( curr->o.mod != prev->o.mod ) && ( size != 0 ) );
            addfn( prev, curr->a.delta, size, cookie, isnewmod );
            if( isnewmod ) {
                size = 0;
            }
            prev = curr;
        }
    }
    size += prev->length;
    addfn( prev, 0, size, cookie, size != 0 );
}

void DBIComment( void )
/*********************/
{
}

void DBIAddModule( mod_entry *obj, section *sect )
/************************************************/
// called just before publics have been assigned addresses between p1 & p2
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIAddModule( obj, sect );
    } else if( LinkFlags & LF_DWARF_DBI_FLAG ) {
        DwarfAddModule( obj, sect );
    } else if( LinkFlags & LF_CV_DBI_FLAG ) {
        CVAddModule( obj, sect );
    }
}

static void DBIGenLocal( void *sdata )
/************************************/
// called during pass 2 segment processing
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIGenLocal( sdata );
    }
}

static void DBIGenLines( mod_entry *mod )
/***************************************/
// called during pass 2 linnum processing
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        DBILineWalk( mod->lines, ODBIGenLines );
    } else if( LinkFlags & LF_DWARF_DBI_FLAG ) {
        DBILineWalk( mod->lines, DwarfGenLines );
    } else if( LinkFlags & LF_CV_DBI_FLAG ) {
        DBILineWalk( mod->lines, CVGenLines );
    }
}

void DBIGenModule( void )
/***********************/
// called at the end of pass2 for a module
{
    if( MOD_NOT_DEBUGGABLE( CurrMod ) )
        return;
    if( LinkFlags & LF_ANY_DBI_FLAG ) {
        Ring2Walk( CurrMod->segs, DBIGenLocal );
        DBIGenLines( CurrMod );
        if( LinkFlags & LF_OLD_DBI_FLAG ) {
            ODBIGenModule();
        } else if( LinkFlags & LF_DWARF_DBI_FLAG ) {
            DwarfGenModule();
        } else {
            CVGenModule();
        }
    }
}

void DBIDefClass( class_entry *class, unsigned_32 size )
/******************************************************/
// called during address calculation
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIDefClass( class, size );
    } else if( LinkFlags & LF_DWARF_DBI_FLAG ) {
        DwarfDefClass( class, size );
    } else if( LinkFlags & LF_CV_DBI_FLAG ) {
        CVDefClass( class, size );
    }
}

void DBIAddLocal( seg_leader *seg, offset length )
/************************************************/
// called during pass 1 final segment processing.
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIAddLocal( seg, length );
    } else if( LinkFlags & LF_CV_DBI_FLAG ) {
        CVAddLocal( seg, length );
    }
}

void DBIModGlobal( void *_sym )
/*****************************/
{
    symbol *sym = _sym;

    if( !IS_SYM_ALIAS( sym ) && (sym->info & SYM_DEAD) == 0 ) {
        if( IS_SYM_IMPORTED( sym )
            || ( sym->p.seg != NULL )
                && !IS_DBG_INFO( sym->p.seg->u.leader )
                && !sym->p.seg->isabs ) {
            DBIAddGlobal( sym );
        }
    }
}

void DBIAddGlobal( symbol *sym )
/******************************/
// called during pass 1 symbol definition
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIAddGlobal( sym );
    } else if( LinkFlags & LF_DWARF_DBI_FLAG ) {
        DwarfAddGlobal( sym );
    } else if( LinkFlags & LF_CV_DBI_FLAG ) {
        CVAddGlobal( sym );
    }
}

void DBIGenGlobal( symbol *sym, section *sect )
/*********************************************/
// called during symbol address calculation (between pass 1 & pass 2)
// also called by loadpe between passes
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIGenGlobal( sym, sect );
    } else if( LinkFlags & LF_DWARF_DBI_FLAG ) {
        DwarfGenGlobal( sym, sect );
    } else if( LinkFlags & LF_CV_DBI_FLAG ) {
        CVGenGlobal( sym, sect );
    }
#ifdef _NOVELL
    if( ( (sym->info & SYM_STATIC) == 0 ) && (LinkFlags & LF_NOVELL_DBI_FLAG) ) {
        NovDBIGenGlobal( sym );
    }
#endif
}

void DBIAddLines( segdata *seg, const void *line, size_t size, bool is32bit )
/***************************************************************************/
// called during pass 1 linnum processing
{
    lineinfo    *info;

    _PermAlloc( info, sizeof( lineinfo ) + size - 1 );
    info->seg = seg;
    info->size = size;
    if( is32bit )
        info->size |= LINE_IS_32BIT;
    memcpy( info->data, line, size );
    RingAppend( &CurrMod->lines, info );
}

unsigned DBICalcLineQty( lineinfo *info )
/***************************************/
{
    unsigned    size;

    size = info->size & ~LINE_IS_32BIT;
    if( info->size & LINE_IS_32BIT ) {
        size /= sizeof( ln_off_386 );
    } else {
        size /= sizeof( ln_off_286 );
    }
    return( size );
}

static bool DoLineWalk( void *info, void *line_walk_cb )
/******************************************************/
{
    if( !((lineinfo *)info)->seg->isdead ) {
        ((line_walk_data *)line_walk_cb)->cbfn( (lineinfo *)info );
    }
    return( false );
}

void DBILineWalk( lineinfo *lines, line_walk_fn *cbfn )
/*****************************************************/
{
    line_walk_data  line_walk_cb;

    line_walk_cb.cbfn = cbfn;
    RingLookup( lines, DoLineWalk, &line_walk_cb );
}

virt_mem DBIAlloc( virt_mem_size size )
/*************************************/
// handy virtual memory allocation routine used inside the debug info generators
{
    if( size == 0 )
        return( 0 );
    return( AllocStg( size ) );
}

void DBIAddrStart( void )
/***********************/
// called after address calculation is done.
{
#ifdef _NOVELL
    if( LinkFlags & LF_NOVELL_DBI_FLAG ) {
        NovDBIAddrStart();
    }
#endif
    if( LinkFlags & LF_CV_DBI_FLAG ) {
        CVAddrStart();
    }
    WalkAllSects( DBIAddrSectStart );
}

void DBIAddrSectStart( section *sect )
/************************************/
// called for each section after address calculation is done.
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIAddrSectStart( sect );
    } else if( LinkFlags & LF_DWARF_DBI_FLAG ) {
        DwarfAddrSectStart( sect );
    }
}

void DBIP2Start( section *sect )
/******************************/
// called for each section just before pass 2 starts
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIP2Start( sect );
    } else if( LinkFlags & LF_DWARF_DBI_FLAG ) {
        SectWalkClass( sect, DwarfGenAddrInfo );
    } else if( LinkFlags & LF_CV_DBI_FLAG ) {
        SectWalkClass( sect, CVGenAddrInfo );
    }
}

void DBIFini( section *sect )
/***************************/
// called after pass 2 is finished, but before load file generation
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIFini( sect );
    } else if( LinkFlags & LF_CV_DBI_FLAG ) {
        CVFini( sect );
    }
}

void DBISectCleanup( section *sect )
/**********************************/
// called when burning down the house
{
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBISectCleanup( sect );
    }
}

void DBICleanup( void )
/*********************/
// called when burning down the house
{
    FreeGroups( DBIGroups );
}

void DBIWrite( void )
/*******************/
// called during load file generation.  It is assumed that the loadfile is
// positioned to the right spot.
{
    outfilelist symfile;
    outfilelist *save;

    if( (LinkFlags & LF_ANY_DBI_FLAG) == 0 )
        return;
    if( LinkFlags & LF_CV_DBI_FLAG ) {
        // write DEBUG_TYPE_MISC: name of file containing the debug info
        if( SymFileName != NULL ) {
            CVWriteDebugTypeMisc( SymFileName );
        } else {
            CVWriteDebugTypeMisc( Root->outfile->fname );
        }
    }
    save = NULL;
    if( SymFileName != NULL ) {
        InitBuffFile( &symfile, SymFileName, false );
        OpenBuffFile( &symfile );
        save = Root->outfile;
        Root->outfile = &symfile;
    }
    if( LinkFlags & LF_OLD_DBI_FLAG ) {
        ODBIWrite();
    } else if( LinkFlags & LF_DWARF_DBI_FLAG ) {
        DwarfWrite();
    } else if( LinkFlags & LF_CV_DBI_FLAG ) {
        CVWrite();
    }
    if( SymFileName != NULL ) {
        CloseBuffFile( &symfile );
        Root->outfile = save;
    }
}
