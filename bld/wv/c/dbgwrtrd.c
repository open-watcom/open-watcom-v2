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
* Description:  Non-blocking debug window
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbginfo.h"
#include "dbgwind.h"
#include "dbgreg.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern bool             RemoteGetRunThreadInfo( int row, char *infotype, int *width, char *header, int maxsize );
extern void             RemoteUpdateRunThread( thread_state *thd );

extern bool             IsThdCurr( thread_state *thd );
extern void             SetUnderLine( a_window *, wnd_line_piece * );
extern void             DbgUpdate( update_list );
extern bool             HaveRemoteRunThread( void );
extern void             RemotePollRunThread( void );
extern void             MakeRunThdCurr( thread_state * );
extern void             RemoteStopThread( thread_state *thd );
extern void             RemoteSignalStopThread( thread_state *thd );

extern thread_state     *HeadThd;
extern char             *TxtBuff;

#include "menudef.h"
static gui_menu_struct RunTrdMenu[] = {
    #include "menurtrd.h"
};

#define RUN_THREAD_INFO_TYPE_NONE       0
#define RUN_THREAD_INFO_TYPE_NAME       1
#define RUN_THREAD_INFO_TYPE_STATE      2
#define RUN_THREAD_INFO_TYPE_CS_EIP     3
#define RUN_THREAD_INFO_TYPE_EXTRA      4

#define TITLE_SIZE      2

#define MAX_PIECE_COUNT     4
#define MAX_HEADER_SIZE     80

static a_window *RunThreadWnd = 0;
static int      PieceCount = 0;
static char     Indents[MAX_PIECE_COUNT + 1];
static char     InfoType[MAX_PIECE_COUNT];
static char     HeaderArr[MAX_PIECE_COUNT][MAX_HEADER_SIZE + 1];

void InitRunThreadWnd()
{
    int     Width;
    int     i;
    bool    ok;

    PieceCount = 0;
    Indents[0] = 0;
    
    for(i  = 0; i < MAX_PIECE_COUNT; i++ ) {
        ok = RemoteGetRunThreadInfo( i, &InfoType[PieceCount], &Width, HeaderArr[PieceCount], MAX_HEADER_SIZE );
        if( ok ) {
            Indents[PieceCount + 1] = Indents[PieceCount] + (char)Width;
            PieceCount++;
        } else
            break;
    }
}

static thread_state     *GetThreadRow( int row )
{
    thread_state    *thd;
    unsigned        num;

    num = 0;
    for( thd = HeadThd; thd != NULL; thd = thd->link ) {
        if( num++ == row ) return( thd );
    }
    return( NULL );
}

static WNDNUMROWS RunTrdNumRows;
static int RunTrdNumRows( a_window *wnd )
{
    thread_state    *thd;
    unsigned        num;

    wnd=wnd;
    num = 0;
    for( thd = HeadThd; thd != NULL; thd = thd->link ) ++num;
    return( num );
}

static WNDCALLBACK RunTrdEventProc;
static bool RunTrdEventProc( a_window * wnd, gui_event gui_ev, void *parm )
{
    parm=parm;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        RunThreadWnd = wnd;
        return( TRUE );
    case GUI_DESTROY :
        RunThreadWnd = 0;
        return( TRUE );
    }
    return( FALSE );
}

static  WNDMENU RunTrdMenuItem;
static void     RunTrdMenuItem( a_window *wnd, unsigned id, int row, int piece )
{
    thread_state        *thd = GetThreadRow( row );

    piece=piece;
    switch( id ) {
    case MENU_INITIALIZE:
        if( thd == NULL ) {
            WndMenuGrayAll( wnd );
        } else {
            switch( thd->state ) {
            case THD_SIGNAL:
                WndMenuEnable( wnd, MENU_RUN_THREAD_STOP, TRUE );
                WndMenuEnable( wnd, MENU_RUN_THREAD_SIGNAL_STOP, TRUE );
                WndMenuEnable( wnd, MENU_RUN_THREAD_CHANGE_TO, FALSE );
                break;

            case THD_DEBUG:
                WndMenuEnable( wnd, MENU_RUN_THREAD_STOP, FALSE );
                WndMenuEnable( wnd, MENU_RUN_THREAD_SIGNAL_STOP, FALSE );
                WndMenuEnable( wnd, MENU_RUN_THREAD_CHANGE_TO, TRUE );
                break;

            case THD_RUN:
            case THD_WAIT:
            case THD_BLOCKED:
                WndMenuEnable( wnd, MENU_RUN_THREAD_STOP, TRUE );
                WndMenuEnable( wnd, MENU_RUN_THREAD_SIGNAL_STOP, FALSE );
                WndMenuEnable( wnd, MENU_RUN_THREAD_CHANGE_TO, FALSE );
                break;

            default:
                WndMenuGrayAll( wnd );
                break;
            }                    
        }
        return;
    case MENU_RUN_THREAD_STOP:
        RemoteStopThread( thd );
        break;
    case MENU_RUN_THREAD_SIGNAL_STOP:
        RemoteSignalStopThread( thd );
        break;
    case MENU_RUN_THREAD_CHANGE_TO:
        MakeRunThdCurr( thd );
        break;
    }
    DbgUpdate( UP_THREAD_STATE );
}

