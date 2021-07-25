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

void GUIAPI GUIGetClientRect( gui_window *wnd, gui_rect *rect )
{
    WPI_RECT    wpi_rect;
    WPI_RECTDIM left, top, right, bottom;
    guix_rect   client;

    wpi_rect = wnd->hwnd_client_rect;
    _wpi_mapwindowpoints( wnd->hwnd, HWND_DESKTOP, (WPI_LPPOINT)&wpi_rect, 2 );
    _wpi_getwrectvalues( wpi_rect, &left, &top, &right, &bottom );
    client.s_x = left;
    client.s_y = _wpi_cvts_y( top );
    client.s_width = right - left;
    client.s_height = _wpi_getheightrect( wpi_rect );
    GUIClientToScaleRect( &client, rect );
}

bool GUIAPI GUIGetPaintRect( gui_window *wnd, gui_rect *rect )
{
    WPI_RECT    wpi_rect;
    WPI_RECTDIM left, top, right, bottom;
    guix_rect   client;

    if( ( wnd->hdc == NULLHANDLE ) || ( wnd->ps == NULL ) ) {
        return( false );
    }

    _wpi_getpaintrect( wnd->ps, &wpi_rect );
    _wpi_getrectvalues( wpi_rect, &left, &top, &right, &bottom );
    client.s_x = left;
    client.s_y = _wpi_cvtc_y_plus1( wnd->hwnd, top );
    client.s_width = right - left;
    client.s_height = _wpi_getheightrect( wpi_rect );
    GUIClientToScaleRect( &client, rect );

    return( true );
}

