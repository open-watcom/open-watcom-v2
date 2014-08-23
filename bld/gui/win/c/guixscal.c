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
#include "guisdef.h"
#include "guiscale.h"
#include "guixutil.h"

WPI_TEXTMETRIC GUItm;

void GUIScaleToScreenRect( gui_rect * rect )
{
    GUIConvertRect( SCALE, SCREEN, rect, false );
}

void GUIScreenToScaleRect( gui_rect * rect )
{
    GUIConvertRect( SCREEN, SCALE, rect, false );
}

void GUIClientToScaleRect( gui_rect * rect )
{
    GUIScreenToScaleRect( rect );
}

/*
 *  GUIToText -- divide by character width, height
 */

void GUIToText( gui_coord *coord , gui_window *wnd )
{
    GUIGetMetrics( wnd );
    coord->y = GUIMulDiv( coord->y, 1, AVGYCHAR( GUItm ) );
    coord->x = GUIMulDiv( coord->x, 1, AVGXCHAR( GUItm ) );
}

gui_ord GUIToTextX( gui_ord ord, gui_window *wnd )
{
    GUIGetMetrics( wnd );
    return( GUIMulDiv( ord, 1, AVGXCHAR( GUItm ) ) );
}

gui_ord GUIToTextY( gui_ord ord, gui_window *wnd )
{
    GUIGetMetrics( wnd );
    return( GUIMulDiv( ord, 1, AVGYCHAR( GUItm ) ) );
}

/*
 *  GUIFromText -- multiply by character widht, height
 */

void GUIFromText( gui_coord *coord, gui_window *wnd )
{
    GUIGetMetrics( wnd );
    coord->y = GUIMulDiv( coord->y, AVGYCHAR( GUItm ), 1 );
    coord->x = GUIMulDiv( coord->x, AVGXCHAR( GUItm ), 1 );
}

/*
 *  GUIFromTextX -- multiply by character width, height
 */

gui_ord GUIFromTextX( gui_ord ord, gui_window *wnd )
{
    GUIGetMetrics( wnd );
    return( ord *= AVGXCHAR( GUItm ) );
}

/*
 *  GUIFromTextY -- multiply by character widht, height
 */

gui_ord GUIFromTextY( gui_ord ord, gui_window *wnd )
{
    GUIGetMetrics( wnd );
    return( GUIMulDiv( ord, AVGYCHAR( GUItm ), 1 ) );
}

/*
 * GUIGetTheDC - get the device context using font information in wnd
 */

bool GUIGetTheDC( gui_window *wnd )
{
    if( wnd->hdc == (WPI_PRES)NULL ) {
        wnd->hdc = _wpi_getpres( wnd->hwnd );
        if ( wnd->font != NULL ) {
            wnd->prev_font = _wpi_selectfont( wnd->hdc, wnd->font );
        } else {
            wnd->prev_font = NULL;
        }
        return( true );
    }
    return( false );
}

void GUIReleaseTheDC( gui_window * wnd )
{
#ifdef __OS2_PM__
    wnd=wnd;
#else
    if( wnd->hdc != (WPI_PRES)NULL ) {
        if( wnd->prev_font != NULL ) {
            _wpi_getoldfont( wnd->hdc, wnd->prev_font );
        }
        _wpi_releasepres( wnd->hwnd, wnd->hdc );
        wnd->hdc = NULL;
    }
#endif
}

/*
 * GUIGetMetrics - Initialize the tm structure with infor for the given window
 */

void GUIGetMetrics( gui_window * wnd )
{
    bool got_new;

    got_new = GUIGetTheDC( wnd );
    _wpi_gettextmetrics( wnd->hdc, &GUItm );
    if( got_new ) {
        GUIReleaseTheDC( wnd );
    }
}


/*
 * GUIGetUpdateRows -- get the start row and number of rows to get
 *                     updated.  Must have called GUIBeginPaint first.
 */


/* We assume that this function is only called between GUIBeginPaint and
 * GUIEndPaint, so the wnd->hdc is valid and wnd->font is selected
 */

void GUIGetUpdateRows( gui_window *wnd, HWND hwnd, gui_ord *start, int *num )
{
    WPI_RECT    rect;
    int         avgy;
    GUI_RECTDIM left;
    GUI_RECTDIM top;
    GUI_RECTDIM right;
    GUI_RECTDIM bottom;

    hwnd = hwnd;

    _wpi_gettextmetrics( wnd->hdc, &GUItm );
    avgy = AVGYCHAR(GUItm);
    _wpi_getpaintrect( wnd->ps, &rect );
    _wpi_getrectvalues( rect, &left, &top, &right, &bottom );

    top    = _wpi_cvtc_y_plus1( hwnd, top );
    bottom = _wpi_cvtc_y_plus1( hwnd, bottom );

    *start = ( gui_ord )( top / avgy );
    *num = ( bottom + avgy - 1 ) / avgy - *start;
    if( ( *start + *num ) > wnd->num_rows ) {
        *num = wnd->num_rows - *start;
    }

    if( GUI_DO_VSCROLL( wnd ) ) {
        *start += GUIGetVScrollRow( wnd );
    }

    if( wnd->flags & PARTIAL_ROWS ) {
        if( ( ( bottom + avgy - 1 ) % avgy ) != 0 ) {
            (*num)++;
        }
    }
}

