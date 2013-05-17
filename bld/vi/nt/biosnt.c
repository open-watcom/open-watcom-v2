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
* Description:  BIOS emulation routines for Win32.
*
****************************************************************************/


#include "vi.h"
#include "win.h"
#include "vibios.h"

typedef struct {
    WORD    vk;
    vi_key  reg;
    vi_key  shift;
    vi_key  ctrl;
    vi_key  alt;
} map;

static const map events[] = {
    { VK_BACK, VI_KEY( BS ), VI_KEY( BS ), VI_KEY( BS ), VI_KEY( BS ) },
    { VK_TAB, VI_KEY( TAB ), VI_KEY( SHIFT_TAB ), VI_KEY( CTRL_TAB ), VI_KEY( ALT_TAB ) },
    { VK_RETURN, VI_KEY( ENTER ), VI_KEY( ENTER ), VI_KEY( ENTER ), VI_KEY( ENTER ) },
    { VK_ESCAPE, VI_KEY( ESC ), VI_KEY( ESC ), VI_KEY( ESC ), VI_KEY( ESC ) },
    { VK_SPACE, ' ', ' ', ' ', ' ' },
    { VK_PRIOR, VI_KEY( PAGEUP ), VI_KEY( SHIFT_PAGEUP ), VI_KEY( CTRL_PAGEUP ), VI_KEY( ALT_PAGEUP ) },
    { VK_NEXT, VI_KEY( PAGEDOWN ), VI_KEY( SHIFT_PAGEDOWN ), VI_KEY( CTRL_PAGEDOWN ), VI_KEY( ALT_PAGEDOWN ) },
    { VK_END, VI_KEY( END ), VI_KEY( SHIFT_END ), VI_KEY( CTRL_END ), VI_KEY( ALT_END ) },
    { VK_HOME, VI_KEY( HOME ), VI_KEY( SHIFT_HOME ), VI_KEY( CTRL_HOME ), VI_KEY( ALT_HOME ) },
    { VK_LEFT, VI_KEY( LEFT ), VI_KEY( SHIFT_LEFT ), VI_KEY( CTRL_LEFT ), VI_KEY( ALT_LEFT ) },
    { VK_UP, VI_KEY( UP ), VI_KEY( SHIFT_UP ), VI_KEY( CTRL_UP ), VI_KEY( ALT_UP ) },
    { VK_RIGHT, VI_KEY( RIGHT ), VI_KEY( SHIFT_RIGHT ), VI_KEY( CTRL_RIGHT ), VI_KEY( ALT_RIGHT ) },
    { VK_DOWN, VI_KEY( DOWN ), VI_KEY( SHIFT_DOWN ), VI_KEY( CTRL_DOWN ), VI_KEY( ALT_DOWN ) },
    { VK_INSERT, VI_KEY( INS ), VI_KEY( SHIFT_INS ), VI_KEY( CTRL_INS ), VI_KEY( ALT_INS ) },
    { VK_DELETE, VI_KEY( DEL ), VI_KEY( SHIFT_DEL ), VI_KEY( CTRL_DEL ), VI_KEY( ALT_DEL ) },
    { 'A', 'a', 'A', VI_KEY( CTRL_A ), VI_KEY( ALT_A ) },
    { 'B', 'b', 'B', VI_KEY( CTRL_B ), VI_KEY( ALT_B ) },
    { 'C', 'c', 'C', VI_KEY( CTRL_C ), VI_KEY( ALT_C ) },
    { 'D', 'd', 'D', VI_KEY( CTRL_D ), VI_KEY( ALT_D ) },
    { 'E', 'e', 'E', VI_KEY( CTRL_E ), VI_KEY( ALT_E ) },
    { 'F', 'f', 'F', VI_KEY( CTRL_F ), VI_KEY( ALT_F ) },
    { 'G', 'g', 'G', VI_KEY( CTRL_G ), VI_KEY( ALT_G ) },
    { 'H', 'h', 'H', VI_KEY( CTRL_H ), VI_KEY( ALT_H ) },
    { 'I', 'i', 'I', VI_KEY( CTRL_I ), VI_KEY( ALT_I ) },
    { 'J', 'j', 'J', VI_KEY( CTRL_J ), VI_KEY( ALT_J ) },
    { 'K', 'k', 'K', VI_KEY( CTRL_K ), VI_KEY( ALT_K ) },
    { 'L', 'l', 'L', VI_KEY( CTRL_L ), VI_KEY( ALT_L ) },
    { 'M', 'm', 'M', VI_KEY( CTRL_M ), VI_KEY( ALT_M ) },
    { 'N', 'n', 'N', VI_KEY( CTRL_N ), VI_KEY( ALT_N ) },
    { 'O', 'o', 'O', VI_KEY( CTRL_O ), VI_KEY( ALT_O ) },
    { 'P', 'p', 'P', VI_KEY( CTRL_P ), VI_KEY( ALT_P ) },
    { 'Q', 'q', 'Q', VI_KEY( CTRL_Q ), VI_KEY( ALT_Q ) },
    { 'R', 'r', 'R', VI_KEY( CTRL_R ), VI_KEY( ALT_R ) },
    { 'S', 's', 'S', VI_KEY( CTRL_S ), VI_KEY( ALT_S ) },
    { 'T', 't', 'T', VI_KEY( CTRL_T ), VI_KEY( ALT_T ) },
    { 'U', 'u', 'U', VI_KEY( CTRL_U ), VI_KEY( ALT_U ) },
    { 'V', 'v', 'V', VI_KEY( CTRL_V ), VI_KEY( ALT_V ) },
    { 'W', 'w', 'W', VI_KEY( CTRL_W ), VI_KEY( ALT_W ) },
    { 'X', 'x', 'X', VI_KEY( CTRL_X ), VI_KEY( ALT_X ) },
    { 'Y', 'y', 'Y', VI_KEY( CTRL_Y ), VI_KEY( ALT_Y ) },
    { 'Z', 'z', 'Z', VI_KEY( CTRL_Z ), VI_KEY( ALT_Z ) },
    { VK_F1, VI_KEY( F1 ), VI_KEY( SHIFT_F1 ), VI_KEY( CTRL_F1 ), VI_KEY( ALT_F1 ) },
    { VK_F2, VI_KEY( F2 ), VI_KEY( SHIFT_F2 ), VI_KEY( CTRL_F2 ), VI_KEY( ALT_F2 ) },
    { VK_F3, VI_KEY( F3 ), VI_KEY( SHIFT_F3 ), VI_KEY( CTRL_F3 ), VI_KEY( ALT_F3 ) },
    { VK_F4, VI_KEY( F4 ), VI_KEY( SHIFT_F4 ), VI_KEY( CTRL_F4 ), VI_KEY( ALT_F4 ) },
    { VK_F5, VI_KEY( F5 ), VI_KEY( SHIFT_F5 ), VI_KEY( CTRL_F5 ), VI_KEY( ALT_F5 ) },
    { VK_F6, VI_KEY( F6 ), VI_KEY( SHIFT_F6 ), VI_KEY( CTRL_F6 ), VI_KEY( ALT_F6 ) },
    { VK_F7, VI_KEY( F7 ), VI_KEY( SHIFT_F7 ), VI_KEY( CTRL_F7 ), VI_KEY( ALT_F7 ) },
    { VK_F8, VI_KEY( F8 ), VI_KEY( SHIFT_F8 ), VI_KEY( CTRL_F8 ), VI_KEY( ALT_F8 ) },
    { VK_F9, VI_KEY( F9 ), VI_KEY( SHIFT_F9 ), VI_KEY( CTRL_F9 ), VI_KEY( ALT_F9 ) },
    { VK_F10, VI_KEY( F10 ), VI_KEY( SHIFT_F10 ), VI_KEY( CTRL_F10 ), VI_KEY( ALT_F10 ) },
    { VK_F11, VI_KEY( F11 ), VI_KEY( SHIFT_F11 ), VI_KEY( CTRL_F11 ), VI_KEY( ALT_F11 ) },
    { VK_F12, VI_KEY( F12 ), VI_KEY( SHIFT_F12 ), VI_KEY( CTRL_F12 ), VI_KEY( ALT_F12 ) }
};

