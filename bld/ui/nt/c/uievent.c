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


#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <dos.h>
#include "uidef.h"
#include "uishift.h"
#include "uimouse.h"

static unsigned char shift_state;

typedef struct {
    WORD vk;
    WORD reg;
    WORD shift;
    WORD ctrl;
    WORD alt;
} map;

static const map events[] = {
    { VK_BACK, EV_RUB_OUT, EV_RUB_OUT, EV_RUB_OUT, EV_RUB_OUT },
    { VK_TAB, EV_TAB_FORWARD, EV_TAB_BACKWARD, EV_CTRL_TAB, EV_TAB_FORWARD },
    { VK_RETURN, EV_ENTER, EV_ENTER, EV_ENTER, EV_ENTER },
    { VK_ESCAPE, EV_ESCAPE, EV_ESCAPE, EV_ESCAPE, EV_ESCAPE },
    { VK_PRIOR, EV_PAGE_UP, EV_PAGE_UP, EV_CTRL_PGUP, EV_ALT_PGUP },
    { VK_NEXT, EV_PAGE_DOWN, EV_PAGE_DOWN, EV_CTRL_PGDN, EV_ALT_PGDN },
    { VK_END, EV_END, EV_END, EV_CTRL_END, EV_ALT_END },
    { VK_HOME, EV_HOME, EV_HOME, EV_CTRL_HOME, EV_ALT_HOME },
    { VK_LEFT, EV_CURSOR_LEFT, EV_CURSOR_LEFT, EV_CTRL_LEFT, EV_CURSOR_LEFT },
    { VK_UP, EV_CURSOR_UP, EV_CURSOR_UP, EV_CTRL_UP, EV_CURSOR_UP },
    { VK_RIGHT, EV_CURSOR_RIGHT, EV_CURSOR_RIGHT, EV_CTRL_RIGHT, EV_CURSOR_RIGHT },
    { VK_DOWN, EV_CURSOR_DOWN, EV_CURSOR_DOWN, EV_CTRL_DOWN, EV_CURSOR_DOWN },
    { VK_INSERT, EV_INSERT, EV_INSERT, EV_CTRL_INSERT, EV_ALT_INSERT},
    { VK_DELETE, EV_DELETE, EV_DELETE, EV_CTRL_DELETE, EV_ALT_DELETE },
    { 'A', 'a', 'A', 'A'-'A'+1, EV_ALT_A },
    { 'B', 'b', 'B', 'B'-'A'+1, EV_ALT_B },
    { 'C', 'c', 'C', 'C'-'A'+1, EV_ALT_C },
    { 'D', 'd', 'D', 'D'-'A'+1, EV_ALT_D },
    { 'E', 'e', 'E', 'E'-'A'+1, EV_ALT_E },
    { 'F', 'f', 'F', 'F'-'A'+1, EV_ALT_F },
    { 'G', 'g', 'G', 'G'-'A'+1, EV_ALT_G },
    { 'H', 'h', 'H', 'H'-'A'+1, EV_ALT_H },
    { 'I', 'i', 'I', 'I'-'A'+1, EV_ALT_I },
    { 'J', 'j', 'J', 'J'-'A'+1, EV_ALT_J },
    { 'K', 'k', 'K', 'K'-'A'+1, EV_ALT_K },
    { 'L', 'l', 'L', 'L'-'A'+1, EV_ALT_L },
    { 'M', 'm', 'M', 'M'-'A'+1, EV_ALT_M },
    { 'N', 'n', 'N', 'N'-'A'+1, EV_ALT_N },
    { 'O', 'o', 'O', 'O'-'A'+1, EV_ALT_O },
    { 'P', 'p', 'P', 'P'-'A'+1, EV_ALT_P },
    { 'Q', 'q', 'Q', 'Q'-'A'+1, EV_ALT_Q },
    { 'R', 'r', 'R', 'R'-'A'+1, EV_ALT_R },
    { 'S', 's', 'S', 'S'-'A'+1, EV_ALT_S },
    { 'T', 't', 'T', 'T'-'A'+1, EV_ALT_T },
    { 'U', 'u', 'U', 'U'-'A'+1, EV_ALT_U },
    { 'V', 'v', 'V', 'V'-'A'+1, EV_ALT_V },
    { 'W', 'w', 'W', 'W'-'A'+1, EV_ALT_W },
    { 'X', 'x', 'X', 'X'-'A'+1, EV_ALT_X },
    { 'Y', 'y', 'Y', 'Y'-'A'+1, EV_ALT_Y },
    { 'Z', 'z', 'Z', 'Z'-'A'+1, EV_ALT_Z },
    { VK_F1, EV_FUNC( 1 ), EV_SHIFT_FUNC( 1 ), EV_CTRL_FUNC( 1 ), EV_ALT_FUNC( 1 ) },
    { VK_F2, EV_FUNC( 2 ), EV_SHIFT_FUNC( 2 ), EV_CTRL_FUNC( 2 ), EV_ALT_FUNC( 2 ) },
    { VK_F3, EV_FUNC( 3 ), EV_SHIFT_FUNC( 3 ), EV_CTRL_FUNC( 3 ), EV_ALT_FUNC( 3 ) },
    { VK_F4, EV_FUNC( 4 ), EV_SHIFT_FUNC( 4 ), EV_CTRL_FUNC( 4 ), EV_ALT_FUNC( 4 ) },
    { VK_F5, EV_FUNC( 5 ), EV_SHIFT_FUNC( 5 ), EV_CTRL_FUNC( 5 ), EV_ALT_FUNC( 5 ) },
    { VK_F6, EV_FUNC( 6 ), EV_SHIFT_FUNC( 6 ), EV_CTRL_FUNC( 6 ), EV_ALT_FUNC( 6 ) },
    { VK_F7, EV_FUNC( 7 ), EV_SHIFT_FUNC( 7 ), EV_CTRL_FUNC( 7 ), EV_ALT_FUNC( 7 ) },
    { VK_F8, EV_FUNC( 8 ), EV_SHIFT_FUNC( 8 ), EV_CTRL_FUNC( 8 ), EV_ALT_FUNC( 8 ) },
    { VK_F9, EV_FUNC( 9 ), EV_SHIFT_FUNC( 9 ), EV_CTRL_FUNC( 9 ), EV_ALT_FUNC( 9 ) },
    { VK_F10, EV_FUNC( 10 ), EV_SHIFT_FUNC( 10 ), EV_CTRL_FUNC( 10 ), EV_ALT_FUNC( 10 ) },
    { VK_F11, EV_FUNC_11, EV_SHIFT_FUNC_11, EV_CTRL_FUNC_11, EV_ALT_FUNC_11 },
    { VK_F12, EV_FUNC_12, EV_SHIFT_FUNC_12, EV_CTRL_FUNC_12, EV_ALT_FUNC_12 }
};

