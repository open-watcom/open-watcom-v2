/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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


#include <rdos.h>
#include "uidef.h"
#include "uishift.h"
#include "uimouse.h"


#define KEY_NUM_ACTIVE          0x200
#define KEY_CAPS_ACTIVE         0x100
#define KEY_PRINT_PRESSED       0x20
#define KEY_SCROLL_PRESSED      0x10
#define KEY_PAUSE_PRESSED       0x8
#define KEY_CTRL_PRESSED        0x4
#define KEY_ALT_PRESSED         0x2
#define KEY_SHIFT_PRESSED       0x1

int                     WaitHandle;

static bool             KeyInstalled;
static ORD              currMouseRow;
static ORD              currMouseCol;
static MOUSESTAT        currMouseStatus;

static shiftkey_event   ShiftkeyEvents[] = {
    EV_SHIFT_PRESS,     EV_SHIFT_RELEASE,   // 0x0001
    EV_ALT_PRESS,       EV_ALT_RELEASE,     // 0x0002
    EV_CTRL_PRESS,      EV_CTRL_RELEASE,    // 0x0004
    ___,                ___,                // 0x0008
    EV_SCROLL_PRESS,    EV_SCROLL_RELEASE,  // 0x0010
    ___,                ___,                // 0x0020
    ___,                ___,                // 0x0040
    ___,                ___,                // 0x0080
    EV_CAPS_PRESS,      EV_CAPS_RELEASE,    // 0x0100
    EV_NUM_PRESS,       EV_NUM_RELEASE      // 0x0200
};

static ui_event KeyEventProc( void )
{
    int                 ext;
    int                 keystate;
    int                 vk;
    int                 scan;
    int                 key;
    unsigned char       ascii;
    ui_event            ui_ev;
    int                 changed;

    if( RdosReadKeyEvent( &ext, &keystate, &vk, &scan ) ) {
        ascii = ext;
        if( scan != 0 && ascii == 0xe0 ) {  /* extended keyboard */
            ascii = 0;
        }
        ui_ev = scan + 0x100;
        /* ignore shift key for numeric keypad if numlock is not on */
        if( ui_ev >= EV_HOME && ui_ev <= EV_DELETE ) {
            if( (keystate & KEY_NUM_ACTIVE) == 0 ) {
                if( keystate & KEY_SHIFT_PRESSED ) {
                    ascii = 0;      /* wipe out digit */
                }
            }
        }
        if( ascii != 0 ) {
            ui_ev = ascii;
            if( (keystate & KEY_ALT_PRESSED) && ( ascii == ' ' ) ) {
                ui_ev = EV_ALT_SPACE;
            } else if( scan != 0 ) {
                switch( ascii + 0x100 ) {
                case EV_RUB_OUT:
                case EV_TAB_FORWARD:
                case EV_ENTER:
                case EV_ESCAPE:
                    ui_ev = ascii + 0x100;
                    break;
                }
            }
        }
    } else {
        changed = ( keystate ^ UIData->old_shift );
        if( changed != 0 ) {
            scan = 1;
            for( key = 0; key < sizeof( ShiftkeyEvents ) / sizeof( ShiftkeyEvents[0] ); key++ ) {
                if( changed & scan ) {
                    if( keystate & scan ) {
                        UIData->old_shift |= scan;
                        return( ShiftkeyEvents[key].press );
                    } else {
                        UIData->old_shift &= ~scan;
                        return( ShiftkeyEvents[key].release );
                    }
                }
                scan <<= 1;
            }
        }
        ui_ev = EV_NO_EVENT;
    }
    return( ui_ev );
}

