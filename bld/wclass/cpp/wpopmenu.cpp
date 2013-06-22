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


#include "wpopmenu.hpp"
#include "wmenusep.hpp"
#include "wwindow.hpp"


WEXPORT WPopupMenu::WPopupMenu( const char *text )
    : WMenu( FALSE )
    , _client( NULL )
    , _popup( NULL )
    , _isMdiPopup( FALSE ) {
/**************************/

    setOwner( NULL );
    setText( text );
    setMenuId( WWindow::_idMaster++ );
    WWindow::_popupIdMap.setThis( this, (WHANDLE)(pointer_int)menuId() );
}


WEXPORT WPopupMenu::~WPopupMenu() {
/*********************************/

    if( _parent != NULL ) {
        _parent->removePopup( this );
    }
    WWindow::_popupIdMap.clearThis( this );
}


void WEXPORT WPopupMenu::onPopup( WObject *client, cbp popup ) {
/**************************************************************/

    _client = client;
    _popup = popup;
}


void WEXPORT WPopupMenu::popup() {
/********************************/

    if( _client && _popup ) {
        (_client->*_popup)( this );
    }
}


void WEXPORT WPopupMenu::hilighted( bool ) {
/******************************************/

}


void WEXPORT WPopupMenu::insertSeparator( int index ) {
/*****************************************************/

    WMenuSeparator* item = new WMenuSeparator();
    _children.insertAt( index, item );
    item->setParent( this );
    if( owner() ) {
        item->attachMenu( owner(), index );
    }
}


WMenuItem * WEXPORT WPopupMenu::insertItem( WMenuItem *item, int index ) {
/************************************************************************/

    _children.insertAt( index, item );
    item->setParent( this );
    if( owner() ) {
        item->attachMenu( owner(), index );
    }
    return( item );
}


WMenuItem * WEXPORT WPopupMenu::removeItem( WMenuItem *item ) {
/*************************************************************/

    return( removeItemAt( _children.indexOfSame( item ) ) );
}


WMenuItem * WEXPORT WPopupMenu::removeItemAt( int index ) {
/*********************************************************/

    if( index >= 0 ) {
        WMenuItem *item = (WMenuItem *)_children.removeAt( index );
        item->detachMenu();
        item->setParent( NULL );
        return( item );
    }
    return( NULL );
}


void WEXPORT WPopupMenu::setMdiPopup() {
/**************************************/

    _isMdiPopup = TRUE;
}


void WEXPORT WPopupMenu::track( WWindow *owner ) {
/************************************************/

    gui_point   p;

    GUIGetMousePosn( owner->handle(), &p );
    setFloatingPopup( TRUE );
    attachChildren( owner );
    GUITrackFloatingPopup( owner->handle(), &p, GUI_TRACK_BOTH, NULL );
    setFloatingPopup( FALSE );
}


void WPopupMenu::attachItem( WWindow *win, int idx ) {
/****************************************************/

    gui_menu_struct     menu_item;
    unsigned long       menu_style;

    menu_item.label = (char *)text();
    menu_item.id = menuId();
    menu_style = GUI_ENABLED;
    if( checked() ) {
        menu_style |= GUI_MENU_CHECKED;
    }
    if( !enabled() ) {
        menu_style |= GUI_GRAYED;
    }
    if( _isMdiPopup ) {
        menu_style |= GUI_MDIWINDOW;
    }
    menu_item.style = (gui_menu_styles)menu_style;
    menu_item.hinttext = NULL;
    menu_item.num_child_menus = 0;
    menu_item.child = NULL;
    if( (parent()->menuId() == WMAIN_MENU_ID) ||
        parent()->isFloatingMain() ) {
        // appending popup menu to menu bar
        //      or
        // appending popup menu to top level floating popup menu
        GUIInsertMenu( win->handle(), idx, &menu_item, isFloatingPopup() );
    } else {
        // popup menu is a menu item
        GUIInsertMenuToPopup( win->handle(), parent()->menuId(), idx,
                              &menu_item, isFloatingPopup() );
    }
}


void WEXPORT WPopupMenu::attachChildren( WWindow *win ) {
/*******************************************************/

    for( int idx = 0; idx < _children.count(); ++idx ) {
        WMenuObject *menu_object = (WMenuObject *)_children[idx];
        menu_object->attachMenu( win, idx );
    }
}


void WEXPORT WPopupMenu::attachMenu( WWindow *win, int idx ) {
/************************************************************/

    setOwner( win );
    attachItem( win, idx );
    attachChildren( win );
}


void WEXPORT WPopupMenu::detachMenu() {
/*************************************/

    if( owner() ) {
        if( owner()->handle() ) {
            if( parent()->isFloatingPopup() ) {
                GUIDeleteMenuItem( owner()->handle(), menuId(), TRUE );
            }
            GUIDeleteMenuItem( owner()->handle(), menuId(), FALSE );
        }
    }
    setOwner( NULL );
}
