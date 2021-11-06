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


#include "wmenuitm.hpp"
#include "wwindow.hpp"
#include "wmenu.hpp"
#include "wpopmenu.hpp"


WEXPORT WMenuItem::WMenuItem( const char *text, WObject* obj, cbm pick,
                              cbh hint, const char *htext )
    : _parent( NULL )
    , _client( obj )
    , _pick( pick )
    , _hint( hint )
    , _hintText( htext )
    , _tagPtr( NULL ) {
/*********************/

    setText( text );
    setMenuId( WWindow::_idMaster++ );
    WWindow::_idMap.setThis( this, (WHANDLE)(pointer_uint)menuId() );
}


WEXPORT WMenuItem::~WMenuItem() {
/*******************************/

    if( parent() ) {
        parent()->removeItem( this );
    }
    WWindow::_idMap.clearThis( this );
}


void WEXPORT WMenuItem::picked() {
/********************************/

    if( _client && _pick ) {
        (_client->*_pick)( this );
    }
}


void WEXPORT WMenuItem::hilighted( bool ) {
/*****************************************/

// Portable UI does not inform WCLASS when a menu item has been hilighted.
// Note that the hint text for a menu item is automatically placed in the
// status window by the portable UI when the menu item has been hilighted.

}


void WMenuItem::attachMenu( WWindow *win, int position )
/******************************************************/
{
    gui_menu_struct     menu_item;
    gui_menu_styles     menu_style;

    menu_item.label = text();
    menu_item.id = menuId();
    menu_style = GUI_STYLE_MENU_ENABLED;
    if( checked() ) {
        menu_style = (gui_menu_styles)( menu_style | GUI_STYLE_MENU_CHECKED );
    }
    if( !enabled() ) {
        menu_style = (gui_menu_styles)( menu_style | GUI_STYLE_MENU_GRAYED );
    }
    menu_item.style = menu_style;
    menu_item.hinttext = (const char *)_hintText;
    menu_item.child = NoMenu;
    if( parent()->isFloatingMain() ) {
        // appending menu item to top level floating popup menu
        GUIInsertMenuByIdx( win->handle(), position, &menu_item, true );
    } else {
        // appending menu item to popup menu
        GUIInsertMenuToPopup( win->handle(), parent()->menuId(), position, &menu_item, parent()->isFloatingPopup() );
    }
    setOwner( win );
}


void WMenuItem::detachMenu() {
/****************************/

    if( owner() ) {
        if( owner()->handle() ) {
            if( parent()->isFloatingPopup() ) {
                GUIDeleteMenuItem( owner()->handle(), menuId(), true );
            }
            GUIDeleteMenuItem( owner()->handle(), menuId(), false );
        }
    }
    setOwner( NULL );
}
