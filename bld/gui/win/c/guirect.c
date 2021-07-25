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
#include "guixutil.h"
#include "guirect.h"


void DoGetRelRect( HWND hwnd, const WPI_RECT *wpi_rect, gui_rect *rect, bool ispopup )
{
    WPI_RECT    parent_wpi_rect;
    HWND        parent;
    gui_coord   pos;
    WPI_RECTDIM left, top, right, bottom;

    _wpi_getrectvalues( *wpi_rect, &left, &top, &right, &bottom );
    rect->width = GUIScreenToScaleH( right - left );
    rect->height = GUIScreenToScaleV( bottom - top );

    pos.x = left;
#ifdef __OS2_PM__
    pos.y = bottom - 1;
#else
    pos.y = top;
#endif
    parent = _wpi_getparent( hwnd );
    if( parent != HWND_DESKTOP && !ispopup ) {
        _wpi_getclientrect( parent, &parent_wpi_rect );
        _wpi_mapwindowpoints( parent, HWND_DESKTOP, (WPI_LPPOINT)&parent_wpi_rect, 2 );
        _wpi_getrectvalues( parent_wpi_rect, &left, &top, &right, &bottom );
        rect->x = GUIScreenToScaleH( pos.x - left );
        rect->y = GUIScreenToScaleV( _wpi_cvth_y( pos.y - top, bottom - top ) );
    } else {
        rect->x = GUIScreenToScaleX( pos.x );
        rect->y = GUIScreenToScaleY( _wpi_cvts_y( pos.y ) );
    }
}

void GUIGetRelRect( HWND hwnd, gui_rect *rect, bool ispopup )
{
    WPI_RECT    wpi_rect;

    _wpi_getwindowrect( hwnd, &wpi_rect );

    DoGetRelRect( hwnd, &wpi_rect, rect, ispopup );
}

/*
 * GUIGetRect -- get the size of the whole window.  Coordinates are
 *               relative to parent if window has a parent
 */

void GUIAPI GUIGetRect( gui_window *wnd, gui_rect *rect )
{
    GUIGetRelRect( GUIGetParentFrameHWND( wnd ), rect, (wnd->style & GUI_POPUP) != 0 );
}

