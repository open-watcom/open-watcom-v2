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
* Description:  Generate dynamic dialogs.
*
****************************************************************************/


/*
*   COMMENTS: This generic dialog box module replaces the old dialog box
*             functions for - Welcome Dialog
*                           - Modify Dialog
*             and is general enough for other dialogs that may be needed
*             in the future.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "setup.h"
#include "guistr.h"
#include "guidlg.h"
#include "dlgbutn.h"
#include "setupinf.h"
#include "dlggen.h"
#include "genvbl.h"
#include "gendlg.h"
#include "utils.h"
#include "ctype.h"

#include "clibext.h"


#define HELP_PREFIX     "Help_"

/* A few new defines, rather than hard numbers in source */
/* Controls per line */
#define MAX_CTRL_PER_LINE      4
#define NORMAL_BUTTONS         (MAX_CTRL_PER_LINE - 1)
#define NORMAL_CHECKMARKS      2

/* Extra chars for CONTROL width calculations */
/* Necessary to handle different character widths when */
/* using proportional fonts (most graphical UI's).     */
#define BUTTON_EXTRA           2

/* Ends up close to horizontal default size for button in dialogs */
#define WIN_BW                15

#define WIN_BUTTON_POS( num, of, cols, bwidth ) \
    (of == 1 ? \
        (cols - bwidth) / 2 : \
        (1 + (num - 1) * (bwidth + BUTTON_GAP( cols, of, bwidth, 1 ))))


typedef struct dlg_window_set {
    dlg_state           state;
    a_dialog_header     *dlg;       /* stuff needed in future */
} DLG_WINDOW_SET;

bool    VisibilityCondition = false;

static gui_control_class ControlClass( gui_ctl_id id, a_dialog_header *dlg )
/**************************************************************************/
/* return the control class of a variable based on its id. */
{
    int                 i;

    for( i = 0; i < dlg->num_controls; i++ ) {
        if( dlg->controls[i].id == id ) {
            return( dlg->controls[i].control_class );
        }
    }
    SetupError( "IDS_CONTROLCLASSERROR" );
    return( GUI_BAD_CLASS );
}


static void SetDynamic( gui_window *gui, vhandle var_handle, bool *drive_checked )
/********************************************************************************/
{
    VBUF        buff;
    const char  *p;

    if( !*drive_checked ) {
        for( p = VarGetStrVal( var_handle ); *p != '\0'; p++ ) {
            if( *p == '%' ) {
                if( strnicmp( p + 1, "DriveFree", 9 ) == 0 ) {
                    CheckDrive( false );
                    *drive_checked = true;
                }
            }
        }
    }
    VbufInit( &buff );
    ReplaceVars( &buff, VarGetStrVal( var_handle ) );
    AddInstallName( &buff );
    GUISetText( gui, VH2ID( var_handle ), VbufString( &buff ) );
    VbufFree( &buff );
}


static void SetDefaultVals( gui_window *gui, a_dialog_header *dlg )
/*****************************************************************/
/* Set the default variable values. Decide how to set these  */
/* default values based on edit control type. */
{
    int                 i;
    vhandle             var_handle;
    char                *cond;
    bool                drive_checked;
    gui_ctl_id          id;

    drive_checked = false;
    for( i = 0; (var_handle = dlg->pVariables[i]) != NO_VAR; ++i ) {
        cond = dlg->pConditions[i];
        if( !dlg->defaults_set && cond != NULL && !VarGetBoolVal( var_handle ) ) {
            if( isdigit( *cond ) ) {
                SetVariableByHandle( var_handle, cond );
            } else if( EvalCondition( cond ) ) {
                SetBoolVariableByHandle( var_handle, true );
            }
        }
        id = VH2ID( var_handle );
        switch( ControlClass( id, dlg ) ) {
        case GUI_STATIC:
            SetDynamic( gui, var_handle, &drive_checked );
            break;
        case GUI_RADIO_BUTTON:
        case GUI_CHECK_BOX:
            GUISetChecked( gui, id, VarGetBoolVal( var_handle ) ? GUI_CHECKED : GUI_NOT_CHECKED );
            break;
        case GUI_EDIT_MLE:
        case GUI_EDIT:
            GUISetText( gui, id, VarGetStrVal( var_handle ) );
            break;
        default:
            break;
        }
    }
    dlg->defaults_set = true;
}

