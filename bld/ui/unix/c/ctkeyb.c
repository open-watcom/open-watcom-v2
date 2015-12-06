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
* Description:  Terminal keyboard input processing.
*
****************************************************************************/


#include <term.h>
#include "ctkeyb.h"

unsigned short   ct_shift_state;

enum {
    EV_STICKY_FUNC      = 0xff0,
    EV_STICKY_SHIFT,
    EV_STICKY_CTRL,
    EV_STICKY_ALT,
    S_FUNC              = S_CAPS
};

#define NUM_ELTS( a )   (sizeof( a ) / sizeof( a[0] ))

struct an_in_term_info {
    EVENT               ev;
    char                *str;
};

#define NUM_IN_TERM_INFO_MAPPINGS 74
struct an_in_term_info InTerminfo[NUM_IN_TERM_INFO_MAPPINGS];

bool init_interminfo( void )
/**************************/
{
    struct an_in_term_info      *entry;

    entry = InTerminfo;

#define evmap( code, terminfo_code )            \
    entry->ev = EV_##code;                      \
    entry->str = terminfo_code;                 \
    ++entry;

    evmap( RUB_OUT,             key_backspace )
    evmap( RUB_OUT,             key_clear )
    evmap( DELETE,              key_dc )
    evmap( CURSOR_DOWN,         key_down )
    evmap( INSERT,              key_ic )
    evmap( F1,                  key_f1 )
    evmap( F2,                  key_f2 )
    evmap( F3,                  key_f3 )
    evmap( F4,                  key_f4 )
    evmap( F5,                  key_f5 )
    evmap( F6,                  key_f6 )
    evmap( F7,                  key_f7 )
    evmap( F8,                  key_f8 )
    evmap( F9,                  key_f9 )
    evmap( F10,                 key_f10 )
    evmap( F11,                 key_f11 )
    evmap( F12,                 key_f12 )
    evmap( SHIFT_F1,            key_f13 )
    evmap( SHIFT_F2,            key_f14 )
    evmap( SHIFT_F3,            key_f15 )
    evmap( SHIFT_F4,            key_f16 )
    evmap( SHIFT_F5,            key_f17 )
    evmap( SHIFT_F6,            key_f18 )
    evmap( SHIFT_F7,            key_f19 )
    evmap( SHIFT_F8,            key_f20 )
    evmap( SHIFT_F9,            key_f21 )
    evmap( SHIFT_F10,           key_f22 )
    evmap( SHIFT_F11,           key_f23 )
    evmap( SHIFT_F12,           key_f24 )
    evmap( CTRL_F1,             key_f25 )
    evmap( CTRL_F2,             key_f26 )
    evmap( CTRL_F3,             key_f27 )
    evmap( CTRL_F4,             key_f28 )
    evmap( CTRL_F5,             key_f29 )
    evmap( CTRL_F6,             key_f30 )
    evmap( CTRL_F7,             key_f31 )
    evmap( CTRL_F8,             key_f32 )
    evmap( CTRL_F9,             key_f33 )
    evmap( CTRL_F10,            key_f34 )
    evmap( CTRL_F11,            key_f35 )
    evmap( CTRL_F12,            key_f36 )
    evmap( ALT_F1,              key_f37 )
    evmap( ALT_F2,              key_f38 )
    evmap( ALT_F3,              key_f39 )
    evmap( ALT_F4,              key_f40 )
    evmap( ALT_F5,              key_f41 )
    evmap( ALT_F6,              key_f42 )
    evmap( ALT_F7,              key_f43 )
    evmap( ALT_F8,              key_f44 )
    evmap( ALT_F9,              key_f45 )
    evmap( ALT_F10,             key_f46 )
    evmap( ALT_F11,             key_f47 )
    evmap( ALT_F12,             key_f48 )
    evmap( HOME,                key_home )
    evmap( CURSOR_LEFT,         key_left )
    evmap( PAGE_DOWN,           key_npage )
    evmap( PAGE_UP,             key_ppage )
    evmap( CURSOR_RIGHT,        key_right )
    evmap( SCROLL_LINE_DOWN,    key_sf )
    evmap( SCROLL_LINE_UP,      key_sr )
    evmap( CURSOR_UP,           key_up )
    evmap( HOME,                key_beg )
    evmap( ESCAPE,              key_cancel )
    evmap( END,                 key_end )
    evmap( ENTER,               key_enter )
    evmap( TAB_FORWARD,         key_next )
    evmap( TAB_BACKWARD,        key_previous )
    evmap( SHIFT_HOME,          key_sbeg )
    evmap( SHIFT_END,           key_send )
    evmap( SHIFT_CURSOR_LEFT,   key_sleft )
    evmap( TAB_BACKWARD,        key_snext )
    evmap( TAB_FORWARD,         key_sprevious )
    evmap( SHIFT_CURSOR_RIGHT,  key_sright )
    evmap( TAB_BACKWARD,        key_btab )

    // Check to see that the correct number of events were added
    return( NUM_ELTS( InTerminfo ) == NUM_IN_TERM_INFO_MAPPINGS );
}

