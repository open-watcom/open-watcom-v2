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


#include "auipvt.h"//

extern  bool    WndSetPoint( a_window *wnd, void *parm, bool exact,
                             wnd_coord *spot, wnd_row row,
                             bool doing_select )
{
    gui_point           point;
    int                 col;
    int                 piece;
    int                 last_piece;
    int                 last_col;
    int                 last_extended_tab_piece;
    int                 last_extended_tab_col;
    bool                got;
    bool                allowed_in_tab;
    wnd_line_piece      line;
    gui_coord           indent;

    spot->row = WND_NO_ROW;
    spot->piece = WND_NO_PIECE;
    spot->col = WND_NO_COL;
    if( doing_select ) {
        allowed_in_tab = _Is( wnd, WSW_SELECT_IN_TABSTOP );
    } else {
        allowed_in_tab = TRUE;
    }
    GUI_GET_POINT( parm, point );
    if( point.x < 0 ) point.x = 0;
    if( point.y < 0 ) point.y = 0;
    if( row == WND_NO_ROW ) row = GUIGetRow( wnd->gui, &point );
    if( row > wnd->rows ) return( FALSE );
    if( row < wnd->title_size ) {
        row -= wnd->top;
    }
    last_piece = WND_NO_PIECE;
    last_extended_tab_piece = WND_NO_PIECE;
    for( piece = 0; ; ++piece ) {
        got = WndGetLine( wnd, row, piece, &line );
        indent.x = line.indent;
        indent.y = 0;
        GUITruncToPixel( &indent );
        line.indent = indent.x;
        if( line.tabstop && ( line.extent == WND_MAX_EXTEND || line.master_tabstop ) ) { // nyi ??
            if( last_extended_tab_piece == WND_NO_PIECE ) {
                last_extended_tab_piece = piece;
                last_extended_tab_col = WndLastCharCol( &line );
            }
        }
        if( !got || line.indent > point.x ) {
            if( last_piece == WND_NO_PIECE ) return( FALSE );
            spot->row = row;
            if( !doing_select && last_extended_tab_piece != WND_NO_PIECE ) {
                spot->piece = last_extended_tab_piece;
                spot->col = last_extended_tab_col;
            } else {
                spot->piece = last_piece;
                spot->col = last_col;
            }
            return( TRUE );
        }
        if( line.bitmap ) {
            if( doing_select ) continue;
            if( line.indent <= point.x && line.indent+line.length > point.x ) {
                spot->row = row;
                spot->piece = piece;
                spot->col = 0;
                return( TRUE );
            }
        } else {
            col = GUIGetStringPos( wnd->gui, line.indent, line.text, point.x );
            if( col != GUI_NO_COLUMN ) { // clicked within this piece
                if( !doing_select && last_extended_tab_piece != WND_NO_PIECE ) {
                    spot->piece = last_extended_tab_piece;
                    spot->col = last_extended_tab_col;
                } else {
                    if( !allowed_in_tab && line.tabstop ) return( FALSE );
                    if( line.static_text ) return( FALSE );
                    spot->piece = piece;
                }
                spot->row = row;
                spot->col = WndCharCol( line.text, col );
                return( TRUE );
            } else if( line.extent == WND_MAX_EXTEND || line.master_tabstop || !exact ) { // nyi ??
                // clicked to right of this hunk -- remember it
                last_piece = piece;
                last_col = WndLastCharCol( &line );
            }
        }
    }
}


extern  void    WndGetCurrent( a_window *wnd, wnd_row *row, int *piece )
{
    *row = WND_NO_ROW;
    if( !WndHasCurrent( wnd ) ) return;
    *row = WndVirtualRow( wnd, wnd->current.row );
    *piece = wnd->current.piece;
}


extern  void    WndNewCurrent( a_window *wnd, wnd_row row, int piece )
{
    wnd->current.row = WndScreenRow( wnd, row );
    wnd->current.piece = piece;
    wnd->current.col = 0;
    WndSetCurrCol( wnd );
    WndDirtyCurr( wnd );
}


