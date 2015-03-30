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
#include "guicontr.h"

/*
 * GUISetFocus -- set input focus to a control in a dialog box or in a window
 */

bool GUISetFocus( gui_window *wnd, gui_ctl_id id )
{
    gui_control_class   control_class;
    control_item        *info;
    HWND                hwnd;

    if( !GUIGetControlClass( wnd, id, &control_class ) ) {
        return( false );
    }

    if( control_class != GUI_EDIT_MLE ) {
        GUISelectAll( wnd, id, true );
    }

    hwnd = _wpi_getdlgitem( wnd->hwnd, id );
    if( hwnd != NULLHANDLE ) { /* in dialog box */
        SetFocus( hwnd );
        return( true );
    } else {
        info = GUIGetControlByID( wnd, id );
        if( info != NULL ) { /* in a window */
            SetFocus( info->hwnd );
            return( true );
        }
    }

    return( false );
}

bool GUIGetFocus( gui_window *wnd, gui_ctl_id *id )
{
    control_item        *info;
    HWND                parent;
    HWND                hwnd;

    if( id != NULL ) {
        hwnd = _wpi_getfocus();
        parent = _wpi_getparent( hwnd );
        if( hwnd != NULLHANDLE ) {
            // this will handle edit boxes in combo boxes having the focus
            if( parent != wnd->hwnd ) {
                hwnd = parent;
                parent = _wpi_getparent( parent );
            }
            if( ( parent == wnd->hwnd ) ) {
                *id = _wpi_getdlgctrlid( hwnd );
                if( *id != 0 ) {
                    return( true );
                }
                info = GUIGetControlByHwnd( wnd, hwnd );
                if( info != NULL ) { /* in a window */
                    *id = info->id;
                    return( true );
                }
            }
        }
    }
    return( false );
}
