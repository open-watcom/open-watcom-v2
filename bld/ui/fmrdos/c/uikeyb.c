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


#include <i86.h>
#include "uidef.h"
#include "uishift.h"

extern void BIOSKeyEnableBuffering( void );
#pragma aux BIOSKeyEnableBuffering =    \
    "mov    al,00h"             \
    "mov    ah,01h"             \
    "int    90h"                \
    modify  [ax];

extern void BIOSKeySetMode( unsigned short mode );
#pragma aux BIOSKeySetMode =    \
    "push   dx"                 \
    "mov    dx,ax"              \
    "mov    ah,02h"             \
    "int    90h"                \
    "pop    dx"                 \
    parm    [ax]                \
    modify  [ax];

extern unsigned short BIOSKeyGetMode( void );
#pragma aux BIOSKeyGetMode =    \
    "push   dx"                 \
    "mov    ah,03h"             \
    "int    90h"                \
    "mov    ax,dx"              \
    "pop    dx"                 \
    value   [ax];

#define FMR_KEY_MODE_MASK       0xC000
#define FMR_KEY_ENCODE_MODE     0x0000
#define FMR_KEY_SCAN_MODE       0x4000

#define ENCODE_MODE             0
#define SCAN_MODE               1

#define FMR_KEY_CODE_MASK       0x0300
#define FMR_KEY_ASCII_CODE      0x0000
#define FMR_KEY_SHIFT_JIS_CODE  0x0200

#define FMR_KEY_MASK_MASK       0x000F
#define FMR_KEY_COMMUNICATION   0x0004
#define FMR_KEY_EDIT            0x0002
#define FMR_KEY_PF              0x0001
#define FMR_KEY_CHAR            0x0000

/* Unlocking function documented incorrectly in Tech Ref */

extern void BIOSKeyBrdUnlock( void );
#pragma aux BIOSKeyBrdUnlock =  \
    "mov    al,00h"             \
    "mov    ah,04h"             \
    "int    90h"                \
    modify  [ax];

extern void BIOSKeyFlush( void );
#pragma aux BIOSKeyFlush =      \
    "mov    al,00h"             \
    "mov    ah,06h"             \
    "int    90h"                \
    modify  [ax];

extern char BIOSKeyHit( void );
#pragma aux BIOSKeyHit =        \
    "push   bx"                 \
    "push   dx"                 \
    "mov    ah,07h"             \
    "int    90h"                \
    "pop    dx"                 \
    "pop    bx"                 \
    value   [al]                \
    modify  [ax];

extern char BIOSKeySense( void );
#pragma aux BIOSKeySense =      \
    "mov    ah,08h"             \
    "int    90h"                \
    value   [al]                \
    modify  [ax];

#define FMR_CAPS_LOCKED         0x01
#define FMR_KANA_LOCKED         0x02
#define FMR_SHIFT_KEY           0x04
#define FMR_GRAPH_KEY           0x08    /* always 0 */
#define FMR_CTRL_KEY            0x10
#define FMR_RIGHT_THUMB_KEY     0x20
#define FMR_LEFT_THUMB_KEY      0x40

/* Extended status */
#define IBMPC_SYSREQ_PRESSED    0x8000
#define IBMPC_CAPSLOCK_PRESSED  0x4000
#define IBMPC_NUMLOCK_PRESSED   0x2000
#define IBMPC_SCRLOCK_PRESSED   0x1000
#define IBMPC_RALT_PRESSED      0x0800
#define IBMPC_RCTRL_PRESSED     0x0400
#define IBMPC_LALT_PRESSED      0x0200
#define IBMPC_LCTRL_PRESSED     0x0100
/* Normal status */
#define IBMPC_INSERT_LOCKED     0x0080
#define IBMPC_CAPS_LOCKED       0x0040
#define IBMPC_NUMS_LOCKED       0x0020
#define IBMPC_SCROLL_LOCKED     0x0010
#define IBMPC_ALT_PRESSED       0x0008
#define IBMPC_CTRL_PRESSED      0x0004
#define IBMPC_LSHIFT_PRESSED    0x0002
#define IBMPC_RSHIFT_PRESSED    0x0001

struct keycode {
    unsigned char key_char;     /* from DL */
    unsigned char key_code;     /* from DH */
    unsigned char key_stat;     /* from BL */
    unsigned char key_numb;     /* from BH */
};

