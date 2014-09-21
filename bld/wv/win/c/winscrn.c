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


#include "dbgdefn.h"
#include "aui.h"
#include <stdui.h>
#include <string.h>
#include <dos.h>
#include <stddef.h>
#include "dosscrn.h"
#include "dbgtoggl.h"
#include <windows.h>
#include "winscrn.h"
#include "swap.h"
#include "dbgscrn.h"

extern void __far HookRtn( unsigned event, unsigned info );
extern void (__far __pascal *HookFunc)( void __far (*)( unsigned, unsigned ) );
extern int      GUIInitMouse( int );
extern void     GUIFiniMouse( void );


extern unsigned char    NECBIOSGetMode(void);
#pragma aux NECBIOSGetMode =                                    \
0X55            /* push   bp                            */      \
0XB4 0X0B       /* mov    ah,b                          */      \
0XCD 0X18       /* int    18                            */      \
0X5D            /* pop    bp                            */      \
        parm caller [ ax ]                                      \
        modify [ bx ];


#define         NEC_20_LINES        0x01
#define         NEC_31_LINES        0x10

extern screen_state             ScrnState;

bool            WantFast;
flip_types      FlipMech;
mode_types      ScrnMode=MD_EGA;
int             ScrnLines=25;
volatile int    BrkPending;
bool WndUseGMouse = FALSE;

static display_configuration    HWDisplay;

void InitHookFunc( void )
{
    HookFunc( HookRtn );
}

void FiniHookFunc( void )
{
}

void Ring_Bell( void )
{
    MessageBeep( 0 );
}

void ForceLines( unsigned lines )
{
    ScrnLines = lines;
}

int SwapScrnLines( void )
{
    return( ScrnLines );
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
            if( ScrnLines > 25 ) ScrnLines = 50;
            win_uisetcolor( M_VGA );
            break;
        case DISP_EGA_COLOUR:
        case DISP_EGA_MONO:
            if( ScrnLines > 25 ) ScrnLines = 43;
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
            extern int  initmouse( int );
            /*
                This is a sideways dive into the UI to get the boundries of
                the mouse cursor properly defined.
            */
            initmouse( 1 );
        }
    }
#endif
    return( FALSE );
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
        return( FALSE );
    } else {
        return( TRUE );
    }
}

bool DebugScreenRecover( void )
{
    return( TRUE );
}

/*
 * UserScreen -- swap to user screen
 */
bool UserScreen( void )
{
    bool rc = FALSE;
    if( FlipMech == FLIP_SWAP ) {
        ToGraphical();
        rc = FALSE;
    } else {
        rc = TRUE;
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
        InvalidateRect( HWND_DESKTOP, NULL, TRUE );
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
    if( _IsOn( SW_USE_MOUSE ) ) GUIInitMouse( 1 );
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
    if( !ChkCntrlr( VIDMONOINDXREG ) ) return( FALSE );
    return( TRUE );
}

static bool TstColour( void )
{
    if( !ChkCntrlr( VIDCOLRINDXREG ) ) return( FALSE );
    return( TRUE );
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

void *uifaralloc( size_t size )
{
    return( ExtraAlloc( size ) );
}

void uifarfree( void *ptr )
{
    ExtraFree( ptr );
}

void uirefresh( void )
{
    extern void uidorefresh(void);

    if( ScrnState & DBG_SCRN_ACTIVE ) {
        uidorefresh();
    }
}

bool SysGUI( void )
{
    return( FALSE );
}
