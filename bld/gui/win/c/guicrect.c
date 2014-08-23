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

void GUIGetClientRect( gui_window *wnd, gui_rect *client )
{
    WPI_RECT    rect;
    GUI_RECTDIM left, top, right, bottom;

    rect = wnd->hwnd_client;
    _wpi_mapwindowpoints( wnd->hwnd, HWND_DESKTOP, (WPI_LPPOINT)&rect, 2 );
    _wpi_getwrectvalues( rect, &left, &top, &right, &bottom );
    top = _wpi_cvts_y( top );
    client->x = left;
    client->y = top;
    client->width = right - left;
    client->height = _wpi_getheightrect( rect );
    GUIClientToScaleRect( client );
}

bool GUIGetPaintRect( gui_window *wnd, gui_rect *paint )
{
    WPI_RECT    rect;
    GUI_RECTDIM left, top, right, bottom;

    if( ( wnd->hdc == (WPI_PRES)NULL ) || ( wnd->ps == NULL ) ) {
        return( false );
    }

    _wpi_getpaintrect( wnd->ps, &rect );
    _wpi_getrectvalues( rect, &left, &top, &right, &bottom );
    top = _wpi_cvtc_y_plus1( wnd->hwnd, top );
    paint->x = left;
    paint->y = top;
    paint->width = right - left;
    paint->height = _wpi_getheightrect( rect );
    GUIClientToScaleRect( paint );

    return( true );
}

