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


#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <dos.h>
#include "uidos.h"
#include "uidef.h"
#include "uishift.h"
#include "windows.h"

#define SCAN( n )       ( ( (n) - 0x100 ) << 8 )

// #define _DEBUG_KEYBOARD
#ifdef _DEBUG_KEYBOARD
#include "biosui.h"
extern WORD far * WindowsKernelPointerTo_A000h;
extern WORD far * WindowsKernelPointerTo_A200h;
#endif

// I love direct writes to screen memory.
#define DO_THAT_FUNKY_KEYBOARD_DEBUGGING_THING \
    { LP_ZEN_CODE codedst; LP_ZEN_ATTR attrdst; char buf[ 80 ]; int i; \
        codedst = CODE_LOOKUP( 5, 0 ); attrdst = ATTR_LOOKUP( 5, 0 ); \
        sprintf( buf, "event=%x;info=%x\0", event, info ); \
        for( i = 0; i < strlen( buf ); i++ ) { \
            codedst[ i ].left = buf[ i ]; codedst[i].right = 0; \
            attrdst[i].left = 0xe1; \
        } \
    };

/*
 * Take particular notice:
 * This code corresponds to the Japanese-character lock on the lower left
 * corner of the NEC keyboard.  If not checked, it generates code 0x15, which
 * is equal to our internal code for CTRL-U.  So I'll map it to the
 * shiftstatus vector if anyone out there ever wants to trap for it.  The
 * thing is, we might one day allow Japanese users to enter Japanese letters
 * into the debugger, which is currently impossible. CSK.
 * There's a problem - for whatever reason, windows only detects the lock
 * coming on, not going off. It's as if it can only catch turning caps lock
 * on, but not off.  So I can't map it to the shiftstatus vector, since
 * once turned on we'll never be able to turn it off.  I have one solution,
 * which I will implement; Turning it on will toggle the bit.  So once on,
 * to turn it off, you must double-click the Kanji-lock button.  Hmmm.
 */
#define VK_KANJILOCK 0x15

// Bleah.
static EVENT alt_char_events[] = {
    EV_ALT_A, EV_ALT_B, EV_ALT_C, EV_ALT_D, EV_ALT_E, EV_ALT_F, EV_ALT_G,
    EV_ALT_H, EV_ALT_I, EV_ALT_J, EV_ALT_K, EV_ALT_L, EV_ALT_M, EV_ALT_N,
    EV_ALT_O, EV_ALT_P, EV_ALT_Q, EV_ALT_R, EV_ALT_S, EV_ALT_T, EV_ALT_U,
    EV_ALT_V, EV_ALT_W, EV_ALT_X, EV_ALT_Y, EV_ALT_Z };

extern unsigned far _child;   /* fake out CG */

typedef struct {
char value;
char regular;
char shifted;
} keytable;

/*
 * KT represents the keys pressed which are leftovers from all the other keys.
 */

static keytable kt[] = {
{ 0xba,         ':',    '*' },
{ 0xbb,         ';',    '+' },
{ 0xbc,         ',',    '<' },
{ 0xbd,         '-',    '=' },
{ 0xbe,         '.',    '>' },
{ 0xbf,         '/',    '?' },

{ 0xc0,         '@',    '~' },

{ 0xdb,         '[',    '{' },
{ 0xdc,         '\\',   '|' },
{ 0xdd,         ']',    '}' },
{ 0xde,         '^',    '`' }
};

static char arith_table[] = {
    '*', '+', ',', '-', '.', '/' };

static unsigned int    ShiftState;
static unsigned         LastKey;
static volatile bool    HaveKey;

#pragma aux set_carry = 0xf9;
extern void set_carry(void);

#pragma aux push_ds = 0x1e;
extern void push_ds(void);

#pragma aux mov_ds_bx = 0x8e 0xdb parm [bx];
extern void mov_ds_bx( unsigned );

#pragma aux pop_ds = 0x1f;
extern void pop_ds(void);

extern void WindowsMouseEvent( unsigned, unsigned );

#pragma aux HookRtn far parm [ax] [cx] modify exact [];

static int CheckState( unsigned info, unsigned down )
{
    unsigned bit;

    switch( info & 0xff ) {
    case VK_KANJILOCK:
        bit = S_KANJI_LOCK;
        break;
    case VK_INSERT:
        if( down ) {
            LastKey = (82 << 8); /* kludge - as if insert pressed */
            HaveKey = TRUE;
        }
        bit = S_INSERT;
        break;
    case VK_CAPITAL:    /* caps lock */
        bit = S_CAPS;
        break;
    case VK_NUMLOCK:
        break;
    case 0x91:          /* scroll lock key */
        break;
    case VK_MENU:       /* alt key */
        bit = S_ALT;
        break;
    case VK_CONTROL:
        bit = S_CTRL;
        break;
    case VK_SHIFT:
        bit = S_SHIFT;
        break;
    default:
        return( FALSE );
    }
    if( bit > S_ALT ) { /* it's a toggle bit */
        if( down ) ShiftState ^= bit;
    } else if( down ) {
        ShiftState |= bit;
    } else {
        ShiftState &= ~bit;
    }
    return( TRUE );
}