extern COORD    BSize;
extern int      PageCnt;
extern int      CurrMouseStatus;
extern int      CurrMouseCol;
extern int      CurrMouseRow;

void BIOSGetColorPalette( void far *a )
{
    a = a;
}

long BIOSGetColorRegister( short a )
{
    a = a;
    return( 0 );
}

void BIOSSetNoBlinkAttr( void )
{
}

void BIOSSetBlinkAttr( void )
{
}

void BIOSSetColorRegister( short reg, char r, char g, char b )
{
    reg = reg; r = r; g = g; b =b;
}

static COORD    _cpos = { 0, 0 };

static int CompareEvents( const void *p1, const void *p2 )
{
    return( ((const map *)p1)->vk - ((const map *)p2)->vk );
}

/*
 * BIOSGetCursor - set current cursor postion
 */
void BIOSSetCursor( char page, char row, char col )
{
    page = page;
    _cpos.X = col;
    _cpos.Y = row;
    SetConsoleCursorPosition( OutputHandle, _cpos );

} /* BIOSSetCursor */

/*
 * BIOSGetCursor - return current cursor postion
 */
short BIOSGetCursor( char page )
{
    short       res;

    page = page;
    res = (_cpos.Y << 8) + _cpos.X;
    return( res );

} /* BIOSGetCursor */

