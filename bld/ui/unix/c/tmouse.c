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
* Description:  Terminal mouse handling.
*
****************************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#if defined( __UNIX__ )

#ifndef HP
    #include <curses.h>
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
#include "unxuiext.h"
#include "ctkeyb.h"
#include <time.h>
#ifdef __LINUX__
#include <sys/socket.h>
#include <sys/un.h>
#endif


#ifdef __LINUX__
static void             GPM_parse( void );
#endif

#define MOUSE_SCALE     8

extern MOUSEORD         MouseRow;
extern MOUSEORD         MouseCol;

extern unsigned short   MouseStatus;
extern bool             MouseInstalled;

static enum {
    M_NONE,
    M_XT,       /* XTerm */
#ifdef __LINUX__
    M_GPM       /* GPM   */
#endif
} MouseType;

#define MAXBUF    30
static char buf[ MAXBUF + 1 ];
static int  new_sample;
int         UIMouseHandle = -1;

#define ANSI_HDR        "\x1b["

#define XT_INIT "\033[?1000h"
#define XT_FINI "\033[?1000l"

static  void tm_error( void )
/***************************/
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
static void XT_parse( void )
/**************************/
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

static int tm_check( unsigned short *status, MOUSEORD *row, MOUSEORD *col, unsigned long *the_time )
/**************************************************************************************************/
{
    if( !MouseInstalled ) {
         uisetmouse( *row, *col );
         return 0;
    }
    UIDebugPrintf1( "mouse_string = '%s'", buf );
    if( new_sample ) {
        switch( MouseType ) {
        case M_XT:
            XT_parse();
            break;
#ifdef __LINUX__
        case M_GPM:
            GPM_parse();
            break;
#endif
        case M_NONE:
            break;
        }
        new_sample = 0;
    }
    *row        = last_row;
    *col        = last_col;
    *status     = last_status;
    *the_time   = (long)time( NULL ) * 1000L;
    uisetmouse( *row, *col );
    return 0;
}

static int tm_stop( void )
/************************/
{
    return 0;
}

static void TryOne( int type, char *test, char *init, char *input )
{
    MOUSEORD    row;
    MOUSEORD    col;

    MouseType = type;
    uimouseforceoff();
    uiwrite( init );
    TrieAdd( EV_MOUSE_PRESS, input );

    MouseInstalled = true;

    UIData->mouse_xscale = 1;
    UIData->mouse_yscale = 1;
    checkmouse( &MouseStatus, &row, &col, &MouseTime );
    MouseRow = row;
    MouseCol = col;
    stopmouse();
}

#ifdef __LINUX__
/* this structure is a mess with respect to binary compatibility:
   old GPM versions did not have wdx and wdy (for wheel mice).
   Patched versions  have it in the middle. Official 1.20.1 has them
   at the end. We assume the old structure, and if we encounter a 4-byte
   read or an invalid "type" it must be one of the new varieties
*/
static struct {
    unsigned char   button;
    unsigned char   modifiers;
    unsigned short  vc;
    short           dx, dy, x, y;
    union {
        struct {
            int     type, clicks, margin;
            short   wdx, wdy;
        } gpm_w1;
        struct {
            short   wdx, wdy;
            int     type, clicks, margin;
        } gpm_w2;
    } tail;
} gpm_buf;

#define GPM_DRAG     2
#define GPM_DOWN     4
#define GPM_UP       8
#define GPM_HARD   256

#define GPM_B_LEFT   4
#define GPM_B_MIDDLE 2
#define GPM_B_RIGHT  1

/* Parse a GPM mouse event. */
static void GPM_parse( void )
/***************************/
{
    int type;
    static int variety = 0;
    last_col = gpm_buf.x - 1;
    last_row = gpm_buf.y - 1;
    /* clip */
    if( last_col < 0 ) last_col = 0;
    if( last_col >= UIData->width ) last_col = UIData->width - 1;
    if( last_row < 0 ) last_row = 0;
    if( last_row >= UIData->height ) last_row = UIData->height - 1;
    type = gpm_buf.tail.gpm_w1.type & 0xf;
    if( variety == 0 ) {
        if( type == GPM_DRAG || type == GPM_DOWN || type == GPM_UP )
            variety = 1;
        else
            variety = 2;
    }
    if( variety == 2 )
        type = gpm_buf.tail.gpm_w2.type & 0xf;
    if( type == GPM_DOWN ) {
        if( gpm_buf.button & GPM_B_LEFT )
            last_status |= MOUSE_PRESS;
        if( gpm_buf.button & GPM_B_MIDDLE )
            last_status |= MOUSE_PRESS_MIDDLE;
        if( gpm_buf.button & GPM_B_RIGHT )
            last_status |= MOUSE_PRESS_RIGHT;
    } else if( type == GPM_UP ) {
        if( gpm_buf.button & GPM_B_LEFT )
            last_status &= ~MOUSE_PRESS;
        if( gpm_buf.button & GPM_B_MIDDLE )
            last_status &= ~MOUSE_PRESS_MIDDLE;
        if( gpm_buf.button & GPM_B_RIGHT )
            last_status &= ~MOUSE_PRESS_RIGHT;
    }
}

