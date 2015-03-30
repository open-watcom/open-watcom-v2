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
#include "guixutil.h"
#include "guipaint.h"

/*
 * GUIWndDirty -- tell the user interface that the contents of window wnd
 *                 are bad.  If wnd is NULL, they are all bad.
 */

void GUIWndDirty( gui_window *wnd )
{
    gui_window *curr;

    if( wnd == NULL ) {
        for( curr = GUIGetFront(); curr != NULL; curr = GUIGetNextWindow( curr ) ) {
            if( GUIGetParentFrameHWND( curr ) != NULLHANDLE ) {
                GUIWndDirty( curr );
            }
        }
    } else {
        //GUIInvalidatePaintHandles( wnd );
        _wpi_invalidaterect( wnd->hwnd, NULL, TRUE );
        wnd->flags &= ~NEEDS_RESIZE_REDRAW;
        _wpi_updatewindow( wnd->hwnd );
    }
}

void GUIControlDirty( gui_window *wnd, gui_ctl_id id )
{
    HWND        control;

    control = _wpi_getdlgitem( wnd->hwnd, id );
    if( control != NULLHANDLE ) {
        //GUIInvalidatePaintHandles( wnd ); // not 100% sure this is required
        _wpi_invalidaterect( control, NULL, TRUE );
        _wpi_updatewindow( control );
    }
}

