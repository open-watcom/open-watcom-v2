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
 *  DBGALL -- general routines for producing debugging information in load file
 *
*/

#include <string.h>
#include "linkstd.h"
#include "alloc.h"
#include "specials.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "fileio.h"
#include "virtmem.h"
#include "exeelf.h"
#include "objcalc.h"
#include "loadfile.h"
#include "objio.h"
#include "dbgcv.h"
#include "objfree.h"
#include "overlays.h"
#include "dbgdwarf.h"
#include "dbgwat.h"
#include "ring.h"
#include "loadnov.h"
#include "dbgcomm.h"
#include "dbgall.h"

char *          SymFileName;
group_entry *   DBIGroups;

static void     DBIGenLocal( segdata * );

extern void ResetDBI( void )
/**************************/
{
    SymFileName = NULL;
    DBIGroups = NULL;
}

extern void DBIInit( void )
/*************************/
// called just after command file parsing
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIInit( Root );
    } else if( LinkFlags & CV_DBI_FLAG ) {
        CVInit();
    } else if( LinkFlags & DWARF_DBI_FLAG ) {
        DwarfInit();
    }
}

extern void DBISectInit( section *sect )
/**************************************/
// called when a section created in command file parsing
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIInit( sect );
    }
}

extern void DBIInitModule( mod_entry *obj )
/*****************************************/
// called before pass 1 is done on the module
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIInitModule( obj );
    } else if( LinkFlags & DWARF_DBI_FLAG ) {
        DwarfInitModule( obj );
    } else if( LinkFlags & CV_DBI_FLAG ) {
        CVInitModule( obj );
    }
}

extern void DBIP1Source( byte *buff, byte *endbuff )
/**************************************************/
{
    int         len;
    byte        major;
    byte        minor;

    major = *buff++;
    minor = *buff++;
    len = endbuff - buff;
    if( len <= 0 ) {
        BadObject();
        return;
    }
    ObjFormat |= FMT_DEBUG_COMENT;
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIP1Source( major, minor, buff, len );
    }
}

extern section * DBIGetSect( char *clname )
/*****************************************/
{
    if( stricmp( clname, _MSTypeClass ) == 0
        || stricmp( clname, _MSLocalClass ) == 0 ) {
        return CurrSect;
    } else if(  stricmp( clname, _DwarfClass ) == 0 ) {
        return Root;
    }
    return NULL;
}

extern void DBIColClass( class_entry *class )
/*******************************************/
{
    bool        isdbi;

    isdbi = TRUE;
    if( stricmp( class->name, _DwarfClass ) == 0 ) {
        class->flags |= CLASS_DWARF;
    } else if( stricmp( class->name, _MSTypeClass ) == 0 ) {
        class->flags |= CLASS_MS_TYPE;
    } else if( stricmp( class->name, _MSLocalClass ) == 0 ) {
        class->flags |= CLASS_MS_LOCAL;
    } else {
        isdbi = FALSE;
    }
}

extern unsigned_16 DBIColSeg( class_entry *class )
/************************************************/
{
    switch( class->flags & CLASS_HANDS_OFF ) {
    case CLASS_DWARF:
        if( CurrMod->modinfo & DBI_TYPE ) {
            CurrMod->modinfo |= MOD_DBI_SEEN;
        }
        return DWARF_DEBUG_OTHER;       // assume other until later.
    case CLASS_MS_TYPE:
        return MS_TYPE;
    case CLASS_MS_LOCAL:
        return MS_LOCAL;
    }
    return 0;
}

extern void DBIP1ModuleScanned( void )
/************************************/
// called in pass 1 when finished looking at a module
// if some segdefs have been delayed due to distributing libraries, this
// will be called twice (once when regular pass 1 is done, once when all
// segdefs processed
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIP1ModuleScanned();
    } else if( LinkFlags & DWARF_DBI_FLAG ) {
        DwarfP1ModuleScanned();
    } else if( LinkFlags & CV_DBI_FLAG ) {
        CVP1ModuleScanned();
    }
}

