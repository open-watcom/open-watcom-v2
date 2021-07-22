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
* Description:  GUI sample program dialogs.
*
****************************************************************************/


#include <string.h>
#include "sample.h"
#include "dlgstat.h"
#include "samprc.rh"


#define GUI_NO_ID   ((gui_ctl_id)-1)

extern out_info Child1_Out;

bool            DialogScaled    = false;
bool            ButtonsScaled   = false;
bool            ControlsScaled  = false;
char * ListBoxData[] = { "one", "two", "three" };
int    NUM_LIST_BOX_DATA = GUI_ARRAY_SIZE( ListBoxData );

static  const char  *LongText = "inserted_really_long_piece_of_text";

static gui_resource DlgWinIcon = { WIN1, "Win1" };

gui_create_info DlgWin = {
    "Dlg Window",
    { 20, 20, 200, 200 },
    GUI_SCROLL_BOTH,
    GUI_SCROLL_EVENTS | GUI_ALL,
    NULL,
    GUI_NO_MENU,                    // Menu array
    GUI_NO_COLOUR,                  // Colour attribute array
    &Child1WndGUIEventProc,         // GUI Event Callback function
    &Child1_Out,                    // Extra
    &DlgWinIcon,                    // Icon
    NULL                            // Menu Resource
};

gui_control_info Controls[] = {
  { GUI_STATIC,
    "Hi there",
    { 25, 180, 125, 45 },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_AUTOMATIC,
    STATIC_CONTROL },
  { GUI_EDIT,
    "hello",
    { 175, 180, 100, 120 },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC | GUI_STYLE_CONTROL_FOCUS | GUI_STYLE_CONTROL_MULTILINE | GUI_STYLE_CONTROL_WANTRETURN,
    EDIT_CONTROL },
  { GUI_GROUPBOX,
    "Group 1",
    { 50, 270, 300, 135 },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_AUTOMATIC,
    GUI_NO_ID},
  { GUI_RADIO_BUTTON,
    "RA&DIO 1",
    { 25, 45, BUTTON_WIDTH, BUTTON_HEIGHT },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_GROUP | GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC,
    RADIOBUTTON_CONTROL1 },
  { GUI_RADIO_BUTTON,
    "&RADIO 2",
    { 150, 45, BUTTON_WIDTH, BUTTON_HEIGHT },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_GROUP | GUI_STYLE_CONTROL_AUTOMATIC | GUI_STYLE_CONTROL_CHECKED,
    RADIOBUTTON_CONTROL2 },
  { GUI_GROUPBOX,
    "Group 2",
    { 10, 270, 340, 135 },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_AUTOMATIC,
    GUI_NO_ID},
  { GUI_CHECK_BOX,
    "&CHECK 1",
    { 25, 315, BUTTON_WIDTH + 20, BUTTON_HEIGHT },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_GROUP | GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC,
    CHECKBOX_CONTROL1 },
  { GUI_CHECK_BOX,
    "CHECK &2",
    { 170, 315, BUTTON_WIDTH + 20, BUTTON_HEIGHT },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_GROUP | GUI_STYLE_CONTROL_CHECKED | GUI_STYLE_CONTROL_AUTOMATIC,
    CHECKBOX_CONTROL2 },
  { GUI_DEFPUSH_BUTTON,
    "OK",
    { 175, 450, BUTTON_WIDTH, BUTTON_HEIGHT },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC,
    OKBUTTON_CONTROL },
  { GUI_PUSH_BUTTON,
    "Cancel",
    { 25, 450, BUTTON_WIDTH, BUTTON_HEIGHT },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC,
    CANCELBUTTON_CONTROL } ,
  { GUI_EDIT_COMBOBOX,
    "Title",
    { 350, 45, 150, 250 },
    NULL,
    GUI_VSCROLL | GUI_HSCROLL,
    GUI_STYLE_CONTROL_TAB_GROUP,
    LISTBOX_CONTROL },
  { GUI_PUSH_BUTTON,
    "&Add Data",
    { 325, 450, BUTTON_WIDTH, BUTTON_HEIGHT },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC,
    ADDBUTTON_CONTROL } ,
  { GUI_PUSH_BUTTON,
    "C&lear",
    { 475, 450, BUTTON_WIDTH, BUTTON_HEIGHT },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC,
    CLEARBUTTON_CONTROL }
};

static gui_create_info DialogControl = {
    "Static Dialog Box",            // Title
    { 100, 100, 700, 700 },         // Position
    GUI_NOSCROLL,                   // Scroll Styles
    GUI_VISIBLE | GUI_CLOSEABLE,    // Window Styles
    NULL,                           // Parent
    GUI_NO_MENU,                    // Menu array
    GUI_NO_COLOUR,                  // Colour attribute array
    &StaticDialogWndGUIEventProc,   // GUI Event Callback function
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
                GUIDisplayMessage( wnd, "Got Invalid Control Class",
                                   "Doing Enum Controls", GUI_INFORMATION );
            }
            return;
        }
    }
    GUIDisplayMessage( wnd, "Got Invalid ID", "Doing Enum Controls",
                       GUI_INFORMATION );
}

static void CheckNumControls( gui_window *wnd, int act_num )
{
    int num;

    num = 0;
    GUIEnumControls( wnd, ContrCallBack, &num );
    if( num == act_num ) {
#if 0
        GUIDisplayMessage( wnd, "Got Correct Number of Controls",
                           "Doing Enum Controls", GUI_INFORMATION );
#endif
    } else {
        GUIDisplayMessage( wnd, "Got Incorrect Number of Controls",
                           "Doing Enum Controls", GUI_STOP );
    }
}

