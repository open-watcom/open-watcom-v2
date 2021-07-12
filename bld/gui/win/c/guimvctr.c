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
#include <stdlib.h>
#include "guixutil.h"
#include "guirect.h"

bool GUIAPI GUIResizeControl( gui_window *wnd, gui_ctl_id id, const gui_rect *rect )
{
    guix_coord  scr_pos;
    guix_coord  scr_size;
    HWND        control;

    control = _wpi_getdlgitem( wnd->hwnd, id );
    if( control != NULLHANDLE ) {
        GUICalcLocation( rect, &scr_pos, &scr_size, wnd->hwnd );
        _wpi_movewindow( control, scr_pos.x, scr_pos.y, scr_size.x, scr_size.y, TRUE );
        return( true );
    }
    return( false );
}

bool GUIAPI GUIGetControlRect( gui_window *wnd, gui_ctl_id id, gui_rect *rect )
{
    HWND        control;

    control = _wpi_getdlgitem( wnd->hwnd, id );
    if( control != NULLHANDLE ) {
        GUIGetRelRect( control, rect, false );
        return( true );
    }
    return( false );
}
