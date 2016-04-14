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


#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <sys/kernel.h>
#include <sys/dev.h>
#include <sys/sidinfo.h>
#include <errno.h>
#include <sys/psinfo.h>
#include <sys/proxy.h>
#include <sys/qioctl.h>
#include <sys/console.h>
#include <fcntl.h>
#include <assert.h>
#include <term.h>
#include <ctype.h>
#include <signal.h>
#include "uidef.h"
#include "uishift.h"

#include "uivirt.h"
#include "qnxuiext.h"
#include "qdebug.h"
#include "ctkeyb.h"

#include "trie.h"
#include "tixparse.h"

enum {
    EV_STICKY_FUNC      = 0xff0,
    EV_STICKY_SHIFT,
    EV_STICKY_CTRL,
    EV_STICKY_ALT,
    EV_STICKY_INTRO     = 0xfff,
    S_FUNC              = S_CAPS,
    S_INTRO             = S_KANJI_LOCK
};

#define NUM_ELTS( a )   (sizeof( a ) / sizeof( a[0] ))

extern struct _console_ctrl *UIConCtrl;

static struct termios   SaveTermSet;
static int              SaveProtocol;
static int              SaveCtrl;
static pid_t            SavePGroup;

static unsigned short   ck_shift_state;
static unsigned short   sticky;
static unsigned short   real_shift;

static int              ck_fini( void );
static int              init_trie( void );

#define evmap( ev, f )  { EV_##ev, offsetof( struct _strs, f ) },

static const struct {
    EVENT               ev;
    unsigned short      offset;
} InTerminfo[] = {
    evmap( RUB_OUT,             _ky_backspace )
    evmap( RUB_OUT,             _ky_clear )
    evmap( DELETE,              _ky_dc )
    evmap( CURSOR_DOWN,         _ky_down )
    evmap( INSERT,              _ky_ic )
    evmap( F1,                  _ky_f1 )
    evmap( F2,                  _ky_f2 )
    evmap( F3,                  _ky_f3 )
    evmap( F4,                  _ky_f4 )
    evmap( F5,                  _ky_f5 )
    evmap( F6,                  _ky_f6 )
    evmap( F7,                  _ky_f7 )
    evmap( F8,                  _ky_f8 )
    evmap( F9,                  _ky_f9 )
    evmap( F10,                 _ky_f10 )
    evmap( F11,                 _ky_f11 )
    evmap( F12,                 _ky_f12 )
    evmap( SHIFT_F1,            _ky_f13 )
    evmap( SHIFT_F2,            _ky_f14 )
    evmap( SHIFT_F3,            _ky_f15 )
    evmap( SHIFT_F4,            _ky_f16 )
    evmap( SHIFT_F5,            _ky_f17 )
    evmap( SHIFT_F6,            _ky_f18 )
    evmap( SHIFT_F7,            _ky_f19 )
    evmap( SHIFT_F8,            _ky_f20 )
    evmap( SHIFT_F9,            _ky_f21 )
    evmap( SHIFT_F10,           _ky_f22 )
    evmap( SHIFT_F11,           _ky_f23 )
    evmap( SHIFT_F12,           _ky_f24 )
    evmap( CTRL_F1,             _ky_f25 )
    evmap( CTRL_F2,             _ky_f26 )
    evmap( CTRL_F3,             _ky_f27 )
    evmap( CTRL_F4,             _ky_f28 )
    evmap( CTRL_F5,             _ky_f29 )
    evmap( CTRL_F6,             _ky_f30 )
    evmap( CTRL_F7,             _ky_f31 )
    evmap( CTRL_F8,             _ky_f32 )
    evmap( CTRL_F9,             _ky_f33 )
    evmap( CTRL_F10,            _ky_f34 )
    evmap( CTRL_F11,            _ky_f35 )
    evmap( CTRL_F12,            _ky_f36 )
    evmap( ALT_F1,              _ky_f37 )
    evmap( ALT_F2,              _ky_f38 )
    evmap( ALT_F3,              _ky_f39 )
    evmap( ALT_F4,              _ky_f40 )
    evmap( ALT_F5,              _ky_f41 )
    evmap( ALT_F6,              _ky_f42 )
    evmap( ALT_F7,              _ky_f43 )
    evmap( ALT_F8,              _ky_f44 )
    evmap( ALT_F9,              _ky_f45 )
    evmap( ALT_F10,             _ky_f46 )
    evmap( ALT_F11,             _ky_f47 )
    evmap( ALT_F12,             _ky_f48 )
    evmap( HOME,                _ky_home )
    evmap( CURSOR_LEFT,         _ky_left )
    evmap( PAGE_DOWN,           _ky_npage )
    evmap( PAGE_UP,             _ky_ppage )
    evmap( CURSOR_RIGHT,        _ky_right )
    evmap( SCROLL_LINE_DOWN,    _ky_sf )
    evmap( SCROLL_LINE_UP,      _ky_sr )
    evmap( CURSOR_UP,           _ky_up )
    evmap( HOME,                _ky_beg )
    evmap( ESCAPE,              _ky_cancel )
    evmap( END,                 _ky_end )
    evmap( ENTER,               _ky_enter )
    evmap( TAB_FORWARD,         _ky_next )
    evmap( TAB_BACKWARD,        _ky_previous )
    evmap( SHIFT_HOME,          _ky_sbeg )
    evmap( SHIFT_END,           _ky_send )
    evmap( SHIFT_CURSOR_LEFT,   _ky_sleft )
    evmap( TAB_BACKWARD,        _ky_snext )
    evmap( TAB_FORWARD,         _ky_sprevious )
    evmap( SHIFT_CURSOR_RIGHT,  _ky_sright )
    evmap( TAB_BACKWARD,        _ky_btab )
};

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
    { EV_TAB_FORWARD, EV_TAB_BACKWARD, EV_CTRL_TAB, EV_TAB_FORWARD },
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
    ck_shift_state = 0;
    real_shift = 0;
}

