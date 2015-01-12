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
#include "gui.h"
#include "guistr.h"
#include "guidlg.h"
#include "dlgbutn.h"
#include "setup.h"
#include "setupinf.h"
#include "dlggen.h"
#include "genvbl.h"
#include "gendlg.h"
#include "utils.h"
#include "ctype.h"
#if !defined( __WATCOMC__ )
#include <clibext.h>
#endif


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


extern vhandle          FullInstall;
extern vhandle          SelectiveInstall;

typedef struct dlg_window_set {
    dlg_state           state;
    a_dialog_header     *current_dialog;     /* stuff needed in future */
} DLG_WINDOW_SET;

dlg_state       GenericDialog( gui_window *parent, a_dialog_header * );
extern void     SetDefaultAutoSetValue( vhandle var_handle );
extern void     ResetDriveInfo();

int VisibilityCondition = 0;

static gui_control_class ControlClass( int id, a_dialog_header *curr_dialog )
/***************************************************************************/
/* return the control class of a variable based on its id. */
{
    int                 i;

    for( i = 0; i < curr_dialog->num_controls; i++ ) {
        if( curr_dialog->controls[i].id == id ) {
            return( curr_dialog->controls[i].control_class );
        }
    }
    SetupError( "IDS_CONTROLCLASSERROR" );
    return( GUI_BAD_CLASS );
}


static void SetDynamic( gui_window *gui, vhandle var_handle, bool *drive_checked )
/*******************************************************************************/
{
    char        buff[256];
    const char  *p;

    p = VarGetStrVal( var_handle );
    if( !*drive_checked ) {
        while( *p ) {
            if( *p == '%' ) {
                if( strnicmp( p, "%DriveFree", 10 ) == 0 ) {
                    CheckDrive( false );
                    *drive_checked = true;
                }
            }
            ++p;
        }
    }
    ReplaceVars( buff, sizeof( buff ), VarGetStrVal( var_handle ) );
    AddInstallName( buff, false );
    GUISetText( gui, VarGetId( var_handle ), buff );
}


static void SetDefaultVals( gui_window *gui, a_dialog_header *curr_dialog )
/*************************************************************************/
/* Set the default variable values. Decide how to set these  */
/* default values based on edit control type. */
{
    int                 i;
    gui_control_class   a_control_class;
    vhandle             var_handle;
    char                *cond;
    bool                drive_checked;

    drive_checked = false;
    for( i = 0; curr_dialog->pVariables[i] != NO_VAR; ++i ) {
        var_handle = curr_dialog->pVariables[i];
        cond = curr_dialog->pConditions[i];
        if( !curr_dialog->defaults_set && cond != NULL && VarGetIntVal( var_handle ) == 0 ) {
            if( isdigit( *cond ) ) {
                SetVariableByHandle( var_handle, cond );
            } else if( EvalCondition( cond ) ) {
                SetVariableByHandle( var_handle, "1" );
            }
        }
        a_control_class = ControlClass( VarGetId( var_handle ), curr_dialog );
        switch( a_control_class ) {
        case GUI_STATIC:
            SetDynamic( gui, var_handle, &drive_checked );
            break;
        case GUI_RADIO_BUTTON:
        case GUI_CHECK_BOX:
            GUISetChecked( gui, VarGetId( var_handle ), VarGetIntVal( var_handle ) ? GUI_CHECKED : GUI_NOT_CHECKED );
            break;
        case GUI_EDIT_MLE:
        case GUI_EDIT:
            GUISetText( gui, VarGetId( var_handle ), VarGetStrVal( var_handle ) );
            break;
        default:
            break;
        }
    }
    curr_dialog->defaults_set = true;
}