extern void WndMoveCurrent( a_window *wnd, wnd_row row, int piece )
{
    WndDirtyCurr( wnd );
    if( row < WndTop( wnd ) ) {
        WndScroll( wnd, row - WndTop( wnd ) );
    } else if( row >= WndTop( wnd ) + WndRows( wnd ) ) {
        WndScroll( wnd, row - WndTop( wnd ) - WndRows( wnd ) + 1 );
    }
    WndNewCurrent( wnd, row, piece );
}


extern  bool    WndHasCurrent( a_window *wnd )
{
    return( wnd->current.row != WND_NO_ROW );
}


extern  void    WndNoCurrent( a_window *wnd )
{
    wnd->current.row = WND_NO_ROW;
    wnd->current.piece = 0;
    wnd->current.col = 0;
    WndSetCurrCol( wnd );
}


extern  wnd_row WndCurrRow( a_window *wnd )
{
    if( !WndHasCurrent( wnd ) ) return( WND_NO_ROW );
    return( WndVirtualRow( wnd, wnd->current.row ) );
}


extern  bool    WndNextCurrent( a_window *wnd, bool wrap )
{
    int                 row;
    int                 piece;
    wnd_line_piece      line;

    if( WndHasCurrent( wnd ) ) {
        piece = wnd->current.piece + 1;
        row = wnd->current.row;
    } else {
        row = wnd->title_size;
        piece = 0;
    }
    WndNextRow( wnd, WND_NO_ROW, WND_SAVE_ROW );
    for( ;; ) {
        for( ;; ) {
            if( !WndGetLine( wnd, row, piece, &line ) ) break;
            if( line.tabstop ) {
                WndDirtyCurr( wnd );
                if( row >= wnd->rows ) {
                    WndScroll( wnd, row - wnd->rows+1 );
                    row = wnd->rows-1;
                }
                wnd->current.row = row;
                wnd->current.piece = piece;
                wnd->current.col = 0;
                WndSetCurrCol( wnd );
                WndDirtyCurr( wnd );
                return( TRUE );
            }
            ++piece;
        }
        if( !wrap ) {
            WndNextRow( wnd, WND_NO_ROW, WND_RESTORE_ROW );
            return( FALSE );
        }
        row = WndNextRow( wnd, row, 1 );
        piece = 0;
        if( !WndGetLine( wnd, row, piece, &line ) ) {
            WndNextRow( wnd, WND_NO_ROW, WND_RESTORE_ROW );
            return( FALSE );
        }
    }
}


extern bool WndPrevCurrent( a_window *wnd, bool wrap )
{
    int                 piece;
    int                 last_piece;
    int                 found_piece;
    int                 row;
    wnd_line_piece      line;

    if( !WndHasCurrent( wnd ) ) return( FALSE );
    row = wnd->current.row;
    last_piece = wnd->current.piece;
    WndNextRow( wnd, WND_NO_ROW, WND_SAVE_ROW );
    for( row = wnd->current.row;; row = WndNextRow( wnd, row, -1 ) ) {
        found_piece = WND_NO_PIECE;
        for( piece = 0; piece < last_piece; ++piece ) {
            if( !WndGetLine( wnd, row, piece, &line ) ) {
                if( piece == 0 ) {
                    WndNextRow( wnd, WND_NO_ROW, WND_RESTORE_ROW );
                    return( FALSE );
                }
                break;
            }
            if( line.tabstop && piece < last_piece ) {
                found_piece = piece;
            }
        }
        if( found_piece != WND_NO_PIECE ) {
            WndDirtyCurr( wnd );
            if( row < wnd->title_size ) {
                WndScroll( wnd, row - wnd->title_size );
                row = wnd->title_size;
            }
            wnd->current.row = row;
            wnd->current.piece = found_piece;
            wnd->current.col = 0;
            WndSetCurrCol( wnd );
            WndGetLine( wnd, row, found_piece, &line );
            WndDirtyCurr( wnd );
            return( TRUE );
        }
        last_piece = WND_NO_PIECE;
        if( !wrap ) {
            WndNextRow( wnd, WND_NO_ROW, WND_RESTORE_ROW );
            return( FALSE );
        }
    }
}


