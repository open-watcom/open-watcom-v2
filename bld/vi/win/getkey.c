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


#include "winvi.h"
#include "keys.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define KEY_BUFFER_SIZE 64

typedef struct vi_key_scancode {
    int scan;
    int ch;
} vi_key_scancode;

static vi_key_scancode  keyBuffer[ KEY_BUFFER_SIZE ];
static volatile int     bufferTop = 0;
static volatile int     bufferBottom = 0;

typedef struct {
    int value;
    int regular;
    int shifted;
    int ctrl;
    int alt;
    int cs;
} keytable;

int find_key_in_table( const void *pkey, const void *pbase )
{
    int vk = *(WORD*)pkey;
    keytable *base = (keytable*)pbase;
    if( vk > base->value ) return( 1 );
    if( vk < base->value ) return( -1 );
    return( 0 );
}

/* should move all constants to keys.h
*/
#define VK_HYPHEN       0xbd

static const keytable keyTable[] = {

    /* new table */
    /* value;   regular; shifted; ctrl; alt; cs; */
    { VK_LBUTTON, -1,-1,-1,-1,-1 },
    { VK_RBUTTON, -1,-1,-1,-1,-1 },
    { VK_CANCEL, VI_KEY( CTRL_C ), VI_KEY( CTRL_C ), VI_KEY( CTRL_C ), VI_KEY( CTRL_C ), VI_KEY( CTRL_C ) },
    { VK_MBUTTON, -1,-1,-1,-1,-1 },
    { VK_BACK, VI_KEY( BS ), VI_KEY( BS ), VI_KEY( CTRL_BS ), VI_KEY( ALT_BS ), 0 },
    { VK_TAB, VI_KEY( TAB ), VI_KEY( SHIFT_TAB ), VI_KEY( CTRL_TAB ), VI_KEY( ALT_TAB ), 0 },
    { VK_CLEAR, -1,-1,-1,-1,-1 },
    { VK_RETURN, VI_KEY( ENTER ), VI_KEY( ENTER ), VI_KEY( ENTER ), VI_KEY( ENTER ), VI_KEY( ENTER ) },
    { VK_SHIFT, -1,-1,-1,-1,-1 },
    { VK_CONTROL, -1,-1,-1,-1,-1 },
    { VK_MENU, -1,-1,-1,-1,-1 },
    { VK_PAUSE, -1,-1,-1,-1,-1 },
    { VK_CAPITAL, -1,-1,-1,-1,-1 },
    { VK_ESCAPE, VI_KEY( ESC ), VI_KEY( ESC ), VI_KEY( ESC ), VI_KEY( ESC ), VI_KEY( ESC ) },
    { VK_SPACE, ' ', ' ', ' ', ' ', ' ' },
    { VK_PRIOR, VI_KEY( PAGEUP ), VI_KEY( SHIFT_PAGEUP ), VI_KEY( CTRL_PAGEUP ), VI_KEY( ALT_PAGEUP ), VI_KEY( CS_PAGEUP ) },
    { VK_NEXT, VI_KEY( PAGEDOWN ), VI_KEY( SHIFT_PAGEDOWN ), VI_KEY( CTRL_PAGEDOWN ), VI_KEY( ALT_PAGEDOWN ), VI_KEY( CS_PAGEDOWN ) },
    { VK_END, VI_KEY( END ), VI_KEY( SHIFT_END ), VI_KEY( CTRL_END ), VI_KEY( ALT_END ), VI_KEY( CS_END ) },
    { VK_HOME, VI_KEY( HOME ), VI_KEY( SHIFT_HOME ), VI_KEY( CTRL_HOME ), VI_KEY( ALT_HOME ), VI_KEY( CS_HOME ) },
    { VK_LEFT, VI_KEY( LEFT ), VI_KEY( SHIFT_LEFT ), VI_KEY( CTRL_LEFT ), VI_KEY( ALT_LEFT ), VI_KEY( CS_LEFT ) },
    { VK_UP, VI_KEY( UP ), VI_KEY( SHIFT_UP ), VI_KEY( CTRL_UP ), VI_KEY( ALT_UP ), VI_KEY( CS_UP ) },
    { VK_RIGHT, VI_KEY( RIGHT ), VI_KEY( SHIFT_RIGHT ), VI_KEY( CTRL_RIGHT ), VI_KEY( ALT_RIGHT ), VI_KEY( CS_RIGHT ) },
    { VK_DOWN, VI_KEY( DOWN ), VI_KEY( SHIFT_DOWN ), VI_KEY( CTRL_DOWN ), VI_KEY( ALT_DOWN ), VI_KEY( CS_DOWN ) },
    { VK_SELECT, -1,-1,-1,-1,-1 },
    { VK_PRINT, -1,-1,-1,-1,-1 },
    { VK_EXECUTE, -1,-1,-1,-1,-1 },
    { VK_SNAPSHOT, -1,-1,-1,-1,-1 },
    { VK_INSERT, VI_KEY( INS ), VI_KEY( SHIFT_INS ), VI_KEY( CTRL_INS ), VI_KEY( ALT_INS ), 0 },
    { VK_DELETE, VI_KEY( DEL ), VI_KEY( SHIFT_DEL ), VI_KEY( CTRL_DEL ), VI_KEY( ALT_DEL ), 0 },
    { VK_HELP, -1,-1,-1,-1,-1 },
    { VK_NUMPAD0, '0', '0', '0', '0', '0' },
    { VK_NUMPAD1, '1', '1', '1', '1', '1' },
    { VK_NUMPAD2, '2', '2', '2', '2', '2' },
    { VK_NUMPAD3, '3', '3', '3', '3', '3' },
    { VK_NUMPAD4, '4', '4', '4', '4', '4' },
    { VK_NUMPAD5, '5', '5', '5', '5', '5' },
    { VK_NUMPAD6, '6', '6', '6', '6', '6' },
    { VK_NUMPAD7, '7', '7', '7', '7', '7' },
    { VK_NUMPAD8, '8', '8', '8', '8', '8' },
    { VK_NUMPAD9, '9', '9', '9', '9', '9' },
    { VK_MULTIPLY,'*', '*', '*', '*', '*' },
    { VK_ADD    ,'+', '+', '+', '+', '+' },
    { VK_SEPARATOR, -1,-1,-1,-1,-1 },
    { VK_SUBTRACT,'-', '-', '-', '-', '-' },
    { VK_DECIMAL,'.', '.', '.', '.', '.' },
    { VK_DIVIDE,'/', '/', '/', '/', '/' },
    { VK_F1     , VI_KEY( F1 ), VI_KEY( SHIFT_F1 ), VI_KEY( CTRL_F1 ), 0, 0 },
    { VK_F2     , VI_KEY( F2 ), VI_KEY( SHIFT_F2 ), VI_KEY( CTRL_F2 ), 0, 0 },
    { VK_F3     , VI_KEY( F3 ), VI_KEY( SHIFT_F3 ), VI_KEY( CTRL_F3 ), 0, 0 },
    { VK_F4     , VI_KEY( F4 ), VI_KEY( SHIFT_F4 ), VI_KEY( CTRL_F4 ), 0, 0 },
    { VK_F5     , VI_KEY( F5 ), VI_KEY( SHIFT_F5 ), VI_KEY( CTRL_F5 ), 0, 0 },
    { VK_F6     , VI_KEY( F6 ), VI_KEY( SHIFT_F6 ), VI_KEY( CTRL_F6 ), 0, 0 },
    { VK_F7     , VI_KEY( F7 ), VI_KEY( SHIFT_F7 ), VI_KEY( CTRL_F7 ), 0, 0 },
    { VK_F8     , VI_KEY( F8 ), VI_KEY( SHIFT_F8 ), VI_KEY( CTRL_F8 ), 0, 0 },
    { VK_F9     , VI_KEY( F9 ), VI_KEY( SHIFT_F9 ), VI_KEY( CTRL_F9 ), 0, 0 },
    { VK_F10    , VI_KEY( F10 ), VI_KEY( SHIFT_F10 ), VI_KEY( CTRL_F10 ), 0, 0 },
    { VK_F11    ,-1,-1,-1,-1,-1},
    { VK_F12    ,-1,-1,-1,-1,-1},
    { VK_F13    ,-1,-1,-1,-1,-1},
    { VK_F14    ,-1,-1,-1,-1,-1},
    { VK_F15    ,-1,-1,-1,-1,-1},
    { VK_F16    ,-1,-1,-1,-1,-1},
    { VK_F17    ,-1,-1,-1,-1,-1},
    { VK_F18    ,-1,-1,-1,-1,-1},
    { VK_F19    ,-1,-1,-1,-1,-1},
    { VK_F20    ,-1,-1,-1,-1,-1},
    { VK_F21    ,-1,-1,-1,-1,-1},
    { VK_F22    ,-1,-1,-1,-1,-1},
    { VK_F23    ,-1,-1,-1,-1,-1},
    { VK_F24    ,-1,-1,-1,-1,-1},
    { VK_NUMLOCK,-1,-1,-1,-1,-1},
    { VK_SCROLL, -1,-1,-1,-1,-1},
    /* new table */
};
#define KEYTABLE_LENGTH  ( sizeof( keyTable ) / sizeof( keytable ) )
#define KEYTABLE_WIDTH   ( sizeof( keytable ) )

