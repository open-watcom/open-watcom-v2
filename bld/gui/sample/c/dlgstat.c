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
* Description:  GUI sample program dialogs.
*
****************************************************************************/


#include <string.h>
#include "sample.h"
#include "dlgstat.h"
#include "samprc.h"


extern out_info Child1_Out;

bool            DialogScaled    = false;
bool            ButtonsScaled   = false;
bool            ControlsScaled  = false;
char * ListBoxData[] = { "one", "two", "three" };
int    NUM_LIST_BOX_DATA = ( sizeof( ListBoxData ) / sizeof( char * ) );

static  const char  *LongText = "inserted_really_long_piece_of_text";

static gui_resource DlgWinIcon = { WIN1, "Win1" };

gui_create_info DlgWin = {
    "Dlg Window",
    { 20, 20, 200, 200 },
    GUI_SCROLL_BOTH,
    GUI_SCROLL_EVENTS | GUI_ALL,
    NULL,
    { 0, NULL },                        // Menu array
    { 0, NULL },                        // Colour attribute array
    &Child1WndGUIEventProc,             // GUI Event Callback function
    &Child1_Out,
    &DlgWinIcon,
    NULL                                // Menu Resource
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
    -1},
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
    -1},
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
    "Static Dialog Box",                // Title
    { 100, 100, 700, 700 },             // Position
    GUI_NOSCROLL,                       // Scroll Styles
    GUI_VISIBLE | GUI_CLOSEABLE,        // Window Styles
    NULL,                               // Parent
    { 0, NULL },                        // Menu array
    { 0, NULL },                        // Colour attribute array
    &StaticDialogWndGUIEventProc,       // GUI Event Callback function
    NULL,                               // Extra
    NULL,                               // Icon
    NULL                                // Menu Resource
};

static const char *ListBoxFunc( const void *data_handle, int item )
{
    return( ((const char **)data_handle)[item] );
}

static void ContrCallBack( gui_window *gui, gui_ctl_id id, void *param )
{
    int                 i;
    int                 *num;
    gui_control_class   control_class;

    gui = gui;
    num = (int *)param;
    for( i = 0; i < NUM_CONTROLS; i++ ) {
        if( Controls[i].id == id ) {
            (*num)++;
            if( !GUIGetControlClass( gui, id, &control_class )
              || ( control_class != Controls[i].control_class ) ) {
                GUIDisplayMessage( gui, "Got Invalid Control Class",
                                   "Doing Enum Controls", GUI_INFORMATION );
            }
            return;
        }
    }
    GUIDisplayMessage( gui, "Got Invalid ID", "Doing Enum Controls",
                       GUI_INFORMATION );
}

static void CheckNumControls( gui_window *gui, int act_num )
{
    int num;

    num = 0;
    GUIEnumControls( gui, ContrCallBack, &num );
    if( num == act_num ) {
#if 0
        GUIDisplayMessage( gui, "Got Correct Number of Controls",
                           "Doing Enum Controls", GUI_INFORMATION );
#endif
    } else {
        GUIDisplayMessage( gui, "Got Incorrect Number of Controls",
                           "Doing Enum Controls", GUI_STOP );
    }
}

static void InitDialog( gui_window *parent )
{
    parent = parent;
}

/*
 * StaticDialogWndGUIEventProc
 */

