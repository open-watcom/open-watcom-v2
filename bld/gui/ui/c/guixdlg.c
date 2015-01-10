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
* Description:  Dialog box low level routines.
*
****************************************************************************/


#include "guiwind.h"
#include "guicontr.h"
#include "guixmain.h"
#include "guixwind.h"
#include "guilistb.h"
#include "guixdlg.h"
#include "guicolor.h"
#include "guihotsp.h"
#include "guixedit.h"
#include "guiutil.h"
#include "guixutil.h"
#include "guistr.h"
#include "guikey.h"
#include <stdio.h>
#include <string.h>

#define GETID( ev ) ( ev - GUI_FIRST_USER_EVENT )

extern EVENT GUIUserEvents[];

static a_radio_group * RadioGroup = NULL;
static bool Group = false;

static EVENT DlgEvents[] = {
    EV_NO_EVENT,
    EV_ESCAPE,
    EV_NO_EVENT,
};

static EVENT ControlEvents[] = {
    EV_NO_EVENT,
    EV_LIST_BOX_CHANGED,
    EV_LIST_BOX_DCLICK,
    EV_LIST_BOX_CLOSED,
    EV_CHECK_BOX_CLICK,
    EV_CURSOR_UP,
    EV_CURSOR_DOWN,
    EV_NO_EVENT
};

static a_field_type ui_types[GUI_NUM_CONTROL_CLASSES] = {
    FLD_HOT,      /* GUI_PUSH_BUTTON    */
    FLD_HOT,      /* GUI_DEFPUSH_BUTTON */
    FLD_RADIO,    /* GUI_RADIO_BUTTON   */
    FLD_CHECK,    /* GUI_CHECK_BOX      */
    FLD_PULLDOWN, /* GUI_COMBOBOX       */
    FLD_EDIT,     /* GUI_EDIT           */
    FLD_LISTBOX,  /* GUI_LISTBOX        */
    FLD_VOID,     /* GUI_SCROLLBAR      */
    FLD_TEXT,     /* GUI_STATIC         */
    FLD_FRAME,    /* GUI_GROUPBOX       */
    FLD_COMBOBOX, /* GUI_EDIT_COMBOBOX  */
    FLD_EDIT_MLE  /* GUI_EDIT_MLE       */
};

/* functions to find dialog boxes */

static  dialog_node     *MyDialog       = NULL;

void GUISetJapanese( void )
{
}

static bool InsertDialog( gui_window *wnd, a_dialog *ui_dlg_info, int num_controls,
                          char *name, bool colours_set )
{
    dialog_node *curr;

    curr = (dialog_node *)GUIMemAlloc( sizeof( dialog_node ) );
    if( curr == NULL ) {
        return( false );
    }
    curr->wnd = wnd;
    curr->ui_dlg_info = ui_dlg_info;
    curr->next = MyDialog;
    curr->num_controls = num_controls;
    curr->name = name;
    curr->colours_set = colours_set;
    MyDialog = curr;
    return( true );
}

void GUIDeleteDialog( a_dialog *ui_dlg_info )
{
    dialog_node *curr;
    dialog_node *prev;

    prev = NULL;
    for( curr = MyDialog; curr != NULL; curr = curr->next ) {
        if( curr->ui_dlg_info == ui_dlg_info ) {
            if( prev != NULL ) {
                prev->next = curr->next;
            } else {
                MyDialog = curr->next;
            }
            GUIMemFree( curr );
            break;
        } else {
            prev = curr;
        }
    }
}

dialog_node *GUIGetDlgByWnd( gui_window *wnd )
{
    dialog_node *curr;

    for( curr = MyDialog; curr != NULL; curr = curr->next ) {
        if( curr->wnd == wnd ) {
            return( curr );
        }
    }
    return( NULL );
}

static dialog_node *GetDialog( a_dialog *ui_dlg_info )
{
    dialog_node *curr;

    for( curr = MyDialog; curr != NULL; curr = curr->next ) {
        if( curr->ui_dlg_info == ui_dlg_info ) {
            return( curr );
        }
    }
    return( NULL );
}