static int ck_unevent( EVENT ev )
/*******************************/

// Somebody wants us to pretend that the specified event has occurred
// (one of EV_SHIFT/CTRL/ALT_RELEASE) so that the corresponding press
// event will be generated for the next keystroke (if that shift key
// is pressed).

{
    ev = ev;
    return( 0 );
}

void intern ck_arm( void )
/************************/
{
    /*
        Yes I know that this can be done in the dev_read call, but there
        are enough weird paths dealing with mouse events under QNX windows
        that we've got problems where sometimes the arming of the proxy
        would not happen. This way, we *know* that the proxy is going to
        be armed for the keyboard (and mouse under QNX windows). If only
        there was a mouse_arm() function. Sigh.
    */
    dev_arm( UIConHandle, UIRemProxy, _DEV_EVENT_INPUT );
}


#define PUSHBACK_SIZE   32

static char     UnreadBuffer[ PUSHBACK_SIZE ];
static int      UnreadPos = sizeof( UnreadBuffer );

int nextc(int n)
/**************/
{
    unsigned char       ch;
    unsigned            least;

    if( UnreadPos < sizeof( UnreadBuffer ) )
        return( UnreadBuffer[ UnreadPos++ ] );

    least = (n != 0) ? 1 : 0;

    dev_arm( UIConHandle, _DEV_DISARM, _DEV_EVENT_INPUT );
    if (dev_read( UIConHandle, &ch, 1, least ,0, n, 0, 0 ) <= 0 ) {
            return -1;
    }
    return ch;
}

