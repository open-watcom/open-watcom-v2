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


#include "guiwind.h"
#include "guiscale.h"
#include "guidraw.h"
#include "guixutil.h"

static bool GUIIntersectRect( SAREA *area, SAREA *bound )
{
    if( ( ( area->row + area->height ) < bound->row ) ||
        ( area->row > ( bound->row + bound->height ) ) ) {
        return( FALSE );
    }
    if( area->row < bound->row ) {
        area->height -= ( bound->row - area->row );
        area->row = bound->row;
    }
    if( ( area->row + area->height ) > ( bound->row + bound->height ) ) {
        if( area->row > bound->row ) {
            area->height = bound->row + bound->height - area->row;
        } else {
            area->height = bound->height;
        }
    }
    if( ( ( area->col + area->width ) < bound->col ) ||
        ( area->col > ( bound->col + bound->width ) ) ) {
        return( FALSE );
    }
    if( area->col < bound->col ) {
        area->width -= ( bound->col - area->col );
        area->col = bound->col;
    }
    if( ( area->col + area->width ) > ( bound->col + bound->width ) ) {
        if( area->col > bound->col ) {
            area->width = bound->col + bound->width - area->col;
        } else {
            area->width = bound->width;
        }
    }
    return( TRUE );
}

static bool AdjustRect( gui_window *wnd, SAREA *area )
{
    if( ( wnd->hgadget != NULL ) && !GUI_HSCROLL_EVENTS_SET( wnd ) ) {
        if( ( area->col + area->width ) < wnd->hgadget->pos ) {
            return( FALSE );
        } else {
            if( area->col < wnd->hgadget->pos ) {
                area->width -= ( wnd->hgadget->pos - area->col );
                area->col = 0;
            } else {
                area->col -= wnd->hgadget->pos;
            }
        }
    }
    if( ( wnd->vgadget != NULL ) && !GUI_VSCROLL_EVENTS_SET( wnd ) ) {
        if( ( area->row + area->height ) < wnd->vgadget->pos ) {
            return( FALSE );
        } else {
            if( area->row < wnd->vgadget->pos ) {
                area->height -= ( wnd->vgadget->pos - area->row );
                area->row = 0;
            } else {
                area->row -= wnd->vgadget->pos;
            }
        }
    }
    area->col += wnd->use.col;
    area->row += wnd->use.row;
    return( GUIIntersectRect( area, &wnd->dirty ) );
}

static bool DrawRect( gui_window *wnd, gui_rect *rect, gui_attr attr,
                      bool fill, bool outline, char draw_char )
{
    SAREA       area;
    gui_coord   coord;

    if( ( rect->width == 0 ) || ( rect->height == 0 ) ||
        ( ( wnd->flags & CONTENTS_INVALID ) == 0 ) ) {
        return( FALSE );
    }
    coord.x = rect->x;
    coord.y = rect->y;
    GUIScaleToScreenR( &coord );
    area.col = coord.x;
    area.row = coord.y;
    coord.x = rect->width;
    coord.y = rect->height;
    GUIScaleToScreenR( &coord );
    area.width = coord.x;
    area.height = coord.y;
    if( AdjustRect( wnd, &area ) ) {
        if( fill ) {
            uivfill( &wnd->screen, area, wnd->colours[attr], draw_char );
        }
        if( outline ) {
            uidrawbox( &wnd->screen, &area, wnd->colours[attr], NULL );
        }
    }
    return( TRUE );
}

bool GUIDrawRect( gui_window *wnd, gui_rect *rect, gui_attr attr )
{
    return( DrawRect( wnd, rect, attr, FALSE, TRUE, DRAW( BLOCK ) ) );
}

bool GUIFillRect( gui_window *wnd, gui_rect *rect, gui_attr attr )
{
    return( DrawRect( wnd, rect, attr, TRUE, FALSE, DRAW( BLOCK ) ) );
}

bool GUIFillBar( gui_window *wnd, gui_rect *rect, gui_attr attr )
{
    return( DrawRect( wnd, rect, attr, TRUE, FALSE, DRAW( TOP_HALF ) ) );
}

bool GUIDrawLine( gui_window *wnd, gui_point *start, gui_point *end,
                  gui_line_styles style, gui_ord thickness, gui_attr attr )
{
    gui_point   my_start;
    gui_point   my_end;
    SAREA       area;
    char        to_use;

    style = style;
    thickness = thickness;
    my_start = *start;
    my_end = *end;
    GUIScaleToScreenRPt( &my_start );
    GUIScaleToScreenRPt( &my_end );

    area.row = my_start.y;
    area.height = my_end.y - my_start.y + 1;
    area.col = my_start.x;
    area.width = my_end.x - my_start.x + 1;

    if( my_start.x == my_end.x ) {
        to_use = DRAW( VERT_FRAME );
    } else {
        if( my_start.y == my_end.y ) {
            to_use = DRAW( HOR_FRAME );
        } else {
            return( FALSE );
        }
    }
    if( AdjustRect( wnd, &area ) ) {
        uivfill( &wnd->screen, area, wnd->colours[attr], to_use );
    }
    return( TRUE );
}

bool GUIFillRectRGB( gui_window *wnd, gui_rect *rect, gui_rgb rgb )
{
    wnd = wnd;
    rect = rect;
    rgb = rgb;
    return( FALSE );
}

bool GUIDrawRectRGB( gui_window *wnd, gui_rect *rect, gui_rgb rgb )
{
    wnd = wnd;
    rect = rect;
    rgb = rgb;
    return( FALSE );
}

bool GUIDrawLineRGB( gui_window *wnd, gui_point *start, gui_point *end,
                     gui_line_styles style, gui_ord thickness, gui_rgb rgb )
{
     wnd = wnd;
     start = start;
     end = end;
     style = style;
     thickness = thickness;
     rgb = rgb;
     return( FALSE );
}
