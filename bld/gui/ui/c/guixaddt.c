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
bool GUIControlSetRedraw( gui_window *wnd, unsigned control, bool redraw )
{
    wnd     = wnd;
    control = control;
    redraw  = redraw;
    return( TRUE );
}

/*
 * RefreshListCombo
 */

static bool RefreshListCombobox( VFIELD *field, gui_window *wnd, unsigned id )
{
    a_dialog    *dialog;

    switch( field->typ ) {
    case FLD_COMBOBOX :
        uiupdatecombobox( (a_combo_box *)field->ptr );
        /* meant to fall through */
    case FLD_PULLDOWN :
        dialog = GUIGetDialog( wnd );
        if( dialog == NULL ) {
            return( FALSE );
        }
        GUIRefreshControl( wnd, id );
        return( TRUE );
        break;
    }
    return( FALSE );
}

static bool GetList( gui_window *wnd, unsigned id, VFIELD **field, a_list **list )
{
    *field = GUIGetField( wnd, id );
    if( *field == NULL ) {
        return( FALSE );
    }
    *list = GUIGetList( *field );
    if( *list == NULL ) {
        return( FALSE );
    }
    return( TRUE );
}

bool GUIAddText( gui_window *wnd, unsigned id, char *text )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListBoxAddText( list, text, -1 ) );
    }
    return( FALSE );
}

bool GUIAddTextList( gui_window *wnd, unsigned id, unsigned items,
                    void *handle, char *(*getstring)(void*,unsigned) )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListBoxAddTextList( list, items, handle, getstring ) );
    }
    return( FALSE );
}

bool GUIDeleteItem( gui_window *wnd, unsigned id, int choice )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListBoxDeleteItem( list, choice ) );
    }
    return( FALSE );
}

bool GUIClearList( gui_window *wnd, unsigned id )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        if( !GUIClearListBox( list ) ) {
            return( FALSE );
        }
        if( !GUISetText( wnd, id, NULL ) ) { /* temporary */
            return( FALSE );
        }
        return( RefreshListCombobox( field, wnd, id ) );
    }
    return( FALSE );
}

int GUIGetCurrSelect( gui_window *wnd, unsigned id )
{
    VFIELD      *field;
    a_list      *list;
    int         ret;

    if( GetList( wnd, id, &field, &list ) ) {
        if( !GUIListCurr( list, 0, FALSE, &ret ) ) {
            return( FALSE );
        }
        return( ret );
    }
    return( -1 );
}

bool GUISetCurrSelect( gui_window *wnd, unsigned id, int choice )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        if( !GUIListCurr( list, choice, TRUE, NULL ) ) {
            return( FALSE );
        }
        return( RefreshListCombobox( field, wnd, id ) );
    }
    return( FALSE );
}

int GUIGetListSize( gui_window *wnd, unsigned id )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListSize( list ) );
    }
    return( 0 );
}

bool GUIInsertText( gui_window *wnd, unsigned id, int choice, char *text )
{
    VFIELD      *field;
    a_list      *list;

    if( GetList( wnd, id, &field, &list ) ) {
        return( GUIListBoxAddText( list, text, choice ) );
    }
    return( FALSE );
}

bool GUISetTopIndex( gui_window *wnd, unsigned id, int choice )
{
    VFIELD      *field;
    a_list      *list;

    choice = choice;
    if( GetList( wnd, id, &field, &list ) ) {
        GUIListBoxTopIndex( list, choice, TRUE, NULL );
        return( RefreshListCombobox( field, wnd, id ) );
    }
    return( FALSE );
}

int GUIGetTopIndex( gui_window *wnd, unsigned id )
{
    VFIELD      *field;
    a_list      *list;
    int         ret;

    if( GetList( wnd, id, &field, &list ) ) {
        if( GUIListBoxTopIndex( list, 0, FALSE, &ret ) ) {
            return( ret );
        }
    }
    return( -1 );
}
