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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "auipvt.h"

extern  bool    WndAtTop( a_window *wnd )
{
    if( !WndHasCurrent( wnd ) ) return( FALSE );
    if( wnd->current.row > wnd->title_size ) return( FALSE );
    return( TRUE );
}


static  bool    WndFindCloseTab( a_window *wnd, int row )
{
    int         piece;

    piece = wnd->current.piece;
    while( piece >= 0 ) {
        if( WndPieceIsTab( wnd, row, piece ) ) {
            wnd->current.piece = piece;
            return( TRUE );
        }
        --piece;
    }
    return( FALSE );
}


static  void    WndMoveUp( a_window *wnd )
{
    if( WndAtTop( wnd ) ) {
        WndDirtyCurr( wnd );
        WndScroll( wnd, -1 );
        if( !WndFindCloseTab( wnd, wnd->current.row ) ) {
            wnd->current.row++;
        }
        WndCurrVisible( wnd );
        WndDirtyCurr( wnd );
    } else if( WndHasCurrent( wnd ) ) {
        WndDirtyCurr( wnd );
        if( WndFindCloseTab( wnd, wnd->current.row-1 ) ) {
            wnd->current.row--;
        }
        WndCurrVisible( wnd );
        WndDirtyCurr( wnd );
    } else {
        if( WndFirstCurrent( wnd ) ) {
            WndDirtyCurr( wnd );
        }
    }
}


static  bool    WndAtBottom( a_window *wnd )
{
    if( !WndHasCurrent( wnd ) ) return( FALSE );
    if( wnd->current.row < wnd->rows-1 ) return( FALSE );
    return( TRUE );
}


static  void    WndMoveDown( a_window *wnd )
{
    if( WndAtBottom( wnd ) ) {
        WndDirtyCurr( wnd );
        WndScroll( wnd, 1 );
        if( !WndFindCloseTab( wnd, wnd->current.row ) ) {
            wnd->current.row--;
        }
        WndCurrVisible( wnd );
        WndDirtyCurr( wnd );
    } else if( WndHasCurrent( wnd ) ) {
        WndDirtyCurr( wnd );
        if( WndFindCloseTab( wnd, wnd->current.row+1 ) ) {
            wnd->current.row++;
        }
        WndCurrVisible( wnd );
        WndDirtyCurr( wnd );
    } else {
        if( WndFirstCurrent( wnd ) ) {
            WndDirtyCurr( wnd );
        }
    }
}

static  void    WndUpOne( a_window *wnd )
{
    if( !WndAtBottom( wnd ) ) {
        if( WndScroll( wnd, -1 ) != 0 ) {
            if( WndHasCurrent( wnd ) ) WndMoveDown( wnd );
        }
    } else {
        WndScroll( wnd, -1 );
    }
}


static  void    WndDownOne( a_window *wnd )
{
    if( !WndAtTop( wnd ) ) {
        if( WndScroll( wnd, 1 ) != 0 ) {
            if( WndHasCurrent( wnd ) ) WndMoveUp( wnd );
        }
    } else {
        WndScroll( wnd, 1 );
    }
}

void WndScrollUp( a_window *wnd )
{
    WndUpOne( wnd );
}

void WndCursorUp( a_window *wnd )
{
    WndNoSelect( wnd );
    WndMoveUp( wnd );
}

void WndScrollDown( a_window *wnd )
{
    WndDownOne( wnd );
}

void WndCursorDown( a_window *wnd )
{
    WndNoSelect( wnd );
    WndMoveDown( wnd );
}

static int WndPageSize( a_window *wnd )
{
    int rows = WndRows( wnd );
    if( rows <= 0 ) return( 0 );
    if( rows <= 8 ) return( rows - 1 );
    return( rows - 2 );
}

void WndPageUp( a_window *wnd )
{
    WndScroll( wnd, -WndPageSize( wnd ) );
}

void WndPageDown( a_window *wnd )
{
    WndScroll( wnd, WndPageSize( wnd ) );
}

bool WndTabLeft( a_window *wnd, bool wrap )
{
    wnd_coord   curr;

    curr = wnd->current;
    if( WndPrevCurrent( wnd, wrap ) ) {
        WndDirtyScreenPiece( wnd, &curr );
        WndCurrVisible( wnd );
        WndDirtyCurr( wnd );
        return( TRUE );
    } else {
        WndSetHScroll( wnd, 0 );
        WndEvent( wnd, GUI_SCROLL_LEFT, NULL );
        return( FALSE );
    }
}

bool WndCursorLeft( a_window *wnd )
{
    int         col;
    wnd_line_piece      line;

    if( _Isnt( wnd, WSW_CHAR_CURSOR )||
        !WndHasCurrent( wnd ) || wnd->current.col == 0 ) {
        if( !WndTabLeft( wnd, FALSE ) ) return( FALSE );
        wnd->current.col = WND_MAX_COL;
        WndSetCurrCol( wnd );
        WndCurrVisible( wnd );
        WndDirtyCurrChar( wnd );
        return( TRUE );
    } else {
        WndGetLine( wnd, wnd->current.row, wnd->current.piece, &line );
        WndDirtyCurrChar( wnd );
        col = wnd->current.col;
        wnd->current.col = WndPrevCharCol( line.text, wnd->current.col );
        WndSetCurrCol( wnd );
        WndCurrVisible( wnd );
        WndDirtyCurrChar( wnd );
        return( col != wnd->current.col );
    }
}

bool WndTabRight( a_window *wnd, bool wrap )
{
    wnd_coord   curr;

    curr = wnd->current;
    if( !WndNextCurrent( wnd, wrap ) ) {
        WndEvent( wnd, GUI_SCROLL_RIGHT, NULL );
        return( FALSE );
    }
    WndDirtyScreenPiece( wnd, &curr );
    WndCurrVisible( wnd );
    WndDirtyCurr( wnd );
    return( TRUE );
}

bool WndCursorRight( a_window *wnd )
{
    wnd_line_piece      line;
    int                 col;
    bool                got;

    got = WndGetLine( wnd, wnd->current.row, wnd->current.piece, &line );
    if( _Isnt( wnd, WSW_CHAR_CURSOR ) ||
        !WndHasCurrent( wnd ) ||
        !got ||
        wnd->current.col + 1 >= line.length ) {
        return( WndTabRight( wnd, FALSE ) );
    } else {
        WndDirtyCurrChar( wnd );
        col = wnd->current.col;
        wnd->current.col += GUICharLen( line.text[wnd->current.col] );
        WndSetCurrCol( wnd );
        WndCurrVisible( wnd );
        WndDirtyCurrChar( wnd );
        return( col != wnd->current.col );
    }
}

void WndScrollTop( a_window *wnd )
{
    WndDirtyCurr( wnd );
    WndScrollAbs( wnd, -wnd->title_size );
    WndFirstCurrent( wnd );
}

void WndScrollBottom( a_window *wnd )
{
    WndDirtyCurr( wnd );
    WndScrollAbs( wnd, WND_MAX_ROW );
    WndLastCurrent( wnd );
}

void WndCursorStart( a_window *wnd )
{
    while( WndCursorLeft( wnd ) );
}

void WndCursorEnd( a_window *wnd )
{
    while( WndCursorRight( wnd ) );
}