void nextc_unget( unsigned char *str, int n )
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

    ev = TrieRead();
    if( sticky & S_INTRO ) {
        switch( ev ) {
        case 'f':
        case 'F':
            ev = EV_STICKY_FUNC;
            sticky &= ~S_INTRO;
            break;
        case 's':
        case 'S':
            ev = EV_STICKY_SHIFT;
            sticky &= ~S_INTRO;
            break;
        case 'c':
        case 'C':
            ev = EV_STICKY_CTRL;
            sticky &= ~S_INTRO;
            break;
        case 'a':
        case 'A':
            ev = EV_STICKY_ALT;
            sticky &= ~S_INTRO;
            break;
        }
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
    case EV_STICKY_INTRO:
        sticky ^= S_INTRO;
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
        if( (real_shift & S_SHIFT) == 0 )
            ev = EV_NO_EVENT;
        real_shift &= ~S_SHIFT;
        break;
    case EV_CTRL_RELEASE:
        if( (real_shift & S_CTRL) == 0 )
            ev = EV_NO_EVENT;
        real_shift &= ~S_CTRL;
        break;
    case EV_ALT_RELEASE:
        if( (real_shift & S_ALT) == 0 )
            ev = EV_NO_EVENT;
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
        if( (real_shift & S_CTRL) == 0 ) {
            /*
                If the ctrl key isn't down (won't ever be on a terminal)
                then we want to see certain CTRL-? combinations come back
                as some standard UI events.
            */
            switch( ev ) {
            case '\x08':
                ev = EV_RUB_OUT;
                break;
            case '\x09':
                ev = EV_TAB_FORWARD;
                break;
            case '\x0c':
                ev = EV_REDRAW_SCREEN;
                break;
            }
        }
        ck_shift_state = sticky | real_shift;
        sticky = 0;
        #define S_MASK  (S_SHIFT|S_CTRL|S_ALT)
        if( ck_shift_state & S_MASK ) {
            search_ev = tolower( ev );
            entry = bsearch( &search_ev, ShiftMap, NUM_ELTS( ShiftMap ),
                                sizeof( ShiftMap[0] ), find_entry );
            if( entry != NULL ) {
                if( ck_shift_state & S_SHIFT ) {
                    ev = entry->shift;
                } else if( ck_shift_state & S_CTRL ) {
                    ev = entry->ctrl;
                } else { /* must be ALT */
                    ev = entry->alt;
                }
            }
        }
        QNXDebugPrintf1( "UI: Something read: %4.4X", ev );
        return( ev );
    }
    ck_shift_state = real_shift;
    QNXDebugPrintf1( "UI: Something read: %4.4X", ev );
    return( ev );
}

EVENT tk_keyboardevent( void )
{
    EVENT       ev;

    ev = ck_keyboardevent();
    if( ev != EV_MOUSE_PRESS )
        return( ev );
    QNXDebugPrintf0( "UI: Mouse event handling" );
    tm_saveevent();
    return( EV_NO_EVENT ); /* make UI check for mouse events */
}

static int ck_stop( void )
{
    dev_read( UIConHandle, NULL, 0, 0, 0, 0, 0, 0 );
    while( Creceive( UILocalProxy, 0, 0 ) > 0 )
        {}
    return 0;
}


static int ck_flush( void )
/*************************/
{
    tcflush( UIConHandle, TCIFLUSH );
    return 0;
}

static int ck_shift_state( void )
/*******************************/
{
    return( ck_shift_state );
}

static int ck_restore( void )
/***************************/
{
    struct termios  new;

    new = SaveTermSet;
    new.c_iflag &= ~(IXOFF | IXON);
    new.c_oflag &= ~OPOST;
    new.c_lflag &= ~(ECHO | ICANON | NOFLSH);
    new.c_lflag |= ISIG;
    if( UIConCtrl != NULL ) {
        /* We have an honest-to-god console. Disable Ctrl/C - use Ctrl/Break */
        new.c_cc[VINTR] = '\0';
    }
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr( UIConHandle, TCSADRAIN, &new );
    if( UIConCtrl != NULL ) {
        console_protocol( UIConCtrl, 0, _CON_PROT_QNX4 );
        console_ctrl( UIConCtrl, 0, CONSOLE_SCANMODE, CONSOLE_SCANMODE );
    }
    return 0;
}

