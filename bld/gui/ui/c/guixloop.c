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
#include "guixutil.h"
#include "uimouse.h"
#include "guiscrol.h"
#include "guixloop.h"
#include "guiwhole.h"
#include "guicontr.h"
#include "guiscale.h"
#include "guifloat.h"
#include "guixkey.h"
#include "guisysme.h"
#include "guixwind.h"
#include "guisize.h"
#include "guismove.h"
#include "guimapky.h"
#include "guixhook.h"
#include "guigadgt.h"
#include "guixinit.h"
#include <stdlib.h>
#include <stdio.h>

/* includes from guixmain.c */
extern  gui_window      *GUICurrWnd;

/* statics */
static  ORD             OldCol          = -1;      /* old column of mouse */
static  ORD             OldRow          = -1;      /* old row of mouse    */

static  gui_window *GUIMouseWnd      = NULL;

static enum {
    MOUSE_FREE,                 /* mouse moving not in press-drag sequence */
    MOUSE_CLIENT,               /* mouse press in client area, no release  */
    MOUSE_MOVE,                 /* window being moved by mouse             */
    MOUSE_SIZE,                 /* window being resized by mouse           */
    MOUSE_MAX_START,            /* mouse press to start maximize           */
    MOUSE_MIN_START,            /* mouse press to start minimize           */
    MOUSE_RESTORE_START         /* mouse press to start restore            */
} MouseState;

EVENT GUIUserEvents[] = {
    GUI_FIRST_USER_EVENT, LAST_EVENT,
    EV_NO_EVENT,
    EV_NO_EVENT
};

static EVENT GUIInternalEvents[] = {
    GUI_FIRST_SYS_MENU, GUI_LAST_SYS_MENU,
    EV_NO_EVENT,
    EV_NO_EVENT
};

static  EVENT           PrevMouseEvent  = EV_NO_EVENT;
static  bool            MinimizedMoved  = false;
static  gui_window      *ButtonDownSent = NULL;

/*
 * SendPointEvent -- send mouse event to use with the point it occured at
 */

static void SendPointEvent( gui_window *wnd, gui_event gui_ev,
                            gui_coord *point )
{
    gui_point   pt;
    bool        down_sent;

    down_sent = ButtonDownSent == wnd;
    switch( gui_ev ) {
    case GUI_LBUTTONDOWN :
    case GUI_RBUTTONDOWN :
        ButtonDownSent = wnd;
        break;
    case GUI_LBUTTONUP :
    case GUI_RBUTTONUP :
        ButtonDownSent = NULL;
        break;
    default :
        break;
    }
    /* if the mouse event is not on the border, or if it is a mouse up on
     * the border
     */
    if( down_sent || ( MouseState == MOUSE_CLIENT ) ) {
        if( ( wnd->hgadget != NULL ) && !GUI_HSCROLL_EVENTS_SET( wnd ) ) {
            point->x += wnd->hgadget->pos;
        }
        if( ( wnd->vgadget != NULL ) && !GUI_VSCROLL_EVENTS_SET( wnd ) ) {
            point->y += wnd->vgadget->pos;
        }
        GUIMakeRelative( wnd, point, &pt );
        GUIEVENTWND( wnd, gui_ev, &pt );
    }
}

static bool ValidMaxRestore( gui_window *wnd, ORD wnd_row, ORD wnd_col )
{
    return( ( wnd->style & GUI_MAXIMIZE ) &&
            GUI_RESIZE_GADGETS_USEABLE( wnd ) &&
            ( wnd_col >= wnd->screen.area.width-MAXOFFSET-1 ) &&
            ( wnd_col <= wnd->screen.area.width-MAXOFFSET+1 ) &&
            ( wnd_row == ( wnd->use.row - 1 ) ) );
}

static bool ValidMin( gui_window *wnd, ORD wnd_row, ORD wnd_col )
{
    return( ( wnd->style & GUI_MINIMIZE ) &&
            GUI_RESIZE_GADGETS_USEABLE( wnd ) &&
            ( wnd_col >= wnd->screen.area.width-MINOFFSET-1 ) &&
            ( wnd_col <= wnd->screen.area.width-MINOFFSET+1 ) &&
            ( wnd_row == ( wnd->use.row - 1 ) ) );
}

