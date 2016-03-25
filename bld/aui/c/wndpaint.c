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

#define WSW_NOT_TO_SCREEN       WSW_UTIL_1

static void WndDrawSelect( a_window *wnd, wnd_line_piece *line,
                           wnd_row row, int piece )
{
    int                 first;
    int                 len;
    gui_ord             indent;

    if( _Is( wnd, WSW_NOT_TO_SCREEN ) ) return;
    if( WndSelected( wnd, line, row, piece, &first, &len ) ){
        indent = line->indent;
        if( first != 0 ) {
            indent += GUIGetExtentX( wnd->gui, line->text, first );
        }
        GUIDrawText( wnd->gui, line->text+first, len, row, indent, WndSelectedAttr );
    }
}


static void WndDrawTheLine( a_window *wnd, wnd_line_piece *line,
                            wnd_row row )
{
    gui_ord             extent;
    gui_point           start,end;
    gui_ord             max_y;
    gui_ord             max_x;
    wnd_bar_info        *bar_info;

    if( _Isnt( wnd, WSW_NOT_TO_SCREEN ) ) {
        if( line->underline ) {
            max_y = WndMaxCharY( wnd );
            start.x = line->indent;
            start.y = row * max_y + max_y / 2;
            end.x = line->indent + line->extent;
            end.y = start.y;
            GUIDrawLine( wnd->gui, &start, &end, GUI_PEN_SOLID, 0, line->attr );
            return;
        } else if( line->vertical_line ) {
            max_y = WndMaxCharY( wnd );
            max_x = WndAvgCharX( wnd );
            start.x = line->indent + max_x / 2;
            end.x = start.x;
            start.y = row * max_y;
            end.y = start.y + max_y;
            GUIDrawLine( wnd->gui, &start, &end, GUI_PEN_SOLID, 0, line->attr );
            return;
        } else if( line->draw_hook | line->draw_line_hook ) {
            if( GUIIsGUI() ) {
                max_y = WndMaxCharY( wnd );
                max_x = WndAvgCharX( wnd );
                start.x = line->indent + max_x / 2;
                end.x = start.x + max_x;
                start.y = row * max_y + max_y / 2;
                end.y = start.y;
                GUIDrawLine( wnd->gui, &start, &end, GUI_PEN_SOLID, 0, line->attr );
                start.x = line->indent + max_x / 2;
                end.x = start.x;
                start.y = row * max_y;
                end.y = start.y;
                if( line->draw_hook ) {
                    end.y += max_y / 2;
                } else {
                    end.y += max_y;
                }
                GUIDrawLine( wnd->gui, &start, &end, GUI_PEN_SOLID, 0, line->attr );
            } else {
                char    ch[3];
                if( line->draw_hook ) {
                    ch[0] = GUIGetCharacter( GUI_INACT_FRAME_LL_CORNER );
                } else {
                    ch[0] = GUIGetCharacter( GUI_INACT_RIGHT_TITLE_MARK );
                }
                ch[1] = GUIGetCharacter( GUI_INACT_FRAME_BOTTOM );
                ch[2] = '\0';
                GUIDrawText( wnd->gui, ch, 2, row, line->indent, line->attr );
            }
            return;
        } else if( line->draw_bar ) {
            bar_info = (wnd_bar_info *)line->text;
            if( bar_info->bar_group ) {
                GUIDrawBarGroup( wnd->gui, row, line->indent,
                            line->extent - bar_info->bar_size2,
                            bar_info->bar_size2, bar_info->bar_style,
                            bar_info->bar_colour, bar_info->bar_colour2,
                            bar_info->bar_selected );
            } else {
                GUIDrawBar( wnd->gui, row, line->indent, line->extent,
                            bar_info->bar_style, bar_info->bar_colour,
                            bar_info->bar_selected );
            }
            return;
        } else if( line->bitmap ) {
            GUIDrawHotSpot( wnd->gui, line->text[0],
                            row, line->indent, line->attr );
            return;
        } else if( line->extent == WND_NO_EXTEND ) {
            GUIDrawText( wnd->gui, line->text, line->length,
                         row, line->indent, line->attr );
        } else {
            GUIDrawTextExtent( wnd->gui, line->text, line->length,
                         row, line->indent, line->attr, line->extent );
        }
    }
    extent = GUIGetExtentX( wnd->gui, line->text, line->length );
    if( line->extent != WND_MAX_EXTEND ) {
        if( line->extent > extent ) {
            extent = line->extent;
        }
    }
    if( line->indent + extent > wnd->max_indent ) {
        wnd->max_indent = line->indent + extent;
    }
}


