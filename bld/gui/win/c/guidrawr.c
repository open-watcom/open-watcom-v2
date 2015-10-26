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
#include "guicolor.h"
#include "guixutil.h"

extern  WPI_INST        GUIMainHInst;

static bool DrawRect( gui_window *wnd, gui_rect *rect, WPI_COLOUR colour,
                      bool fill, bool outline )
{
    WPI_RECT    wnd_rect;
    gui_coord   pos;
    gui_coord   size;
    HBRUSH      brush;
    int         hscroll;
    int         vscroll;
    int         win_height;

    if( ( rect->width == 0 ) || ( rect->height == 0 ) ) {
        return( false );
    }

    if( GUI_DO_VSCROLL( wnd ) ) {
        vscroll = GUIGetScrollPos( wnd, SB_VERT );
    } else {
        vscroll = 0;
    }

    if( GUI_DO_HSCROLL( wnd ) ) {
        hscroll = GUIGetScrollPos( wnd, SB_HORZ );
    } else {
        hscroll = 0;
    }

    win_height = _wpi_getheightrect( wnd->hwnd_client_rect );

    pos.x = rect->x;
    pos.y = rect->y;
    size.x = rect->width;
    if( rect->width < 0 ) {
        pos.x += rect->width;
        size.x *= -1;
    }
    size.y = rect->height;
    if( rect->height < 0 ) {
        pos.y += rect->height;
        size.y *= -1;
    }

    GUIScaleToScreenR( &pos );
    GUIScaleToScreenR( &size );

    pos.x -= hscroll;
    pos.y -= vscroll;

    pos.y  = _wpi_cvth_y_size_plus1( pos.y, win_height, size.y );

    _wpi_setrectvalues( &wnd_rect, pos.x, pos.y, pos.x + size.x, pos.y + size.y );
    if( GUIIsRectInUpdateRect( wnd, &wnd_rect ) ) {
        brush = _wpi_createsolidbrush( colour );
        if( fill ) {
            _wpi_fillrect( wnd->hdc, &wnd_rect, colour, brush );
        }
        if( outline ) {
            _wpi_borderrect( wnd->hdc, &wnd_rect, brush, colour, colour );
        }
        _wpi_deletebrush( brush );
    }
    return( true );
}

bool GUIFillRect( gui_window *wnd, gui_rect *rect, gui_attr attr )
{
    return( DrawRect( wnd, rect, GUIGetFore( wnd, attr ), true, false ) );
}

bool GUIDrawRect( gui_window *wnd, gui_rect *rect, gui_attr attr )
{
    return( DrawRect( wnd, rect, GUIGetFore( wnd, attr ), false, true ) );
}

bool GUIFillRectRGB( gui_window *wnd, gui_rect *rect, gui_rgb rgb )
{
    return( DrawRect( wnd, rect, GETRGB( rgb ), true, false ) );
}

bool GUIDrawRectRGB( gui_window *wnd, gui_rect *rect, gui_rgb rgb )
{
    return( DrawRect( wnd, rect, GETRGB( rgb ), false, true ) );
}

static bool DrawLine( gui_window *wnd, gui_point *start, gui_point *end,
                      gui_line_styles style, gui_ord thickness,
                      WPI_COLOUR colour )
{
    gui_point   my_start;
    gui_point   my_end;
    HPEN        pen;
    int         win_style;
    gui_coord   coord;
    HPEN        old_pen;
    int         hscroll;
    int         vscroll;
    WPI_POINT   pt;
    int         win_height;

    my_start = *start;
    my_end = *end;
    GUIScaleToScreenRPt( &my_start );
    GUIScaleToScreenRPt( &my_end );
    if( GUI_DO_VSCROLL( wnd ) ) {
        vscroll = GUIGetScrollPos( wnd, SB_VERT );
    } else {
        vscroll = 0;
    }
    if( GUI_DO_HSCROLL( wnd ) ) {
        hscroll = GUIGetScrollPos( wnd, SB_HORZ );
    } else {
        hscroll = 0;
    }
    switch( style ) {
    case GUI_PEN_SOLID :
        win_style = PS_SOLID;
        coord.x = thickness;
        GUIScaleToScreenR( &coord );
        break;
    case GUI_PEN_DASH :
        coord.x = 1;
        win_style = PS_DASH;
        break;
    case GUI_PEN_DOT :
        coord.x = 1;
        win_style = PS_DOT;
        break;
    case GUI_PEN_DASHDOT :
        coord.x = 1;
        win_style = PS_DASHDOT;
        break;
    case GUI_PEN_DASHDOTDOT :
        coord.x = 1;
        win_style = PS_DASHDOTDOT;
        break;
    }
    pen = _wpi_createpen( win_style, coord.x, colour );

    old_pen = _wpi_selectpen( wnd->hdc, pen );

    win_height = _wpi_getheightrect( wnd->hwnd_client_rect );

    pt.x = my_start.x - hscroll;
    pt.y = my_start.y - vscroll;
    pt.y = _wpi_cvth_y_plus1( pt.y, win_height );
    _wpi_movetoex( wnd->hdc, &pt, &pt );

    pt.x = my_end.x - hscroll;
    pt.y = my_end.y - vscroll;
    pt.y = _wpi_cvth_y_plus1( pt.y, win_height );
    _wpi_lineto( wnd->hdc, &pt );

    if( old_pen != NULLHANDLE ) {
        _wpi_getoldpen( wnd->hdc, old_pen );
    }

    _wpi_deletepen( pen );

    return( true );
}

bool GUIDrawLine( gui_window *wnd, gui_point *start, gui_point *end,
                  gui_line_styles style, gui_ord thickness, gui_attr attr )
{
    return( DrawLine( wnd, start, end, style, thickness,
                      GUIGetFore( wnd, attr ) ) );
}

bool GUIDrawLineRGB( gui_window *wnd, gui_point *start, gui_point *end,
                     gui_line_styles style, gui_ord thickness, gui_rgb rgb )
{
    return( DrawLine( wnd, start, end, style, thickness,
                      GETRGB( rgb ) ) );
}