static void SetFocusCtrl( gui_window *gui, a_dialog_header *dlg )
/***************************************************************/
/* Set the default variable values. Decide how to set these  */
/* default values based on edit control type. */
{
    int                 i, j;
    gui_control_class   control_class;

    for( i = 0; i < dlg->num_controls; i++ ) {
        switch( dlg->controls[i].control_class ) {
        case GUI_EDIT_MLE:
        case GUI_EDIT:
            GUISetFocus( gui, dlg->controls[i].id );
            return;
        case GUI_RADIO_BUTTON:
        case GUI_CHECK_BOX:
            for( j = i; j < dlg->num_controls; ++j ) {
                control_class = dlg->controls[j].control_class;
                if( control_class != GUI_RADIO_BUTTON && control_class != GUI_CHECK_BOX ) {
                    break;
                }
                if( GUIIsChecked( gui, dlg->controls[j].id ) == GUI_CHECKED ) {
                    GUISetFocus( gui, dlg->controls[j].id );
                    return;
                }
            }
            GUISetFocus( gui, dlg->controls[i].id );
            return;
        default:
            break;
        }
    }
    for( i = 0; i < dlg->num_controls; i++ ) {
        if( dlg->controls[i].control_class == GUI_DEFPUSH_BUTTON ) {
            GUISetFocus( gui, dlg->controls[i].id );
            return;
        }
    }
}


static void GetVariableVals( gui_window *gui, a_dialog_header *dlg, bool closing )
/********************************************************************************/
/* Get the input variable values. Decide how to set these */
/* default values based on edit control type. */
{
    char                *text;
    int                 i;
    vhandle             var_handle;
    bool                drive_checked;
    gui_ctl_id          id;

    drive_checked = false;
    for( i = 0; (var_handle = dlg->pVariables[i]) != NO_VAR; i++ ) {
        id = VH2ID( var_handle );
        switch( ControlClass( id, dlg ) ) {
        case GUI_STATIC:
            if( !closing ) {
                SetDynamic( gui, var_handle, &drive_checked );
            }
            break;
        case GUI_RADIO_BUTTON:
        case GUI_CHECK_BOX:
            if( GUIIsChecked( gui, id ) == GUI_CHECKED ) {
                if( VarIsRestrictedFalse( var_handle ) ) {
                    if( !closing ) {
                        MsgBox( gui, "IDS_NODISKFOROPTION", GUI_OK );
                        GUISetChecked( gui, id, GUI_NOT_CHECKED );
                        if( var_handle == FullInstall ) {
                            GUISetChecked( gui, VH2ID( SelectiveInstall ), GUI_CHECKED );
                        }
                    }
                    SetBoolVariableByHandle( var_handle, false );
                    if( var_handle == FullInstall ) {
                        SetBoolVariableByHandle( SelectiveInstall, true );
                    }
                } else {
                    SetBoolVariableByHandle( var_handle, true );
                }
            } else {
                SetBoolVariableByHandle( var_handle, false );
            }
            break;
        case GUI_EDIT:
            text = GUIGetText( gui, id );
            if( text != NULL ) {
                SetVariableByHandle( var_handle, text );
                GUIMemFree( text );
            }
            break;
        default:
            break;
        }
    }
}


static void CheckAnyCheck( gui_window *gui, a_dialog_header *dlg )
/****************************************************************/
/* If any of the sub-dialog check boxes are on, turn on the */
/* 'anycheck' variable */
{
    int                 i;
    vhandle             var_handle;

    for( i = 0; (var_handle = dlg->pVariables[i]) != NO_VAR; i++ ) {
        if( ControlClass( VH2ID( var_handle ), dlg ) == GUI_CHECK_BOX && VarGetBoolVal( var_handle ) ) {
            SetBoolVariableByHandle( dlg->any_check, true );
            GUISetChecked( gui, VH2ID( dlg->any_check ), GUI_CHECKED );
            return;
        }
    }
    SetBoolVariableByHandle( dlg->any_check, false );
    GUISetChecked( gui, VH2ID( dlg->any_check ), GUI_NOT_CHECKED );
}


