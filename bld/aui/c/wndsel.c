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


#include "_aui.h"

wnd_row         WndMenuRow;
wnd_piece       WndMenuPiece;


void WndSelectEvent( a_window wnd, gui_event gui_ev, void *parm )
{
    gui_key             key;
    gui_keystate        state;

    switch( gui_ev ) {
    case GUI_RBUTTONDOWN:
    case GUI_LBUTTONDOWN:
        WndNoSelect( wnd );
        return;
    case GUI_MOUSEMOVE:
        if( WndSwitchOn( wnd, WSW_SELECTING_WITH_KEYBOARD ) )
            break;
        /* fall through */
    case GUI_PAINT:
        return;
    case GUI_KEYDOWN:
    case GUI_KEYUP:
        GUI_GET_KEY_STATE( parm, key, state );
        switch( key ) {
        case GUI_KEY_UP:
        case GUI_KEY_DOWN:
        case GUI_KEY_LEFT:
        case GUI_KEY_RIGHT:
        case GUI_KEY_HOME:
        case GUI_KEY_END:
            if( GUI_SHIFT_STATE( state ) )
                return;
            /* fall through */
        case GUI_KEY_ESCAPE:
            WndNoSelect( wnd );
            break;
        default :
            break;
        }
    default :
        break;
    }
    WndClrSwitches( wnd, WSW_SELECTING | WSW_SELECTING_WITH_KEYBOARD );
}


void    WndSelEnds( a_window wnd, wnd_coord **pstart, wnd_coord **pend )
{
    wnd_coord   *start;
    wnd_coord   *end;

    start = &wnd->sel_start;
    end = &wnd->sel_end;
    if( start->row > end->row ) {
        end = &wnd->sel_start;
        start = &wnd->sel_end;
    } else if( start->row == end->row ) {
        if( start->piece > end->piece ) {
            end = &wnd->sel_start;
            start = &wnd->sel_end;
        } else if( start->piece == end->piece ) {
            if( start->colidx > end->colidx ) {
                end = &wnd->sel_start;
                start = &wnd->sel_end;
            }
        }
    }
    *pstart = start;
    *pend = end;
}

bool     WndSelected( a_window wnd, wnd_line_piece *line, wnd_row row, wnd_piece piece, wnd_colidx *first_colidx, size_t *len )
{
    wnd_coord   *start;
    wnd_coord   *end;

    if( wnd->sel_end.row == WND_NO_ROW )
        return( false );
    if( line->bitmap )
        return( false );

    /* figure out start and end */
    WndSelEnds( wnd, &start, &end );
    if( row == start->row && piece == start->piece ) {
        *first_colidx = start->colidx;
        if( row == end->row && piece == end->piece ) {
            *len = end->colidx - start->colidx + 1;
        } else {
            *len = line->length - start->colidx;
        }
        return( true );
    }
    *first_colidx = 0;
    if( row == end->row && piece == end->piece ) {
        *len = end->colidx + 1;
        return( true );
    }
    *len = line->length;
    if( start->row != end->row ) {
        if( row == start->row ) {
            if( piece > start->piece ) {
                return( true );
            }
        } else if( row == end->row ) {
            if( piece < end->piece ) {
                return( true );
            }
        } else if( row > start->row && row < end->row ) {
            return( true );
        }
    } else if( row == start->row ) {
        if( piece > start->piece && piece < end->piece ) {
            return( true );
        }
    }
    return( false );
}


bool    WndSelSetStart( a_window wnd, void *parm )
{
    WndNoSelect( wnd );
    return( WndSetPoint( wnd, parm, false, &wnd->sel_start, WND_NO_ROW, true ) );
}


bool    WndSelGetEndPiece( a_window wnd, void *parm, wnd_coord *piece )
{
    bool        success;

    if( wnd->sel_start.row == WND_NO_ROW )
        return( false );
    if( WndSwitchOff( wnd, WSW_MULTILINE_SELECT ) ) {
        success = WndSetPoint( wnd, parm, false, piece, wnd->sel_start.row, true );
    } else {
        success = WndSetPoint( wnd, parm, false, piece, WND_NO_ROW, true );
    }
    return( success );
}