static bool GetIndexOfField( a_dialog *ui_dlg_info, VFIELD *field, int num_controls,
                             int *index )
{
    int i;

    for( i = 0; i < num_controls; i++ ) {
        if( &ui_dlg_info->fields[i] == field ) {
            *index = i;
            return( true );
        }
    }
    return( false );
}

unsigned GUIGetControlId( gui_window *wnd, VFIELD *field )
{
    gui_control *control;
    int         index;
    dialog_node *dlg_node;

    dlg_node = GUIGetDlgByWnd( wnd );
    if( dlg_node == NULL ) {
        return( 0 );
    }
    if( !GetIndexOfField( dlg_node->ui_dlg_info, field, dlg_node->num_controls, &index ) ) {
        return( 0 );
    }
    for( control = wnd->controls; control != NULL; control = control->sibling ) {
        if( control->index == index ) {
            return( control->id );
        }
    }
    return( 0 );
}

a_dialog *GUIGetDialog( gui_window *wnd )
{
    dialog_node *dlg_node;

    dlg_node = GUIGetDlgByWnd( wnd );
    if( dlg_node != NULL ) {
        return( dlg_node->ui_dlg_info );
    } else {
        return( NULL );
    }
}

VFIELD *GUIGetField( gui_window *wnd, unsigned id )
{
    a_dialog    *ui_dlg_info;
    dialog_node *dlg_node;
    gui_control *control;

    dlg_node = GUIGetDlgByWnd( wnd );
    if( dlg_node != NULL ) {
        ui_dlg_info = dlg_node->ui_dlg_info;
        control = GUIGetControl( wnd, id );
        if( control != NULL && ( control->index < dlg_node->num_controls ) ) {
            return( &ui_dlg_info->fields[control->index] );
        }
    }
    return( NULL );
}

/************************** end of dialog box functions *********************/

void GUIPushControlEvents( void )
{
    uipushlist( ControlEvents );
}

void GUIPopControlEvents( void )
{
    uipoplist( /* ControlEvents */ );
}

static bool ResetFieldSize( dialog_node *dlg_node, int new_num )
{
    VFIELD      *fields;

    fields = (VFIELD *)GUIMemRealloc( dlg_node->ui_dlg_info->fields,
                                   ( new_num + 1 ) * sizeof( VFIELD ) );
    if( fields != NULL ) {
        dlg_node->ui_dlg_info->fields = fields;
        memset( &fields[new_num], 0, sizeof( VFIELD ) );
        return( true );
    }
    return( false );
}

bool GUIInsertDialog( gui_window *wnd )
{
    a_dialog    *ui_dlg_info;
    dialog_node *dlg_node;

    if( GUIGetDlgByWnd( wnd ) != NULL ) {
        return( true );
    } else {
        ui_dlg_info = (a_dialog *)GUIMemAlloc( sizeof( a_dialog ) );
        if( ui_dlg_info == NULL ) {
            return( false );
        }
        memset( ui_dlg_info, 0, sizeof( a_dialog ) );
        ui_dlg_info->vs = &wnd->screen;
        if( InsertDialog( wnd, ui_dlg_info, 0, NULL, false ) ) {
            dlg_node = GUIGetDlgByWnd( wnd );
            if( dlg_node != NULL ) {
                if( ResetFieldSize( dlg_node, 0 ) ) {
                    uireinitdialog( ui_dlg_info, ui_dlg_info->fields );
                    return( true );
                }
            }
        }
        return( false );
    }
}

void GUIRefreshControl( gui_window *wnd, unsigned id )
{
    a_dialog    *ui_dlg_info;
    bool        colours_set;
    VFIELD      *field;

    field = GUIGetField( wnd, id );
    if( field != NULL ) {
        ui_dlg_info = GUIGetDialog( wnd );
        if( ui_dlg_info != NULL ) {
            colours_set = GUISetDialColours();
            uiprintfield( ui_dlg_info, field );
            if( colours_set ) {
                GUIResetDialColours();
            }
        }
    }
}

/*
 * GUIGetList -- return the list in the given field
 */

a_list *GUIGetList( VFIELD *field )
{
    a_list      *list;
    a_combo_box *combo_box;

    list = NULL;
    switch( field->typ ) {
    case FLD_PULLDOWN :
    case FLD_LISTBOX :
    case FLD_EDIT_MLE:
        list = field->u.list;
        break;
    case FLD_COMBOBOX :
        combo_box = field->u.combo;
        list = &combo_box->list;
        break;
    }
    return( list );
}