static bool MSSkip( void )
/************************/
{
    bool        iscv;
    bool        seencmt;

    if( !(ObjFormat & FMT_OMF) ) {
        return (LinkFlags & DWARF_DBI_FLAG) != 0;
    } else {
        iscv = (LinkFlags & CV_DBI_FLAG) != 0;
        seencmt = (ObjFormat & FMT_DEBUG_COMENT) != 0;
        return !(iscv ^ seencmt) || LinkFlags & DWARF_DBI_FLAG;
    }
}

extern bool DBISkip( unsigned_16 info )
/*************************************/
// returns TRUE we should skip processing this segment because we are
// ignoring debugging information
{
    switch( info ) {
    case MS_TYPE:
        return !(CurrMod->modinfo & DBI_TYPE) || MSSkip();
    case MS_LOCAL:
        return !(CurrMod->modinfo & DBI_LOCAL) || MSSkip();
    case NOT_DEBUGGING_INFO:
        return FALSE;
    default:
        return !(CurrMod->modinfo & DBI_TYPE) || !(LinkFlags & DWARF_DBI_FLAG);
    }
}

extern bool DBINoReloc( unsigned_16 info )
/****************************************/
// called to see if we should handle a relocation specially.
{
    return info != 0;
}

static void AddNovGlobals( mod_entry *mod )
/*****************************************/
{
#ifdef _NOVELL
    Ring2Walk( mod->publist, NovDBIAddGlobal );
#endif
}

extern void DBIPreAddrCalc( void )
/********************************/
{
    void (*modptr)( mod_entry * );
    void (*segptr)( seg_leader * );

    if( LinkFlags & NOVELL_DBI_FLAG ) {
        WalkMods( AddNovGlobals );
    }
    if( !(LinkFlags & ANY_DBI_FLAG) ) return;
    if( LinkFlags & OLD_DBI_FLAG ) {
        modptr = ODBIP1ModuleFinished;
        segptr = ODBIAddAddrInfo;
    } else if( LinkFlags & DWARF_DBI_FLAG ) {
        modptr = DwarfP1ModuleFinished;
        segptr = DwarfAddAddrInfo;
    } else {
        modptr = CVP1ModuleFinished;
        segptr = CVAddAddrInfo;
    }
    WalkMods( modptr );
    WalkLeaders( segptr );
    if( LinkFlags & DWARF_DBI_FLAG ) {
        WalkMods( DwarfStoreAddrInfo );
    }
}

extern void DBIAddrInfoScan( seg_leader *seg,
                         void (*initfn)(segdata *, void *),
                         void (*addfn)(segdata *, offset, offset, void *, bool),
                         void * cookie )
/******************************************************************************/
{
    segdata *   prev;
    segdata *   curr;
    offset      size;
    bool        isnewmod;

    if( seg->dbgtype != NOT_DEBUGGING_INFO ) return;
    if( seg->class->flags & (CLASS_STACK|CLASS_IDATA)
                && (FmtData.dll || FmtData.type & MK_PE) ) return;
    prev = RingStep( seg->pieces, NULL );
    for(;;) {
        if( prev == NULL ) return;
        if( !prev->isdead ) break;
        prev = RingStep( seg->pieces, prev );
    }
    initfn( prev, cookie );
    size = 0;
    curr = RingStep( seg->pieces, prev );
    while( curr != NULL ) {
        if( !curr->isdead ) {
            size += curr->a.delta - prev->a.delta;
            isnewmod = curr->o.mod != prev->o.mod && size != 0;
            addfn( prev, curr->a.delta, size, cookie, isnewmod );
            if( isnewmod ) {
                size = 0;
            }
            prev = curr;
        }
        curr = RingStep( seg->pieces, curr );
    }
    size += prev->length;
    addfn( prev, 0, size, cookie, size != 0 );
}

extern void DBIComment( void )
/****************************/
{
}

