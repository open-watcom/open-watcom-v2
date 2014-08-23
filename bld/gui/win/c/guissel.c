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
#include "guicutil.h"

static bool GUISelect( gui_window *wnd, unsigned id, bool set,
                       WPI_PARAM2 lparam, WPI_MRESULT *ret )
{
    gui_control_class   control_class;
    UINT                msg;
    WPI_PARAM2          wparam;

    if( !GUIGetControlClass( wnd, id, &control_class ) ) {
        return( false );
    }
    switch( control_class ) {
    case GUI_EDIT_MLE:
        if( set ) {
            msg = MLM_SETSEL;
        } else {
            msg = MLM_GETSEL;
        }
        break;
    case GUI_EDIT:
        if( set ) {
            msg = EM_SETSEL;
        } else {
            msg = EM_GETSEL;
        }
        break;
    case GUI_EDIT_COMBOBOX :
        if( set ) {
            msg = CB_SETEDITSEL;
        } else {
            msg = CB_GETEDITSEL;
        }
        break;
    default :
        return( false );
    }
    #ifdef __OS2_PM__
        wparam = lparam;
        lparam = 0;
    #elif defined(__NT__)
        wparam = (INT)LOWORD( lparam );
        lparam = (INT)HIWORD( lparam );
    #else
        wparam = 0;
    #endif
    return( GUIToControl( wnd, id, msg, wparam, lparam, ret ) );
}

bool GUISelectAll( gui_window *wnd, unsigned id, bool select )
{
    WPI_PARAM2          lparam;

#ifdef __OS2_PM__
    if( select ) {
        lparam = (WPI_PARAM2)MAKELONG( 0, 255 );
    } else {
        lparam = (WPI_PARAM2)MAKELONG( 0, 0 );
    }
#else
    if( select ) {
        lparam = (WPI_PARAM2)MAKELONG( 0, -1 );
    } else {
        lparam = (WPI_PARAM2)MAKELONG( -1, -1 );
    }
#endif
    return( GUISelect( wnd, id, true, lparam, NULL ) );
}

bool GUISetEditSelect( gui_window *wnd, unsigned id, int start, int end )
{
    return( GUISelect( wnd, id, true, (WPI_PARAM2)MAKELONG( start, end ), NULL ) );
}

bool GUIGetEditSelect( gui_window *wnd, unsigned id, int *start, int *end )
{
    bool        ret;
    WPI_MRESULT result;

    ret = GUISelect( wnd, id, false, 0, &result );
    if( start != NULL ) {
        *start = LOWORD( result );
    }
    if( end != NULL ) {
        *end = HIWORD( result );
    }
    return( ret );
}

void GUIScrollCaret( gui_window *wnd, unsigned id )
{
    WPI_MRESULT result;
#ifdef  __NT__
    GUIToControl( wnd, id, EM_SCROLLCARET, 0, 0, &result );
#else
    GUIToControl( wnd, id, EM_GETSEL, 0, 0, &result );
    GUIToControl( wnd, id, EM_SETSEL, 0, result, &result );
#endif
}