static void CheckChildChecks( a_dialog_header *dlg )
/*********************************************************/
/* If all child checks are off, set them to the defaults */
{
    int                 i;
    vhandle             var_handle;

    for( i = 0; (var_handle = dlg->pVariables[i]) != NO_VAR; i++ ) {
        if( ControlClass( VH2ID( var_handle ), dlg ) == GUI_CHECK_BOX && VarGetBoolVal( var_handle ) ) {
            return;
        }
    }
    for( i = 0; (var_handle = dlg->pVariables[i]) != NO_VAR; i++ ) {
        if( ControlClass( VH2ID( var_handle ), dlg ) == GUI_CHECK_BOX ) {
            SetDefaultAutoSetValue( var_handle );
        }
    }
}


dlg_state IdToDlgState( gui_ctl_id id )
/*************************************/
{
    switch( id ) {
    case CTL_OK:
        return( DLG_NEXT );
    case CTL_PREVIOUS:
        return( DLG_PREV );
    case CTL_FIRST:
        return( DLG_START );
    case CTL_SKIP:
        return( DLG_SKIP );
    case CTL_DONE:
        return( DLG_DONE );
    }
    return( DLG_CANCEL );
}

static void UpdateControlVisibility( gui_window *gui, a_dialog_header *dlg, bool init )
/*********************************************************************************************/
// Hide controls which have the optional visibility condition evaluate to false, and show
// the ones that have it evaluate to true. (Controls with no vis. condition are always shown.)
// When a control is hidden, the controls BELOW it move up by the hidden control's height.
// A currently hidden controls that needs to be made visible causes all controls BELOW it
// to move down by the "appearing" control's height so that it can be shown again.
// The dialog window height also gets affected, while the top edge remains fixed on the screen.
//
// NOTE: be careful with RADIO_BUTTONS... making radio buttons appear and disappear because of
// other radio buttons in the same dialog being checked may not work (nor does it make sense anyways)
{
    gui_ord             last_height;
    gui_rect            rect;
    gui_rect            control_rect;
    gui_rect            parent_rect;
    gui_window          *parent;
    int                 i, j, sign;
    gui_ctl_id          id_i;
    gui_ctl_id          id_j;
    gui_ctl_id          checked_radio_id;
    gui_ctl_id          new_check_candidate;
    gui_ctl_id          focus_id;
    gui_ctl_id          new_focus_id;
    bool                enabled;
    bool                *control_on_new_line;
    bool                visible_checked_radiobutton;
    vhandle             var_handle;
    gui_control_class   control_class;

    if( gui == NULL )
        return;
    if( init ) {
        // How much the height of the dialog has changed from its original value
        dlg->height_change = 0;

        // The GUIGetRect below gets the dialog's position so that when it is resized,
        // it can be put back in its original position

        // NOTE that GUIGetRect does not want to give the EXACT location of the dialog,
        // so if you set the position to the coordinates that you got with GUIGetRect,
        // the dialog will MOVE slightly (not pleasing to the eye).
        // The current hack is to get the original coordinates, and go back to them every time
        // the dialog is resized with GUIResizeWindow.
        // This way, the dialog will be off by the same amount from the original
        // every time, and not appear to move.
        GUIGetRect( gui, &dlg->original_rect );

        // Make child windows appear in the correct place, since GUIGetRect() gives back
        // a rect that is tranlsated off by the parent's x and y.
        parent = GUIGetParentWindow( gui );
        if( parent != MainWnd && parent != NULL ) {
            GUIGetRect( parent, &parent_rect );
            dlg->original_rect.x -= parent_rect.x;
            dlg->original_rect.y -= parent_rect.y;
        }

        // Return if there are radio buttons
        // because UpdateControlVisibility is going to be called again anyways after init.
        // Without returning, the visibility conditions having to do with radio buttons
        // would not work sometimes.
        // We do not want to return if there are no radio buttons
        // because UpdateControlVisibility will not be called before showing
        // the dialog in that case.
        for( i = 0; i < dlg->num_controls; i++ ) {
            if( dlg->controls[i].control_class == GUI_RADIO_BUTTON ) {
                return;
            }
        }
    }
    memcpy( &rect, &dlg->original_rect, sizeof( gui_rect ) );
    last_height = rect.height + dlg->height_change;

    GUIGetFocus( gui, &focus_id );

    control_on_new_line = GUIMemAlloc( sizeof( bool ) * dlg->num_controls );

    // Figure out which controls are on a separate line from the last control
    control_on_new_line[0] = true;
    for( i = 1; i < dlg->num_controls; i++ ) {
        control_on_new_line[i] = ( dlg->controls[i].rect.y > dlg->controls[i - 1].rect.y );
    }

    // Initialize Variables corresponding to radio buttons and check boxes
    // This needs to be done for the part further below that checks to see if any
    // buttons are checked to work.
    // Also, figure out which radio button is currently checked.

    checked_radio_id = 0;
    for( i = 0; i < dlg->num_controls; i++ ) {
        control_class = dlg->controls[i].control_class;
        if( control_class == GUI_RADIO_BUTTON || control_class == GUI_CHECK_BOX ) {
            id_i = dlg->controls[i].id;
            if( GUIIsChecked( gui, id_i ) == GUI_CHECKED ) {
                vhandle var_handle_i = GetVariableById( id_i );
                if( control_class == GUI_RADIO_BUTTON ) {
                    checked_radio_id = id_i;
                }
                for( j = 0; (var_handle = dlg->pVariables[j]) != NO_VAR; j++ ) {
                    if( var_handle_i == var_handle ) {
                        SetBoolVariableByHandle( var_handle, true );
                    }
                }
            }
        }
    }

    // Allow EvalCondition to evaluate conditions with no 'shortcuts'
    // (See GetOptionVarValue() in setupinf.c)
    // Kind of like an on and off (below) switch
    // for special behaviour of GetOptionVarValue()
    // SetBoolVariableByName( "_Visibility_Condition_", true );
    VisibilityCondition = true;

    for( i = 0; i < dlg->num_controls; i++ ) {
        // Figure out which controls to hide and which to show.
        // Move all the controls below any control in transition
        // either up or down.

        if( dlg->controls_ext[i].pVisibilityConds != NULL ) {
            id_i = dlg->controls[i].id;
            enabled = GUIIsControlEnabled( gui, id_i );
            if( EvalCondition( dlg->controls_ext[i].pVisibilityConds ) ) {
                if( !enabled ) {
                    GUIEnableControl( gui, id_i, true );
                    sign = 1;
                } else {
                    continue;
                }
            } else if( enabled ) {
                GUIEnableControl( gui, id_i, false );
                GUIHideControl( gui, id_i );
                sign = -1;
            } else {
                continue;
            }
            if( control_on_new_line[i] ) {
                gui_ord height = dlg->controls[i].rect.height * sign;
                for( j = i + 1; j < dlg->num_controls; j++ ) {
                    if( control_on_new_line[j] ) {
                        break;
                    }
                }
                for( ; j < dlg->num_controls; j++ ) {
                    id_j = dlg->controls[j].id;
                    enabled = GUIIsControlEnabled( gui, id_j );
                    GUIGetControlRect( gui, dlg->controls[j].id, &control_rect );
                    control_rect.y += height;
                    if( enabled ) {
                        GUIHideControl( gui, id_j );
                        // control will be made visible again below
                        // after being moved.
                    }
                    GUIResizeControl( gui, id_j, &control_rect );
                }
                dlg->height_change += height;
            }
        }
    }

    GUIMemFree( control_on_new_line );

    // SetBoolVariableByName( "_Visibility_Condition_", false );
    VisibilityCondition = false;

    visible_checked_radiobutton = false;
    new_check_candidate = CTL_NULL;
    for( i = 0; i < dlg->num_controls; i++ ) {
        id_i = dlg->controls[i].id;

        // figure out if there are no enabled radio buttons that are checked
        // use this info further down...
        if( dlg->controls[i].control_class == GUI_RADIO_BUTTON &&
            !visible_checked_radiobutton && GUIIsControlEnabled( gui, id_i ) ) {
            if( GUIIsChecked( gui, id_i ) == GUI_CHECKED ) {
                visible_checked_radiobutton = true;
            } else if( new_check_candidate == CTL_NULL ) {
                new_check_candidate = id_i;
            }
        }

        // show enabled controls that were hidden for moving only
        if( GUIIsControlEnabled( gui, id_i ) && !GUIIsControlVisible( gui, id_i ) ) {
            GUIShowControl( gui, id_i );
        }
    }

    // Keep the original focus in case it has changed.
    GUIGetFocus( gui, &new_focus_id );
    if( new_focus_id != focus_id && GUIIsControlEnabled( gui, focus_id ) ) {
        GUISetFocus( gui, focus_id );
    }

    if( GUIIsControlEnabled( gui, checked_radio_id ) ) {
        GUISetChecked( gui, checked_radio_id, GUI_CHECKED );
    } else if( !visible_checked_radiobutton && new_check_candidate != CTL_NULL ) {
        // 'Check' a visible radio button if the currently checked button
        // is invisible.
        GUISetChecked( gui, new_check_candidate, GUI_CHECKED );
    }

    rect.height += dlg->height_change;

    if( rect.height != last_height ) {
        GUIResizeWindow( gui, &rect );
    }
}

