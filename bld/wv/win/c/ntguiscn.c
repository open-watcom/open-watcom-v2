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


#include <stdlib.h>
#include <stddef.h>
#include <windows.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgscrn.h"
#include "guiwin.h"
#include "dbgmain.h"

extern void     *ExtraAlloc( size_t );
extern void     ExtraFree( void * );
extern void     SaveMainScreen(char*);
extern void     RestoreMainScreen(char*);
extern void     TellHWND( HWND );

extern a_window *WndMain;

volatile bool   BrkPending;

static HWND     HwndFore = NULL;
static HWND     FirstForeWnd = NULL;


void TellWinHandle( void )
{
    if( _IsOn( SW_POWERBUILDER ) ) return;
    TellHWND( GUIGetSysHandle( WndGui( WndMain ) ) );
}

void Ring_Bell( void )
{
    MessageBeep( 0 );
}

unsigned ConfigScreen( void )
{
    return( 0 );
}

unsigned GetSystemDir( char *buff, unsigned buff_len )
{
    buff[ 0 ] = '\0';
    GetWindowsDirectory( buff, buff_len );
    return( strlen( buff ) );
}

void InitScreen( void )
{
    RestoreMainScreen( "WDNT" );
    FirstForeWnd = GetForegroundWindow();
}

bool UsrScrnMode( void )
{
    return( FALSE );
}

void DbgScrnMode( void )
{
}

static enum {
    DEBUG_SCREEN,
    USER_SCREEN,
    UNKNOWN_SCREEN
} ScreenState = UNKNOWN_SCREEN;

void UnknownScreen( void )
{
    ScreenState = UNKNOWN_SCREEN;
}

bool DebugScreen( void )
{
    HWND        hwnd;
    HWND        fore;

    if( ScreenState == DEBUG_SCREEN ) return( FALSE );
    if( _IsOn( SW_POWERBUILDER ) ) return( FALSE );
    if( WndMain ) {
        ScreenState = DEBUG_SCREEN;
        hwnd = GUIGetSysHandle( WndGui( WndMain ) );
        fore = GetForegroundWindow();
        if( fore != hwnd ) {
            HwndFore = fore;
        }
        if( GUIIsMinimized( WndGui( WndMain ) ) ) GUIRestoreWindow( WndGui( WndMain ) );
        if( IsWindow( hwnd ) ) SetForegroundWindow( hwnd );
        if( _IsOn( SW_POWERBUILDER ) ) {
            WndRestoreWindow( WndMain );
        }
    }
    return( FALSE );
}

bool DebugScreenRecover( void )
{
    return( TRUE );
}


bool UserScreen( void )
{
    if( ScreenState == USER_SCREEN ) return( FALSE );
    if( _IsOn( SW_POWERBUILDER ) ) return( FALSE );
    if( WndMain ) {
        ScreenState = USER_SCREEN;
        if( _IsOn( SW_POWERBUILDER ) ) {
            WndMinimizeWindow( WndMain );
        } else {
            if( IsWindow( HwndFore ) ) {
                SetForegroundWindow( HwndFore );
            } else {
                HwndFore = NULL;
            }
        }
    }
    return( FALSE );
}

void SaveMainWindowPos( void )
{
    SaveMainScreen( "WDNT" );
}

void FiniScreen( void )
{
    if( _IsOn( SW_POWERBUILDER ) ) return;
    if( IsWindow( FirstForeWnd ) ) {
        SetForegroundWindow( FirstForeWnd );
    }
}

bool SysGUI( void )
{
    return( TRUE );
}

void SetNumLines( int num )
{
    num = num;
}

void SetNumColumns( int num )
{
    num = num;
}

bool ScreenOption( const char *start, unsigned len, int pass )
{
    start=start;len=len;pass=pass;
    return( false );
}

void ScreenOptInit( void )
{
}
