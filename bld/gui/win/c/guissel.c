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
#include "guicutil.h"

static bool GUISelect( gui_window *wnd, gui_ctl_id id, bool set, WPI_PARAM2 lparam, WPI_MRESULT *ret )
{
    gui_control_class   control_class;
    UINT                msg;

    if( !GUIGetControlClass( wnd, id, &control_class ) ) {
        return( false );
    }
    switch( control_class ) {
    case GUI_EDIT_MLE:
        msg = ( set ) ? MLM_SETSEL : MLM_GETSEL;
        break;
    case GUI_EDIT:
        msg = ( set ) ? EM_SETSEL : EM_GETSEL;
        break;
    case GUI_EDIT_COMBOBOX:
        msg = ( set ) ? CB_SETEDITSEL : CB_GETEDITSEL;
        break;
    default:
        return( false );
    }
#ifdef __OS2_PM__
    return( GUIToControl( wnd, id, msg, (WPI_PARAM1)lparam, (WPI_PARAM2)0, ret ) );
#elif defined(__NT__)
    return( GUIToControl( wnd, id, msg, (WPI_PARAM1)(INT)LOWORD( lparam ), (WPI_PARAM2)(INT)HIWORD( lparam ), ret ) );
#else
    return( GUIToControl( wnd, id, msg, (WPI_PARAM1)0, lparam, ret ) );
#endif
}

bool GUIAPI GUISelectAll( gui_window *wnd, gui_ctl_id id, bool select )
{
#ifdef __OS2_PM__
    return( GUISelect( wnd, id, true, (WPI_PARAM2)MAKELONG( 0, ( select ) ? 255 : 0 ), NULL ) );
#else
    return( GUISelect( wnd, id, true, (WPI_PARAM2)MAKELONG( ( select ) ? 0 : -1, -1 ), NULL ) );
#endif
}

bool GUIAPI GUISetEditSelect( gui_window *wnd, gui_ctl_id id, int start, int end )
{
    return( GUISelect( wnd, id, true, (WPI_PARAM2)MAKELONG( start, end ), NULL ) );
}

bool GUIAPI GUIGetEditSelect( gui_window *wnd, gui_ctl_id id, int *start, int *end )
{
    bool        ret;
    WPI_MRESULT result;

    ret = GUISelect( wnd, id, false, (WPI_PARAM2)0, &result );
    if( start != NULL ) {
        *start = LOWORD( result );
    }
    if( end != NULL ) {
        *end = HIWORD( result );
    }
    return( ret );
}

void GUIAPI GUIScrollCaret( gui_window *wnd, gui_ctl_id id )
{
    WPI_MRESULT result;
#ifdef  __NT__
    GUIToControl( wnd, id, EM_SCROLLCARET, (WPI_PARAM1)0, (WPI_PARAM2)0, &result );
#else
    GUIToControl( wnd, id, EM_GETSEL, (WPI_PARAM1)0, (WPI_PARAM2)0, &result );
    GUIToControl( wnd, id, EM_SETSEL, (WPI_PARAM1)0, (WPI_PARAM2)result, &result );
#endif
}
