/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
#include "uiintern.h"
#include "uiextrn.h"
#include "ctkeyb.h"
#include "uicurshk.h"


static MONITOR ui_data = {
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
    int                 rows;
    int                 cols;
    LP_PIXEL            scrn;
    unsigned            size;
    unsigned            i;

    if( console_size( UIConCtrl, UIConsole, 0, 0, &rows, &cols ) != 0 ) {
        return( false );
    }
    UIData->width = cols;
    UIData->height = rows;
    size = UIData->width * UIData->height;
    scrn = UIData->screen.origin;
    {
#ifdef _M_I86
        unsigned    seg;

        if( scrn == NULL ) {
            seg = qnx_segment_alloc( size * sizeof( PIXEL ) );
        } else {
            seg = qnx_segment_realloc( _FP_SEG( scrn ), size * sizeof( PIXEL ) );
        }
        if( seg == -1 )
            return( false );
        scrn = _MK_FP( seg, 0 );
#else
        scrn = uirealloc( scrn, size * sizeof( PIXEL ) );
        if( scrn == NULL ) {
            return( false );
        }
#endif
    }
    for( i = 0; i < size; ++i ) {
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
    /* unused parameters */ (void)signo;

    StatePending = 1;
}


static ui_event cd_sizeevent( void )
/**********************************/
{
    SAREA       area;
    unsigned    state;
    unsigned    arm;

    if( !StatePending )
        return( EV_NO_EVENT );
    StatePending = 0;
    state = console_state( UIConCtrl, UIConsole, 0,
                (_CON_EVENT_ACTIVE | _CON_EVENT_INACTIVE | _CON_EVENT_SIZE) );
    arm = 0;
    if( state & _CON_EVENT_INACTIVE ) {
        clear_shift();
        arm = _CON_EVENT_ACTIVE;
    } else if( state & _CON_EVENT_ACTIVE ) {
        arm = _CON_EVENT_INACTIVE;
    }
    console_arm( UIConCtrl, UIConsole, 0, arm | _CON_EVENT_SIZE );
    if( (state & _CON_EVENT_SIZE) == 0 )
        return( EV_NO_EVENT );
    if( !uiinlists( EV_BACKGROUND_RESIZE ) )
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

static bool initmonitor( void )
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
            _CON_EVENT_SIZE | _CON_EVENT_ACTIVE | _CON_EVENT_SIZE );

    return( true );
}


/* update the physical screen with contents of virtual copy */

static void my_console_write(
    struct _console_ctrl    *cc,
    int                     console,
    unsigned                offset,
    LP_STRING               buf,
    int                     nbytes,
    CURSORORD               crow,
    CURSORORD               ccol,
    int                     type )
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
        msg.write.curs_row = crow;
        msg.write.curs_col = ccol;
        msg.write.curs_type = type;
        msg.write.offset = offset;
        msg.write.nbytes = nbytes;

        _setmx( &sx[1], buf, nbytes );
        _setmx( &sx[0], &msg.write, sizeof( msg.write ) - sizeof( msg.write.data ) );

        _setmx( &rx, &msg.write_reply, sizeof( msg.write_reply ) );

        Sendmx(cc->driver, 2, 1, sx, &rx );
}


static bool cd_init( void )
/*************************/
{
    int         initialized;

    initialized = false;
    if( UIData == NULL ) {
        UIData = &ui_data;
    }
    if( !initmonitor() )
        return( false );
    if( !setupscrnbuff() )
        return( false );

    uiinitcursor();
    initkeyboard();
    UIData->mouse_acc_delay = uiclockdelay( 277 /* ms */ );
    UIData->mouse_rpt_delay = uiclockdelay( 100 /* ms */ );
    UIData->mouse_clk_delay = uiclockdelay( 277 /* ms */ );
    UIData->tick_delay      = uiclockdelay( 500 /* ms */ );
    UIData->f10menus        = true;
    return( true );
}


static bool cd_fini( void )
/*************************/
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
    CURSORORD       crow;
    CURSORORD       ccol;
    int             type;
    int             i;

    crow = UIData->cursor_row;
    ccol = UIData->cursor_col;
    switch( UIData->cursor_type ) {
    case C_OFF:     type = CURSOR_OFF;          break;
    case C_NORMAL:  type = CURSOR_UNDERLINE;    break;
    case C_INSERT:  type = CURSOR_BLOCK;        break;
    }
    if( area == NULL ) {
        my_console_write( UIConCtrl, UIConsole, 0,
                        (LP_STRING)UIData->screen.origin, 0,
                        crow, ccol, type );
    } else {
        count = area->width * sizeof( PIXEL );
        for( i = area->row; i < ( area->row + area->height ); i++ ) {
            offset = ( i * UIData->width + area->col ) * sizeof( PIXEL );
            my_console_write( UIConCtrl, UIConsole, offset,
                            (LP_STRING)UIData->screen.origin + offset, count,
                            crow, ccol, type );
        }
    }
    return( 0 );
}

static int cd_refresh( bool must )
{
    /* unused parameters */ (void)must;

    return( 0 );
}

static int UIHOOK cd_getcur( CURSORORD *crow, CURSORORD *ccol, CURSOR_TYPE *ctype, CATTR *cattr )
/***********************************************************************************************/
{
    *crow = UIData->cursor_row;
    *ccol = UIData->cursor_col;
    *ctype = UIData->cursor_type;
    *cattr = CATTR_NONE;
    return( 0 );
}


static int UIHOOK cd_setcur( CURSORORD crow, CURSORORD ccol, CURSOR_TYPE ctype, CATTR cattr )
/*******************************************************************************************/
{
    /* unused parameters */ (void)cattr;

    if( ( ctype != UIData->cursor_type ) ||
        ( crow != UIData->cursor_row ) ||
        ( ccol != UIData->cursor_col ) ) {
        UIData->cursor_type = ctype;
        UIData->cursor_row = crow;
        UIData->cursor_col = ccol;
        newcursor();
        physupdate( NULL );
    }
    return( 0 );
}

static ui_event cd_event( void )
{
    ui_event    ui_ev;

    ui_ev = cd_sizeevent();
    if( ui_ev > EV_NO_EVENT )
        return( ui_ev );
    ui_ev = mouseevent();
    if( ui_ev > EV_NO_EVENT )
        return( ui_ev );
    ui_ev = ck_keyboardevent();
    if( ui_ev == EV_NO_EVENT )
        return( ui_ev );
    uihidemouse();
    return( ui_ev );
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
