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
#include "guixutil.h"
#include "guirect.h"

void DoGetRelRect( HWND hwnd, WPI_RECT *win, gui_rect *rect, bool is_popup )
{
    WPI_RECT    parent_rect;
    HWND        parent;
    gui_coord   pos;
    gui_coord   size;
    GUI_RECTDIM left, top, right, bottom;

    _wpi_getrectvalues( *win, &left, &top, &right, &bottom );
    pos.x = left;
    pos.y = top;
    size.x = right;
    size.y = bottom;

    size.x -= pos.x;
    size.y -= pos.y;
#ifdef __OS2_PM__
    pos.y += size.y - 1;
#endif

    parent = _wpi_getparent( hwnd );
    if( parent != HWND_DESKTOP && !is_popup ) {
        _wpi_getclientrect( parent, &parent_rect );
        _wpi_mapwindowpoints( parent, HWND_DESKTOP,
                              (WPI_LPPOINT)&parent_rect, 2 );
        _wpi_getrectvalues( parent_rect, &left, &top, &right, &bottom );
        pos.x -= left;
        pos.y -= top;
        pos.y = _wpi_cvth_y( pos.y, bottom - top );
        GUIScreenToScaleR( &pos );
    } else {
        pos.y = _wpi_cvts_y( pos.y );
        GUIScreenToScale( &pos );
    }
    GUIScreenToScaleR( &size );
    rect->x = pos.x;
    rect->y = pos.y;
    rect->width = size.x;
    rect->height = size.y;
}

void GUIGetRelRect( HWND hwnd, gui_rect *rect, bool is_popup )
{
    WPI_RECT    win;

    _wpi_getwindowrect( hwnd, &win );

    DoGetRelRect( hwnd, &win, rect, is_popup );
}

/*
 * GUIGetRect -- get the size of the whole window.  Coordinates are
 *               relative to parent if window has a parent
 */

void GUIGetRect( gui_window *wnd, gui_rect *rect )
{
    GUIGetRelRect( GUIGetParentFrameHWND( wnd ), rect, ( wnd->style & GUI_POPUP ) != 0 );
}

