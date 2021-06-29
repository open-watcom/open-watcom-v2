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
* Description:  AUI dialog window.
*
****************************************************************************/


#include "_aui.h"
#include "guidlg.h"
#include "wnddlg.h"
#include "watcom.h"
#include "wresdefn.h"


#define MAX_DLG_NESTS   5

static gui_window       *Parents[MAX_DLG_NESTS];
static GUICALLBACK      *dlgGUIEventProcs[MAX_DLG_NESTS];
static int              Nested = -1;

static bool dlgOpenGUIEventProc( gui_window *gui, gui_event gui_ev, void *parm )
{
    bool                rc;
    gui_mcursor_handle  old_cursor;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        old_cursor = WndHourGlass( NULL );
        ++Nested;
        Parents[Nested] = gui;
        rc = dlgGUIEventProcs[Nested]( gui, gui_ev, parm );
        WndHourGlass( old_cursor );
        break;
    case GUI_DESTROY:
        rc = dlgGUIEventProcs[Nested]( gui, gui_ev, parm );
        --Nested;
        break;
    default:
        rc = dlgGUIEventProcs[Nested]( gui, gui_ev, parm );
        break;
    }
    return( rc );
}


gui_window *DlgOpenGetGUIParent( void )
{
    if( Nested >= MAX_DLG_NESTS )
        return( NULL );
    return( ( Nested == -1 ) ? WndMain->gui : Parents[Nested] );
}

void DlgOpen( const char *title, gui_text_ord rows, gui_text_ord cols,
                     gui_control_info *ctl, int num_controls,
                     GUICALLBACK *gui_call_back, void *extra )
{
    gui_window  *parent;

    parent = DlgOpenGetGUIParent();
    dlgGUIEventProcs[Nested + 1] = gui_call_back;
    GUIModalDlgOpen( parent, title, rows, cols, ctl, num_controls, dlgOpenGUIEventProc, extra );
}

static gui_create_info ResDialog = {
    NULL,                           // Title
    { 0, 0, 0, 0 },                 // Position
    GUI_NOSCROLL,                   // Scroll Styles
    GUI_VISIBLE | GUI_CLOSEABLE,    // Window Styles
    NULL,                           // Parent
    GUI_NO_MENU,                    // Menu array
    GUI_NO_COLOUR,                  // Colour attribute array
    dlgOpenGUIEventProc,            // GUI Event Callback function
    NULL,                           // Extra
    NULL,                           // Icon
    NULL                            // Menu Resource
};

void ResDlgOpen( GUICALLBACK *gui_call_back, void *extra, int dlg_id )
{
    ResDialog.parent = DlgOpenGetGUIParent();
    dlgGUIEventProcs[Nested + 1] = gui_call_back;
    ResDialog.extra = extra;
    GUICreateResDialog( &ResDialog, MAKEINTRESOURCE( dlg_id ) );
}
