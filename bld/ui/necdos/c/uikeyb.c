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
#include "uidos.h"
#include "uidef.h"
#include "uishift.h"
#include "biosui.h"

#define         READ_KEY_DATA           0x00
#define         TEST_KEY_BUFFER         0x01

#define         NEC_SHIFT_KEY           0x01
#define         NEC_CAPS_KEY            0x02
#define         NEC_KANJI_KEY           0x04
#define         NEC_GRPH_KEY            0x08
#define         NEC_CTRL_KEY            0x10

#define         EV_BACKWARD_TAB         EV_TAB_BACKWARD
#define         EV_FORWARD_TAB          EV_TAB_FORWARD

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

/*
 * definitions of the EV_ZEN_* guys.
 */
enum {
    EV_ZEN_STOP = EV_LAST_ENUMERATED + 1,
    EV_ALT_ZEN_STOP,
    EV_SHIFT_ZEN_STOP,
    EV_CTRL_ZEN_STOP,
    EV_ZEN_COPY,
    EV_SHIFT_ZEN_COPY,
    EV_ALT_ZEN_COPY,
    EV_CTRL_ZEN_COPY
};

typedef struct one_event {
    EVENT       e_normal;
    EVENT       e_shift;
    EVENT       e_alt;
    EVENT       e_ctrl;
} ONE_EVENT;

#define __ZIP EV_NO_EVENT
#define CANONICAL( e ) { EV_##e, EV_SHIFT_##e, EV_ALT_##e, EV_CTRL_##e }

const ONE_EVENT other_events[] = {
    CANONICAL( PAGE_DOWN ),
    CANONICAL( PAGE_UP ),
    { EV_INSERT,        __ZIP,          EV_ALT_INSERT,  EV_CTRL_INSERT },
    { EV_DELETE,        __ZIP,          EV_ALT_DELETE,  EV_CTRL_DELETE },
    CANONICAL( CURSOR_UP ),
    CANONICAL( CURSOR_LEFT ),
    CANONICAL( CURSOR_RIGHT ),
    CANONICAL( CURSOR_DOWN ),
    CANONICAL( HOME ),
    CANONICAL( END )
};

const ONE_EVENT top_row_events[] = {
    CANONICAL( ZEN_STOP ),
    CANONICAL( ZEN_COPY ),
    CANONICAL( FUNC( 1 ) ),
    CANONICAL( FUNC( 2 ) ),
    CANONICAL( FUNC( 3 ) ),
    CANONICAL( FUNC( 4 ) ),
    CANONICAL( FUNC( 5 ) ),
    CANONICAL( FUNC( 6 ) ),
    CANONICAL( FUNC( 7 ) ),
    CANONICAL( FUNC( 8 ) ),
    CANONICAL( FUNC( 9 ) ),
    CANONICAL( FUNC( 10 ) )
};

unsigned int extern uiextkeyboard()
/***********************************************************************
 uiextkeyboard:
 -------------
    Input:      NONE
    Output:     result of enhanced test (TRUE/FALSE)

    This function tests whether current keyboard are enhanced version
    or not.  However, for NEC we always return false
************************************************************************/
{
    return( FALSE );
}


unsigned int intern getkey()
/***********************************************************************
 getkey:
 ------
    Input:      NONE
    Output:     AX register are set to scan code and internal code

    This function read the key information, which is stored at the
    beginning of the key buffer, by converting to key data code
************************************************************************/
{
    return( BIOSGetKeyboard( READ_KEY_DATA ) );
}


int intern checkkey()
/***********************************************************************
 checkkey:
 --------
    Input:      NONE
    Output:     Keyboard checking result (AX)

    Read the key information that is stored in the beginning of the
    key buffer by converting th key data.  If there is no key code in
    the buffer return 0
************************************************************************/
{
    return( BIOSKeyboardHit( TEST_KEY_BUFFER ) );
}


void intern flushkey()
/***********************************************************************
 flushkey:
 --------
    Input:      NONE
    Output:     NONE

    This function read the keyboard buffer until there are no key value
    in the buffer
************************************************************************/
{
    while( checkkey() ) {
        getkey();
    }
}


unsigned char intern checkshift()
/***********************************************************************
 checkshift:
 ----------
    Input:      NONE
    Output:     shift status bits

    This function read NEC shift status and return regular PC status
    bit pattern.  For now I just implement their common features (e.g.
    shift, caps, ctrl).  However, NEC does not provide some key status
    (e.g. left, right shift , alt, scroll lock, num lock, insert mode)
    of regular PC, but it also provide some of key status (e.g. Kanji lock,
    grph) which are not provided in regular PCs.
    Since we need to use Alt key we simulated this using Grph key in NEC.
************************************************************************/
{
    unsigned char nec_shift_bits; /* NEC shift bit pattern */
    unsigned char reg_shift_bits; /* regular PC shift bit pattern */

    nec_shift_bits = BIOSSenseShift();
    reg_shift_bits =
        ( ( nec_shift_bits & NEC_SHIFT_KEY ) ? S_SHIFT : 0 ) |
        ( ( nec_shift_bits & NEC_CTRL_KEY ) ? S_CTRL : 0 ) |
        ( ( nec_shift_bits & NEC_CAPS_KEY ) ? S_CAPS : 0 ) |
        ( ( nec_shift_bits & NEC_GRPH_KEY ) ? S_ALT : 0 );

    return( reg_shift_bits );
}