static ui_event MouseEventProc( void )
{
    MOUSESTAT   stat;
    int         row;
    int         col;

    stat = 0;
    if( RdosGetLeftButton() )
        stat |= UI_MOUSE_PRESS;
    if( RdosGetRightButton() )
        stat |= UI_MOUSE_PRESS_RIGHT;

    RdosGetMousePosition(  &col, &row );

    if( stat != currMouseStatus ) {
        if( (stat & UI_MOUSE_PRESS) == 0 && (currMouseStatus & UI_MOUSE_PRESS) )
            RdosGetLeftButtonReleasePosition( &col, &row );

        if( (stat & UI_MOUSE_PRESS_RIGHT) == 0 && (currMouseStatus & UI_MOUSE_PRESS_RIGHT) )
            RdosGetRightButtonReleasePosition( &col, &row );

        if( (stat & UI_MOUSE_PRESS) && (currMouseStatus & UI_MOUSE_PRESS) == 0 )
            RdosGetLeftButtonPressPosition( &col, &row );

        if( (stat & UI_MOUSE_PRESS_RIGHT) && (currMouseStatus & UI_MOUSE_PRESS_RIGHT) == 0 ) {
            RdosGetRightButtonPressPosition( &col, &row );
        }
    }
    currMouseRow = row;
    currMouseCol = col;
    currMouseStatus = stat;

    return mouseevent();
}

bool intern initkeyboard( void )
{
    if( !KeyInstalled ) {
        if( WaitHandle == 0 )
            WaitHandle = RdosCreateWait();

        RdosAddWaitForKeyboard( WaitHandle, (int)KeyEventProc );
    }
    KeyInstalled = true;

    return( true );
}

void intern finikeyboard( void )
{
    if( KeyInstalled ) {
        if( !MouseInstalled ) {
            RdosCloseWait( WaitHandle );
            WaitHandle = 0;
        }
    }
    KeyInstalled = false;
}

void intern flushkey( void )
{
    RdosClearKeyboard();
}

void intern kbdspawnstart( void )
{
    finikeyboard();
}

void intern kbdspawnend( void )
{
    initkeyboard();
}

void intern mousespawnstart( void )
{
    uihidemouse();
}

void intern mousespawnend( void )
{
}

void uimousespeed( unsigned speed )
{
    /* unused parameters */ (void)speed;
}

bool UIAPI initmouse( init_mode install )
{
    MOUSETIME   tmp;

    if( install == INIT_MOUSELESS ) {
        return( false );
    }
    UIData->mouse_xscale = 8;
    UIData->mouse_yscale = 8;

    if( !MouseInstalled ) {
        if( WaitHandle == 0 )
            WaitHandle = RdosCreateWait();

        RdosAddWaitForMouse( WaitHandle, (int)MouseEventProc );
        RdosSetMouseWindow( 0, 0, 8 * UIData->width - 1, 8 * UIData->height - 1 );
        RdosSetMouseMickey( 8, 8 );
        RdosShowMouse();
    }

    MouseOn = false;
    MouseInstalled = true;

    UIData->mouse_swapped = false;
    checkmouse( &MouseStatus, &MouseRow, &MouseCol, &tmp );
    return( MouseInstalled );
}

void UIAPI finimouse( void )
{
    if( MouseInstalled ) {
        uioffmouse();

        if( !KeyInstalled ) {
            RdosCloseWait( WaitHandle );
            WaitHandle = 0;
        }
        MouseInstalled = false;
    }
}

void UIAPI uisetmouseposn( ORD row, ORD col )
{
    uisetmouse( row, col );
}

void intern checkmouse( MOUSESTAT *pstatus, MOUSEORD *prow, MOUSEORD *pcol, MOUSETIME *ptime )
{
    *pstatus = currMouseStatus;
    *prow = currMouseRow;
    *pcol = currMouseCol;
    *ptime = uiclock();
    uisetmouse( *prow, *pcol );
}

unsigned char UIAPI uicheckshift( void )
/***************************************/
{
    unsigned char   kst = 0;
    int             state = RdosGetKeyboardState();

    if( state & KEY_NUM_ACTIVE )
        kst |= 0x20;

    if( state & KEY_CAPS_ACTIVE )
        kst |= 0x40;

    if( state & KEY_SCROLL_PRESSED )
        kst |= 0x10;

    if( state & KEY_CTRL_PRESSED )
        kst |= 0x4;

    if( state & KEY_ALT_PRESSED )
        kst |= 0x8;

    if( state & KEY_SHIFT_PRESSED )
        kst |= 0x1;

    return kst;
}