static void SetFocusCtrl( gui_window *gui, a_dialog_header *curr_dialog )
/*************************************************************************/
/* Set the default variable values. Decide how to set these  */
/* default values based on edit control type. */
{
    int                 i, j;

    for( i = 0; i < curr_dialog->num_controls; i++ ) {
        switch( curr_dialog->controls[i].control_class ) {
        case GUI_EDIT_MLE:
        case GUI_EDIT:
            GUISetFocus( gui, curr_dialog->controls[i].id );
            return;
        case GUI_RADIO_BUTTON:
        case GUI_CHECK_BOX:
            for( j = i;; ++j ) {
                if( curr_dialog->controls[j].control_class != GUI_RADIO_BUTTON &&
                    curr_dialog->controls[j].control_class != GUI_CHECK_BOX ) {
                    break;
                }
                if( GUIIsChecked( gui, curr_dialog->controls[j].id ) == GUI_CHECKED ) {
                    GUISetFocus( gui, curr_dialog->controls[j].id );
                    return;
                }
            }
            GUISetFocus( gui, curr_dialog->controls[i].id );
            return;
        default:
            break;
        }
    }
    for( i = 0; i < curr_dialog->num_controls; i++ ) {
        if( curr_dialog->controls[i].control_class == GUI_DEFPUSH_BUTTON ) {
            GUISetFocus( gui, curr_dialog->controls[i].id );
            return;
        }
    }
}


static void GetVariableVals( gui_window *gui,
                             a_dialog_header *curr_dialog, bool closing )