bool    WndSelSetEnd( a_window wnd, void *parm )
{
    bool                success;

    if( wnd->sel_start.row == WND_NO_ROW )
        return( false );
    success = WndSelGetEndPiece( wnd, parm, &wnd->sel_end );
    return( success );
}


void     WndSelPieceChange( a_window wnd, wnd_coord *piece )
{
    wnd_coord           old_sel_end;
    wnd_colidx          end_colidx;

    if( wnd->keyindex != 0 )
        return;
    if( wnd->sel_start.row == WND_NO_ROW )
        return;
    if( wnd->sel_end.row == WND_NO_ROW ) {
        old_sel_end = wnd->sel_start;
    } else {
        old_sel_end = wnd->sel_end;
    }
    wnd->sel_end = *piece;
    if( old_sel_end.row != wnd->sel_end.row ) {
        if( old_sel_end.row == wnd->sel_end.row + 1 || old_sel_end.row == wnd->sel_end.row - 1 ) {
            WndDirtyScreenRow( wnd, old_sel_end.row );
            WndDirtyScreenRow( wnd, wnd->sel_end.row );
        } else {
            WndSetRepaint( wnd );
        }
    } else if( old_sel_end.piece != wnd->sel_end.piece ) {
        WndDirtyScreenPiece( wnd, &old_sel_end );
        WndDirtyScreenPiece( wnd, &wnd->sel_end );
    } else if( old_sel_end.colidx != wnd->sel_end.colidx ) {
        if( old_sel_end.colidx > wnd->sel_end.colidx ) {
            end_colidx = old_sel_end.colidx;
            old_sel_end.colidx = wnd->sel_end.colidx;
        } else {
            end_colidx = wnd->sel_end.colidx;
        }
        WndDirtyScreenRange( wnd, &old_sel_end, end_colidx );
    }
}


void     WndSelChange( a_window wnd, void *parm )
{
    wnd_coord   piece;

    WndSelGetEndPiece( wnd, parm, &piece );
    WndSelPieceChange( wnd, &piece );
}

void    WndNoSelect( a_window wnd )
{
    wnd_coord   *start;
    wnd_coord   *end;

    if( WndSwitchOn( wnd, WSW_SELECT_EVENT ) )
        return;
    if( wnd->sel_end.row == WND_NO_ROW )
        return;
    WndSelEnds( wnd, &start, &end );

    if( start->row == end->row ) {
        if( start->piece == end->piece ) {
            WndDirtyScreenRange( wnd, start, end->colidx );
        } else {
            WndDirtyScreenRow( wnd, start->row );
        }
    } else {
        WndSetRepaint( wnd );
    }
    WndNullPopItem( wnd );
    wnd->sel_start.row = WND_NO_ROW;
    wnd->sel_end.row = WND_NO_ROW;
}

wnd_row WndSelRow( a_window wnd )
{
    if( wnd->sel_start.row == WND_NO_ROW )
        return( WND_NO_ROW );
    return( WndVirtualRow( wnd, wnd->sel_start.row ) );
}


void    SetWndMenuRow( a_window wnd )
{
    if( wnd->sel_end.row != WND_NO_ROW ) {
        WndMenuRow = wnd->sel_end.row;
        WndMenuPiece = wnd->sel_end.piece;
    } else if( wnd->current.row != WND_NO_ROW ) {
        WndMenuRow = wnd->current.row;
        WndMenuPiece = wnd->current.piece;
    } else {
        WndMenuRow = WND_NO_ROW;
        WndMenuPiece = WND_NO_PIECE;
    }
}

void WndToSelectMode( a_window wnd )
{
    if( WndHasCurrent( wnd ) && WndSwitchOn( wnd, WSW_CHAR_CURSOR ) ) {
        if( WndSwitchOff( wnd, WSW_SELECTING ) ) {
            WndNoSelect( wnd );
            wnd->sel_start = wnd->current;
            WndSetSwitches( wnd, WSW_SELECTING | WSW_SELECTING_WITH_KEYBOARD );
        }
    }
    WndSetSwitches( wnd, WSW_SELECT_EVENT );
}

void WndEndSelectEvent( a_window wnd )
{
    WndClrSwitches( wnd, WSW_SELECT_EVENT );
}
