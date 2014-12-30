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


extern void     __FAR *ExtraAlloc( size_t );
extern void     ExtraFree( void __FAR * );
extern int      GUIInitMouse( int );
extern void     GUIFiniMouse( void );

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

unsigned ConfigScreen()
{
    return( 0 );
}


/*
 * InitScreen
 */

void InitScreen()
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
    if( _IsOn( SW_USE_MOUSE ) ) GUIInitMouse( 1 );
}


/*
 * UsrScrnMode -- setup the user screen mode
 */

bool UsrScrnMode()
{
    return( FALSE );
}


void DbgScrnMode( void )
{
}


/*
 * DebugScreen -- swap/page to debugger screen
 */

bool DebugScreen()
{
    return( FALSE );
}

bool DebugScreenRecover()
{
    return( TRUE );
}


/*
 * UserScreen -- swap/page to user screen
 */

bool UserScreen()
{
    return( FALSE );
}

void SaveMainWindowPos()
{
}

void FiniScreen()
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
bool SysGUI()
{
    return( FALSE );
}
void PopErrBox( const char *buff )
{
    WriteText( STD_ERR, buff, strlen( buff ) );
}
