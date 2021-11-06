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
#include "guiscale.h"
#include "guimenu.h"
#include "guisysme.h"
#include "guixloop.h"
#include "guixutil.h"
#include "guifloat.h"
#include "guievent.h"
#include <ctype.h>
#include <string.h>


typedef enum {
    MENU_NONE,
    MENU_FLOAT,
    MENU_SYS
} MenuStatus;

UIMENUITEM          *GUIPopupMenu   = NULL;

static MenuStatus   MenuState       = MENU_NONE;
static gui_window   *MenuWnd        = NULL;

/*
 * MapLocation --
 */

static void MapLocation( gui_window *wnd, const gui_point *point, guix_point *scr_point )
{
    guix_ord    scr_x;
    guix_ord    scr_y;

    scr_x = GUIScaleToScreenH( point->x );
    scr_y = GUIScaleToScreenV( point->y );
    if( GUI_DO_HSCROLL( wnd ) ) {
        scr_x -= wnd->hgadget->pos;
    }
    if( GUI_DO_VSCROLL( wnd ) ) {
        scr_y -= wnd->vgadget->pos;
    }
    scr_point->x = scr_x + wnd->use.col + 1;
    scr_point->y = scr_y + wnd->use.row + 1;
}

/*
 * GUIProcessMenuCurr
 */

void GUIProcessMenuCurr( UIMENUITEM *menuitem )
{
    gui_window          *top_wnd;
    gui_ctl_id          id;
    gui_window          *menu_wnd;
    gui_menu_styles     style;
    hint_type           type;

    id = 0;
    if( ( menuitem == NULL ) || ( menuitem->event == EV_NO_EVENT ) ) {
        style = GUI_STYLE_MENU_IGNORE;
    } else {
        if( menuitem->flags & ITEM_SEPARATOR ) {
            style = GUI_STYLE_MENU_SEPARATOR;
        } else {
            if( IS_CTLEVENT( menuitem->event ) ) {
                id = EV2ID( menuitem->event );
                if( MENUGRAYED( *menuitem ) ) {
                    style = GUI_STYLE_MENU_GRAYED;
                } else {
                    style = GUI_STYLE_MENU_ENABLED;
                }
            } else {
                style = GUI_STYLE_MENU_IGNORE;
            }
        }
    }
    type = MENU_HINT;
    top_wnd = GUIGetMenuWindow();
    menu_wnd = NULL;
    switch( MenuState ) {
    case MENU_NONE:
        menu_wnd = top_wnd;
        if( top_wnd != NULL && GUIHasToolBar( top_wnd ) && GUIToolBarFixed( top_wnd ) ) {
            if( GUIHasHintText( menu_wnd, id, TOOL_HINT ) ) {
                type = TOOL_HINT;
            }
        }
        break;
    case MENU_FLOAT:
        type = FLOAT_HINT;
        /* fall through */
    case MENU_SYS:
        menu_wnd = MenuWnd;
    }
    if( ( top_wnd != NULL ) && ( menu_wnd != NULL ) ) {
        GUIDisplayHintText( top_wnd, menu_wnd, id, type, style );
    }
}

/*
 * GUICreateMenuPopup - create a floating popup menu
 */

