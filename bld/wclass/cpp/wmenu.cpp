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


#include "wmenu.hpp"
#include "wwindow.hpp"
#include "wmenuitm.hpp"
#include "wmenusep.hpp"
#include "wpopmenu.hpp"


WEXPORT WMenu::WMenu( bool )
    : _parent( NULL )
    , _isFloatingPopup( FALSE ) {
/*******************************/

    setMenuId( WMAIN_MENU_ID );
    setOwner( NULL );
}


WEXPORT WMenu::~WMenu() {
/***********************/

    if( owner() != NULL ) {
        owner()->clearMenu();
    }
    while( _children.count() > 0 ) {
        delete _children[ _children.count()-1 ];
    }
}


WPopupMenu* WEXPORT WMenu::insertPopup( WPopupMenu *popup, int index ) {
/**********************************************************************/

    _children.insertAt( index, popup );
    popup->setParent( this );
    if( owner() ) {
        popup->attachMenu( owner(), index );
    }
    return popup;
}


WPopupMenu * WEXPORT WMenu::removePopup( WPopupMenu *popup ) {
/************************************************************/

    return( removePopupAt( _children.indexOfSame( popup ) ) );
}


WPopupMenu * WEXPORT WMenu::removePopupAt( int index ) {
/******************************************************/

    if( index >= 0 ) {
        WPopupMenu *popup = (WPopupMenu *)_children.removeAt( index );
        popup->detachMenu();
        popup->setParent( NULL );
        return popup;
    }
    return NULL;
}


void WEXPORT WMenu::enableItem( bool enable, int index ) {
/********************************************************/

    WMenuObject *item = (WMenuObject *)_children[index];
    item->setEnable( enable );
    if( owner() ) {
        if( owner()->handle() ) {
            if( isFloatingPopup() ) {
                GUIEnableMenuItem( owner()->handle(), item->menuId(), enable, TRUE );
            }
            GUIEnableMenuItem( owner()->handle(), item->menuId(), enable, FALSE );
        }
    }
}

bool WEXPORT WMenu::itemEnabled( int index ) {
/********************************************/

    WMenuObject *item = (WMenuObject *)_children[index];
    return( (bool) item->enabled() );
}

void WEXPORT WMenu::checkItem( bool check, int index ) {
/******************************************************/

    WMenuObject *item = (WMenuObject *)_children[index];
    item->setCheck( check );
    if( owner() ) {
        if( owner()->handle() ) {
            if( isFloatingPopup() ) {
                GUICheckMenuItem( owner()->handle(), item->menuId(), check, TRUE );
            }
            GUICheckMenuItem( owner()->handle(), item->menuId(), check, FALSE );
        }
    }
}


void WEXPORT WMenu::setItemText( const char *text, int index ) {
/**************************************************************/

    WMenuObject *item = (WMenuObject *)_children[index];
    item->setText( text );
    if( owner() ) {
        if( owner()->handle() ) {
            if( isFloatingPopup() ) {
                GUISetMenuText( owner()->handle(), item->menuId(), (char *)text, TRUE );
            }
            GUISetMenuText( owner()->handle(), item->menuId(), (char *)text, FALSE );
        }
    }
}


void WMenu::attachMenu( WWindow *win, int ) {
/*******************************************/

    for( int idx = 0; idx < _children.count(); ++idx ) {
        WPopupMenu *menu_popup = (WPopupMenu *)_children[idx];
        menu_popup->attachMenu( win, idx );
    }
    setOwner( win );
}


void WMenu::detachMenu() {
/************************/

    // Check the handle in case WWindow::close() is called.
    // Calling WWindow::close() destroys the window before
    // the destructor for WWindow is invoked.
    if( owner()->handle() != NULL ) {
        GUIResetMenus( owner()->handle(), 0, NULL );
    }
    setOwner( NULL );
}


bool WMenu::isFloatingPopup() {
/*****************************/

    if( _isFloatingPopup ) return( TRUE );
    if( parent() ) {
        return( parent()->isFloatingPopup() );
    }
    return( FALSE );
}