extern  bool    WndFirstCurrent( a_window *wnd )
{
    WndNoCurrent( wnd );
    return( WndNextCurrent( wnd, TRUE ) );
}


extern  bool    WndLastCurrent( a_window *wnd )
{
    if( !WndFirstCurrent( wnd ) ) return( FALSE );
    while( WndNextCurrent( wnd, TRUE ) );
    while( WndTabLeft( wnd, FALSE ) );
    return( TRUE );
}

extern void     WndCheckCurrentValid( a_window *wnd )
{
    wnd_line_piece      line;

    if( !WndHasCurrent( wnd ) ) return;
    if( !WndGetLine( wnd, wnd->current.row, wnd->current.piece, &line ) ) {
        WndLastCurrent( wnd );
    } else {
        if( wnd->current.row >= wnd->rows ) {
            WndScroll( wnd, wnd->current.row - wnd->rows + 1 );
        }
    }
}

static void WndAdjustCurrCol( a_window *wnd, wnd_line_piece *line )
{
    if( !WndHasCurrent( wnd ) ) return;
    if( line->length == 0 ) {
        wnd->current.col = 0;
    } else if( wnd->current_col >= line->length ) {
        wnd->current.col = WndLastCharCol( line );
    } else {
        wnd->current.col = wnd->current_col;
    }
}


void WndCurrVisible( a_window *wnd )
{
    wnd_line_piece      line;
    int                 len;

    if( !WndHasCurrent( wnd ) ) return;
    if( wnd != WndFindActive() ) return;
    if( WndGetLine( wnd, wnd->current.row, wnd->current.piece, &line ) ) {
        WndAdjustCurrCol( wnd, &line );
        /* try to make whole selection visible for searching */
        if( wnd->sel_start.row == wnd->current.row &&
            wnd->sel_start.piece == wnd->current.piece &&
            wnd->sel_start.col == wnd->current.col &&
            wnd->sel_end.row == wnd->current.row &&
            wnd->sel_end.piece == wnd->current.piece ) {
            len = wnd->sel_end.col - wnd->sel_start.col;
            if( len < 0 ) len = -len;
            ++len;
        } else {
            len = 1;
        }
        WndHScrollToCurr( wnd, len );
        if( line.length == 0 ) {
            wnd->current.col = 0;
        } else if( wnd->current.col >= line.length ) {
            wnd->current.col = WndLastCharCol( &line );
        }
    }
}


extern  void    WndDirtyCurr( a_window *wnd )
{
    if( WndHasCurrent( wnd ) ) {
        if( _Is( wnd, WSW_CHAR_CURSOR ) && _Isnt( wnd, WSW_HIGHLIGHT_CURRENT ) ) {
            WndDirtyScreenChar( wnd, &wnd->current );
        } else {
            WndDirtyScreenPiece( wnd, &wnd->current );
        }
        if( _Is( wnd, WSW_SELECTING ) ) {
            WndSelPieceChange( wnd, &wnd->current );
        }
    }
    WndEvent( wnd, GUI_NO_EVENT, NULL );
}


extern  void    WndDirtyCurrChar( a_window *wnd )
{
    if( WndHasCurrent( wnd ) ) {
        WndDirtyScreenChar( wnd, &wnd->current );
        if( _Is( wnd, WSW_SELECTING ) ) {
            WndSelPieceChange( wnd, &wnd->current );
        }
    }
}

extern void WndSetCurrCol( a_window *wnd )
{
    wnd->current_col = wnd->current.col;
}