/***********************************************************************/
/* Get the input variable values. Decide how to set these */
/* default values based on edit control type. */
{
    char                *text;
    int                 i;
    gui_control_class   a_control_class;
    vhandle             *pVariable;
    vhandle             var_handle;
    bool                drive_checked;

    drive_checked = false;
    pVariable = curr_dialog->pVariables;
    for( i = 0; pVariable[i] != NO_VAR; i++ ) {
        var_handle = pVariable[i];
        a_control_class = ControlClass( VarGetId( var_handle ), curr_dialog );
        switch( a_control_class ) {
        case GUI_STATIC:
            if( !closing ) {
                SetDynamic( gui, var_handle, &drive_checked );
            }
            break;
        case GUI_RADIO_BUTTON:
        case GUI_CHECK_BOX:
            if( GUIIsChecked( gui, VarGetId( var_handle ) ) == GUI_CHECKED ) {
                if( VarIsRestrictedFalse( var_handle ) ) {
                    if( !closing ) {
                        MsgBox( gui, "IDS_NODISKFOROPTION", GUI_OK );
                        GUISetChecked( gui, VarGetId( var_handle ), GUI_NOT_CHECKED );
                        if( var_handle == FullInstall ) {
                            GUISetChecked( gui, VarGetId( SelectiveInstall ), GUI_CHECKED );
                        }
                    }
                    SetVariableByHandle( var_handle, "0" );
                    if( var_handle == FullInstall ) {
                        SetVariableByHandle( SelectiveInstall, "1" );
                    }
                } else {
                    SetVariableByHandle( var_handle, "1" );
                }
            } else {
                SetVariableByHandle( var_handle, "0" );
            }
            break;
        case GUI_EDIT:
            text = GUIGetText( gui, VarGetId( var_handle ) );
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


static void CheckAnyCheck( gui_window *gui, a_dialog_header *child )
/******************************************************************/
/* If any of the sub-dialog check boxes are on, turn on the */
/* 'anycheck' variable */
{
    int                 i;

    for( i = 0; child->pVariables[i] != NO_VAR; i++ ) {
        if( ControlClass( VarGetId( child->pVariables[i] ), child ) == GUI_CHECK_BOX &&
            VarGetIntVal( child->pVariables[i] ) ) {
            SetVariableByHandle( child->any_check, "1" );
            GUISetChecked( gui, VarGetId( child->any_check ), GUI_CHECKED );
            return;
        }
    }
    SetVariableByHandle( child->any_check, "0" );
    GUISetChecked( gui, VarGetId( child->any_check ), GUI_NOT_CHECKED );
}


static void CheckChildChecks( a_dialog_header *child )
/******************************************************************/
/* If all child checks are off, set them to the defaults */
{
    int                 i;

    for( i = 0; child->pVariables[i] != NO_VAR; i++ ) {
        if( ControlClass( VarGetId( child->pVariables[i] ), child ) == GUI_CHECK_BOX &&
            VarGetIntVal( child->pVariables[i] ) ) {
            return;
        }
    }
    for( i = 0; child->pVariables[i] != NO_VAR; i++ ) {
        if( ControlClass( VarGetId( child->pVariables[i] ), child ) == GUI_CHECK_BOX ) {
            SetDefaultAutoSetValue( child->pVariables[i] );
        }
    }
}


static void DoBeep( void )
/************************/
{
#if defined( __WINDOWS__ ) || defined( __NT__ )
    MessageBeep( 0 );
#elif defined( __OS2__ )
    DosBeep( 750, 250 );
#else
    putchar( 7 );
    fflush( stdout );
#endif
}

dlg_state IdToDlgState( int id )
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
    return( DLG_CAN );
}

static void UpdateControlVisibility( gui_window *gui, a_dialog_header *curr_dialog, bool init )
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
    unsigned            id_i;
    unsigned            id_j;
    unsigned            checked_radio_id = 0;
    unsigned            new_check_candidate;
    unsigned            focus;
    unsigned            new_focus;
    bool                enabled;
    bool                control_on_new_line[MAX_VARS];
    bool                visible_checked_radiobutton;
    vhandle             var_handle;
//    vhandle             *pVariable;

    if( gui == NULL )
        return;
    if( init ) {
        // How much the height of the dialog has changed from its original value
        curr_dialog->height_change = 0;

        // The GUIGetRect below gets the dialog's position so that when it is resized,
        // it can be put back in its original position

        // NOTE that GUIGetRect does not want to give the EXACT location of the dialog,
        // so if you set the position to the coordinates that you got with GUIGetRect,
        // the dialog will MOVE slightly (not pleasing to the eye).
        // The current hack is to get the original coordinates, and go back to them every time
        // the dialog is resized with GUIResizeWindow.
        // This way, the dialog will be off by the same amount from the original
        // every time, and not appear to move.
        GUIGetRect( gui, &curr_dialog->original_rect );

        // Make child windows appear in the correct place, since GUIGetRect() gives back
        // a rect that is tranlsated off by the parent's x and y.
        parent = GUIGetParentWindow( gui );
        if( parent != MainWnd && parent != NULL ) {
            GUIGetRect( parent, &parent_rect );
            curr_dialog->original_rect.x -= parent_rect.x;
            curr_dialog->original_rect.y -= parent_rect.y;
        }

        // Return if there are radio buttons
        // because UpdateControlVisibility is going to be called again anyways after init.
        // Without returning, the visibility conditions having to do with radio buttons
        // would not work sometimes.
        // We do not want to return if there are no radio buttons
        // because UpdateControlVisibility will not be called before showing
        // the dialog in that case.
        for( i = 0; i < curr_dialog->num_controls; i++ ) {
            if( curr_dialog->controls[i].control_class == GUI_RADIO_BUTTON ) {
                return;
            }
        }
    }
    memcpy( &rect, &curr_dialog->original_rect, sizeof( gui_rect ) );
    last_height = rect.height + curr_dialog->height_change;

    control_on_new_line[0] = true;

    GUIGetFocus( gui, &focus );

    // Figure out which controls are on a separate line from the last control
    for( i = 1; i < curr_dialog->num_controls; i++ ) {
        if( curr_dialog->controls[i].rect.y >
            curr_dialog->controls[i - 1].rect.y ) {
            control_on_new_line[i] = true;
        } else {
            control_on_new_line[i] = false;
        }
    }

    // Initialize Variables corresponding to radio buttons and check boxes
    // This needs to be done for the part further below that checks to see if any
    // buttons are checked to work.
    // Also, figure out which radio button is currently checked.

    for( i = 0; i < curr_dialog->num_controls; i++ ) {
        if( curr_dialog->controls[i].control_class == GUI_RADIO_BUTTON ||
            curr_dialog->controls[i].control_class == GUI_CHECK_BOX ) {
//            pVariable = curr_dialog->pVariables;
            var_handle = curr_dialog->controls[i].id;
            if( curr_dialog->controls[i].control_class == GUI_RADIO_BUTTON &&
                GUIIsChecked( gui, VarGetId( var_handle ) ) == GUI_CHECKED ) {
                checked_radio_id = var_handle;
            }
            for( j = 0; curr_dialog->pVariables[j] != NO_VAR; j++ ) {
                if( curr_dialog->pVariables[j] == var_handle &&
                    GUIIsChecked( gui, VarGetId( var_handle ) ) == GUI_CHECKED ) {
                    SetVariableByHandle( var_handle, "1" );
                }
            }
        }
    }

    // Allow EvalCondition to evaluate conditions with no 'shortcuts'
    // (See GetOptionVarValue() in setupinf.c)
    // Kind of like an on and off (below) switch
    // for special behaviour of GetOptionVarValue()
    // SetVariableByName( "_Visibility_Condition_", "1" );
    VisibilityCondition = 1;

    for( i = 0; i < curr_dialog->num_controls; i++ ) {
    // Figure out which controls to hide and which to show.
    // Move all the controls below any control in transition
    // either up or down.

        if( curr_dialog->pVisibilityConds[i] != NULL ) {
            id_i = curr_dialog->controls[i].id;
            enabled = GUIIsControlEnabled( gui, id_i );
            if( EvalCondition( curr_dialog->pVisibilityConds[i] ) ) {
                if( !enabled ) {
                    GUIEnableControl( gui, id_i, true );
                    sign = 1;
                } else {
                    continue;
                }
            } else if( enabled ) {
                GUIEnableControl( gui, id_i, false );
                GUIHideControl( gui, curr_dialog->controls[i].id );
                sign = -1;
            } else {
                continue;
            }
            if( control_on_new_line[i] ) {
                for( j = i + 1; j < curr_dialog->num_controls &&
                                !control_on_new_line[j]; j++ );
                for( ; j < curr_dialog->num_controls; j++ ) {
                    id_j = curr_dialog->controls[j].id;
                    enabled = GUIIsControlEnabled( gui, id_j );
                    GUIGetControlRect( gui, curr_dialog->controls[j].id, &control_rect );
                    control_rect.y += curr_dialog->controls[i].rect.height * sign;
                    if( enabled ) {
                        GUIHideControl( gui, id_j );
                        // control will be made visible again below
                        // after being moved.
                    }
                    GUIResizeControl( gui, id_j, &control_rect );
                }
                curr_dialog->height_change += curr_dialog->controls[i].rect.height * sign;
            }
        }
    }

    // SetVariableByName( "_Visibility_Condition_", "0" );
    VisibilityCondition = 0;

    visible_checked_radiobutton = false;
    new_check_candidate = 0;

    for( i = 0; i < curr_dialog->num_controls; i++ ) {
        id_i = curr_dialog->controls[i].id;

        // figure out if there are no enabled radio buttons that are checked
        // use this info further down...
        if( curr_dialog->controls[i].control_class == GUI_RADIO_BUTTON &&
            !visible_checked_radiobutton && GUIIsControlEnabled( gui, id_i ) ) {
            if( GUIIsChecked( gui, id_i ) == GUI_CHECKED ) {
                visible_checked_radiobutton = true;
            } else if( new_check_candidate == 0 ) {
                new_check_candidate = id_i;
            }
        }

        // show enabled controls that were hidden for moving only
        if( GUIIsControlEnabled( gui, id_i ) && !GUIIsControlVisible( gui, id_i ) ) {
            GUIShowControl( gui, id_i );
        }
    }

    // Keep the original focus in case it has changed.
    GUIGetFocus( gui, &new_focus );
    if( new_focus != focus && GUIIsControlEnabled( gui, focus ) ) {
        GUISetFocus( gui, focus );
    }

    if( GUIIsControlEnabled( gui, checked_radio_id ) ) {
        GUISetChecked( gui, checked_radio_id, GUI_CHECKED );
    } else if( !visible_checked_radiobutton && new_check_candidate != 0 ) {
        // 'Check' a visible radio button if the currently checked button
        // is invisible.
        GUISetChecked( gui, new_check_candidate, GUI_CHECKED );
    }

    rect.height += curr_dialog->height_change;

    if( rect.height != last_height ) {
        GUIResizeWindow( gui, &rect );
    }
}

static GUICALLBACK GenericEventProc;
static bool GenericEventProc( gui_window *gui, gui_event gui_ev, void *param )
/****************************************************************************/
{
#if !defined( _UI )
    static bool         first_time = true;
#endif
    unsigned            id;
    DLG_WINDOW_SET      *result;
    a_dialog_header     *curr_dialog;
    static bool         initializing = false;
    char                buff[MAXBUF];
    gui_text_metrics    metrics;

    if( gui == NULL )
        return( false );
    result = GUIGetExtra( gui );
    curr_dialog = result->current_dialog;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        initializing = true;
        UpdateControlVisibility( gui, curr_dialog, true );
        SetDefaultVals( gui, curr_dialog );
        SetFocusCtrl( gui, curr_dialog );
        GUIGetTextMetrics( gui, &metrics );
        if( stricmp( curr_dialog->name, "InsertDisk" ) == 0 ) {
            DoBeep();
        }
#if defined( __OS2__ )
        {
            int         i;
            for( i = 0; i < curr_dialog->num_controls; i++ ) {
                if( curr_dialog->controls[i].control_class == GUI_EDIT_MLE ) {
                    GUILimitEditText( gui, curr_dialog->controls[i].id, -1 );
                    GUISetText( gui, curr_dialog->controls[i].id,
                                curr_dialog->controls[i].text );
                }
            }
        }
#endif
#if defined( _UI )
        if( stricmp( curr_dialog->name, "Welcome" ) == 0 ) {
            if( GetVariableIntVal( "AutoOptionsDialog" ) == 1 ) {
                // call Options dialog
                DoDialogWithParent( gui, "Options" );
            }
        }
#endif
#if !defined( _UI )
        first_time = true;
#endif
        initializing = false;
        return( true );
#if !defined( _UI )
    case GUI_PAINT:
        if( first_time ) {
            first_time = false;
            if( stricmp( curr_dialog->name, "Welcome" ) == 0 ) {
                if( GetVariableIntVal( "AutoOptionsDialog" ) == 1 ) {
                    // call Options dialog
                    DoDialogWithParent( gui, "Options" );
                }
            }
        }
        break;
#endif
    case GUI_DESTROY:
        break;
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_OK:
        case CTL_PREVIOUS:
        case CTL_FIRST:
        case CTL_SKIP:
            GetVariableVals( gui, curr_dialog, true );
            GUICloseDialog( gui );
            result->state = IdToDlgState( id );
            break;
        case CTL_CANCEL:
            GUICloseDialog( gui );
            result->state = DLG_CAN;
            break;
        case CTL_DONE:
            GUICloseDialog( gui );
            result->state = DLG_DONE;
            break;
        case CTL_OPTIONS:  // Options button on Welcome dialog
            // call Options dialog
            DoDialogWithParent( gui, "Options" );
            GetVariableVals( gui, curr_dialog, false );
            break;
        default:
            {
                const char      *dlg_name;
                a_dialog_header *child;
                int             old_val;
                dlg_state       return_state;

                dlg_name = VarGetStrVal( GetVariableById( id ) );
                if( dlg_name != NULL ) {
                    child = FindDialogByName( dlg_name );
                    if( child != NULL ) {
                        GetVariableVals( gui, curr_dialog, false );
                        if( child->any_check != NO_VAR ) {
                            old_val = VarGetIntVal( child->any_check );
                            SetVariableByHandle( child->any_check, "1" );
                            GUISetChecked( gui, VarGetId( child->any_check ), GUI_CHECKED );
                            CheckChildChecks( child );
                            return_state = DoDialogByPointer( gui, child );
                            if( return_state != DLG_CAN &&
                                return_state != DLG_DONE ) {
                                CheckAnyCheck( gui, child );
                            } else {
                                SetVariableByHandle( child->any_check, old_val ? "1" : "0" );
                                GUISetChecked( gui, VarGetId( child->any_check ), old_val ? GUI_CHECKED : GUI_NOT_CHECKED );
                            }
                        } else {
                            DoDialogByPointer( gui, child );
                        }
                    }
                }
            }
            if( !initializing )
                GetVariableVals( gui, curr_dialog, false );

            UpdateControlVisibility( gui, curr_dialog, false );

            break;
        case CTL_HELP:
            strcpy( buff, "Help_" );
            strcat( buff, curr_dialog->name );
            DoDialogWithParent( gui, buff );
            break;
        }
        return( true );
    default:
        break;
    }
    return( false );
}


