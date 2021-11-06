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


#include <stddef.h>
#include <windows.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "winscrn.h"
#include "dbgscrn.h"
#include "dbgcmdln.h"
#include "wndsys.h"
#include "trpsys.h"
#include "dbglkup.h"
#include "dbginit.h"
#include "wininit.h"
#include "guigsysh.h"


enum {
    DEBUG_SCREEN,
    USER_SCREEN,
    UNKNOWN_SCREEN
} ScreenState = UNKNOWN_SCREEN;

static unsigned     ScrnLines = 50;
static bool         WantFast;
static bool         TrapForceHardMode = false;
static unsigned     FlipMech;
static unsigned     ScrnMode;
static HWND         FocusWnd;

static const char ScreenOptNameTab[] = {
    #define pick(t,e)   t "\0"
        SCREEN_OPTS()
    #undef pick
};

#if 0
void ToggleHardMode( void )
{
    TrapForceHardMode = !TrapForceHardMode;
#ifdef GUI_IS_GUI
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
    TRAP_EXTFUNC( UnLockInput )();
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
    buff[0] = NULLCHAR;
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
        TRAP_EXTFUNC( UnLockInput )();
    }
    return( false );
}

void SaveMainWindowPos( void )
{
    SaveMainScreen( "WDWIN" );
}

char *GUIGetWindowClassName( void )
{
    return( "WatcomDebugger" );
}

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