bool ShiftDown( void )
{
    return( ( GetKeyState( VK_SHIFT ) & ~0x01 ) != 0 );
}

bool CapsDown( void ){
    return( ( GetKeyState( VK_CAPITAL ) & 0x01 ) != 0  );
}

bool CtrlDown( void )
{
    return( ( GetKeyState( VK_CONTROL ) & ~0x01 ) != 0 );
}

/*
 * We can do Alt keys I believe but not sure we want to
 * interfere with Windows accelerators (ie I don't want to
 * go add the cases everywhere below).
 */
bool AltDown( void )
{
    return( ( GetKeyState( VK_MENU ) & ~0x01 ) != 0 );
}

int ConvertWierdCharacter( WORD vk, WORD data )
{
    char keyboard_state[256];
    unsigned int scancode = LOBYTE( data );
    #if defined(__NT__)
        WORD    newkey;
    #else
        DWORD   newkey;
    #endif

    GetKeyboardState( keyboard_state );
    ToAscii( vk, scancode, keyboard_state, &newkey, FALSE );

    return( (char)newkey );
}

int MapVirtualKeyToVIKey( WORD vk, WORD data )
{
    int     t, ch;
    bool        altdown, shiftdown, ctrldown, capsdown;
    keytable *key;

    shiftdown = ( GetKeyState( VK_SHIFT ) & ~0x01 ) != 0;
    capsdown  = ( GetKeyState( VK_CAPITAL ) & 0x01 );
    ctrldown  = ( GetKeyState( VK_CONTROL ) & ~0x01 ) != 0;
    altdown   = ( GetKeyState( VK_MENU ) & ~0x01 ) != 0;

    // don't touch anything when alt down because we'll screw up
    // menu accelerators etc.
    // (exceptions to exception: ALT_HYPHEN, ALT_BS)
    if( altdown && !ctrldown ) {
        if( vk != VK_BACK && vk != VK_HYPHEN ) {
            return( -1 );
        }
    }

    if( vk >= 'A' && vk <= 'Z' ) {
        t = vk - 'A';
        if( ctrldown && altdown ) {
            ch = ConvertWierdCharacter( vk, data );
        } else if( ctrldown ) {
            ch = VI_KEY( CTRL_A ) + t;
        } else if( shiftdown && capsdown ) {
            ch = 'a' + t;
        } else if( shiftdown || capsdown ) {
            ch = 'A' + t;
        } else {
            ch = 'a' + t;
        }
    } else {
        key = (keytable *)bsearch( (const void *)(&vk), keyTable,
                 KEYTABLE_LENGTH, KEYTABLE_WIDTH, find_key_in_table );
        if( key != NULL && vk == key->value ) {
            if( ctrldown && shiftdown ) {
                ch = key->cs;
            } else if( ctrldown ) {
                ch = key->ctrl;
            } else if( shiftdown ) {
                ch = key->shifted;
            } else if( altdown ) {
                ch = key->alt;
            } else {
                ch = key->regular;
            }
            return( ch );
        }

        /* we didn't find it in any place we looked */
        ch = ConvertWierdCharacter( vk, data );
    }
    return( ch );
}