static WNDREFRESH RunTrdRefresh;
static void RunTrdRefresh( a_window *wnd )
{
    thread_state    *thd;
    int             row;

    row = 0;
    for( thd = HeadThd; thd != NULL; thd = thd->link ) {
        if( IsThdCurr( thd ) ) {
            WndMoveCurrent( wnd, row, 0 );
            break;
        }
        ++row;
    }
    WndNoSelect( wnd );
    WndRepaint( wnd );
}

static  bool    RunTrdGetLine( a_window *wnd, int row, int piece,
                               wnd_line_piece *line )
{
    thread_state        *thd = GetThreadRow( row );

    line->indent = Indents[ piece ] * WndAvgCharX( wnd );
    if( row < 0 ) {
        row += TITLE_SIZE;
        switch( row ) {
        case 0:
            if( piece < PieceCount ) {
                line->text = HeaderArr[ piece ];
                return( TRUE );
            } 
            return( FALSE );
        case 1:
            if( piece != 0 ) return( FALSE );
            SetUnderLine( wnd, line );
            return( TRUE );
        default:
            return( FALSE );
        }
    } else {
        if( thd == NULL ) return( FALSE );
        line->tabstop = FALSE;
        line->use_prev_attr = TRUE;
        line->extent = WND_MAX_EXTEND;
        switch( InfoType[ piece ] ) {
        case RUN_THREAD_INFO_TYPE_NAME:
            line->tabstop = TRUE;
            line->use_prev_attr = FALSE;
            line->text = thd->name;
            return( TRUE );
        case RUN_THREAD_INFO_TYPE_STATE:
            if( IsThdCurr( thd ) && ( thd->state == THD_DEBUG ) ) {
                line->text = LIT( Current );
            } else {
                switch( thd->state ) {
                case THD_THAW:
                    line->text = LIT( Runnable );
                    break;
                case THD_FREEZE:
                    line->text = LIT( Frozen );
                    break;
                case THD_WAIT:
                    line->text = LIT( Wait );
                    break;  
                case THD_SIGNAL:
                    line->text = LIT( Signal );
                    break;  
                case THD_KEYBOARD:
                    line->text = LIT( Keyboard );
                    break;  
                case THD_BLOCKED:
                    line->text = LIT( Blocked );
                    break;  
                case THD_RUN:
                    line->text = LIT( Executing );
                    break;  
                case THD_DEBUG:
                    line->text = LIT( Debug );
                    break;
                case THD_DEAD:
                    line->text = LIT( Dead );
                    break;
                }
            }
            return( TRUE );
        case RUN_THREAD_INFO_TYPE_EXTRA:
            line->tabstop = FALSE;
            line->use_prev_attr = TRUE;
            line->text = thd->extra;
            return( TRUE );
        case RUN_THREAD_INFO_TYPE_CS_EIP:
            line->tabstop = FALSE;
            line->use_prev_attr = TRUE;
            if( thd->cs ) {
                sprintf(TxtBuff, "%04hX:%08lX", thd->cs, thd->eip );
                line->text = TxtBuff;
            } else {
                line->text = "";
            }
            return( TRUE );
        }
    }
    return( FALSE );
}

wnd_info RunTrdInfo = {
    RunTrdEventProc,
    RunTrdRefresh,
    RunTrdGetLine,
    RunTrdMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    WndFirstMenuItem,
    RunTrdNumRows,
    NoNextRow,
    NoNotify,
    UP_THREAD_STATE,
    DefPopUp( RunTrdMenu ),
};


a_window *WndRunTrdOpen()
{
    return( DbgTitleWndCreate( LIT( WindowThreads ), &RunTrdInfo, WND_RUN_THREAD, NULL,
                               &TrdIcon, TITLE_SIZE, TRUE ) );
}

void RunThreadNotify( void )
{
    thread_state    *thd;

    if( HeadThd && HaveRemoteRunThread() ) {
        RemotePollRunThread();

        if( RunThreadWnd ) {
            for( thd = HeadThd; thd != NULL; thd = thd->link ) {
                RemoteUpdateRunThread( thd );
            }
            WndRepaint( RunThreadWnd );
        }
    }
}
