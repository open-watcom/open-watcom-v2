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

bool    WndSetPoint( a_window wnd, void *parm, bool exact,
                             wnd_coord *spot, wnd_row row,
                             bool doing_select )
{
    gui_point           point;
    wnd_colidx          colidx;
    wnd_piece           piece;
    wnd_piece           last_piece;
    wnd_colidx          last_colidx;
    wnd_piece           last_extended_tab_piece;
    wnd_colidx          last_extended_tab_colidx;
    bool                got;
    bool                allowed_in_tab;
    wnd_line_piece      line;
    gui_coord           indent;

    spot->row = WND_NO_ROW;
    spot->piece = WND_NO_PIECE;
    spot->colidx = WND_NO_COLIDX;
    if( doing_select ) {
        allowed_in_tab = WndSwitchOn( wnd, WSW_SELECT_IN_TABSTOP );
    } else {
        allowed_in_tab = true;
    }
    GUI_GET_POINT( parm, point );
    if( point.x < 0 )
        point.x = 0;
    if( point.y < 0 )
        point.y = 0;
    if( row == WND_NO_ROW )
        row = GUIGetRow( wnd->gui, &point );
    if( row > wnd->rows )
        return( false );
    if( row < wnd->title_rows ) {
        row -= wnd->top;
    }
    last_piece = WND_NO_PIECE;
    last_colidx = 0;
    last_extended_tab_piece = WND_NO_PIECE;
    last_extended_tab_colidx = 0;
    for( piece = 0; ; ++piece ) {
        got = WndGetLine( wnd, row, piece, &line );
        indent.x = line.indent;
        indent.y = 0;
        GUITruncToPixel( &indent );
        line.indent = indent.x;
        if( line.tabstop && ( line.extent == WND_MAX_EXTEND || line.master_tabstop ) ) { // nyi ??
            if( last_extended_tab_piece == WND_NO_PIECE ) {
                last_extended_tab_piece = piece;
                last_extended_tab_colidx = WndLastCharColIdx( &line );
            }
        }
        if( !got || line.indent > point.x ) {
            if( last_piece == WND_NO_PIECE )
                return( false );
            spot->row = row;
            if( !doing_select && last_extended_tab_piece != WND_NO_PIECE ) {
                spot->piece = last_extended_tab_piece;
                spot->colidx = last_extended_tab_colidx;
            } else {
                spot->piece = last_piece;
                spot->colidx = last_colidx;
            }
            return( true );
        }
        if( line.bitmap ) {
            if( doing_select )
                continue;
            if( line.indent <= point.x && line.indent + line.length > point.x ) {
                spot->row = row;
                spot->piece = piece;
                spot->colidx = 0;
                return( true );
            }
        } else {
            gui_text_ord    text_pos;

            text_pos = GUIGetStringPos( wnd->gui, line.indent, line.text, point.x );
            if( text_pos != GUI_TEXT_NO_COLUMN ) { // clicked within this piece
                colidx = (wnd_colidx)text_pos;
                if( !doing_select && last_extended_tab_piece != WND_NO_PIECE ) {
                    spot->piece = last_extended_tab_piece;
                    spot->colidx = last_extended_tab_colidx;
                } else {
                    if( !allowed_in_tab && line.tabstop )
                        return( false );
                    if( line.static_text )
                        return( false );
                    spot->piece = piece;
                }
                spot->row = row;
                spot->colidx = WndCharColIdx( line.text, colidx );
                return( true );
            } else if( line.extent == WND_MAX_EXTEND || line.master_tabstop || !exact ) { // nyi ??
                // clicked to right of this hunk -- remember it
                last_piece = piece;
                last_colidx = WndLastCharColIdx( &line );
            }
        }
    }
}


void    WndGetCurrent( a_window wnd, wnd_row *row, wnd_piece *piece )
{
    *row = WND_NO_ROW;
    if( !WndHasCurrent( wnd ) )
        return;
    *row = WndVirtualRow( wnd, wnd->current.row );
    *piece = wnd->current.piece;
}