/*
 * WindowsKeyPush - handle the press of a key
*/
bool WindowsKeyPush( WORD vk, WORD data )
{
    int         ch;

    // if we're holding everything just eat the key quietly
    if( EditFlags.HoldEverything ) {
        return( TRUE );
    }
    ch = MapVirtualKeyToVIKey( vk, data );
    if( ch != -1 ) {
        keyBuffer[ bufferTop ].ch = ch;
        keyBuffer[ bufferTop ].scan = LOBYTE( data );
        bufferTop = ( bufferTop + 1 ) % KEY_BUFFER_SIZE;
        return( TRUE );
    }
    return( FALSE );

} /* WindowsKeyPush */

/*
 * KeyboardHit - test for a waiting key
*/
bool KeyboardHit( void )
{
    if( bufferTop != bufferBottom ) {
        return( TRUE );
    }
    MessageLoop( TRUE );
    if( bufferTop != bufferBottom ) {
        return( TRUE );
    }
    return( FALSE );

} /* KeyboardHit */

/*
 * GetKeyboard - get a keyboard result
*/
int GetKeyboard( int *scan )
{
    int     ch;

    ch = keyBuffer[ bufferBottom ].ch;
    if( scan ) {
        *scan = keyBuffer[ bufferBottom ].scan;
    }
    bufferBottom = ( bufferBottom + 1 ) % KEY_BUFFER_SIZE;
    return( ch );

} /* GetKeyboard */

int GetMouseEvent( void )
{
    return( FALSE );
}

bool TestMouseEvent( bool mouse )
{
    mouse = mouse;
    return( FALSE );
}