extern MOUSEORD MouseRow;
extern MOUSEORD MouseCol;
extern bool     MouseOn;
extern bool     MouseInstalled;
extern WORD     MouseStatus;

extern HANDLE   InputHandle;
static ORD      currMouseRow;
static ORD      currMouseCol;
static ORD      currMouseStatus;

static void setshiftstate( BOOL has_shift, BOOL has_ctrl, BOOL has_alt )
{
    shift_state = 0;
    if( has_shift ) {
        shift_state |= S_SHIFT;
    }
    if( has_ctrl ) {
        shift_state |= S_CTRL;
    }
    if( has_alt ) {
        shift_state |= S_ALT;
    }
}

static int CompareEvents( const void *p1, const void *p2 )
{
    return( ((map*)p1)->vk - ((map*)p2)->vk );
}

bool intern initkeyboard( void )
{
//    InputHandle = CreateFile( "CONIN$", GENERIC_READ | GENERIC_WRITE,
//                      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
//                      OPEN_EXISTING, 0, NULL );
//    SetConsoleMode( InputHandle, ENABLE_MOUSE_INPUT );
    return( true );
}

void intern finikeyboard( void )
{
//    if( InputHandle != NULL ) {
//      CloseHandle( InputHandle );
//      InputHandle = NULL;
//    }
}

