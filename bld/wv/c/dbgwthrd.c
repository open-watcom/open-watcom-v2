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
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgutil.h"

extern bool             IsThdCurr( thread_state *thd );
extern void             MakeThdCurr( thread_state * );
extern void             RemoteThdName( dtid_t, char * );
extern void             SetUnderLine( a_window *, wnd_line_piece * );
extern void             DbgUpdate( update_list );

#include "menudef.h"
static gui_menu_struct TrdMenu[] = {
    #include "menutrd.h"
};

// gud enuf for now
#define ID_WIDTH        11
#define STATE_WIDTH     10

#define TITLE_SIZE      2

enum {
    PIECE_ID,
    PIECE_STATE,
    PIECE_NAME,
    PIECE_LAST,
};

static char     Indents[PIECE_LAST] = { 0, ID_WIDTH, ID_WIDTH+STATE_WIDTH };


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

static WNDNUMROWS TrdNumRows;
static int TrdNumRows( a_window *wnd )
{
    thread_state    *thd;
    unsigned        num;

    wnd=wnd;
    num = 0;
    for( thd = HeadThd; thd != NULL; thd = thd->link ) ++num;
    return( num );
}

static  WNDMENU TrdMenuItem;
static void     TrdMenuItem( a_window *wnd, unsigned id, int row, int piece )
{
    thread_state        *thd = GetThreadRow( row );

    piece=piece;
    switch( id ) {
    case MENU_INITIALIZE:
        if( thd == NULL ) {
            WndMenuGrayAll( wnd );
        } else {
            switch( thd->state ) {
            case THD_THAW:
            case THD_FREEZE:
                WndMenuEnable( wnd, MENU_THREAD_FREEZE, TRUE );
                WndMenuEnable( wnd, MENU_THREAD_THAW, TRUE );
                WndMenuEnable( wnd, MENU_THREAD_CHANGE_TO, TRUE );
                break;
            case THD_DEBUG:
                WndMenuEnable( wnd, MENU_THREAD_FREEZE, FALSE );
                WndMenuEnable( wnd, MENU_THREAD_THAW, FALSE );
                WndMenuEnable( wnd, MENU_THREAD_CHANGE_TO, TRUE );
                break;
            default:
                WndMenuGrayAll( wnd );
                break;
            }                    
        }
        return;
    case MENU_THREAD_FREEZE:
        if( thd->state == THD_THAW ) thd->state = THD_FREEZE;
        break;
    case MENU_THREAD_THAW:
        if( thd->state == THD_FREEZE ) thd->state = THD_THAW;
        break;
    case MENU_THREAD_CHANGE_TO:
        switch( thd->state ) {
        case THD_THAW:
        case THD_FREEZE:
        case THD_DEBUG:    
            MakeThdCurr( thd );
            break;
        }
    }
    DbgUpdate( UP_THREAD_STATE );
}


static WNDREFRESH TrdRefresh;
static void TrdRefresh( a_window *wnd )
{
    thread_state    *thd;
    int                 row;

    row = 0;
    for( thd = HeadThd; thd != NULL; thd = thd->link ) {
        if( IsThdCurr( thd ) ) {
            WndMoveCurrent( wnd, row, PIECE_ID );
            break;
        }
        ++row;
    }
    WndNoSelect( wnd );
    WndRepaint( wnd );
}


static  bool    TrdGetLine( a_window *wnd, int row, int piece,
                             wnd_line_piece *line )
{
    thread_state        *thd = GetThreadRow( row );

    line->indent = Indents[ piece ] * WndAvgCharX( wnd );
    if( row < 0 ) {
        row += TITLE_SIZE;
        switch( row ) {
        case 0:
            switch( piece ) {
            case PIECE_ID:
                line->text = LIT_DUI( ID );
                return( TRUE );
            case PIECE_STATE:
                line->text = LIT_DUI( State );
                return( TRUE );
            case PIECE_NAME:
                line->text = TxtBuff;
                RemoteThdName( 0, TxtBuff ); // nyi - pui - line up in proportional font
                return( TRUE );
            default:
                return( FALSE );
            }
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
        switch( piece ) {
        case PIECE_ID:
            line->tabstop = TRUE;
            line->use_prev_attr = FALSE;
            line->text = TxtBuff;
            CnvULongHex( thd->tid, TxtBuff, TXT_LEN );
            return( TRUE );
        case PIECE_STATE:
            if( IsThdCurr( thd ) ) {
                line->text = LIT_ENG( Current );
            } else {
                switch( thd->state ) {
                case THD_THAW:
                    line->text = LIT_ENG( Runnable );
                    break;
                case THD_FREEZE:
                    line->text = LIT_ENG( Frozen );
                    break;
                case THD_WAIT:
                    line->text = LIT_ENG( Wait );
                    break;  
                case THD_SIGNAL:
                    line->text = LIT_ENG( Signal );
                    break;  
                case THD_KEYBOARD:
                    line->text = LIT_ENG( Keyboard );
                    break;  
                case THD_BLOCKED:
                    line->text = LIT_ENG( Blocked );
                    break;  
                case THD_RUN:
                    line->text = LIT_ENG( Executing );
                    break;  
                case THD_DEBUG:
                    line->text = LIT_ENG( Debug );
                    break;
                case THD_DEAD:
                    line->text = LIT_ENG( Dead );
                    break;
                }
            }
            return( TRUE );
        case PIECE_NAME:
            line->tabstop = FALSE;
            line->use_prev_attr = TRUE;
            line->text = thd->name;
            return( TRUE );
        }
    }
    return( FALSE );
}


wnd_info TrdInfo = {
    NoEventProc,
    TrdRefresh,
    TrdGetLine,
    TrdMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    WndFirstMenuItem,
    TrdNumRows,
    NoNextRow,
    NoNotify,
    ChkFlags,
    UP_THREAD_STATE,
    DefPopUp( TrdMenu ),
};

a_window *WndTrdOpen()
{
    return( DbgTitleWndCreate( LIT_DUI( WindowThreads ), &TrdInfo, WND_THREAD, NULL,
                               &TrdIcon, TITLE_SIZE, TRUE ) );
}
