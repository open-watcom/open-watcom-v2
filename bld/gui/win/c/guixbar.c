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

#define SHADOW_OFFSET   3
#define BAR_INSET       2

static bool DrawSimpleBar( gui_window *wnd, const guix_rect *rect, WPI_COLOUR colour,
                    bool selected, bool full_bar )
{
    WPI_RECT    wpi_rect;
    guix_coord  pos;
    guix_coord  size;
    HBRUSH      interior_brush;
    HBRUSH      frame_brush;
    guix_ord    hscroll;
    guix_ord    vscroll;
    guix_ord    win_height;

    if( ( rect->s_width == 0 ) || ( rect->s_height == 0 ) ) {
        return( false );
    }

    vscroll = 0;
    if( GUI_DO_VSCROLL( wnd ) ) {
        vscroll = GUIGetScrollPos( wnd, SB_VERT );
    }

    hscroll = 0;
    if( GUI_DO_HSCROLL( wnd ) ) {
        hscroll = GUIGetScrollPos( wnd, SB_HORZ );
    }

    win_height = _wpi_getheightrect( wnd->hwnd_client_rect );

    pos.x = rect->s_x;
    pos.y = rect->s_y;
    size.x = rect->s_width;
    if( rect->s_width < 0 ) {
        pos.x += rect->s_width;
        size.x *= -1;
    }
    size.y = rect->s_height;
    if( rect->s_height < 0 ) {
        pos.y += rect->s_height;
        size.y *= -1;
    }

    pos.x -= hscroll;
    pos.y -= vscroll;

    if( !full_bar ) {
        pos.y  += BAR_INSET;
        size.y -= ( BAR_INSET * 2 );
    }

    pos.y  = _wpi_cvth_y_size_plus1( pos.y, win_height, size.y );

    _wpi_setrectvalues( &wpi_rect, pos.x, pos.y, pos.x + size.x, pos.y + size.y );

    if( selected ) {
        colour = RGB(0, 0x80, 0 );  // GUI_GREEN
    }

    interior_brush = _wpi_createsolidbrush( colour );
    frame_brush = _wpi_createsolidbrush( RGB(0,0,0) );
    _wpi_fillrect( wnd->hdc, &wpi_rect, colour, interior_brush );
    _wpi_borderrect( wnd->hdc, &wpi_rect, frame_brush, RGB(0,0,0), RGB(0,0,0) );
    _wpi_deletebrush( interior_brush );
    _wpi_deletebrush( frame_brush );

    return( true );
}

static bool DrawShadowBar( gui_window *wnd, const guix_rect *rect, WPI_COLOUR colour, bool selected )
{
    WPI_RECT    bar_wpi_rect;
    WPI_RECT    shadow_wpi_rect;
    guix_coord  pos;
    guix_coord  size;
    guix_ord    shadow_offset;
    HBRUSH      interior_brush;
    HBRUSH      shadow_brush;
    guix_ord    hscroll;
    guix_ord    vscroll;
    guix_ord    win_height;

    if( ( rect->s_width == 0 ) || ( rect->s_height == 0 ) ) {
        return( false );
    }

    vscroll = 0;
    if( GUI_DO_VSCROLL( wnd ) ) {
        vscroll = GUIGetScrollPos( wnd, SB_VERT );
    }

    hscroll = 0;
    if( GUI_DO_HSCROLL( wnd ) ) {
        hscroll = GUIGetScrollPos( wnd, SB_HORZ );
    }

    win_height = _wpi_getheightrect( wnd->hwnd_client_rect );

    pos.x = rect->s_x;
    pos.y = rect->s_y;
    size.x = rect->s_width;
    if( rect->s_width < 0 ) {
        pos.x += rect->s_width;
        size.x *= -1;
    }
    size.y = rect->s_height;
    if( rect->s_height < 0 ) {
        pos.y += rect->s_height;
        size.y *= -1;
    }

    pos.x -= hscroll;
    pos.y -= vscroll;

    pos.y  += BAR_INSET;
    size.y -= ( BAR_INSET * 2 );

    pos.y  = _wpi_cvth_y_size_plus1( pos.y, win_height, size.y );

    shadow_offset = 0;
    if( size.x > SHADOW_OFFSET ) {
        shadow_offset = size.x - SHADOW_OFFSET;
        if( shadow_offset > SHADOW_OFFSET ) {
            shadow_offset = SHADOW_OFFSET;
        }
    }

    if( selected ) {
        colour = RGB(0, 0x80, 0 );  // GUI_GREEN
    }

    if( shadow_offset != 0 ) {
#ifndef __OS2_PM__
        _wpi_setrectvalues( &shadow_wpi_rect, pos.x + shadow_offset, pos.y + shadow_offset, pos.x + size.x, pos.y + size.y );
#else
        _wpi_setrectvalues( &shadow_wpi_rect, pos.x + shadow_offset, pos.y, pos.x + size.x, pos.y + size.y - shadow_offset );
#endif
        shadow_brush = _wpi_createsolidbrush( RGB(0,0,0) );
        _wpi_fillrect( wnd->hdc, &shadow_wpi_rect, RGB(0,0,0), shadow_brush );
        _wpi_deletebrush( shadow_brush );
    }

#ifndef __OS2_PM__
    _wpi_setrectvalues( &bar_wpi_rect, pos.x, pos.y, pos.x + size.x - shadow_offset, pos.y + size.y - shadow_offset );
#else
    _wpi_setrectvalues( &bar_wpi_rect, pos.x, pos.y + shadow_offset, pos.x + size.x - shadow_offset, pos.y + size.y );
#endif
    interior_brush = _wpi_createsolidbrush( colour );
    _wpi_fillrect( wnd->hdc, &bar_wpi_rect, colour, interior_brush );
    _wpi_deletebrush( interior_brush );

    return( true );
}

