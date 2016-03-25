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
#include <stdlib.h>

static  void    DoWndDirtyScreenPiece( a_window *wnd, wnd_row row,
                                       int piece, int col, int end_col )
{
    int         i;
    int         same_row;

    /* check if any piece contains this one */
    same_row = -1;
    for( i = 0; i < wnd->dirtyrects; ++i ) {
        if( wnd->dirty[i].row != row ) continue;
        same_row = i;
        if( wnd->dirty[i].piece == WND_NO_PIECE || piece == WND_NO_PIECE ) {
            wnd->dirty[i].piece = WND_NO_PIECE;
            return;
        }
        if( wnd->dirty[i].piece != piece ) continue;
        if( wnd->dirty[i].col == WND_NO_COL || col == WND_NO_COL ) {
            wnd->dirty[i].col = WND_NO_COL;
            return;
        }
        if( col < wnd->dirty[i].col ) wnd->dirty[i].col = col;
        if( end_col > wnd->dirty[i].end_col ) wnd->dirty[i].end_col = end_col;
        return;
    }
    if( wnd->dirtyrects >= WndMaxDirtyRects ) {
        if( same_row != -1 ) {
            wnd->dirty[same_row].piece = WND_NO_PIECE;
        } else {
            WndRepaint( wnd );
        }
        return;
    }
    wnd->dirty[ (unsigned)wnd->dirtyrects ].row = row;
    wnd->dirty[ (unsigned)wnd->dirtyrects ].piece = piece;
    wnd->dirty[ (unsigned)wnd->dirtyrects ].col = col;
    wnd->dirty[ (unsigned)wnd->dirtyrects ].end_col = end_col;
    wnd->dirtyrects++;
}


extern  void    WndDirtyScreenRange( a_window *wnd, wnd_coord *piece, int end_col )
{
    DoWndDirtyScreenPiece( wnd, piece->row, piece->piece, piece->col, end_col );
}


extern  void    WndDirtyScreenChar( a_window *wnd, wnd_coord *piece )
{
    DoWndDirtyScreenPiece( wnd, piece->row, piece->piece, piece->col, piece->col );
}


extern  void    WndDirtyScreenPiece( a_window *wnd, wnd_coord *piece )
{
    DoWndDirtyScreenPiece( wnd, piece->row, piece->piece, WND_NO_COL, WND_NO_COL );
}


extern  void    WndAdjustDirt( a_window *wnd, int by )
{
    int         i;

    for( i = 0; i < wnd->dirtyrects; ++i ) {
        wnd->dirty[i].row += by;
    }
}


extern  void    WndDirtyScreenRow( a_window *wnd, wnd_row row )
{
    wnd_coord   piece;

    piece.row = row;
    piece.piece = WND_NO_PIECE;
    WndDirtyScreenPiece( wnd, &piece );
}


extern  void    WndDirtyRect( a_window *wnd, gui_ord x, wnd_row y,
                                             gui_ord width, wnd_row height )
{
    gui_rect    rect;

    rect.x = x;
    rect.y = WndScreenRow( wnd, y ) * WndMaxCharY( wnd );
    rect.width = width;
    rect.height = height * WndMaxCharY( wnd );
    GUIWndDirtyRect( wnd->gui, &rect );
}


extern  void    WndRowDirty( a_window *wnd, wnd_row row )
{
    WndKillCacheEntries( wnd, row, WND_NO_PIECE );
    WndDirtyScreenRow( wnd, WndScreenRow( wnd, row ) );
}


extern  void    WndRowDirtyImmed( a_window *wnd, wnd_row row )
{
    int         screen_row;

    screen_row = WndScreenRow( wnd, row );
    if( screen_row > 0 && screen_row < wnd->rows ) {
        WndKillCacheEntries( wnd, row, WND_NO_PIECE );
        GUIWndDirtyRow( wnd->gui, screen_row );
    }
}


extern  void    WndPieceDirty( a_window *wnd, wnd_row row, int piece )
{
    wnd_coord   coord;

    WndKillCacheEntries( wnd, row, piece );
    coord.row = WndScreenRow( wnd, row );
    coord.piece = piece;
    WndDirtyScreenPiece( wnd, &coord );
}


bool            WndDoingRefresh = { FALSE };

extern  bool    WndStopRefresh( bool stop )
{
    bool        old;

    old = WndDoingRefresh;
    WndDoingRefresh = stop;
    return( old );
}

