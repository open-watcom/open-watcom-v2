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
#include "dbgwind.h"
#include "dosscrn.h"
#include "dbgscrn.h"

extern void     *ExtraAlloc( size_t );
extern void     ExtraFree( void * );
extern HWND     GUIGetSysHandle( gui_window * );
extern void     GUISetModalDlgs( bool modal );
extern void     SaveMainScreen( char * );
extern void     RestoreMainScreen( char * );

extern int              HardModeRequired;
extern a_window         *WndMain;
extern volatile bool    BrkPending;
#ifdef __GUI__
extern void  (__pascal *SetHardMode)( char );
extern void  (__pascal *UnLockInput)( void );
#endif

unsigned        ScrnLines = 50;
unsigned        FlipMech;
unsigned        ScrnMode;
HWND            DebuggerHwnd;
bool            WantFast;
int             ForceHardMode;
static HWND     FocusWnd;

#if 0
ToggleHardMode( void )
{
    ForceHardMode = !ForceHardMode;
    #ifdef __GUI__
        SetHardMode( ForceHardMode );
        GUICheckMenuItem( WndGui( WndMain ),
                      MENU_MAIN_WINDOW_HARD_MODE, ForceHardMode );
    #endif
    GUISetModalDlgs( ForceHardMode || HardModeRequired );
}
#endif

void InitHookFunc( void )
{
}

void FiniHookFunc( void )
{
    UnLockInput();
}

void ForceLines( unsigned lines )
{
    ScrnLines = lines;
}

void Ring_Bell( void )
{
    MessageBeep( 0 );
}

unsigned ConfigScreen( void )
{
    FlipMech = FLIP_SWAP;
    return( 0 );
}

unsigned GetSystemDir( char *buff, unsigned max )
{
    buff[ 0 ] = '\0';
    GetWindowsDirectory( buff, max );
    return( strlen( buff ) );
}

void InitScreen( void )
{
    FocusWnd = GetFocus();
    RestoreMainScreen( "WDWIN" );
    GUISetModalDlgs( FALSE );
}

void FiniScreen( void )
{
    if( IsWindow( FocusWnd ) ) {
        SetFocus( FocusWnd );
    }
}

bool UsrScrnMode( void )
{
    return( FALSE );
}

void DbgScrnMode( void )
{
}

enum {
    DEBUG_SCREEN,
    USER_SCREEN,
    UNKNOWN_SCREEN
} ScreenState = UNKNOWN_SCREEN;

void UnknownScreen()
{
    ScreenState = UNKNOWN_SCREEN;
}

bool DebugScreen( void )
{
    if( ScreenState == DEBUG_SCREEN ) return( FALSE );
    if( WndMain ) {
        ScreenState = DEBUG_SCREEN;
        GUISetModalDlgs( ForceHardMode || HardModeRequired );
        SetFocus( GUIGetSysHandle( WndGui( WndMain ) ) );
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
    if( WndMain ) {
        ScreenState = USER_SCREEN;
        UnLockInput();
    }
    return( FALSE );
}

void SaveMainWindowPos( void )
{
    SaveMainScreen( "WDWIN" );
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
    return( TRUE );
}

char *GUIGetWindowClassName( void )
{
    return( "WatcomDebugger" );
}
