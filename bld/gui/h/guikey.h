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


#ifndef _GUIKEY_H_
#define _GUIKEY_H_

#define GUI_SCAN_OFFSET         256
#define GUI_CTRL_ASCII_OFFSET   512
#define GUI_ASCII( name, ascii ) GUI_KEY_##name = ascii
#define GUI_SCAN( name, scan ) GUI_KEY_##name = GUI_SCAN_OFFSET + scan
#define GUI_CTRL_ASCII( name, ascii ) GUI_KEY_##name = GUI_CTRL_ASCII_OFFSET + ascii
typedef enum gui_key {
    GUI_ASCII( CTRL_KP_ENTER,           0x0a ),
    GUI_ASCII( KP_ENTER,                0x0d ),
    GUI_ASCII( KP_ASTERISK,             0x2a ),
    GUI_ASCII( KP_PLUS,                 0x2b ),
    GUI_ASCII( KP_MINUS,                0x2d ),
    GUI_ASCII( KP_PERIOD,               0x2e ),
    GUI_ASCII( KP_SLASH,                0x2f ),
    GUI_ASCII( CTRL_A,                  0x01 ),
    GUI_ASCII( CTRL_B,                  0x02 ),
    GUI_ASCII( CTRL_C,                  0x03 ),
    GUI_ASCII( CTRL_D,                  0x04 ),
    GUI_ASCII( CTRL_E,                  0x05 ),
    GUI_ASCII( CTRL_F,                  0x06 ),
    GUI_ASCII( CTRL_G,                  0x07 ),
    GUI_CTRL_ASCII( CTRL_H,             0x08 ),
    GUI_CTRL_ASCII( CTRL_I,             0x09 ),
    GUI_ASCII( TAB,                     0x09 ),
    GUI_ASCII( CTRL_J,                  0x0a ),
    GUI_ASCII( CTRL_K,                  0x0b ),
    GUI_ASCII( CTRL_L,                  0x0c ),
    GUI_CTRL_ASCII( CTRL_M,             0x0d ),
    GUI_ASCII( CTRL_N,                  0x0e ),
    GUI_ASCII( CTRL_O,                  0x0f ),
    GUI_ASCII( CTRL_P,                  0x10 ),
    GUI_ASCII( CTRL_Q,                  0x11 ),
    GUI_ASCII( CTRL_R,                  0x12 ),
    GUI_ASCII( CTRL_S,                  0x13 ),
    GUI_ASCII( CTRL_T,                  0x14 ),
    GUI_ASCII( CTRL_U,                  0x15 ),
    GUI_ASCII( CTRL_V,                  0x16 ),
    GUI_ASCII( CTRL_W,                  0x17 ),
    GUI_ASCII( CTRL_X,                  0x18 ),
    GUI_ASCII( CTRL_Y,                  0x19 ),
    GUI_ASCII( CTRL_Z,                  0x1a ),
    GUI_ASCII( CTRL_BACKSLASH,          0x1c ),
    GUI_ASCII( CTRL_RIGHT_BRACKET,      0x1d ),
    GUI_CTRL_ASCII( CTRL_LEFT_BRACKET,  0x1b ),
    GUI_ASCII( CTRL_6,                  0x1e ),
    GUI_ASCII( CTRL_MINUS,              0x1f ),
    GUI_ASCII( CTRL_BACKSPACE,          0x7f ),
    GUI_ASCII( CTRL_ENTER,              0x0a ),
    GUI_ASCII( BACKQUOTE,               '`' ),
    GUI_ASCII( 1,                       '1' ),
    GUI_ASCII( 2,                       '2' ),
    GUI_ASCII( 3,                       '3' ),
    GUI_ASCII( 4,                       '4' ),
    GUI_ASCII( 5,                       '5' ),
    GUI_ASCII( 6,                       '6' ),
    GUI_ASCII( 7,                       '7' ),
    GUI_ASCII( 8,                       '8' ),
    GUI_ASCII( 9,                       '9' ),
    GUI_ASCII( 0,                       '0' ),
    GUI_ASCII( MINUS,                   '-' ),
    GUI_ASCII( EQUAL,                   '=' ),
    GUI_ASCII( q,                       'q' ),
    GUI_ASCII( w,                       'w' ),
    GUI_ASCII( e,                       'e' ),
    GUI_ASCII( r,                       'r' ),
    GUI_ASCII( t,                       't' ),
    GUI_ASCII( y,                       'y' ),
    GUI_ASCII( u,                       'u' ),
    GUI_ASCII( i,                       'i' ),
    GUI_ASCII( o,                       'o' ),
    GUI_ASCII( p,                       'p' ),
    GUI_ASCII( LEFT_BRACKET,            '[' ),
    GUI_ASCII( RIGHT_BRACKET,           ']' ),
    GUI_ASCII( BACKSLASH,               '\\' ),
    GUI_ASCII( a,                       'a' ),
    GUI_ASCII( s,                       's' ),
    GUI_ASCII( d,                       'd' ),
    GUI_ASCII( f,                       'f' ),
    GUI_ASCII( g,                       'g' ),
    GUI_ASCII( h,                       'h' ),
    GUI_ASCII( j,                       'j' ),
    GUI_ASCII( k,                       'k' ),
    GUI_ASCII( l,                       'l' ),
    GUI_ASCII( SEMICOLON,               ';' ),
    GUI_ASCII( QUOTE,                   '\'' ),
    GUI_ASCII( z,                       'z' ),
    GUI_ASCII( x,                       'x' ),
    GUI_ASCII( c,                       'c' ),
    GUI_ASCII( v,                       'v' ),
    GUI_ASCII( b,                       'b' ),
    GUI_ASCII( n,                       'n' ),
    GUI_ASCII( m,                       'm' ),
    GUI_ASCII( COMMA,                   ',' ),
    GUI_ASCII( PERIOD,                  '.' ),
    GUI_ASCII( SLASH,                   '/' ),
    GUI_ASCII( TILDE,                   '~' ),
    GUI_ASCII( EXCLAMATION,             '!' ),
    GUI_ASCII( AT,                      '@' ),
    GUI_ASCII( OCTATHORPE,              '#' ),
    GUI_ASCII( DOLLAR,                  '$' ),
    GUI_ASCII( PERCENT,                 '%' ),
    GUI_ASCII( CARET,                   '^' ),
    GUI_ASCII( AMPERSAND,               '&' ),
    GUI_ASCII( ASTERISK,                '*' ),
    GUI_ASCII( LEFT_PAREN,              '(' ),
    GUI_ASCII( RIGHT_PAREN,             ')' ),
    GUI_ASCII( UNDERSCORE,              '_' ),
    GUI_ASCII( PLUS,                    '+' ),
    GUI_ASCII( Q,                       'Q' ),
    GUI_ASCII( W,                       'W' ),
    GUI_ASCII( E,                       'E' ),
    GUI_ASCII( R,                       'R' ),
    GUI_ASCII( T,                       'T' ),
    GUI_ASCII( Y,                       'Y' ),
    GUI_ASCII( U,                       'U' ),
    GUI_ASCII( I,                       'I' ),
    GUI_ASCII( O,                       'O' ),
    GUI_ASCII( P,                       'P' ),
    GUI_ASCII( LEFT_BRACE,              '{' ),
    GUI_ASCII( RIGHT_BRACE,             '}' ),
    GUI_ASCII( BAR,                     '|' ),
    GUI_ASCII( A,                       'A' ),
    GUI_ASCII( S,                       'S' ),
    GUI_ASCII( D,                       'D' ),
    GUI_ASCII( F,                       'F' ),
    GUI_ASCII( G,                       'G' ),
    GUI_ASCII( H,                       'H' ),
    GUI_ASCII( J,                       'J' ),
    GUI_ASCII( K,                       'K' ),
    GUI_ASCII( L,                       'L' ),
    GUI_ASCII( COLON,                   ':' ),
    GUI_ASCII( DOUBLEQUOTE,             '"' ),
    GUI_ASCII( Z,                       'Z' ),
    GUI_ASCII( X,                       'X' ),
    GUI_ASCII( C,                       'C' ),
    GUI_ASCII( V,                       'V' ),
    GUI_ASCII( B,                       'B' ),
    GUI_ASCII( N,                       'N' ),
    GUI_ASCII( M,                       'M' ),
    GUI_ASCII( LESS,                    '<' ),
    GUI_ASCII( GREATER,                 '>' ),
    GUI_ASCII( QUESTIONMARK,            '?' ),
    GUI_ASCII( BACKSPACE,               0x08 ),
    GUI_ASCII( SPACE,                   ' ' ),
    GUI_ASCII( ENTER,                   0x0d ),
    GUI_ASCII( ESCAPE,                  0x1b ),
    GUI_ASCII( KP_0,                    '0' ),
    GUI_ASCII( KP_1,                    '1' ),
    GUI_ASCII( KP_2,                    '2' ),
    GUI_ASCII( KP_3,                    '3' ),
    GUI_ASCII( KP_4,                    '4' ),
    GUI_ASCII( KP_5,                    '5' ),
    GUI_ASCII( KP_6,                    '6' ),
    GUI_ASCII( KP_7,                    '7' ),
    GUI_ASCII( KP_8,                    '8' ),
    GUI_ASCII( KP_9,                    '9' ),
    GUI_SCAN( HOME,                     0x47 ),
    GUI_SCAN( END,                      0x4f ),
    GUI_SCAN( UP,                       0x48 ),
    GUI_SCAN( DOWN,                     0x50 ),
    GUI_SCAN( PAGEUP,                   0x49 ),
    GUI_SCAN( PAGEDOWN,                 0x51 ),
    GUI_SCAN( LEFT,                     0x4b ),
    GUI_SCAN( RIGHT,                    0x4d ),
    GUI_SCAN( BLANK,                    0x4c ),
    GUI_SCAN( INSERT,                   0x52 ),
    GUI_SCAN( DELETE,                   0x53 ),
    GUI_SCAN( CTRL_HOME,                0x77 ),
    GUI_SCAN( CTRL_END,                 0x75 ),
    GUI_SCAN( CTRL_UP,                  0x8d ),
    GUI_SCAN( CTRL_DOWN,                0x91 ),
    GUI_SCAN( CTRL_PAGEUP,              0x84 ),
    GUI_SCAN( CTRL_PAGEDOWN,            0x76 ),
    GUI_SCAN( CTRL_LEFT,                0x73 ),
    GUI_SCAN( CTRL_RIGHT,               0x74 ),
    GUI_SCAN( CTRL_BLANK,               0x8f ),
    GUI_SCAN( CTRL_INSERT,              0x92 ),
    GUI_SCAN( CTRL_DELETE,              0x93 ),
    GUI_SCAN( CTRL_KP_PLUS,             0x90 ),
    GUI_SCAN( CTRL_KP_MINUS,            0x8e ),
    GUI_SCAN( CTRL_KP_ASTERISK,         0x96 ),
    GUI_SCAN( CTRL_KP_SLASH,            0x95 ),
    GUI_SCAN( ALT_KP_PLUS,              0x4e ),
    GUI_SCAN( ALT_KP_MINUS,             0x4a ),
    GUI_SCAN( ALT_KP_ASTERISK,          0x37 ),
    GUI_SCAN( ALT_KP_SLASH,             0xa4 ),
    GUI_SCAN( ALT_KP_ENTER,             0xa6 ),
    GUI_SCAN( ALT_EXT_INSERT,           0xa2 ),
    GUI_SCAN( ALT_EXT_DELETE,           0xa3 ),
    GUI_SCAN( ALT_EXT_HOME,             0x97 ),
    GUI_SCAN( ALT_EXT_END,              0x9f ),
    GUI_SCAN( ALT_EXT_PAGEUP,           0x99 ),
    GUI_SCAN( ALT_EXT_PAGEDOWN,         0xa1 ),
    GUI_SCAN( ALT_EXT_LEFT,             0x9b ),
    GUI_SCAN( ALT_EXT_RIGHT,            0x9d ),
    GUI_SCAN( ALT_EXT_UP,               0x98 ),
    GUI_SCAN( ALT_EXT_DOWN,             0xa0 ),
    GUI_SCAN( F1,                       0x3b ),
    GUI_SCAN( F2,                       0x3c ),
    GUI_SCAN( F3,                       0x3d ),
    GUI_SCAN( F4,                       0x3e ),
    GUI_SCAN( F5,                       0x3f ),
    GUI_SCAN( F6,                       0x40 ),
    GUI_SCAN( F7,                       0x41 ),
    GUI_SCAN( F8,                       0x42 ),
    GUI_SCAN( F9,                       0x43 ),
    GUI_SCAN( F10,                      0x44 ),
    GUI_SCAN( F11,                      0x85 ),
    GUI_SCAN( F12,                      0x86 ),
    GUI_SCAN( CTRL_F1,                  0x5e ),
    GUI_SCAN( CTRL_F2,                  0x5f ),
    GUI_SCAN( CTRL_F3,                  0x60 ),
    GUI_SCAN( CTRL_F4,                  0x61 ),
    GUI_SCAN( CTRL_F5,                  0x62 ),
    GUI_SCAN( CTRL_F6,                  0x63 ),
    GUI_SCAN( CTRL_F7,                  0x64 ),
    GUI_SCAN( CTRL_F8,                  0x65 ),
    GUI_SCAN( CTRL_F9,                  0x66 ),
    GUI_SCAN( CTRL_F10,                 0x67 ),
    GUI_SCAN( CTRL_F11,                 0x89 ),
    GUI_SCAN( CTRL_F12,                 0x8a ),
    GUI_SCAN( ALT_F1,                   0x68 ),
    GUI_SCAN( ALT_F2,                   0x69 ),
    GUI_SCAN( ALT_F3,                   0x6a ),
    GUI_SCAN( ALT_F4,                   0x6b ),
    GUI_SCAN( ALT_F5,                   0x6c ),
    GUI_SCAN( ALT_F6,                   0x6d ),
    GUI_SCAN( ALT_F7,                   0x6e ),
    GUI_SCAN( ALT_F8,                   0x6f ),
    GUI_SCAN( ALT_F9,                   0x70 ),
    GUI_SCAN( ALT_F10,                  0x71 ),
    GUI_SCAN( ALT_F11,                  0x8b ),
    GUI_SCAN( ALT_F12,                  0x8c ),
    GUI_SCAN( SHIFT_F1,                 0x54 ),
    GUI_SCAN( SHIFT_F2,                 0x55 ),
    GUI_SCAN( SHIFT_F3,                 0x56 ),
    GUI_SCAN( SHIFT_F4,                 0x57 ),
    GUI_SCAN( SHIFT_F5,                 0x58 ),
    GUI_SCAN( SHIFT_F6,                 0x59 ),
    GUI_SCAN( SHIFT_F7,                 0x5a ),
    GUI_SCAN( SHIFT_F8,                 0x5b ),
    GUI_SCAN( SHIFT_F9,                 0x5c ),
    GUI_SCAN( SHIFT_F10,                0x5d ),
    GUI_SCAN( SHIFT_F11,                0x87 ),
    GUI_SCAN( SHIFT_F12,                0x88 ),
    GUI_SCAN( SHIFT_TAB,                0x0f ),
    GUI_SCAN( ALT_BACKQUOTE,            0x29 ),
    GUI_SCAN( ALT_1,                    0x78 ),
    GUI_SCAN( ALT_2,                    0x79 ),
    GUI_SCAN( ALT_3,                    0x7a ),
    GUI_SCAN( ALT_4,                    0x7b ),
    GUI_SCAN( ALT_5,                    0x7c ),
    GUI_SCAN( ALT_6,                    0x7d ),
    GUI_SCAN( ALT_7,                    0x7e ),
    GUI_SCAN( ALT_8,                    0x7f ),
    GUI_SCAN( ALT_9,                    0x80 ),
    GUI_SCAN( ALT_0,                    0x81 ),
    GUI_SCAN( ALT_MINUS,                0x82 ),
    GUI_SCAN( ALT_EQUAL,                0x83 ),
    GUI_SCAN( ALT_Q,                    0x10 ),
    GUI_SCAN( ALT_W,                    0x11 ),
    GUI_SCAN( ALT_E,                    0x12 ),
    GUI_SCAN( ALT_R,                    0x13 ),
    GUI_SCAN( ALT_T,                    0x14 ),
    GUI_SCAN( ALT_Y,                    0x15 ),
    GUI_SCAN( ALT_U,                    0x16 ),
    GUI_SCAN( ALT_I,                    0x17 ),
    GUI_SCAN( ALT_O,                    0x18 ),
    GUI_SCAN( ALT_P,                    0x19 ),
    GUI_SCAN( ALT_LEFT_BRACKET,         0x1a ),
    GUI_SCAN( ALT_RIGHT_BRACKET,        0x1b ),
    GUI_SCAN( ALT_BACKSLASH,            0x2b ),
    GUI_SCAN( ALT_A,                    0x1e ),
    GUI_SCAN( ALT_S,                    0x1f ),
    GUI_SCAN( ALT_D,                    0x20 ),
    GUI_SCAN( ALT_F,                    0x21 ),
    GUI_SCAN( ALT_G,                    0x22 ),
    GUI_SCAN( ALT_H,                    0x23 ),
    GUI_SCAN( ALT_J,                    0x24 ),
    GUI_SCAN( ALT_K,                    0x25 ),
    GUI_SCAN( ALT_L,                    0x26 ),
    GUI_SCAN( ALT_SEMICOLON,            0x27 ),
    GUI_SCAN( ALT_QUOTE,                0x28 ),
    GUI_SCAN( ALT_Z,                    0x2c ),
    GUI_SCAN( ALT_X,                    0x2d ),
    GUI_SCAN( ALT_C,                    0x2e ),
    GUI_SCAN( ALT_V,                    0x2f ),
    GUI_SCAN( ALT_B,                    0x30 ),
    GUI_SCAN( ALT_N,                    0x31 ),
    GUI_SCAN( ALT_M,                    0x32 ),
    GUI_SCAN( ALT_COMMA,                0x33 ),
    GUI_SCAN( ALT_PERIOD,               0x34 ),
    GUI_SCAN( ALT_SLASH,                0x35 ),
    GUI_SCAN( ALT_BACKSPACE,            0x0e ),
    GUI_SCAN( ALT_ENTER,                0x1c ),
    GUI_SCAN( ALT_TAB,                  0xa5 ),
    GUI_SCAN( ALT_ESCAPE,               0x01 ),
    GUI_SCAN( CTRL_2,                   0x03 ),
    GUI_SCAN( CTRL_8,                   0x09 ),
    GUI_SCAN( CTRL_TAB,                 0x94 )
} gui_key;
#undef GUI_ASCII
#undef GUI_SCAN

