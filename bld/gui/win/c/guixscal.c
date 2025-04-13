/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
#include "guixutil.h"


WPI_TEXTMETRIC GUItm;

void GUIClientToScaleRect( const guix_rect *client, gui_rect *rect )
{
    GUIScaleFromScreenRect( client, rect );
}

/*
 *  GUITextFromScreenH -- divide by character width, height
 *  !! dimension only !!
 */
gui_text_ord GUITextFromScreenH( guix_ord ord, gui_window *wnd )
{
    GUIGetMetrics( wnd );
    return( GUIMulDiv( gui_text_ord, ord, 1, AVGXCHAR( GUItm ) ) );
}

/*
 *  GUITextFromScreenV -- divide by character width, height
 *  !! dimension only !!
 */
gui_text_ord GUITextFromScreenV( guix_ord ord, gui_window *wnd )
{
    GUIGetMetrics( wnd );
    return( GUIMulDiv( gui_text_ord, ord, 1, AVGYCHAR( GUItm ) ) );
}

/*
 *  GUITextToScreenH -- multiply by character width, height
 *  !! dimension only !!
 */
guix_ord GUITextToScreenH( gui_text_ord text_ord, gui_window *wnd )
{
    GUIGetMetrics( wnd );
    return( GUIMulDiv( gui_ord, text_ord, AVGXCHAR( GUItm ), 1 ) );
}

/*
 *  GUITextToScreenV -- multiply by character widht, height
 *  !! dimension only !!
 */
guix_ord GUITextToScreenV( gui_text_ord text_ord, gui_window *wnd )
{
    GUIGetMetrics( wnd );
    return( GUIMulDiv( gui_ord, text_ord, AVGYCHAR( GUItm ), 1 ) );
}

/*
 * GUIGetTheDC - get the device context using font information in wnd
 */
bool GUIGetTheDC( gui_window *wnd )
{
    if( wnd->hdc == NULLHANDLE ) {
        wnd->hdc = _wpi_getpres( wnd->hwnd );
        if( wnd->font != NULL ) {
            wnd->prev_font = _wpi_selectfont( wnd->hdc, wnd->font );
        } else {
            wnd->prev_font = NULLHANDLE;
        }
        return( true );
    }
    return( false );
}

void GUIReleaseTheDC( gui_window *wnd )
{
#ifdef __OS2_PM__
    wnd=wnd;
#else
    if( wnd->hdc != NULLHANDLE ) {
        if( wnd->prev_font != NULL ) {
            _wpi_getoldfont( wnd->hdc, wnd->prev_font );
        }
        _wpi_releasepres( wnd->hwnd, wnd->hdc );
        wnd->hdc = NULLHANDLE;
    }
#endif
}

/*
 * GUIGetMetrics - Initialize the tm structure with info for the given window
 */
void GUIGetMetrics( gui_window *wnd )
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
void GUIGetUpdateRows( gui_window *wnd, HWND hwnd, gui_rows_set *rows_set )
{
    WPI_RECT    wpi_rect;
    int         avgy;
    WPI_RECTDIM left, top, right, bottom;

    /* unused parameters */ (void)hwnd;

    /* We assume that this function is only called between GUIBeginPaint and
     * GUIEndPaint, so the wnd->hdc is valid and wnd->font is selected
     */
    _wpi_gettextmetrics( wnd->hdc, &GUItm );
    avgy = AVGYCHAR( GUItm );
    _wpi_getpaintrect( wnd->ps, &wpi_rect );
    _wpi_getrectvalues( wpi_rect, &left, &top, &right, &bottom );

    top    = _wpi_cvtc_y_plus1( hwnd, top );
    bottom = _wpi_cvtc_y_plus1( hwnd, bottom );

    rows_set->start = (gui_text_ord)( top / avgy );
    rows_set->count = (gui_text_ord)( ( bottom + avgy - 1 ) / avgy ) - rows_set->start;
    if( ( rows_set->start + rows_set->count ) > wnd->num_rows ) {
        rows_set->count = wnd->num_rows - rows_set->start;
    }

    if( GUI_DO_VSCROLL( wnd ) ) {
        rows_set->start += GUIGetVScrollRow( wnd );
    }

    if( wnd->flags & PARTIAL_ROWS ) {
        if( ( ( bottom + avgy - 1 ) % avgy ) != 0 ) {
            rows_set->count++;
        }
    }
}
