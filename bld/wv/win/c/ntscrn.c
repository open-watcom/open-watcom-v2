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
#include <windows.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbglit.h"
#include "stdui.h"
#include "dbgscrn.h"
#include "gui.h"
#include "guigmous.h"

extern void     *ExtraAlloc( size_t );
extern void     ExtraFree( void * );

static unsigned ScrnLines=50;
static unsigned ScrnColumns=80;
HWND            DebuggerHwnd;
HWND            HwndFore = NULL;


void Ring_Bell( void )
{
    Beep( 1000, 250 );
}

unsigned ConfigScreen( void )
{
    return( 0 );
}

static char OldTitle[256];

void InitScreen( void )
{
    DebuggerHwnd = GetForegroundWindow();
    GetConsoleTitle( OldTitle, sizeof( OldTitle ) );
    SetConsoleTitle( LIT_DUI( The_WATCOM_Debugger ) );
    if( uistart() ) {
        if( _IsOn( SW_USE_MOUSE ) ) GUIInitMouse( 1 );
    }
}

bool UsrScrnMode( void )
{
    return( FALSE );
}

bool DebugScreen( void )
{
    HWND        fore;

    fore = GetForegroundWindow();
    if( fore != DebuggerHwnd ) {
        HwndFore = fore;
    }
    if( !IsIconic( DebuggerHwnd ) ) SetForegroundWindow( DebuggerHwnd );
    return( FALSE );
}

bool DebugScreenRecover( void )
{
    DebugScreen();
    return( TRUE );
}

bool UserScreen( void )
{
    if( IsWindow( HwndFore ) ) {
        if( !IsIconic( DebuggerHwnd ) ) SetForegroundWindow( HwndFore );
    } else {
        HwndFore = NULL;
    }
    return( FALSE );
}

void SaveMainWindowPos( void )
{
}

void FiniScreen( void )
{
    SetConsoleTitle( OldTitle );
    DebugScreen();
    if( _IsOn( SW_USE_MOUSE ) ) GUIFiniMouse();
    uistop();
}

void *uifaralloc( size_t size )
{
    return( ExtraAlloc( size ) );
}

void uifarfree( void *ptr )
{
    ExtraFree( ptr );
}

bool SysGUI( void )
{
    return( FALSE );
}

int mygetlasterr( void )
{
    return( GetLastError() );
}

void SetNumLines( int num )
{
    ScrnLines = num;
}

void SetNumColumns( int num )
{
    ScrnColumns=num;
}

bool ScreenOption( const char *start, unsigned len, int pass )
{
    start=start;len=len;pass=pass;
    return( false );
}

void ScreenOptInit( void )
{
}
