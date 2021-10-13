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
#include <string.h>
#include <stdlib.h>
#include "guixutil.h"
#include "guiscale.h"
#include "guigetx.h"
#include "guixhot.h"
#include "guixdraw.h"
#include "guistr.h"
#include "guicolor.h"


typedef struct draw_cache {
    WPI_COLOUR  back;
    WPI_COLOUR  colour;
    HBRUSH      brush;
    HPEN        pen;
} draw_cache;

/*
 * SetText -- set the current text colour and background colour properly
 *            for the given attributes
 */

static void SetText( gui_window *wnd, WPI_COLOUR fore, WPI_COLOUR back )
{
    _wpi_settextcolor( wnd->hdc, _wpi_getnearestcolor( wnd->hdc, fore ) );
    _wpi_setbackcolour( wnd->hdc, _wpi_getnearestcolor( wnd->hdc , back ) );
}

static void GUIDrawTextBitmapRGB( gui_window *wnd, const char *text,
                            unsigned in_width, unsigned in_height, const gui_coord *pos,
                            WPI_COLOUR fore, WPI_COLOUR back, gui_ord extentx,
                            bool draw_extent, int hotspot_no )
{
    guix_ord    nDrawX;
    guix_ord    nDrawY;
    guix_ord    width;
    guix_ord    height;
    HBRUSH      brush;
    HBRUSH      old_brush;
    HPEN        pen;
    HPEN        old_pen;
    int         old_rop;
    size_t      num_chars;
    WPI_RECT    wpi_rect;
    guix_ord    hscroll_pos;
    WPI_COLOUR  colour;
    WPI_RECTDIM left, top, right, bottom;
    WPI_RECTDIM paint_left, paint_top, paint_right, paint_bottom;
    //draw_cache        dcache;

    if( ( wnd->hdc == NULLHANDLE ) || ( wnd->ps == NULL ) ||
        ( ( text == NULL ) && ( hotspot_no == 0 ) ) ||
        ( ( hotspot_no > 0 ) && ( in_height == 0 ) ) ) {
        return;
    }
    old_rop = 0;
    old_brush = WPI_NULL;
    brush = WPI_NULL;
    old_pen = WPI_NULL;
    pen = WPI_NULL;
    GUIGetMetrics( wnd );
    if( hotspot_no == 0 ) {
        height = AVGYCHAR( GUItm );
        num_chars = strlen( text );
        if( num_chars > in_width ) {
            num_chars = in_width;
        }
        width = GUIGetTextExtentX( wnd, text, num_chars );
    } else {
        /* bitmap size */
        height = in_height;
        width = in_width;
    }

    wpi_rect = wnd->hwnd_client_rect;
    _wpi_getrectvalues( wpi_rect, &left, &top, &right, &bottom );
    _wpi_getpaintrect( wnd->ps, &wpi_rect );
    _wpi_getwrectvalues( wpi_rect, &paint_left, &paint_top, &paint_right, &paint_bottom );
    top = paint_top / height * height;
    bottom = ( paint_bottom + height - 1 ) / height * height;

    if( GUI_DO_HSCROLL( wnd ) ) {
        hscroll_pos = GUIGetScrollPos( wnd, SB_HORZ );
    } else {
        hscroll_pos = 0;
    }

    nDrawY = GUIScaleToScreenV( pos->y );
    if( GUI_DO_VSCROLL( wnd ) ) {
        nDrawY -= GUIGetScrollPos( wnd, SB_VERT );
    }
    nDrawX = left + GUIScaleToScreenH( pos->x ) - hscroll_pos;

    if( draw_extent ) {
        /* blanks out some portion of rest of the line */
        if( extentx != GUI_NO_COLUMN ) {
            right = nDrawX + GUIScaleToScreenX( extentx );
        }
    } else {
        right = nDrawX + width;
    }
    nDrawY = _wpi_cvth_y_size( nDrawY, _wpi_getheightrect( wnd->hwnd_client_rect ), height );

    _wpi_setrectvalues( &wpi_rect, nDrawX, nDrawY, right, nDrawY + height );
    if( GUIIsRectInUpdateRect( wnd, &wpi_rect ) ) {
        colour = _wpi_getnearestcolor( wnd->hdc, back );
        brush = _wpi_createsolidbrush( colour );
        pen = _wpi_createpen( PS_SOLID, 1, colour );
        if( pen == WPI_NULL ) {
            GUIError(LIT( Pen_Failed ));
        }
        old_brush = _wpi_selectbrush( wnd->hdc, brush );
        old_pen = _wpi_selectpen( wnd->hdc, pen );
#ifdef __OS2_PM__
        _wpi_rectangle( wnd->hdc, nDrawX, nDrawY + 1, right, nDrawY + height - 1 );
#else
        _wpi_rectangle( wnd->hdc, nDrawX, nDrawY, right, nDrawY + height );
#endif

        /* if visible even with scrolling */
        if( nDrawX < ( paint_right + hscroll_pos ) ) {
            if( hotspot_no == 0 ) {
#ifdef __OS2_PM__
                nDrawY += _wpi_metricdescent( GUItm );
#endif
                old_rop = _wpi_setrop2( wnd->hdc, R2_COPYPEN );
                SetText( wnd, fore, back );
                _wpi_textout( wnd->hdc, nDrawX, nDrawY, text, num_chars );
            } else {
                GUIDrawBitmap( hotspot_no, wnd->hdc, nDrawX, nDrawY, colour );
            }
        }
        /* restore old resources */
        if( old_rop != 0 ) {
            _wpi_setrop2( wnd->hdc, old_rop );
        }
        if( old_brush != WPI_NULL ) {
            _wpi_getoldbrush( wnd->hdc, old_brush );
        }
        if( brush != WPI_NULL ) {
            _wpi_deletebrush( brush );
        }
        if( old_pen != WPI_NULL ) {
            _wpi_getoldpen( wnd->hdc, old_pen );
        }
        if( pen != WPI_NULL ) {
            _wpi_deletepen( pen );
        }
    }
}

void GUIDrawBitmapAttr( gui_window *wnd, const guix_coord *size, const gui_coord *pos, gui_attr attr, int hotspot_no )
{
    WPI_COLOUR  fore, back;

    fore = GUIGetFore( wnd, attr );
    back = GUIGetBack( wnd, attr );

    GUIDrawTextBitmapRGB( wnd, NULL, size->x, size->y, pos, fore, back, GUI_NO_COLUMN, false, hotspot_no );
}

void GUIXDrawText( gui_window *wnd, const char *text, size_t length, const gui_coord *pos,
                   gui_attr attr, gui_ord extentx, bool draw_extent )
{
    WPI_COLOUR  fore, back;

    fore = GUIGetFore( wnd, attr );
    back = GUIGetBack( wnd, attr );

    GUIDrawTextBitmapRGB( wnd, text, length, 0, pos, fore, back, extentx, draw_extent, 0 );
}

void GUIXDrawTextRGB( gui_window *wnd, const char *text, size_t length, const gui_coord *pos,
                      gui_rgb fore, gui_rgb back, gui_ord extentx, bool draw_extent )
{
    GUIDrawTextBitmapRGB( wnd, text, length, 0, pos, GETRGB( fore ), GETRGB( back ), extentx, draw_extent, 0 );
}