/*
 * ProcessMouseReleaseDrag -- take action because mouse was released.
 *                            Also use in the case of drag.
 */

static void ProcessMouseReleaseDrag( EVENT ev, gui_event gui_ev, ORD row,
                                     ORD col )
{
    gui_coord   point;
    ORD         wnd_row;
    ORD         wnd_col;

    OldCol = col;
    OldRow = row;

    if( GUIMouseWnd == NULL ) {
        return;
    }

    wnd_row = row - GUIMouseWnd->screen.area.row;
    wnd_col = col - GUIMouseWnd->screen.area.col;
    switch( MouseState ) {
    case MOUSE_RESTORE_START :
        if( ValidMaxRestore( GUIMouseWnd, wnd_row, wnd_col ) &&
            ( gui_ev == GUI_LBUTTONUP ) ) {
                GUIZoomWnd( GUIMouseWnd, GUI_NONE );
        }
        break;
    case MOUSE_MAX_START :
        if( ValidMaxRestore( GUIMouseWnd, wnd_row, wnd_col ) &&
            ( gui_ev == GUI_LBUTTONUP ) ) {
            GUIZoomWnd( GUIMouseWnd, GUI_MAXIMIZE );
        }
        break;
    case MOUSE_MIN_START :
        if( ValidMin( GUIMouseWnd, wnd_row, wnd_col ) &&
            ( gui_ev == GUI_LBUTTONUP ) ) {
            GUIZoomWnd( GUIMouseWnd, GUI_MINIMIZE );
        }
        break;
    case MOUSE_SIZE:
        if( !( GUIMouseWnd->style & GUI_RESIZEABLE ) ) {
            return;
        }
        /* fall through */
    case MOUSE_MOVE:
        if( ev == EV_MOUSE_RELEASE_R ) {
            return;
        }
        GUIDoMoveResize( GUIMouseWnd, row, col, ev, NULL );
        break;
    default :
        if( GUIMouseWnd != NULL ) {
            point.x = (gui_ord)col;
            point.y = (gui_ord)row;
            SendPointEvent( GUIMouseWnd, gui_ev, &point );
        }
    }
    if( ev == EV_MOUSE_RELEASE || ev == EV_MOUSE_RELEASE_R ) {
        MouseState = MOUSE_FREE;
    }
}

/*
 * ProcessMousePos - if the mouse has move, send the new position and the
 *                   indicated event to the window containing the mouse
 */

static bool ProcessMousePos( gui_event gui_ev, ORD row, ORD col, gui_window * wnd )
{
    gui_coord    point;

    OldCol = col;
    OldRow = row;
    if( wnd == NULL ) {
        return( false );
    }
    point.x = (gui_ord)col;
    point.y = (gui_ord)row;
    SendPointEvent( wnd, gui_ev, &point );
    return( true );
}

EVENT GUICreatePopup( gui_window *wnd, gui_coord *point )
{
    EVENT       ev;

    point->x -= wnd->screen.area.col;
    point->y -= (wnd->screen.area.row - 1);
    uipushlist( NULL );
    uipushlist( GUIInternalEvents );
    uipushlist( GUIUserEvents );
    ev = GUICreateMenuPopup( wnd, point, wnd->menu, GUI_TRACK_LEFT, NULL );
    uipoplist( /* GUIUserEvents */ );
    uipoplist( /* GUIInternalEvents */ );
    uipoplist( /* NULL */ );
    return( ev );
}