static void term_handler( int signo )
{
    signo = signo;
    ck_fini();
    _exit( 0 );
}

/* We want to ignore the following escapes when running on a console */
#include "conesc.gh"

#if 0
static const char * const ConIgnore[] = {
    "\xff\x20",                 /* extended key follows */
    "\xff\x21",                 /* key up */
    "\xff\x28",                 /* E0 code */
    "\xff\x29",                 /* right shift follows */
    "\xff\x28\xff\x23",         /* fake shift down */
    "\xff\x28\xff\x22",         /* fake shift up */
    "\xff\x28\xff\x29\xff\x23", /* fake right shift down */
    "\xff\x28\xff\x29\xff\x22", /* fake right shift up */
};
#endif

static int ck_init( void )
/************************/
{
    unsigned    i;
    EVENT       ev;

    tcgetattr( UIConHandle, &SaveTermSet );

    if( !init_trie() )
        return( false );

    switch( ti_read_tix( UIConCtrl == NULL ) ) {
    case TIX_FAIL:
        return( false );
    case TIX_NOFILE:
        if( UIConCtrl != NULL ) {
            for( i = 0; i < NUM_ELTS( ConEscapes ); i += strlen( ConEscapes + i ) + 1 ) {
                ev = ConEscapes[i + 0] + (ConEscapes[i + 1] << 8);
                i += 2;
                if( !TrieAdd( ev, ConEscapes + i ) ) {
                    return( false );
                }
            }
        }
    }

    if( UIConCtrl != NULL ) {
        SaveProtocol = console_protocol( UIConCtrl, 0, _CON_PROT_QNX4 );
        SaveCtrl = console_ctrl( UIConCtrl, 0, CONSOLE_SCANMODE, CONSOLE_SCANMODE );
    }
    SavePGroup = tcgetpgrp( UIConHandle );
    tcsetpgrp( UIConHandle, UIPGroup );
    restorekeyb();
    signal( SIGTERM, &term_handler );
    return( true );
}


static int ck_fini( void )
/************************/
{
    savekeyb();
    tcsetpgrp( UIConHandle, SavePGroup );
    signal( SIGTERM, SIG_DFL );
    return 0;
}

static int ck_save( void )
{
    tcsetattr( UIConHandle, TCSADRAIN, &SaveTermSet );
    if( UIConCtrl != NULL ) {
        console_protocol( UIConCtrl, 0, SaveProtocol );
        console_ctrl( UIConCtrl, 0, SaveCtrl, CONSOLE_SCANMODE );
        /* There might be a KEY-UP event stuck in the buffer */
        tcflush( UIConHandle, TCIFLUSH );
    }
    return 0;
}

static int init_trie( void )
{
    charoffset  *coffs;         // start of char-offset table
    char        buff[2];
    int         i;

    if( !TrieInit() )
        return( false );

    buff[1] = '\0';
    for( i = 0; i < NUM_ELTS( InStandard ); ++i ) {
        buff[0] = InStandard[i].ch;
        if( !TrieAdd( InStandard[i].ev, buff ) ) {
            TrieFini();
            return( false );
        }
    }
    for( i = 0; i < NUM_ELTS( InTerminfo ); ++i ) {
        coffs = (charoffset *)&__cur_term->_strs;
        coffs = (void *)((char *)coffs + InTerminfo[i].offset );
        if( !TrieAdd( InTerminfo[i].ev, __cur_term->_strtab + *coffs ) ) {
            TrieFini();
            return( false );
        }
    }
    return( true );
}

Keyboard ConsKeyboard = {
        ck_init,
        ck_fini,
        ck_arm,
        ck_save,
        ck_restore,
        ck_flush,
        ck_stop,
        ck_shift_state,
        ck_unevent
};
