/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgmem.h"
#include "dbgrep.h"
#include "dbgevent.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgutil.h"
#include "dbgbrk.h"
#include "dbgparse.h"
#include "wndsys.h"
#include "dbgprog.h"
#include "dipimp.h"
#include "dipinter.h"
#include "dbgreg.h"
#include "dbgwglob.h"
#include "dbgwinsp.h"
#include "menudef.h"


enum {
    PIECE_ADDRESS,
    PIECE_SOURCE,
    PIECE_COMMAND,
    PIECE__LAST
};

extern event_record *EventList;

static gui_ord      Indents[PIECE__LAST];

static int          LastEventCount;

static gui_menu_struct RepMenu[] = {
    #include "menurep.h"
};

static void RepInitEv( event_record *ev )
{
    DIPHDL( cue, cueh );
    if( ev->cue == NULL ) {
        if( DeAliasAddrCue( NO_MOD, ev->ip, cueh ) != SR_NONE ) {
            ev->cue = CopySourceLine( cueh );
        }
        if( ev->cue == NULL ) {
            UnAsm( ev->ip, TxtBuff, TXT_LEN );
            ev->cue = DupStr( TxtBuff );
        }
    }
    if( ev->addr_string == NULL ) {
        ev->addr_string = DupStr( GetEventAddress( ev ) );
    }
}

static event_record *RepGetEvent( int row )
{
    int                 count;
    event_record        *ev;

    count = 0;
    for( ev = EventList; ev != NULL; ev = ev->next ) {
        if( count == row ) {
            RepInitEv( ev );
            return( ev );
        }
        count++;
    }
    return( NULL );
}


static wnd_row RepNumRows( a_window wnd )
{
    wnd_row             count;
    event_record        *ev;

    /* unused parameters */ (void)wnd;

    count = 0;
    for( ev = EventList; ev != NULL; ev = ev->next ) {
        ++count;
    }
    return( count );
}


static void RepRefresh( a_window wnd )
{
    event_record        *ev;
    gui_ord             extent, max_addr, max_cue;
    int                 count, row;

    if( EventList != NULL )
        WndMoveCurrent( wnd, RepNumRows( wnd ) - 1, 0 );
    max_addr = 0;
    max_cue = 0;
    count = 0;
    for( ev = EventList; ev != NULL; ev = ev->next ) {
        RepInitEv( ev );
        extent = WndExtentX( wnd, ev->addr_string );
        if( max_addr < extent )
            max_addr = extent;
        extent = WndExtentX( wnd, ev->cue );
        if( max_cue < extent )
            max_cue = extent;
        ++count;
    }
    max_addr += WndMaxCharX( wnd );
    max_cue += WndMaxCharX( wnd );
    if( Indents[PIECE_SOURCE] != max_addr ||
        Indents[PIECE_COMMAND] != max_addr + max_cue ) {
        WndSetRepaint( wnd );
    } else {
        row = count;
        while( --row >= LastEventCount ) {
            WndRowDirty( wnd, row );
        }
    }
    LastEventCount = count;
    Indents[PIECE_ADDRESS] = 0;
    Indents[PIECE_SOURCE] = max_addr;
    Indents[PIECE_COMMAND] = max_addr + max_cue;
}


static void RepMenuItem( a_window wnd, gui_ctl_id id, wnd_row row, wnd_piece piece )
{
    event_record        *ev;

    /* unused parameters */ (void)wnd; (void)piece;

    ev = RepGetEvent( row );
    switch( id ) {
    case MENU_INITIALIZE:
        WndMenuGrayAll( wnd );
        if( ev != NULL ) {
            WndMenuEnable( wnd, MENU_REPLAY_GOTO, true );
            if( !IS_NIL_ADDR( ev->ip ) ) {
                WndMenuEnable( wnd, MENU_REPLAY_SOURCE, true );
                WndMenuEnable( wnd, MENU_REPLAY_ASSEMBLY, true );
            }
        }
        break;
    case MENU_REPLAY_SOURCE:
        WndSrcInspect( ev->ip );
        break;
    case MENU_REPLAY_ASSEMBLY:
        WndAsmInspect( ev->ip );
        break;
    case MENU_REPLAY_GOTO:
        ReplayTo( ev );
        break;
    }
}


static  bool    RepGetLine( a_window wnd, wnd_row row, wnd_piece piece, wnd_line_piece *line )
{
    event_record        *ev;

    /* unused parameters */ (void)wnd;

    ev = RepGetEvent( row );
    if( ev == NULL )
        return( false );
    line->tabstop = false;
    if( piece >= PIECE__LAST )
        return( false );
    line->indent = Indents[piece];
    switch( piece ) {
    case PIECE_ADDRESS:
        line->tabstop = true;
        line->text = ev->addr_string;
        return( true );
    case PIECE_SOURCE:
        line->text = ev->cue;
        return( true );
    case PIECE_COMMAND:
        line->text = ev->cmd->buff;
        line->attr = WND_STANDOUT;
        return( true );
    default:
        return( false );
    }
}


static bool RepWndEventProc( a_window wnd, gui_event gui_ev, void *parm )
{
    /* unused parameters */ (void)parm;

    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        RepRefresh( wnd );
        return( true );
    }
    return( false );
}

static bool ChkUpdate( void )
{
    return( UpdateFlags & UP_EVENT_CHANGE );
}

wnd_info RepInfo = {
    RepWndEventProc,
    RepRefresh,
    RepGetLine,
    RepMenuItem,
    NoVScroll,
    NoBegPaint,
    NoEndPaint,
    NoModify,
    RepNumRows,
    NoNextRow,
    NoNotify,
    ChkUpdate,
    PopUp( RepMenu )
};


a_window WndRepOpen( void )
{
    return( DbgWndCreate( LIT_DUI( WindowReplay ), &RepInfo, WND_REPLAY, NULL, &RepIcon ) );
}