void WndPaintDirt( a_window *wnd )
{
    int                 i;
    wnd_line_piece      line;
    wnd_line_piece      next_piece_line;
    gui_rect            rect;
    wnd_dirt            *dirt;
    gui_coord           size;
    gui_coord           half_char;
    a_window            *next;
//    a_window            *last;

//    last = NULL;
    for( wnd = WndNext( NULL ); wnd != NULL; wnd = next ) {
        next = WndNext( wnd );
        if( wnd->vscroll_pending != 0 ) {
            if( wnd->hscroll_pending != -1 ) {
                _Set( wnd, WSW_REPAINT );
            }
            if( _Is( wnd, WSW_REPAINT ) ) {
                if( wnd->hscroll_pending != -1 ) {
                    GUIInitHScroll( wnd->gui, wnd->hscroll_pending );
                }
                WndRepaint( wnd );
                wnd->hscroll_pending = -1;
                wnd->vscroll_pending = 0;
            } else {
                for( i = 0; i < wnd->title_size; ++i ) {
                    GUIDrawTextExtent( wnd->gui, " ", 1, i, 0, GUI_BACKGROUND, GUI_NO_COLUMN );
                }
                GUIDoVScrollClip( wnd->gui, wnd->vscroll_pending,
                                  wnd->title_size, wnd->rows - 1 );
                wnd->vscroll_pending = 0;
            }
        }
        if( _Is( wnd, WSW_REPAINT ) ) {
            _Clr( wnd, WSW_REPAINT );
            WndKillCacheLines( wnd );
            WndCheckCurrentValid( wnd );
            GUIWndDirty( wnd->gui );
            if( wnd->max_indent != 0 && wnd->width >= wnd->max_indent ) {
                GUISetHScroll( wnd->gui, 0 );
                wnd->hscroll_pending = -1;
                GUISetHScrollRange( wnd->gui, wnd->max_indent );
            }
            next = wnd;
        } else {
            for( i = 0; i < wnd->dirtyrects; ++i ) {
                dirt = &wnd->dirty[i];
                if( dirt->row < 0 ) continue;
                if( dirt->row >= wnd->rows ) continue;
                if( dirt->piece == WND_NO_PIECE ) {
                    if( !WndGetLine( wnd, dirt->row, 0, &line ) ) continue;
                    GUIWndDirtyRow( wnd->gui, dirt->row );
                } else {
                    if( !WndGetLine( wnd, dirt->row, dirt->piece, &line ) ) continue;
                    if( line.bitmap ) {
                        GUIGetHotSpotSize( line.text[0], &size );
                        rect.x = line.indent;
                        rect.y = dirt->row * wnd->max_char.y;
                        rect.width = line.length;
                        rect.height = size.y;
                    } else if( dirt->col != WND_NO_COL ) {
                        if( line.length == 0 ) line.text = " ";
                        rect.x = line.indent;
                        rect.x += GUIGetExtentX(wnd->gui, line.text, dirt->col);
                        rect.y = dirt->row * wnd->max_char.y;
                        rect.width = GUIGetExtentX( wnd->gui, line.text+dirt->col, dirt->end_col-dirt->col+GUICharLen( line.text[dirt->col] ) );
                        rect.height = wnd->max_char.y;
                    } else if( line.extent == WND_MAX_EXTEND || line.master_tabstop ) {
                        rect.width = 0;
                        rect.height = 0;
                        GUIWndDirtyRow( wnd->gui, dirt->row );
                    } else {
                        rect.x = line.indent;
                        if( WndGetLine( wnd, dirt->row, dirt->piece+1, &next_piece_line ) ) {
                            if( next_piece_line.indent < line.indent ) {
                                rect.width = WND_APPROX_SIZE;
                            } else {
                                rect.width = next_piece_line.indent - line.indent;
                            }
                        } else {
                            rect.width = WND_APPROX_SIZE;
                        }
                        rect.y = dirt->row * wnd->max_char.y;
                        rect.height = wnd->max_char.y;
                    }
                    /* begin kludge for Kerning problem */
                    if( rect.width != 0 || rect.height != 0 ) {
                        half_char.x = WndAvgCharX( wnd ) / 2;
                        half_char.y = 0;
                        GUITruncToPixel( &half_char );
                        if( rect.x < half_char.x ) {
                            rect.width += half_char.x + rect.x;
                            rect.x = 0;
                        } else {
                            rect.x -= half_char.x;
                            rect.width += half_char.x + half_char.x;
                        }
                        GUIWndDirtyRect( wnd->gui, &rect );
                    }
                }
            }
        }
        if( wnd->hscroll_pending != -1 ) {
            GUISetHScroll( wnd->gui, wnd->hscroll_pending );
            wnd->hscroll_pending = -1;
        }
        wnd->dirtyrects = 0;
        WndSetThumb( wnd );
    }
}

extern  void    WndFreshAll()
{
    a_window            *wnd;

    if( WndDoingRefresh )
        return;
    WndDoingRefresh = TRUE;
    for( wnd = WndNext( NULL ); wnd != NULL; wnd = WndNext( wnd ) ) {
        if( wnd->info->chkflags != NULL && wnd->info->chkflags( wnd->info->flags ) ) {
            WndRefresh( wnd );
        }
    }
    WndStartFreshAll();
    WndPaintDirt( NULL );
    WndEndFreshAll();
    WndDoingRefresh = FALSE;
}
