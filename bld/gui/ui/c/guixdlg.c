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
static bool Group = FALSE;

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

static bool InsertDialog( gui_window *wnd, a_dialog *dialog, int num_controls,
                          char *name, bool colours_set )
{
    dialog_node *curr;

    curr = (dialog_node *) GUIMemAlloc( sizeof( dialog_node ) );
    if( curr == NULL ) {
        return( FALSE );
    }
    curr->wnd = wnd;
    curr->dialog = dialog;
    curr->next = MyDialog;
    curr->num_controls = num_controls;
    curr->name = name;
    curr->colours_set = colours_set;
    MyDialog = curr;
    return( TRUE );
}

void GUIDeleteDialog( a_dialog *dialog )
{
    dialog_node *curr;
    dialog_node *prev;

    prev = NULL;
    for( curr = MyDialog; curr != NULL; curr = curr->next ) {
        if( curr->dialog == dialog ) {
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

static dialog_node *GetDialog( a_dialog *dialog )
{
    dialog_node *curr;

    for( curr = MyDialog; curr != NULL; curr = curr->next ) {
        if( curr->dialog == dialog ) {
            return( curr );
        }
    }
    return( NULL );
}

static bool GetIndexOfField( a_dialog *dialog, VFIELD *field, int num_controls,
                             int *index )
{
    int i;

    for( i = 0; i < num_controls; i++ ) {
        if( &dialog->fields[i] == field ) {
            *index = i;
            return( TRUE );
        }
    }
    return( FALSE );
}

unsigned GUIGetControlId( gui_window *wnd, VFIELD *field )
{
    gui_control *control;
    int         index;
    dialog_node *node;

    node = GUIGetDlgByWnd( wnd );
    if( node == NULL ) {
        return( 0 );
    }
    if( !GetIndexOfField( node->dialog, field, node->num_controls, &index ) ) {
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
    dialog_node *node;

    node = GUIGetDlgByWnd( wnd );
    if( node != NULL ) {
        return( node->dialog );
    } else {
        return( NULL );
    }
}

VFIELD *GUIGetField( gui_window *wnd, unsigned id )
{
    a_dialog    *ui_dialog;
    dialog_node *node;
    gui_control *control;

    node = GUIGetDlgByWnd( wnd );
    if( node != NULL ) {
        ui_dialog = node->dialog;
        control = GUIGetControl( wnd, id );
        if( control != NULL && ( control->index < node->num_controls ) ) {
            return( &ui_dialog->fields[control->index] );
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

static bool ResetFieldSize( dialog_node *dialog, int new_num )
{
    VFIELD      *fields;

    fields = (VFIELD *)GUIMemRealloc( dialog->dialog->fields,
                                   ( new_num + 1 ) * sizeof( VFIELD ) );
    if( fields != NULL ) {
        dialog->dialog->fields = fields;
        memset( &fields[new_num], 0, sizeof( VFIELD ) );
        return( TRUE );
    }
    return( FALSE );
}

bool GUIInsertDialog( gui_window *wnd )
{
    a_dialog    *dialog;
    dialog_node *node;

    if( GUIGetDlgByWnd( wnd ) != NULL ) {
        return( TRUE );
    } else {
        dialog = (a_dialog *)GUIMemAlloc( sizeof( a_dialog ) );
        if( dialog == NULL ) {
            return( FALSE );
        }
        memset( dialog, 0, sizeof( a_dialog ) );
        dialog->vs = &wnd->screen;
        if( InsertDialog( wnd, dialog, 0, NULL, FALSE ) ) {
            node = GUIGetDlgByWnd( wnd );
            if( node != NULL ) {
                if( ResetFieldSize( node, 0 ) ) {
                    uireinitdialog( dialog, dialog->fields );
                    return( TRUE );
                }
            }
        }
        return( FALSE );
    }
}

void GUIRefreshControl( gui_window *wnd, unsigned id )
{
    a_dialog    *dialog;
    bool        colours_set;
    VFIELD      *field;

    field = GUIGetField( wnd, id );
    if( field != NULL ) {
        dialog = GUIGetDialog( wnd );
        if( dialog != NULL ) {
            colours_set = GUISetDialColours();
            uiprintfield( dialog, field );
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
        list = (a_list *)field->ptr;
        break;
    case FLD_COMBOBOX :
        combo_box = (a_combo_box *)field->ptr;
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
    dialog_node *node;

    if( id == NULL ) {
        return( FALSE );
    }
    node = GUIGetDlgByWnd( wnd );
    if( node != NULL ) {
        if( node->dialog->curr != NULL ) {
            *id = GUIGetControlId( wnd, node->dialog->curr );
            return( *id != (unsigned)NULL );
        }
    }
    return( FALSE );
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
        GUIFreeHotSpot( (a_hot_spot *)field->ptr );
        break;
    case FLD_RADIO :
        radio = (a_radio *)field->ptr;
        if( ( group != NULL ) && ( *group != radio->group ) ) {
            FreeRadioGroup( radio->group );
            *group = radio->group;
        }
        GUIFreeRadio( radio );
        break;
    case FLD_CHECK :
        GUIFreeCheck( (a_check *)field->ptr );
        break;
    case FLD_COMBOBOX :
        combo_box = (a_combo_box *)field->ptr;
        GUIFreeList( &combo_box->list, FALSE );
        FreeEdit( &combo_box->edit, FALSE, TRUE );
        GUIMemFree( combo_box );
        break;
    case FLD_EDIT :
        FreeEdit( field->ptr, TRUE, FALSE );
        break;
    case FLD_INVISIBLE_EDIT :
        FreeEdit( field->ptr, TRUE, TRUE );
        break;
    case FLD_EDIT_MLE:
    case FLD_LISTBOX :
    case FLD_PULLDOWN :
        GUIFreeList( (a_list *)field->ptr, TRUE );
        break;
    case FLD_FRAME :
    case FLD_TEXT :
        if( field->ptr != NULL ) {
            GUIMemFree( (char *)field->ptr );
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
        if( fields[i].ptr != NULL ) {
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

bool GUIDoAddControl( gui_control_info *info, gui_window *wnd, VFIELD *field )
{
    a_radio             *radio;
    a_check             *check;
    a_combo_box         *combo_box;
    an_edit_control     *edit_control;
    bool                group_allocated;
    SAREA               area;

    group_allocated = FALSE;
    if( (info->style & GUI_GROUP) &&
        (info->control_class == GUI_RADIO_BUTTON) ) {
        if( !Group ) {
            RadioGroup = (a_radio_group * )GUIMemAlloc( sizeof( a_radio_group ) );
            if( RadioGroup == NULL ) {
                return( FALSE );
            }
            RadioGroup->value = -1;
            if( !GUIStrDup( info->text, &RadioGroup->caption ) ) {
                CleanUpRadioGroups();
                return( FALSE );
            }
            Group = TRUE;
            group_allocated = TRUE;
        }
    }

    GUIGetSAREA( wnd, &area );
    if( !GUISetDialogArea( wnd, &field->area, &info->rect, &area ) ) {
        return( FALSE );
    }

    field->typ = ui_types[info->control_class];
    if( field->typ == FLD_EDIT && ( info->style & GUI_EDIT_INVISIBLE ) ) {
        field->typ = FLD_INVISIBLE_EDIT;
    }

    switch( field->typ ) {
    case FLD_HOT :
        if( !GUICreateHot( info, (a_hot_spot_field *)field ) ) {
            return( FALSE );
        }
        break;
    case FLD_RADIO :
        radio = (a_radio * )GUIMemAlloc( sizeof( a_radio ) );
        field->ptr = radio;
        if( ( radio == NULL ) || !GUIStrDup( info->text, &radio->str ) ) {
            if( group_allocated ) {
                CleanUpRadioGroups();
            }
            return( FALSE );
        }
        radio->value = GUI_FIRST_USER_EVENT + info->id;
        radio->group = RadioGroup;
        if( ( info->style & GUI_CHECKED ) &&
            ( info->style & GUI_AUTOMATIC ) ) {
            RadioGroup->def = radio->value;
            RadioGroup->value = radio->value;
        }
        break;
    case FLD_CHECK :
        check = (a_check * )GUIMemAlloc( sizeof( a_check ) );
        field->ptr = check;
        if( ( check == NULL ) || !GUIStrDup( info->text, &check->str ) ) {
            return( FALSE );
        }
        check->val = 0;
        if( ( info->style & GUI_CHECKED ) &&
            ( info->style & GUI_AUTOMATIC ) ) {
            check->def = TRUE;
        } else {
            check->def = FALSE;
        }
        break;
    case FLD_COMBOBOX :
        combo_box = (a_combo_box * )GUIMemAlloc( sizeof( a_combo_box ) );
        if( combo_box == NULL ) {
            return( FALSE );
        }
        field->ptr = combo_box;
        combo_box->edit.buffer = NULL;
        combo_box->edit.length = 0;
        if( !GUIFillInListBox( &combo_box->list ) ) {
            return( FALSE );
        }
        if( info->text != NULL ) {
            if( !GUISetEditText( &combo_box->edit, info->text, FALSE ) ) {
                return( FALSE );
            }
        }
        combo_box->perm = FALSE;
        break;
    case FLD_EDIT :
    case FLD_INVISIBLE_EDIT :
        edit_control = (an_edit_control * )GUIMemAlloc( sizeof( an_edit_control ) );
        if( edit_control == NULL ) {
            return( FALSE );
        }
        field->ptr = edit_control;
        edit_control->buffer = NULL;
        GUISetEditText( edit_control, info->text, field->typ != FLD_EDIT );
        break;
    case FLD_PULLDOWN :
    case FLD_LISTBOX :
        field->ptr = (void *)GUICreateListBox();
        if( field->ptr == NULL ) {
            return( FALSE );
        }
        break;
    case FLD_EDIT_MLE:
        field->ptr = (void *)GUICreateEditMLE( info->text );
        if( field->ptr == NULL ) {
            return( FALSE );
        }
        break;
    case FLD_TEXT :
    case FLD_FRAME :
        if( !GUIStrDup( info->text, (char **)&field->ptr ) ) {
            return( FALSE );
        }
        break;
    default :
        return( FALSE );
        break;
    }
    if( (info->style & GUI_GROUP)  &&
        (info->control_class == GUI_RADIO_BUTTON) ) {
        if( Group && !group_allocated ) {
            RadioGroup = NULL;
            Group = FALSE;
        }
    }
    return( TRUE );
}

bool GUIDeleteField( gui_window *wnd, unsigned id )
{
    dialog_node *dialog;
    a_dialog    *ui_dialog;
    int         index;
    VFIELD      *new_fields;
    VFIELD      *field;
    int         new_index;
    int         i;
    gui_control *control;

    dialog = GUIGetDlgByWnd( wnd );
    if( dialog == NULL ) {
        return( FALSE );
    }
    ui_dialog = dialog->dialog;
    field = GUIGetField( wnd, id );
    if( GetIndexOfField( ui_dialog, field, dialog->num_controls, &index ) ) {
        GUIDoFreeField( field, NULL );
        new_fields = (VFIELD *)GUIMemAlloc( sizeof( VFIELD ) * dialog->num_controls );
        for( i=0; i <= dialog->num_controls; i++ ) {
            new_index = i;
            if( i != index ) {
                if( i > index ) {
                    new_index--;
                    control = GUIGetControlByIndex( wnd, i );
                    if( control != NULL ) {
                        control->index = new_index;
                    }
                }
                memcpy( &new_fields[new_index], &ui_dialog->fields[i], sizeof( VFIELD ) );
            }
            if( ui_dialog->other == &ui_dialog->fields[i] ) {
                if( i == index ) {
                    ui_dialog->other = NULL;
                } else {
                    ui_dialog->other = &new_fields[new_index];
                }
            }
            if( ui_dialog->curr == &ui_dialog->fields[i] ) {
                if( i == index ) {
                    ui_dialog->curr = NULL;
                } else {
                    ui_dialog->curr = &new_fields[new_index];
                }
            }
            if( ui_dialog->first == &ui_dialog->fields[i] ) {
                if( i == index ) {
                    ui_dialog->curr = NULL;
                } else {
                    ui_dialog->first = &new_fields[new_index];
                }
            }
        }
        GUIMemFree( ui_dialog->fields );
        ui_dialog->fields = new_fields;
        dialog->num_controls--;
        return( TRUE );
    }
    return( FALSE );
}

gui_control *GUIAddAControl( gui_control_info *info, gui_window *wnd )
{
    dialog_node *dialog;
    VFIELD      *new_field;
    gui_control *control;

    dialog = GUIGetDlgByWnd( wnd );
    if( dialog == NULL ) {
        return( NULL );
    }
    if( !ResetFieldSize( dialog, dialog->num_controls + 1 ) ) {
        return( NULL );
    }
    new_field = &dialog->dialog->fields[dialog->num_controls];
    if( !GUIDoAddControl( info, wnd, new_field ) ) {
        ResetFieldSize( dialog, dialog->num_controls );
        return( NULL );
    }

    control = GUIInsertControl( wnd, info, dialog->num_controls );
    if( control == NULL ) {
        GUIDoFreeField( new_field, NULL );
        ResetFieldSize( dialog, dialog->num_controls );
    }
    return( control );
}

static void EditNotify( gui_key key, a_dialog *ui_dialog, gui_window *wnd )
{
    gui_key_control     key_control;

    if( ui_dialog->curr != NULL ) {
        switch( ui_dialog->curr->typ ) {
        case FLD_EDIT :
        case FLD_INVISIBLE_EDIT :
            key_control.key_state.key = key;
            GUIGetKeyState( &key_control.key_state.state );
            key_control.id = GUIGetControlId( wnd, ui_dialog->curr );
            if( key_control.id != (unsigned)NULL ) {
                GUIEVENTWND( wnd, GUI_KEY_CONTROL, &key_control );
            }
        }
    }
}

bool GUIResizeDialog( gui_window *wnd, SAREA *new_area )
{
    a_dialog    *ui_dialog;

    ui_dialog = GUIGetDialog( wnd );
    if( ui_dialog != NULL ) {
        uiresizedialog( ui_dialog, new_area );
        return( TRUE );
    }
    return( FALSE );
}

static void CheckNotify( a_dialog *ui_dialog, gui_window *wnd )
{
    unsigned    id;

    if( ui_dialog->curr != NULL ) {
        id = GUIGetControlId( wnd, ui_dialog->curr );
        if( id != (unsigned)NULL ) {
            GUIEVENTWND( wnd, GUI_CONTROL_CLICKED, &id );
        }
    }
}

static void ListNotify( EVENT ev, a_dialog *ui_dialog, gui_window *wnd )
{
    gui_event   gui_ev;
    a_list      *list;
    unsigned    id;

    if( ui_dialog->curr != NULL ) {
        list = GUIGetList( ui_dialog->curr );
        id = GUIGetControlId( wnd, ui_dialog->curr );
        if( id != (unsigned)NULL ) {
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

void GUIFocusChangeNotify( a_dialog *ui_dialog )
{
    unsigned    id;
    gui_window  *wnd;
    dialog_node *node;

    node = GetDialog( ui_dialog );
    if( node != NULL ) {
        wnd = node->wnd;
        if( ( ui_dialog->other != NULL ) && ( wnd != NULL ) ) {
            switch( ui_dialog->other->typ ) {
            case FLD_EDIT :
            case FLD_INVISIBLE_EDIT :
            case FLD_PULLDOWN :
            case FLD_LISTBOX :
            case FLD_COMBOBOX :
            case FLD_EDIT_MLE :
                id = GUIGetControlId( wnd, ui_dialog->other );
                GUIEVENTWND( wnd, GUI_CONTROL_NOT_ACTIVE, &id );
            }
        }
    }
}

EVENT GUIProcessControlNotify( EVENT ev, a_dialog *ui_dialog, gui_window *wnd )
{
    unsigned    id;

    switch( ev ) {
    case EV_CHECK_BOX_CLICK :
        CheckNotify( ui_dialog, wnd );
        return( EV_NO_EVENT );
    case EV_LIST_BOX_DCLICK :
    case EV_LIST_BOX_CHANGED :
    case EV_LIST_BOX_CLOSED :
        ListNotify( ev, ui_dialog, wnd );
        return( EV_NO_EVENT );
    case EV_CURSOR_UP :
        EditNotify( GUI_KEY_UP, ui_dialog, wnd );
        return( EV_NO_EVENT );
    case EV_CURSOR_DOWN :
        EditNotify( GUI_KEY_DOWN, ui_dialog, wnd );
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

bool GUIXCreateDialog( gui_create_info *dialog, gui_window *wnd,
                       int num_controls, gui_control_info *info_controls,
                       bool sys, long dlg_id )
{
    EVENT       ev;
    int         i;
    a_dialog    *ui_dialog;
    VFIELD      *fields;
    char        *title;
    VFIELD      *focus;
    int         size;
    bool        colours_set;

    if( dlg_id != -1 ) {
        if( !GUICreateDialogFromRes( dlg_id, dialog->parent,
                                     dialog->call_back, dialog->extra ) ) {
            return( FALSE );
        }
        GUIMemFree( wnd );
        return( TRUE );
    }

    sys = sys;
    RadioGroup = NULL;
    Group = FALSE;
    fields = NULL;
    title = NULL;
    ui_dialog = NULL;
    colours_set = FALSE;

    wnd->flags |= DIALOG;
    if( !GUISetupStruct( wnd, dialog, TRUE ) ) {
        return( FALSE );
    }

    size = ( num_controls + 1 ) * sizeof( VFIELD );
    fields = (VFIELD *)GUIMemAlloc( size );
    if( fields == NULL ) {
       return( FALSE );
    }
    memset( fields, 0, size );
    focus = NULL;
    for( i = 0; i < num_controls; i++ ) {
        uiyield();
        if( !GUIDoAddControl( &info_controls[i], wnd, &fields[i] ) ) {
            GUIFreeDialog( ui_dialog, fields, title, colours_set, TRUE );
            return( FALSE );
        } else {
            if( ( focus == NULL ) && ( info_controls[i].style & GUI_FOCUS ) ) {
                focus = &fields[i];
            }
        }
    }
    CleanUpRadioGroups();
    fields[num_controls].typ = FLD_VOID; /* mark end of list */

    if( !GUIStrDup( dialog->text, &title ) ) {
        GUIFreeDialog( ui_dialog, fields, title, colours_set, TRUE );
        return( FALSE );
    }
    colours_set = GUISetDialColours();
    ui_dialog = uibegdialog( title, fields, wnd->screen.area.height,
                             wnd->screen.area.width, wnd->screen.area.row,
                             wnd->screen.area.col );
    if( ui_dialog == NULL ) {
        GUIFreeDialog( ui_dialog, fields, title, colours_set, TRUE );
        return( FALSE );
    }
    if( focus != NULL ) {
        uidialogsetcurr( ui_dialog, focus );
    }
    if( !InsertDialog( wnd, ui_dialog, num_controls, title, colours_set ) ) {
        GUIFreeDialog( ui_dialog, fields, title, colours_set, TRUE );
        return( FALSE );
    }
    for( i = 0; i < num_controls; i++ ) {
        uiyield();
        GUIInsertControl( wnd, &info_controls[i], i );
    }
    GUIEVENTWND( wnd, GUI_INIT_DIALOG, NULL );
    uipushlist( NULL );
    uipushlist( GUIUserEvents );
    GUIPushControlEvents();
    uipushlist( DlgEvents );
    while( ( GetDialog( ui_dialog ) != NULL ) ) {
        ev = uidialog( ui_dialog );
        switch( ev ) {
        case EV_KILL_UI:
            uiforceevadd( EV_KILL_UI );
        case EV_ESCAPE:
            GUIEVENTWND( wnd, GUI_DIALOG_ESCAPE, NULL );
            GUICloseDialog( wnd );
            break;
        default :
            GUIProcessControlNotify( ev, ui_dialog, wnd );
        }
    }
    return( TRUE );
}

void GUIFreeDialog( a_dialog *dialog, VFIELD *fields, char *title,
                    bool colours_set, bool is_dialog )
{
    if( dialog != NULL ) {
        if( is_dialog ) {
            uienddialog( dialog );
        } else {
            uifreedialog( dialog );
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
    a_dialog    *dialog;
    char        *name;
    dialog_node *node;
    bool        colours_set;

    node = GUIGetDlgByWnd( wnd );
    if( node != NULL ) {
        dialog = node->dialog;
        fields = dialog->fields;
        name = node->name;
        colours_set = node->colours_set;
    }
    GUIPopControlEvents();
    uipoplist( /* DlgEvents */ );
    uipoplist( /* GUIUserEvents */ );
    uipoplist( /* NULL */ );
    GUIDestroyDialog( wnd );
}

bool GUIGetDlgRect( gui_window *wnd, SAREA *area )
{
    a_dialog    *ui_dialog;

    if( GUI_IS_DIALOG( wnd ) ) {
        ui_dialog = GUIGetDialog( wnd );
        if( ui_dialog != NULL ) {
            uigetdialogarea( ui_dialog, area );
        } else {
            COPYAREA( wnd->screen.area, *area );
        }
        GUIAdjustDialogArea( area, +1 );
        return( TRUE );
    }
    return( FALSE );
}