static void ProcessMinimizedMouseEvent( EVENT ev, ORD row, ORD col )
{
    gui_coord point;

    switch( ev ) {
    case EV_MOUSE_PRESS :
        if( GUIStartMoveResize( GUIMouseWnd, row, col, RESIZE_NONE ) ) {
            MouseState = MOUSE_MOVE;
        }
        MinimizedMoved = false;
        break;
   case EV_MOUSE_DRAG :
        MinimizedMoved = true;
        GUIDoMoveResize( GUIMouseWnd, row, col, ev, NULL );
        break;
    case EV_MOUSE_RELEASE :
        if( MouseState == MOUSE_MOVE ) {
            GUIDoMoveResize( GUIMouseWnd, row, col, ev, NULL );
            MouseState = MOUSE_FREE;
        }
        if( !MinimizedMoved ) {
            point.x = (gui_ord)col;
            point.y = (gui_ord)row;
            if( GUICreatePopup( GUICurrWnd, &point ) == EV_MOUSE_DCLICK ) {
                GUIZoomWnd( GUICurrWnd, GUI_NONE );
            }
        }
        break;
    case EV_MOUSE_DCLICK :
        GUIZoomWnd( GUICurrWnd, GUI_NONE );
        break;
    }
}

/*
 * ProcessMousePress -- respond to the press of the mouse
 */

static void ProcessMousePress( EVENT ev, gui_event gui_ev, ORD row, ORD col,
                               bool new_curr_wnd )
{
    gui_coord   point;
    ORD         wnd_row;
    ORD         wnd_col;
    resize_dir  dir;
    bool        use_gadgets;

    point.x = (gui_ord)col;
    point.y = (gui_ord)row;
    OldCol = col;
    OldRow = row;
    if( GUICurrWnd == NULL ) {
        return;
    }
    dir = RESIZE_NONE;
    if( MouseState == MOUSE_SIZE || MouseState == MOUSE_MOVE ) {
        return;
    }
    MouseState = MOUSE_FREE; /* default */
    if( !( GUICurrWnd->style & GUI_VISIBLE ) ) {
        return;
    }
    wnd_row = row - GUICurrWnd->screen.area.row;
    wnd_col = col - GUICurrWnd->screen.area.col;
    if( wnd_row < GUICurrWnd->use.row ) {
        use_gadgets = !new_curr_wnd || ( GUIGetWindowStyles() &
                      ( GUI_INACT_GADGETS | GUI_INACT_SAME ) );
        if( use_gadgets && GUI_HAS_CLOSER( GUICurrWnd ) &&
            ( wnd_col >= CLOSER_COL -1 ) && ( wnd_col <= CLOSER_COL+1 ) ) {
            if( ( GUICurrWnd->menu != NULL ) && ( ev == EV_MOUSE_PRESS ) ) {
                point.x = GUICurrWnd->screen.area.col;
                ev = GUICreatePopup( GUICurrWnd, &point );
            }
            if( (GUICurrWnd->style & GUI_CLOSEABLE) && (ev == EV_MOUSE_DCLICK) ) {
                if( GUICloseWnd( GUICurrWnd ) ) {
                    MouseState = MOUSE_FREE;
                }
            }
        } else if( use_gadgets && ValidMaxRestore( GUICurrWnd, wnd_row, wnd_col ) ) {
            if( GUI_WND_MAXIMIZED( GUICurrWnd ) ) {
                MouseState = MOUSE_RESTORE_START;
            } else {
                MouseState = MOUSE_MAX_START;
            }
        } else if( use_gadgets && ValidMin( GUICurrWnd, wnd_row, wnd_col ) &&
                   ( ev == EV_MOUSE_PRESS ) ) {
            MouseState = MOUSE_MIN_START;
        } else if( (GUICurrWnd->style & GUI_RESIZEABLE) && (ev == EV_MOUSE_PRESS) &&
                   ( ( wnd_col == 0) || (wnd_col == GUICurrWnd->screen.area.width-1) ) ) {
            dir = RESIZE_UP;
        } else if( ( ev == EV_MOUSE_DCLICK ) || ( ev == EV_MOUSE_PRESS ) ) {
            if( GUIStartMoveResize( GUICurrWnd, row, col, RESIZE_NONE ) ) {
                MouseState = MOUSE_MOVE;
            }
        }
    } else if( GUIPtInRect( &GUICurrWnd->use, wnd_row, wnd_col ) ) {
        MouseState = MOUSE_CLIENT;
        SendPointEvent( GUICurrWnd, gui_ev, &point );
    } else if( ( GUICurrWnd->style & GUI_RESIZEABLE ) && ( ev == EV_MOUSE_PRESS ) &&
               ( wnd_row == GUICurrWnd->screen.area.height-1 ) &&
               ( ( wnd_col == 0 ) || ( wnd_col == GUICurrWnd->screen.area.width-1 ) ) ) {
        dir = RESIZE_DOWN;
    }
    if( dir != RESIZE_NONE ) {
        if( wnd_col == 0 ) {
            dir |= RESIZE_LEFT;
        } else {
            dir |= RESIZE_RIGHT;
        }
        if( GUIStartMoveResize( GUICurrWnd, row, col, dir ) ) {
            MouseState = MOUSE_SIZE;
        }
    }
    GUIMouseWnd = GUICurrWnd;
}