static void AdjustDialogControls( a_dialog_header *curr_dialog )
/**************************************************************/
{
    int                 i,j;
    int                 num_push_buttons, curr_button;
    gui_control_class   a_control_class;
    gui_control_info    *control, *next, *prev;
    int                 width;
    int                 but_pos;
    int                 max_width;

    if( curr_dialog->def_dlg )
        return;

    if( curr_dialog->adjusted )
        return;

    curr_dialog->adjusted = true;
    width = curr_dialog->cols;

    for( i = 0; i < curr_dialog->num_controls; i++ ) {
        control = &curr_dialog->controls[i];
        a_control_class = control->control_class;

        switch( a_control_class ) {

        case GUI_RADIO_BUTTON:
#if !defined( _UI )
            /* Align left edge of control with left of leftmost Button */
            but_pos = WIN_BUTTON_POS( 1, MAX_CTRL_PER_LINE, width, WIN_BW );
            control->rect.x = DLG_COL( but_pos );
#endif
            control->rect.width = DLG_COL( width - C0 - 1 );
            break;

        case GUI_CHECK_BOX:
#if !defined( _UI )
            /* Align left edge of control with left of leftmost Button */
            but_pos = WIN_BUTTON_POS( 1, MAX_CTRL_PER_LINE, width, WIN_BW );
            control->rect.x = DLG_COL( but_pos );
#endif
            // look for another control on this row
            control->rect.width = DLG_COL( width - C0 - 1 );
            if( i + 1 >= curr_dialog->num_controls )
                break;

            next = &curr_dialog->controls[i + 1];
            if( next->rect.y != control->rect.y )
                break;

            control->rect.width = DLG_COL( (width / 2) - C0 - 1 );
            if( next->control_class != GUI_CHECK_BOX )
                break;

            j = i;
            num_push_buttons = 1;
            while( ++j < curr_dialog->num_controls ) {
                next = &curr_dialog->controls[j];
                if( next->control_class != GUI_CHECK_BOX || next->rect.y != control->rect.y )
                    break;
                ++num_push_buttons;
            }
            curr_button = 0;
            while( i < j ) {
                control->rect.width = DLG_COL( (width / num_push_buttons) - C0 - 1 );
#if defined( __NT__ ) && !defined( _UI )
                but_pos = WIN_BUTTON_POS( curr_button + 1, NORMAL_CHECKMARKS, width, WIN_BW );
                control->rect.x = DLG_COL( but_pos );
#else
                control->rect.x = DLG_COL( C0 + (width / num_push_buttons) * curr_button );
#endif
                ++curr_button;
                ++i;
                ++control;
            }
            --i;
            break;

        case GUI_PUSH_BUTTON:
        case GUI_DEFPUSH_BUTTON:
            j = i;
            num_push_buttons = 1;
            while( ++j < curr_dialog->num_controls ) {
                next = &curr_dialog->controls[j];
                if( ( next->control_class != GUI_PUSH_BUTTON &&
                      next->control_class != GUI_DEFPUSH_BUTTON ) ||
                    next->rect.y != control->rect.y ) {
                    break;
                } else {
                    ++num_push_buttons;
                    ++i;
                }
            }
            if( j == curr_dialog->num_controls ) {
                for( curr_button = 1; curr_button <= num_push_buttons; ++curr_button ) {
                    max_width = strlen( control->text ) + BUTTON_EXTRA;
#if !defined( _UI )
                    if( max_width < WIN_BW ) {
                        max_width = WIN_BW;
                    }
                    but_pos = WIN_BUTTON_POS( curr_button, num_push_buttons, width, WIN_BW );
                    control->rect.x = DLG_COL( but_pos );
                    control->rect.width = DLG_COL( max_width );
#else
                    if( max_width < BW ) {
                        max_width = BW;
                    }
                    but_pos = BUTTON_POS( curr_button, num_push_buttons, width, BW );
                    control->rect.x = DLG_COL( but_pos );
                    control->rect.width = DLG_COL( max_width );
#endif
                    ++control;
                }
            } else {
                max_width = strlen( control->text ) + BUTTON_EXTRA;
#if !defined( _UI )
                if( max_width < WIN_BW ) {
                    max_width = WIN_BW;
                }
                but_pos = WIN_BUTTON_POS( NORMAL_BUTTONS, NORMAL_BUTTONS, width, WIN_BW );
                control->rect.x = DLG_COL( but_pos );
                /* The dynamic system does not handle buttons too wide for dialog. */
                control->rect.width = max_width;
                /* control->rect.width = DLG_COL( WIN_BW ); */
#else
                if( max_width < BW ) {
                    max_width = BW;
                }
                but_pos = BUTTON_POS( NORMAL_BUTTONS, MAX_CTRL_PER_LINE, width, BW );
                control->rect.x = DLG_COL( but_pos );
                control->rect.width = max_width;
#endif
            }
            break;

        case GUI_EDIT:
            if( i > 0 ) {
                prev = &curr_dialog->controls[i - 1];
                if( prev->control_class == GUI_PUSH_BUTTON &&
                    prev->rect.y == control->rect.y ) {
                    // dialog_edit_button  (edit control and push button together)
                    control->rect.width = DLG_COL( prev->rect.x - control->rect.x - BUTTON_EXTRA );
                    break;
                }
            }
            if( control->text != NULL ) {
                control->rect.width = DLG_COL( width - strlen( control->text ) - BUTTON_EXTRA );
            } else {
                control->rect.width = DLG_COL( width - C0 - 5 );
            }
            break;

        case GUI_STATIC:
            if( control->id != -1 && curr_dialog->pVisibilityConds[i] == NULL &&
                (i <= 0 || (curr_dialog->controls[i - 1].control_class != GUI_EDIT &&
                curr_dialog->controls[i - 1].control_class != GUI_PUSH_BUTTON) ||
                curr_dialog->controls[i - 1].rect.y != curr_dialog->controls[i].rect.y) ) {
#if !defined( _UI )
                /* Align left edge of control with left of leftmost Button */
                but_pos = WIN_BUTTON_POS( 1, MAX_CTRL_PER_LINE, width, WIN_BW );
                control->rect.x = DLG_COL( but_pos );
#endif
                control->rect.width = DLG_COL( width - 1 );
            } else {
                if( control->rect.width > DLG_COL( width ) ) {
                    control->rect.width = DLG_COL( width );
                }
            }
            break;

        default:
            break;
        }
    }
}


extern dlg_state GenericDialog( gui_window *parent, a_dialog_header *curr_dialog )
/********************************************************************************/
{
    char                *title;
    DLG_WINDOW_SET      result;
    int                 width;
    int                 height;
    char                buff[MAXBUF];

    if( curr_dialog == NULL ) {
        return( DLG_CAN );
    }
    AdjustDialogControls( curr_dialog );

    result.state = DLG_CAN;
    result.current_dialog = curr_dialog;
    if( curr_dialog->title != NULL ) {
        title = curr_dialog->title;
    } else {
        title = ReplaceVars( buff, sizeof( buff ), GetVariableStrVal( "AppName" ) );
    }
    width = curr_dialog->cols;
    height = curr_dialog->rows;
#if defined( __OS2__ ) && !defined( _UI )
    height -= 1;
#endif
    if( width < strlen( title ) + WIDTH_BORDER + 2 ) {
        width = strlen( title ) + WIDTH_BORDER + 2;
    }

    GUIRefresh();
    GUIModalDlgOpen( parent == NULL ? MainWnd : parent, title, height, width,
                     curr_dialog->controls, curr_dialog->num_controls,
                     &GenericEventProc, &result );
    ResetDriveInfo();
    return( result.state );
}

