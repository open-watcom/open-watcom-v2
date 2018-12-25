/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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


#include <unistd.h>
#include <string.h>
#include <sys/proxy.h>
#include <sys/types.h>
#include <sys/timers.h>
#include <sys/kernel.h>
#include <sys/osinfo.h>
#include <sys/dev.h>
#include <i86.h>
#include "uidef.h"
#include "uimouse.h"
#include "trie.h"
#include "qdebug.h"
#include "uivirts.h"
#include "uiextrn.h"
#include "ctkeyb.h"


#define MOUSE_SCALE         8

#define MAXBUF              30

#define QW_BELL             "\x07"

#define QNX_HDR             _ESC "/"
#define ANSI_HDR            _ESC "["

#define QW_TEST             "10t"
#define QW_TEST_RESPONSE    "10;"
#define QW_INIT             ">1;6;7;8h" _ESC "/>9l"
#define QW_FINI             ">1;6;7;8l"

#define XT_INIT             _ESC "[?1000h"
#define XT_FINI             _ESC "[?1000l"

static enum {
    M_NONE,
    M_QW,       /* QNX mode QNX Windows/Photon */
    M_AW,       /* ANSI mode QNX Windows/Photon */
    M_XT        /* XTerm */
} MouseType;

static char         buf[MAXBUF + 1];
static int          new_sample;
static int          last_row;
static int          last_col;
static MOUSESTAT    last_status;

static  void tm_error( void )
/***************************/
{
//    uiwritec( QW_BELL );
}

/*-
 * tm_check
 *   1- parse the mouse string
 *   2- Update externals if valid.
 *   3- set parameters to reflect it externals
 */

/* Parse a QNX Windows/Photon mouse event. */
static void QW_parse( void )
{
    int func, mrow, mcol, mbut, mclick;
    char *p = buf;

    mclick = 1;
    if( (func = strtol( p, &p, 10 )) > 3276 || *p != ';' )
        return;
    if( (mrow = strtol( p + 1, &p, 10 )) > 3276 || *p != ';' )
        return;
    if( (mcol = strtol( p + 1, &p, 10 )) > 3276 || *p != ';' )
        return;
    mbut = strtol( p + 1, &p, 10 );
    if( *p == ';' ) {
        mclick = strtol( p + 1, &p, 10 );
    }
    if( *p != 't' )
        return;
    last_row = mrow;
    last_col = mcol;
    /*
       The func is actually 31, 32, etc, but the leading "3" was swallowed
       while recognizing that we had a mouse event
    */
    switch( func ) {
    case 1:
        last_status = 0;
        if( mbut & 004 )
            last_status |= UI_MOUSE_PRESS;
        if( mbut & 002 )
            last_status |= UI_MOUSE_PRESS_MIDDLE;
        if( mbut & 001 )
            last_status |= UI_MOUSE_PRESS_RIGHT;
        break;
    case 2:
        last_status = 0;
        break;
    }
}

/* Parse an xterm mouse event. */
static void XT_parse( void )
{
    last_col = buf[1] - 0x21;
    last_row = buf[2] - 0x21;
    switch( buf[0] & 0x03 ) {
    case 0: last_status |= UI_MOUSE_PRESS; break;
    case 1: last_status |= UI_MOUSE_PRESS_MIDDLE; break;
    case 2: last_status |= UI_MOUSE_PRESS_RIGHT; break;
    case 3: last_status = 0;
    }
}


static bool tm_check( MOUSESTAT *status, MOUSEORD *row, MOUSEORD *col, MOUSETIME *time )
/**************************************************************************************/
{
    if( MouseInstalled ) {
        QNXDebugPrintf1("mouse_string = '%s'", buf);
        if( new_sample ) {
            switch( MouseType ) {
            case M_QW:
            case M_AW:
                QW_parse();
                break;
            case M_XT:
                XT_parse();
                break;
            }
            new_sample = 0;
        }
        *row = last_row;
        *col = last_col;
        *status = last_status;
        *time = uiclock();
    }
    uisetmouse( *row, *col );
    return( false );
}

static int tm_stop( void )
/************************/
{
    return( 0 );
}

