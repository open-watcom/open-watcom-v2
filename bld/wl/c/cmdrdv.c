/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
 *  CMDRDV : command line parsing for the RDOS device load file format.
 *
*/

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
#include "cmdrdv.h"


#ifdef _RDOS

void SetRdosFmt( void )
/**********************
 * set up the structures needed to be able to process something in RDOS mode.
 */
{
    if( LinkState & LS_FMT_INITIALIZED )
        return;
    LinkState |= LS_FMT_INITIALIZED;
    FmtData.u.rdos.code_seg = 0;
    FmtData.u.rdos.data_seg = 0;
    FmtData.u.rdos.code_sel = 0;
    FmtData.u.rdos.data_sel = 0;
    FmtData.u.rdos.driver = 0;
    FmtData.u.rdos.mboot = 0;
}

void FreeRdosFmt( void )
/**********************/
{
}


/****************************************************************
 * "OPtion" Directive
 ****************************************************************/

static bool ProcRdosCodeSel( void )
/**********************************
 * process CODESelector option
 */
{
    return( GetLong( &FmtData.u.rdos.code_sel ) );
}

static bool ProcRdosDataSel( void )
/**********************************
 * process DataSelector option
 */
{
    return( GetLong( &FmtData.u.rdos.data_sel ) );
}

static parse_entry  MainOptions[] = {
    "CODESelector", ProcRdosCodeSel,    MK_RDOS, 0,
    "DATASelector", ProcRdosDataSel,    MK_RDOS, 0,
    NULL
};

bool ProcRdosOptions( void )
/**************************/
{
    return( ProcOne( MainOptions, SEP_NO ) );
}


/****************************************************************
 * "Format" Directive
 ****************************************************************/

static bool ProcRdos16( void )
/*******************************/
{
    return( true );
}

static parse_entry  RdosSubFormats[] = {
    "16",           ProcRdos16,      MK_RDOS_16, 0,
    NULL
};

static bool ProcRdosDev( void )
/*****************************/
{
    if( !ProcOne( RdosSubFormats, SEP_NO ) ) {
        HintFormat( MK_RDOS_32 );       // set to 32-bit RDOS mode
    }
    Extension = E_RDV;
    FmtData.u.rdos.driver = 1;
    FmtData.u.rdos.mboot = 0;
    return( true );
}

static bool ProcRdosBin( void )
/*******************************/
{
    if( !ProcOne( RdosSubFormats, SEP_NO ) ) {
        HintFormat( MK_RDOS_32 );       // set to 32-bit RDOS mode
    }
    Extension = E_BIN;
    FmtData.u.rdos.driver = 0;
    FmtData.u.rdos.mboot = 0;
    return( true );
}

static bool ProcRdosMboot( void )
/*******************************/
{
    Extension = E_BIN;
    FmtData.u.rdos.driver = 0;
    FmtData.u.rdos.mboot = 1;
    return( true );
}

static parse_entry  RdosFormats[] = {
    "DEV",          ProcRdosDev,        MK_RDOS, 0,
    "BIN",          ProcRdosBin,        MK_RDOS, 0,
    "MBOOT",        ProcRdosMboot,      MK_RDOS_16, 0,
    NULL
};

bool ProcRdosFormat( void )
/*************************/
{
    LinkState |= LS_MAKE_RELOCS | LS_FMT_DECIDED;   // make relocations;
    ProcOne( RdosFormats, SEP_NO );
    return( true );
}

#endif