void    WndNewCurrent( a_window wnd, wnd_row row, wnd_piece piece )
{
    wnd->current.row = WndScreenRow( wnd, row );
    wnd->current.piece = piece;
    wnd->current.colidx = 0;
    WndSetCurrCol( wnd );
    WndDirtyCurr( wnd );
}


void WndMoveCurrent( a_window wnd, wnd_row row, wnd_piece piece )
{
    WndDirtyCurr( wnd );
    if( row < WndTop( wnd ) ) {
        WndVScroll( wnd, row - WndTop( wnd ) );
    } else if( row >= WndTop( wnd ) + WndRows( wnd ) ) {
        WndVScroll( wnd, row - WndTop( wnd ) - WndRows( wnd ) + 1 );
    }
    WndNewCurrent( wnd, row, piece );
}


bool    WndHasCurrent( a_window wnd )
{
    return( wnd->current.row != WND_NO_ROW );
}


void    WndNoCurrent( a_window wnd )
{
    wnd->current.row = WND_NO_ROW;
    wnd->current.piece = 0;
    wnd->current.colidx = 0;
    WndSetCurrCol( wnd );
}


wnd_row WndCurrRow( a_window wnd )
{
    if( !WndHasCurrent( wnd ) )
        return( WND_NO_ROW );
    return( WndVirtualRow( wnd, wnd->current.row ) );
}


bool    WndNextCurrent( a_window wnd, bool wrap )
{
    wnd_row             row;
    wnd_piece           piece;
    wnd_line_piece      line;

    if( WndHasCurrent( wnd ) ) {
        piece = wnd->current.piece + 1;
        row = wnd->current.row;
    } else {
        row = wnd->title_rows;
        piece = 0;
    }
    WndNextRow( wnd, WND_NO_ROW, WND_SAVE_ROW );
    for( ;; ) {
        for( ;; ) {
            if( !WndGetLine( wnd, row, piece, &line ) )
                break;
            if( line.tabstop ) {
                WndDirtyCurr( wnd );
                if( row >= wnd->rows ) {
                    WndVScroll( wnd, row - wnd->rows + 1 );
                    row = wnd->rows - 1;
                }
                wnd->current.row = row;
                wnd->current.piece = piece;
                wnd->current.colidx = 0;
                WndSetCurrCol( wnd );
                WndDirtyCurr( wnd );
                return( true );
            }
            ++piece;
        }
        if( !wrap ) {
            WndNextRow( wnd, WND_NO_ROW, WND_RESTORE_ROW );
            return( false );
        }
        row = WndNextRow( wnd, row, 1 );
        piece = 0;
        if( !WndGetLine( wnd, row, piece, &line ) ) {
            WndNextRow( wnd, WND_NO_ROW, WND_RESTORE_ROW );
            return( false );
        }
    }
}


bool WndPrevCurrent( a_window wnd, bool wrap )
{
    wnd_piece           piece;
    wnd_piece           last_piece;
    wnd_piece           found_piece;
    wnd_row             row;
    wnd_line_piece      line;

    if( !WndHasCurrent( wnd ) )
        return( false );
    row = wnd->current.row;
    last_piece = wnd->current.piece;
    WndNextRow( wnd, WND_NO_ROW, WND_SAVE_ROW );
    for( row = wnd->current.row;; row = WndNextRow( wnd, row, -1 ) ) {
        found_piece = WND_NO_PIECE;
        for( piece = 0; piece < last_piece; ++piece ) {
            if( !WndGetLine( wnd, row, piece, &line ) ) {
                if( piece == 0 ) {
                    WndNextRow( wnd, WND_NO_ROW, WND_RESTORE_ROW );
                    return( false );
                }
                break;
            }
            if( line.tabstop && piece < last_piece ) {
                found_piece = piece;
            }
        }
        if( found_piece != WND_NO_PIECE ) {
            WndDirtyCurr( wnd );
            if( row < wnd->title_rows ) {
                WndVScroll( wnd, row - wnd->title_rows );
                row = wnd->title_rows;
            }
            wnd->current.row = row;
            wnd->current.piece = found_piece;
            wnd->current.colidx = 0;
            WndSetCurrCol( wnd );
            WndGetLine( wnd, row, found_piece, &line );
            WndDirtyCurr( wnd );
            return( true );
        }
        last_piece = WND_NO_PIECE;
        if( !wrap ) {
            WndNextRow( wnd, WND_NO_ROW, WND_RESTORE_ROW );
            return( false );
        }
    }
}