static void FreeEdit( an_edit_control *edit_control, bool free_edit, bool is_GUI_data )
{
    void        uifree( void * );  // Function in ui project

    if( is_GUI_data )
        GUIMemFree( edit_control->buffer );
    else
        uifree( edit_control->buffer );
    if( free_edit ) {
        GUIMemFree( edit_control );
    }
}

bool GUIGetFocus( gui_window *wnd, unsigned *id )
{
    dialog_node *dlg_node;

    if( id == NULL ) {
        return( false );
    }
    dlg_node = GUIGetDlgByWnd( wnd );
    if( dlg_node != NULL ) {
        if( dlg_node->ui_dlg_info->curr != NULL ) {
            *id = GUIGetControlId( wnd, dlg_node->ui_dlg_info->curr );
            return( *id != 0 );
        }
    }
    return( false );
}

static void FreeRadioGroup( a_radio_group * group )
{
    GUIMemFree( group->caption );
    GUIMemFree( group );
}

static void GUIFreeRadio( a_radio * radio )
{
    GUIMemFree( radio->str );
    GUIMemFree( radio );
}

static void GUIFreeCheck( a_check * check )
{
    GUIMemFree( check->str );
    GUIMemFree( check );
}

/*
 * GUIDoFreeField -- free memory allocation during GUIDoAddControl
 */

void GUIDoFreeField( VFIELD *field, a_radio_group **group )
{
    a_radio     *radio;
    a_combo_box *combo_box;

    switch( field->typ ) {
    case FLD_HOT :
        GUIFreeHotSpot( field->u.hs );
        break;
    case FLD_RADIO :
        radio = field->u.radio;
        if( ( group != NULL ) && ( *group != radio->group ) ) {
            FreeRadioGroup( radio->group );
            *group = radio->group;
        }
        GUIFreeRadio( radio );
        break;
    case FLD_CHECK :
        GUIFreeCheck( field->u.check );
        break;
    case FLD_COMBOBOX :
        combo_box = field->u.combo;
        GUIFreeList( &combo_box->list, false );
        FreeEdit( &combo_box->edit, false, true );
        GUIMemFree( combo_box );
        break;
    case FLD_EDIT :
        FreeEdit( field->u.edit, true, false );
        break;
    case FLD_INVISIBLE_EDIT :
        FreeEdit( field->u.edit, true, true );
        break;
    case FLD_EDIT_MLE:
    case FLD_LISTBOX :
    case FLD_PULLDOWN :
        GUIFreeList( field->u.list, true );
        break;
    case FLD_FRAME :
    case FLD_TEXT :
        if( field->u.str != NULL ) {
            GUIMemFree( field->u.str );
        }
        break;
    }
}

static void FreeFields( VFIELD *fields )
{
    int                 i;
    a_radio_group       *group;

    if( fields == NULL ) {
        return;
    }
    group = NULL;
    for( i = 0; fields[i].typ != FLD_VOID; i++ ) {
        if( fields[i].u.ptr != NULL ) {
            GUIDoFreeField( &fields[i], &group );
        }
    }
    GUIMemFree( fields );
}

static void CleanUpRadioGroups( void )
{
    if( RadioGroup != NULL ) {
        FreeRadioGroup( RadioGroup );
    }
}