static bool GenericGUIEventProc( gui_window *gui, gui_event gui_ev, void *param )
/*******************************************************************************/
{
#if defined( GUI_IS_GUI )
    static bool         first_time = true;
#endif
    gui_ctl_id          id;
    DLG_WINDOW_SET      *result;
    a_dialog_header     *dlg;
    static bool         initializing = false;
    char                buff[MAXBUF];
    gui_text_metrics    metrics;

    if( gui == NULL )
        return( false );
    result = GUIGetExtra( gui );
    dlg = result->dlg;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        initializing = true;
        UpdateControlVisibility( gui, dlg, true );
        SetDefaultVals( gui, dlg );
        SetFocusCtrl( gui, dlg );
        GUIGetTextMetrics( gui, &metrics );
#if defined( __OS2__ )
        {
            int         i;
            for( i = 0; i < dlg->num_controls; i++ ) {
                if( dlg->controls[i].control_class == GUI_EDIT_MLE ) {
                    id = dlg->controls[i].id;
                    GUILimitEditText( gui, id, -1 );
                    GUISetText( gui, id, dlg->controls[i].text );
                }
            }
        }
#endif
#if !defined( GUI_IS_GUI )
        if( stricmp( dlg->name, "Welcome" ) == 0 ) {
            if( GetVariableBoolVal( "AutoOptionsDialog" ) ) {
                // call Options dialog
                DoDialogWithParent( gui, "Options" );
            }
        }
#endif
#if defined( GUI_IS_GUI )
        first_time = true;
#endif
        initializing = false;
        return( true );
#if defined( GUI_IS_GUI )
    case GUI_PAINT:
        if( first_time ) {
            first_time = false;
            if( stricmp( dlg->name, "Welcome" ) == 0 ) {
                if( GetVariableBoolVal( "AutoOptionsDialog" ) ) {
                    // call Options dialog
                    DoDialogWithParent( gui, "Options" );
                }
            }
        }
        break;
#endif
    case GUI_DESTROY:
        return( true );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_OK:
        case CTL_PREVIOUS:
        case CTL_FIRST:
        case CTL_SKIP:
            GetVariableVals( gui, dlg, true );
            GUICloseDialog( gui );
            result->state = IdToDlgState( id );
            return( true );
        case CTL_CANCEL:
            GUICloseDialog( gui );
            result->state = DLG_CANCEL;
            return( true );
        case CTL_DONE:
            GUICloseDialog( gui );
            result->state = DLG_DONE;
            return( true );
        case CTL_OPTIONS:  // Options button on Welcome dialog
            // call Options dialog
            DoDialogWithParent( gui, "Options" );
            GetVariableVals( gui, dlg, false );
            return( true );
        default:
            {
                const char      *name;
                a_dialog_header *dlg_child;
                bool            old_val;
                dlg_state       return_state;

                name = VarGetStrVal( GetVariableById( id ) );
                if( name != NULL ) {
                    dlg_child = FindDialogByName( name );
                    if( dlg_child != NULL ) {
                        GetVariableVals( gui, dlg, false );
                        if( dlg_child->any_check != NO_VAR ) {
                            old_val = VarGetBoolVal( dlg_child->any_check );
                            SetBoolVariableByHandle( dlg_child->any_check, true );
                            GUISetChecked( gui, VH2ID( dlg_child->any_check ), GUI_CHECKED );
                            CheckChildChecks( dlg_child );
                            return_state = DoDialogByPointer( gui, dlg_child );
                            if( return_state != DLG_CANCEL &&
                                return_state != DLG_DONE ) {
                                CheckAnyCheck( gui, dlg_child );
                            } else {
                                SetBoolVariableByHandle( dlg_child->any_check, old_val );
                                GUISetChecked( gui, VH2ID( dlg_child->any_check ), old_val ? GUI_CHECKED : GUI_NOT_CHECKED );
                            }
                        } else {
                            DoDialogByPointer( gui, dlg_child );
                        }
                    }
                }
                if( !initializing )
                    GetVariableVals( gui, dlg, false );

                UpdateControlVisibility( gui, dlg, false );
                return( true );
            }
        case CTL_HELP:
            strcpy( buff, HELP_PREFIX );
            strncpy( buff + sizeof( HELP_PREFIX ) - 1, dlg->name, MAXBUF - sizeof( HELP_PREFIX ) );
            buff[MAXBUF - 1] = '\0';
            DoDialogWithParent( gui, buff );
            return( true );
        }
        break;
    default:
        break;
    }
    return( false );
}


