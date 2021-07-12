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
* Description:  Dynamic dialogs for GUI lib sample program.
*
****************************************************************************/


#include <string.h>
#include "sample.h"
#include "dlgstat.h"
#include "dlgdynam.h"

static GUICALLBACK DynamicDialogWndGUIEventProc;

static  const char  *LongText = "inserted_really_long_piece_of_text";

static gui_create_info DialogControl = {
    "Dynamic Dialog Box",           // Title
    { 150, 250, 700, 700 },         // Position
    GUI_NOSCROLL,                   // Scroll Styles
    GUI_VISIBLE | GUI_CLOSEABLE,    // Window Styles
    NULL,                           // Parent
    GUI_NO_MENU,                    // Menu array
    GUI_NO_COLOUR,                  // Colour attribute array
    &DynamicDialogWndGUIEventProc,  // GUI Event Callback function
    NULL,                           // Extra
    NULL,                           // Icon
    NULL                            // Menu Resource
};

static const char *ListBoxFunc( const void *data_handle, int item )
{
    return( ((const char **)data_handle)[item] );
}

static void ContrCallBack( gui_window *wnd, gui_ctl_id id, void *param )
{
    int                 i;
    int                 *num;
    gui_control_class   control_class;

    num = (int *)param;
    for( i = 0; i < NUM_CONTROLS; i++ ) {
        if( Controls[i].id == id ) {
            (*num)++;
            if( !GUIGetControlClass( wnd, id, &control_class )
              || ( control_class != Controls[i].control_class ) ) {
                GUIDisplayMessage( wnd, "Got Invalid Control Class", "Doing Enum Controls", GUI_INFORMATION );
            }
            return;
        }
    }
    GUIDisplayMessage( wnd, "Got Invalid ID", "Doing Enum Controls", GUI_INFORMATION );
}

static void CheckNumControls( gui_window *wnd, int act_num )
{
    int num;

    num = 0;
    GUIEnumControls( wnd, ContrCallBack, &num );
    if( num == act_num ) {
#if 0
        GUIDisplayMessage( wnd, "Got Correct Number of Controls", "Doing Enum Controls", GUI_INFORMATION );
#endif
    } else {
        GUIDisplayMessage( wnd, "Got Incorrect Number of Controls", "Doing Enum Controls", GUI_STOP );
    }
}

static void InitDialog( gui_window *parent_wnd )
{
    int i;

    for( i = 0; i < NUM_CONTROLS; i++ ) {
        Controls[i].parent = parent_wnd;
        if( !ControlsScaled ) {
            SetWidthHeight( &Controls[i].rect, Controls[i].parent != NULL );
        }
        GUIAddControl( &Controls[i], &ToolPlain, &ToolStandout );
    }
    ControlsScaled = true;
}

/*
 * DynamicDialogWndGUIEventProc
 */