bool GUIDoAddControl( gui_control_info *ctl_info, gui_window *wnd, VFIELD *field )
{
    a_radio             *radio;
    a_check             *check;
    a_combo_box         *combo_box;
    an_edit_control     *edit_control;
    bool                group_allocated;
    SAREA               area;
    bool                ok;

    group_allocated = false;
    if( (ctl_info->style & GUI_GROUP) &&
        (ctl_info->control_class == GUI_RADIO_BUTTON) ) {
        if( !Group ) {
            RadioGroup = (a_radio_group *)GUIMemAlloc( sizeof( a_radio_group ) );
            if( RadioGroup == NULL ) {
                return( false );
            }
            RadioGroup->value = -1;
            RadioGroup->caption = GUIStrDup( ctl_info->text, &ok );
            if( !ok ) {
                CleanUpRadioGroups();
                return( false );
            }
            Group = true;
            group_allocated = true;
        }
    }

    GUIGetSAREA( wnd, &area );
    if( !GUISetDialogArea( wnd, &field->area, &ctl_info->rect, &area ) ) {
        return( false );
    }

    field->typ = ui_types[ctl_info->control_class];
    if( field->typ == FLD_EDIT && ( ctl_info->style & GUI_EDIT_INVISIBLE ) ) {
        field->typ = FLD_INVISIBLE_EDIT;
    }

    switch( field->typ ) {
    case FLD_HOT :
        if( !GUICreateHot( ctl_info, field ) ) {
            return( false );
        }
        break;
    case FLD_RADIO :
        radio = (a_radio *)GUIMemAlloc( sizeof( a_radio ) );
        field->u.radio = radio;
        ok = false;
        if( radio != NULL ) {
            radio->str = GUIStrDup( ctl_info->text, &ok );
        }
        if( !ok ) {
            if( group_allocated ) {
                CleanUpRadioGroups();
            }
            return( false );
        }
        radio->value = GUI_FIRST_USER_EVENT + ctl_info->id;
        radio->group = RadioGroup;
        if( ( ctl_info->style & GUI_CHECKED ) &&
            ( ctl_info->style & GUI_AUTOMATIC ) ) {
            RadioGroup->def = radio->value;
            RadioGroup->value = radio->value;
        }
        break;
    case FLD_CHECK :
        check = (a_check *)GUIMemAlloc( sizeof( a_check ) );
        field->u.check = check;
        ok = false;
        if( check != NULL )
            check->str = GUIStrDup( ctl_info->text, &ok );
        if( !ok ) {
            return( false );
        }
        check->val = 0;
        if( ( ctl_info->style & GUI_CHECKED ) &&
            ( ctl_info->style & GUI_AUTOMATIC ) ) {
            check->def = true;
        } else {
            check->def = false;
        }
        break;
    case FLD_COMBOBOX :
        combo_box = (a_combo_box *)GUIMemAlloc( sizeof( a_combo_box ) );
        if( combo_box == NULL ) {
            return( false );
        }
        field->u.combo = combo_box;
        combo_box->edit.buffer = NULL;
        combo_box->edit.length = 0;
        if( !GUIFillInListBox( &combo_box->list ) ) {
            return( false );
        }
        if( ctl_info->text != NULL ) {
            if( !GUISetEditText( &combo_box->edit, ctl_info->text, false ) ) {
                return( false );
            }
        }
        combo_box->perm = false;
        break;
    case FLD_EDIT :
    case FLD_INVISIBLE_EDIT :
        edit_control = (an_edit_control *)GUIMemAlloc( sizeof( an_edit_control ) );
        if( edit_control == NULL ) {
            return( false );
        }
        field->u.edit = edit_control;
        edit_control->buffer = NULL;
        GUISetEditText( edit_control, ctl_info->text, field->typ != FLD_EDIT );
        break;
    case FLD_PULLDOWN :
    case FLD_LISTBOX :
        field->u.list = (a_list *)GUICreateListBox();
        if( field->u.list == NULL ) {
            return( false );
        }
        break;
    case FLD_EDIT_MLE:
        field->u.list = (a_list *)GUICreateEditMLE( ctl_info->text );
        if( field->u.list == NULL ) {
            return( false );
        }
        break;
    case FLD_TEXT :
    case FLD_FRAME :
        field->u.str = GUIStrDup( ctl_info->text, &ok );
        if( !ok ) {
            return( false );
        }
        break;
    default :
        return( false );
        break;
    }
    if( (ctl_info->style & GUI_GROUP)  &&
        (ctl_info->control_class == GUI_RADIO_BUTTON) ) {
        if( Group && !group_allocated ) {
            RadioGroup = NULL;
            Group = false;
        }
    }
    return( true );
}