static void ProcessInitPopupEvent( void )
{
    MENUITEM menu;
    unsigned id;

    if( uigetcurrentmenu ( &menu ) ) {
        id = (unsigned) menu.event - GUI_FIRST_USER_EVENT;
        if ( id ) {
            GUIEVENTWND( GUICurrWnd, GUI_INITMENUPOPUP, &id );
        }
    }
}

static void ProcessScrollEvent( EVENT ev  )
{
    gui_event   gui_ev;
    p_gadget    gadget;
    int         diff;
    bool        events;
    gui_event   notify;

    switch( ev ) {
    case EV_SCROLL_UP :
        events = GUICurrWnd->style & GUI_VSCROLL_EVENTS;
        gui_ev = GUI_SCROLL_UP;
        diff = -1;
        gadget = GUICurrWnd->vgadget;
        break;
    case EV_SCROLL_DOWN :
        events = GUICurrWnd->style & GUI_VSCROLL_EVENTS;
        gui_ev = GUI_SCROLL_DOWN;
        diff = 1;
        gadget = GUICurrWnd->vgadget;
        break;
    case EV_SCROLL_PAGE_UP :
        events = GUICurrWnd->style & GUI_VSCROLL_EVENTS;
        gui_ev = GUI_SCROLL_PAGE_UP;
        diff = -GUICurrWnd->use.height;
        gadget = GUICurrWnd->vgadget;
        break;
    case EV_SCROLL_PAGE_DOWN :
        events = GUICurrWnd->style & GUI_VSCROLL_EVENTS;
        gui_ev = GUI_SCROLL_PAGE_DOWN;
        diff = GUICurrWnd->use.height;
        gadget = GUICurrWnd->vgadget;
        break;
    case EV_SCROLL_LEFT :
        events = GUICurrWnd->style & GUI_HSCROLL_EVENTS;
        gui_ev = GUI_SCROLL_LEFT;
        diff = -1;
        gadget = GUICurrWnd->hgadget;
        break;
    case EV_SCROLL_RIGHT :
        events = GUICurrWnd->style & GUI_HSCROLL_EVENTS;
        gui_ev = GUI_SCROLL_RIGHT;
        diff = 1;
        gadget = GUICurrWnd->hgadget;
        break;
    case EV_SCROLL_LEFT_PAGE :
        events = GUICurrWnd->style & GUI_HSCROLL_EVENTS;
        gui_ev = GUI_SCROLL_PAGE_LEFT;
        diff = -GUICurrWnd->use.width;
        gadget = GUICurrWnd->hgadget;
        break;
    case EV_SCROLL_RIGHT_PAGE :
        events = GUICurrWnd->style & GUI_HSCROLL_EVENTS;
        gui_ev = GUI_SCROLL_PAGE_RIGHT;
        diff = GUICurrWnd->use.width;
        gadget = GUICurrWnd->hgadget;
        break;
    }
    if( events ) {
        GUIEVENTWND( GUICurrWnd, gui_ev, NULL );
    } else {
        GUIScroll( diff, gadget );
        if( gadget->dir == HORIZONTAL ) {
            notify = GUI_HSCROLL_NOTIFY;
        } else {
            notify = GUI_VSCROLL_NOTIFY;
        }
        GUIEVENTWND( GUICurrWnd, notify, NULL );
    }
}