/*
 * eventWeWant - test an input record and see if it is one
 *               we want to handle
 */
static BOOL eventWeWant( INPUT_RECORD *ir )
{
    WORD            vk;
    DWORD           st;
    DWORD           ss;
    static short    alt_numpad_number = 0;

    if( ir->EventType == MOUSE_EVENT ) {
        CurrMouseCol = ir->Event.MouseEvent.dwMousePosition.X;
        CurrMouseRow = ir->Event.MouseEvent.dwMousePosition.Y;
        CurrMouseStatus = 0;
        st = ir->Event.MouseEvent.dwButtonState;
        if( st & (FROM_LEFT_2ND_BUTTON_PRESSED | FROM_LEFT_3RD_BUTTON_PRESSED |
                  FROM_LEFT_4TH_BUTTON_PRESSED | FROM_LEFT_1ST_BUTTON_PRESSED ) ) {
            CurrMouseStatus |= MOUSE_LEFT_BUTTON_DOWN;
        }
        if( st & RIGHTMOST_BUTTON_PRESSED ) {
            CurrMouseStatus |= MOUSE_RIGHT_BUTTON_DOWN;
        }
        return( TRUE );
    }
    if( ir->EventType != KEY_EVENT ) {
        return( FALSE );
    }

    if( alt_numpad_number >= 0 &&
        !ir->Event.KeyEvent.bKeyDown &&
        ir->Event.KeyEvent.wVirtualKeyCode == VK_MENU ) {
        ir->Event.KeyEvent.wVirtualKeyCode = 0;
        if( alt_numpad_number < 256 ) {
            ir->Event.KeyEvent.uChar.AsciiChar = alt_numpad_number;
        } else {
            ir->Event.KeyEvent.uChar.AsciiChar = 0;
        }
        alt_numpad_number = -1;
        return( TRUE );
    }

    if( !ir->Event.KeyEvent.bKeyDown ) {
        return( FALSE );
    }
    vk = ir->Event.KeyEvent.wVirtualKeyCode;
    if( vk == VK_CONTROL || vk == VK_SHIFT || vk == VK_MENU || vk == VK_CAPITAL ) {
        return( FALSE );
    }
    ss = ir->Event.KeyEvent.dwControlKeyState;
    if( (ss & (RIGHT_ALT_PRESSED  | LEFT_ALT_PRESSED)) && !(ss &(ENHANCED_KEY)) ) {
        if( alt_numpad_number == -1 ) {
            alt_numpad_number = 0;
        }
        alt_numpad_number *= 10;
        switch( ir->Event.KeyEvent.wVirtualScanCode ) {
        case 0x47: alt_numpad_number += 7; break;
        case 0x48: alt_numpad_number += 8; break;
        case 0x49: alt_numpad_number += 9; break;
        case 0x4b: alt_numpad_number += 4; break;
        case 0x4c: alt_numpad_number += 5; break;
        case 0x4d: alt_numpad_number += 6; break;
        case 0x4f: alt_numpad_number += 1; break;
        case 0x50: alt_numpad_number += 2; break;
        case 0x51: alt_numpad_number += 3; break;
        case 0x52: alt_numpad_number += 0; break;
        default:
            /* not an alt-numpad */
            alt_numpad_number = -1;
            return( TRUE );
        }
        return( FALSE );
    }
    return( TRUE );

} /* eventWeWant */