static bool DynamicDialogWndGUIEventProc( gui_window *wnd, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;
    char        *new;
    unsigned    j;
    int         i;
    char        *text;
    int         sel;
    int         size;
    int         num;
    gui_rect    rect;
    int         extent, top, start, end;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        GUIGetRect( wnd, &rect );
        GUIGetClientRect( wnd, &rect );
        InitDialog( wnd );
        CheckNumControls( wnd, NUM_CONTROLS );
        for( j = RADIOBUTTON_CONTROL1_IDX; j <= RADIOBUTTON_CONTROL2_IDX; j++ ) {
            if( (Controls[j].style & GUI_STYLE_CONTROL_CHECKED)
              && (Controls[j].style & GUI_STYLE_CONTROL_AUTOMATIC) == 0 ) {
                GUISetChecked( wnd, Controls[j].id, GUI_CHECKED );
            }
        }
        for( j = CHECKBOX_CONTROL1_IDX; j <= CHECKBOX_CONTROL2_IDX; j++ ) {
            if( (Controls[j].style & GUI_STYLE_CONTROL_CHECKED)
              && (Controls[j].style & GUI_STYLE_CONTROL_AUTOMATIC) == 0 ) {
                GUISetChecked( wnd, Controls[j].id, GUI_CHECKED );
            }
        }

        for( i = 0; i < NUM_LIST_BOX_DATA; i++ ) {
            GUIInsertText( wnd, LISTBOX_CONTROL, 0, ListBoxData[i] );
        }

        GUISetChecked( wnd, RADIOBUTTON_CONTROL1, GUI_NOT_CHECKED );

        GUISetText( wnd, EDIT_CONTROL, "default" );
        GUISelectAll( wnd, EDIT_CONTROL, true );

        GUIClearText( wnd, STATIC_CONTROL );
        GUISetText( wnd, STATIC_CONTROL, "default" );
        text = GUIGetText( wnd, STATIC_CONTROL );
        GUIMemFree( text );

        text = GUIGetText( wnd, LISTBOX_CONTROL );
        GUIMemFree( text );

        GUIClearText( wnd, LISTBOX_CONTROL );
//      GUISetFocus( wnd, LISTBOX_CONTROL );

        GUISetText( wnd, LISTBOX_CONTROL, "default" );
        GUIDropDown( wnd, LISTBOX_CONTROL, true );
//      GUISelectAll( wnd, LISTBOX_CONTROL, true );
//      GUISetFocus( wnd, CHECKBOX_CONTROL1 );
//      GUISetCurrSelect( wnd, LISTBOX_CONTROL, 1 );
        return( true );
    case GUI_CONTROL_NOT_ACTIVE:
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL:
            num = -1;
            GUIGetCurrSelect( wnd, LISTBOX_CONTROL, &num );
            text = GUIGetListItem( wnd, LISTBOX_CONTROL, num );
            GUIMemFree( text );
            GUISetListItemData( wnd, LISTBOX_CONTROL, num, (void *)num );
            num = (int)GUIGetListItemData( wnd, LISTBOX_CONTROL, num );
            break;
        case EDIT_CONTROL:
            new = GUIGetText( wnd, EDIT_CONTROL );
            GUIMemFree( new );
            break;
        }
        return( true );
    case GUI_CONTROL_RCLICKED:
        GUI_GETID( param, id );
        text = GUIGetText( wnd, id );
        GUIDisplayMessage( wnd, text, text, GUI_ABORT_RETRY_IGNORE );
        GUIMemFree( text );
        return( true );
    case GUI_CONTROL_DCLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL:
            num = -1;
            GUIGetCurrSelect( wnd, LISTBOX_CONTROL, &num );
            text = GUIGetListItem( wnd, LISTBOX_CONTROL, num );
            GUIMemFree( text );
            return( true );
        }
        break;
    case GUI_CONTROL_CLICKED:
        GUIGetFocus( wnd, &id );
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL:
            text = GUIGetText( wnd, LISTBOX_CONTROL );
            GUIMemFree( text );
            num = -1;
            GUIGetCurrSelect( wnd, LISTBOX_CONTROL, &num );
            text = GUIGetListItem( wnd, LISTBOX_CONTROL, num );
            GUIMemFree( text );
            // GUIDeleteItem( wnd, LISTBOX_CONTROL, num );
            return( true );
        case OKBUTTON_CONTROL:
            for( id = RADIOBUTTON_CONTROL1_IDX; id <= RADIOBUTTON_CONTROL2_IDX; id++ ) {
                if( GUIIsChecked( wnd, Controls[id].id ) ) {
                    Controls[id].style |= GUI_STYLE_CONTROL_CHECKED;
                } else {
                    Controls[id].style &= ~GUI_STYLE_CONTROL_CHECKED;
                }
            }
            for( id = CHECKBOX_CONTROL1_IDX; id <= CHECKBOX_CONTROL2_IDX; id++ ) {
                if( GUIIsChecked( wnd, Controls[id].id ) ) {
                    Controls[id].style |= GUI_STYLE_CONTROL_CHECKED;
                } else {
                    Controls[id].style &= ~GUI_STYLE_CONTROL_CHECKED;
                }
            }
            text = GUIGetText( wnd, LISTBOX_CONTROL );
            GUIMemFree( text );
            sel = -1;
            GUIGetCurrSelect( wnd, LISTBOX_CONTROL, &sel );
            if( wnd == DialogWindow ) {
                GUIDestroyWnd( wnd );
            } else {
                GUIDisplayMessage( wnd, "OK Button", "Got dialog item : ", GUI_ABORT_RETRY_IGNORE );
                GUIGetNewVal( "Enter New Value", "wesley", &text );
                if( text != NULL ) {
                    GUIMemFree( text );
                }
                GUISetFocus( wnd, EDIT_CONTROL );
#if 0
                new = GUIGetText( wnd, EDIT_CONTROL );
                GUIMemFree( OldValue );
                OldValue = new;
                GUIDisplayMessage( wnd, "OK Button", "Got dialog item : ", GUI_ABORT_RETRY_IGNORE );
                GUICloseDialog( wnd );
#endif
            }
            return( true );
        case CANCELBUTTON_CONTROL:
