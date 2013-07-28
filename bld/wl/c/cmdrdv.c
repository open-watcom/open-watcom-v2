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
 *  CMDRDV : command line parsing for the RDOS device load file format.
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
#include "cmdrdv.h"

void SetRdosFmt( void )
/*********************/
// set up the structures needed to be able to process something in RDOS mode.
{
    if( LinkState & FMT_INITIALIZED ) return;
    LinkState |= FMT_INITIALIZED;
    FmtData.u.rdos.code_seg = 0;
    FmtData.u.rdos.data_seg = 0;
    FmtData.u.rdos.code_sel = 0;
    FmtData.u.rdos.data_sel = 0;
    FmtData.u.rdos.bitness = 16;
    FmtData.u.rdos.mboot = 0;
}

void FreeRdosFmt( void )
/**********************/
{
}

extern bool ProcRdos( void )
/*************************/
{
    LinkState |= MAKE_RELOCS | FMT_DECIDED;     // make relocations;
    ProcOne( RdosOptions, SEP_NO, FALSE );
    return( TRUE );
}

extern bool ProcRdosDev16( void )
/*************************/
{
    Extension = E_RDV;
    FmtData.u.rdos.bitness = 16;
    FmtData.u.rdos.mboot = 0;
    if( FmtData.osname == NULL )
        FmtData.osname = "16-bit RDOS Device driver";
    return( TRUE );
}

extern bool ProcRdosDev32( void )
/*************************/
{
    Extension = E_RDV;
    FmtData.u.rdos.bitness = 32;
    FmtData.u.rdos.mboot = 0;
    if( FmtData.osname == NULL )
        FmtData.osname = "32-bit RDOS Device driver";
    return( TRUE );
}

extern bool ProcRdosBin16( void )
/*************************/
{
    Extension = E_BIN;
    FmtData.u.rdos.bitness = 16;
    FmtData.u.rdos.mboot = 0;
    if( FmtData.osname == NULL )
        FmtData.osname = "16-bit RDOS Binary";
    return( TRUE );
}

extern bool ProcRdosBin32( void )
/*************************/
{
    Extension = E_BIN;
    FmtData.u.rdos.bitness = 32;
    FmtData.u.rdos.mboot = 0;
    if( FmtData.osname == NULL )
        FmtData.osname = "32-bit RDOS Binary";
    return( TRUE );
}

extern bool ProcRdosMboot( void )
/*************************/
{
    Extension = E_BIN;
    FmtData.u.rdos.bitness = 16;
    FmtData.u.rdos.mboot = 1;
    if( FmtData.osname == NULL )
        FmtData.osname = "RDOS Multiboot Stub";
    return( TRUE );
}

bool ProcRdosCodeSel( void )
/***************************/
/* process CODESelector option */
{
    return( GetLong( &FmtData.u.rdos.code_sel ) );
}

bool ProcRdosDataSel( void )
/***************************/
/* process DataSelector option */
{
    return( GetLong( &FmtData.u.rdos.data_sel ) );
}
