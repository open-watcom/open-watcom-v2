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
#include "guix.h"
#include "guiscale.h"
#include "guimenu.h"
#include "guisysme.h"
#include "guixloop.h"
#include "guixutil.h"
#include "guifloat.h"
#include <ctype.h>
#include <string.h>

typedef enum {
    MENU_NONE,
    MENU_FLOAT,
    MENU_SYS
} MenuStatus;

static  MenuStatus      MenuState       = MENU_NONE;
static  gui_window      *MenuWnd        = NULL;
        MENUITEM        *GUIPopupMenu   = NULL;

extern EVENT GUIUserEvents[];

/*
 * MapLocation --
 */

static void MapLocation( gui_window *wnd, gui_point *point )
{
    GUIScaleToScreenRPt( point );
    if( ( wnd->hgadget != NULL ) && !GUI_HSCROLL_EVENTS_SET( wnd ) ) {
        point->x -= wnd->hgadget->pos;
    }
    if( ( wnd->vgadget != NULL ) && !GUI_VSCROLL_EVENTS_SET( wnd ) ) {
        point->y -= wnd->vgadget->pos;
    }
    point->x += wnd->use.col + 1;
    point->y += wnd->use.row + 1;
}

/*
 * GUIProcessMenuCurr
 */

void GUIProcessMenuCurr( MENUITEM *menu )
{
    gui_window          *top_wnd;
    gui_ctl_id          id;
    gui_window          *menu_wnd;
    gui_menu_styles     style;
    hint_type           type;

    id = 0;
    if( ( menu == NULL ) || ( menu->event == EV_NO_EVENT ) ) {
        style = GUI_IGNORE;
    } else {
        if( menu->flags & ITEM_SEPARATOR ) {
            style = GUI_SEPARATOR;
        } else {
            if( IS_CTLEVENT( menu->event ) ) {
                id = EV2ID( menu->event );
                if( menu->flags & ITEM_GRAYED ) {
                    style = GUI_GRAYED;
                } else {
                    style = GUI_ENABLED;
                }
            } else {
                style = GUI_IGNORE;
            }
        }
    }
    type = MENU_HINT;
    top_wnd = GUIGetMenuWindow();
    menu_wnd = NULL;
    switch( MenuState ) {
    case MENU_NONE :
        menu_wnd = top_wnd;
        if( top_wnd != NULL && GUIHasToolBar( top_wnd ) && GUIToolBarFixed( top_wnd ) ) {
            if( GUIHasHintText( menu_wnd, id, TOOL_HINT ) ) {
                type = TOOL_HINT;
            }
        }
        break;
    case MENU_FLOAT :
        type = FLOAT_HINT;
    case MENU_SYS :
        menu_wnd = MenuWnd;
    }
    if( ( top_wnd != NULL ) && ( menu_wnd != NULL ) ) {
        GUIDisplayHintText( top_wnd, menu_wnd, id, type, style );
    }
}

/*
 * GUICreateMenuPopup - create a floating popup menu
 */