static bool SetCurrWnd( EVENT ev, gui_window *curr )
{
    switch( ev ) {
    case EV_MOUSE_DCLICK_R :
    case EV_MOUSE_PRESS_R :
    case EV_MOUSE_PRESS :
    case EV_MOUSE_DCLICK :
    case EV_SCROLL_UP :
    case EV_SCROLL_DOWN :
    case EV_SCROLL_PAGE_UP :
    case EV_SCROLL_PAGE_DOWN :
    case EV_SCROLL_LEFT :
    case EV_SCROLL_RIGHT :
    case EV_PAGE_LEFT :
    case EV_PAGE_RIGHT :
    case EV_SCROLL_VERTICAL :
    case EV_SCROLL_HORIZONTAL :
        if( ( curr != GUICurrWnd ) && ( curr != NULL ) ) {
            GUIBringToFront( curr );
            return( true );
        }
        break;
    }
    return( false );
}

gui_window *GUIGetMenuWindow( void )
{
    gui_window *menu_window;

    menu_window = GUIGetFront();
    if( menu_window != NULL ) {
        menu_window = GUIGetTopWnd( menu_window );
        if( menu_window->vbarmenu == NULL ) {
            menu_window = NULL;
        }
    }
    return( menu_window );
}

static void DoScrollDrag( p_gadget gadget, int prev, int diff )
{
    gui_event top;
    gui_event bottom;
    gui_event scroll;

    prev = prev;
    if( gadget->dir == VERTICAL ) {
        top = GUI_SCROLL_TOP;
        bottom = GUI_SCROLL_BOTTOM;
        scroll = GUI_SCROLL_VERTICAL;
    } else {
        top = GUI_SCROLL_FULL_LEFT;
        bottom = GUI_SCROLL_FULL_RIGHT;
        scroll = GUI_SCROLL_HORIZONTAL;
    }
    uisetgadgetnodraw( gadget, gadget->pos - diff );
    if( prev + diff == 0 ) {
        GUIEVENTWND( GUICurrWnd, top, NULL );
    } else if( ( prev + diff ) == ( gadget->total_size - gadget->page_size ) ) {
        GUIEVENTWND( GUICurrWnd, bottom, NULL );
    } else {
        GUIEVENTWND( GUICurrWnd, scroll, &diff );
    }
}

/*
 * CheckPrevEvent -- this routine causes the sequence of mouse events :
 *                   P-U-D-U-D-U-D-U to come out P-U-D-U P-U-D-U
 *                   where P is Press, U is Up and D is dclick.
 *                   This is for consistency with windows.
 */

static EVENT CheckPrevEvent( EVENT ev )
{
    switch( ev ) {
    case EV_MOUSE_PRESS :
    case EV_MOUSE_PRESS_R :
        PrevMouseEvent = ev;
        break;
    case EV_MOUSE_DCLICK :
        if( ev == PrevMouseEvent ) {
            ev = EV_MOUSE_PRESS;
        }
        PrevMouseEvent = ev;
        break;
    case EV_MOUSE_DCLICK_R :
        if( ev == PrevMouseEvent ) {
            ev = EV_MOUSE_PRESS_R;
        }
        PrevMouseEvent = ev;
    }
    return( ev );
}

static EVENT MapMiddleToRight( EVENT ev )
{
    switch( ev ) {
    case EV_MOUSE_PRESS_M :
        ev = EV_MOUSE_PRESS_R;
        break;
    case EV_MOUSE_DRAG_M :
        ev = EV_MOUSE_DRAG_R;
        break;
    case EV_MOUSE_RELEASE_M :
        ev = EV_MOUSE_RELEASE_R;
        break;
    case EV_MOUSE_DCLICK_M :
        ev = EV_MOUSE_DCLICK_R;
        break;
    case EV_MOUSE_REPEAT_M :
        ev = EV_MOUSE_REPEAT_R;
        break;
    case EV_MOUSE_HOLD_M :
        ev = EV_MOUSE_HOLD_R;
        break;
    }
    return( ev );
}