bool StaticDialogWndGUIEventProc( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;
    char        *new;
    unsigned    i;
    char        *text;
    int         sel;
    int         size;
    int         num;
    gui_rect    rect;
    int         extent, top, start, end;
    gui_rgb     rgb, green;

    switch( gui_ev ) {
    case GUI_PAINT :
        GUIGetRGB( GUI_BRIGHT_YELLOW, &rgb );
        GUIGetRGB( GUI_GREEN, &green );
        GUIDrawTextRGB( gui, "Line Draw!", 10, 0, 1, green, rgb );
        GUIDrawTextRGB( gui, "Line Draw!", 10, 1, 1, rgb, green );
        GUIDrawBar( gui,  7, 1, 30, GUI_BAR_SIMPLE, GUI_FRAME_ACTIVE, true );
        GUIDrawBar( gui,  8, 1, 70, GUI_BAR_SIMPLE, GUI_FRAME_ACTIVE, false );
        return( true );
    case GUI_RESIZE :
        return( true );
    case GUI_INIT_DIALOG :
        GUIGetRect( gui, &rect );
        GUIGetClientRect( gui, &rect );
        InitDialog( gui );
        CheckNumControls( gui, NUM_CONTROLS );
        for( id = RADIOBUTTON_CONTROL1_IDX; id <= RADIOBUTTON_CONTROL2_IDX; id++ ) {
            if( (Controls[id].style & GUI_STYLE_CONTROL_CHECKED)
              && (Controls[id].style & GUI_STYLE_CONTROL_AUTOMATIC) == 0 ) {
                GUISetChecked( gui, Controls[id].id, GUI_CHECKED );
            }
        }
        for( id = CHECKBOX_CONTROL1_IDX; id <= CHECKBOX_CONTROL2_IDX; id++ ) {
            if( (Controls[id].style & GUI_STYLE_CONTROL_CHECKED)
              && (Controls[id].style & GUI_STYLE_CONTROL_AUTOMATIC) == 0 ) {
                GUISetChecked( gui, Controls[id].id, GUI_CHECKED );
            }
        }

        for( i = 0; i < NUM_LIST_BOX_DATA; i++ ) {
            GUIInsertText( gui, LISTBOX_CONTROL, 0, ListBoxData[i] );
        }

        GUISetChecked( gui, RADIOBUTTON_CONTROL1, GUI_NOT_CHECKED );


        GUIClearText( gui, STATIC_CONTROL );
        GUISetText( gui, STATIC_CONTROL, "default" );
        text = GUIGetText( gui, STATIC_CONTROL );
        GUIMemFree( text );

        text = GUIGetText( gui, LISTBOX_CONTROL );
        GUIMemFree( text );

        GUIClearText( gui, LISTBOX_CONTROL );
//      GUISetFocus( gui, LISTBOX_CONTROL );

        GUISetText( gui, LISTBOX_CONTROL, "default" );
        GUIDropDown( gui, LISTBOX_CONTROL, true );
//      GUISelectAll( gui, LISTBOX_CONTROL, true );
//      GUISetFocus( gui, CHECKBOX_CONTROL1 );
//      GUISetCurrSelect( gui, LISTBOX_CONTROL, 1 );
        GUISetText( gui, EDIT_CONTROL, "default" );
        //GUISelectAll( gui, EDIT_CONTROL, true );
        GUISetFocus( gui, EDIT_CONTROL );
        DlgWin.parent = gui;
        SetWidthHeight( &DlgWin.rect, DlgWin.parent != NULL );
        GUICreateWindow( &DlgWin );
        return( true );
    case GUI_CONTROL_NOT_ACTIVE :
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL :
            num = -1;
            GUIGetCurrSelect( gui, id, &num );
            text = GUIGetListItem( gui, id, num );
            GUIMemFree( text );
            GUISetListItemData( gui, id, num, (void *)num );
            num = (int)GUIGetListItemData( gui, id, num );
            break;
        case EDIT_CONTROL :
            new = GUIGetText( gui, id );
            GUIMemFree( new );
            break;
        }
        return( true );
    case GUI_CONTROL_RCLICKED :
        GUI_GETID( param, id );
        text = GUIGetText( gui, id );
        GUIDisplayMessage( gui, text, text, GUI_ABORT_RETRY_IGNORE );
        GUIMemFree( text );
        GUIGetRect( gui, &rect );
        rect.width += 25;
        GUIResizeWindow( gui, &rect );
        return( true );
    case GUI_CONTROL_DCLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL :
            num = -1;
            GUIGetCurrSelect( gui, id, &num );
            text = GUIGetListItem( gui, id, num );
            GUIMemFree( text );
            return( true );
        }
        break;
    case GUI_CONTROL_CLICKED :
        GUIGetFocus( gui, &id );
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL :
            text = GUIGetText( gui, id );
            GUIMemFree( text );
            num = -1;
            GUIGetCurrSelect( gui, id, &num );
            text = GUIGetListItem( gui, id, num );
            GUIMemFree( text );
            // GUIDeleteItem( gui, id, num );
            return( true );
        case OKBUTTON_CONTROL :
            for( id = RADIOBUTTON_CONTROL1_IDX; id <= RADIOBUTTON_CONTROL2_IDX; id++ ) {
                if( GUIIsChecked( gui, Controls[id].id ) ) {
                    Controls[id].style |= GUI_STYLE_CONTROL_CHECKED;
                } else {
                    Controls[id].style &= ~GUI_STYLE_CONTROL_CHECKED;
                }
            }
            for( id = CHECKBOX_CONTROL1_IDX; id <= CHECKBOX_CONTROL2_IDX; id++ ) {
                if( GUIIsChecked( gui, Controls[id].id ) ) {
                    Controls[id].style |= GUI_STYLE_CONTROL_CHECKED;
                } else {
                    Controls[id].style &= ~GUI_STYLE_CONTROL_CHECKED;
                }
            }
            text = GUIGetText( gui, LISTBOX_CONTROL );
            GUIMemFree( text );
            sel = -1;
            GUIGetCurrSelect( gui, LISTBOX_CONTROL, &sel );
            if( gui == DialogWindow ) {
                GUIDestroyWnd( gui );
            } else {
                GUIDisplayMessage( gui, "OK Button", "Got dialog item : ",
                                   GUI_ABORT_RETRY_IGNORE );
                GUIGetNewVal( "Enter New Value", "wesley", &text );
                if( text != NULL ) {
                    GUIMemFree( text );
                }
                GUISetFocus( gui, EDIT_CONTROL );
#if 0
                new = GUIGetText( gui, EDIT_CONTROL );
                GUIMemFree( OldValue );
                OldValue = new;
                GUIDisplayMessage( gui, "OK Button", "Got dialog item : ",
                                   GUI_ABORT_RETRY_IGNORE );
                GUICloseDialog( gui );
#endif
            }
            return( true );
        case CANCELBUTTON_CONTROL :