bool GUIDeleteField( gui_window *wnd, unsigned id )
{
    dialog_node *dlg_node;
    a_dialog    *ui_dlg_info;
    int         index;
    VFIELD      *new_fields;
    VFIELD      *field;
    int         new_index;
    int         i;
    gui_control *control;

    dlg_node = GUIGetDlgByWnd( wnd );
    if( dlg_node == NULL ) {
        return( false );
    }
    ui_dlg_info = dlg_node->ui_dlg_info;
    field = GUIGetField( wnd, id );
    if( GetIndexOfField( ui_dlg_info, field, dlg_node->num_controls, &index ) ) {
        GUIDoFreeField( field, NULL );
        new_fields = (VFIELD *)GUIMemAlloc( sizeof( VFIELD ) * dlg_node->num_controls );
        for( i=0; i <= dlg_node->num_controls; i++ ) {
            new_index = i;
            if( i != index ) {
                if( i > index ) {
                    new_index--;
                    control = GUIGetControlByIndex( wnd, i );
                    if( control != NULL ) {
                        control->index = new_index;
                    }
                }
                memcpy( &new_fields[new_index], &ui_dlg_info->fields[i], sizeof( VFIELD ) );
            }
            if( ui_dlg_info->other == &ui_dlg_info->fields[i] ) {
                if( i == index ) {
                    ui_dlg_info->other = NULL;
                } else {
                    ui_dlg_info->other = &new_fields[new_index];
                }
            }
            if( ui_dlg_info->curr == &ui_dlg_info->fields[i] ) {
                if( i == index ) {
                    ui_dlg_info->curr = NULL;
                } else {
                    ui_dlg_info->curr = &new_fields[new_index];
                }
            }
            if( ui_dlg_info->first == &ui_dlg_info->fields[i] ) {
                if( i == index ) {
                    ui_dlg_info->first = NULL;
                } else {
                    ui_dlg_info->first = &new_fields[new_index];
                }
            }
        }
        GUIMemFree( ui_dlg_info->fields );
        ui_dlg_info->fields = new_fields;
        dlg_node->num_controls--;
        return( true );
    }
    return( false );
}

gui_control *GUIAddAControl( gui_control_info *ctl_info, gui_window *wnd )
{
    dialog_node *dlg_node;
    VFIELD      *new_field;
    gui_control *control;

    dlg_node = GUIGetDlgByWnd( wnd );
    if( dlg_node == NULL ) {
        return( NULL );
    }
    if( !ResetFieldSize( dlg_node, dlg_node->num_controls + 1 ) ) {
        return( NULL );
    }
    new_field = &dlg_node->ui_dlg_info->fields[dlg_node->num_controls];
    if( !GUIDoAddControl( ctl_info, wnd, new_field ) ) {
        ResetFieldSize( dlg_node, dlg_node->num_controls );
        return( NULL );
    }

    control = GUIInsertControl( wnd, ctl_info, dlg_node->num_controls );
    if( control == NULL ) {
        GUIDoFreeField( new_field, NULL );
        ResetFieldSize( dlg_node, dlg_node->num_controls );
    }
    return( control );
}

static void EditNotify( gui_key key, a_dialog *ui_dlg_info, gui_window *wnd )
{
    gui_key_control     key_control;

    if( ui_dlg_info->curr != NULL ) {
        switch( ui_dlg_info->curr->typ ) {
        case FLD_EDIT :
        case FLD_INVISIBLE_EDIT :
            key_control.key_state.key = key;
            GUIGetKeyState( &key_control.key_state.state );
            key_control.id = GUIGetControlId( wnd, ui_dlg_info->curr );
            if( key_control.id != 0 ) {
                GUIEVENTWND( wnd, GUI_KEY_CONTROL, &key_control );
            }
        }
    }
}

bool GUIResizeDialog( gui_window *wnd, SAREA *new_area )
{
    a_dialog    *ui_dlg_info;

    ui_dlg_info = GUIGetDialog( wnd );
    if( ui_dlg_info != NULL ) {
        uiresizedialog( ui_dlg_info, new_area );
        return( true );
    }
    return( false );
}

static void CheckNotify( a_dialog *ui_dlg_info, gui_window *wnd )
{
    unsigned    id;

    if( ui_dlg_info->curr != NULL ) {
        id = GUIGetControlId( wnd, ui_dlg_info->curr );
        if( id != 0 ) {
            GUIEVENTWND( wnd, GUI_CONTROL_CLICKED, &id );
        }
    }
}