extern void DBIAddModule( mod_entry *obj, section *sect )
/***********************************************************/
// called just before publics have been assigned addresses between p1 & p2
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIAddModule( obj, sect );
    } else if( LinkFlags & DWARF_DBI_FLAG ) {
        DwarfAddModule( obj, sect );
    } else if( LinkFlags & CV_DBI_FLAG ) {
        CVAddModule( obj, sect );
    }
}

extern void DBIGenModule( void )
/******************************/
// called at the end of pass2 for a module
{
    if( MOD_NOT_DEBUGGABLE(CurrMod) ) return;
    if( LinkFlags & ANY_DBI_FLAG ) {
        Ring2Walk( CurrMod->segs, DBIGenLocal );
        DBIGenLines( CurrMod );
        if( LinkFlags & OLD_DBI_FLAG ) {
            ODBIGenModule();
        } else if( LinkFlags & DWARF_DBI_FLAG ) {
            DwarfGenModule();
        } else {
            CVGenModule();
        }
    }
}

extern void DBIDefClass( class_entry *cl, unsigned_32 size )
/**********************************************************/
// called during address calculation
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIDefClass( cl, size );
    } else if( LinkFlags & DWARF_DBI_FLAG ) {
        DwarfDefClass( cl, size );
    } else if( LinkFlags & CV_DBI_FLAG ) {
        CVDefClass( cl, size );
    }
}

extern void DBIAddLocal( unsigned_16 info, offset length )
/********************************************************/
// called during pass 1 final segment processing.
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIAddLocal( info, length );
    } else if( LinkFlags & CV_DBI_FLAG ) {
        CVAddLocal( info, length );
    }
}

static void DBIGenLocal( segdata *sdata )
/***************************************/
// called during pass 2 segment processing
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIGenLocal( sdata );
    }
}

extern void DBIModGlobal( symbol *sym )
/*************************************/
{
    if( !IS_SYM_ALIAS(sym) && !(sym->info & SYM_DEAD) ) {
        if( IS_SYM_IMPORTED(sym) ||
            ( sym->p.seg != NULL
            && sym->p.seg->u.leader->dbgtype == NOT_DEBUGGING_INFO
            && !sym->p.seg->isabs)
          )
        {
            DBIAddGlobal( sym );
        }
    }
}

extern void DBIAddGlobal( symbol *sym )
/*************************************/
// called during pass 1 symbol definition
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIAddGlobal( sym );
    } else if( LinkFlags & DWARF_DBI_FLAG ) {
        DwarfAddGlobal( sym );
    } else if( LinkFlags & CV_DBI_FLAG ) {
        CVAddGlobal( sym );
    }
}

extern void DBIGenGlobal( symbol * sym, section *sect )
/*****************************************************/
// called during symbol address calculation (between pass 1 & pass 2)
// also called by loadpe between passes
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIGenGlobal( sym, sect );
    } else if( LinkFlags & DWARF_DBI_FLAG ) {
        DwarfGenGlobal( sym, sect );
    } else if( LinkFlags & CV_DBI_FLAG ) {
        CVGenGlobal( sym, sect );
    }
#ifdef _NOVELL
    if( (sym->info & SYM_STATIC) == 0 && LinkFlags & NOVELL_DBI_FLAG ) {
        NovDBIGenGlobal( sym );
    }
#endif
}

extern void DBIAddLines( segdata *seg, void *line, unsigned size, bool is32bit )
/******************************************************************************/
// called during pass 1 linnum processing
{
    lineinfo *  info;

    _PermAlloc( info, sizeof(lineinfo) + size - 1 );
    info->seg = seg;
    info->size = size;
    if( is32bit ) info->size |= LINE_IS_32BIT;
    memcpy( info->data, line, size );
    RingAppend( &CurrMod->lines, info );
}

extern unsigned CalcLineQty( unsigned size, bool is32bit )
/********************************************************/
{
    if( is32bit ) {
        size /= sizeof( ln_off_386 );
    } else {
        size /= sizeof( ln_off_286 );
    }
    return size;
}

