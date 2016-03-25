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
#include <windows.h>
#include "uidos.h"
#include "uidef.h"
#include "uishift.h"
#include "biosui.h"


#define PICK_ONE(x) PickOne( EV_ALT_##x, EV_CTRL_##x, EV_SHIFT_##x, EV_##x )

typedef struct {
    unsigned char   value;
    char            regular;
    char            shifted;
} keytable;

static keytable kt[] = {
    { 0xbc, ',',  '<' },
    { 0xbe, '.',  '>' },
    { 0xbf, '/',  '?' },
    { 0xba, ';',  ':' },
    { 0xde, '\'', '"' },
    { 0xdb, '[',  '{' },
    { 0xdd, ']',  '}' },
    { 0xdc, '\\', '|' },
    { 0xc0, '`',  '~' },
    { 0xbd, '-',  '_' },
    { 0xbb, '=',  '+' }
};

static unsigned char    ShiftState;
static unsigned         LastKey;
static volatile bool    HaveKey;

#pragma aux set_carry = 0xf9;
extern void set_carry( void );

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
        if( down ) {
            ShiftState ^= bit;
        }
    } else if( down ) {
        ShiftState |= bit;
    } else {
        ShiftState &= ~bit;
    }
    return( TRUE );
}

static unsigned PickOne( unsigned alt, unsigned ctrl, unsigned shift, unsigned plain )
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
            switch( key ) {
            case VK_F1:
                LastKey = PICK_ONE( F1 );
                break;
            case VK_F2:
                LastKey = PICK_ONE( F2 );
                break;
            case VK_F3:
                LastKey = PICK_ONE( F3 );
                break;
            case VK_F4:
                LastKey = PICK_ONE( F4 );
                break;
            case VK_F5:
                LastKey = PICK_ONE( F5 );
                break;
            case VK_F6:
                LastKey = PICK_ONE( F6 );
                break;
            case VK_F7:
                LastKey = PICK_ONE( F7 );
                break;
            case VK_F8:
                LastKey = PICK_ONE( F8 );
                break;
            case VK_F9:
                LastKey = PICK_ONE( F9 );
                break;
            case VK_F10:
                LastKey = PICK_ONE( F10 );
                break;
            case VK_F11:
                LastKey = PICK_ONE( F11 );
                break;
            case VK_F12:
                LastKey = PICK_ONE( F12 );
                break;
            case VK_PRIOR:
                LastKey = PICK_ONE( PAGE_UP );
                break;
            case VK_NEXT:
                LastKey = PICK_ONE( PAGE_DOWN );
                break;
            case VK_END:
                LastKey = PICK_ONE( END );
                break;
            case VK_HOME:
                LastKey = PICK_ONE( HOME );
                break;
            case VK_LEFT:
                LastKey = PICK_ONE( CURSOR_LEFT );
                break;
            case VK_RIGHT:
                LastKey = PICK_ONE( CURSOR_RIGHT );
                break;
            case VK_UP:
                LastKey = PICK_ONE( CURSOR_UP );
                break;
            case VK_DOWN:
                LastKey = PICK_ONE( CURSOR_DOWN );
                break;
            case VK_TAB:
                LastKey = PICK_ONE( TAB );
                break;
            case VK_RETURN:
                if( ShiftState & S_CTRL )
                    info = (info & ~0xff) | 0x0a;   /* Line Feed */
                LastKey = info;
                break;
            case VK_SELECT:
            case VK_PRINT:
            case VK_EXECUTE:
            case VK_SNAPSHOT:
            case VK_INSERT:
            case VK_DELETE:
            case VK_HELP:
                LastKey = info & ~0xff;
                break;
            default:
                if( key >= 'A' && key <= 'Z' ) {
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
                } else {
                    if( ShiftState & S_ALT ) {
                        key = 0;
                    } else {
                        for( i = 0; i < ( sizeof( kt ) / sizeof( keytable ) ); i++ ) {
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
                break;
            }
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
void __far __loadds win_uihookrtn( unsigned event, unsigned info )
{
    MyHookRtn( event, info );
    set_carry();
}

static EVENT    EventsPress[] = {
    EV_SHIFT_PRESS,
    EV_SHIFT_PRESS,
    EV_CTRL_PRESS,
    EV_ALT_PRESS,
    EV_SCROLL_PRESS,
    EV_NUM_PRESS,
    EV_CAPS_PRESS,
    EV_INSERT_PRESS
};

static EVENT    EventsRelease[] = {
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

extern void __far __pascal SetEventHook( LPVOID );

bool intern initkeyboard( void )
/******************************/
{
    SetEventHook( &win_uihookrtn );
    return( false );
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
        ev = scan + 0x100;
        if( ascii != 0 ) {
            ev = ascii;
            if( ( newshift & S_ALT ) && ( ascii == ' ' ) ) {
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
        if( !iskeyboardchar( ev ) ) {
            ev = EV_NO_EVENT;
        }
    } else {
        changed = ( newshift ^ UIData->old_shift );
        if( changed != 0 ) {
            key = 0;
            scan = 1;
            while( scan < (1 << 8) ) {
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

unsigned char UIAPI uicheckshift( void )
/***************************************/
{
    return( ShiftState );
}

#if 0
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
#endif