#if 0
            GUIDisplayMessage( wnd, "Cancel\nButton CancelButton CancelButtonCancelButton Cancel Button Cancel Button "
            "Cancel Button Cancel Button Cancel\tButton Cancel Button\t\t\tCancel Button", "Got dialog item : ", GUI_STOP );
#endif
            GUICloseDialog( wnd );
            return( true );
        case EDIT_CONTROL:
            GUIDisplayMessage( wnd, "Edit Control", "Got dialog item : ", GUI_QUESTION );
            return( true );
        case STATIC_CONTROL:
            GUIDisplayMessage( wnd, "Static Control", "Got dialog item : ", GUI_STOP );
            return( true );
        case ADDBUTTON_CONTROL:
            GUIAddText( wnd, LISTBOX_CONTROL, "lisa" );
            GUIAddTextList( wnd, LISTBOX_CONTROL, NUM_LIST_BOX_DATA, ListBoxData, ListBoxFunc );
            return( true );
        case CLEARBUTTON_CONTROL:
            if( !GUIIsControlVisible( wnd, RADIOBUTTON_CONTROL1 ) ) {
                GUIShowControl( wnd, RADIOBUTTON_CONTROL1 );
            } else {
                GUIHideControl( wnd, RADIOBUTTON_CONTROL1 );
            }
            GUIEnableControl( wnd, RADIOBUTTON_CONTROL2, !GUIIsControlEnabled( wnd, RADIOBUTTON_CONTROL2 ) );
            GUIDisplayMessage( wnd, "Clearing", "Sample Application", GUI_OK_CANCEL );
            GUISetChecked( wnd, RADIOBUTTON_CONTROL2, GUI_CHECKED );
            GUIClearText( wnd, STATIC_CONTROL );
            GUIClearText( wnd, EDIT_CONTROL );
            size = GUIGetListSize( wnd, LISTBOX_CONTROL );
//          GUIClearList( wnd, LISTBOX_CONTROL );
            GUIInsertText( wnd, LISTBOX_CONTROL, 3, LongText );
            extent = GUIGetExtentX( wnd, LongText, strlen( LongText ) );
            GUISetHorizontalExtent( wnd, LISTBOX_CONTROL, extent );
            GUISetCurrSelect( wnd, LISTBOX_CONTROL, 3 );
            GUISetTopIndex( wnd, LISTBOX_CONTROL, 1 );
            top = GUIGetTopIndex( wnd, LISTBOX_CONTROL );
            GUISetFocus( wnd, EDIT_CONTROL );
            if( GUIGetControlRect( wnd, LISTBOX_CONTROL, &rect ) ) {
                rect.width += rect.width / 2;
                GUIResizeControl( wnd, LISTBOX_CONTROL, &rect );
            }
            GUIDropDown( wnd, LISTBOX_CONTROL, false );
            GUIDeleteControl( wnd, CANCELBUTTON_CONTROL );
            GUIGetEditSelect( wnd, LISTBOX_CONTROL, &start, &end );
            GUISetEditSelect( wnd, LISTBOX_CONTROL, 1, 3 );
            GUIGetEditSelect( wnd, LISTBOX_CONTROL, &start, &end );
#if 0
            GUIGetRect( wnd, &rect );
            rect.height = ( rect.height * 5 ) / 4;
            if( !GUIResizeWindow( wnd, &rect ) ) {
                GUIGetAbsRect( wnd, &rect );
                rect.height = ( rect.height * 5 ) / 4;
                GUIResizeWindow( wnd, &rect );
            }
#endif
            return( true );
        }
    default:
        break;
    }
    return( false );
}

void DynamicDialogCreate( gui_window *parent_wnd )
{
    int i;

    Controls[STATIC_CONTROL_IDX].text = OldValue;
    Controls[EDIT_CONTROL_IDX].text = OldValue;
    DialogControl.parent = parent_wnd;

    if( !DialogScaled ) {
        SetWidthHeight( &DialogControl.rect, DialogControl.parent != NULL );
        DialogScaled = true;
    }

    if( !GUIIsGUI() && !ButtonsScaled ) {
        for( i = 0; i < NUM_CONTROLS; i++ ) {
            if( ( Controls[i].control_class == GUI_PUSH_BUTTON )
              || ( Controls[i].control_class == GUI_DEFPUSH_BUTTON ) ) {
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

    DialogControl.extra = NULL;
    GUICreateDialog( &DialogControl, 0, NULL );
}