#define SUB_CREATE_EVENT( x ) { \
                if( ShiftState & S_ALT ) { \
                    info = SCAN( EV_ALT_##x ); \
                } else if( ShiftState & S_CTRL ) { \
                    info = SCAN( EV_CTRL_##x ); \
                } else if( ShiftState & S_SHIFT ) { \
                    info = SCAN( EV_SHIFT_##x ); \
                } else { \
                    info = SCAN( EV_##x ); \
                } \
        }

#define SUB_CREATE_NO_SHIFT( x ) { \
                if( ShiftState & S_ALT ) { \
                    info = SCAN( EV_ALT_##x ); \
                } else if( ShiftState & S_CTRL ) { \
                    info = SCAN( EV_CTRL_##x ); \
                } else { \
                    info = SCAN( EV_##x ); \
                } \
        }

#define CREATE_EVENT( x, y ) \
                if( key == VK_##x ) SUB_CREATE_EVENT( y )

#define CONT_EVENT_CHAIN( x, y ) \
                else if( key == VK_##x ) SUB_CREATE_EVENT( y )

#define CONT_NO_SHIFT( x, y ) \
                else if( key == VK_##x ) SUB_CREATE_NO_SHIFT( y )

#include "biosui.h"
extern WORD far * WindowsKernelPointerTo_A000h;
extern WORD far * WindowsKernelPointerTo_A200h;

static void MyHookRtn( unsigned event, unsigned info )
{
    unsigned    key;
    int         i;
    switch( event ) {
    case WM_SYSKEYDOWN :
    case WM_KEYDOWN:
        #ifdef _DEBUG_KEYBOARD
        DO_THAT_FUNKY_KEYBOARD_DEBUGGING_THING;
        #endif
        if( !CheckState( info, 1 ) ) {
            key = info & 0xff;
            // Is the Key a function key?
            if( key >= VK_F1 && key <= VK_F10 ) {
                key = key - VK_F1 + 1;
                SUB_CREATE_EVENT( FUNC( key ) );
                key = 0;
            } else if( key == VK_F11 ) {
                SUB_CREATE_EVENT( FUNC_11 );
                key = 0;
            } else if( key == VK_F12 ) {
                SUB_CREATE_EVENT( FUNC_12 );
                key = 0;
            } else if( key >= 'A' && key <= 'Z' ) {
                if( ShiftState & S_ALT ) {
                    info = SCAN( alt_char_events[ key - 'A' ] );
                    key = 0;
                } else if( ShiftState & S_CTRL ) {
                    // obviously returning control characters.
                    key -= 'A'-1;
                } else if( !(ShiftState & S_SHIFT) ) {
                    if( !(ShiftState & S_CAPS) ) key += 'a' - 'A';
                } else if( ShiftState & S_CAPS ) {
                    key += 'a' - 'A';
                }
            } else if( key >= '0' && key <= '9' ) {
                if( ShiftState & S_ALT ) {
                    // Ignoring these for now.
                    key = 0;
                } else if( ShiftState & S_SHIFT ) {
                    key = " !\"#$%&'()"[key-'0'];
                }
            } else if( key == VK_ESCAPE ) {
                info = SCAN( EV_ESCAPE );
                key = 0;
            } else if( key >= VK_PRIOR && key <= VK_HELP ) {
                CREATE_EVENT(           PRIOR,          PAGE_UP )
                CONT_EVENT_CHAIN(       NEXT,           PAGE_DOWN )
                CONT_EVENT_CHAIN(       END,            END )
                CONT_EVENT_CHAIN(       HOME,           HOME )
                CONT_EVENT_CHAIN(       LEFT,           CURSOR_LEFT )
                CONT_EVENT_CHAIN(       UP,             CURSOR_UP )
                CONT_EVENT_CHAIN(       RIGHT,          CURSOR_RIGHT )
                CONT_EVENT_CHAIN(       DOWN,           CURSOR_DOWN )
                CONT_NO_SHIFT(          DELETE,         DELETE )
                CONT_EVENT_CHAIN(       HELP,           END );
                // Delete put in for NEC.
                // Help takes the place of the END key.  Leaving END in also
                // just in case.
                // select, execute, snapshot ignored.
                // insert handled elsewhere
                key = 0;
            } else if( (key >= VK_NUMPAD0) && (key <= VK_NUMPAD9) ) {
                key = key - VK_NUMPAD0 + '0';
            } else if( (key >= VK_MULTIPLY) && (key <= VK_DIVIDE) ) {
                /*
                 * This section a little uncertain: when the windows doc
                 * says 'VK_ADD' do they mean _both_ the plus key on the main
                 * keyboard _and_ the one on the numeric keypad?
                 */
                key = arith_table[ key - VK_MULTIPLY ];
            } else if( key == 0x92 ) {
                // unassigned in windows docs, but seems to be
                // the = sign on the numeric keypad on the NEC.
                // so, why not include it?
                key = '=';
            } else if( key == VK_RETURN ) {
                if( ShiftState & S_CTRL ) key = 0x0a;   /* Line Feed */
            } else if( key == VK_TAB ) {
                if( ShiftState & S_ALT ) {
                    info = SCAN( 0x1a5 );
                } else if( ShiftState & S_CTRL ) {
                    info = SCAN( EV_CTRL_TAB );
                } else if( ShiftState & S_SHIFT ) {
                    info = SCAN( EV_TAB_BACKWARD );
                } else {
                    info = SCAN( EV_TAB_FORWARD );
                }
                key = 0;
            } else {
                if( ShiftState & S_ALT ) key = 0;
                else {
                    for( i =0;i< (sizeof( kt ) / sizeof( keytable ));i++ ) {
                        if( key == kt[i].value ) {
                            if( ShiftState & S_SHIFT ) {
                                key = kt[i].shifted;
                            } else {
                                key = kt[i].regular;
                            }
                        }
                    }
                }
            }
            LastKey = (info & ~0xff) | key;
            HaveKey = TRUE;
        }
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP :
        CheckState( info, 0 );
        break;
    default:
        WindowsMouseEvent( event, info );
        break;
    }
}

void far HookRtn( unsigned event, unsigned info )
{
    /* this stuff is to get around a CG bug */
    push_ds();
    mov_ds_bx( FP_SEG( &_child ) );
    MyHookRtn( event, info );
    pop_ds();
    set_carry();
}

static          EVENT                   EventsPress[]   = {
                EV_SHIFT_PRESS,
                EV_SHIFT_PRESS,
                EV_CTRL_PRESS,
                EV_ALT_PRESS,
                EV_SCROLL_PRESS,
                EV_NUM_PRESS,
                EV_CAPS_PRESS,
                EV_INSERT_PRESS
};

static          EVENT                   EventsRelease[] = {
                EV_SHIFT_RELEASE,
                EV_SHIFT_RELEASE,
                EV_CTRL_RELEASE,
                EV_ALT_RELEASE,
                EV_SCROLL_RELEASE,
                EV_NUM_RELEASE,
                EV_CAPS_RELEASE,
                EV_INSERT_RELEASE
};

unsigned int intern getkey()
/**************************/
{
    int       ev;

    while( !HaveKey )
        {}
    ev = LastKey;
    HaveKey = FALSE;
    return( ev );
}


int intern checkkey()
/*******************/
{
    return( HaveKey );
}


void intern flushkey()
/********************/
{
    while( checkkey() ) {
        getkey();
    }
}


unsigned char intern checkshift()
/*******************************/
{
    return( ShiftState );
}

extern void far pascal SetEventHook( LPVOID );

bool intern initkeyboard()
/************************/
{
    SetEventHook( &HookRtn );
    return( FALSE );
}

void intern finikeyboard()
/************************/
{
    SetEventHook( NULL );
}


EVENT intern keyboardevent()
/**************************/
{
    register    unsigned int            key;
    register    unsigned int            scan;
    register    unsigned char           ascii;
    register    EVENT                   ev;
    register    unsigned char           newshift;
    register    unsigned char           changed;

    newshift = checkshift();
    /* checkkey must take precedence over shift change so that  *
     * typing characters by holding the alt key and typing the  *
     * ascii code on the numeric keypad works                   */
    if( checkkey() ) {
        key = getkey();
        scan = (unsigned char) ( key >> 8 ) ;
        ascii = (unsigned char) key;
        if( ascii == 0 ) {
            ev = 0x100 + scan;
        } else {
            ev = ascii;
            if( ( newshift & S_ALT ) && ( ascii == ' ' ) ) {
                ev = EV_ALT_SPACE;
            } else if( scan != 0 ) {
                switch( ev + 0x100 ) {
                case EV_RUB_OUT:
                case EV_TAB_FORWARD:
                case EV_RETURN:
                case EV_ESCAPE:
                    ev += 0x100;
                    break;
                }
            }
        }
        if( ev > EV_LAST_KEYBOARD ) {
            ev = EV_NO_EVENT;
        }
    } else {
        changed = ( newshift ^ UIData->old_shift );
        if( changed != 0 ) {
            key = 0;
            scan = 1;
            while( scan < 0x100 ) {
                if( ( changed & scan ) != 0 ) {
                    if( ( newshift & scan ) != 0 ) {
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

unsigned char global uicheckshift()
/*******************************/
{
    return( ShiftState );
}

#if defined( FORCE_WINDOWS_MESSAGE_LOOP )
void WindowsMessageLoop( int yield )
{
MSG msg;

        while( PeekMessage( &msg, NULL, NULL, NULL, PM_NOREMOVE | PM_NOYIELD ) ) {
            GetMessage( &msg, NULL, NULL, NULL );
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        if( yield ) Yield();

} /* WindowsMessageLoop */
#endif