unsigned char global uicheckshift()
/***********************************************************************
 uicheckshift:
 ------------
    Input:      NONE
    Output:     shift status bits

    This function is a UI layer version of function checkshift
************************************************************************/
{
    return( checkshift() );
}


bool intern initkeyboard()
/***********************************************************************
 initkeyboard:
 ------------
    Input:      NONE
    Output:     initialization result (TRUE always (???))

    This function initialize keyboard interface and initialize keyboard
    buffer.
************************************************************************/
{
    BIOSInitKeyboard();
    return( TRUE );
}


EVENT intern keyboardevent()
/***********************************************************************
 keyboardevent:
 -------------
    Input:      NONE
    Output:     return keyboard event

    This function checks for any keyboard event (i.e. press, release etc)
    and returns event if found
************************************************************************/
{
    register    unsigned int            key;
    register    unsigned int            scan;
    register    unsigned char           ascii;
    register    EVENT                   ev = EV_NO_EVENT;
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

        /* let's first check Alt(Grph) character combination */
        if( ascii == 0 ) {
            /*
             * This is a NEC special key...
             */
            if( ( scan >= 0x60 ) && ( scan <= 0x6B ) ) {
                if( newshift & S_ALT ) {
                    ev = top_row_events[ scan - 0x60 ].e_alt;
                } else if( newshift & S_CTRL ) {
                    ev = top_row_events[ scan - 0x60 ].e_ctrl;
                } else if( newshift & S_SHIFT ) {
                    ev = top_row_events[ scan - 0x60 ].e_shift;
                } else {
                    ev = top_row_events[ scan - 0x60 ].e_normal;
                }
            } else if( ( scan >= 0x36 ) && ( scan <= 0x3F ) ) {
                if( newshift & S_ALT ) {
                    if( ( newshift & S_CTRL ) && ( scan == 0x39 ) ) {
                        ev = EV_CTRL_ALT_DELETE;
                        newshift ^= S_ALT; // get rid of alt press.
                    } else {
                        ev = other_events[ scan - 0x36 ].e_alt;
                    }
                } else if( newshift & S_CTRL ) {
                    ev = other_events[ scan - 0x36 ].e_ctrl;
                } else if( newshift & S_SHIFT ) {
                    ev = other_events[ scan - 0x36 ].e_shift;
                } else {
                    ev = other_events[ scan - 0x36 ].e_normal;
                }
            } else {
                if( scan == 0xAE ) {
                    ev = EV_SHIFT_HOME;
                }
            }
        } else if( newshift & S_ALT ) {
            /*
             * Align the scan codes for letters with the IBM codes.
             * ( NEC BIOS book , p. 198 )
             */
            if( ( scan >= 0x10 ) && ( scan <= 0x19 ) ) {
                ev = 0x100 + scan;
            } else if( ( scan >= 0x1D ) && ( scan <= 0x25 ) ) {
                ev = 0x101 + scan;
            } else if( ( scan >= 0x29 ) && ( scan <= 0x2F ) ) {
                ev = 0x103 + scan;
            } else if( ascii == ' ' ) {
                ev = EV_ALT_SPACE;
            }
        } else {
            ev = ascii;
            switch( ev + 0x100 ) {
            case EV_RUB_OUT:
                ev += 0x100;
                break;
            case EV_TAB_FORWARD:
                if( newshift & S_CTRL ) {
                    ev = EV_CTRL_TAB;
                } else if( newshift & S_SHIFT ) {
                    ev = EV_BACKWARD_TAB;
                } else {
                    ev = EV_FORWARD_TAB;
                }
                break;
            case EV_RETURN:
                if( newshift & S_CTRL ) {
                    ev = EV_CTRL_RETURN;
                } else {
                    ev = EV_RETURN;
                }
                break;
            case EV_ESCAPE:
                ev += 0x100;
                break;
            }
        }
#ifdef FD6
        if( ev > EV_LAST_KEYBOARD ) {\
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


EVENT global uikeyboardevent()
/***********************************************************************
 uikeyboardevent:
 ---------------
    Input:      NONE
    Output:     keyboard event

    This function is a UI layer version of function keyboardevent
************************************************************************/
{
    return( keyboardevent() );
}
