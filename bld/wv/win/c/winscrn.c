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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <dos.h>
#include <stddef.h>
#include <windows.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "stdui.h"
#include "winscrn.h"
#include "swap.h"
#include "dbgscrn.h"
#include "uirefrhk.h"
#include "guigmous.h"
#include "dbgcmdln.h"
#include "dbglkup.h"
#include "trptypes.h"
#include "trpld.h"
#include "trpsys.h"
#include "dbginit.h"
#include "wininit.h"
#include "setevent.h"
#include "uiwinhk.h"
#include "int10.h"


#define TstMono()       ChkCntrlr( VIDMONOINDXREG )
#define TstColour()     ChkCntrlr( VIDCOLORINDXREG )

static bool             WantFast;
static int              ScrnLines = 25;
static flip_types       FlipMech;
static mode_types       ScrnMode;
static display_config   HWDisplay;

static const char ScreenOptNameTab[] = {
    #define pick(t,e)   t "\0"
        SCREEN_OPTS()
    #undef pick
};

void InitHookFunc( void )
{
    TRAP_EXTFUNC( InputHook )( win_uihookrtn );
}

void FiniHookFunc( void )
{
}

void Ring_Bell( void )
{
    MessageBeep( 0 );
}

/*
 * UsrScrnMode -- setup the user screen mode
 */
bool UsrScrnMode( void )
{
#if 0
    if( UIData->height != ScrnLines ) {
        UIData->height = ScrnLines;
        if( _IsOn( SW_USE_MOUSE ) ) {
            /*
                This is a sideways dive into the UI to get the boundries of
                the mouse cursor properly defined.
            */
            initmouse( INIT_MOUSE );
        }
    }
#endif
    return( false );
}

void DbgScrnMode( void )
{
}

/*
 * DebugScreen -- swap to debugger screen
 */
bool DebugScreen( void )
{
    if( FlipMech == FLIP_SWAP ) {
        ToCharacter();
        WndDirty( NULL );
        return( false );
    } else {
        return( true );
    }
}

bool DebugScreenRecover( void )
{
    return( true );
}

/*
 * UserScreen -- swap to user screen
 */
bool UserScreen( void )
{
    bool rc = false;
    if( FlipMech == FLIP_SWAP ) {
        ToGraphical();
        rc = false;
    } else {
        rc = true;
    }
    return( rc );
}

void SaveMainWindowPos( void )
{
}

void FiniScreen( void )
{
    if( _IsOn( SW_USE_MOUSE ) )
        GUIFiniMouse();
    uistop();
    if( FlipMech == FLIP_SWAP ) {
        FiniSwapper();
        InvalidateRect( HWND_DESKTOP, NULL, true );
    }
}

void InitScreen( void )
{
    if( FlipMech == FLIP_SWAP ) {
        InitSwapper( WantFast );
    }
    if( FlipMech == FLIP_SWAP ) {
        ToCharacter();
    }
    uistart();
    UIData->height = ScrnLines;
    if( _IsOn( SW_USE_MOUSE ) ) {
        GUIInitMouse( INIT_MOUSE );
    }
}

static bool ChkCntrlr( unsigned port )
{
    unsigned char   curr;
    bool            rtrn;

    curr = VIDGetRow( port );
    VIDSetRow( port, 0x5a );
    VIDWait();
    VIDWait();
    VIDWait();
    rtrn = ( VIDGetRow( port ) == 0x5a );
    VIDSetRow( port, curr );
    return( rtrn );
}

static void GetDispConfig( void )
{
    int10_ega_info      info;
    unsigned char       curr_mode;
    hw_display_type     temp;
    unsigned            dev_config;

    dev_config = _BIOSVideoDevCombCode();
    HWDisplay.active = dev_config & 0xff;
    HWDisplay.alt = (dev_config >> 8) & 0xff;
    if( HWDisplay.active != DISP_NONE )
        return;
    /* have to figure it out ourselves */
    curr_mode = _BIOSVideoGetMode() & 0x7f;
    info = _BIOSVideoEGAInfo();
    if( info.switches < 12 && info.mem <= 3 && info.mono <= 1 ) {
        /* we have an EGA */
        if( info.mono == 0 ) {
            HWDisplay.active = DISP_EGA_COLOUR;
            if( TstMono() ) {
                HWDisplay.alt = DISP_MONOCHROME;
            }
        } else {
            HWDisplay.active = DISP_EGA_MONO;
            if( TstColour() ) {
                HWDisplay.alt = DISP_CGA;
            }
        }
        if( HWDisplay.active == DISP_EGA_COLOUR && ISMONOMODE( curr_mode )
         || HWDisplay.active == DISP_EGA_MONO && !ISMONOMODE( curr_mode ) ) {
            /* EGA is not the active display */

            temp = HWDisplay.active;
            HWDisplay.active = HWDisplay.alt;
            HWDisplay.alt = temp;
        }
        return;
    }
    if( TstMono() ) {
        /* have a monochrome display */
        HWDisplay.active = DISP_MONOCHROME;
        if( TstColour() ) {
            if( curr_mode == 7 ) {
                HWDisplay.alt    = DISP_CGA;
            } else {
                HWDisplay.active = DISP_CGA;
                HWDisplay.alt    = DISP_MONOCHROME;
            }
        }
        return;
    }
    /* only thing left is a single CGA display */
    HWDisplay.active = DISP_CGA;
}

/*
 * ConfigScreen -- figure out screen configuration we're going to use.
 */
unsigned ConfigScreen( void )
{
    GetDispConfig();
    if( !(FlipMech == FLIP_TWO && HWDisplay.alt == DISP_MONOCHROME) ) {
        FlipMech = FLIP_SWAP;
        switch( HWDisplay.active ) {
        case DISP_VGA_MONO:
        case DISP_VGA_COLOUR:
            if( ScrnLines > 25 )
                ScrnLines = 50;
            win_uisetcolor( M_VGA );
            break;
        case DISP_EGA_COLOUR:
        case DISP_EGA_MONO:
            if( ScrnLines > 25 )
                ScrnLines = 43;
            // fall through
        default:
            win_uisetcolor( M_EGA );
            break;
        }
    } else {
        win_uisetmono();
    }
    return( 0 );
}

/*****************************************************************************\
 *                                                                           *
 *            Replacement routines for User Interface library                *
 *                                                                           *
\*****************************************************************************/

void UIAPI uirefresh( void )
{
    if( ScrnState & DBG_SCRN_ACTIVE ) {
        _uirefresh();
    }
}

/*****************************************************************************/

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
