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
#include "guipaint.h"

/*
 * GUIWndDirtyRect -- tell the user interface that one rectangle of the
 *                    contents of window wnd are bad.
 */

void GUIWndDirtyRect( gui_window *wnd, gui_rect *rect )
{
    gui_rect    my_rect;
    WPI_RECT    win_rect;
    int         left, top, right, bottom;

    my_rect = *rect;
    GUIScaleToScreenRect( &my_rect );
    left = my_rect.x;
    if( GUI_DO_HSCROLL( wnd ) ) {
        left -= GUIGetScrollPos( wnd, SB_HORZ );
    }
    top = my_rect.y;
    if( GUI_DO_VSCROLL( wnd ) ) {
        top -= GUIGetScrollPos( wnd, SB_VERT );
    }
    right = left + my_rect.width;
    bottom = top + my_rect.height;
    _wpi_setwrectvalues( &win_rect, left, top, right, bottom );
    _wpi_cvtc_rect_plus1( wnd->hwnd, &win_rect );
    //GUIInvalidatePaintHandles( wnd );
    _wpi_invalidaterect( wnd->hwnd, &win_rect, TRUE );
    _wpi_updatewindow( wnd->hwnd );
}