EVENT GUICreateMenuPopup( gui_window *wnd, gui_point *location,
                          MENUITEM *menu, gui_mouse_track track,
                          gui_ctl_id *curr_id )
{
    EVENT       ev;
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

    MenuWnd = wnd;
    if( MenuState == MENU_NONE ) {
        MenuState = MENU_SYS;
    }
    if( menu == NULL ) {
        return( EV_NO_EVENT );
    }
    attr_active         = UIData->attrs[ATTR_ACTIVE];
    attr_hot            = UIData->attrs[ATTR_HOT];
    attr_curr_active    = UIData->attrs[ATTR_CURR_ACTIVE];
    attr_hot_curr       = UIData->attrs[ATTR_HOT_CURR];
    attr_inactive       = UIData->attrs[ATTR_INACTIVE];
    attr_curr_inactive  = UIData->attrs[ATTR_CURR_INACTIVE];
    attr_menu           = UIData->attrs[ATTR_MENU];

    UIData->attrs[ATTR_ACTIVE]          = wnd->colours[GUI_MENU_PLAIN];
    UIData->attrs[ATTR_HOT]             = wnd->colours[GUI_MENU_STANDOUT];
    UIData->attrs[ATTR_CURR_ACTIVE]     = wnd->colours[GUI_MENU_ACTIVE];
    UIData->attrs[ATTR_HOT_CURR]        = wnd->colours[GUI_MENU_ACTIVE_STANDOUT];
    UIData->attrs[ATTR_INACTIVE]        = wnd->colours[GUI_MENU_GRAYED];
    UIData->attrs[ATTR_CURR_INACTIVE]   = wnd->colours[GUI_MENU_GRAYED_ACTIVE];
    UIData->attrs[ATTR_MENU]            = wnd->colours[GUI_MENU_FRAME];

    ev = 0;
    if( ( curr_id != NULL ) && ( *curr_id != 0 ) ) {
        ev = ID2EV( *curr_id );
    }
    top = GUIGetTopWnd( wnd );
    COPYAREA( top->use, area );
    area.row += top->screen.area.row;
    area.col += top->screen.area.col;

    if( !uiposfloatingpopup( menu, &desc, wnd->screen.area.row + location->y,
                            wnd->screen.area.col + location->x, &area, NULL ) ) {
        return( EV_NO_EVENT );
    }
    ev = uicreatepopupinarea( menu, &desc, track & GUI_TRACK_LEFT,
                              track & GUI_TRACK_RIGHT, ev, &area, false );

    if( ev == EV_KILL_UI ) {
        uiforceevadd( EV_KILL_UI );
        ev = EV_NO_EVENT;
    }

    GUIProcessMenuCurr( NULL );

    UIData->attrs[ATTR_ACTIVE]          = attr_active;
    UIData->attrs[ATTR_HOT]             = attr_hot;
    UIData->attrs[ATTR_CURR_ACTIVE]     = attr_curr_active;
    UIData->attrs[ATTR_HOT_CURR]        = attr_hot_curr;
    UIData->attrs[ATTR_INACTIVE]        = attr_inactive;
    UIData->attrs[ATTR_CURR_INACTIVE]   = attr_curr_inactive;
    UIData->attrs[ATTR_MENU]            = attr_menu;

    if( ( ev != EV_MOUSE_DCLICK ) && ( ev != EV_NO_EVENT ) ) {
        if( IS_CTLEVENT( ev ) ) {
            id = EV2ID( ev );
            GUIEVENTWND( wnd, GUI_CLICKED, &id );
            if( curr_id != NULL ) {
                *curr_id = id;
            }
        } else {
            GUIProcessSysMenuEvent( ev, wnd );
        }
    }
    if( MenuState == MENU_SYS ) {
        MenuState = MENU_NONE;
    }
    MenuWnd = NULL;

    return( ev );
}

static void TrackPopup( gui_window *wnd, gui_point *location,
                        MENUITEM *new_menu, gui_mouse_track track, gui_ctl_id *curr_id )
{
    MapLocation( wnd, location );

    MenuState = MENU_FLOAT;
    uipushlist( NULL );
    uipushlist( GUIUserEvents );
    GUICreateMenuPopup( wnd, location, new_menu, track, curr_id );
    uipoplist( /* GUIUserEvents */ );
    uipoplist( /* NULL */ );
    MenuState = MENU_NONE;
    GUIFreeMenuItems( new_menu );
}

/*
 * GUIXCreateFloatingPopup - create a floating popup menu
 */

bool GUIXCreateFloatingPopup( gui_window *wnd, gui_point *location,
                             int num_menu_items, gui_menu_struct *menu,
                             gui_mouse_track track, gui_ctl_id *curr_id )
{
    MENUITEM    *new_menu;

    if( !GUICreateMenuItems( num_menu_items, menu, &new_menu ) ) {
        return( false );
    }
    TrackPopup( wnd, location, new_menu, track, curr_id );
    return( true );
}

bool GUITrackFloatingPopup( gui_window *wnd, gui_point *location,
                        gui_mouse_track track, gui_ctl_id *curr_id )
{
    if( GUIPopupMenu != NULL ) {
        TrackPopup( wnd, location, GUIPopupMenu, track, curr_id );
        GUIPopupMenu = NULL;
        return( true );
    }
    return( false );
}

