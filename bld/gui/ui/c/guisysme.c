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
#include "guisysme.h"
#include "guisize.h"
#include "guimenu.h"
#include "guixwind.h"
#include "guixloop.h"
#include "guismove.h"
#include "guiutil.h"
#include "guistr.h"

static MENUITEM SystemMenu[NUM_SYSTEM_MENUS] = {
  { NULL,       GUI_MENU_RESTORE,  0              },
  { NULL,       GUI_MENU_MOVE,     0              },
  { NULL,       GUI_MENU_SIZE,     0              },
  { NULL,       GUI_MENU_MINIMIZE, 2              },
  { NULL,       GUI_MENU_MAXIMIZE, 2              },
  { NULL,       EV_NO_EVENT,       ITEM_SEPARATOR }, /* separator */
  { NULL,       GUI_MENU_CLOSE,    0              },
};

void GUISetSystemMenuFlags( gui_window *wnd )
{
    int         i;
    MENUITEM    *menu;

    if( !( wnd->style & GUI_SYSTEM_MENU ) || ( wnd->menu == NULL ) ) {
        return;
    }
    menu = wnd->menu;

    for( i = 0; i < NUM_SYSTEM_MENUS; i++ ) {
        switch( menu[i].event ) {
        case GUI_MENU_MOVE :
            if( GUI_WND_MAXIMIZED( wnd ) ) {
                GUIChangeMenu( &menu[i], GUI_GRAYED );
            } else {
                GUIChangeMenu( &menu[i], GUI_ENABLED );
            }
            break;
        case GUI_MENU_SIZE :
            if( GUI_WND_MINIMIZED( wnd ) || !( wnd->style & GUI_RESIZEABLE ) ) {
                GUIChangeMenu( &menu[i], GUI_GRAYED );
            } else {
                GUIChangeMenu( &menu[i], GUI_ENABLED );
            }
            break;
        case GUI_MENU_MINIMIZE :
            if( GUI_WND_MINIMIZED( wnd ) || !( wnd->style & GUI_MINIMIZE ) ) {
                GUIChangeMenu( &menu[i], GUI_GRAYED );
            } else {
                GUIChangeMenu( &menu[i], GUI_ENABLED );
            }
            break;
        case GUI_MENU_MAXIMIZE :
            if( GUI_WND_MAXIMIZED( wnd ) || !( wnd->style & GUI_MAXIMIZE ) ) {
                GUIChangeMenu( &menu[i], GUI_GRAYED );
            } else {
                GUIChangeMenu( &menu[i], GUI_ENABLED );
            }
            break;
        case GUI_MENU_RESTORE :
            if( GUI_WND_MAXIMIZED( wnd ) || GUI_WND_MINIMIZED( wnd ) ) {
                GUIChangeMenu( &menu[i], GUI_ENABLED );
            } else {
                GUIChangeMenu( &menu[i], GUI_GRAYED );
            }
            break;
        }
    }
}

bool GUISetSystemMenu( MENUITEM *menu, gui_create_styles style )
{
    int i;

    for( i = 0; i < NUM_SYSTEM_MENUS; i++ ) {
        menu[i].event = SystemMenu[i].event;
        menu[i].flags = SystemMenu[i].flags;
        switch( menu[i].event ) {
        case GUI_MENU_MOVE :
            if( !GUIStrDup( LIT( Move ), &menu[i].name ) ) {
                return( FALSE );
            }
            break;
        case GUI_MENU_SIZE :
            if( !GUIStrDup( LIT( Size ), &menu[i].name ) ) {
                return( FALSE );
            }
            if( !( style & GUI_RESIZEABLE ) ) {
                GUIChangeMenu( &menu[i], GUI_GRAYED );
            }
            break;
        case GUI_MENU_RESTORE :
            if( !GUIStrDup( LIT( Restore ), &menu[i].name ) ) {
                return( FALSE );
            }
            GUIChangeMenu( &menu[i], GUI_GRAYED );
            break;
        case GUI_MENU_MINIMIZE :
            if( !GUIStrDup( LIT( Minimize ), &menu[i].name ) ) {
                return( FALSE );
            }
            if( !( style & GUI_MINIMIZE ) ) {
                GUIChangeMenu( &menu[i], GUI_GRAYED );
            }
            break;
        case GUI_MENU_MAXIMIZE :
            if( !GUIStrDup( LIT( Maximize ), &menu[i].name ) ) {
                return( FALSE );
            }
            if( !( style & GUI_MAXIMIZE ) ) {
                GUIChangeMenu( &menu[i], GUI_GRAYED );
            }
            break;
        case GUI_MENU_CLOSE :
            if( !GUIStrDup( LIT( Close ), &menu[i].name ) ) {
                return( FALSE );
            }
            if( !( style & GUI_CLOSEABLE ) ) {
                GUIChangeMenu( &menu[i], GUI_GRAYED );
            }
            break;
        }
    }
    return( TRUE );
}

void GUIProcessSysMenuEvent( EVENT ev, gui_window *wnd )
{
    switch( ev ) {
    case GUI_MENU_RESTORE :
        GUIZoomWnd( wnd, NONE );
        break;
    case GUI_MENU_MOVE :
        GUIStartKeyboardMoveResize( wnd, TRUE );
        break;
    case GUI_MENU_SIZE :
        GUIStartKeyboardMoveResize( wnd, FALSE );
        break;
    case GUI_MENU_MINIMIZE :
        GUIZoomWnd( wnd, GUI_MINIMIZE );
        break;
    case GUI_MENU_MAXIMIZE :
        GUIZoomWnd( wnd, GUI_MAXIMIZE );
        break;
    case GUI_MENU_CLOSE :
        GUICloseWnd( wnd );
        break;
    }
}