bool    WndFirstCurrent( a_window wnd )
{
    WndNoCurrent( wnd );
    return( WndNextCurrent( wnd, true ) );
}


bool    WndLastCurrent( a_window wnd )
{
    if( !WndFirstCurrent( wnd ) )
        return( false );
    while( WndNextCurrent( wnd, true ) );
    while( WndTabLeft( wnd, false ) );
    return( true );
}

void     WndCheckCurrentValid( a_window wnd )
{
    wnd_line_piece      line;

    if( !WndHasCurrent( wnd ) )
        return;
    if( !WndGetLine( wnd, wnd->current.row, wnd->current.piece, &line ) ) {
        WndLastCurrent( wnd );
    } else {
        if( wnd->current.row >= wnd->rows ) {
            WndVScroll( wnd, wnd->current.row - wnd->rows + 1 );
        }
    }
}

static void WndAdjustCurrColIdx( a_window wnd, wnd_line_piece *line )
{
    if( !WndHasCurrent( wnd ) )
        return;
    if( line->length == 0 ) {
        wnd->current.colidx = 0;
    } else if( wnd->current_colidx >= line->length ) {
        wnd->current.colidx = WndLastCharColIdx( line );
    } else {
        wnd->current.colidx = wnd->current_colidx;
    }
}


void WndCurrVisible( a_window wnd )
{
    wnd_line_piece      line;
    size_t              len;

    if( !WndHasCurrent( wnd ) )
        return;
    if( wnd != WndFindActive() )
        return;
    if( WndGetLine( wnd, wnd->current.row, wnd->current.piece, &line ) ) {
        WndAdjustCurrColIdx( wnd, &line );
        /* try to make whole selection visible for searching */
        if( wnd->sel_start.row == wnd->current.row &&
            wnd->sel_start.piece == wnd->current.piece &&
            wnd->sel_start.colidx == wnd->current.colidx &&
            wnd->sel_end.row == wnd->current.row &&
            wnd->sel_end.piece == wnd->current.piece ) {
            if( wnd->sel_end.colidx < wnd->sel_start.colidx ) {
                len = wnd->sel_start.colidx - wnd->sel_end.colidx + 1;
            } else {
                len = wnd->sel_end.colidx - wnd->sel_start.colidx + 1;
            }
        } else {
            len = 1;
        }
        WndHScrollToCurr( wnd, len );
        if( line.length == 0 ) {
            wnd->current.colidx = 0;
        } else if( wnd->current.colidx >= line.length ) {
            wnd->current.colidx = WndLastCharColIdx( &line );
        }
    }
}


void    WndDirtyCurr( a_window wnd )
{
    if( WndHasCurrent( wnd ) ) {
        if( WndSwitchOn( wnd, WSW_CHAR_CURSOR ) && WndSwitchOff( wnd, WSW_HIGHLIGHT_CURRENT ) ) {
            WndDirtyScreenChar( wnd, &wnd->current );
        } else {
            WndDirtyScreenPiece( wnd, &wnd->current );
        }
        if( WndSwitchOn( wnd, WSW_SELECTING ) ) {
            WndSelPieceChange( wnd, &wnd->current );
        }
    }
    WNDEVENT( wnd, GUI_NO_EVENT, NULL );
}


void    WndDirtyCurrChar( a_window wnd )
{
    if( WndHasCurrent( wnd ) ) {
        WndDirtyScreenChar( wnd, &wnd->current );
        if( WndSwitchOn( wnd, WSW_SELECTING ) ) {
            WndSelPieceChange( wnd, &wnd->current );
        }
    }
}

void WndSetCurrCol( a_window wnd )
{
    wnd->current_colidx = wnd->current.colidx;
}