static const struct {
    EVENT       ev;
    char        ch;
} InStandard[] = {
/*

See ck_keyboard_event function for special handling of these codes.

    { EV_REDRAW_SCREEN, '\x0c' },
    { EV_RUB_OUT,       '\x08' },
    { EV_TAB_FORWARD,   '\x09' },
*/
    { EV_ENTER,         '\r' },
    { EV_ENTER,         '\n' },
    { EV_RUB_OUT,       '\x7f' },
    { EV_ESCAPE,        '\x1b' },
};

typedef struct {
    EVENT       normal;
    EVENT       shift;
    EVENT       ctrl;
    EVENT       alt;
} event_shift_map;

#define SPECIAL_MAP( name, c )  { c, c, c, EV_ALT_##name }
#define LETTER_MAP( let, c )    { c+' ', c, c-'@', EV_ALT_##let }
#define MOTION_MAP( k )         { EV_##k,EV_SHIFT_##k,EV_CTRL_##k,EV_ALT_##k}
#define FUNC_MAP( k )           MOTION_MAP(k)

/*
    NOTE: this table has to be in increasing value of the first column.
*/
static const event_shift_map ShiftMap[] = {
    SPECIAL_MAP( SPACE, ' ' ),
    SPECIAL_MAP( MINUS, '-' ),
    SPECIAL_MAP( 0, '0' ),
    SPECIAL_MAP( 1, '1' ),
    SPECIAL_MAP( 2, '2' ),
    SPECIAL_MAP( 3, '3' ),
    SPECIAL_MAP( 4, '4' ),
    SPECIAL_MAP( 5, '5' ),
    SPECIAL_MAP( 6, '6' ),
    SPECIAL_MAP( 7, '7' ),
    SPECIAL_MAP( 8, '8' ),
    SPECIAL_MAP( 9, '9' ),
    SPECIAL_MAP( EQUAL, '=' ),
    LETTER_MAP( A, 'A' ),
    LETTER_MAP( B, 'B' ),
    LETTER_MAP( C, 'C' ),
    LETTER_MAP( D, 'D' ),
    LETTER_MAP( E, 'E' ),
    LETTER_MAP( F, 'F' ),
    LETTER_MAP( G, 'G' ),
    LETTER_MAP( H, 'H' ),
    LETTER_MAP( I, 'I' ),
    LETTER_MAP( J, 'J' ),
    LETTER_MAP( K, 'K' ),
    LETTER_MAP( L, 'L' ),
    LETTER_MAP( M, 'M' ),
    LETTER_MAP( N, 'N' ),
    LETTER_MAP( O, 'O' ),
    LETTER_MAP( P, 'P' ),
    LETTER_MAP( Q, 'Q' ),
    LETTER_MAP( R, 'R' ),
    LETTER_MAP( S, 'S' ),
    LETTER_MAP( T, 'T' ),
    LETTER_MAP( U, 'U' ),
    LETTER_MAP( V, 'V' ),
    LETTER_MAP( W, 'W' ),
    LETTER_MAP( X, 'X' ),
    LETTER_MAP( Y, 'Y' ),
    LETTER_MAP( Z, 'Z' ),
    FUNC_MAP( F1 ),
    FUNC_MAP( F2 ),
    FUNC_MAP( F3 ),
    FUNC_MAP( F4 ),
    FUNC_MAP( F5 ),
    FUNC_MAP( F6 ),
    FUNC_MAP( F7 ),
    FUNC_MAP( F8 ),
    FUNC_MAP( F9 ),
    FUNC_MAP( F10 ),
    MOTION_MAP( HOME ),
    MOTION_MAP( CURSOR_UP ),
    MOTION_MAP( PAGE_UP ),
    MOTION_MAP( CURSOR_LEFT ),
    MOTION_MAP( CURSOR_RIGHT ),
    MOTION_MAP( END ),
    MOTION_MAP( CURSOR_DOWN ),
    MOTION_MAP( PAGE_DOWN ),
    MOTION_MAP( INSERT ),
    MOTION_MAP( DELETE ),
    FUNC_MAP( F11 ),
    FUNC_MAP( F12 ),
};


