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
#include <stdio.h>
#include <fcntl.h>
#include <sys/seginfo.h>
#include <sys/console.h>
#include <sys/con_msg.h>
#include <sys/dev.h>
#include <sys/osinfo.h>
#include <sys/sidinfo.h>
#include <sys/psinfo.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/proxy.h>
#include <sys/vc.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <i86.h>
#include "uidef.h"

#include "uivirt.h"
#include "qnxuiext.h"
#include "ctkeyb.h"

static          MONITOR                 ui_data         =       {
                25,
                80,
                M_CGA,
                NULL,
                NULL,
                NULL,
                NULL,
                4,
                1
};

struct _console_ctrl *UIConCtrl = NULL;

bool ConsCheck( void )
{
    if( UIConCtrl == NULL )
        return( false );
    UIDisableShiftChanges = false;
    return( true );
}

bool QnxWCheck( void )
{
    if( !ConsCheck() )
        return( false );
    return( strcmp( GetTermType(), "qnxw" ) == 0 );
}


static bool setupscrnbuff( void )
/*******************************/
{
    int                 rows, cols;
    LP_PIXEL            scrn;
    size_t              num;
    int                 i;

    if( console_size( UIConCtrl, UIConsole, 0, 0, &rows, &cols ) != 0 ) {
        return( false );
    }
    UIData->width = cols;
    UIData->height = rows;
    num = UIData->width * UIData->height * 2;
    scrn = UIData->screen.origin;
#if defined( __386__ )
    scrn = realloc( scrn, num );
    if( scrn == NULL )
        return( false );
#else
    {
        unsigned                seg;

        if( scrn == NULL ) {
            seg = qnx_segment_alloc( num );
        } else {
            seg = qnx_segment_realloc( FP_SEG( scrn ), num );
        }
        if( seg == -1 )
            return( false );
        scrn = MK_FP( seg, 0 );
    }
#endif
    num /= 2;
    for( i = 0; i < num; ++i ) {
        scrn[i].ch = ' ';       /* a space with normal attributes */
        scrn[i].attr = 7;       /* a space with normal attributes */
    }
    UIData->screen.origin = scrn;
    UIData->screen.increment = UIData->width;
    return( true );
}

static volatile int     StatePending;

static void state_handler( int signo )
/************************************/
{
    signo = signo;
    StatePending = 1;
}


static EVENT cd_sizeevent( void )
/*******************************/
{
    SAREA       area;
    unsigned    state;
    unsigned    arm;

    if( !StatePending )
        return( EV_NO_EVENT );
    StatePending = 0;
    state = console_state( UIConCtrl, UIConsole, 0,
                (_CON_EVENT_ACTIVE|_CON_EVENT_INACTIVE|_CON_EVENT_SIZE) );
    arm = 0;
    if( state & _CON_EVENT_INACTIVE ) {
        clear_shift();
        arm = _CON_EVENT_ACTIVE;
    } else if( state & _CON_EVENT_ACTIVE ) {
        arm = _CON_EVENT_INACTIVE;
    }
    console_arm( UIConCtrl, UIConsole, 0, arm | _CON_EVENT_SIZE );
    if( !(state & _CON_EVENT_SIZE) )
        return( EV_NO_EVENT );
    if( !uiinlist( EV_BACKGROUND_RESIZE ) )
        return( EV_NO_EVENT );
    if( !setupscrnbuff() )
        return( EV_NO_EVENT );
    closebackground();
    openbackground();
    area.row = 0;
    area.col = 0;
    area.height = UIData->height;
    area.width = UIData->width;
    uidirty( area );
    return( EV_BACKGROUND_RESIZE );
}

bool intern initmonitor( void )
/*****************************/
{
    struct _osinfo      info;

    if( UIConCtrl == NULL )
        return( false );

    qnx_osinfo( UIConNid, &info );
    switch( info.primary_monitor ) {
    case _MONITOR_PGS:
    case _MONITOR_CGA:
    case _MONITOR_PS30_COLOR:
        UIData->colour = M_CGA;
        break;
    case _MONITOR_EGA_COLOR:
        UIData->colour = M_EGA;
        break;
    case _MONITOR_VGA_COLOR:
        UIData->colour = M_VGA;
        break;
    case _MONITOR_EGA_MONO:
    case _MONITOR_VGA_MONO:
    case _MONITOR_PS30_MONO:
        UIData->colour = M_BW;
        break;
    default:
        UIData->colour = M_MONO;
        break;
    }

    /* notify if screen size changes */
    signal( SIGDEV, &state_handler );
    console_arm( UIConCtrl, UIConsole, 0,
            _CON_EVENT_SIZE|_CON_EVENT_ACTIVE|_CON_EVENT_SIZE );

    return( true );
}


