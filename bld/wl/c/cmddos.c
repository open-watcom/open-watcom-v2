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
 *  CMDDOS : command line parsing for the DOS load file format.
 *
*/

#include <string.h>
#include "linkstd.h"
#include "alloc.h"
#include "command.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "dbgall.h"
#include "cmdall.h"
#include "overlays.h"
#include "objcalc.h"
#include "cmdline.h"
#include "cmddos.h"

byte            OvlLevel;

#ifdef _INT_DEBUG
static  void            PrintOvl( void );
#endif
static bool             AddClass( void );

extern void SetDosFmt( void )
/***************************/
{
    Extension = E_LOAD;
}

extern bool ProcDos( void )
/*************************/
{
    OvlLevel = 0;
    ProcOne( DosOptions, SEP_NO, FALSE );
    return( TRUE );
}

static void SetOvlClasses( void )
/*******************************/
// make sure the overlay loader is always "overlayed".
{
    list_of_names *     ovlmgr;

    if( OvlClasses != NULL ) {
        _PermAlloc( ovlmgr, sizeof( list_of_names ) + OVL_MGR_CL_LEN );
        memcpy( ovlmgr->name, OvlMgrClass, OVL_MGR_CL_LEN+1 );
        ovlmgr->next_name = OvlClasses;
        OvlClasses = ovlmgr;
    }
}

static bool AddClass( void )
/**************************/
{
    list_of_names       *ovlclass;

    _PermAlloc( ovlclass, sizeof( list_of_names ) + Token.len );
    memcpy( ovlclass->name, Token.this, Token.len );
    ovlclass->name[ Token.len ] = '\0';
    ovlclass->next_name = OvlClasses;
    OvlClasses = ovlclass;
    return( TRUE );
}

extern bool ProcOverlay( void )
/*****************************/
{
    return( ProcArgList( &AddClass, TOK_INCLUDE_DOT ) );
}

extern bool ProcDistribute( void )
/********************************/
{
    FmtData.u.dos.distribute = TRUE;
    return( TRUE );
}

extern bool ProcPadSections( void )
/*********************************/
{
    FmtData.u.dos.pad_sections = TRUE;
    return( TRUE );
}

extern bool ProcFixedLib( void )
/******************************/
{
    bool    ret;

    CmdFlags |= CF_SET_SECTION;
    ret = ProcLibrary();
    CmdFlags &= ~CF_SET_SECTION;
    return( ret );
}

// this is an arbitrary non-zero value put in the sect->relocs field to
// signify that ProcBegin already made a new section, so ProcSection
// should not.

#define SECT_ALREADY_MADE 1

extern bool ProcBegin( void )
/***************************/
/* process a new overlay area */
{
    section *       oldsect;
    file_list **    oldflist;
    section *       sect;

    LnkMsg( FTL+MSG_OVERLAYS_NOT_SUPPORTED, NULL );
    LinkState |= FMT_SPECIFIED;      // she must want DOS mode.
    if( OvlLevel > 0 && FmtData.u.dos.dynamic ) {
        CmdFlags &= ~CF_AUTOSECTION;        // merge old area with this.
    } else {
        oldsect = CurrSect;
        oldflist = CurrFList;
        sect = NewSection();
        if( LinkFlags & ANY_DBI_FLAG ) {
            DBISectInit( sect );
        }
        NewArea( sect );
        sect->relocs = SECT_ALREADY_MADE;
        CurrSect = sect;
        CurrFList = &sect->files;
    }
    OvlLevel++;
    while( ProcOne( Sections, SEP_NO, FALSE ) != FALSE ) {}  // NULL LOOP
    if( OvlLevel == 0 || !FmtData.u.dos.dynamic ) {
        CurrFList = oldflist;
        CurrSect = oldsect;
    }
    return( TRUE );
}

extern bool ProcInto( void )
/**************************/
// Process the into keyword.
{
    bool ret;

    ret = GetToken( SEP_NO, TOK_INCLUDE_DOT | TOK_IS_FILENAME );
    if( ret ) {
        CurrSect->outfile = NewOutFile( FileName( Token.this,Token.len,
                                                          E_OVL, FALSE ) );
    } else {
        LnkMsg( LOC+LINE+WRN+MSG_DIRECTIVE_ERR, "s", "into" );
    }
    return( ret );
}

static void NewArea( section *sect )
/**********************************/
/* allocate a new area including this section */
{
    ovl_area            *ovl;
    ovl_area            **owner;

    _PermAlloc( ovl, sizeof( ovl_area ) );
    ovl->next_area = NULL;
    ovl->sections = sect;
    sect->parent = CurrSect;
    owner = &CurrSect->areas;
    while( *owner != NULL ) {
        owner = &(*owner)->next_area;
    }
    *owner = ovl;
}


static void MakeNonArea( void )
/*****************************/
/* make a new overlay area for non-overlay classes */
{
    NonSect = NewSection();   // No debug info in nonsect.
    NewArea( NonSect );
}


extern bool ProcEnd( void )
/*************************/
/* process the end of an overlay area */
{
    if( OvlLevel > 0 ) {  // OvlLevel should always be > 0, but just in case..
        OvlLevel--;
    }
    if( CurrSect->relocs != 0 ) {   // this only happens if the user specifies
        CurrSect->relocs = 0;       // an overlay area with no files in it.
    }
    return( FALSE );    /*  cause loop to be exited in ProcBegin */
}