void intern flushkey( void )
{
    DWORD       oldinputmode;

    GetConsoleMode( InputHandle, &oldinputmode );
    SetConsoleMode( InputHandle, oldinputmode & ~ENABLE_MOUSE_INPUT );
    FlushConsoleInputBuffer( InputHandle );
    SetConsoleMode( InputHandle, oldinputmode );
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
    DWORD       tmp;

    if( install == 0 ) {
        return( FALSE );
    }
    UIData->mouse_xscale = 1;  /* Craig -- do not delete or else! */
    UIData->mouse_yscale = 1;  /* Craig -- do not delete or else! */
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

/*
 * eventWeWant - test an input record and see if it is one
 *               we want to handle
 */
static BOOL eventWeWant( INPUT_RECORD *ir )
{
    WORD        vk;
    DWORD       st;

    if( ir->EventType == KEY_EVENT ) {
        if( !ir->Event.KeyEvent.bKeyDown ) {
            return( FALSE );
        }
        vk = ir->Event.KeyEvent.wVirtualKeyCode;
        if( vk == VK_CONTROL || vk == VK_SHIFT || vk == VK_MENU ) {
            return( FALSE );
        }
        return( TRUE );
    }
    if( ir->EventType == MOUSE_EVENT ) {
        currMouseCol = ir->Event.MouseEvent.dwMousePosition.X;
        currMouseRow = ir->Event.MouseEvent.dwMousePosition.Y;
        currMouseStatus = 0;
        st = ir->Event.MouseEvent.dwButtonState;
        if( st & (FROM_LEFT_2ND_BUTTON_PRESSED|FROM_LEFT_3RD_BUTTON_PRESSED |
                    FROM_LEFT_4TH_BUTTON_PRESSED|FROM_LEFT_1ST_BUTTON_PRESSED|
                    RIGHTMOST_BUTTON_PRESSED) ) {
            currMouseStatus = MOUSE_PRESS;
        }
        if( st & RIGHTMOST_BUTTON_PRESSED ) {
            currMouseStatus = MOUSE_PRESS_RIGHT;
        }
        return( TRUE );
    }
    return( FALSE );

} /* eventWeWant */

EVENT intern getanyevent( void )
{
    INPUT_RECORD        ir;
    DWORD               rd,ss;
    WORD                vk;
    EVENT               ascii;
    BOOL                has_alt, has_shift, has_ctrl;
    map                 *ev,what;
    EVENT               evnt;

    for( ;; ) {
        PeekConsoleInput( InputHandle, &ir, 1, &rd );
        if( rd == 0 ) return( EV_NO_EVENT );
        ReadConsoleInput( InputHandle, &ir, 1, &rd );
        if( eventWeWant( &ir ) ) {
            if( ir.EventType != MOUSE_EVENT ) break;
            evnt = mouseevent();
            if( evnt > EV_NO_EVENT ) return( evnt );
        }
    }

    vk = ir.Event.KeyEvent.wVirtualKeyCode;
    ascii = ir.Event.KeyEvent.uChar.AsciiChar;
    ss = ir.Event.KeyEvent.dwControlKeyState;
    has_shift = ss & SHIFT_PRESSED;
    has_ctrl = ss & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED);
    has_alt = ss & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED);
    setshiftstate( has_shift, has_ctrl, has_alt );
    what.vk = vk;

    ev = bsearch( &what, events, sizeof( events )/sizeof( map ),
                    sizeof( what ), CompareEvents );
    if( ev != NULL ) {
        if( has_shift ) {
            ascii = ev->shift;
        } else if( has_ctrl ) {
            ascii = ev->ctrl;
        } else if( has_alt ) {
            ascii = ev->alt;
        } else {
            ascii = ev->reg;
        }
    } else if( ascii == 0 ) {
        ascii = EV_NO_EVENT;
    }
    if( ascii > EV_NO_EVENT ) {
        uihidemouse();
    }
    return( ascii );

} /* getanyevent */


void intern waitforevent( void )
{
    DWORD               rd;
    INPUT_RECORD        ir;

    for( ;; ) {
        for( ;; ) {
            PeekConsoleInput( InputHandle, &ir, 1, &rd );
            if( rd > 0 ) {
                if( eventWeWant( &ir ) ) {
                    return;
                }
                ReadConsoleInput( InputHandle, &ir, 1, &rd );
            } else {
                break;
            }
        }
        WaitForSingleObject( InputHandle, INFINITE );
    }

} /* waitforevent */

unsigned char UIAPI uicheckshift( void )
/***************************************/
{
    return( shift_state );
}