extern char BIOSKeyGet( struct keycode * );
#pragma aux BIOSKeyGet =        \
    "push   si"                 \
    "push   bx"                 \
    "push   dx"                 \
    "mov    si,ax"              \
    "mov    al,01h"             \
    "mov    ah,09h"             \
    "int    90h"                \
    "mov    [si],dx"            \
    "mov    2[si],bx"           \
    "pop    dx"                 \
    "pop    bx"                 \
    "pop    si"                 \
    "mov    al,ah"              \
    parm    [ax]                \
    value   [al]                \
    modify  [ax];

extern char BIOSKeyMatInp( unsigned short offset, unsigned short segment );
#pragma aux BIOSKeyMatInp =     \
    "push   ds"                 \
    "push   di"                 \
    "mov    ds,dx"              \
    "mov    di,ax"              \
    "mov    ah,0ah"             \
    "int    90h"                \
    "pop    di"                 \
    "pop    ds"                 \
    "mov    al,ah"              \
    parm    [ax][dx]            \
    value   [al]                \
    modify  [ax];

static  EVENT   EventsPress[]   = {
    EV_SHIFT_PRESS,
    EV_SHIFT_PRESS,
    EV_CTRL_PRESS,
    EV_ALT_PRESS,
    EV_SCROLL_PRESS,
    EV_NUM_PRESS,
    EV_CAPS_PRESS,
    EV_INSERT_PRESS
};

static  EVENT   EventsRelease[] = {
    EV_SHIFT_RELEASE,
    EV_SHIFT_RELEASE,
    EV_CTRL_RELEASE,
    EV_ALT_RELEASE,
    EV_SCROLL_RELEASE,
    EV_NUM_RELEASE,
    EV_CAPS_RELEASE,
    EV_INSERT_RELEASE
};


bool intern initkeyboard()
/***********************************************************************
 initkeyboard:
 ------------
    Input:      NONE
    Output:     initialization result (TRUE if successful)

    This function clears the keyboard buffer, enables buffering and
    sets key reading mode to encode mode (other possibilty is make/break
    codes).
************************************************************************/
{
    unsigned short mode;

    /* enable buffering and clear keyboard buffer */
    BIOSKeyEnableBuffering();
    /* unlock keyboard */
    BIOSKeyBrdUnlock();
    /* check state of key code mode */
    mode = BIOSKeyGetMode();
    /* if not encode mode or some classes of keys disabled */
    if( mode & (FMR_KEY_MODE_MASK | FMR_KEY_MASK_MASK ) != 0 ) {
        mode = ( mode & ~( FMR_KEY_MODE_MASK | FMR_KEY_MASK_MASK ) )
                | FMR_KEY_ENCODE_MODE;
        BIOSKeySetMode( mode );
        mode = BIOSKeyGetMode();
    }
    /* return whether "encode mode" is established */
    return( (mode & FMR_KEY_MODE_MASK) == FMR_KEY_ENCODE_MODE );
}


unsigned int extern uiextkeyboard()
/***********************************************************************
 uiextkeyboard:
 -------------
    Input:      NONE
    Output:     result of enhanced test (TRUE/FALSE)

    This function tests whether current keyboard is an enhanced keyboard
    or not.  For the FMR, we always return false
************************************************************************/
{
    return( FALSE );
}

// unsigned short intern getkey()
unsigned int   intern getkey()
/***********************************************************************
 getkey:
 ------
    Input:      NONE
    Output:     AX register is set to the scan code and internal code

    This function gets the key information, which is stored at the
    beginning of the key buffer, and returns an ASCII code and key number.
************************************************************************/
{
    struct keycode key_info;
    unsigned short mode;
    unsigned short key_data;

    key_data = 0xffff;
    if( BIOSKeyGet( &key_info ) == 0 ) {
        if( key_info.key_code != 0xFF ) {
            mode = CheckScanMode();
            if( mode == ENCODE_MODE ) {
                key_data = (key_info.key_numb << 8) | key_info.key_char;
            } else {
                /* SCAN_MODE - try to set keyboard to the way we want it */
                initkeyboard();
            }
        }
    }
    return( key_data );
}

static int CheckScanMode( void )
{
    unsigned short mode;

    mode = BIOSKeyGetMode();
    if( (mode & FMR_KEY_MODE_MASK) == FMR_KEY_ENCODE_MODE ) {
        return( ENCODE_MODE );
    }
    return( SCAN_MODE );
}

int intern checkkey()
/***********************************************************************
 checkkey:
 --------
    Input:      NONE
    Output:     Keyboard checking result (AX)

    Return number of characters in keyboard buffer.
************************************************************************/
{
    return( (int) BIOSKeyHit() );
}


void intern flushkey()
/***********************************************************************
 flushkey:
 --------
    Input:      NONE
    Output:     NONE

    This function empties the keyboard buffer.
************************************************************************/
{
    BIOSKeyFlush();
}


