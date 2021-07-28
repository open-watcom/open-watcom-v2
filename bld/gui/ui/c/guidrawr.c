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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "guiwind.h"
#include "guiscale.h"
#include "guidraw.h"
#include "guixutil.h"

static bool IntersectRect( SAREA *area, SAREA *bound )
{
    if( ( ( area->row + area->height ) < bound->row ) ||
        ( area->row > ( bound->row + bound->height ) ) ) {
        return( false );
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
        return( false );
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
    return( true );
}

static bool AdjustRect( gui_window *wnd, SAREA *area )
{
    if( GUI_DO_HSCROLL( wnd ) ) {
        if( ( area->col + area->width ) < wnd->hgadget->pos ) {
            return( false );
        } else {
            if( area->col < wnd->hgadget->pos ) {
                area->width -= ( wnd->hgadget->pos - area->col );
                area->col = 0;
            } else {
                area->col -= wnd->hgadget->pos;
            }
        }
    }
    if( GUI_DO_VSCROLL( wnd ) ) {
        if( ( area->row + area->height ) < wnd->vgadget->pos ) {
            return( false );
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
    return( IntersectRect( area, &wnd->dirty ) );
}

static bool DrawRect( gui_window *wnd, const gui_rect *rect, gui_attr attr,
                      bool fill, bool outline, char draw_char )
{
    SAREA       area;

    if( ( rect->width == 0 ) || ( rect->height == 0 ) || ( (wnd->flags & CONTENTS_INVALID) == 0 ) ) {
        return( false );
    }
    GUIScaleToScreenRectR( rect, &area );
    if( AdjustRect( wnd, &area ) ) {
        if( fill ) {
            uivfill( &wnd->vs, area, WNDATTR( wnd, attr ), draw_char );
        }
        if( outline ) {
            uidrawbox( &wnd->vs, &area, WNDATTR( wnd, attr ), NULL );
        }
    }
    return( true );
}

bool GUIAPI GUIDrawRect( gui_window *wnd, const gui_rect *rect, gui_attr attr )
{
    return( DrawRect( wnd, rect, attr, false, true, DRAWC1( RECT_AREA ) ) );
}

bool GUIAPI GUIFillRect( gui_window *wnd, const gui_rect *rect, gui_attr attr )
{
    return( DrawRect( wnd, rect, attr, true, false, DRAWC1( RECT_AREA ) ) );
}

bool GUIAPI GUIFillBar( gui_window *wnd, const gui_rect *rect, gui_attr attr )
{
    return( DrawRect( wnd, rect, attr, true, false, DRAWC1( BAR_AREA ) ) );
}

bool GUIAPI GUIDrawLine( gui_window *wnd, const gui_point *start, const gui_point *end,
                  gui_line_styles style, gui_ord thickness, gui_attr attr )
{
    guix_ord    scr_start_x;
    guix_ord    scr_start_y;
    guix_ord    scr_end_x;
    guix_ord    scr_end_y;
    SAREA       area;
    char        to_use;

    /* unused parameters */ (void)style; (void)thickness;

    scr_start_x = GUIScaleToScreenH( start->x );
    scr_start_y = GUIScaleToScreenV( start->y );
    scr_end_x = GUIScaleToScreenH( end->x );
    scr_end_y = GUIScaleToScreenV( end->y );

    area.row = scr_start_y;
    area.height = scr_end_y - scr_start_y + 1;
    area.col = scr_start_x;
    area.width = scr_end_x - scr_start_x + 1;

    if( scr_start_x == scr_end_x ) {
        to_use = DRAWC1( LINE_VERT );
    } else {
        if( scr_start_y == scr_end_y ) {
            to_use = DRAWC1( LINE_HOR );
        } else {
            return( false );
        }
    }
    if( AdjustRect( wnd, &area ) ) {
        uivfill( &wnd->vs, area, WNDATTR( wnd, attr ), to_use );
    }
    return( true );
}

bool GUIAPI GUIFillRectRGB( gui_window *wnd, const gui_rect *rect, gui_rgb rgb )
{
    /* unused parameters */ (void)wnd; (void)rect; (void)rgb;

    return( false );
}

bool GUIAPI GUIDrawRectRGB( gui_window *wnd, const gui_rect *rect, gui_rgb rgb )
{
    /* unused parameters */ (void)wnd; (void)rect; (void)rgb;

    return( false );
}

bool GUIAPI GUIDrawLineRGB( gui_window *wnd, const gui_point *start, const gui_point *end,
                     gui_line_styles style, gui_ord thickness, gui_rgb rgb )
{
    /* unused parameters */ (void)wnd; (void)start; (void)end; (void)style; (void)thickness; (void)rgb;

    return( false );
}