typedef struct {
    gui_key             key;
    gui_keystate        state;
} gui_key_state;

typedef struct {
    gui_ctl_id          id;
    gui_key_state       key_state;
} gui_key_control;

/*
 **************************************************************
 * GUI_KEYUP :
 * GUI_KEYDOWN :
 *      1 parameter  - gui_key               : GUI_GET_KEY
 *      2 parameters - gui_key, gui_keystate : GUI_GET_KEY_STATE
 **************************************************************
 * GUI_KEY_CONTROL :
 * 2 parameters - unsigned, gui_key                : GUI_GET_KEY_CONTROL
 * 3 parameters - unsigned, gui_key, gui_key_state : GUI_GET_KEY_STATE_CONTROL
 **************************************************************
 */

#define GUI_GET_KEY( param, inkey ) ( inkey = (( gui_key_state *)param)->key )

#define GUI_GET_KEY_STATE( param, inkey, instate ) {            \
                inkey = (( gui_key_state *)param)->key;         \
                instate = (( gui_key_state *)param)->state;     \
                                                }               \

#define GUI_GET_KEY_CONTROL( param, inid, inkey )       {               \
        inid = ((gui_key_control *)param)->id;                  \
        inkey = (( gui_key_control *)param)->key_state.key;     \
                                                }               \

#define GUI_GET_KEY_STATE_CONTROL( param, inid, inkey, instate ){       \
        inid = ((gui_key_control *)param)->id;                  \
        inkey = (( gui_key_control *)param)->key_state.key;     \
        instate = (( gui_key_control *)param)->key_state.state; \
                                        }                       \

#endif // _GUIKEY_H_
