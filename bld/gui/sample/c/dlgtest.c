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
* Description:  Dialog test for GUI lib sample program.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include "watcom.h"
#include "sample.h"
#include "dlgstat.h"
#include "dlgtest.h"
#include "wresdefn.h"


static GUICALLBACK TestDialogWndGUIEventProc;

static gui_create_info DialogControl = {
    "Test Dialog Box",                  // Title
    { 150, 250, 700, 700 },             // Position
    GUI_NOSCROLL,                       // Scroll Styles
    GUI_VISIBLE | GUI_CLOSEABLE,        // Window Styles
    NULL,                               // Parent
    0, NULL,                            // Menu array
    0, NULL,                            // Colour attribute array
    &TestDialogWndGUIEventProc,         // GUI Event Callback function
    NULL,                               // Extra
    NULL,                               // Icon
    NULL                                // Menu Resource
};

/*
 * TestDialogWndGUIEventProc
 */

static bool TestDialogWndGUIEventProc( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;
    char        *new;
    char        *text;
    int         num;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG :
        return( true );
    case GUI_CONTROL_NOT_ACTIVE :
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL :
            num = -1;
            GUIGetCurrSelect( gui, LISTBOX_CONTROL, &num );
            text = GUIGetListItem( gui, LISTBOX_CONTROL, num );
            GUIMemFree( text );
            GUISetListItemData( gui, LISTBOX_CONTROL, num, (void *)num );
            num = (int)GUIGetListItemData( gui, LISTBOX_CONTROL, num );
            break;
        case EDIT_CONTROL :
            new = GUIGetText( gui, EDIT_CONTROL );
            GUIMemFree( new );
            break;
        }
        return( true );
    case GUI_CONTROL_RCLICKED :
        GUI_GETID( param, id );
        text = GUIGetText( gui, id );
        GUIDisplayMessage( gui, text, text, GUI_ABORT_RETRY_IGNORE );
        GUIMemFree( text );
        return( true );
    case GUI_CONTROL_DCLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL :
            num = -1;
            GUIGetCurrSelect( gui, LISTBOX_CONTROL, &num );
            text = GUIGetListItem( gui, LISTBOX_CONTROL, num );
            GUIMemFree( text );
            return( true );
        }
        break;
    case GUI_CONTROL_CLICKED :
        GUIGetFocus( gui, &id );
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL :
            text = GUIGetText( gui, LISTBOX_CONTROL );
            GUIMemFree( text );
            num = -1;
            GUIGetCurrSelect( gui, LISTBOX_CONTROL, &num );
            text = GUIGetListItem( gui, LISTBOX_CONTROL, num );
            GUIMemFree( text );
            return( true );
        case OKBUTTON_CONTROL :
            num = CHECKBOX_CONTROL2;
            if( gui == DialogWindow ) {
                GUIDestroyWnd( gui );
            } else {
                GUIDisplayMessage( gui, "OK Button", "Got dialog item : ",
                                   GUI_ABORT_RETRY_IGNORE );
                GUIGetNewVal( "Enter New Value", "wesley", &text );
                if( text != NULL ) {
                    GUIMemFree( text );
                }
            }
            return( true );
        case CANCELBUTTON_CONTROL :
            GUICloseDialog( gui );
            return( true );
        case EDIT_CONTROL :
            GUIDisplayMessage( gui, "Edit Control", "Got dialog item : ", GUI_QUESTION );
            return( true );
        case STATIC_CONTROL :
            GUIDisplayMessage( gui, "Static Control", "Got dialog item : ", GUI_STOP );
            return( true );
        case ADDBUTTON_CONTROL :
            return( true );
        case CLEARBUTTON_CONTROL :
            return( true );
        }
        break;
    default :
        break;
    }
    return( false );
}

#if 0
static void TestDialogInit( void )
{
    if( !GUIIsGUI() ) {
        DialogScaled = true;
        ControlsScaled = true;
    }
}
#endif

void TestDialogCreate( gui_window *parent )
{
    int         i;
    char        *text;
    char        *ep;

    DialogControl.parent = parent;

    if( !DialogScaled ) {
        SetWidthHeight( &DialogControl.rect, DialogControl.parent != NULL );
        DialogScaled = true;
    }

    if( !GUIIsGUI() && !ButtonsScaled ) {
        for( i = 0; i < NUM_CONTROLS; i++ ) {
            if( ( Controls[i].control_class == GUI_PUSH_BUTTON ) ||
                ( Controls[i].control_class == GUI_DEFPUSH_BUTTON ) ) {
                Controls[i].rect.height *= 2;
            }
        }
        ButtonsScaled = true;
    }

    if( !ControlsScaled ) {
        for( i = 0; i < NUM_CONTROLS; i++ ) {
            Controls[i].parent = NULL;
            SetWidthHeight( &Controls[i].rect, true );
        }
        ControlsScaled = true;
    }

    i = -1;
    while( (i<0) || (i>=13) ) {
        GUIGetNewVal( "Which Control (0 - 12) ( -1 to quit ) ", "0", &text );
        if( text != NULL ) {
            i = strtoul( text, &ep, 10 );
            if( *ep ) {
                i = -1;
            }
            GUIMemFree( text );
        }
        if( i == -1 ) {
            break;
        }
    }

    if( i >=0 && i < 13 ) {
        DialogControl.extra = NULL;
        GUICreateDialog( &DialogControl, 1, &Controls[i] );
    }
}

static bool DummyWndGUIEventProc( gui_window *gui, gui_event gui_ev, void *param )
{
    /* unused parameters */ (void)gui; (void)gui_ev; (void)param;

    return( false );
}

static gui_create_info ResDialog = {
    NULL,                               // Title
    { 0, 0, 0, 0 },                     // Position
    GUI_NOSCROLL,                       // Scroll Styles
    GUI_VISIBLE | GUI_CLOSEABLE,        // Window Styles
    NULL,                               // Parent
    0, NULL,                            // Menu array
    0, NULL,                            // Colour attribute array
    &DummyWndGUIEventProc,              // GUI Event Callback function
    NULL,                               // Extra
    NULL,                               // Icon
    NULL                                // Menu Resource
};

void ResDialogCreate( gui_window *parent )
{
    res_name_or_id  dlg_id;
    char            *text;
    char            *ep;
    unsigned long   id;

    dlg_id = NULL;
    while( dlg_id == NULL ) {
        GUIGetNewVal( "Which Dialog?", "200", &text );
        if( text != NULL ) {
            id = strtoul( text, &ep, 10 );
            if( *ep != '\0' ) {
                dlg_id = NULL;
            } else {
                dlg_id = MAKEINTRESOURCE( id );
            }
            GUIMemFree( text );
        }
    }

    ResDialog.parent = parent;
    GUICreateResDialog( &ResDialog, dlg_id );
}
