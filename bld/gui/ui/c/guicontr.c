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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "guiwind.h"
#include "guicontr.h"
#include "guixdlg.h"
#include "guixedit.h"
#include "guihotsp.h"
#include "guilistb.h"
#include "guiscale.h"
#include "guixutil.h"
#include "guicolor.h"
#include <string.h>

extern EVENT GUIUserEvents[];

/*
 * GUIAddControl -- add the given control to the parent window
 */

bool GUIAddControl( gui_control_info *info, gui_colour_set *plain,
                    gui_colour_set *standout )
{
    gui_control *control;
    bool        first_control;
    dialog_node *node;
    a_dialog    *dialog;

    plain = plain;
    standout = standout;

    if( ( info == NULL ) || ( info->parent == NULL ) ) {
        return( FALSE );
    }
    first_control = ( GUIGetDlgByWnd( info->parent ) == NULL );
    if( first_control ) {
        if( !GUIInsertDialog( info->parent ) ) {
            return( FALSE );
        }
    }
    control = GUIAddAControl( info, info->parent );
    if( control != NULL ) {
        dialog = GUIGetDialog( info->parent );
        uireinitdialog( dialog, dialog->fields );
        return( TRUE );
    }
    if( first_control ) {
        node = GUIGetDlgByWnd( info->parent );
        if( node != NULL ) {
            GUIDeleteDialog( node->dialog );
        }
    }
    GUIDeleteField( info->parent, info->id );
    GUIFree( control );
    return( FALSE );
}

/*
 * GUIGetControl -- Get the control on wnd with id
 */

gui_control *GUIGetControl( gui_window *wnd, unsigned id )
{
    gui_control *control;

    for( control = wnd->controls; control != NULL; control = control->sibling ) {
        if( control->id == id ) {
            return( control );
        }
    }
    return( NULL );
}

/*
 * GUIGetControlByIndex -- Get the control on wnd with index
 */

gui_control *GUIGetControlByIndex( gui_window *wnd, int index )
{
    gui_control *control;

    for( control = wnd->controls; control != NULL; control = control->sibling ) {
        if( control->index == index ) {
            return( control );
        }
    }
    return( NULL );
}

static bool DeleteControl( gui_window *wnd, unsigned id )
{
    gui_control *control;
    gui_control *prev;

    prev = NULL;
    for( control = wnd->controls; control != NULL; control = control->sibling ) {
        if( control->id == id ) {
            if( prev != NULL ) {
                prev->sibling = control->sibling;
            } else {
                wnd->controls = control->sibling;
            }
            GUIFree( control );
            return( TRUE );
        }
        prev = control;
    }
    return( FALSE );
}

gui_control *GUIInsertControl( gui_window *wnd, gui_control_info *info,
                               int index )
{
    gui_control *control;
    dialog_node *dialog;

    dialog = GUIGetDlgByWnd( wnd );
    if( dialog == NULL ) {
        return( NULL );
    }
    control = (gui_control *)GUIAlloc( sizeof( gui_control ) );
    if( control != NULL ) {
        control->class = info->control_class;
        control->id = info->id;
        control->parent = wnd;
        control->index = index;
        control->sibling = wnd->controls;
        wnd->controls = control;
        dialog->num_controls++;
    }
    return( control );
}

bool GUIDeleteControl( gui_window *wnd, unsigned id )
{
    gui_control *control;
    gui_rect    rect;

    if( !GUI_IS_DIALOG( wnd ) ) {
        GUIGetControlRect( wnd, id, &rect );
    }
    control = GUIGetControl( wnd, id );
    if( GUIDeleteField( wnd, id ) ) {
        if( DeleteControl( wnd, id ) ) {
            if( GUI_IS_DIALOG( wnd ) ) {
                uiredrawdialog( GUIGetDialog( wnd ) );
            } else {
                GUIWndDirtyRect( wnd, &rect );
            }
        }
    }
    return( FALSE );
}