static void ListNotify( EVENT ev, a_dialog *ui_dlg_info, gui_window *wnd )
{
    gui_event   gui_ev;
    a_list      *list;
    unsigned    id;

    if( ui_dlg_info->curr != NULL ) {
        list = GUIGetList( ui_dlg_info->curr );
        id = GUIGetControlId( wnd, ui_dlg_info->curr );
        if( id != 0 ) {
            switch( ev ) {
            case EV_LIST_BOX_CHANGED :
                gui_ev = GUI_CONTROL_CLICKED;
                break;
            case EV_LIST_BOX_DCLICK :
                gui_ev = GUI_CONTROL_DCLICKED;
                break;
            case EV_LIST_BOX_CLOSED :
                gui_ev = GUI_CONTROL_CLICKED;
                break;
            default :
                return;
            }
            GUIEVENTWND( wnd, gui_ev, &id );
        }
    }
}

void GUIFocusChangeNotify( a_dialog *ui_dlg_info )
{
    unsigned    id;
    gui_window  *wnd;
    dialog_node *dlg_node;

    dlg_node = GetDialog( ui_dlg_info );
    if( dlg_node != NULL ) {
        wnd = dlg_node->wnd;
        if( ( ui_dlg_info->other != NULL ) && ( wnd != NULL ) ) {
            switch( ui_dlg_info->other->typ ) {
            case FLD_EDIT :
            case FLD_INVISIBLE_EDIT :
            case FLD_PULLDOWN :
            case FLD_LISTBOX :
            case FLD_COMBOBOX :
            case FLD_EDIT_MLE :
                id = GUIGetControlId( wnd, ui_dlg_info->other );
                GUIEVENTWND( wnd, GUI_CONTROL_NOT_ACTIVE, &id );
            }
        }
    }
}

EVENT GUIProcessControlNotify( EVENT ev, a_dialog *ui_dlg_info, gui_window *wnd )
{
    unsigned    id;

    switch( ev ) {
    case EV_CHECK_BOX_CLICK :
        CheckNotify( ui_dlg_info, wnd );
        return( EV_NO_EVENT );
    case EV_LIST_BOX_DCLICK :
    case EV_LIST_BOX_CHANGED :
    case EV_LIST_BOX_CLOSED :
        ListNotify( ev, ui_dlg_info, wnd );
        return( EV_NO_EVENT );
    case EV_CURSOR_UP :
        EditNotify( GUI_KEY_UP, ui_dlg_info, wnd );
        return( EV_NO_EVENT );
    case EV_CURSOR_DOWN :
        EditNotify( GUI_KEY_DOWN, ui_dlg_info, wnd );
        return( EV_NO_EVENT );
    default :
        if( ev >= GUI_FIRST_USER_EVENT ) {
            id = GETID( ev );
            GUIEVENTWND( wnd, GUI_CONTROL_CLICKED, &id );
            return( EV_NO_EVENT );
        }
        return( ev );
    }
}

/*
 * GUIXCreateDialog -- create a dialog window
 */