unsigned char intern checkshift()
/***********************************************************************
 checkshift:
 ----------
    Input:      NONE
    Output:     shift status bits

    This function reads the FMR shift status and returns an equivalent IBM AT
    status.  We implement only their common features (e.g. shift, caps, ctrl).
    However, the FMR does or does not provide status for the following keys:

        Sys Req Pressed         no
        Caps Lock Pressed       no (use Matrix Read)
        Num Lock Pressed        no
        Scroll Lock Pressed     no (use Matrix Read for PAUSE key)
        Right Alt Pressed       no (there is no right ALT key on keyboard)
        Right Ctrl Pressed      no (there is no right CTRL key on keyboard)
        Left Alt Pressed        no (use Matrix Read)
        Left Ctrl Pressed       maybe (it does provide Ctrl Pressed)

        Insert Lock             no
        Caps Lock               yes
        Numeric Lock            no
        Scroll Lock             no
        Alt Pressed             no (use Matrix Read)
        Ctrl Pressed            yes
        Left Shift Pressed      maybe (it does provide Shift Pressed)
        Right Shift Pressed     maybe (it does provide Shift Pressed)

************************************************************************/
{
    unsigned char  fmr_shift_bits; /* FMR shift bits */
    unsigned short reg_shift_bits; /* regular AT PC extended shift bits */
    char           fmr_key_array[ 16 ];

    reg_shift_bits = 0x0000;
    fmr_shift_bits = BIOSKeySense();

    BIOSKeyMatInp( FP_OFF(fmr_key_array), FP_SEG(fmr_key_array) );

    /* check caps lock key */
    if( fmr_shift_bits & FMR_CAPS_LOCKED ) {
        reg_shift_bits |= IBMPC_CAPS_LOCKED;
    }

    /* now check SHIFT key status */
    if( fmr_shift_bits & FMR_SHIFT_KEY ) {
        reg_shift_bits |= IBMPC_LSHIFT_PRESSED;
    }

    /* now check ALT key status */
    if( fmr_key_array[ 11 ] & 0x10 ) {
        reg_shift_bits |= IBMPC_ALT_PRESSED | IBMPC_LALT_PRESSED;
    }

    /* now check PF20 (alternate ALT) key status */
    if( fmr_key_array[ 15 ] & 0x08 ) {
        reg_shift_bits |= IBMPC_ALT_PRESSED | IBMPC_RALT_PRESSED;
    }

    /* now check CAP key status */
    if( fmr_key_array[ 10 ] & 0x20 ) {
        reg_shift_bits |= IBMPC_CAPSLOCK_PRESSED;
    }

    /* now check PAUSE key status */
    if( fmr_key_array[ 15 ] & 0x10 ) {
        reg_shift_bits |= IBMPC_SCRLOCK_PRESSED;
    }

    /* now check CTRL key status */
    if( fmr_shift_bits & FMR_CTRL_KEY ) {
        reg_shift_bits |= IBMPC_CTRL_PRESSED | IBMPC_LCTRL_PRESSED;
    }

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

/* scan code translation table used with Alt key */

static const char scan_tran_table[0x36] = {
/* AT CODE     CHAR  FMR CODE */
    0xFF,   /* ????  0x00 */
    0x01,   /* ESC   0x01 */
    0x78,   /* 1     0x02 */
    0x79,   /* 2     0x03 */
    0x7a,   /* 3     0x04 */
    0x7b,   /* 4     0x05 */
    0x7c,   /* 5     0x06 */
    0x7d,   /* 6     0x07 */
    0x7e,   /* 7     0x08 */
    0x7f,   /* 8     0x09 */
    0x80,   /* 9     0x0a */
    0x81,   /* 0     0x0b */
    0x82,   /* = -   0x0c */
    0x83,   /* _ ^   0x0d */
    0x2b,   /* | \   0x0e */
    0x0e,   /* <==   0x0f */
    0x0f,   /* TAB   0x10 */
    0x10,   /* Q     0x11 */
    0x11,   /* W     0x12 */
    0x12,   /* E     0x13 */
    0x13,   /* R     0x14 */
    0x14,   /* T     0x15 */
    0x15,   /* Y     0x16 */
    0x16,   /* U     0x17 */
    0x17,   /* I     0x18 */
    0x18,   /* O     0x19 */
    0x19,   /* P     0x1a */
    0x29,   /* ` @   0x1b */
    0x1A,   /* { [   0x1c */
    0x1C,   /* Enter 0x1d */
    0x1e,   /* A     0x1e */
    0x1f,   /* S     0x1f */
    0x20,   /* D     0x20 */
    0x21,   /* F     0x21 */
    0x22,   /* G     0x22 */
    0x23,   /* H     0x23 */
    0x24,   /* J     0x24 */
    0x25,   /* K     0x25 */
    0x26,   /* L     0x26 */
    0x27,   /* + ;   0x27 */
    0x28,   /* * :   0x28*/
    0x1B,   /* } ]   0x29 */
    0x2c,   /* Z     0x2A */
    0x2d,   /* X     0x2B */
    0x2e,   /* C     0x2C */
    0x2f,   /* V     0x2D */
    0x30,   /* B     0x2E */
    0x31,   /* N     0x2F */
    0x32,   /* M     0x30 */
    0x33,   /* < ,   0x31 */
    0x34,   /* > .   0x32 */
    0x35,   /* ? /   0x33 */
    0x36,   /* _ "   0x34 */
    0x39    /* Space 0x35 */
};

EVENT intern keyboardevent()
/***********************************************************************
 keyboardevent:
 -------------
    Input:      NONE
    Output:     return keyboard event

    This function checks for any keyboard event (i.e. press, release, etc.)
    and returns event if found
************************************************************************/
{
    register    unsigned int            key;
    register    unsigned char           scan;
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

        ev = ascii;

        /*
            Let's first check for special escape sequences for INS, DEL,
            PF1, PF2, ... keys. These keys generate two characters
            (e.g., ESC V) with identical scan codes.  We must ignore the
            first character which has ASCII code 0x1B and process the
            second one next time around.  Otherwise, it will look like
            you have bouncing INS, DEL, PF1, ... keys.
        */

        if( ascii == 0x1b && scan != 1 ) {  /* if it is not the ESC key */
            ev = EV_NO_EVENT;

        } else if( newshift & S_ALT ) {
            if( scan <= 0x35 ) {
                ev = 0x100 + scan_tran_table[scan];
            } else if( scan == 0x4e ) {
                ev = EV_ALT_HOME;
            } else if( scan == 0x74 ) {     /* PF13 on FM-R keyboard */
                ev = EV_ALT_HOME;
            } else if( scan == 0x75 ) {     /* PF14 on FM-R keyboard */
                ev = EV_ALT_END;
            } else if( scan == 0x7a ) {     /* PF19 on FM-R keyboard */
                ev = EV_ALT_END;
            } else if( scan == 0x7b ) {     /* PF20 on FM-R keyboard */
                ev = EV_ALT_PRESS;          /* we already faked S_ALT */
            } else if( scan == 0x4d ) {
                ev = EV_ALT_CURSOR_UP;
            } else if( scan == 0x50 ) {
                ev = EV_ALT_CURSOR_DOWN;
            } else if( scan == 0x6e ) {
                ev = EV_ALT_PGUP;
            } else if( scan == 0x70 ) {
                ev = EV_ALT_PGDN;
            } else if( scan == 0x48 ) {
                ev = EV_ALT_INSERT;
            } else if( scan == 0x4b ) {
                ev = EV_ALT_DELETE;
            } else if( scan >= 0x5d && scan <= 0x66 ) {
                ev = EV_ALT_FUNC( scan - 0x5c );
            } else if( scan == 0x69 ) {
                ev = EV_ALT_FUNC_11;
            } else if( scan == 0x5b ) {
                ev = EV_ALT_FUNC_12;
            }
        } else if( newshift & S_CTRL ) {
            if( scan == 0x01 ) {
                ev = EV_ESCAPE;
            } else if( scan == 0x30 ) {     /* Ctrl/M */
                ev = EV_RETURN;             /* same as EV_ENTER */
            } else if( scan == 0x1d ) {     /* Ctrl/Enter doesn't generate 0a */
                ev = EV_CTRL( 'j' );
            } else if( scan == 0x4f ) {
                ev = EV_CTRL_LEFT;
            } else if( scan == 0x51 ) {
                ev = EV_CTRL_RIGHT;
            } else if( scan == 0x4e ) {
                ev = EV_CTRL_HOME;
            } else if( scan == 0x74 ) {     /* PF13 on FM-R keyboard */
                ev = EV_CTRL_HOME;
            } else if( scan == 0x75 ) {     /* PF14 on FM-R keyboard */
                ev = EV_CTRL_END;
            } else if( scan == 0x7a ) {     /* PF19 on FM-R keyboard */
                ev = EV_CTRL_END;
            } else if( scan == 0x70 ) {
                ev = EV_CTRL_PGDN;
            } else if( scan == 0x6e ) {
                ev = EV_CTRL_PGUP;
            } else if( scan == 0x4d ) {
                ev = EV_CTRL_UP;
            } else if( scan == 0x50 ) {
                ev = EV_CTRL_DOWN;
            } else if( scan == 0x48 ) {
                ev = EV_CTRL_INSERT;
            } else if( scan == 0x4b ) {
                ev = EV_CTRL_DELETE;
            } else if( scan == 0x10 ) {
                ev = EV_CTRL_TAB;
            } else if( scan >= 0x5d && scan <= 0x66 ) {
                ev = EV_CTRL_FUNC( scan - 0x5c );
            } else if( scan == 0x69 ) {
                ev = EV_CTRL_FUNC_11;
            } else if( scan == 0x5b ) {
                ev = EV_CTRL_FUNC_12;
            }
        } else {
            if( scan == 0x01 ) {
                ev = EV_ESCAPE;
            } else if( scan == 0x0f ) {
                ev = EV_RUB_OUT;
            } else if( scan == 0x1d ) {
                ev = EV_ENTER;
            } else if( scan == 0x45 ) {     /* big enter key on numeric pad */
                ev = EV_ENTER;
            } else if( scan == 0x73 ) {     /* big key under arrow keys */
                ev = EV_ENTER;
            } else if( scan == 0x4e ) {
                ev = EV_HOME;
            } else if( scan == 0x74 ) {     /* PF13 on FM-R keyboard */
                ev = EV_HOME;
            } else if( scan == 0x75 ) {     /* PF14 on FM-R keyboard */
                ev = EV_END;
            } else if( scan == 0x7a ) {     /* PF19 on FM-R keyboard */
                ev = EV_END;
            } else if( scan == 0x6e ) {
                ev = EV_PAGE_UP;
            } else if( scan == 0x70 ) {
                ev = EV_PAGE_DOWN;
            } else if( scan == 0x4d ) {
                ev = EV_CURSOR_UP;
            } else if( scan == 0x50 ) {
                ev = EV_CURSOR_DOWN;
            } else if( scan == 0x4f ) {
                ev = EV_CURSOR_LEFT;
            } else if( scan == 0x51 ) {
                ev = EV_CURSOR_RIGHT;
            } else if( scan == 0x48 ) {
                ev = EV_INSERT;
            } else if( scan == 0x4b ) {
                ev = EV_DELETE;
            }
            if( newshift & S_SHIFT ) {
                if( scan == 0x10 ) {
                    ev = EV_TAB_BACKWARD;
//  not used but generated by Windows - CSK defined these
//              } else if( scan == 0x4e ) {
//                  ev = EV_SHIFT_HOME;
//              } else if( scan == 0x74 ) {     /* PF13 on FM-R keyboard */
//                  ev = EV_SHIFT_HOME;
//              } else if( scan == 0x75 ) {     /* PF14 on FM-R keyboard */
//                  ev = EV_SHIFT_END;
//              } else if( scan == 0x7a ) {     /* PF19 on FM-R keyboard */
//                  ev = EV_SHIFT_END;
//              } else if( scan == 0x6e ) {
//                  ev = EV_SHIFT_PAGE_UP;
//              } else if( scan == 0x70 ) {
//                  ev = EV_SHIFT_PAGE_DOWN;
//              } else if( scan == 0x4d ) {
//                  ev = EV_SHIFT_CURSOR_UP;
//              } else if( scan == 0x50 ) {
//                  ev = EV_SHIFT_CURSOR_DOWN;
//              } else if( scan == 0x4f ) {
//                  ev = EV_SHIFT_CURSOR_LEFT;
//              } else if( scan == 0x51 ) {
//                  ev = EV_SHIFT_CURSOR_RIGHT;
                } else if( scan >= 0x5d && scan <= 0x66 ) {
                    ev = EV_SHIFT_FUNC( scan - 0x5c );
                } else if( scan == 0x69 ) {
                    ev = EV_SHIFT_FUNC_11;
                } else if( scan == 0x5b ) {
                    ev = EV_SHIFT_FUNC_12;
                }
            } else {
                if( scan == 0x10 ) {
                    ev = EV_TAB_FORWARD;
                } else if( scan >= 0x5d && scan <= 0x66 ) {
                    ev = EV_FUNC( scan - 0x5c );
                } else if( scan == 0x69 ) {
                    ev = EV_FUNC_11;
                } else if( scan == 0x5b ) {
                    ev = EV_FUNC_12;
                }
            }
        }
#ifdef FD6
        if( ev > EV_LAST_KEYBOARD ) {
            ev = EV_NO_EVENT;
        }
#endif
    } else {    /* no key was available */
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
