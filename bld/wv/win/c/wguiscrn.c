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
#include "guiwin.h"
#include "dbgcmdln.h"
#include "wndsys.h"
#include "trpsys.h"
#include "dbglkup.h"
#include "dbginit.h"
#include "wininit.h"


extern a_window         *WndMain;
extern volatile bool    BrkPending;

unsigned        ScrnLines = 50;
unsigned        FlipMech;
unsigned        ScrnMode;
HWND            DebuggerHwnd;
bool            WantFast;
bool            TrapForceHardMode = false;
static HWND     FocusWnd;

#if 0
ToggleHardMode( void )
{
    TrapForceHardMode = !TrapForceHardMode;
#ifdef __GUI__
    TrapSetHardMode( TrapForceHardMode );
    GUICheckMenuItem( WndGui( WndMain ), MENU_MAIN_WINDOW_HARD_MODE, TrapForceHardMode );
#endif
    GUISetModalDlgs( TrapForceHardMode || TrapHardModeRequired );
}
#endif

void InitHookFunc( void )
{
}

void FiniHookFunc( void )
{
    TrapUnLockInput();
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

size_t GetSystemDir( char *buff, size_t buff_len )
{
    buff[ 0 ] = '\0';
    GetWindowsDirectory( buff, buff_len );
    return( strlen( buff ) );
}

void InitScreen( void )
{
    FocusWnd = GetFocus();
    RestoreMainScreen( "WDWIN" );
    GUISetModalDlgs( false );
}

void FiniScreen( void )
{
    if( IsWindow( FocusWnd ) ) {
        SetFocus( FocusWnd );
    }
}

bool UsrScrnMode( void )
{
    return( false );
}

void DbgScrnMode( void )
{
}

enum {
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
    if( ScreenState == DEBUG_SCREEN )
        return( false );
    if( WndMain ) {
        ScreenState = DEBUG_SCREEN;
        GUISetModalDlgs( TrapForceHardMode || TrapHardModeRequired );
        SetFocus( GUIGetSysHandle( WndGui( WndMain ) ) );
    }
    return( false );
}

bool DebugScreenRecover( void )
{
    return( true );
}

bool UserScreen( void )
{
    if( ScreenState == USER_SCREEN )
        return( false );
    if( WndMain ) {
        ScreenState = USER_SCREEN;
        TrapUnLockInput();
    }
    return( false );
}

void SaveMainWindowPos( void )
{
    SaveMainScreen( "WDWIN" );
}

bool SysGUI( void )
{
    return( true );
}

char *GUIGetWindowClassName( void )
{
    return( "WatcomDebugger" );
}

static const char ScreenOptNameTab[] = {
    "Monochrome\0"
    "Color\0"
    "Colour\0"
    "Ega43\0"
    "FAstswap\0"
    "Vga50\0"
    "Overwrite\0"
    "Page\0"
    "Swap\0"
    "Two\0"
};

enum {
    OPT_MONO,
    OPT_COLOR,
    OPT_COLOUR,
    OPT_EGA43,
    OPT_FASTSWAP,
    OPT_VGA50,
    OPT_OVERWRITE,
    OPT_PAGE,
    OPT_SWAP,
    OPT_TWO
};

int SwapScrnLines( void )
{
    return( ScrnLines );
}

static void GetLines( void )
{
    if( HasEquals() ) {
        ScrnLines = GetValue();
    }
}


static void SetEGA43( void )
{
    FlipMech = FLIP_SWAP;
    ScrnMode = MD_EGA;
    ScrnLines = 43;
}

static void SetVGA50( void )
{
    FlipMech = FLIP_SWAP;
    ScrnMode = MD_EGA;
    ScrnLines = 50;
}

void SetNumLines( int num )
{
    if( num >= 43 ) {
        if( num >= 50 ) {
            SetVGA50();
        } else {
            SetEGA43();
        }
    }
}

void SetNumColumns( int num )
{
    num=num;
}

bool ScreenOption( const char *start, unsigned len, int pass )
{
    pass=pass;
    switch( Lookup( ScreenOptNameTab, start, len ) ) {
    case OPT_COLOR:
    case OPT_COLOUR:
        GetLines();
        break;
    case OPT_MONO:
        GetLines();
        break;
    case OPT_FASTSWAP:
        WantFast = true;
        break;
    case OPT_EGA43:
        SetEGA43();
        break;
    case OPT_VGA50:
        SetVGA50();
        break;
    case OPT_OVERWRITE:
    case OPT_PAGE:
    case OPT_SWAP:
        FlipMech = FLIP_SWAP;
        break;
    case OPT_TWO:
        FlipMech = FLIP_TWO;
        break;
    default:
        return( false );
    }
    return( true );
}


void ScreenOptInit( void )
{
    ScrnMode = MD_DEFAULT;
    FlipMech = FLIP_TWO;
}