/* update the physical screen with contents of virtual copy */

static void my_console_write( struct _console_ctrl *cc, int console, unsigned offset,
         unsigned char __FAR *buf, int nbytes, int row, int col, int type)
{
        struct _mxfer_entry sx[2];
        struct _mxfer_entry rx;
        union _console_msg {
            struct _console_write           write;
            struct _console_write_reply     write_reply;
        } msg;

        msg.write.type = _CONSOLE_WRITE;
        msg.write.handle = cc->handle;
        msg.write.console = console;
        msg.write.curs_row = row;
        msg.write.curs_col = col;
        msg.write.curs_type = type;
        msg.write.offset = offset;
        msg.write.nbytes = nbytes;

        _setmx( &sx[1], buf, nbytes );
        _setmx( &sx[0], &msg.write, sizeof(msg.write)-sizeof(msg.write.data) );

        _setmx( &rx, &msg.write_reply, sizeof( msg.write_reply ) );

        Sendmx(cc->driver, 2, 1, sx, &rx );
}


static int cd_init( void )
/************************/
{
    int                 initialized;

    initialized = FALSE;
    if( UIData == NULL ) {
        UIData = &ui_data;
    }
    if( !initmonitor() )
        return( false );
    if( !setupscrnbuff() )
        return( false );

    uiinitcursor();
    initkeyboard();
    UIData->mouse_acc_delay = 277;
    UIData->mouse_rpt_delay = 100;
    UIData->mouse_clk_delay = 277;
    UIData->tick_delay      = 500;
    UIData->f10menus        = true;
    return( true );
}


static int cd_fini( void )
/************************/
{
    finikeyboard();
    uifinicursor();
    console_close( UIConCtrl );
    return( false );
}

static int cd_update( SAREA *area )
/*********************************/
{
    unsigned short  offset; /* pixel offset into buffer to begin update at */
    unsigned short  count;  /* number of pixels to update */
    int     row, col, type, i;

    row = UIData->cursor_row;
    col = UIData->cursor_col;
    switch( UIData->cursor_type ) {
    case C_OFF:     type = CURSOR_OFF;          break;
    case C_NORMAL:  type = CURSOR_UNDERLINE;    break;
    case C_INSERT:  type = CURSOR_BLOCK;        break;
    }
    if( area == NULL ) {
        my_console_write( UIConCtrl, UIConsole, 0,
                        (LP_STRING)UIData->screen.origin, 0,
                        row, col, type );
    } else {
        count = area->width * sizeof( PIXEL );
        for( i = area->row; i < (area->row + area->height); i++ ) {
            offset = ( i * UIData->width + area->col ) * sizeof( PIXEL );
            my_console_write( UIConCtrl, UIConsole, offset,
                            offset + (LP_STRING)UIData->screen.origin, count,
                            row, col, type );
        }
    }
    return( 0 );
}

static int cd_refresh(int must)
{
    must = must;
    return( 0 );
}

static int cd_getcur( ORD *row, ORD *col, int *type, int *attr )
/**************************************************************/
{
    *row = UIData->cursor_row;
    *col = UIData->cursor_col;
    *type = UIData->cursor_type;
    *attr = 0;
    return( 0 );
}


static int cd_setcur( ORD row, ORD col, int typ, int attr )
/*********************************************************/
{
    attr = attr;
    if( ( typ != UIData->cursor_type ) ||
        ( row != UIData->cursor_row ) ||
        ( col != UIData->cursor_col ) ) {
        UIData->cursor_type = typ;
        UIData->cursor_row = row;
        UIData->cursor_col = col;
        newcursor();
        physupdate( NULL );
    }
    return( 0 );
}

EVENT cd_event( void )
{
    EVENT       ev;

    ev = cd_sizeevent();
    if( ev > EV_NO_EVENT )
        return( ev );
    ev = mouseevent();
    if( ev > EV_NO_EVENT )
        return( ev );
    ev = ck_keyboardevent();
    if( ev == EV_NO_EVENT )
        return( ev );
    uihidemouse();
    return( ev );
}

Display ConsDisplay = {
        cd_init,
        cd_fini,
        cd_update,
        cd_refresh,
        cd_getcur,
        cd_setcur,
        cd_event,
};

Display QnxWDisplay = {
        cd_init,
        cd_fini,
        cd_update,
        cd_refresh,
        cd_getcur,
        cd_setcur,
        td_event,
};
