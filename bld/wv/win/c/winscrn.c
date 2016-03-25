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


#include <dos.h>
#include <stddef.h>
#include <windows.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "stdui.h"
#include "dosscrn.h"
#include "winscrn.h"
#include "swap.h"
#include "dbgscrn.h"
#include "uidbg.h"
#include "guigmous.h"
#include "dbgcmdln.h"
#include "dbglkup.h"
#include "trptypes.h"
#include "trpld.h"
#include "trpsys.h"
#include "dbginit.h"
#include "wininit.h"


extern unsigned char    NECBIOSGetMode(void);
#pragma aux NECBIOSGetMode =                                    \
0X55            /* push   bp                            */      \
0XB4 0X0B       /* mov    ah,b                          */      \
0XCD 0X18       /* int    18                            */      \
0X5D            /* pop    bp                            */      \
        parm caller [ax]                                      \
        modify [bx];


#define         NEC_20_LINES        0x01
#define         NEC_31_LINES        0x10

extern volatile bool   BrkPending;

bool            WantFast;
flip_types      FlipMech;
mode_types      ScrnMode = MD_EGA;
int             ScrnLines = 25;
bool            WndUseGMouse = false;

static display_configuration    HWDisplay;

void InitHookFunc( void )
{
    TrapInputHook( win_uihookrtn );
}

void FiniHookFunc( void )
{
}

void Ring_Bell( void )
{
    MessageBeep( 0 );
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
            // fall thru
        default:
            win_uisetcolor( M_EGA );
            break;
        }
    } else {
        win_uisetmono();
    }
    return( 0 );
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
            initmouse( 1 );
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
    if( _IsOn( SW_USE_MOUSE ) ) GUIFiniMouse();
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
        GUIInitMouse( 1 );
    }
}

static bool ChkCntrlr( int port )
{
    char curr;
    bool rtrn;

    curr = VIDGetRow( port );
    VIDSetRow( port, 0X5A );
    VIDWait();
    VIDWait();
    VIDWait();
    rtrn = ( VIDGetRow( port ) == 0X5A );
    VIDSetRow( port, curr );
    return( rtrn );
}

static bool TstMono( void )
{
    if( !ChkCntrlr( VIDMONOINDXREG ) ) return( false );
    return( true );
}

static bool TstColour( void )
{
    if( !ChkCntrlr( VIDCOLRINDXREG ) ) return( false );
    return( true );
}

static void GetDispConfig( void )
{
    signed long         info;
    unsigned char       colour;
    unsigned char       memory;
    unsigned char       swtchs;
    unsigned char       curr_mode;
    hw_display_type     temp;

    HWDisplay = BIOSDevCombCode();
    if( HWDisplay.active != DISP_NONE ) return;
    /* have to figure it out ourselves */
    curr_mode = BIOSGetMode() & 0x7f;
    info = BIOSEGAInfo();
    memory = info;
    colour = info >> 8;
    swtchs = info >> 16;
    if( swtchs < 12 && memory <= 3 && colour <= 1 ) {
        /* we have an EGA */
        if( colour == 0 ) {
            HWDisplay.active = DISP_EGA_COLOUR;
            if( TstMono() ) HWDisplay.alt = DISP_MONOCHROME;
        } else {
            HWDisplay.active = DISP_EGA_MONO;
            if( TstColour() ) HWDisplay.alt = DISP_CGA;
        }
        if( HWDisplay.active == DISP_EGA_COLOUR
                && (curr_mode==7 || curr_mode==15)
         || HWDisplay.active == DISP_EGA_MONO
                && (curr_mode!=7 && curr_mode!=15) ) {
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
            if( curr_mode != 7 ) {
                HWDisplay.active = DISP_CGA;
                HWDisplay.alt    = DISP_MONOCHROME;
            } else {
                HWDisplay.alt    = DISP_CGA;
            }
        }
        return;
    }
    /* only thing left is a single CGA display */
    HWDisplay.active = DISP_CGA;
}

/*****************************************************************************\
 *                                                                           *
 *            Replacement routines for User Interface library                *
 *                                                                           *
\*****************************************************************************/

void uirefresh( void )
{
    if( ScrnState & DBG_SCRN_ACTIVE ) {
        _uirefresh();
    }
}

bool SysGUI( void )
{
    return( false );
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
