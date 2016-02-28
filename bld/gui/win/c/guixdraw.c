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
#include <string.h>
#include <stdlib.h>
#include "guixutil.h"
#include "guiscale.h"
#include "guigetx.h"
#include "guixdraw.h"
#include "guixhot.h"
#include "guistr.h"
#include "guicolor.h"

typedef struct draw_cache {
    WPI_COLOUR  back;
    WPI_COLOUR  colour;
    HBRUSH      brush;
    HPEN        pen;
} draw_cache;

extern  WPI_TEXTMETRIC  GUItm;

/*
 * SetText -- set the current text colour and background colour properly
 *            for the given attributes
 */

static void SetText( gui_window * wnd, WPI_COLOUR fore, WPI_COLOUR back )
{
    _wpi_settextcolor( wnd->hdc, _wpi_getnearestcolor( wnd->hdc, fore ) );
    _wpi_setbackcolour( wnd->hdc, _wpi_getnearestcolor( wnd->hdc , back ) );
}

void GUIDrawTextBitmapRGB( gui_window *wnd, const char *text,
                            size_t length, int height, gui_coord *pos,
                            WPI_COLOUR fore, WPI_COLOUR back, gui_ord extentx,
                            bool draw_extent, int bitmap )
{
    int         nDrawX, nDrawY;
    int         lenx;
    HBRUSH      brush;
    HBRUSH      old_brush;
    HPEN        pen;
    HPEN        old_pen;
    int         old_rop;
    size_t      num_chars;
    WPI_RECT    rect;
    gui_coord   indent;
    int         hscroll_pos;
    gui_coord   extent;
    WPI_COLOUR  colour;
    GUI_RECTDIM left, top, right, bottom;
    GUI_RECTDIM paint_left, paint_top, paint_right, paint_bottom;
    WPI_RECT    paint_rect;
    WPI_RECT    draw_rect;
    //draw_cache        dcache;

    if( ( wnd->hdc == NULLHANDLE ) || ( wnd->ps == NULL ) ||
        ( ( text == NULL ) && ( bitmap == 0 ) ) ||
        ( ( bitmap != 0 ) && ( height == 0 ) ) ) {
        return;
    }
    old_rop = 0;
    old_brush = (HBRUSH)NULL;
    brush = (HBRUSH)NULL;
    old_pen = (HPEN)NULL;
    pen = (HPEN)NULL;
    GUIGetMetrics( wnd );
    if( !bitmap ) {
        height = AVGYCHAR(GUItm);
    }
    rect = wnd->hwnd_client_rect;
    _wpi_getrectvalues( rect, &left, &top, &right, &bottom);
    _wpi_getpaintrect( wnd->ps, &paint_rect );
    _wpi_getwrectvalues( paint_rect, &paint_left, &paint_top, &paint_right,
                         &paint_bottom );
    top = paint_top / height * height;
    bottom = ( paint_bottom + height - 1) / height * height;

    if( GUI_DO_HSCROLL( wnd ) ) {
        hscroll_pos = GUIGetScrollPos( wnd, SB_HORZ );
    } else {
        hscroll_pos = 0;
    }

    if( bitmap == 0 ) {
        num_chars = strlen( text );
        if( num_chars > length ) {
            num_chars = length;
        }
    }

    indent.x = pos->x;
    indent.y = pos->y;
    GUIScaleToScreenR( &indent );
    nDrawY = indent.y;
    if( GUI_DO_VSCROLL( wnd ) ) {
        nDrawY -= GUIGetScrollPos( wnd, SB_VERT );
    }
    nDrawX = left;
    nDrawX += ( indent.x - hscroll_pos );

    if( bitmap > 0 ) {
        lenx = length ;
    } else {
        lenx = GUIGetTextExtentX( wnd, text, num_chars );
    }

    if( draw_extent ) {
        /* blanks out some portion of rest of the line */
        if( extentx != GUI_NO_COLUMN ) {
            extent.x = extentx;
            GUIScaleToScreen( &extent );
            right = nDrawX + extent.x;
        }
    } else {
        right = nDrawX + lenx;
    }
    nDrawY = _wpi_cvth_y_size( nDrawY, _wpi_getheightrect(wnd->hwnd_client_rect), height );

    _wpi_setrectvalues( &draw_rect, nDrawX, nDrawY, right, nDrawY+height );
    if( GUIIsRectInUpdateRect( wnd, &draw_rect ) ) {
        colour = _wpi_getnearestcolor( wnd->hdc, back );
        brush = _wpi_createsolidbrush( colour );
        pen = _wpi_createpen( PS_SOLID, 1, colour );
        if( pen == NULLHANDLE ) {
            GUIError(LIT( Pen_Failed ));
        }
        old_brush = _wpi_selectbrush( wnd->hdc, brush );
        old_pen = _wpi_selectpen( wnd->hdc, pen );
#ifdef __OS2_PM__
        _wpi_rectangle( wnd->hdc, nDrawX, nDrawY+1, right, nDrawY + height - 1 );
#else
        _wpi_rectangle( wnd->hdc, nDrawX, nDrawY, right, nDrawY + height);
#endif

        /* if visible even with scrolling */
        if( nDrawX < ( paint_right + hscroll_pos ) ) {
            if( bitmap > 0 ) {
                GUIDrawBitmap( bitmap, wnd->hdc, nDrawX, nDrawY, colour);
            } else {
#ifdef __OS2_PM__
                nDrawY += _wpi_metricdescent( GUItm );
#endif
                old_rop = _wpi_setrop2( wnd->hdc, R2_COPYPEN );
                SetText( wnd, fore, back );
                _wpi_textout( wnd->hdc, nDrawX, nDrawY, text, num_chars );
            }
        }
        /* restore old resources */
        if( old_rop != 0 ) {
            _wpi_setrop2( wnd->hdc, old_rop );
        }
        if( old_brush != (HBRUSH)NULL ) {
            _wpi_getoldbrush( wnd->hdc, old_brush );
        }
        if( brush != (HBRUSH)NULL ) {
            _wpi_deletebrush( brush );
        }
        if( old_pen != (HPEN)NULL ) {
            _wpi_getoldpen( wnd->hdc, old_pen );
        }
        if( pen != (HPEN)NULL ) {
            _wpi_deletepen( pen );
        }
    }
}

void GUIDrawTextBitmapAttr( gui_window *wnd, const char *text, size_t length,
                            int height, gui_coord *pos,
                            gui_attr attr, gui_ord extentx,
                            bool draw_extent, int bitmap )
{
    WPI_COLOUR  fore, back;

    fore = GUIGetFore( wnd, attr );
    back = GUIGetBack( wnd, attr );

    GUIDrawTextBitmapRGB( wnd, text, length, height, pos, fore, back, extentx,
                           draw_extent, bitmap );
}

void GUIXDrawText( gui_window *wnd, const char *text, size_t length, gui_coord *pos,
                   gui_attr attr, gui_ord extentx, bool draw_extent )
{
    GUIDrawTextBitmapAttr( wnd, text, length, 0, pos, attr, extentx, draw_extent, 0 );
}

void GUIXDrawTextRGB( gui_window *wnd, const char *text, size_t length, gui_coord *pos,
                      gui_rgb fore, gui_rgb back, gui_ord extentx,
                      bool draw_extent )
{
    GUIDrawTextBitmapRGB( wnd, text, length, 0, pos, GETRGB(fore), GETRGB(back),
                          extentx, draw_extent, 0 );
}