/*
 * BIOSKeyboardHit - read the keyboard
 */
vi_key BIOSGetKeyboard( int *scan )
{
    INPUT_RECORD        ir;
    DWORD               rd, ss;
    WORD                vk;
    BOOL                has_alt, has_shift, has_ctrl, has_capsl;
    map                 *ev, what;
    vi_key              key;

    do {
        ReadConsoleInput( InputHandle, &ir, 1, &rd );
    } while( !eventWeWant( &ir ) );
    if( ir.EventType == MOUSE_EVENT ) {
        return( VI_KEY( MOUSEEVENT ) );
    }
    vk = ir.Event.KeyEvent.wVirtualKeyCode;
    key = (unsigned char)ir.Event.KeyEvent.uChar.AsciiChar;
    ss = ir.Event.KeyEvent.dwControlKeyState;

    has_shift = ((ss & SHIFT_PRESSED) ? TRUE : FALSE);
    has_ctrl = ((ss & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)) ? TRUE : FALSE);
    has_alt = ((ss & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED)) ? TRUE : FALSE);
    has_capsl = ((ss & CAPSLOCK_ON) ? TRUE : FALSE);
    what.vk = vk;

    ev = bsearch( &what, events, sizeof( events ) / sizeof( events[0] ), sizeof( what ), CompareEvents );
    if( ev != NULL ) {
        if( has_ctrl && has_alt ) {
            // it handles AltGr + key
        } else if( has_ctrl ) {
            key = ev->ctrl;
        } else if( has_alt ) {
            key = ev->alt;
        // Caps lock has efect to keys which generate character
        // don't apply to extended keys as down, up, page down,
        // page up, insert, end, delete, ....
        } else if( has_shift ^ ( key && has_capsl ) ) {
            if( key == 0 ) {
                key = ev->shift;
            }
        } else {
            if( key == 0 ) {
                key = ev->reg;
            }
        }
    }
    if( key == 0 ) {    // ignore unknown keys
        key = VI_KEY( DUMMY );
    }
    if( scan != NULL ) {
        *scan = 0;
    }
    return( key );

} /* BIOSGetKeyboard */

/*
 * BIOSKeyboardHit - test if a key is waiting
 */
bool BIOSKeyboardHit( void )
{
    DWORD               rd;
    INPUT_RECORD        ir;
    bool                rc;

    for( ;; ) {
        PeekConsoleInput( InputHandle, &ir, 1, &rd );
        if( rd == 0 ) {
            rc = FALSE;
            break;
        }
        if( eventWeWant( &ir ) ) {
            rc = TRUE;
            break;
        }
        ReadConsoleInput( InputHandle, &ir, 1, &rd );
    }
    return( rc );

} /* BIOSKeyboardHit */

/*
 * BIOSUpdateScreen - update the screen
 */
void  BIOSUpdateScreen( unsigned offset, unsigned nchars )
{
    SMALL_RECT  sr;
    COORD       bcoord;
    unsigned    oend;

    if( PageCnt > 0 || nchars == 0 ) {
        return;
    }

    oend = offset + nchars - 1;

    bcoord.Y = sr.Top = offset / EditVars.WindMaxWidth;
    bcoord.X = sr.Left = offset - sr.Top * EditVars.WindMaxWidth;
    sr.Bottom = oend / EditVars.WindMaxWidth;
    sr.Right = oend - sr.Bottom * EditVars.WindMaxWidth;

    WriteConsoleOutput( OutputHandle, Scrn, BSize, bcoord, &sr );

} /* BIOSUpdateScreen */