static void InitDialog( gui_window *parent_wnd )
{
    parent_wnd = parent_wnd;
}

/*
 * StaticDialogWndGUIEventProc
 */

bool StaticDialogWndGUIEventProc( gui_window *wnd, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;
    char        *new;
    int         i;
    char        *text;
    int         sel;
    int         size;
    int         num;
    gui_rect    rect;
    int         extent, top, start, end;
    gui_rgb     rgb, green;

    switch( gui_ev ) {
    case GUI_PAINT:
        GUIGetRGB( GUI_BR_YELLOW, &rgb );
        GUIGetRGB( GUI_GREEN, &green );
        GUIDrawTextRGB( wnd, "Line Draw!", 10, 0, 1, green, rgb );
        GUIDrawTextRGB( wnd, "Line Draw!", 10, 1, 1, rgb, green );
        GUIDrawBar( wnd,  7, 1, 30, GUI_BAR_SIMPLE, GUI_FRAME_ACTIVE, true );
        GUIDrawBar( wnd,  8, 1, 70, GUI_BAR_SIMPLE, GUI_FRAME_ACTIVE, false );
        return( true );
    case GUI_RESIZE:
        return( true );
    case GUI_INIT_DIALOG:
        GUIGetRect( wnd, &rect );
        GUIGetClientRect( wnd, &rect );
        InitDialog( wnd );
        CheckNumControls( wnd, NUM_CONTROLS );
        for( id = RADIOBUTTON_CONTROL1_IDX; id <= RADIOBUTTON_CONTROL2_IDX; id++ ) {
            if( (Controls[id].style & GUI_STYLE_CONTROL_CHECKED)
              && (Controls[id].style & GUI_STYLE_CONTROL_AUTOMATIC) == 0 ) {
                GUISetChecked( wnd, Controls[id].id, GUI_CHECKED );
            }
        }
        for( id = CHECKBOX_CONTROL1_IDX; id <= CHECKBOX_CONTROL2_IDX; id++ ) {
            if( (Controls[id].style & GUI_STYLE_CONTROL_CHECKED)
              && (Controls[id].style & GUI_STYLE_CONTROL_AUTOMATIC) == 0 ) {
                GUISetChecked( wnd, Controls[id].id, GUI_CHECKED );
            }
        }

        for( i = 0; i < NUM_LIST_BOX_DATA; i++ ) {
            GUIInsertText( wnd, LISTBOX_CONTROL, 0, ListBoxData[i] );
        }

        GUISetChecked( wnd, RADIOBUTTON_CONTROL1, GUI_NOT_CHECKED );


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
        GUISetText( wnd, EDIT_CONTROL, "default" );
        //GUISelectAll( wnd, EDIT_CONTROL, true );
        GUISetFocus( wnd, EDIT_CONTROL );
        DlgWin.parent = wnd;
        SetWidthHeight( &DlgWin.rect, DlgWin.parent != NULL );
        GUICreateWindow( &DlgWin );
        return( true );
    case GUI_CONTROL_NOT_ACTIVE:
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL:
            num = -1;
            GUIGetCurrSelect( wnd, id, &num );
            text = GUIGetListItem( wnd, id, num );
            GUIMemFree( text );
            GUISetListItemData( wnd, id, num, (void *)num );
            num = (int)GUIGetListItemData( wnd, id, num );
            break;
        case EDIT_CONTROL:
            new = GUIGetText( wnd, id );
            GUIMemFree( new );
            break;
        }
        return( true );
    case GUI_CONTROL_RCLICKED:
        GUI_GETID( param, id );
        text = GUIGetText( wnd, id );
        GUIDisplayMessage( wnd, text, text, GUI_ABORT_RETRY_IGNORE );
        GUIMemFree( text );
        GUIGetRect( wnd, &rect );
        rect.width += 25;
        GUIResizeWindow( wnd, &rect );
        return( true );
    case GUI_CONTROL_DCLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL:
            num = -1;
            GUIGetCurrSelect( wnd, id, &num );
            text = GUIGetListItem( wnd, id, num );
            GUIMemFree( text );
            return( true );
        }
        break;
    case GUI_CONTROL_CLICKED:
        GUIGetFocus( wnd, &id );
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL:
            text = GUIGetText( wnd, id );
            GUIMemFree( text );
            num = -1;
            GUIGetCurrSelect( wnd, id, &num );
            text = GUIGetListItem( wnd, id, num );
            GUIMemFree( text );
            // GUIDeleteItem( wnd, id, num );
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
                GUIDisplayMessage( wnd, "OK Button", "Got dialog item : ",
                                   GUI_ABORT_RETRY_IGNORE );
                GUIGetNewVal( "Enter New Value", "wesley", &text );
                if( text != NULL ) {
                    GUIMemFree( text );
                }
                GUISetFocus( wnd, EDIT_CONTROL );
#if 0
                new = GUIGetText( wnd, EDIT_CONTROL );
                GUIMemFree( OldValue );
                OldValue = new;
                GUIDisplayMessage( wnd, "OK Button", "Got dialog item : ",
                                   GUI_ABORT_RETRY_IGNORE );
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
            GUISelectAll( wnd, EDIT_CONTROL, true );
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
        break;
    default:
        break;
    }
    return( false );
}

void StaticDialogInit( void )
{
    if( !GUIIsGUI() ) {
        DialogScaled = true;
        ControlsScaled = true;
    }
}

void StaticDialogCreate( gui_window *parent_wnd )
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
    GUICreateDialog( &DialogControl, NUM_CONTROLS, Controls );
}
