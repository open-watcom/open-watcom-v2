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


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgmem.h"
#include "dbgrep.h"
#include "dbgevent.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgutil.h"

extern void             RecordPointStart(void);
extern void             RecordNewProg(void);
extern void             PointFini( void );
extern int              GetStackPos( void );
extern char             *GetCmdName( int index );
extern unsigned         UndoLevel( void );
extern address          GetRegIP( void );
extern  a_window        *WndSrcInspect( address addr );
extern  a_window        *WndAsmInspect( address addr );
extern unsigned         ReqExpr( void );
extern char             *CopySourceLine( cue_handle *ch );
extern char             *GetEventAddress( event_record *ev );
extern void             ReplayTo( event_record *ev );


enum {
    PIECE_ADDRESS,
    PIECE_SOURCE,
    PIECE_COMMAND,
    PIECE__LAST
};

static gui_ord          Indents[ PIECE__LAST ];


#include "menudef.h"

static gui_menu_struct RepMenu[] = {
    #include "menurep.h"
};

extern event_record *EventList;
static int      LastEventCount;

static void RepInitEv( event_record *ev )
{
    DIPHDL( cue, ch );
    if( ev->cue == NULL ) {
        if( DeAliasAddrCue( NO_MOD, ev->ip, ch ) != SR_NONE ) {
            ev->cue = CopySourceLine( ch );
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


static WNDNUMROWS RepNumRows;
static int RepNumRows( a_window *wnd )
{
    int         count;
    event_record        *ev;

    wnd = wnd;
    count = 0;
    for( ev = EventList; ev != NULL; ev = ev->next ) {
        ++count;
    }
    return( count );
}


static WNDREFRESH RepRefresh;
static void RepRefresh( a_window *wnd )
{
    event_record        *ev;
    gui_ord             extent,max_addr, max_cue;
    int                 count,row;

    if( EventList != NULL ) WndMoveCurrent( wnd, RepNumRows( wnd )-1, 0 );
    max_addr = 0;
    max_cue = 0;
    count = 0;
    for( ev = EventList; ev != NULL; ev = ev->next ) {
        RepInitEv( ev );
        extent = WndExtentX( wnd, ev->addr_string );
        if( extent > max_addr ) max_addr = extent;
        extent = WndExtentX( wnd, ev->cue );
        if( extent > max_cue ) max_cue = extent;
        ++count;
    }
    max_addr += WndMaxCharX( wnd );
    max_cue += WndMaxCharX( wnd );
    if( Indents[ PIECE_SOURCE ] != max_addr ||
        Indents[ PIECE_COMMAND ] != max_addr + max_cue ) {
        WndRepaint( wnd );
    } else {
        row = count;
        while( --row >= LastEventCount ) {
            WndRowDirty( wnd, row );
        }
    }
    LastEventCount = count;
    Indents[ PIECE_ADDRESS ] = 0;
    Indents[ PIECE_SOURCE ] = max_addr;
    Indents[ PIECE_COMMAND ] = max_addr + max_cue;
}


static  WNDMENU RepMenuItem;
static void     RepMenuItem( a_window *wnd, unsigned id, int row, int piece )
{
    event_record        *ev;

    ev = RepGetEvent( row );
    wnd=wnd;piece=piece;
    switch( id ) {
    case MENU_INITIALIZE:
        WndMenuGrayAll( wnd );
        if( ev != NULL ) {
            WndMenuEnable( wnd, MENU_REPLAY_GOTO, TRUE );
            if( !IS_NIL_ADDR( ev->ip ) ) {
                WndMenuEnable( wnd, MENU_REPLAY_SOURCE, TRUE );
                WndMenuEnable( wnd, MENU_REPLAY_ASSEMBLY, TRUE );
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


static  WNDGETLINE RepGetLine;
static  bool    RepGetLine( a_window *wnd, int row, int piece,
                            wnd_line_piece *line )
{
    event_record        *ev;

    wnd = wnd;
    ev = RepGetEvent( row );
    if( ev == NULL ) return( FALSE );
    line->tabstop = FALSE;
    if( piece >= PIECE__LAST ) return( FALSE );
    line->indent = Indents[ piece ];
    switch( piece ) {
    case PIECE_ADDRESS:
        line->tabstop = TRUE;
        line->text = ev->addr_string;
        return( TRUE );
    case PIECE_SOURCE:
        line->text = ev->cue;
        return( TRUE );
    case PIECE_COMMAND:
        line->text = ev->cmd->buff;
        line->attr = WND_STANDOUT;
        return( TRUE );
    default:
        return( FALSE );
    }
}


static WNDCALLBACK RepEventProc;
static bool RepEventProc( a_window * wnd, gui_event gui_ev, void *parm )
{
    parm=parm;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        RepRefresh( wnd );
        return( TRUE );
    }
    return( FALSE );
}

wnd_info RepInfo = {
    RepEventProc,
    RepRefresh,
    RepGetLine,
    RepMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    NoModify,
    RepNumRows,
    NoNextRow,
    NoNotify,
    ChkFlags,
    UP_EVENT_CHANGE,
    DefPopUp( RepMenu )
};


extern WNDOPEN WndRepOpen;
extern a_window *WndRepOpen( void )
{
    return( DbgWndCreate( LIT_DUI( WindowReplay ), &RepInfo, WND_REPLAY, NULL, &RepIcon ) );
}