#if 0
            GUIDisplayMessage( gui, "Cancel\nButton CancelButton CancelButtonCancelButton Cancel Button Cancel Button "
            "Cancel Button Cancel Button Cancel\tButton Cancel Button\t\t\tCancel Button", "Got dialog item : ", GUI_STOP );
#endif
            GUICloseDialog( gui );
            return( true );
        case EDIT_CONTROL :
            GUIDisplayMessage( gui, "Edit Control", "Got dialog item : ", GUI_QUESTION );
            return( true );
        case STATIC_CONTROL :
            GUIDisplayMessage( gui, "Static Control", "Got dialog item : ", GUI_STOP );
            return( true );
        case ADDBUTTON_CONTROL :
            GUIAddText( gui, LISTBOX_CONTROL, "lisa" );
            GUIAddTextList( gui, LISTBOX_CONTROL, NUM_LIST_BOX_DATA, ListBoxData, ListBoxFunc );
            GUISelectAll( gui, EDIT_CONTROL, true );
            return( true );
        case CLEARBUTTON_CONTROL :
            if( !GUIIsControlVisible( gui, RADIOBUTTON_CONTROL1 ) ) {
                GUIShowControl( gui, RADIOBUTTON_CONTROL1 );
            } else {
                GUIHideControl( gui, RADIOBUTTON_CONTROL1 );
            }
            GUIEnableControl( gui, RADIOBUTTON_CONTROL2, !GUIIsControlEnabled( gui, RADIOBUTTON_CONTROL2 ) );
            GUIDisplayMessage( gui, "Clearing", "Sample Application", GUI_OK_CANCEL );
            GUISetChecked( gui, RADIOBUTTON_CONTROL2, GUI_CHECKED );
            GUIClearText( gui, STATIC_CONTROL );
            GUIClearText( gui, EDIT_CONTROL );
            size = GUIGetListSize( gui, LISTBOX_CONTROL );
//          GUIClearList( gui, LISTBOX_CONTROL );
            GUIInsertText( gui, LISTBOX_CONTROL, 3, LongText );
            extent = GUIGetExtentX( gui, LongText, strlen( LongText ) );
            GUISetHorizontalExtent( gui, LISTBOX_CONTROL, extent );
            GUISetCurrSelect( gui, LISTBOX_CONTROL, 3 );
            GUISetTopIndex( gui, LISTBOX_CONTROL, 1 );
            top = GUIGetTopIndex( gui, LISTBOX_CONTROL );
            GUISetFocus( gui, EDIT_CONTROL );
            if( GUIGetControlRect( gui, LISTBOX_CONTROL, &rect ) ) {
                rect.width += rect.width / 2;
                GUIResizeControl( gui, LISTBOX_CONTROL, &rect );
            }
            GUIDropDown( gui, LISTBOX_CONTROL, false );
            GUIDeleteControl( gui, CANCELBUTTON_CONTROL );
            GUIGetEditSelect( gui, LISTBOX_CONTROL, &start, &end );
            GUISetEditSelect( gui, LISTBOX_CONTROL, 1, 3 );
            GUIGetEditSelect( gui, LISTBOX_CONTROL, &start, &end );
#if 0
            GUIGetRect( gui, &rect );
            rect.height = ( rect.height * 5 ) / 4;
            if( !GUIResizeWindow( gui, &rect ) ) {
                GUIGetAbsRect( gui, &rect );
                rect.height = ( rect.height * 5 ) / 4;
                GUIResizeWindow( gui, &rect );
            }
#endif
            return( true );
        }
        break;
    default :
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

void StaticDialogCreate( gui_window *parent )
{
    int i;

    Controls[STATIC_CONTROL_IDX].text = OldValue;
    Controls[EDIT_CONTROL_IDX].text = OldValue;
    DialogControl.parent = parent;

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
