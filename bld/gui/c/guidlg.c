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

static bool CreateDlg( gui_create_info *dialog, int num_controls,
                       gui_control_info *controls, bool sys, long dlg_id )
{
    gui_window *wnd;

    wnd = GUISetupWnd( dialog );
    if( wnd == NULL ) {
        return( FALSE );
    }
    if( GUIXCreateDialog( dialog, wnd, num_controls, controls, sys, dlg_id ) ) {
        return( TRUE );
    } else {
        GUIFreeWindowMemory( wnd, FALSE, TRUE );
        return( FALSE );
    }
}

bool GUICreateDialog( gui_create_info *dialog, int num_controls,
                      gui_control_info *controls )
{
    return( CreateDlg( dialog, num_controls, controls, FALSE, -1 ) );
}

bool GUICreateResDialog( gui_create_info *dialog, long dlg_id )
{
    return( CreateDlg( dialog, 0, NULL, FALSE, dlg_id ) );
}

bool GUICreateSysModalDialog( gui_create_info *dialog, int num_controls,
                              gui_control_info *controls )
{
    return( CreateDlg( dialog, num_controls, controls, TRUE, -1 ) );
}
