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
* Description:  Dynamic dialogs for GUI lib sample program.
*
****************************************************************************/


#include <string.h>
#include "sample.h"
#include "dlgstat.h"
#include "dlgdynam.h"

static bool DynamicDialogEventWnd( gui_window *gui, gui_event gui_ev, void *param );

static  char            *LongText       = "inserted_really_long_piece_of_text";

static gui_create_info DialogControl = {
    "Dynamic Dialog Box",               // Title
    { 150, 250, 700, 700 },             // Position
    GUI_NOSCROLL,                       // Scroll Styles
    GUI_VISIBLE | GUI_CLOSEABLE,        // Window Styles
    NULL,                               // Parent
    0,                                  // Number of menus
    NULL,                               // Menu's
    0,                                  // Number of color attributes
    NULL,                               // Array of color attributes
    &DynamicDialogEventWnd,             // Callback function
    NULL,                               // Extra
    NULL,                               // Icon
    0
};

static char *ListBoxFunc( void *data_handle, int item )
{
    return( ((char **)data_handle)[item] );
}

static void ContrCallBack( gui_window *gui, unsigned id, void *param )
{
    int                 i;
    int                 *num;
    gui_control_class   control_class;

    gui = gui;
    num = (int *)param;
    for( i = 0; i < NUM_CONTROLS; i ++ ) {
        if( Controls[i].id == id ) {
            (*num)++;
            if( !GUIGetControlClass( gui, id, &control_class ) ||
                ( control_class != Controls[i].control_class ) ) {
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
    GUIEnumControls( gui, &ContrCallBack, &num );
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
    int i;

    for( i = 0; i < NUM_CONTROLS; i ++ ) {
        Controls[i].parent = parent;
        if( !ControlsScaled ) {
            SetWidthHeight( &Controls[i].rect, Controls[i].parent != NULL );
        }
        GUIAddControl( &Controls[i], &ToolPlain, &ToolStandout );
    }
    ControlsScaled = true;
}

/*
 * DynamicDialogEventWnd
 */

bool DynamicDialogEventWnd( gui_window *gui, gui_event gui_ev, void *param )
{
    bool        ret;
    unsigned    id;
    char        *new;
    unsigned    i;
    char        *text;
    int         sel;
    int         size;
    int         num;
    gui_rect    rect;
    int         extent, top, start, end;

    ret = true;
    switch( gui_ev ) {
    case GUI_INIT_DIALOG :
        GUIGetRect( gui, &rect );
        GUIGetClientRect( gui, &rect );
        InitDialog( gui );
        CheckNumControls( gui, NUM_CONTROLS );
        for( i = RADIOBUTTON_CONTROL1; i <= RADIOBUTTON_CONTROL2; i++ ) {
            if( ( Controls[i].style & GUI_CHECKED ) &&
                !( Controls[i].style & GUI_AUTOMATIC ) ) {
                GUISetChecked( gui, i, true );
            }
        }
        num = CHECKBOX_CONTROL2;
        for( i = CHECKBOX_CONTROL1; i <= num; i++ ) {
            if( ( Controls[i].style & GUI_CHECKED ) &&
                !( Controls[i].style & GUI_AUTOMATIC ) ) {
                GUISetChecked( gui, i, true );
            }
        }

        for( i = 0; i < NUM_LIST_BOX_DATA; i++ ) {
            GUIInsertText( gui, LISTBOX_CONTROL, 0, ListBoxData[i] );
        }

        GUISetChecked( gui, RADIOBUTTON_CONTROL1, false );

        GUISetText( gui, EDIT_CONTROL, "default" );
        GUISelectAll( gui, EDIT_CONTROL, true );

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
        break;
    case GUI_CONTROL_NOT_ACTIVE :
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL :
            num  = GUIGetCurrSelect( gui, LISTBOX_CONTROL );
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
        break;
    case GUI_CONTROL_RCLICKED :
        GUI_GETID( param, id );
        text = GUIGetText( gui, id );
        GUIDisplayMessage( gui, text, text, GUI_ABORT_RETRY_IGNORE );
        GUIMemFree( text );
        break;
    case GUI_CONTROL_DCLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL :
            num  = GUIGetCurrSelect( gui, LISTBOX_CONTROL );
            text = GUIGetListItem( gui, LISTBOX_CONTROL, num );
            GUIMemFree( text );
            break;
        }
        break;
    case GUI_CONTROL_CLICKED :
        GUIGetFocus( gui, &id );
        GUI_GETID( param, id );
        switch( id ) {
        case LISTBOX_CONTROL :
            text = GUIGetText( gui, LISTBOX_CONTROL );
            GUIMemFree( text );
            num  = GUIGetCurrSelect( gui, LISTBOX_CONTROL );
            text = GUIGetListItem( gui, LISTBOX_CONTROL, num );
            GUIMemFree( text );
           // GUIDeleteItem( gui, LISTBOX_CONTROL, num );
            break;
        case OKBUTTON_CONTROL :
            for( i = RADIOBUTTON_CONTROL1; i <= RADIOBUTTON_CONTROL2; i++ ) {
                if( GUIIsChecked( gui, i ) ) {
                    Controls[i].style |= GUI_CHECKED;
                } else {
                    Controls[i].style &= ~GUI_CHECKED;
                }
            }
            num = CHECKBOX_CONTROL2;
            for( i = CHECKBOX_CONTROL1; i <= num; i++ ) {
                if( GUIIsChecked( gui, i ) ) {
                    Controls[i].style |= GUI_CHECKED;
                } else {
                    Controls[i].style &= ~GUI_CHECKED;
                }
            }
            text = GUIGetText( gui, LISTBOX_CONTROL );
            GUIMemFree( text );
            sel = GUIGetCurrSelect( gui, LISTBOX_CONTROL );
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
            break;
        case CANCELBUTTON_CONTROL :
#if 0
            GUIDisplayMessage( gui, "Cancel\nButton CancelButton CancelButtonCancelButton Cancel Button Cancel Button Cancel Button Cancel Button Cancel\tButton Cancel Button\t\t\tCancel Button", "Got dialog item : ",
                               GUI_STOP );
#endif
            GUICloseDialog( gui );
            break;
        case EDIT_CONTROL :
            GUIDisplayMessage( gui, "Edit Control", "Got dialog item : ",
                               GUI_QUESTION );
            break;
        case STATIC_CONTROL :
            GUIDisplayMessage( gui, "Static Control", "Got dialog item : ",
                               GUI_STOP );
            break;
        case ADDBUTTON_CONTROL :
            GUIAddText( gui, LISTBOX_CONTROL, "lisa" );
            GUIAddTextList( gui, LISTBOX_CONTROL, NUM_LIST_BOX_DATA, ListBoxData, &ListBoxFunc );
            break;
        case CLEARBUTTON_CONTROL :
            if( !GUIIsControlVisible( gui, RADIOBUTTON_CONTROL1 ) ) {
                GUIShowControl( gui, RADIOBUTTON_CONTROL1 );
            } else {
                GUIHideControl( gui, RADIOBUTTON_CONTROL1 );
            }
            GUIEnableControl( gui, RADIOBUTTON_CONTROL2,
                              !GUIIsControlEnabled( gui, RADIOBUTTON_CONTROL2 ) );
            GUIDisplayMessage( gui, "Clearing", "Sample Application", GUI_OK_CANCEL );
            GUISetChecked( gui, RADIOBUTTON_CONTROL2, true );
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
            break;
        }
        break;
    default :
        ret = false;
        break;
    }
    return( ret );
}

void DynamicDialogCreate( gui_window *parent )
{
    int i;

    Controls[STATIC_CONTROL].text = OldValue;
    Controls[EDIT_CONTROL].text = OldValue;
    DialogControl.parent = parent;

    if( !DialogScaled ) {
        SetWidthHeight( &DialogControl.rect, DialogControl.parent != NULL );
        DialogScaled = true;
    }

    if( !GUIIsGUI() && !ButtonsScaled ) {
        for( i = 0; i < NUM_CONTROLS; i ++ ) {
            if( ( Controls[i].control_class == GUI_PUSH_BUTTON ) ||
                ( Controls[i].control_class == GUI_DEFPUSH_BUTTON ) ) {
                Controls[i].rect.height *= 2;
            }
        }
        ButtonsScaled = true;
    }

    if( !ControlsScaled ) {
        for( i = 0; i < NUM_CONTROLS; i ++ ) {
            Controls[i].parent = NULL;
            SetWidthHeight( &Controls[i].rect, true );
        }
        ControlsScaled = true;
    }

    DialogControl.extra = NULL;
    GUICreateDialog( &DialogControl, 0, NULL );
}