static void DoMouseInit( int type, const char *init, const char *input )
{
    struct _osinfo      osinfo;
    MOUSEORD            row;
    MOUSEORD            col;

    MouseInstalled = true;
    MouseType = type;

    uimouseforceoff();
    uiwrite( init );
    TrieAdd( EV_MOUSE_PRESS, input );

    UIData->mouse_xscale = 1;
    UIData->mouse_yscale = 1;

    qnx_osinfo( 0, &osinfo );
    _SysTime = (struct _timesel __far *)MK_FP( osinfo.timesel, 0 );

    checkmouse( &MouseStatus, &row, &col, &MouseTime );
    MouseRow = row;
    MouseCol = col;
    _stopmouse();
}

static bool tm_init( init_mode install )
/**************************************/
{
    char    *term;

    MouseInstalled = false;
    MouseType = M_NONE;

    if( install == INIT_MOUSELESS )
        return( false );

    term = GetTermType();
    if( strcmp( term, "xterm" ) == 0 ) {
        DoMouseInit( M_XT, XT_INIT, ANSI_HDR "M" );
    } else if( strstr( term, "qnx" ) != 0 ) {
        uiwritec( QNX_HDR QW_TEST );
        TrieAdd( EV_MOUSE_PRESS, QNX_HDR QW_TEST_RESPONSE );
    } else {
        uiwritec( ANSI_HDR QW_TEST );
        TrieAdd( EV_MOUSE_PRESS, ANSI_HDR QW_TEST_RESPONSE );
    }
    return( true );
}

static bool tm_fini( void )
/*************************/
{
    switch( MouseType ) {
    case M_QW:
        uiwritec( QNX_HDR QW_FINI );
        break;
    case M_AW:
        uiwritec( ANSI_HDR QW_FINI );
        break;
    case M_XT:
        uiwritec( XT_FINI );
        break;
    }
    return( false );
}

static int tm_set_speed( unsigned speed )
/***************************************/

/* Set speed of mouse. 1 is fastest; the higher the number the slower
 * it goes.
 * Since the mouse_param "gain" parameter is a "multiplier", a higher
 * number means faster. So, we've got to map from one scheme to another.
 * For now, 10 will be fastest and 1 will be slowest.
 */
{
    /* unused parameters */ (void)speed;

    return( 0 );
}

static int tm_wait_mouse( void )
/******************************/
{
    return( -1 );
}

void tm_saveevent( void )
/***********************/
{
    int i;
    int c;

    if( new_sample ) {
        tm_error();
        return;
    }
    switch( MouseType ) {
    case M_NONE:
        /* eat the remainder of the version ID response. */
        for( ;; ) {
            c = nextc( 20 );
            if( c == -1 )
                return;
            if( c == 't' ) {
                break;
            }
        }
        if( strstr( GetTermType(), "qnx" ) != 0 ) {
            DoMouseInit( M_QW, QNX_HDR QW_INIT, QNX_HDR "3" );
        } else {
            DoMouseInit( M_AW, ANSI_HDR QW_INIT, ANSI_HDR "3" );
        }
        return;
    case M_QW:
    case M_AW:
        for( i = 0; i < MAXBUF; ++i ) {
            c = nextc( 20 );
            if( c == -1 ) {
               tm_error();
               return;
            }
            buf[i] = c;
            if( buf[i] == 't' )
                break;
            if( buf[i] == _ESC_CHAR ) {
                tm_error();
                c = nextc( 20 );
                if( c == -1 ) {
                    tm_error();
                    return;
                }
                i = 0;
            }
        }
        break;
    case M_XT:
        for( i = 0; i < 3; ++i ) {
            c = nextc( 10 );
            if( c == -1 ) {
                tm_error();
                return;
            }
            buf[i] = c;
        }
        break;
    }
    if( i == MAXBUF )
        tm_error();
    buf[i + 1] = '\0';
    new_sample = 1;
}

Mouse TermMouse = {
    tm_init,
    tm_fini,
    tm_set_speed,
    tm_stop,
    tm_check,
    tm_wait_mouse
};