static void AdjustDialogControls( a_dialog_header *dlg )
/******************************************************/
{
    int                 i,j;
    int                 num_push_buttons, curr_button;
    gui_control_info    *control, *next, *prev;
    int                 width;
    int                 but_pos;
    int                 max_width;

    if( dlg->def_dlg )
        return;

    if( dlg->adjusted )
        return;

    dlg->adjusted = true;
    width = dlg->cols;

    for( i = 0; i < dlg->num_controls; i++ ) {
        control = &dlg->controls[i];
        switch( control->control_class ) {
        case GUI_RADIO_BUTTON:
#if defined( GUI_IS_GUI )
            /* Align left edge of control with left of leftmost Button */
            but_pos = WIN_BUTTON_POS( 1, MAX_CTRL_PER_LINE, width, WIN_BW );
            control->rect.x = but_pos;
#endif
            control->rect.width = width - C0 - 1;
            break;

        case GUI_CHECK_BOX:
#if defined( GUI_IS_GUI )
            /* Align left edge of control with left of leftmost Button */
            but_pos = WIN_BUTTON_POS( 1, MAX_CTRL_PER_LINE, width, WIN_BW );
            control->rect.x = but_pos;
#endif
            // look for another control on this row
            control->rect.width = width - C0 - 1;
            if( i + 1 >= dlg->num_controls )
                break;

            next = control + 1;
            if( next->rect.y != control->rect.y )
                break;

            control->rect.width = ( width / 2 ) - C0 - 1;
            if( next->control_class != GUI_CHECK_BOX )
                break;

            num_push_buttons = 1;
            for( j = i + 1; j < dlg->num_controls; ++j ) {
                next = dlg->controls + j;
                if( next->control_class != GUI_CHECK_BOX || next->rect.y != control->rect.y )
                    break;
                ++num_push_buttons;
            }
            curr_button = 0;
            while( i < j ) {
                control->rect.width = ( width / num_push_buttons ) - C0 - 1;
#if defined( __NT__ ) && defined( GUI_IS_GUI )
                but_pos = WIN_BUTTON_POS( curr_button + 1, NORMAL_CHECKMARKS, width, WIN_BW );
#else
                but_pos = C0 + ( width / num_push_buttons ) * curr_button;
#endif
                control->rect.x = but_pos;
                ++curr_button;
                ++i;
                ++control;
            }
            --i;
            break;

        case GUI_PUSH_BUTTON:
        case GUI_DEFPUSH_BUTTON:
            num_push_buttons = 1;
            for( j = i + 1; j < dlg->num_controls; ++j ) {
                next = dlg->controls + j;
                if( ( next->control_class != GUI_PUSH_BUTTON &&
                      next->control_class != GUI_DEFPUSH_BUTTON ) ||
                    next->rect.y != control->rect.y ) {
                    break;
                }
                ++num_push_buttons;
                ++i;
            }
            if( j == dlg->num_controls ) {
                for( curr_button = 1; curr_button <= num_push_buttons; ++curr_button ) {
                    max_width = strlen( control->text ) + BUTTON_EXTRA;
#if defined( GUI_IS_GUI )
                    if( max_width < WIN_BW ) {
                        max_width = WIN_BW;
                    }
                    but_pos = WIN_BUTTON_POS( curr_button, num_push_buttons, width, WIN_BW );
#else
                    if( max_width < BW ) {
                        max_width = BW;
                    }
                    but_pos = BUTTON_POS( curr_button, num_push_buttons, width, BW );
#endif
                    control->rect.x = but_pos;
                    control->rect.width = max_width;
                    ++control;
                }
            } else {
                max_width = strlen( control->text ) + BUTTON_EXTRA;
#if defined( GUI_IS_GUI )
                if( max_width < WIN_BW ) {
                    max_width = WIN_BW;
                }
                but_pos = WIN_BUTTON_POS( NORMAL_BUTTONS, NORMAL_BUTTONS, width, WIN_BW );
                /* The dynamic system does not handle buttons too wide for dialog. */
                /* control->rect.width = WIN_BW; */
#else
                if( max_width < BW ) {
                    max_width = BW;
                }
                but_pos = BUTTON_POS( NORMAL_BUTTONS, MAX_CTRL_PER_LINE, width, BW );
#endif
                control->rect.x = but_pos;
                control->rect.width = max_width;
            }
            break;

        case GUI_EDIT:
            if( i > 0 ) {
                prev = control - 1;
                if( prev->control_class == GUI_PUSH_BUTTON && prev->rect.y == control->rect.y ) {
                    // dialog_edit_button  (edit control and push button together)
                    control->rect.width = prev->rect.x - control->rect.x - BUTTON_EXTRA;
                    break;
                }
            }
            if( control->text != NULL ) {
                control->rect.width = width - strlen( control->text ) - BUTTON_EXTRA;
            } else {
                control->rect.width = width - C0 - 5;
            }
            break;

        case GUI_STATIC:
            if( i > 0 ) {
                prev = control - 1;
                if( prev->control_class == GUI_PUSH_BUTTON || prev->control_class == GUI_EDIT ) {
                    if( prev->rect.y == control->rect.y ) {
                        if( control->rect.width > width ) {
                            control->rect.width = width;
                        }
                        break;
                    }
                }
            }
            if( control->id == GUI_NO_ID || dlg->controls_ext[i].pVisibilityConds != NULL ) {
                if( control->rect.width > width ) {
                    control->rect.width = width;
                }
            } else {
#if defined( GUI_IS_GUI )
                /* Align left edge of control with left of leftmost Button */
                but_pos = WIN_BUTTON_POS( 1, MAX_CTRL_PER_LINE, width, WIN_BW );
                control->rect.x = but_pos;
#endif
                control->rect.width = width - 1;
            }
            break;

        default:
            break;
        }
    }
}


dlg_state GenericDialog( gui_window *parent, a_dialog_header *dlg )
/*****************************************************************/
{
    VBUF                title;
    DLG_WINDOW_SET      result;
    int                 width;
    int                 height;

    if( dlg == NULL ) {
        return( DLG_CANCEL );
    }
    AdjustDialogControls( dlg );
    result.state = DLG_CANCEL;
    result.dlg = dlg;
    VbufInit( &title );
    if( dlg->title != NULL ) {
        VbufConcStr( &title, dlg->title );
    } else {
        ReplaceVars( &title, GetVariableStrVal( "AppName" ) );
    }
    width = dlg->cols;
    height = dlg->rows;
#if defined( __OS2__ ) && defined( GUI_IS_GUI )
    height -= 1;
#endif
    if( width < VbufLen( &title ) + WIDTH_BORDER + 2 ) {
        width = VbufLen( &title ) + WIDTH_BORDER + 2;
    }

    GUIRefresh();
    GUIModalDlgOpen( parent == NULL ? MainWnd : parent, VbufString( &title ), height, width,
                     dlg->controls, dlg->num_controls,
                     &GenericGUIEventProc, &result );
    ResetDriveInfo();
    VbufFree( &title );
    return( result.state );
}
