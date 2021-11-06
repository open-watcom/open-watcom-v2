/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Command line parsing for the DOS load file format.
*
****************************************************************************/


#include <string.h>
#include "linkstd.h"
#include "alloc.h"
#include "cmdutils.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "dbgall.h"
#include "cmdall.h"
#include "overlays.h"
#include "objcalc.h"
#include "cmdline.h"
#include "cmddos.h"
#include "distrib.h"


#ifdef _EXE

/*
 * this is an arbitrary non-zero value put in the sect->relocs field to
 * signify that ProcBegin already made a new section, so ProcSection
 * should not.
 */
#define SECT_ALREADY_MADE 1

static byte         OvlLevel;

void SetDosFmt( void )
/********************/
{
    Extension = E_LOAD;
}

void FreeDosFmt( void )
/*********************/
{
}

overlay_ref GetOvlRef( void )
/***************************/
{
    /* OvlSectNum value 0 is reserved for Root */
    return( ( OvlLevel == 0 ) ? 0 : OvlSectNum - 1 );
}

#ifdef _INT_DEBUG
static void PrintAreas( OVL_AREA *ovlarea );

static void PrintSect( section *sect )
/************************************/
{
    file_list   *list;

    OvlLevel++;
    for( ; sect != NULL; sect = sect->next_sect ) {
        DEBUG(( DBG_OLD, "" ));
        DEBUG(( DBG_OLD, "OverLay #%d   Level %d", sect->ovlref, OvlLevel ));
        DEBUG(( DBG_OLD, "Files:" ));
        if( sect->files == NULL ) {
            DEBUG(( DBG_OLD, "\"Non-section\"" ));
        }
        for( list = sect->files; list != NULL; list = list->next_file ) {
            DEBUG(( DBG_OLD, "%s", list->infile->name ));
        }
        PrintAreas( sect->areas );
    }
    OvlLevel--;
}

static void PrintAreas( OVL_AREA *ovlarea )
/*****************************************/
{
    for( ; ovlarea != NULL; ovlarea = ovlarea->next_area ) {
        DEBUG(( DBG_OLD, "" ));
        DEBUG(( DBG_OLD, "" ));
        DEBUG(( DBG_OLD, "Begin OverLay Area" ));
        PrintSect( ovlarea->sections );
        DEBUG(( DBG_OLD, "" ));
        DEBUG(( DBG_OLD, "End OverLay Area" ));
    }
}

static void PrintOvl( void )
/**************************/
{
    OvlLevel = 0;
    PrintAreas( Root->areas );
}
#endif

static void SetOvlClasses( void )
/********************************
 * make sure the overlay loader is always "overlayed".
 */
{
    list_of_names       *ovlmgr;

    if( OvlClasses != NULL ) {
        _PermAlloc( ovlmgr, sizeof( list_of_names ) + OVL_MGR_CL_LEN );
        memcpy( ovlmgr->name, OvlMgrClass, OVL_MGR_CL_LEN+1 );
        ovlmgr->next_name = OvlClasses;
        OvlClasses = ovlmgr;
    }
}

static section *OvlNewSection( void )
/***********************************/
{
    OvlSectNum++;
    return( NewSection() );
}

static void NewArea( section *sect )
/***********************************
 * allocate a new area including this section
 */
{
    ovl_area            *ovl;
    ovl_area            **owner;

    _PermAlloc( ovl, sizeof( ovl_area ) );
    ovl->next_area = NULL;
    ovl->sections = sect;
    sect->parent = CurrSect;
    for( owner = &CurrSect->areas; *owner != NULL; ) {
        owner = &(*owner)->next_area;
    }
    *owner = ovl;
}

static void MakeNonArea( void )
/******************************
 * make a new overlay area for non-overlay classes
 */
{
    NonSect = OvlNewSection();   // No debug info in nonsect.
    NewArea( NonSect );
}

void CmdOvlFini( void )
/*********************/
{
    if( OvlLevel != 0 ) {
        Ignite();
        LnkMsg( LOC+LINE+FTL+MSG_EXPECTING_END, NULL );
    }
    if( FmtData.u.dos.dynamic &&
        ( ( Root->areas == NULL ) || ( Root->areas->next_area != NULL ) ) ) {
        Ignite();
        LnkMsg( LOC+LINE+FTL+MSG_INCORRECT_NUM_AREAS, NULL );
    }
    SetOvlClasses();
    MakeNonArea();
#ifdef _INT_DEBUG
    PrintOvl();
#endif
}