bool GUIAPI GUIDrawBar( gui_window *wnd, gui_text_ord row, gui_ord start, gui_ord width,
                 gui_bar_styles bstyle, gui_attr attr, bool selected )
{
    bool                ret;
    guix_rect           rect;
    gui_text_metrics    metrics;
    WPI_COLOUR          colour;

    if( (wnd == NULL) || (wnd->hdc == NULLHANDLE) || (wnd->ps == NULL) ) {
        return( false );
    }

    ret = true;

    GUIGetTextMetrics( wnd, &metrics );
    rect.s_x = GUIScaleToScreenH( start );
    rect.s_y = GUIScaleToScreenV( row * metrics.avg.y );
    rect.s_width = GUIScaleToScreenH( width );
    rect.s_height = GUIScaleToScreenV( metrics.avg.y );
    if( width > 0 && rect.s_width == 0 ) {
        rect.s_width = 1;
    }

    colour = GUIGetFore( wnd, attr );

    switch( bstyle ) {
    case GUI_BAR_SIMPLE:
        ret = DrawSimpleBar( wnd, &rect, colour, selected, false );
        break;
    case GUI_BAR_SHADOW:
        ret = DrawShadowBar( wnd, &rect, colour, selected );
        break;
    case GUI_FULL_BAR:
        ret = DrawSimpleBar( wnd, &rect, colour, selected, true );
        break;
    case GUI_BAR_3DRECT1:
    case GUI_BAR_3DRECT2:
    default:
        ret = false;
    }

    return( ret );
}


bool GUIAPI GUIDrawBarGroup( gui_window *wnd, gui_text_ord row, gui_ord start,
                      gui_ord width1, gui_ord width2, gui_bar_styles bstyle,
                      gui_attr attr1, gui_attr attr2, bool selected )
{
    gui_ord     stretch_width;
    bool        ret;

    if( width1 == 0 ) {
        ret = true;
    } else {
        stretch_width = width1;
        if( bstyle == GUI_BAR_SHADOW ) {
            gui_ord    x;

            x = GUIScreenToScaleX( SHADOW_OFFSET );
            if( width2 < x ) {
                if( width1 > x ) {
                    width1 -= x;
                }
                x = width2;
            }
            stretch_width += x;
        }
        ret = GUIDrawBar( wnd, row, start, stretch_width, bstyle, attr1, selected );
    }
    if( ret ) {
        ret = GUIDrawBar( wnd, row, start + width1, width2, bstyle, attr2, selected );
    }
    return( ret );
}
