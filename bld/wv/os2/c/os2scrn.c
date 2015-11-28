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


#include <stddef.h>
#define INCL_SUB
#include <os2.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgio.h"
#include "stdui.h"
#include "dbgscrn.h"
#include "gui.h"
#include "guigmous.h"
#include "dbginit.h"


unsigned            NumLines;
unsigned            NumColumns;
static VIOMODEINFO  SaveMode;

void Ring_Bell( void )
{
    DosBeep( 1000, 250 );
}


/*
 * ConfigScreen -- figure out screen configuration we're going to use.
 */

unsigned ConfigScreen( void )
{
    return( 0 );
}


/*
 * InitScreen
 */

void InitScreen( void )
{
    VIOMODEINFO     new;

    SaveMode.cb = sizeof( SaveMode );
    VioGetMode( &SaveMode, 0 );
    if( NumLines != 0 || NumColumns != 0 ) {
        new = SaveMode;
        if( NumLines != 0 ) new.row = NumLines;
        if( NumColumns != 0 ) new.col = NumColumns;
        VioSetMode( &new, 0 );
    }
    uistart();
    if( _IsOn( SW_USE_MOUSE ) ) {
        GUIInitMouse( 1 );
    }
}


/*
 * UsrScrnMode -- setup the user screen mode
 */

bool UsrScrnMode( void )
{
    return( false );
}


void DbgScrnMode( void )
{
}


/*
 * DebugScreen -- swap/page to debugger screen
 */

bool DebugScreen( void )
{
    return( false );
}

bool DebugScreenRecover( void )
{
    return( true );
}


/*
 * UserScreen -- swap/page to user screen
 */

bool UserScreen( void )
{
    return( false );
}

void SaveMainWindowPos( void )
{
}

void FiniScreen( void )
{
    if( _IsOn( SW_USE_MOUSE ) ) GUIFiniMouse();
    uistop();
    if( NumLines != 0 || NumColumns != 0 ) VioSetMode( &SaveMode, 0 );
}


/*****************************************************************************\
 *                                                                           *
 *            Replacement routines for User Interface library                *
 *                                                                           *
\*****************************************************************************/

void __FAR *uifaralloc( size_t size )
{
    return( ExtraAlloc( size ) );
}


void uifarfree( void __FAR *ptr )
{
    ExtraFree( ptr );
}
bool SysGUI( void )
{
    return( false );
}
void PopErrBox( const char *buff )
{
    WriteText( STD_ERR, buff, strlen( buff ) );
}

void SetNumLines( int num )
{
    if( num < 10 )
        num = 10;
    if( num > 99 )
        num = 99;
    NumLines = num;
}

void SetNumColumns( int num )
{
    if( num < 25 )
        num = 25;
    if( num > 255 )
        num = 255;
    NumColumns = num;
}

bool ScreenOption( const char *start, unsigned len, int pass )
{
    start=start;len=len;pass=pass;
    return( false );
}

void ScreenOptInit( void )
{
}
