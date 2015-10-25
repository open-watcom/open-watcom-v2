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
* Description:  AUI dialog window.
*
****************************************************************************/


#include "auipvt.h"
#include "guidlg.h"
#include "wnddlg.h"

#define MAX_DLG_NESTS   5
static gui_window       *Parents[MAX_DLG_NESTS];
static GUICALLBACK      *Routines[MAX_DLG_NESTS];
static int              Nested = -1;

static bool DlgEventProc( gui_window * gui, gui_event event, void *parm )
{
    bool        rc;
    void        *cursor;

    switch( event ) {
    case GUI_INIT_DIALOG:
        cursor = WndHourGlass( NULL );
        ++Nested;
        Parents[ Nested ] = gui;
        rc = Routines[ Nested ]( gui, event, parm );
        WndHourGlass( cursor );
        break;
    case GUI_DESTROY:
        rc = Routines[ Nested ]( gui, event, parm );
        --Nested;
        break;
    default:
        rc = Routines[ Nested ]( gui, event, parm );
        break;
    }
    return( rc );
}


gui_window *DlgGetParent( void )
{
    if( Nested >= MAX_DLG_NESTS )
        return( NULL );
    return( ( Nested == -1 ) ? WndMain->gui : Parents[ Nested ] );
}

void DlgOpen( const char *title, int rows, int cols,
                     gui_control_info *ctl, int num_controls,
                     GUICALLBACK *rtn, void *extra )
{
    gui_window  *parent;

    parent = DlgGetParent();
    Routines[Nested + 1] = rtn;
    GUIModalDlgOpen( parent, title, rows, cols, ctl, num_controls, DlgEventProc, extra );
}

static gui_create_info ResDialog = {
    NULL,                               // Title
    { 0, 0, 0, 0 },                     // Position
    GUI_NOSCROLL,                       // Scroll Styles
    GUI_VISIBLE | GUI_CLOSEABLE,        // Window Styles
    NULL,                               // Parent
    0,                                  // Number of menus
    NULL,                               // Menu's
    0,                                  // Number of color attributes
    NULL,                               // Array of color attributes
    DlgEventProc,                       // Callback function
    NULL,                               // Extra
    NULL,                               // Icon
    NULL                                // Menu Resource
};

void ResDlgOpen( GUICALLBACK *rtn, void *extra, res_name_or_id dlg_id )
{
    ResDialog.parent = DlgGetParent();
    Routines[Nested + 1] = rtn;
    ResDialog.extra = extra;
    GUICreateResDialog( &ResDialog, dlg_id );
}
