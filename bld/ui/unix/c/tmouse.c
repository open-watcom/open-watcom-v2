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


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#if defined( UNIX )

#ifndef HP
    #include <curses.h>
    #include <termio.h>
#else
    #include <stdarg.h>
    #include <curses.h>
#endif

#endif
#include <term.h>
#include "uidef.h"
#include "uimouse.h"
#include "trie.h"
#include "qdebug.h"

#include "uivirt.h"
#include "qnxuiext.h"
#include <time.h>

extern          void                    stopmouse(void);
extern          int                     nextc(int);
extern          char                    *GetTermType(void);

#define         MOUSE_SCALE             8

extern          ORD                     MouseRow;
extern          ORD                     MouseCol;
extern          bool                    MouseOn;

extern          unsigned short          MouseStatus;
extern          bool                    MouseInstalled;

static enum {
    M_NONE,
    M_XT        /* XTerm */
} MouseType;

#define MAXBUF    30
static char buf[ MAXBUF + 1 ];
static int new_sample;

#define ANSI_HDR        "\x1b["

#define XT_INIT "\033[?1000h"
#define XT_FINI "\033[?1000l"

static  void tm_error()
/*********************/
{
}

/*-
 * tm_check
 *   1- parse the mouse string
 *   2- Update externals if valid.
 *   3- set parameters to reflect it externals
 */

static int last_row, last_col, last_status;

/* Parse an xterm mouse event. */
static void XT_parse()
/********************/
{
    last_col = buf[1] - 0x21;
    last_row = buf[2] - 0x21;
    switch( buf[0] & 0x03 ) {
    case 0: last_status |= MOUSE_PRESS; break;
    case 1: last_status |= MOUSE_PRESS_MIDDLE; break;
    case 2: last_status |= MOUSE_PRESS_RIGHT; break;
    case 3: last_status = 0;
    }
}

static int tm_check( unsigned short *status, unsigned short *row,
                        unsigned short *col, unsigned long *the_time )
/**********************************************/
{

    if( !MouseInstalled ) {
         uisetmouse(*row, *col);
         return 0;
    }
    QNXDebugPrintf1("mouse_string = '%s'", buf);
    if( new_sample ) {
        switch( MouseType ) {
        case M_XT:
            XT_parse();
            break;
        case M_NONE:
            break;
        }
        new_sample = 0;
    }
    *row        = last_row;
    *col        = last_col;
    *status     = last_status;
    *the_time   = (long)time( NULL ) * 1000L;
    uisetmouse(*row, *col);
    return 0;
}

static int tm_stop()
/*********************/
{
    return 0;
}

static void TryOne( int type, char *test, char *init, char *input )
{
    unsigned short      row;
    unsigned short      col;

    MouseType = type;
    uimouseforceoff();
    write( UIConHandle, init, strlen( init ) );
    TrieAdd( EV_MOUSE_PRESS, input );

    MouseInstalled = TRUE;

    UIData->mouse_xscale = 1;
    UIData->mouse_yscale = 1;
    checkmouse( &MouseStatus, &row, &col, &MouseTime );
    MouseRow = row;
    MouseCol = col;
    stopmouse();
}

static int tm_init( bool install )
/******************************/
{
    char        *term;
    bool        kmous;                          // Does key_mouse exist?

    MouseInstalled      = FALSE;
    MouseType           = M_NONE;
    kmous               = ( key_mouse != NULL );

    if( !install ) return( FALSE );

    term = GetTermType();
    if( term != NULL && strstr( term, "xterm" ) != NULL ) {
        if( kmous ) {
            TryOne( M_XT, NULL, XT_INIT, key_mouse );
        } else {
            TryOne( M_XT, NULL, XT_INIT, ANSI_HDR "M" );
        }
        return( TRUE );
    }

    return( FALSE );
}

static int tm_fini()
/*********************/
{
    switch( MouseType ) {
        case M_XT:
            write( UIConHandle, XT_FINI, sizeof( XT_FINI ) - 1 );
            break;
        default :
            break;
    }
    return 0;
}

static int tm_set_speed( unsigned speed )
/****************************************/

/* Set speed of mouse. 1 is fastest; the higher the number the slower
 * it goes.
 * Since the mouse_param "gain" parameter is a "multiplier", a higher
 * number means faster. So, we've got to map from one scheme to another.
 * For now, 10 will be fastest and 1 will be slowest.
 */
{
    speed = speed;
    return 0;
}

void tm_saveevent()
/*****************/
{
    int i;
    int c;

    if( new_sample ) {
        tm_error();
        return;
    }
    switch( MouseType ) {
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
        default :
            break;
    }
    if( i == MAXBUF ) tm_error();
    buf[i+1] = '\0';
    new_sample = 1;
}

Mouse TermMouse = {
        tm_init,
        tm_fini,
        ( int (*) (int) )tm_set_speed,
        tm_stop,
        tm_check,
};