void intern clear_shift( void )
{
    ct_shift_state = 0;
}

void intern ck_arm( void )
/************************/
{
}

#define PUSHBACK_SIZE   32

static char     UnreadBuffer[PUSHBACK_SIZE];
static int      UnreadPos = sizeof( UnreadBuffer );

int nextc( int n )      // delay in 0.1 seconds -- not to exceed 9
/****************/
{
    unsigned char   ch;
    int             test;       // Used to test the return of read()
    int             fds;

    if( UnreadPos < sizeof( UnreadBuffer ) )
        return( UnreadBuffer[UnreadPos++] );

    n *= 100000;
    fds = kb_wait( n / 1000000, n % 1000000 );
    if( fds == 1 ) {                    // Console has input
        test = read( UIConHandle, &ch, 1 );
        if( test == -1 ) {
            return( -1 );
        }
    } else if( fds == 2 ) {
        return( 256 ); /* mouse event */
    } else {
        return( -1 );
    }
    return( ch );
}

void nextc_unget( char *str, int n )
/**********************************/
{
    UnreadPos -= n;
    //assert( UnreadPos >= 0 );
    memcpy( &(UnreadBuffer[UnreadPos]), str, n );
}


static int find_entry( const void *pkey, const void *pbase )
{
    const EVENT                 *evp = pkey;
    const event_shift_map       *entry = pbase;

    return( *evp - entry->normal );
}

