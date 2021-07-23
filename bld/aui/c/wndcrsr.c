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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "_aui.h"


bool    WndAtTop( a_window wnd )
{
    if( !WndHasCurrent( wnd ) )
        return( false );
    if( wnd->current.row > wnd->title_rows )
        return( false );
    return( true );
}


static  bool    WndFindCloseTab( a_window wnd, wnd_row row )
{
    wnd_piece   piece;

    piece = wnd->current.piece;
    do {
        if( WndPieceIsTab( wnd, row, piece ) ) {
            wnd->current.piece = piece;
            return( true );
        }
    } while( piece-- > 0 );
    return( false );
}


static  void    WndMoveUp( a_window wnd )
{
    if( WndAtTop( wnd ) ) {
        WndDirtyCurr( wnd );
        WndVScroll( wnd, -1 );
        if( !WndFindCloseTab( wnd, wnd->current.row ) ) {
            wnd->current.row++;
        }
        WndCurrVisible( wnd );
        WndDirtyCurr( wnd );
    } else if( WndHasCurrent( wnd ) ) {
        WndDirtyCurr( wnd );
        if( WndFindCloseTab( wnd, wnd->current.row - 1 ) ) {
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


static bool     WndAtBottom( a_window wnd )
{
    if( !WndHasCurrent( wnd ) )
        return( false );
    if( wnd->current.row < wnd->rows - 1 )
        return( false );
    return( true );
}


static void     WndMoveDown( a_window wnd )
{
    if( WndAtBottom( wnd ) ) {
        WndDirtyCurr( wnd );
        WndVScroll( wnd, 1 );
        if( !WndFindCloseTab( wnd, wnd->current.row ) ) {
            wnd->current.row--;
        }
        WndCurrVisible( wnd );
        WndDirtyCurr( wnd );
    } else if( WndHasCurrent( wnd ) ) {
        WndDirtyCurr( wnd );
        if( WndFindCloseTab( wnd, wnd->current.row + 1 ) ) {
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

static  void    WndUpOne( a_window wnd )
{
    if( !WndAtBottom( wnd ) ) {
        if( WndVScroll( wnd, -1 ) != 0 ) {
            if( WndHasCurrent( wnd ) ) {
                WndMoveDown( wnd );
            }
        }
    } else {
        WndVScroll( wnd, -1 );
    }
}


static  void    WndDownOne( a_window wnd )
{
    if( !WndAtTop( wnd ) ) {
        if( WndVScroll( wnd, 1 ) != 0 ) {
            if( WndHasCurrent( wnd ) ) {
                WndMoveUp( wnd );
            }
        }
    } else {
        WndVScroll( wnd, 1 );
    }
}

void WndScrollUp( a_window wnd )
{
    WndUpOne( wnd );
}

void WndCursorUp( a_window wnd )
{
    WndNoSelect( wnd );
    WndMoveUp( wnd );
}

void WndScrollDown( a_window wnd )
{
    WndDownOne( wnd );
}

void WndCursorDown( a_window wnd )
{
    WndNoSelect( wnd );
    WndMoveDown( wnd );
}

static int WndPageSize( a_window wnd )
{
    int     rows;

    rows = WndRows( wnd );
    if( rows <= 0 )
        return( 0 );
    if( rows <= 8 )
        return( rows - 1 );
    return( rows - 2 );
}

void WndPageUp( a_window wnd )
{
    WndVScroll( wnd, -WndPageSize( wnd ) );
}

void WndPageDown( a_window wnd )
{
    WndVScroll( wnd, WndPageSize( wnd ) );
}

bool WndTabLeft( a_window wnd, bool wrap )
{
    wnd_coord   curr;

    curr = wnd->current;
    if( WndPrevCurrent( wnd, wrap ) ) {
        WndDirtyScreenPiece( wnd, &curr );
        WndCurrVisible( wnd );
        WndDirtyCurr( wnd );
        return( true );
    } else {
        WndSetHScroll( wnd, 0 );
        WNDEVENT( wnd, GUI_SCROLL_LEFT, NULL );
        return( false );
    }
}

static bool WndCursorLeftCheck( a_window wnd )
{
    wnd_colidx      colidx;
    wnd_line_piece  line;

    if( WndSwitchOff( wnd, WSW_CHAR_CURSOR ) || !WndHasCurrent( wnd ) || wnd->current.colidx == 0 ) {
        if( !WndTabLeft( wnd, false ) )
            return( false );
        wnd->current.colidx = WND_MAX_COLIDX;
        WndSetCurrCol( wnd );
        WndCurrVisible( wnd );
        WndDirtyCurrChar( wnd );
        return( true );
    } else {
        WndGetLine( wnd, wnd->current.row, wnd->current.piece, &line );
        WndDirtyCurrChar( wnd );
        colidx = wnd->current.colidx;
        wnd->current.colidx = WndPrevCharColIdx( line.text, wnd->current.colidx );
        WndSetCurrCol( wnd );
        WndCurrVisible( wnd );
        WndDirtyCurrChar( wnd );
        return( colidx != wnd->current.colidx );
    }
}

void WndCursorLeft( a_window wnd )
{
    WndCursorLeftCheck( wnd );
}

bool WndTabRight( a_window wnd, bool wrap )
{
    wnd_coord   curr;

    curr = wnd->current;
    if( !WndNextCurrent( wnd, wrap ) ) {
        WNDEVENT( wnd, GUI_SCROLL_RIGHT, NULL );
        return( false );
    }
    WndDirtyScreenPiece( wnd, &curr );
    WndCurrVisible( wnd );
    WndDirtyCurr( wnd );
    return( true );
}

static bool WndCursorRightCheck( a_window wnd )
{
    wnd_line_piece      line;
    wnd_colidx          colidx;
    bool                got;

    got = WndGetLine( wnd, wnd->current.row, wnd->current.piece, &line );
    if( WndSwitchOff( wnd, WSW_CHAR_CURSOR ) || !WndHasCurrent( wnd ) || !got || wnd->current.colidx + 1 >= line.length ) {
        return( WndTabRight( wnd, false ) );
    } else {
        WndDirtyCurrChar( wnd );
        colidx = wnd->current.colidx;
        wnd->current.colidx += GUICharLen( UCHAR_VALUE( line.text[wnd->current.colidx] ) );
        WndSetCurrCol( wnd );
        WndCurrVisible( wnd );
        WndDirtyCurrChar( wnd );
        return( colidx != wnd->current.colidx );
    }
}

void WndCursorRight( a_window wnd )
{
    WndCursorRightCheck( wnd );
}

void WndScrollTop( a_window wnd )
{
    WndDirtyCurr( wnd );
    WndVScrollAbs( wnd, -wnd->title_rows );
    WndFirstCurrent( wnd );
}

void WndScrollBottom( a_window wnd )
{
    WndDirtyCurr( wnd );
    WndVScrollAbs( wnd, WND_MAX_ROW );
    WndLastCurrent( wnd );
}

void WndCursorStart( a_window wnd )
{
    while( WndCursorLeftCheck( wnd ) );
}

void WndCursorEnd( a_window wnd )
{
    while( WndCursorRightCheck( wnd ) );
}