/*
 * GUIProcessEvent -- Main event loop to process UI events
 */

bool GUIProcessEvent( EVENT ev )
{
    gui_event   gui_ev;
    ORD         row, col;
    gui_window  *wnd;
    int         prev;
    int         diff;
    unsigned    id;
    gui_window  *menu_window;
    bool        new_curr_wnd;
    VSCREEN     *screen;

    // this is processed before all others and signals the end for all
    // GUI UI windows ( unconditional )
    if( ev == EV_KILL_UI ) {
        GUIDestroyWnd( NULL );
        return( false );
    }

    ev = MapMiddleToRight( ev );
    ev = CheckPrevEvent( ev );
    wnd = NULL;
    if( uimouseinstalled() ) {
        screen = uivmousepos( NULL, &row, &col );
        if( screen != NULL && (screen->flags & V_GUI_WINDOW) != 0 ) {
            wnd = (gui_window *)((char *)screen - offsetof( gui_window, screen ));
        }
    }
    if( GUIDoKeyboardMoveResize( ev ) ) {
        return( true );
    }
    if( MouseState == MOUSE_MOVE || MouseState == MOUSE_SIZE ) {
        if( GUIDoMoveResizeCheck( GUIMouseWnd, ev, row, col ) ) {
            MouseState = MOUSE_FREE;
            return( true );
        }
        if( GUI_WND_MINIMIZED( GUIMouseWnd ) ) {
            switch( ev ) {
            case EV_MOUSE_DCLICK :
            case EV_MOUSE_RELEASE :
            case EV_MOUSE_DRAG :
                ProcessMinimizedMouseEvent( ev, row, col );
            }
        } else {
            switch( ev ) {
            case EV_MOUSE_RELEASE :
            case EV_MOUSE_DRAG :
            case EV_MOUSE_DRAG_R :
                ProcessMouseReleaseDrag( ev, GUI_LBUTTONUP, row, col );
            }
        }
        return( true );
    }
    new_curr_wnd = SetCurrWnd( ev, wnd );
    if( GUIProcessAltMenuEvent( ev ) ) {
        return( true );
    }
    /* Only deal with press and dclick events for minimized windows.
     * All other non-menu events are ingored.
     */
    if( ( ev < GUI_FIRST_USER_EVENT ) && ( GUICurrWnd != NULL ) &&
        GUI_WND_MINIMIZED( GUICurrWnd ) ) {
        /* ignore event if mouse not in minimized current window */
        if( GUICurrWnd == wnd ) {
            switch( ev ) {
            case EV_MOUSE_PRESS :
            case EV_MOUSE_DCLICK :
            case EV_MOUSE_RELEASE :
                GUIMouseWnd = GUICurrWnd;
                ProcessMinimizedMouseEvent( ev, row, col );
                break;
            }
        }
        return( true );
    }
    if( ( GUICurrWnd != NULL ) && GUIIsOpen( GUICurrWnd ) && ( ev < GUI_FIRST_USER_EVENT ) ) {
        /* see if any of the controls in the window consume the event */
        ev = GUIProcessControlEvent( GUICurrWnd, ev, row, col );
        /* See if the event is for on of the scroll bars. */
        /* Diff and prev are used if the event return is  */
        /* EV_SCROLL_HORIZONTAL or EV_SCROLL_VERTICAL.    */
        if( !new_curr_wnd || ( GUIGetWindowStyles() & ( GUI_INACT_GADGETS+GUI_INACT_SAME ) ) ) {
            ev = GUIGadgetFilter( GUICurrWnd, ev, &prev, &diff );
        }
        if( ev == EV_NO_EVENT ) {
            return( true );
        }
    }
    gui_ev = GUI_NO_EVENT;
    ev = GUIMapKeys( ev );
    switch( ev ) {
    case EV_MOUSE_DCLICK_R :
        ProcessMousePos( GUI_RBUTTONDBLCLK, row, col, wnd );
        return( true );
        break;
    case EV_MOUSE_RELEASE_R :
        ProcessMouseReleaseDrag( ev, GUI_RBUTTONUP, row, col );
        return( true );
        break;
    case EV_MOUSE_DRAG_R :
        if( GUICurrWnd != GUIMouseWnd ) {
            /* got drag without press first */
            ProcessMousePress( EV_MOUSE_PRESS_R, GUI_LBUTTONDOWN, row, col,
                               new_curr_wnd );
        }
    case EV_MOUSE_MOVE :
        ProcessMousePos( GUI_MOUSEMOVE, row, col, wnd );
        return( true );
        break;
    case EV_MOUSE_RELEASE :
        ProcessMouseReleaseDrag( ev, GUI_LBUTTONUP, row, col );
        return( true );
        break;
    case EV_MOUSE_DRAG :
        if( GUICurrWnd != GUIMouseWnd ) {
            /* got drag without press first */
            ProcessMousePress( EV_MOUSE_PRESS, GUI_LBUTTONDOWN, row, col,
                               new_curr_wnd );
        }
        ProcessMouseReleaseDrag( ev, GUI_MOUSEMOVE, row, col );
        return( true );
        break;
    case EV_MOUSE_PRESS_R :
        ProcessMousePress( ev, GUI_RBUTTONDOWN, row, col, new_curr_wnd );
        return( true );
        break;
    case EV_MOUSE_PRESS :
        ProcessMousePress( ev, GUI_LBUTTONDOWN, row, col, new_curr_wnd );
        return( true );
        break;
    case EV_MOUSE_DCLICK :
        ProcessMousePress( ev, GUI_LBUTTONDBLCLK, row, col, new_curr_wnd );
        return( true );
        break;
    case EV_NO_EVENT :
        gui_ev = GUI_NO_EVENT;
        break;
    case EV_SCROLL_UP :
    case EV_SCROLL_DOWN :
    case EV_SCROLL_PAGE_UP :
    case EV_SCROLL_PAGE_DOWN :
    case EV_SCROLL_LEFT :
    case EV_SCROLL_RIGHT :
    case EV_SCROLL_LEFT_PAGE :
    case EV_SCROLL_RIGHT_PAGE :
        if( GUICurrWnd != NULL ) {
            ProcessScrollEvent( ev );
            return( true );
        }
        break;
    case EV_SCROLL_VERTICAL :
        if( GUI_VSCROLL_EVENTS_SET( GUICurrWnd ) ) {
            DoScrollDrag( GUICurrWnd->vgadget, prev, diff );
        } else {
            GUIWholeWndDirty( GUICurrWnd );
        }
        return( true );
        break;
    case EV_SCROLL_HORIZONTAL :
        if( GUI_HSCROLL_EVENTS_SET( GUICurrWnd ) ) {
            DoScrollDrag( GUICurrWnd->hgadget, prev, diff );
        } else {
            GUIWholeWndDirty( GUICurrWnd );
        }
        return( true );
        break;
    case EV_MENU_INITPOPUP :
        ProcessInitPopupEvent();
        return( true );
        break;
    #if 0
    case EV_BACKGROUND_RESIZE :
        {
            gui_window          *root;
            root = GUIGetRootWindow();
            if( root != NULL ) {
                GUIZoomWnd( root, GUI_NONE );
            }
        }
        return( true );
        break;
    #endif
    default :
        if( ev >= GUI_FIRST_USER_EVENT ) {
            if( !GUIMDIProcessEvent( ev ) ) {
                menu_window = GUIGetMenuWindow();
                if( menu_window != NULL ) {
                    id = ev - GUI_FIRST_USER_EVENT;
                    GUIEVENTWND( menu_window, GUI_CLICKED, &id );
                }
            }
            return( true );
        }
        break;
    }
    if( ( GUICurrWnd != NULL ) && (gui_ev != GUI_NO_EVENT ) ) {
        GUIEVENTWND( GUICurrWnd, gui_ev, NULL );
    }
    return( true );
}