extern void MakeNewSection( void )
/********************************/
{
    section             *sect;

    if( CurrSect->relocs != SECT_ALREADY_MADE ) {
        sect = NewSection();
        if( LinkFlags & ANY_DBI_FLAG ) {
            DBISectInit( sect );
        }
        if( CmdFlags & CF_AUTOSECTION ) {
            sect->outfile = CurrSect->outfile;
        }
        sect->parent = CurrSect->parent;
        CurrSect->next_sect = sect;
        CurrSect = sect;
        CurrFList = &sect->files;
    } else {
        CurrSect->relocs = 0;
    }
}

extern bool ProcSection( void )
/*****************************/
/* process SECTION command */
{
    if( OvlLevel == 0 ) {
        LnkMsg( LOC+LINE+WRN+MSG_NO_SECTION_IN_ROOT, NULL );
    } else {
        MakeNewSection();
        ProcOne( SectOptions, SEP_NO, FALSE );      // check for INTO
        while( ProcOne( Directives, SEP_NO, FALSE ) != FALSE ) {
            RestoreParser();
        }
    }
    return( TRUE );
}

extern bool ProcSmall( void )
/***************************/
{
    FmtData.u.dos.ovl_short = TRUE;
    return( TRUE );
}

extern bool ProcCom( void )
/*************************/
{
    Extension = E_COM;
    return( TRUE );
}

extern bool ProcDynamic( void )
/*****************************/
{
    FmtData.u.dos.dynamic = TRUE;
    return( TRUE );
}

static bool AddNoVector( void )
/*****************************/
{
    symbol *    sym;

    sym = SymXOp( ST_CREATE | ST_REFERENCE, Token.this, Token.len );
    sym->u.d.ovlstate |= (OVL_FORCE | OVL_NO_VECTOR);
    return TRUE ;
}

extern bool ProcNoVector( void )
/******************************/
{
    return( ProcArgList( AddNoVector, TOK_INCLUDE_DOT ) );
}

static bool AddVector( void )
/***************************/
{
    Vectorize( SymXOp( ST_CREATE | ST_REFERENCE, Token.this, Token.len ) );
    return(TRUE);
}

extern bool ProcVector( void )
/****************************/
{
    return( ProcArgList( AddVector, TOK_INCLUDE_DOT ) );
}

static bool AddForceVector( void )
/********************************/
{
    symbol  *sym;

    sym = SymXOp( ST_CREATE | ST_REFERENCE, Token.this, Token.len );
    Vectorize( sym );
    sym->u.d.ovlstate |= OVL_ALWAYS;
    return(TRUE);
}

extern bool ProcForceVector( void )
/*********************************/
{
    return( ProcArgList( AddForceVector, TOK_INCLUDE_DOT ) );
}

extern bool ProcAutoSection( void )
/*********************************/
{
    if( OvlLevel == 0 ) {
        LnkMsg( LOC+LINE+WRN+MSG_NO_SECTION_IN_ROOT, NULL );
    } else {
        MakeNewSection();
        ProcOne( SectOptions, SEP_NO, FALSE );      // check for INTO
        CmdFlags |= CF_AUTOSECTION | CF_SECTION_THERE;
        while( ProcOne( Directives, SEP_NO, FALSE ) != FALSE ) {}
        CmdFlags &= ~CF_AUTOSECTION;
    }
    return( TRUE );
}

extern bool ProcNoIndirect( void )
/********************************/
{
    FmtData.u.dos.noindirect = TRUE;
    return( TRUE );
}

extern bool ProcStandard( void )
/******************************/
{
    FmtData.u.dos.dynamic = FALSE;
    return( TRUE );
}

extern bool ProcArea( void )
/**************************/
// process the area size directive.
{
    unsigned long   value;
    bool            ret;

    ret = GetLong( &value );
    if( ret ) {
        AreaSize = (value + 15) >> 4;
    }
    return( ret );
}

extern void CmdOvlFini( void )
/****************************/
{
    if( OvlLevel != 0 ) {
        Ignite();
        LnkMsg( LOC+LINE+FTL+MSG_EXPECTING_END, NULL );
    }
    if( FmtData.u.dos.dynamic &&
        (Root->areas == NULL || Root->areas->next_area != NULL) ) {
        Ignite();
        LnkMsg( LOC+LINE+FTL+MSG_INCORRECT_NUM_AREAS, NULL );
    }
    SetOvlClasses();
    MakeNonArea();
#ifdef _INT_DEBUG
    PrintOvl();
#endif
}

#ifdef _INT_DEBUG
static void PrintOvl( void )
/**************************/
{
    OvlLevel = 0;
    PrintAreas( Root->areas );
}

static void PrintAreas( OVL_AREA *ovlarea )
{
    while( ovlarea != NULL ) {
        DEBUG(( DBG_OLD, "" ));
        DEBUG(( DBG_OLD, "" ));
        DEBUG(( DBG_OLD, "Begin OverLay Area" ));
        PrintSect( ovlarea->sections );
        DEBUG(( DBG_OLD, "" ));
        DEBUG(( DBG_OLD, "End OverLay Area" ));
        ovlarea = ovlarea->next_area;
    }
}


static void PrintSect( section *sect )
/************************************/
{
    file_list * list;

    OvlLevel++;
    while( sect != NULL ) {
        DEBUG(( DBG_OLD, "" ));
        DEBUG(( DBG_OLD, "OverLay #%d   Level %d", sect->ovl_num, OvlLevel ));
        DEBUG(( DBG_OLD, "Files:" ));
        list = sect->files;
        while( list != NULL ) {
            DEBUG(( DBG_OLD, "%s", list->file->name ));
            list = list->next_file;
        }
        PrintAreas( sect->areas );
        sect = sect->next_sect;
    }
    OvlLevel--;
}
#endif
