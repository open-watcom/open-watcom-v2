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

extern void WndFixedThumb( a_window *wnd )
{
    WndSetVScrollRange( wnd, WndRows( wnd ) * 2 );
    WndSetThumbPercent( wnd, 50 );
}


extern void WndSetThumbPercent( a_window *wnd, int percent )
{
    GUISetVScrollThumb( wnd->gui, percent );
}

extern void WndSetThumbPos( a_window *wnd, int pos )
{
    GUIInitVScrollRow( wnd->gui, pos );
}

extern  void    WndSetVScrollRange( a_window *wnd, wnd_row rows )
{
    if( rows > wnd->max_row ) wnd->max_row = rows;
    GUISetVScrollRangeRows( wnd->gui, rows );
}

extern  void    WndResetScroll( a_window *wnd )
{
    wnd->max_indent = 0;
    wnd->max_row = 0;
}

extern gui_ord  WndVScrollWidth( a_window *wnd )
{
    gui_system_metrics metrics;

    wnd = wnd;
    GUIGetSystemMetrics( &metrics );
    return( metrics.scrollbar_size.x );
}

extern bool WndHasNumRows( a_window *wnd )
{
    return( wnd->info->numrows != NoNumRows );
}

extern void WndSetThumb( a_window *wnd )
{
    int         thumb;
    int         rows;
    int         bottom_blank;
    int         scrolled;

    if( WndHasNumRows( wnd ) ) {
        scrolled = 0;
        rows = WndNumRows( wnd );
        bottom_blank = wnd->rows - ( rows - wnd->top );
        if( bottom_blank >= wnd->top ) bottom_blank = wnd->top;
        if( bottom_blank > 0 ) {
            if( WndHasCurrent( wnd ) ) {
                WndDirtyCurr( wnd );
            }
            scrolled = WndScroll( wnd, -bottom_blank );
            if( WndHasCurrent( wnd ) ) {
                wnd->current.row += bottom_blank;
                WndDirtyCurr( wnd );
            }
        }
        if( wnd->rows >= rows ) {
            thumb = 0;
            if( scrolled != 0 ) WndRepaint( wnd );
        } else {
            thumb =  wnd->top * 100L / ( rows - wnd->rows );
        }
        if( thumb == 0 && wnd->top != 0 ) thumb = 1;
        WndSetVScrollRange( wnd, rows );
        WndSetThumbPos( wnd, wnd->top );
    }
}

extern WNDSCROLL WndScroll;
int WndScroll( a_window * wnd, int lines )
{
    int         new_top;
    int         total_rows;
    int         rows;
    wnd_line_piece      line;

    if( lines == 0 ) return( 0 );
    WndNoSelect( wnd );
    rows = wnd->rows;
    WndDirtyCurr( wnd );
    if( !WndHasNumRows( wnd ) ) {
        WndKillCacheLines( wnd );
        lines = wnd->info->scroll( wnd, lines );
        if( lines != 0 && !( wnd->switches & WSW_REPAINT ) ) {
            WndAdjustDirt( wnd, -lines );
            GUIDoVScrollClip( wnd->gui, lines, wnd->title_size, wnd->rows - 1 );
        }
    } else {
        total_rows = WndNumRows( wnd );
        new_top = wnd->top + lines;
        if( new_top > total_rows - rows ) new_top = total_rows - rows;
        if( new_top < 0 ) new_top = 0;
        lines = new_top - wnd->top;
        if( lines == 0 ) return( lines );
        wnd->top = new_top;
        WndSetThumb( wnd );
        wnd->vscroll_pending += lines;
        WndAdjustDirt( wnd, -lines );
    }
    WndDirtyCurr( wnd );
    if( !WndGetLine( wnd, wnd->current.row, wnd->current.piece, &line ) ) {
        WndNoCurrent( wnd );
    }
    WndCurrVisible( wnd );
    WndDirtyCurr( wnd );
    return( lines );
}

extern int WndScrollAbs( a_window * wnd, int line )
{
    return( WndScroll( wnd, WndScreenRow( wnd, line ) ) );
}

extern  void    WndSetHScroll( a_window *wnd, gui_ord indent )
{
    wnd->hscroll_pending = indent;
}

static gui_ord  WndHScrollPos( a_window *wnd )
{
    if( wnd->hscroll_pending != -1 ) {
        return( wnd->hscroll_pending );
    } else {
        return( GUIGetHScroll( wnd->gui ) );
    }
}


static gui_ord HScrollTo( a_window *wnd, gui_ord left, gui_ord rite )
{
    gui_ord             hscroll;

    hscroll = WndHScrollPos( wnd );
    if( left < hscroll ) {
        return( left );
    } else if( rite > hscroll + wnd->width ) {
        return( rite - wnd->width );
    } else {
        return( hscroll );
    }
}


extern  gui_ord WndCurrHScrollPos( a_window *wnd, int len )
{
    gui_ord             whole_extent;
    gui_ord             sel_extent;
    gui_ord             sel_indent;
    gui_ord             hscroll;
    wnd_line_piece      line;

    WndGetLine( wnd, wnd->current.row, wnd->current.piece, &line );
    whole_extent = GUIGetExtentX( wnd->gui, line.text, line.length );
    if( whole_extent <= wnd->width && _Isnt( wnd, WSW_CHAR_CURSOR ) ) {
        hscroll = HScrollTo( wnd, line.indent, line.indent + whole_extent );
    } else {
        sel_extent = GUIGetExtentX( wnd->gui, line.text+wnd->current.col, len );
        sel_indent = GUIGetExtentX( wnd->gui, line.text, wnd->current.col );
        hscroll = HScrollTo( wnd, line.indent + sel_indent,
                   line.indent + sel_indent + sel_extent );
    }
    return( hscroll );
}


extern  void    WndHScrollToCurr( a_window *wnd, int len )
{
    gui_ord     hscroll;

    hscroll = WndCurrHScrollPos( wnd, len );
    if( hscroll != WndHScrollPos( wnd ) ) {
        WndSetHScroll( wnd, hscroll );
    }
}

extern void WndHScrollNotify( a_window *wnd )
{
    if( !WndHasCurrent( wnd ) ) return;
    if( WndHScrollPos( wnd ) != WndCurrHScrollPos( wnd, 1 ) ) {
        WndDirtyScreenPiece( wnd, &wnd->current );
        WndNoCurrent( wnd );
    }
}
