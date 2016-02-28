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
#include "guilistb.h"
#include "guixdlg.h"
#include "guicontr.h"
#include <string.h>

/*
 * GUIControlSetRedraw -- set the redraw flag for a given window control
 */
bool GUIControlSetRedraw( gui_window *wnd, gui_ctl_id id, bool redraw )
{
    wnd     = wnd;
    id      = id;
    redraw  = redraw;
    return( true );
}

/*
 * RefreshListCombo
 */

static bool RefreshListCombobox( VFIELD *field, gui_window *wnd, gui_ctl_id id )
{
    a_dialog    *ui_dlg_info;

    switch( field->typ ) {
    case FLD_COMBOBOX :
        uiupdatecombobox( field->u.combo );
        /* meant to fall through */
    case FLD_PULLDOWN :
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

bool GUIAddText( gui_window *wnd, gui_ctl_id id, const char *text )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListBoxAddText( list, text, -1 ) );
    }
    return( false );
}

bool GUIAddTextList( gui_window *wnd, gui_ctl_id id, int items,
                    const void *data_handle, GUIPICKGETTEXT *getstring )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListBoxAddTextList( list, items, data_handle, getstring ) );
    }
    return( false );
}

bool GUIDeleteItem( gui_window *wnd, gui_ctl_id id, gui_ctl_idx choice )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListBoxDeleteItem( list, choice ) );
    }
    return( false );
}

bool GUIClearList( gui_window *wnd, gui_ctl_id id )
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

gui_ctl_idx GUIGetCurrSelect( gui_window *wnd, gui_ctl_id id )
{
    VFIELD      *field;
    a_list      *list;
    gui_ctl_idx ret;

    if( GetList( wnd, id, &field, &list ) ) {
        if( !GUIListCurr( list, 0, false, &ret ) ) {
            return( 0 );
        }
        return( ret );
    }
    return( -1 );
}

bool GUISetCurrSelect( gui_window *wnd, gui_ctl_id id, gui_ctl_idx choice )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        if( !GUIListCurr( list, choice, true, NULL ) ) {
            return( false );
        }
        return( RefreshListCombobox( field, wnd, id ) );
    }
    return( false );
}

gui_ctl_idx GUIGetListSize( gui_window *wnd, gui_ctl_id id )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListSize( list ) );
    }
    return( 0 );
}

bool GUIInsertText( gui_window *wnd, gui_ctl_id id, gui_ctl_idx choice, const char *text )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListBoxAddText( list, text, choice ) );
    }
    return( false );
}

bool GUISetTopIndex( gui_window *wnd, gui_ctl_id id, gui_ctl_idx choice )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        GUIListBoxTopIndex( list, choice, true, NULL );
        return( RefreshListCombobox( field, wnd, id ) );
    }
    return( false );
}

gui_ctl_idx GUIGetTopIndex( gui_window *wnd, gui_ctl_id id )
{
    VFIELD      *field;
    a_list      *list;
    gui_ctl_idx ret;

    if( GetList( wnd, id, &field, &list ) ) {
        if( GUIListBoxTopIndex( list, 0, false, &ret ) ) {
            return( ret );
        }
    }
    return( -1 );
}
