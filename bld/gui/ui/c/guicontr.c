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

bool GUIAddControl( gui_control_info *ctl_info, gui_colour_set *plain,
                    gui_colour_set *standout )
{
    gui_control *control;
    bool        first_control;
    dialog_node *dlg_node;
    a_dialog    *ui_dlg_info;

    plain = plain;
    standout = standout;

    if( ( ctl_info == NULL ) || ( ctl_info->parent == NULL ) ) {
        return( false );
    }
    first_control = ( GUIGetDlgByWnd( ctl_info->parent ) == NULL );
    if( first_control ) {
        if( !GUIInsertDialog( ctl_info->parent ) ) {
            return( false );
        }
    }
    control = GUIAddAControl( ctl_info, ctl_info->parent );
    if( control != NULL ) {
        ui_dlg_info = GUIGetDialog( ctl_info->parent );
        uireinitdialog( ui_dlg_info, ui_dlg_info->fields );
        return( true );
    }
    if( first_control ) {
        dlg_node = GUIGetDlgByWnd( ctl_info->parent );
        if( dlg_node != NULL ) {
            GUIDeleteDialog( dlg_node->ui_dlg_info );
        }
    }
    GUIDeleteField( ctl_info->parent, ctl_info->id );
    GUIMemFree( control );
    return( false );
}

/*
 * GUIGetControl -- Get the control on wnd with id
 */

gui_control *GUIGetControl( gui_window *wnd, gui_ctl_id id )
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

static bool DeleteControl( gui_window *wnd, gui_ctl_id id )
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
            GUIMemFree( control );
            return( true );
        }
        prev = control;
    }
    return( false );
}

gui_control *GUIInsertControl( gui_window *wnd, gui_control_info *ctl_info, int index )
{
    gui_control *control;
    dialog_node *dlg_node;

    dlg_node = GUIGetDlgByWnd( wnd );
    if( dlg_node == NULL ) {
        return( NULL );
    }
    control = (gui_control *)GUIMemAlloc( sizeof( gui_control ) );
    if( control != NULL ) {
        control->control_class = ctl_info->control_class;
        control->id = ctl_info->id;
        control->parent = wnd;
        control->index = index;
        control->sibling = wnd->controls;
        wnd->controls = control;
        dlg_node->num_controls++;
    }
    return( control );
}

bool GUIDeleteControl( gui_window *wnd, gui_ctl_id id )
{
//    gui_control *control;
    gui_rect    rect;

    if( !GUI_IS_DIALOG( wnd ) ) {
        GUIGetControlRect( wnd, id, &rect );
    }
//    control = GUIGetControl( wnd, id );
    GUIGetControl( wnd, id );
    if( GUIDeleteField( wnd, id ) ) {
        if( DeleteControl( wnd, id ) ) {
            if( GUI_IS_DIALOG( wnd ) ) {
                uiredrawdialog( GUIGetDialog( wnd ) );
            } else {
                GUIWndDirtyRect( wnd, &rect );
            }
        }
    }
    return( false );
}

void GUIFreeAllControls( gui_window *wnd )
{
    gui_control *control;
    gui_control *next;
    dialog_node *dlg_node;
    a_dialog    *ui_dlg_info;

    for( control = wnd->controls; control != NULL; control = next ) {
        next = control->sibling;
        GUIMemFree( control );
    }
    wnd->controls = NULL;
    dlg_node = GUIGetDlgByWnd( wnd );
    if( dlg_node != NULL ) {
        GUIFreeDialog( dlg_node->ui_dlg_info, dlg_node->ui_dlg_info->fields, dlg_node->name,
                       dlg_node->colours_set, GUI_IS_DIALOG( wnd ) );
        ui_dlg_info = dlg_node->ui_dlg_info;
        GUIDeleteDialog( ui_dlg_info );
        if( !GUI_IS_DIALOG( wnd ) ) {
            GUIMemFree( ui_dlg_info );
        }
    }
}

bool GUILimitEditText( gui_window *wnd, gui_ctl_id id, int len )
{
    wnd=wnd;
    id=id;
    len=len;
    return( true );
}

void GUIResizeControls( gui_window *wnd, int row_diff, int col_diff )
{
    dialog_node *dlg_node;
    int         i;

    dlg_node = GUIGetDlgByWnd( wnd );
    if( dlg_node != NULL ) {
        for( i = 0; i < dlg_node->num_controls; i++ ) {
            uimovefield( dlg_node->ui_dlg_info, &dlg_node->ui_dlg_info->fields[i],
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
    a_dialog    *ui_dlg_info;
    bool        colours_set;

    row = row;
    col = col;
    ui_dlg_info = GUIGetDialog( wnd );
    if( ui_dlg_info != NULL ) {
        colours_set = GUISetDialColours();
        uipushlist( NULL );
        uipushlist( GUIUserEvents );
        GUIPushControlEvents();
        ev = uiprocessdialogevent( ev, ui_dlg_info );
        GUIPopControlEvents();
        uipoplist( /* GUIUserEvents */ );
        uipoplist( /* NULL */ );
        if( colours_set ) {
            GUIResetDialColours();
        }
        return( GUIProcessControlNotify( ev, ui_dlg_info, wnd ) );
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

bool GUIGetControlClass( gui_window *wnd, gui_ctl_id id,
                         gui_control_class *control_class )
{
    gui_control *control;

    if( control_class == NULL ) {
        return( false );
    }
    control = GUIGetControl( wnd, id );
    if( control != NULL ) {
        *control_class = control->control_class;
        return( true );
    }
    return( false );
}

bool GUIGetControlRect( gui_window *wnd, gui_ctl_id id, gui_rect *rect )
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
        return( true );
    }
    return( false );
}

extern bool GUIResizeControl( gui_window *wnd, gui_ctl_id id, gui_rect *rect )
{
    SAREA       area;
    a_dialog    *ui_dlg_info;
    VFIELD      *field;
    SAREA       new_area;
    gui_rect    old_rect;

    field = GUIGetField( wnd, id );
    ui_dlg_info = GUIGetDialog( wnd );
    if( ( field != NULL ) && ( ui_dlg_info != NULL ) ) {
        GUIGetSAREA( wnd, &area );
        if( !GUI_IS_DIALOG( wnd ) ) {
            GUIGetControlRect( wnd, id, &old_rect );
        }
        if( !GUISetDialogArea( wnd, &new_area, rect, &area ) ) {
            return( false );
        }
        COPYAREA( new_area, field->area );
        if( !GUI_IS_DIALOG( wnd ) ) {
            GUIWndDirtyRect( wnd, &old_rect );
            GUIRefreshControl( wnd, id );
        } else {
            uiredrawdialog( ui_dlg_info );
        }
    }
    return( false );
}

bool GUIIsControlVisible( gui_window *wnd, gui_ctl_id id )
{
    gui_control *control;

    control = GUIGetControl( wnd, id );
    return( control != NULL );
}
