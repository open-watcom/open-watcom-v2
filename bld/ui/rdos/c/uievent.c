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


#include <rdos.h>
#include <stdlib.h>
#include <malloc.h>
#include <dos.h>
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

int                  WaitHandle;

static int                  KeyInstalled;

static ORD                  currMouseRow;
static ORD                  currMouseCol;
static ORD                  currMouseStatus;

extern MOUSEORD             MouseRow;
extern MOUSEORD             MouseCol;
extern bool                 MouseOn;
extern bool                 MouseInstalled;
extern unsigned short int   MouseStatus;

static EVENT    EventsPress[]   = {
    EV_SHIFT_PRESS,
    EV_ALT_PRESS,
    EV_CTRL_PRESS,
    EV_NO_EVENT,
    EV_SCROLL_PRESS,
    EV_NO_EVENT,
    EV_CAPS_PRESS,
    EV_NUM_PRESS
};

static EVENT    EventsRelease[] = {
    EV_SHIFT_RELEASE,
    EV_ALT_RELEASE,
    EV_CTRL_RELEASE,
    EV_NO_EVENT,
    EV_SCROLL_RELEASE,
    EV_NO_EVENT,
    EV_CAPS_RELEASE,
    EV_NUM_RELEASE
};

static EVENT KeyEventProc()
{
    int                 ext;
    int                 keystate;
    int                 vk;
    int                 scan;
    unsigned char       key;
    unsigned char       ascii;
    EVENT               ev;
    unsigned char       changed;

    if( RdosReadKeyEvent( &ext, &keystate, &vk, &scan ) ) {
        ascii = ext;
        if( scan != 0 && ascii == 0xe0 ) {  /* extended keyboard */
            ascii = 0;
        }
        ev = scan + 0x100;
        /* ignore shift key for numeric keypad if numlock is not on */
        if( ev >= EV_HOME && ev <= EV_DELETE ) {
            if( ( keystate & KEY_NUM_ACTIVE ) == 0 ) {
                if( ( keystate & KEY_SHIFT_PRESSED ) != 0 ) {
                    ascii = 0;      /* wipe out digit */
                }
            }
        }
        if( ascii != 0 ) {
            ev = ascii;
            if( ( keystate & KEY_ALT_PRESSED ) && ( ascii == ' ' ) ) {
                ev = EV_ALT_SPACE;
            } else if( scan != 0 ) {
                switch( ascii + 0x100 ) {
                case EV_RUB_OUT:
                case EV_TAB_FORWARD:
                case EV_ENTER:
                case EV_ESCAPE:
                    ev = ascii + 0x100;
                    break;
                }
            }
        }
    } else {
        changed = ( keystate ^ UIData->old_shift );
        if( changed != 0 ) {
            key = 0;
            scan = 1;
            while( scan < (1 << 8) ) {
                if( ( changed & scan ) != 0 ) {
                    if( ( keystate & scan ) != 0 ) {
                        UIData->old_shift |= scan;
                        return( EventsPress[ key ] );
                    } else {
                        UIData->old_shift &= ~scan;
                        return( EventsRelease[ key ] );
                    }
                }
                scan <<= 1;
                ++key;
            }
        }
        ev = EV_NO_EVENT;
    }
    return( ev );
}

static EVENT MouseEventProc()
{
    ORD stat = 0;
    int row;
    int col;
    
    if( RdosGetLeftButton() )
        stat |= MOUSE_PRESS;

    if( RdosGetRightButton() )
        stat |= MOUSE_PRESS_RIGHT;

    RdosGetMousePosition(  &col, &row );

    if( stat != currMouseStatus ) {
        if( !(stat & MOUSE_PRESS) && (currMouseStatus & MOUSE_PRESS) )
            RdosGetLeftButtonReleasePosition( &col, &row );
    
        if( !(stat & MOUSE_PRESS_RIGHT) && (currMouseStatus & MOUSE_PRESS_RIGHT) )
            RdosGetRightButtonReleasePosition( &col, &row );
    
        if( (stat & MOUSE_PRESS) && !(currMouseStatus & MOUSE_PRESS) )
            RdosGetLeftButtonPressPosition( &col, &row );
    
        if( (stat & MOUSE_PRESS_RIGHT) && !(currMouseStatus & MOUSE_PRESS_RIGHT) )
            RdosGetRightButtonPressPosition( &col, &row );
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

        RdosAddWaitForKeyboard( WaitHandle, &KeyEventProc );
    }
    KeyInstalled = TRUE;
    
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
    KeyInstalled = FALSE;
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
    if( (int)speed <= 0 ) {
        speed = 1;
    }
}

bool UIAPI initmouse( int install )
{
    unsigned long tmp;

    if( install == 0 ) {
        return( FALSE );
    }
    UIData->mouse_xscale = 8;
    UIData->mouse_yscale = 8;

    if( !MouseInstalled ) {
        if( WaitHandle == 0 )
            WaitHandle = RdosCreateWait();

        RdosAddWaitForMouse( WaitHandle, &MouseEventProc );
        RdosSetMouseWindow( 0, 0, 8 * 80 - 1, 8 * 25 - 1 );
        RdosSetMouseMickey( 8, 8 );
    }

    MouseOn = FALSE;
    MouseInstalled = TRUE;

    UIData->mouse_swapped = FALSE;
    checkmouse( &MouseStatus, &MouseRow, &MouseCol, &tmp );
    return( MouseInstalled );
}

void intern finimouse( void )
{
    if( MouseInstalled ) {
        uioffmouse();

        if( !KeyInstalled ) {
            RdosCloseWait( WaitHandle );
            WaitHandle = 0;
        }
        MouseInstalled = FALSE;
    }
}

void UIAPI uisetmouseposn( ORD row, ORD col )
{
    uisetmouse( row, col );
}

void intern checkmouse( unsigned short *pstatus, MOUSEORD *prow,
                        MOUSEORD *pcol, unsigned long *ptime )
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
    unsigned char kst = 0;
    int state = RdosGetKeyboardState();
    
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
