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
#include <stdlib.h>
#include "guixutil.h"
#include "guirect.h"

bool GUIResizeControl( gui_window *wnd, unsigned id, gui_rect *rect )
{
    gui_coord   pos;
    gui_coord   size;
    HWND        control;

    control = _wpi_getdlgitem( wnd->hwnd, id );
    if( control != NULLHANDLE ) {
        GUICalcLocation( rect, &pos, &size, wnd->hwnd );
        _wpi_movewindow( control, pos.x, pos.y, size.x, size.y, TRUE );
        return( true );
    }
    return( false );
}

bool GUIGetControlRect( gui_window *wnd, unsigned id, gui_rect *rect )
{
    HWND        control;

    control = _wpi_getdlgitem( wnd->hwnd, id );
    if( control != NULLHANDLE ) {
        GUIGetRelRect( control, rect, false );
        return( true );
    }
    return( false );
}
