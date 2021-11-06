/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "guixutil.h"
#include "guismove.h"
#include "guixutil.h"
#include "guiutil.h"
#include "guistr.h"

static UIMENUITEM SystemMenu[NUM_SYSTEM_MENUS] = {
  { NULL,   EV_SYS_MENU_RESTORE,    0              },
  { NULL,   EV_SYS_MENU_MOVE,       0              },
  { NULL,   EV_SYS_MENU_SIZE,       0              },
  { NULL,   EV_SYS_MENU_MINIMIZE,   2              },
  { NULL,   EV_SYS_MENU_MAXIMIZE,   2              },
  { NULL,   ___,                    ITEM_SEPARATOR }, /* separator */
  { NULL,   EV_SYS_MENU_CLOSE,      0              },
};

void GUISetSystemMenuFlags( gui_window *wnd )
{
    int         i;
    UIMENUITEM  *menuitems;

    if( (wnd->style & GUI_SYSTEM_MENU) == 0 || ( wnd->menu == NULL ) ) {
        return;
    }
    menuitems = wnd->menu;

    for( i = 0; i < NUM_SYSTEM_MENUS; i++ ) {
        switch( menuitems[i].event ) {
        case EV_SYS_MENU_MOVE:
            if( GUI_WND_MAXIMIZED( wnd ) ) {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_GRAYED );
            } else {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_ENABLED );
            }
            break;
        case EV_SYS_MENU_SIZE:
            if( GUI_WND_MINIMIZED( wnd ) || (wnd->style & GUI_RESIZEABLE) == 0 ) {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_GRAYED );
            } else {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_ENABLED );
            }
            break;
        case EV_SYS_MENU_MINIMIZE:
            if( GUI_WND_MINIMIZED( wnd ) || (wnd->style & GUI_MINIMIZE) == 0 ) {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_GRAYED );
            } else {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_ENABLED );
            }
            break;
        case EV_SYS_MENU_MAXIMIZE:
            if( GUI_WND_MAXIMIZED( wnd ) || (wnd->style & GUI_MAXIMIZE) == 0 ) {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_GRAYED );
            } else {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_ENABLED );
            }
            break;
        case EV_SYS_MENU_RESTORE:
            if( GUI_WND_MAXIMIZED( wnd ) || GUI_WND_MINIMIZED( wnd ) ) {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_ENABLED );
            } else {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_GRAYED );
            }
            break;
        }
    }
}

bool GUISetSystemMenu( UIMENUITEM *menuitems, gui_create_styles style )
{
    int     i;
    bool    ok;

    for( i = 0; i < NUM_SYSTEM_MENUS; i++ ) {
        menuitems[i].event = SystemMenu[i].event;
        menuitems[i].flags = SystemMenu[i].flags;
        switch( menuitems[i].event ) {
        case EV_SYS_MENU_MOVE:
            menuitems[i].name = GUIStrDup( LIT( Move ), &ok );
            if( !ok ) {
                return( false );
            }
            break;
        case EV_SYS_MENU_SIZE:
            menuitems[i].name = GUIStrDup( LIT( Size ), &ok );
            if( !ok ) {
                return( false );
            }
            if( (style & GUI_RESIZEABLE) == 0 ) {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_GRAYED );
            }
            break;
        case EV_SYS_MENU_RESTORE:
            menuitems[i].name = GUIStrDup( LIT( Restore ), &ok );
            if( !ok ) {
                return( false );
            }
            GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_GRAYED );
            break;
        case EV_SYS_MENU_MINIMIZE:
            menuitems[i].name = GUIStrDup( LIT( Minimize ), &ok );
            if( !ok ) {
                return( false );
            }
            if( (style & GUI_MINIMIZE) == 0 ) {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_GRAYED );
            }
            break;
        case EV_SYS_MENU_MAXIMIZE:
            menuitems[i].name = GUIStrDup( LIT( Maximize ), &ok );
            if( !ok ) {
                return( false );
            }
            if( (style & GUI_MAXIMIZE) == 0 ) {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_GRAYED );
            }
            break;
        case EV_SYS_MENU_CLOSE:
            menuitems[i].name = GUIStrDup( LIT( Close ), &ok );
            if( !ok ) {
                return( false );
            }
            if( (style & GUI_CLOSEABLE) == 0 ) {
                GUIChangeMenu( &menuitems[i], GUI_STYLE_MENU_GRAYED );
            }
            break;
        }
    }
    return( true );
}

void GUIProcessSystemMenuEvent( ui_event ui_ev, gui_window *wnd )
{
    switch( ui_ev ) {
    case EV_SYS_MENU_RESTORE:
        GUIZoomWnd( wnd, GUI_NONE );
        break;
    case EV_SYS_MENU_MOVE:
        GUIStartKeyboardMoveResize( wnd, true );
        break;
    case EV_SYS_MENU_SIZE:
        GUIStartKeyboardMoveResize( wnd, false );
        break;
    case EV_SYS_MENU_MINIMIZE:
        GUIZoomWnd( wnd, GUI_MINIMIZE );
        break;
    case EV_SYS_MENU_MAXIMIZE:
        GUIZoomWnd( wnd, GUI_MAXIMIZE );
        break;
    case EV_SYS_MENU_CLOSE:
        GUICloseWnd( wnd );
        break;
    }
}
