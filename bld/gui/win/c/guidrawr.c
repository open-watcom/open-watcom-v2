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
#include "guicolor.h"
#include "guixutil.h"
#include "guixwind.h"


static bool DrawRect( gui_window *wnd, const gui_rect *rect, WPI_COLOUR colour, bool fill, bool outline )
{
    WPI_RECT    wpi_rect;
    guix_ord    pos_x;
    guix_ord    pos_y;
    guix_ord    size_x;
    guix_ord    size_y;
    HBRUSH      brush;
    guix_ord    win_height;
    gui_ord     pos;
    gui_ord     size;

    if( ( rect->width == 0 ) || ( rect->height == 0 ) ) {
        return( false );
    }

    win_height = _wpi_getheightrect( wnd->hwnd_client_rect );

    pos = rect->x;
    size = rect->width;
    if( size < 0 ) {
        pos += rect->width;
        size *= -1;
    }
    pos_x = GUIScaleToScreenH( pos );
    size_x = GUIScaleToScreenH( size );
    if( GUI_DO_HSCROLL( wnd ) ) {
        pos_x += GUIGetScrollPos( wnd, SB_HORZ );
    }

    pos = rect->y;
    size = rect->height;
    if( size < 0 ) {
        pos += rect->height;
        size *= -1;
    }
    pos_y = GUIScaleToScreenH( pos );
    size_y = GUIScaleToScreenH( size );
    if( GUI_DO_VSCROLL( wnd ) ) {
        pos_y += GUIGetScrollPos( wnd, SB_VERT );
    }
    pos_y  = _wpi_cvth_y_size_plus1( pos_y, win_height, size_y );

    _wpi_setrectvalues( &wpi_rect, pos_x, pos_y, pos_x + size_x, pos_y + size_y );
    if( GUIIsRectInUpdateRect( wnd, &wpi_rect ) ) {
        brush = _wpi_createsolidbrush( colour );
        if( fill ) {
            _wpi_fillrect( wnd->hdc, &wpi_rect, colour, brush );
        }
        if( outline ) {
            _wpi_borderrect( wnd->hdc, &wpi_rect, brush, colour, colour );
        }
        _wpi_deletebrush( brush );
    }
    return( true );
}

bool GUIAPI GUIFillRect( gui_window *wnd, const gui_rect *rect, gui_attr attr )
{
    return( DrawRect( wnd, rect, GUIGetFore( wnd, attr ), true, false ) );
}

bool GUIAPI GUIDrawRect( gui_window *wnd, const gui_rect *rect, gui_attr attr )
{
    return( DrawRect( wnd, rect, GUIGetFore( wnd, attr ), false, true ) );
}

bool GUIAPI GUIFillRectRGB( gui_window *wnd, const gui_rect *rect, gui_rgb rgb )
{
    return( DrawRect( wnd, rect, GETRGB( rgb ), true, false ) );
}

bool GUIAPI GUIDrawRectRGB( gui_window *wnd, const gui_rect *rect, gui_rgb rgb )
{
    return( DrawRect( wnd, rect, GETRGB( rgb ), false, true ) );
}

static bool DrawLine( gui_window *wnd, const gui_point *start, const gui_point *end,
                      gui_line_styles style, gui_ord thickness, WPI_COLOUR colour )
{
    guix_ord    scr_start_x;
    guix_ord    scr_start_y;
    guix_ord    scr_end_x;
    guix_ord    scr_end_y;
    HPEN        pen;
    int         win_style;
    guix_ord    pen_thickness;
    guix_ord    win_height;
    HPEN        old_pen;
    guix_ord    scroll;
    WPI_POINT   wpi_point;

    switch( style ) {
    case GUI_PEN_SOLID:
        win_style = PS_SOLID;
        pen_thickness = GUIScaleToScreenH( thickness );
        break;
    case GUI_PEN_DASH:
        pen_thickness = 1;
        win_style = PS_DASH;
        break;
    case GUI_PEN_DOT:
        pen_thickness = 1;
        win_style = PS_DOT;
        break;
    case GUI_PEN_DASHDOT:
        pen_thickness = 1;
        win_style = PS_DASHDOT;
        break;
    case GUI_PEN_DASHDOTDOT:
        pen_thickness = 1;
        win_style = PS_DASHDOTDOT;
        break;
    default:
        pen_thickness = 0;
        win_style = 0;
        break;
    }

    scr_start_x = GUIScaleToScreenH( start->x );
    scr_start_y = GUIScaleToScreenV( start->y );
    scr_end_x = GUIScaleToScreenH( end->x );
    scr_end_y = GUIScaleToScreenV( end->y );

    if( GUI_DO_HSCROLL( wnd ) ) {
        scroll = GUIGetScrollPos( wnd, SB_HORZ );
        scr_start_x -= scroll;
        scr_end_x -= scroll;
    }
    if( GUI_DO_VSCROLL( wnd ) ) {
        scroll = GUIGetScrollPos( wnd, SB_VERT );
        scr_start_y -= scroll;
        scr_end_y -= scroll;
    }

    win_height = _wpi_getheightrect( wnd->hwnd_client_rect );

    pen = _wpi_createpen( win_style, pen_thickness, colour );
    old_pen = _wpi_selectpen( wnd->hdc, pen );

    wpi_point.x = scr_start_x;
    wpi_point.y = _wpi_cvth_y_plus1( scr_start_y, win_height );
    _wpi_movetoex( wnd->hdc, &wpi_point, &wpi_point );

    wpi_point.x = scr_end_x;
    wpi_point.y = _wpi_cvth_y_plus1( scr_end_y, win_height );
    _wpi_lineto( wnd->hdc, &wpi_point );

    if( old_pen != WPI_NULL ) {
        _wpi_getoldpen( wnd->hdc, old_pen );
    }

    _wpi_deletepen( pen );

    return( true );
}

bool GUIAPI GUIDrawLine( gui_window *wnd, const gui_point *start, const gui_point *end,
                  gui_line_styles style, gui_ord thickness, gui_attr attr )
{
    return( DrawLine( wnd, start, end, style, thickness, GUIGetFore( wnd, attr ) ) );
}

bool GUIAPI GUIDrawLineRGB( gui_window *wnd, const gui_point *start, const gui_point *end,
                     gui_line_styles style, gui_ord thickness, gui_rgb rgb )
{
    return( DrawLine( wnd, start, end, style, thickness, GETRGB( rgb ) ) );
}