static void WndDrawCursor( a_window *wnd, wnd_line_piece *line,
                           wnd_row row, int piece )
{
    const char  *p;

    if( _Is( wnd, WSW_NOT_TO_SCREEN ) ) return;
    if( _Isnt( wnd, WSW_CHAR_CURSOR ) ) return;
    if( !line->tabstop ) return;
    if( wnd->current.row != row ) return;
    if( wnd->current.piece != piece ) return;
    if( wnd->current.col < 0 ) return;
    if( line->length == 0 ) {
        GUIDrawText( wnd->gui, " ", 1, row, line->indent, WndCursorAttr );
    } else if( wnd->current.col < line->length ) {
        line->indent += GUIGetExtentX( wnd->gui, line->text, wnd->current.col );
        p = line->text + wnd->current.col;
        GUIDrawText( wnd->gui, p, GUICharLen( *p ),
                     row, line->indent, WndCursorAttr );
    }
}

static void    WndPaintRows( a_window *wnd, wnd_row start_row, int num )
{
    wnd_row             row;
    wnd_row             row_to_get;
    int                 piece;
    wnd_line_piece      line;
    wnd_row             notify_row;
    int                 notify_piece;
    wnd_attr            piece0_attr;
    wnd_attr            prev_attr;
    bool                had_cache;

    had_cache = WndSetCache( wnd, FALSE );
    WndBegPaint( wnd, start_row, num );
    notify_row = WND_NO_ROW;
    prev_attr = 0;
    for( row = start_row; row < start_row + num; ++row ) {
        for( piece = 0; ; ++piece ) {
            if( row < wnd->title_size ) {
                row_to_get = row - wnd->top;
            } else {
                row_to_get = row;
            }
            if( !WndGetLine( wnd, row_to_get, piece, &line ) ) break;
            if( line.tabstop && wnd->current.row == row &&
                wnd->current.piece == piece ) {
                notify_row = row;
                notify_piece = piece;
                if( _Is( wnd, WSW_HIGHLIGHT_CURRENT ) ) {
                    line.attr = WndMapTabAttr( line.attr );
                }
            }
            if( piece == 0 ) {
                piece0_attr = line.attr;
            } else if( line.use_piece0_attr ) {
                line.attr = piece0_attr;
            } if( line.use_prev_attr ) {
                line.attr = prev_attr;
            }
            prev_attr = line.attr;
            WndDrawTheLine( wnd, &line, row );
            if( !line.bitmap ) {
                WndDrawSelect( wnd, &line, row, piece );
                WndDrawCursor( wnd, &line, row, piece );
            }
        }
    }
    WndEndPaint( wnd, start_row, num );
    WndSetCache( wnd, had_cache );
    if( wnd->max_indent != 0 ) {
        GUISetHScrollRange( wnd->gui, wnd->max_indent );
    }
    if( notify_row != WND_NO_ROW ) {
        WndNotify( wnd, notify_row, notify_piece );
    }
}


void    WndProcPaint( a_window *wnd, void *parm )
{
    wnd_row             row;
    int                 num;

    GUI_GET_ROWS( parm, row, num );
    WndPaintRows( wnd, row, num );
}


void    WndForcePaint( a_window *wnd )
{
    _Set( wnd, WSW_NOT_TO_SCREEN );
    if( wnd->max_indent == 0 ) WndPaintRows( wnd, 0, wnd->rows );
    _Clr( wnd, WSW_NOT_TO_SCREEN );
}