static int gpm_tm_init( void )
/****************************/
{
    struct {
        unsigned short  eventMask;
        unsigned short  defaultMask;
        unsigned short  minMod;
        unsigned short  maxMod;
        int             pid;
        int             vc;
    } gpm_conn;
    struct sockaddr_un  sau;
    char                tty_name[20];
    char                procname[30];
    int                 len, mult;

    UIMouseHandle = socket( PF_UNIX, SOCK_STREAM, 0 );
    if( UIMouseHandle < 0 )
        return( false );
    sau.sun_family = AF_UNIX;
    strcpy( sau.sun_path, "/dev/gpmctl" );
    if( connect( UIMouseHandle, (struct sockaddr *)&sau, sizeof sau ) < 0 )
        goto out;
    gpm_conn.eventMask = GPM_DRAG | GPM_UP | GPM_DOWN;
    gpm_conn.defaultMask = ~GPM_HARD;
    gpm_conn.pid = getpid();
    gpm_conn.vc = gpm_conn.minMod = gpm_conn.maxMod = 0;
    sprintf( procname, "/proc/self/fd/%d", UIConHandle );
    len = readlink( procname, tty_name, sizeof( tty_name ) - 1 );
    if( len < 0 )
        goto out;
    if( memcmp( tty_name, "/dev/tty", len ) == 0 ) {
        len = readlink( "/proc/self/fd/0", tty_name, sizeof( tty_name ) - 1 );
        if( len < 0 ) {
            goto out;
        }
    }
    len--;
    mult = 1;
    while( len && tty_name[len] >= '0' && tty_name[len] <= '9' ) {
        gpm_conn.vc += ( tty_name[len] - '0' ) * mult;
        len--;
        mult *= 10;
    }
    write( UIMouseHandle, &gpm_conn, sizeof gpm_conn );
    TryOne( M_GPM, NULL, "", ANSI_HDR "M" );
    MouseType = M_GPM;
    return( true );
 out:
    close( UIMouseHandle );
    UIMouseHandle = -1;
    return( false );
}
#endif

static int tm_init( int install )
/*******************************/
{
    bool        kmous;                          // Does key_mouse exist?

    MouseInstalled  = false;
    MouseType       = M_NONE;
    kmous           = ( key_mouse != NULL );

    if( install == 0 )
        return( false );

    if( strstr( GetTermType(), "xterm" ) != NULL ) {
        if( kmous ) {
            TryOne( M_XT, NULL, XT_INIT, key_mouse );
        } else {
            TryOne( M_XT, NULL, XT_INIT, ANSI_HDR "M" );
        }
        return( true );
    }

#ifdef __LINUX__
    return( gpm_tm_init() );
#else
    return( false );
#endif
}

static int tm_fini( void )
/************************/
{
    switch( MouseType ) {
    case M_XT:
        uiwritec( XT_FINI );
        break;
#ifdef __LINUX__
    case M_GPM:
        close( UIMouseHandle );
        break;
#endif
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
        case M_XT:
            for( i = 0; i < 3; ++i ) {
                c = nextc( 10 );
                if( c == -1 ) {
                    tm_error();
                    return;
                }
                buf[i] = c;
            }
            if( i == MAXBUF ) tm_error();
            buf[i+1] = '\0';
            break;
#ifdef __LINUX__
        case M_GPM: {
            /* start with the old gpm structure without wdx/wdy */
            static size_t gpm_buf_size = sizeof gpm_buf - 4;
            i = read( UIMouseHandle, &gpm_buf, gpm_buf_size );
            if( i < gpm_buf_size ) {
                if( i == 4 )
                    gpm_buf_size = 28;
                tm_error();
                return;
            }
            break;
        }
#endif
        default :
            break;
    }
    new_sample = 1;
}

Mouse TermMouse = {
    tm_init,
    tm_fini,
    (int (*)(int))tm_set_speed,
    tm_stop,
    tm_check,
};