ui_event GUICreateMenuPopup( gui_window *wnd, guix_point *scr_location, UIMENUITEM *menuitems,
                                    gui_mouse_track track, gui_ctl_id *curr_id )
{
    ui_event    ui_ev;
    gui_ctl_id  id;
    ATTR        attr_active;
    ATTR        attr_hot;
    ATTR        attr_curr_active;
    ATTR        attr_hot_curr;
    ATTR        attr_inactive;
    ATTR        attr_curr_inactive;
    ATTR        attr_menu;
    gui_window  *top;
    SAREA       area;
    DESCMENU    desc;
    bool        ok;

    MenuWnd = wnd;
    if( MenuState == MENU_NONE ) {
        MenuState = MENU_SYS;
    }
    if( menuitems == NULL ) {
        return( EV_NO_EVENT );
    }
    attr_active         = uisetattr( ATTR_ACTIVE,           WNDATTR( wnd, GUI_MENU_PLAIN ) );
    attr_hot            = uisetattr( ATTR_HOT,              WNDATTR( wnd, GUI_MENU_STANDOUT ) );
    attr_curr_active    = uisetattr( ATTR_CURR_ACTIVE,      WNDATTR( wnd, GUI_MENU_ACTIVE ) );
    attr_hot_curr       = uisetattr( ATTR_HOT_CURR,         WNDATTR( wnd, GUI_MENU_ACTIVE_STANDOUT ) );
    attr_inactive       = uisetattr( ATTR_INACTIVE,         WNDATTR( wnd, GUI_MENU_GRAYED ) );
    attr_curr_inactive  = uisetattr( ATTR_CURR_INACTIVE,    WNDATTR( wnd, GUI_MENU_GRAYED_ACTIVE ) );
    attr_menu           = uisetattr( ATTR_MENU,             WNDATTR( wnd, GUI_MENU_FRAME ) );

    ui_ev = EV_NO_EVENT;
    if( ( curr_id != NULL ) && ( *curr_id != 0 ) ) {
        ui_ev = ID2EV( *curr_id );
    }
    top = GUIGetTopWnd( wnd );
    COPYRECTX( top->use, area );
    area.row += top->vs.area.row;
    area.col += top->vs.area.col;
    ok = uiposfloatingpopup( menuitems, &desc, wnd->vs.area.row + scr_location->y,
                            wnd->vs.area.col + scr_location->x, &area, NULL );
    if( ok ) {
        ui_ev = uicreatepopupinarea( menuitems, &desc, track & GUI_TRACK_LEFT,
                              track & GUI_TRACK_RIGHT, ui_ev, &area, false );

        if( ui_ev == EV_KILL_UI ) {
            uiforceevadd( EV_KILL_UI );
            ui_ev = EV_NO_EVENT;
        }

        GUIProcessMenuCurr( NULL );
    }
    uisetattr( ATTR_ACTIVE,         attr_active );
    uisetattr( ATTR_HOT,            attr_hot );
    uisetattr( ATTR_CURR_ACTIVE,    attr_curr_active );
    uisetattr( ATTR_HOT_CURR,       attr_hot_curr );
    uisetattr( ATTR_INACTIVE,       attr_inactive );
    uisetattr( ATTR_CURR_INACTIVE,  attr_curr_inactive );
    uisetattr( ATTR_MENU,           attr_menu );

    if( !ok ) {
        return( EV_NO_EVENT );
    }

    if( ( ui_ev != EV_MOUSE_DCLICK ) && ( ui_ev != EV_NO_EVENT ) ) {
        if( IS_CTLEVENT( ui_ev ) ) {
            id = EV2ID( ui_ev );
            GUIEVENT( wnd, GUI_CLICKED, &id );
            if( curr_id != NULL ) {
                *curr_id = id;
            }
        } else {
            GUIProcessSystemMenuEvent( ui_ev, wnd );
        }
    }
    if( MenuState == MENU_SYS ) {
        MenuState = MENU_NONE;
    }
    MenuWnd = NULL;

    return( ui_ev );
}

static void TrackPopup( gui_window *wnd, const gui_point *location,
                        UIMENUITEM *menuitems, gui_mouse_track track, gui_ctl_id *curr_id )
{
    guix_point  scr_location;

    MapLocation( wnd, location, &scr_location );

    MenuState = MENU_FLOAT;
    uipushlist( NULL );
    uipushlist( GUIUserEvents );
    GUICreateMenuPopup( wnd, &scr_location, menuitems, track, curr_id );
    uipoplist( /* GUIUserEvents */ );
    uipoplist( /* NULL */ );
    MenuState = MENU_NONE;
}

/*
 * GUIXCreateFloatingPopup - create a floating popup menu
 */

bool GUIXCreateFloatingPopup( gui_window *wnd, const gui_point *location,
                             const gui_menu_items *menus,
                             gui_mouse_track track, gui_ctl_id *curr_id )
{
    UIMENUITEM  *menuitems;

    if( !GUICreateMenuItems( menus, &menuitems ) ) {
        return( false );
    }
    TrackPopup( wnd, location, menuitems, track, curr_id );
    GUIFreeMenuItems( menuitems );
    return( true );
}

bool GUIAPI GUITrackFloatingPopup( gui_window *wnd, const gui_point *location,
                        gui_mouse_track track, gui_ctl_id *curr_id )
{
    if( GUIPopupMenu != NULL ) {
        TrackPopup( wnd, location, GUIPopupMenu, track, curr_id );
        GUIFreeMenuItems( GUIPopupMenu );
        GUIPopupMenu = NULL;
        return( true );
    }
    return( false );
}
