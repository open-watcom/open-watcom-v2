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
#include "guisetup.h"
#include "guix.h"

/*
 * GUICreateDlg
 */

static bool CreateDlg( gui_create_info *dlg_info, int num_controls,
                       gui_control_info *controls_info, bool sys, res_name_or_id dlg_id )
{
    gui_window *wnd;

    wnd = GUISetupWnd( dlg_info );
    if( wnd == NULL ) {
        return( false );
    }
    if( GUIXCreateDialog( dlg_info, wnd, num_controls, controls_info, sys, dlg_id ) ) {
        return( true );
    } else {
        GUIFreeWindowMemory( wnd, false, true );
        return( false );
    }
}

bool GUICreateDialog( gui_create_info *dlg_info, int num_controls,
                      gui_control_info *controls_info )
{
    return( CreateDlg( dlg_info, num_controls, controls_info, false, NULL ) );
}

bool GUICreateResDialog( gui_create_info *dlg_info, res_name_or_id dlg_id )
{
    return( CreateDlg( dlg_info, 0, NULL, false, dlg_id ) );
}

bool GUICreateSysModalDialog( gui_create_info *dlg_info, int num_controls,
                              gui_control_info *controls_info )
{
    return( CreateDlg( dlg_info, num_controls, controls_info, true, NULL ) );
}
