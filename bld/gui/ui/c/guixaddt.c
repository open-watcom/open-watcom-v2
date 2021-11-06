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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "guiwind.h"
#include "guilistb.h"
#include "guixdlg.h"
#include "guicontr.h"
#include <string.h>

/*
 * GUIControlSetRedraw -- set the redraw flag for a given window control
 */
bool GUIAPI GUIControlSetRedraw( gui_window *wnd, gui_ctl_id id, bool redraw )
{
    /* unused parameters */ (void)wnd; (void)id; (void)redraw;
    return( true );
}

/*
 * RefreshListCombo
 */

static bool RefreshListCombobox( VFIELD *field, gui_window *wnd, gui_ctl_id id )
{
    a_dialog    *ui_dlg_info;

    switch( field->typ ) {
    case FLD_COMBOBOX:
        uiupdatecombobox( field->u.combo );
        /* meant to fall through */
    case FLD_PULLDOWN:
        ui_dlg_info = GUIGetDialog( wnd );
        if( ui_dlg_info == NULL ) {
            return( false );
        }
        GUIRefreshControl( wnd, id );
        return( true );
        break;
    }
    return( false );
}

static bool GetList( gui_window *wnd, gui_ctl_id id, VFIELD **field, a_list **list )
{
    *field = GUIGetField( wnd, id );
    if( *field == NULL ) {
        return( false );
    }
    *list = GUIGetList( *field );
    if( *list == NULL ) {
        return( false );
    }
    return( true );
}

bool GUIAPI GUIAddText( gui_window *wnd, gui_ctl_id id, const char *text )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListBoxAddText( list, text, -1 ) );
    }
    return( false );
}

bool GUIAPI GUIAddTextList( gui_window *wnd, gui_ctl_id id, int num_items,
                    const void *data_handle, GUIPICKGETTEXT *getstring )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListBoxAddTextList( list, num_items, data_handle, getstring ) );
    }
    return( false );
}

bool GUIAPI GUIDeleteItem( gui_window *wnd, gui_ctl_id id, int choice )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListBoxDeleteItem( list, choice ) );
    }
    return( false );
}

bool GUIAPI GUIClearList( gui_window *wnd, gui_ctl_id id )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        if( !GUIClearListBox( list ) ) {
            return( false );
        }
        if( !GUISetText( wnd, id, NULL ) ) { /* temporary */
            return( false );
        }
        return( RefreshListCombobox( field, wnd, id ) );
    }
    return( false );
}

bool GUIAPI GUIGetCurrSelect( gui_window *wnd, gui_ctl_id id, int *choice )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListGetCurr( list, choice ) );
    }
    return( false );
}

bool GUIAPI GUISetCurrSelect( gui_window *wnd, gui_ctl_id id, int choice )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        if( !GUIListSetCurr( list, choice ) ) {
            return( false );
        }
        return( RefreshListCombobox( field, wnd, id ) );
    }
    return( false );
}

int GUIAPI GUIGetListSize( gui_window *wnd, gui_ctl_id id )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListSize( list ) );
    }
    return( 0 );
}

bool GUIAPI GUIInsertText( gui_window *wnd, gui_ctl_id id, int choice, const char *text )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListBoxAddText( list, text, choice ) );
    }
    return( false );
}

bool GUIAPI GUISetTopIndex( gui_window *wnd, gui_ctl_id id, int choice )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        GUIListBoxSetTopIndex( list, choice );
        return( RefreshListCombobox( field, wnd, id ) );
    }
    return( false );
}

int GUIAPI GUIGetTopIndex( gui_window *wnd, gui_ctl_id id )
{
    VFIELD      *field;
    a_list      *list;
    int         choice;

    if( GetList( wnd, id, &field, &list ) ) {
        if( GUIListBoxGetTopIndex( list, &choice ) ) {
            return( choice );
        }
    }
    return( -1 );
}