void MakeNewSection( void )
/*************************/
{
    section             *sect;

    if( CurrSect->relocs != SECT_ALREADY_MADE ) {
        sect = OvlNewSection();
        if( LinkFlags & LF_ANY_DBI_FLAG ) {
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


/****************************************************************
 * "OPtion" Directive
 ****************************************************************/

static bool ProcDistribute( void )
/********************************/
{
    FmtData.u.dos.distribute = true;
    return( true );
}

static bool ProcPadSections( void )
/*********************************/
{
    FmtData.u.dos.pad_sections = true;
    return( true );
}

static bool ProcSmall( void )
/***************************/
{
    FmtData.u.dos.ovl_short = true;
    return( true );
}

static bool ProcDynamic( void )
/*****************************/
{
    FmtData.u.dos.dynamic = true;
    return( true );
}

static bool ProcNoIndirect( void )
/********************************/
{
    FmtData.u.dos.noindirect = true;
    return( true );
}

static bool ProcFullHeader( void )
/********************************/
{
    FmtData.u.dos.full_mz_hdr = true;
    return( true );
}

static bool ProcStandard( void )
/******************************/
{
    FmtData.u.dos.dynamic = false;
    return( true );
}

static bool ProcArea( void )
/***************************
 * process the area size directive.
 */
{
    unsigned_32     value;
    bool            ret;

    ret = GetLong( &value );
    if( ret ) {
        OvlAreaSize = (value + FmtData.SegMask) >> FmtData.SegShift;
    }
    return( ret );
}

static parse_entry  MainOptions[] = {
    "SMall",        ProcSmall,          MK_OVERLAYS, 0,
    "DIStribute",   ProcDistribute,     MK_OVERLAYS, 0,
    "DYNamic",      ProcDynamic,        MK_OVERLAYS, 0,
    "STANdard",     ProcStandard,       MK_OVERLAYS, 0,
    "NOIndirect",   ProcNoIndirect,     MK_OVERLAYS, 0,
    "ARea",         ProcArea,           MK_OVERLAYS, 0,
    "PADSections",  ProcPadSections,    MK_OVERLAYS, 0,
    "FULLHeader",   ProcFullHeader,     MK_DOS, 0,
    NULL
};

bool ProcDosOptions( void )
/*************************/
{
    return( ProcOne( MainOptions, SEP_NO ) );
}


/****************************************************************
 * "OVerlay" Directive
 ****************************************************************/

static bool AddClass( void )
/**************************/
{
    list_of_names       *ovlclass;

    _PermAlloc( ovlclass, sizeof( list_of_names ) + Token.len );
    memcpy( ovlclass->name, Token.this, Token.len );
    ovlclass->name[Token.len] = '\0';
    ovlclass->next_name = OvlClasses;
    OvlClasses = ovlclass;
    return( true );
}

bool ProcOverlay( void )
/**********************/
{
    return( ProcArgList( AddClass, TOK_INCLUDE_DOT ) );
}


/****************************************************************
 * "FIXedlib" Directive
 ****************************************************************/

bool ProcFixedLib( void )
/***********************/
{
    bool    ret;

    CmdFlags |= CF_SET_SECTION;
    ret = ProcLibrary();
    CmdFlags &= ~CF_SET_SECTION;
    return( ret );
}


/****************************************************************
 * "Begin" Directive
 ****************************************************************/

static bool ProcInto( void )
/***************************
 * Process the INTO keyword.
 */
{
    if( GetToken( SEP_NO, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        CurrSect->outfile = NewOutFile( FileName( Token.this, Token.len, E_OVL, false ) );
        return( true );
    }
    LnkMsg( LOC+LINE+WRN+MSG_DIRECTIVE_ERR, "s", "into" );
    return( false );
}

static parse_entry  SectOptions[] = {
    "INto",         ProcInto,           MK_OVERLAYS, 0,
    NULL
};

static bool ProcSection( void )
/******************************
 * process SECTION command
 */
{
    if( OvlLevel == 0 ) {
        LnkMsg( LOC+LINE+WRN+MSG_NO_SECTION_IN_ROOT, NULL );
    } else {
        MakeNewSection();
        ProcOne( SectOptions, SEP_NO );      // check for INTO
        while( DoParseDirective() ) {
            RestoreParser();
        }
    }
    return( true );
}

static bool ProcAutoSection( void )
/*********************************/
{
    if( OvlLevel == 0 ) {
        LnkMsg( LOC+LINE+WRN+MSG_NO_SECTION_IN_ROOT, NULL );
    } else {
        MakeNewSection();
        ProcOne( SectOptions, SEP_NO );      // check for INTO
        CmdFlags |= CF_AUTOSECTION | CF_SECTION_THERE;
        while( DoParseDirective() ) {
        }
        CmdFlags &= ~CF_AUTOSECTION;
    }
    return( true );
}

static bool ProcEnd( void )
/**************************
 * process the end of an overlay area
 */
{
    if( OvlLevel > 0 ) {  // OvlLevel should always be > 0, but just in case..
        OvlLevel--;
    }
    if( CurrSect->relocs != 0 ) {   // this only happens if the user specifies
        CurrSect->relocs = 0;       // an overlay area with no files in it.
    }
    return( false );    /*  cause loop to be exited in ProcBegin */
}

static parse_entry  Sections[] = {
    "Section",      ProcSection,        MK_OVERLAYS, 0,
    "AUTOSection",  ProcAutoSection,    MK_OVERLAYS, 0,
    "End",          ProcEnd,            MK_OVERLAYS, 0,
    NULL
};

bool ProcBegin( void )
/*********************
 * process a new overlay area
 */
{
    section         *oldsect;
    file_list       **oldflist;
    section         *sect;

    LinkState |= LS_FMT_SPECIFIED;      // she must want DOS mode.
    if( ( OvlLevel > 0 ) && FmtData.u.dos.dynamic ) {
        oldsect = NULL;
        oldflist = NULL;
        CmdFlags &= ~CF_AUTOSECTION;    // merge old area with this.
    } else {
        oldsect = CurrSect;
        oldflist = CurrFList;
        sect = OvlNewSection();
        if( LinkFlags & LF_ANY_DBI_FLAG ) {
            DBISectInit( sect );
        }
        NewArea( sect );
        sect->relocs = SECT_ALREADY_MADE;
        CurrSect = sect;
        CurrFList = &sect->files;
    }
    OvlLevel++;
    while( ProcOne( Sections, SEP_NO ) ) {
        // NULL LOOP
    }
    if( ( OvlLevel == 0 ) || !FmtData.u.dos.dynamic ) {
        CurrFList = oldflist;
        CurrSect = oldsect;
    }
    return( true );
}


/****************************************************************
 * "NOVector" Directive
 ****************************************************************/

static bool AddNoVector( void )
/*****************************/
{
    symbol      *sym;

    sym = SymOp( ST_CREATE_REFERENCE, Token.this, Token.len );
    sym->u.d.ovlstate |= ( OVL_FORCE | OVL_NO_VECTOR );
    return( true );
}

bool ProcNoVector( void )
/***********************/
{
    return( ProcArgList( AddNoVector, TOK_INCLUDE_DOT ) );
}


/****************************************************************
 * "VECtor" Directive
 ****************************************************************/

static bool AddVector( void )
/***************************/
{
    OvlVectorize( SymOp( ST_CREATE_REFERENCE, Token.this, Token.len ) );
    return( true );
}

bool ProcVector( void )
/*********************/
{
    return( ProcArgList( AddVector, TOK_INCLUDE_DOT ) );
}


/****************************************************************
 * "FORCEVector" Directive
 ****************************************************************/

static bool AddForceVector( void )
/********************************/
{
    symbol  *sym;

    sym = SymOp( ST_CREATE_REFERENCE, Token.this, Token.len );
    OvlVectorize( sym );
    sym->u.d.ovlstate |= OVL_ALWAYS;
    return(true);
}

bool ProcForceVector( void )
/**************************/
{
    return( ProcArgList( AddForceVector, TOK_INCLUDE_DOT ) );
}


/****************************************************************
 * "FORMat" Directive
 ****************************************************************/

static bool ProcCom( void )
/*************************/
{
    Extension = E_COM;
    return( true );
}

static parse_entry  DosFormats[] = {
    "COM",          ProcCom,            MK_COM, 0,
    NULL
};

bool ProcDosFormat( void )
/************************/
{
    OvlLevel = 0;
    ProcOne( DosFormats, SEP_NO );
    return( true );
}

#endif
