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
#include "guicutil.h"
#include "guixutil.h"

static HWND GUIGetControlHWND( gui_window *wnd, unsigned id )
{
    HWND                control;
    control = _wpi_getdlgitem( wnd->hwnd, id );
    return( control );
}

bool GUIToControl( gui_window *wnd, unsigned id, WPI_MSG msg,
                   WPI_PARAM1 wparam, WPI_PARAM2 lparam, WPI_MRESULT *ret )
{
    HWND                control;
    WPI_MRESULT         ret_val;

    control = GUIGetControlHWND( wnd, id );
    if( control != NULLHANDLE ) {
        ret_val = GUISendMessage( control, msg, wparam, lparam );
        if( ret != NULL ) {
            *ret = ret_val;
        }
        return( true );
    }
    return( false );
}

static bool GUIToComboListControl( gui_window *wnd, unsigned id, WPI_MSG msg,
                            WPI_PARAM1 wparam, WPI_PARAM2 lparam,
                            WPI_MRESULT *ret )
{
    return( GUIToControl( wnd, id, msg, wparam, lparam, ret ) );
}

WPI_MRESULT GUIToComboList( gui_window *wnd, unsigned id, WPI_MSG lb_msg,
                            WPI_MSG cb_msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam,
                            WPI_MRESULT ret )
{
    gui_control_class   control_class;
    WPI_MRESULT         my_ret;

    if( !GUIGetControlClass( wnd, id, &control_class ) ) {
        return( ret );
    }
    my_ret = ret;
    switch( control_class ) {
    case GUI_LISTBOX :
        GUIToComboListControl( wnd, id, lb_msg, wparam, lparam, &my_ret );
        break;
    case GUI_COMBOBOX :
    case GUI_EDIT_COMBOBOX :
        GUIToComboListControl( wnd, id, cb_msg, wparam, lparam, &my_ret );
        break;
    default :
        return( ret );
    }
    return( my_ret );
}

bool GUIGetControlClass( gui_window *wnd, unsigned id,
                         gui_control_class *control_class )
{
    control_item        *item;

    if( control_class == NULL ) {
        return( false );
    }
    item = GUIGetControlByID( wnd, id );
    if( item == NULL ) {
        return( false );
    }
    *control_class = item->control_class;
    return( true );
}