void GUIFreeAllControls( gui_window *wnd )
{
    gui_control *control;
    gui_control *next;
    dialog_node *node;
    a_dialog    *dialog;

    for( control = wnd->controls; control != NULL; control = next ) {
        next = control->sibling;
        GUIFree( control );
    }
    wnd->controls = NULL;
    node = GUIGetDlgByWnd( wnd );
    if( node != NULL ) {
        GUIFreeDialog( node->dialog, node->dialog->fields, node->name,
                       node->colours_set, GUI_IS_DIALOG( wnd ) );
        dialog = node->dialog;
        GUIDeleteDialog( dialog );
        if( !GUI_IS_DIALOG( wnd ) ) {
            GUIFree( dialog );
        }
    }
}

bool GUILimitEditText( gui_window *wnd, unsigned id, int len )
{
    wnd=wnd;
    id=id;
    len=len;
    return( TRUE );
}

void GUIResizeControls( gui_window *wnd, int row_diff, int col_diff )
{
    dialog_node *dialog;
    int         i;

    dialog = GUIGetDlgByWnd( wnd );
    if( dialog != NULL ) {
        for( i = 0; i < dialog->num_controls; i++ ) {
            uimovefield( dialog->dialog, &dialog->dialog->fields[i],
                         row_diff, col_diff );
        }
    }
}
/*
 * GUIProcessControlEvent -- send the given event to the given control
 */

EVENT GUIProcessControlEvent( gui_window *wnd, EVENT ev, gui_ord row,
                              gui_ord col )
{
    a_dialog    *dialog;
    bool        colours_set;

    row = row;
    col = col;
    dialog = GUIGetDialog( wnd );
    if( dialog != NULL ) {
        colours_set = GUISetDialColours();
        uipushlist( NULL );
        uipushlist( GUIUserEvents );
        GUIPushControlEvents();
        ev = uiprocessdialogevent( ev, dialog );
        GUIPopControlEvents();
        uipoplist( /* GUIUserEvents */ );
        uipoplist( /* NULL */ );
        if( colours_set ) {
            GUIResetDialColours();
        }
        return( GUIProcessControlNotify( ev, dialog, wnd ) );
   } else {
        return( ev );
    }
}

void GUIEnumControls( gui_window *wnd, CONTRENUMCALLBACK *func, void *param )
{
    gui_control *curr;

    for( curr = wnd->controls; curr != NULL; curr = curr->sibling ) {
        (*func)( wnd, curr->id, param );
    }
}

bool GUIGetControlClass( gui_window *wnd, unsigned id,
                         gui_control_class *control_class )
{
    gui_control *control;

    if( control_class == NULL ) {
        return( FALSE );
    }
    control = GUIGetControl( wnd, id );
    if( control != NULL ) {
        *control_class = control->class;
        return( TRUE );
    }
    return( FALSE );
}

bool GUIGetControlRect( gui_window *wnd, unsigned id, gui_rect *rect )
{
    VFIELD      *field;
    SAREA       area;

    field = GUIGetField( wnd, id );
    if( field != NULL ) {
        COPYAREA( field->area, area  );
        if( !GUI_IS_DIALOG( wnd ) ) {
            area.row--;
            area.col--;
        }
        GUIScreenToScaleRectR( &area, rect );
        return( TRUE );
    }
    return( FALSE );
}

extern bool GUIResizeControl( gui_window *wnd, unsigned id, gui_rect *rect )
{
    SAREA       area;
    a_dialog    *ui_dialog;
    VFIELD      *field;
    SAREA       new_area;
    gui_rect    old_rect;

    field = GUIGetField( wnd, id );
    ui_dialog = GUIGetDialog( wnd );
    if( ( field != NULL ) && ( ui_dialog != NULL ) ) {
        GUIGetSAREA( wnd, &area );
        if( !GUI_IS_DIALOG( wnd ) ) {
            GUIGetControlRect( wnd, id, &old_rect );
        }
        if( !GUISetDialogArea( wnd, &new_area, rect, &area ) ) {
            return( FALSE );
        }
        COPYAREA( new_area, field->area );
        if( !GUI_IS_DIALOG( wnd ) ) {
            GUIWndDirtyRect( wnd, &old_rect );
            GUIRefreshControl( wnd, id );
        } else {
            uiredrawdialog( ui_dialog );
        }
    }
    return( FALSE );
}

bool GUIIsControlVisible( gui_window *wnd, unsigned id )
{
    gui_control *control;

    control = GUIGetControl( wnd, id );
    return( control != NULL );
}