EVENT ck_keyboardevent( void )
/****************************/
{
    EVENT                       ev;
    EVENT                       search_ev;
    event_shift_map             *entry;
    static unsigned short       sticky;
    static unsigned short       real_shift;

    ev = TrieRead();
    ck_shift_state();
    if( ct_shift_state != ( real_shift | sticky ) ) {
        /* did it change? */
        real_shift = ct_shift_state;
    }
    switch( ev ) {
    case EV_STICKY_FUNC:
        sticky ^= S_FUNC;
        break;
    case EV_STICKY_SHIFT:
        sticky ^= S_SHIFT;
        break;
    case EV_STICKY_CTRL:
        sticky ^= S_CTRL;
        break;
    case EV_STICKY_ALT:
        sticky ^= S_ALT;
        break;
    case EV_SHIFT_PRESS:
        real_shift |= S_SHIFT;
        break;
    case EV_CTRL_PRESS:
        real_shift |= S_CTRL;
        break;
    case EV_ALT_PRESS:
        real_shift |= S_ALT;
        break;
    case EV_SHIFT_RELEASE:
        if( !(real_shift & S_SHIFT) ) ev = EV_NO_EVENT;
        real_shift &= ~S_SHIFT;
        break;
    case EV_CTRL_RELEASE:
        if( !(real_shift & S_CTRL) ) ev = EV_NO_EVENT;
        real_shift &= ~S_CTRL;
        break;
    case EV_ALT_RELEASE:
        if( !(real_shift & S_ALT) ) ev = EV_NO_EVENT;
        real_shift &= ~S_ALT;
        break;
    case EV_NO_EVENT:
        break;
    default:
        if( sticky & S_FUNC ) {
            switch( ev ) {
            case '1':
                ev = EV_F1;
                break;
            case '2':
                ev = EV_F2;
                break;
            case '3':
                ev = EV_F3;
                break;
            case '4':
                ev = EV_F4;
                break;
            case '5':
                ev = EV_F5;
                break;
            case '6':
                ev = EV_F6;
                break;
            case '7':
                ev = EV_F7;
                break;
            case '8':
                ev = EV_F8;
                break;
            case '9':
                ev = EV_F9;
                break;
            case '0':
                ev = EV_F10;
                break;
            case 'A':
            case 'a':
                ev = EV_F11;
                break;
            case 'B':
            case 'b':
                ev = EV_F12;
                break;
            }
            sticky &= ~S_FUNC;
        }
        if( !(real_shift & S_CTRL) ) {
            /*
                If the ctrl key isn't down (won't ever be on a terminal)
                then we want to see certain CTRL-? combinations come back
                as some standard UI events.
            */
            switch( ev ) {
            case '\x08':
               /* ctrl-backspace often does the opposite of backspace */
                if( strcmp(key_backspace, "\x08" ) == 0 )
                    ev = EV_RUB_OUT;
                else
                    ev = EV_CTRL_BACKSPACE;
                break;
            case '\x09':
                ev = EV_TAB_FORWARD;
                break;
            case '\x0c':
                ev = EV_REDRAW_SCREEN;
                break;
            }
        }
        ct_shift_state = sticky | real_shift;
        sticky = 0;

        #define S_MASK  (S_SHIFT|S_CTRL|S_ALT)

        if( ct_shift_state & S_MASK ) {
            search_ev = tolower( ev );
            entry = bsearch( &search_ev, ShiftMap, NUM_ELTS( ShiftMap ),
                                sizeof( ShiftMap[0] ), find_entry );
            if( entry != NULL ) {
                if( ct_shift_state & S_SHIFT ) {
                    ev = entry->shift;
                } else if( ct_shift_state & S_CTRL ) {
                    ev = entry->ctrl;
                } else { /* must be ALT */
                    ev = entry->alt;
                }
            }
        }
        if( ev ) {
            UIDebugPrintf1( "UI: Something read: %4.4X", ev );
        }
        return( ev );
    }
    ct_shift_state = real_shift;
    if( ev ) {
        UIDebugPrintf1( "UI: Something read: %4.4X", ev );
    }
    return( ev );
}

EVENT tk_keyboardevent( void )
{
    EVENT       ev;

    ev = ck_keyboardevent();
    if( ev != EV_MOUSE_PRESS )
        return( ev );
    UIDebugPrintf0( "UI: Mouse event handling" );
    tm_saveevent();
    return( EV_NO_EVENT ); /* make UI check for mouse events */
}


int init_trie( void )
/*******************/
{
    const char  *str;
    char        buff[2];
    int         i;

    if( !TrieInit() )
        return( FALSE );

    if( !init_interminfo() )
        return( FALSE );

    str = getenv( "TERM" );
    /* attempt to adjust backspace with the terminfo definition */
    if( str != NULL && strncmp( str, "xterm", 5 ) == 0 ) {
        if( strcmp( key_backspace, "\x08" ) == 0 ) {
            uiwritec( "\x1b[?67h" );
        } else if( strcmp( key_backspace, "\x7f" ) == 0 ) {
            uiwritec( "\x1b[?67l" );
        }
    }

    buff[1] = '\0';
    for( i = 0; i < NUM_ELTS( InStandard ); ++i ) {
        buff[0] = InStandard[i].ch;
        if( !TrieAdd( InStandard[i].ev, buff ) ) {
            TrieFini();
            return( FALSE );
        }
    }

    for( i = 0; i < NUM_IN_TERM_INFO_MAPPINGS; ++i ) {
        str = InTerminfo[i].str;
        if( !TrieAdd( InTerminfo[i].ev, ( str ? str : "" ) ) ) {
            TrieFini();
            return( FALSE );
        }
    }
    return( TRUE );
}
