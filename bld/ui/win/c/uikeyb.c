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
* Description:  Keyboard input for Windows.
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


typedef struct {
    unsigned char   value;
    char            regular;
    char            shifted;
} keytable;

static keytable kt[] = {
{ 0xbc,',','<' },
{ 0xbe,'.','>' },
{ 0xbf,'/','?' },
{ 0xba,';',':' },
{ 0xde,'\'','"' },
{ 0xdb,'[','{' },
{ 0xdd,']','}' },
{ 0xdc,'\\','|' },
{ 0xc0,'`','~' },
{ 0xbd,'-','_' },
{ 0xbb,'=','+' }
};

static unsigned char    ShiftState;
static unsigned         LastKey;
static volatile bool    HaveKey;

#pragma aux set_carry = 0xf9;
extern void set_carry( void );

extern void WindowsMouseEvent( unsigned, unsigned );

#pragma aux HookRtn far parm [ax] [cx] modify exact [];

static int CheckState( unsigned info, unsigned down )
{
    unsigned bit;

    switch( info & 0xff ) {
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
        bit = S_NUM;
        break;
    case 0x91:          /* scroll lock key */
        bit = S_SCROLL;
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

unsigned PickOne( unsigned alt, unsigned ctrl, unsigned shift, unsigned plain )
{
    unsigned    info;

    if( ShiftState & S_ALT ) {
        info = alt;
    } else if( ShiftState & S_CTRL ) {
        info = ctrl;
    } else if( ShiftState & S_SHIFT ) {
        info = shift;
    } else {
        info = plain;
    }
    return( ( info - 0x100 ) << 8 ); // fake up scan code
}

static void MyHookRtn( unsigned event, unsigned info )
{
    unsigned    key;
    int         i;
    switch( event ) {
    case WM_SYSKEYDOWN :
    case WM_KEYDOWN:
        if( !CheckState( info, 1 ) ) {
            key = info & 0xff;
            if( key >= VK_F1 && key <= VK_F10 ) {
                key = key - VK_F1 + 1;
                info = PickOne( EV_ALT_FUNC( key ),
                                EV_CTRL_FUNC( key ),
                                EV_SHIFT_FUNC( key ),
                                EV_FUNC( key ) );
                key = 0;
            } else if( key == VK_F11 ) {
                info = PickOne( EV_ALT_FUNC_11,
                                EV_CTRL_FUNC_11,
                                EV_SHIFT_FUNC_11,
                                EV_FUNC_11 );
                key = 0;
            } else if( key == VK_F12 ) {
                info = PickOne( EV_ALT_FUNC_12,
                                EV_CTRL_FUNC_12,
                                EV_SHIFT_FUNC_12,
                                EV_FUNC_12 );
                key = 0;
            } else if( key >= 'A' && key <= 'Z' ) {
                if( ShiftState & S_ALT ) {
                    key = 0;
                } else if( ShiftState & S_CTRL ) {
                    key -= 'A'-1;
                } else if( !(ShiftState & S_SHIFT) ) {
                    if( !(ShiftState & S_CAPS) ) key += 'a' - 'A';
                } else if( ShiftState & S_CAPS ) {
                    key += 'a' - 'A';
                }
            } else if( key >= '0' && key <= '9' ) {
                if( ShiftState & S_ALT ) {
                    key = 0;
                } else if( ShiftState & S_SHIFT ) {
                    key = ")!@#$%^&*("[key-'0'];
                }
            } else if( key == VK_PRIOR ) {
                info = PickOne( EV_ALT_PGUP,
                                EV_CTRL_PGUP,
                                EV_PAGE_UP,
                                EV_PAGE_UP );
                key = 0;
            } else if( key == VK_NEXT ) {
                info = PickOne( EV_ALT_PGDN,
                                EV_CTRL_PGDN,
                                EV_PAGE_DOWN,
                                EV_PAGE_DOWN );
                key = 0;
            } else if( key == VK_END ) {
                info = PickOne( EV_ALT_END,
                                EV_CTRL_END,
                                EV_SHIFT_END,
                                EV_END );
                key = 0;
            } else if( key == VK_HOME ) {
                info = PickOne( EV_ALT_HOME,
                                EV_CTRL_HOME,
                                EV_SHIFT_HOME,
                                EV_HOME );
                key = 0;
            } else if( key == VK_LEFT ) {
                info = PickOne( EV_ALT_CURSOR_LEFT,
                                EV_CTRL_CURSOR_LEFT,
                                EV_SHIFT_CURSOR_LEFT,
                                EV_CURSOR_LEFT );
                key = 0;
            } else if( key == VK_RIGHT ) {
                info = PickOne( EV_ALT_CURSOR_RIGHT,
                                EV_CTRL_CURSOR_RIGHT,
                                EV_SHIFT_CURSOR_RIGHT,
                                EV_CURSOR_RIGHT );
                key = 0;
            } else if( key == VK_UP ) {
                info = PickOne( EV_ALT_CURSOR_UP,
                                EV_CTRL_CURSOR_UP,
                                EV_SHIFT_CURSOR_UP,
                                EV_CURSOR_UP );
                key = 0;
            } else if( key == VK_DOWN ) {
                info = PickOne( EV_ALT_CURSOR_DOWN,
                                EV_CTRL_CURSOR_DOWN,
                                EV_SHIFT_CURSOR_DOWN,
                                EV_CURSOR_DOWN );
                key = 0;
            } else if( key == VK_RETURN ) {
                if( ShiftState & S_CTRL ) key = 0x0a;   /* Line Feed */
            } else if( key == VK_TAB ) {
                info = PickOne( 0x1a5,
                                EV_CTRL_TAB,
                                EV_TAB_BACKWARD,
                                EV_TAB_FORWARD );
                key = 0;
            } else if( key >= VK_SELECT && key <= VK_HELP ) {
                key = 0;
            } else {
                if( ShiftState & S_ALT ) {
                    key = 0;
                } else {
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

/*
 * The handler installed by SetEventHook uses non-standard calling convention.
 * Arguments are passed in ax and cx, and setting carry flag before exit
 * may cause the message to be discarded. Also, the routine has to set ds
 * to the proper value (ie. no multiple instances - but it may not be possible
 * to register multiple event hooks anyway). See Undocumented Windows.
 *
 * Note - this keyboard input method looks like a really ugly hack.
 */ 
void __far __loadds HookRtn( unsigned event, unsigned info )
{
    MyHookRtn( event, info );
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

unsigned int intern getkey( void )
/********************************/
{
    int       ev;

    while( !HaveKey )
        {}
    ev = LastKey;
    HaveKey = FALSE;
    return( ev );
}


int intern checkkey( void )
/*************************/
{
    return( HaveKey );
}


void intern flushkey( void )
/**************************/
{
    while( checkkey() ) {
        getkey();
    }
}


unsigned char intern checkshift( void )
/*************************************/
{
    return( ShiftState );
}

#if 0
static POINT p;

/* These next two routines are for use by WVIDEO */
void HookInQueue( void )
{
//  GetCursorPos( &p );
//  ShowCursor( FALSE );
}

void HookOutQueue( void )
{
//  SetCursorPos( p.x, p.y );
//  ShowCursor( TRUE );
}
#endif

extern void far pascal SetEventHook( LPVOID );

bool intern initkeyboard( void )
/******************************/
{
    SetEventHook( &HookRtn );
    return( FALSE );
}

void intern finikeyboard( void )
/******************************/
{
    SetEventHook( NULL );
}


EVENT intern keyboardevent( void )
/********************************/
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

unsigned char global uicheckshift( void )
/***************************************/
{
    return( ShiftState );
}
#define FORCE_WINDOWS_MESSAGE_LOOP
#if defined( FORCE_WINDOWS_MESSAGE_LOOP )
void WindowsMessageLoop( int yield )
{
MSG msg;

        while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE | PM_NOYIELD ) ) {
            GetMessage( &msg, NULL, 0, 0 );
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        if( yield ) Yield();

} /* WindowsMessageLoop */
#endif
