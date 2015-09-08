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
#include <dos.h>
#ifdef __386__
#include <conio.h>
#endif
#include "uidef.h"
#include "uidos.h"
#include "uishift.h"
#include "biosui.h"

#define         NRM_KEY_READ            0x00
#define         NRM_KEY_STAT            0x01
#define         NRM_KEY_SHFT            0x02
#define         NRM_KEY_WRITE           0x05

#define         EXT_KEY_READ            0x10
#define         EXT_KEY_STAT            0x11
#define         EXT_KEY_SHFT            0x12

static unsigned char    ReadReq;      /* this will be 0x00 or 0x10 */

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

unsigned int extern uiextkeyboard( void )
/***************************************/
{
    return( ReadReq != NRM_KEY_READ );
}


unsigned int intern getkey( void )
/********************************/
{
    return( BIOSGetKeyboard( ReadReq ) );
}


int intern checkkey( void )
/*************************/
{
    return( BIOSKeyboardHit( ReadReq + 1 ) );
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
    return( BIOSGetKeyboard( ReadReq + 2 ) );
}


unsigned char UIAPI uicheckshift( void )
/***************************************/

{
    return( checkshift() );
}


#define RSH(x)  ( ( ( x ) & 0x0002 ) != 0 )
#define LSH(x)  ( ( ( x ) & 0x0002 ) != 0 )
#define CT(x)   ( ( ( x ) & 0x0004 ) != 0 )
#define AL(x)   ( ( ( x ) & 0x0008 ) != 0 )
#define LCT(x)  ( ( ( x ) & 0x0100 ) != 0 )
#define LAL(x)  ( ( ( x ) & 0x0200 ) != 0 )
#define RCT(x)  ( ( ( x ) & 0x0400 ) != 0 )
#define RAL(x)  ( ( ( x ) & 0x0800 ) != 0 )

bool intern initkeyboard( void )
/******************************/
{
    unsigned x;

    ReadReq = NRM_KEY_READ;
    x = BIOSTestKeyboard();
    if( (x & 0xff) == 0xff ) return( TRUE ); /* too many damn keys pressed! */
    if( AL( x ) != ( RAL( x ) || LAL( x ) ) ) return( TRUE );
    if( CT( x ) != ( RCT( x ) || LCT( x ) ) ) return( TRUE );
    ReadReq = EXT_KEY_READ;
    return( TRUE );
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
        if( scan != 0 && ascii == 0xe0 ) {  /* extended keyboard */
            ascii = 0;
        }
        /* ignore shift key for numeric keypad if numlock is not on */
        if( scan + 0x100 >= EV_HOME && scan + 0x100 <= EV_DELETE ) {
            if( ( newshift & S_NUM ) == 0 ) {
                if( ( newshift & S_SHIFT ) != 0 ) {
                    ascii = 0;      /* wipe out digit */
                }
            }
        }
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
#ifdef FD6
        if( !iskeyboardchar( ev ) ) {
            ev = EV_NO_EVENT;
        }
#endif
    } else {
        changed = ( newshift ^ UIData->old_shift );
        if( changed != 0 ) {
            key = 0;
            scan = 1;
            while( scan < 256 ) {
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

EVENT UIAPI uikeyboardevent( void )
/**********************************/
{
    return( keyboardevent() );
}