static bool DoLineWalk( lineinfo *info,
                         void (*cbfn)(segdata *,void*,unsigned,bool) )
/********************************************************************/
{
    unsigned    size;
    bool        is32bit;

    if( !info->seg->isdead ) {
        size = info->size & ~LINE_IS_32BIT;
        is32bit = (info->size & LINE_IS_32BIT) != 0;
        cbfn( info->seg, info->data, size, is32bit );
    }
    return FALSE;
}

extern void DBILineWalk( void *lines,
                         void (*cbfn)(segdata *,void*,unsigned,bool) )
/********************************************************************/
{
    RingLookup( lines, DoLineWalk, cbfn );
}

static void DBIGenLines( mod_entry *mod )
/***************************************/
// called during pass 2 linnum processing
{
    void (*fn)(segdata *,void *, unsigned, bool );

    if( LinkFlags & OLD_DBI_FLAG ) {
        fn = ODBIGenLines;
    } else if( LinkFlags & DWARF_DBI_FLAG ) {
        fn = DwarfGenLines;
    } else if( LinkFlags & CV_DBI_FLAG ) {
        fn = CVGenLines;
    }
    DBILineWalk( mod->lines, fn );
}

extern virt_mem DBIAlloc( unsigned long size )
/********************************************/
// handy virtual memory allocation routine used inside the debug info generators
{
    if( size == 0 ) return( 0 );
    return( AllocStg( size ) );
}

extern void DBIAddrStart( void )
/******************************/
// called after address calculation is done.
{
#ifdef _NOVELL
    if( LinkFlags & NOVELL_DBI_FLAG ) {
        NovDBIAddrStart();
    }
#endif
    if( LinkFlags & CV_DBI_FLAG ) {
        CVAddrStart();
    }
    DBIAddrSectStart( Root );
}

extern void DBIAddrSectStart( section * sect )
/********************************************/
// called for each section after address calculation is done.
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIAddrSectStart( sect );
    } else if( LinkFlags & DWARF_DBI_FLAG ) {
        DwarfAddrSectStart( sect );
    }
}

extern void DBIP2Start( section *sect )
/*************************************/
// called for each section just before pass 2 starts
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIP2Start( sect );
    } else if( LinkFlags & DWARF_DBI_FLAG ) {
        SectWalkClass( sect, DwarfGenAddrInfo );
    } else if( LinkFlags & CV_DBI_FLAG ) {
        SectWalkClass( sect, CVGenAddrInfo );
    }
}

extern void DBIFini( section *sect )
/**********************************/
// called after pass 2 is finished, but before load file generation
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBIFini( sect );
    } else if( LinkFlags & CV_DBI_FLAG ) {
        CVFini( sect );
    }
}

extern void DBISectCleanup( section *sect )
/*****************************************/
// called when burning down the house
{
    if( LinkFlags & OLD_DBI_FLAG ) {
        ODBISectCleanup( sect );
    }
}

extern void DBICleanup( void )
/****************************/
// called when burning down the house
{
    FreeGroups( DBIGroups );
}

extern void WriteDBI( void )
/**************************/
// called during load file generation.  It is assumed that the loadfile is
// positioned to the right spot.
{
    outfilelist symfile;
    outfilelist *save;

    if( !(LinkFlags & ANY_DBI_FLAG) ) return;
    if( SymFileName != NULL ) {
        InitBuffFile( &symfile, SymFileName );
        OpenBuffFile( &symfile );
        save = CurrSect->outfile;
        CurrSect->outfile = &symfile;
    }
    if( LinkFlags & OLD_DBI_FLAG ) {
        OWriteDBI();
    } else if( LinkFlags & DWARF_DBI_FLAG ) {
        DwarfWriteDBI();
    } else if( LinkFlags & CV_DBI_FLAG ) {
        CVWriteDBI();
    }
    if( SymFileName != NULL ) {
        CloseBuffFile( &symfile );
        CurrSect->outfile = save;
    }
}