bool GUIXCreateDialog( gui_create_info *dlg_info, gui_window *wnd,
                       int num_controls, gui_control_info *controls_info,
                       bool sys, long dlg_id )
{
    EVENT       ev;
    int         i;
    a_dialog    *ui_dlg_info;
    VFIELD      *fields;
    char        *title;
    VFIELD      *focus;
    int         size;
    bool        colours_set;
    bool        ok;

    if( dlg_id != -1 ) {
        if( !GUICreateDialogFromRes( dlg_id, dlg_info->parent,
                                     dlg_info->call_back, dlg_info->extra ) ) {
            return( false );
        }
        GUIMemFree( wnd );
        return( true );
    }

    sys = sys;
    RadioGroup = NULL;
    Group = false;
    fields = NULL;
    title = NULL;
    ui_dlg_info = NULL;
    colours_set = false;

    wnd->flags |= DIALOG;
    if( !GUISetupStruct( wnd, dlg_info, true ) ) {
        return( false );
    }

    size = ( num_controls + 1 ) * sizeof( VFIELD );
    fields = (VFIELD *)GUIMemAlloc( size );
    if( fields == NULL ) {
       return( false );
    }
    memset( fields, 0, size );
    focus = NULL;
    for( i = 0; i < num_controls; i++ ) {
        uiyield();
        if( !GUIDoAddControl( &controls_info[i], wnd, &fields[i] ) ) {
            GUIFreeDialog( ui_dlg_info, fields, title, colours_set, true );
            return( false );
        } else {
            if( ( focus == NULL ) && ( controls_info[i].style & GUI_FOCUS ) ) {
                focus = &fields[i];
            }
        }
    }
    CleanUpRadioGroups();
    fields[num_controls].typ = FLD_VOID; /* mark end of list */
    title = GUIStrDup( dlg_info->title, &ok );
    if( !ok ) {
        GUIFreeDialog( ui_dlg_info, fields, title, colours_set, true );
        return( false );
    }
    colours_set = GUISetDialColours();
    ui_dlg_info = uibegdialog( title, fields, wnd->screen.area.height,
                             wnd->screen.area.width, wnd->screen.area.row,
                             wnd->screen.area.col );
    if( ui_dlg_info == NULL ) {
        GUIFreeDialog( ui_dlg_info, fields, title, colours_set, true );
        return( false );
    }
    if( focus != NULL ) {
        uidialogsetcurr( ui_dlg_info, focus );
    }
    if( !InsertDialog( wnd, ui_dlg_info, num_controls, title, colours_set ) ) {
        GUIFreeDialog( ui_dlg_info, fields, title, colours_set, true );
        return( false );
    }
    for( i = 0; i < num_controls; i++ ) {
        uiyield();
        GUIInsertControl( wnd, &controls_info[i], i );
    }
    GUIEVENTWND( wnd, GUI_INIT_DIALOG, NULL );
    uipushlist( NULL );
    uipushlist( GUIUserEvents );
    GUIPushControlEvents();
    uipushlist( DlgEvents );
    while( ( GetDialog( ui_dlg_info ) != NULL ) ) {
        ev = uidialog( ui_dlg_info );
        switch( ev ) {
        case EV_KILL_UI:
            uiforceevadd( EV_KILL_UI );
        case EV_ESCAPE:
            GUIEVENTWND( wnd, GUI_DIALOG_ESCAPE, NULL );
            GUICloseDialog( wnd );
            break;
        default :
            GUIProcessControlNotify( ev, ui_dlg_info, wnd );
        }
    }
    return( true );
}

void GUIFreeDialog( a_dialog *ui_dlg_info, VFIELD *fields, char *title,
                    bool colours_set, bool is_dialog )
{
    if( ui_dlg_info != NULL ) {
        if( is_dialog ) {
            uienddialog( ui_dlg_info );
        } else {
            uifreedialog( ui_dlg_info );
        }
    }
    if( colours_set ) {
        GUIResetDialColours();
    }
    FreeFields( fields );
    GUIMemFree( title );
}

/*
 * GUICloseDialog -- close the given dialog box
 */

void GUICloseDialog( gui_window *wnd )
{
    VFIELD      *fields;
    a_dialog    *ui_dlg_info;
    char        *name;
    dialog_node *dlg_node;
    bool        colours_set;

    dlg_node = GUIGetDlgByWnd( wnd );
    if( dlg_node != NULL ) {
        ui_dlg_info = dlg_node->ui_dlg_info;
        fields = ui_dlg_info->fields;
        name = dlg_node->name;
        colours_set = dlg_node->colours_set;
    }
    GUIPopControlEvents();
    uipoplist( /* DlgEvents */ );
    uipoplist( /* GUIUserEvents */ );
    uipoplist( /* NULL */ );
    GUIDestroyDialog( wnd );
}

bool GUIGetDlgRect( gui_window *wnd, SAREA *area )
{
    a_dialog    *ui_dlg_info;

    if( GUI_IS_DIALOG( wnd ) ) {
        ui_dlg_info = GUIGetDialog( wnd );
        if( ui_dlg_info != NULL ) {
            uigetdialogarea( ui_dlg_info, area );
        } else {
            COPYAREA( wnd->screen.area, *area );
        }
        GUIAdjustDialogArea( area, +1 );
        return( true );
    }
    return( false );
}
