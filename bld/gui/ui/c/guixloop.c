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
#include "guixutil.h"
#include "guiscrol.h"
#include "guixloop.h"
#include "guiwhole.h"
#include "guicontr.h"
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
#include "guievent.h"
#include <stdlib.h>
#include <stdio.h>


ui_event GUIUserEvents[] = {
    GUI_FIRST_USER_EVENT, LAST_EVENT,
    __rend__,
    __end__
};

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

static ui_event GUIInternalEvents[] = {
    EV_SYS_MENU_FIRST, EV_SYS_MENU_LAST,
    __rend__,
    __end__
};

static  ui_event        PrevMouseEvent  = EV_NO_EVENT;
static  bool            MinimizedMoved  = false;
static  gui_window      *ButtonDownSent = NULL;

/*
 * sendPointGUIEvent -- send mouse event to use with the point it occured at
 */

static void sendPointGUIEvent( gui_window *wnd, gui_event gui_ev, const guix_point *scr_point )
{
    bool        down_sent;

    down_sent = ( ButtonDownSent == wnd );
    switch( gui_ev ) {
    case GUI_LBUTTONDOWN:
    case GUI_RBUTTONDOWN:
        ButtonDownSent = wnd;
        break;
    case GUI_LBUTTONUP:
    case GUI_RBUTTONUP:
        ButtonDownSent = NULL;
        break;
    default:
        break;
    }
    /* if the mouse event is not on the border, or if it is a mouse up on
     * the border
     */
    if( down_sent || ( MouseState == MOUSE_CLIENT ) ) {
        gui_point   point;

        GUIMakeRelative( wnd, scr_point, &point );
        GUIEVENT( wnd, gui_ev, &point );
    }
}

static bool ValidMaxRestore( gui_window *wnd, ORD wnd_row, ORD wnd_col )
{
    return( (wnd->style & GUI_MAXIMIZE) &&
            GUI_RESIZE_GADGETS_USEABLE( wnd ) &&
            ( wnd_col >= wnd->vs.area.width - MAXOFFSET - 1 ) &&
            ( wnd_col <= wnd->vs.area.width - MAXOFFSET + 1 ) &&
            ( wnd_row == ( wnd->use.row - 1 ) ) );
}

static bool ValidMin( gui_window *wnd, ORD wnd_row, ORD wnd_col )
{
    return( (wnd->style & GUI_MINIMIZE) &&
            GUI_RESIZE_GADGETS_USEABLE( wnd ) &&
            ( wnd_col >= wnd->vs.area.width - MINOFFSET - 1 ) &&
            ( wnd_col <= wnd->vs.area.width - MINOFFSET + 1 ) &&
            ( wnd_row == ( wnd->use.row - 1 ) ) );
}

/*
 * ProcessMouseReleaseDrag -- take action because mouse was released.
 *                            Also use in the case of drag.
 */

static void ProcessMouseReleaseDrag( ui_event ui_ev, gui_event gui_ev, ORD row, ORD col )
{
    ORD         wnd_row;
    ORD         wnd_col;

    OldCol = col;
    OldRow = row;

    if( GUIMouseWnd == NULL ) {
        return;
    }

    wnd_row = row - GUIMouseWnd->vs.area.row;
    wnd_col = col - GUIMouseWnd->vs.area.col;
    switch( MouseState ) {
    case MOUSE_RESTORE_START:
        if( ValidMaxRestore( GUIMouseWnd, wnd_row, wnd_col ) && ( gui_ev == GUI_LBUTTONUP ) ) {
            GUIZoomWnd( GUIMouseWnd, GUI_NONE );
        }
        break;
    case MOUSE_MAX_START:
        if( ValidMaxRestore( GUIMouseWnd, wnd_row, wnd_col ) && ( gui_ev == GUI_LBUTTONUP ) ) {
            GUIZoomWnd( GUIMouseWnd, GUI_MAXIMIZE );
        }
        break;
    case MOUSE_MIN_START:
        if( ValidMin( GUIMouseWnd, wnd_row, wnd_col ) && ( gui_ev == GUI_LBUTTONUP ) ) {
            GUIZoomWnd( GUIMouseWnd, GUI_MINIMIZE );
        }
        break;
    case MOUSE_SIZE:
        if( (GUIMouseWnd->style & GUI_RESIZEABLE) == 0 ) {
            return;
        }
        /* fall through */
    case MOUSE_MOVE:
        if( ui_ev == EV_MOUSE_RELEASE_R ) {
            return;
        }
        GUIDoMoveResize( GUIMouseWnd, row, col, ui_ev, NULL );
        break;
    default:
        if( GUIMouseWnd != NULL ) {
            guix_point  scr_point;

            scr_point.x = col;
            scr_point.y = row;
            sendPointGUIEvent( GUIMouseWnd, gui_ev, &scr_point );
        }
    }
    if( ui_ev == EV_MOUSE_RELEASE || ui_ev == EV_MOUSE_RELEASE_R ) {
        MouseState = MOUSE_FREE;
    }
}

/*
 * ProcessMousePos - if the mouse has move, send the new position and the
 *                   indicated event to the window containing the mouse
 */

static bool ProcessMousePos( gui_event gui_ev, ORD row, ORD col, gui_window * wnd )
{
    guix_point  scr_point;

    OldCol = col;
    OldRow = row;
    if( wnd == NULL ) {
        return( false );
    }
    scr_point.x = col;
    scr_point.y = row;
    sendPointGUIEvent( wnd, gui_ev, &scr_point );
    return( true );
}

ui_event GUICreatePopup( gui_window *wnd, const guix_coord *scr_point )
{
    ui_event    ui_ev;
    guix_coord  scr_location;

    scr_location.x = scr_point->x - wnd->vs.area.col;
    scr_location.y = scr_point->y - (wnd->vs.area.row - 1);
    uipushlist( NULL );
    uipushlist( GUIInternalEvents );
    uipushlist( GUIUserEvents );
    ui_ev = GUICreateMenuPopup( wnd, &scr_location, wnd->menu, GUI_TRACK_LEFT, NULL );
    uipoplist( /* GUIUserEvents */ );
    uipoplist( /* GUIInternalEvents */ );
    uipoplist( /* NULL */ );
    return( ui_ev );
}

static void ProcessMinimizedMouseEvent( ui_event ui_ev, ORD row, ORD col )
{
    switch( ui_ev ) {
    case EV_MOUSE_PRESS:
        if( GUIStartMoveResize( GUIMouseWnd, row, col, RESIZE_NONE ) ) {
            MouseState = MOUSE_MOVE;
        }
        MinimizedMoved = false;
        break;
    case EV_MOUSE_DRAG:
        MinimizedMoved = true;
        GUIDoMoveResize( GUIMouseWnd, row, col, ui_ev, NULL );
        break;
    case EV_MOUSE_RELEASE:
        if( MouseState == MOUSE_MOVE ) {
            GUIDoMoveResize( GUIMouseWnd, row, col, ui_ev, NULL );
            MouseState = MOUSE_FREE;
        }
        if( !MinimizedMoved ) {
            guix_coord  scr_point;

            scr_point.x = col;
            scr_point.y = row;
            if( GUICreatePopup( GUICurrWnd, &scr_point ) == EV_MOUSE_DCLICK ) {
                GUIZoomWnd( GUICurrWnd, GUI_NONE );
            }
        }
        break;
    case EV_MOUSE_DCLICK:
        GUIZoomWnd( GUICurrWnd, GUI_NONE );
        break;
    }
}

/*
 * ProcessMousePress -- respond to the press of the mouse
 */

static void ProcessMousePress( ui_event ui_ev, gui_event gui_ev, ORD row, ORD col, bool new_curr_wnd )
{
    ORD         wnd_row;
    ORD         wnd_col;
    resize_dir  dir;
    bool        use_gadgets;

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
    if( (GUICurrWnd->style & GUI_VISIBLE) == 0 ) {
        return;
    }
    wnd_row = row - GUICurrWnd->vs.area.row;
    wnd_col = col - GUICurrWnd->vs.area.col;
    if( wnd_row < GUICurrWnd->use.row ) {
        use_gadgets = ( !new_curr_wnd || (GUIGetWindowStyles() & (GUI_INACT_GADGETS | GUI_INACT_SAME)) );
        if( use_gadgets && GUI_HAS_CLOSER( GUICurrWnd ) &&
            ( wnd_col >= CLOSER_COL - 1 ) && ( wnd_col <= CLOSER_COL + 1 ) ) {
            if( ( GUICurrWnd->menu != NULL ) && ( ui_ev == EV_MOUSE_PRESS ) ) {
                guix_coord  scr_point;

                scr_point.x = GUICurrWnd->vs.area.col;
                scr_point.y = row;
                ui_ev = GUICreatePopup( GUICurrWnd, &scr_point );
            }
            if( (GUICurrWnd->style & GUI_CLOSEABLE) && (ui_ev == EV_MOUSE_DCLICK) ) {
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
        } else if( use_gadgets && ValidMin( GUICurrWnd, wnd_row, wnd_col ) && ( ui_ev == EV_MOUSE_PRESS ) ) {
            MouseState = MOUSE_MIN_START;
        } else if( (GUICurrWnd->style & GUI_RESIZEABLE) && (ui_ev == EV_MOUSE_PRESS) &&
                   ( ( wnd_col == 0) || (wnd_col == GUICurrWnd->vs.area.width - 1) ) ) {
            dir = RESIZE_UP;
        } else if( ( ui_ev == EV_MOUSE_DCLICK ) || ( ui_ev == EV_MOUSE_PRESS ) ) {
            if( GUIStartMoveResize( GUICurrWnd, row, col, RESIZE_NONE ) ) {
                MouseState = MOUSE_MOVE;
            }
        }
    } else if( GUIPtInRect( &GUICurrWnd->use, wnd_row, wnd_col ) ) {
        guix_point  scr_point;

        MouseState = MOUSE_CLIENT;
        scr_point.x = col;
        scr_point.y = row;
        sendPointGUIEvent( GUICurrWnd, gui_ev, &scr_point );
    } else if( (GUICurrWnd->style & GUI_RESIZEABLE) && ( ui_ev == EV_MOUSE_PRESS ) &&
               ( wnd_row == GUICurrWnd->vs.area.height - 1 ) &&
               ( ( wnd_col == 0 ) || ( wnd_col == GUICurrWnd->vs.area.width - 1 ) ) ) {
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
    UIMENUITEM  currmenuitem;
    gui_ctl_id  id;

    if( uigetcurrentmenu( &currmenuitem ) ) {
        id = EV2ID( currmenuitem.event );
        if( id != 0 ) {
            GUIEVENT( GUICurrWnd, GUI_INITMENUPOPUP, &id );
        }
    }
}

static void ProcessScrollEvent( ui_event ui_ev  )
{
    gui_event   gui_ev;
    p_gadget    gadget;
    int         diff;
    bool        events;
    gui_event   notify;

    switch( ui_ev ) {
    case EV_SCROLL_UP:
        events = ( (GUICurrWnd->style & GUI_VSCROLL_EVENTS) != 0 );
        gui_ev = GUI_SCROLL_UP;
        diff = -1;
        gadget = GUICurrWnd->vgadget;
        break;
    case EV_SCROLL_DOWN:
        events = ( (GUICurrWnd->style & GUI_VSCROLL_EVENTS) != 0 );
        gui_ev = GUI_SCROLL_DOWN;
        diff = 1;
        gadget = GUICurrWnd->vgadget;
        break;
    case EV_SCROLL_PAGE_UP:
        events = ( (GUICurrWnd->style & GUI_VSCROLL_EVENTS) != 0 );
        gui_ev = GUI_SCROLL_PAGE_UP;
        diff = -GUICurrWnd->use.height;
        gadget = GUICurrWnd->vgadget;
        break;
    case EV_SCROLL_PAGE_DOWN:
        events = ( (GUICurrWnd->style & GUI_VSCROLL_EVENTS) != 0 );
        gui_ev = GUI_SCROLL_PAGE_DOWN;
        diff = GUICurrWnd->use.height;
        gadget = GUICurrWnd->vgadget;
        break;
    case EV_SCROLL_LEFT:
        events = ( (GUICurrWnd->style & GUI_HSCROLL_EVENTS) != 0 );
        gui_ev = GUI_SCROLL_LEFT;
        diff = -1;
        gadget = GUICurrWnd->hgadget;
        break;
    case EV_SCROLL_RIGHT:
        events = ( (GUICurrWnd->style & GUI_HSCROLL_EVENTS) != 0 );
        gui_ev = GUI_SCROLL_RIGHT;
        diff = 1;
        gadget = GUICurrWnd->hgadget;
        break;
    case EV_SCROLL_LEFT_PAGE:
        events = ( (GUICurrWnd->style & GUI_HSCROLL_EVENTS) != 0 );
        gui_ev = GUI_SCROLL_PAGE_LEFT;
        diff = -GUICurrWnd->use.width;
        gadget = GUICurrWnd->hgadget;
        break;
    case EV_SCROLL_RIGHT_PAGE:
        events = ( (GUICurrWnd->style & GUI_HSCROLL_EVENTS) != 0 );
        gui_ev = GUI_SCROLL_PAGE_RIGHT;
        diff = GUICurrWnd->use.width;
        gadget = GUICurrWnd->hgadget;
        break;
    default:
        return;
    }
    if( events ) {
        GUIEVENT( GUICurrWnd, gui_ev, NULL );
    } else {
        GUIScroll( diff, gadget );
        notify = ( gadget->dir == HORIZONTAL ) ? GUI_HSCROLL_NOTIFY : GUI_VSCROLL_NOTIFY;
        GUIEVENT( GUICurrWnd, notify, NULL );
    }
}

static bool SetCurrWnd( ui_event ui_ev, gui_window *curr )
{
    switch( ui_ev ) {
    case EV_MOUSE_DCLICK_R:
    case EV_MOUSE_PRESS_R:
    case EV_MOUSE_PRESS:
    case EV_MOUSE_DCLICK:
    case EV_SCROLL_UP:
    case EV_SCROLL_DOWN:
    case EV_SCROLL_PAGE_UP:
    case EV_SCROLL_PAGE_DOWN:
    case EV_SCROLL_LEFT:
    case EV_SCROLL_RIGHT:
    case EV_PAGE_LEFT:
    case EV_PAGE_RIGHT:
    case EV_SCROLL_VERTICAL:
    case EV_SCROLL_HORIZONTAL:
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
    gui_event gui_ev_top;
    gui_event gui_ev_bottom;
    gui_event gui_ev_scroll;

    /* unused parameters */ (void)prev;

    if( gadget->dir == VERTICAL ) {
        gui_ev_top = GUI_SCROLL_TOP;
        gui_ev_bottom = GUI_SCROLL_BOTTOM;
        gui_ev_scroll = GUI_SCROLL_VERTICAL;
    } else {
        gui_ev_top = GUI_SCROLL_FULL_LEFT;
        gui_ev_bottom = GUI_SCROLL_FULL_RIGHT;
        gui_ev_scroll = GUI_SCROLL_HORIZONTAL;
    }
    uisetgadgetnodraw( gadget, gadget->pos - diff );
    if( prev + diff == 0 ) {
        GUIEVENT( GUICurrWnd, gui_ev_top, NULL );
    } else if( ( prev + diff ) == ( gadget->total_size - gadget->page_size ) ) {
        GUIEVENT( GUICurrWnd, gui_ev_bottom, NULL );
    } else {
        GUIEVENT( GUICurrWnd, gui_ev_scroll, &diff );
    }
}

/*
 * CheckPrevEvent -- this routine causes the sequence of mouse events:
 *                   P-U-D-U-D-U-D-U to come out P-U-D-U P-U-D-U
 *                   where P is Press, U is Up and D is dclick.
 *                   This is for consistency with windows.
 */

static ui_event CheckPrevEvent( ui_event ui_ev )
{
    switch( ui_ev ) {
    case EV_MOUSE_PRESS:
    case EV_MOUSE_PRESS_R:
        PrevMouseEvent = ui_ev;
        break;
    case EV_MOUSE_DCLICK:
        if( ui_ev == PrevMouseEvent ) {
            ui_ev = EV_MOUSE_PRESS;
        }
        PrevMouseEvent = ui_ev;
        break;
    case EV_MOUSE_DCLICK_R:
        if( ui_ev == PrevMouseEvent ) {
            ui_ev = EV_MOUSE_PRESS_R;
        }
        PrevMouseEvent = ui_ev;
    }
    return( ui_ev );
}

static ui_event MapMiddleToRight( ui_event ui_ev )
{
    switch( ui_ev ) {
    case EV_MOUSE_PRESS_M:
        ui_ev = EV_MOUSE_PRESS_R;
        break;
    case EV_MOUSE_DRAG_M:
        ui_ev = EV_MOUSE_DRAG_R;
        break;
    case EV_MOUSE_RELEASE_M:
        ui_ev = EV_MOUSE_RELEASE_R;
        break;
    case EV_MOUSE_DCLICK_M:
        ui_ev = EV_MOUSE_DCLICK_R;
        break;
    case EV_MOUSE_REPEAT_M:
        ui_ev = EV_MOUSE_REPEAT_R;
        break;
    case EV_MOUSE_HOLD_M:
        ui_ev = EV_MOUSE_HOLD_R;
        break;
    }
    return( ui_ev );
}

/*
 * GUIProcessEvent -- Main event loop to process UI events
 */

bool GUIProcessEvent( ui_event ui_ev )
{
    ORD         row, col;
    gui_window  *wnd;
    int         prev;
    int         diff;
    gui_ctl_id  id;
    gui_window  *menu_window;
    bool        new_curr_wnd;
    VSCREEN     *vs;

    // this is processed before all others and signals the end for all
    // GUI UI windows ( unconditional )
    if( ui_ev == EV_KILL_UI ) {
        GUIDestroyWnd( NULL );
        return( false );
    }

    ui_ev = MapMiddleToRight( ui_ev );
    ui_ev = CheckPrevEvent( ui_ev );
    wnd = NULL;
    if( uimouseinstalled() ) {
        vs = uivmousepos( NULL, &row, &col );
        if( vs != NULL && ISGUIWINDOW( vs ) ) {
            wnd = (gui_window *)((char *)vs - offsetof( gui_window, vs ));
        }
    }
    if( GUIDoKeyboardMoveResize( ui_ev ) ) {
        return( true );
    }
    if( MouseState == MOUSE_MOVE || MouseState == MOUSE_SIZE ) {
        if( GUIDoMoveResizeCheck( GUIMouseWnd, ui_ev, row, col ) ) {
            MouseState = MOUSE_FREE;
            return( true );
        }
        if( GUI_WND_MINIMIZED( GUIMouseWnd ) ) {
            switch( ui_ev ) {
            case EV_MOUSE_DCLICK:
            case EV_MOUSE_RELEASE:
            case EV_MOUSE_DRAG:
                ProcessMinimizedMouseEvent( ui_ev, row, col );
            }
        } else {
            switch( ui_ev ) {
            case EV_MOUSE_RELEASE:
            case EV_MOUSE_DRAG:
            case EV_MOUSE_DRAG_R:
                ProcessMouseReleaseDrag( ui_ev, GUI_LBUTTONUP, row, col );
            }
        }
        return( true );
    }
    new_curr_wnd = SetCurrWnd( ui_ev, wnd );
    if( GUIProcessAltMenuEvent( ui_ev ) ) {
        return( true );
    }
    /* Only deal with press and dclick events for minimized windows.
     * All other non-menu events are ingored.
     */
    if( !IS_CTLEVENT( ui_ev ) && ( GUICurrWnd != NULL ) && GUI_WND_MINIMIZED( GUICurrWnd ) ) {
        /* ignore event if mouse not in minimized current window */
        if( GUICurrWnd == wnd ) {
            switch( ui_ev ) {
            case EV_MOUSE_PRESS:
            case EV_MOUSE_DCLICK:
            case EV_MOUSE_RELEASE:
                GUIMouseWnd = GUICurrWnd;
                ProcessMinimizedMouseEvent( ui_ev, row, col );
                break;
            }
        }
        return( true );
    }
    if( !IS_CTLEVENT( ui_ev ) && ( GUICurrWnd != NULL ) && GUIIsOpen( GUICurrWnd ) ) {
        /* see if any of the controls in the window consume the event */
        ui_ev = GUIProcessControlEvent( GUICurrWnd, ui_ev, row, col );
        /* See if the event is for on of the scroll bars. */
        /* Diff and prev are used if the event return is  */
        /* EV_SCROLL_HORIZONTAL or EV_SCROLL_VERTICAL.    */
        if( !new_curr_wnd || (GUIGetWindowStyles() & (GUI_INACT_GADGETS | GUI_INACT_SAME)) ) {
            ui_ev = GUIGadgetFilter( GUICurrWnd, ui_ev, &prev, &diff );
        }
        if( ui_ev == EV_NO_EVENT ) {
            return( true );
        }
    }
    ui_ev = GUIMapKeys( ui_ev );
    switch( ui_ev ) {
    case EV_MOUSE_DCLICK_R:
        ProcessMousePos( GUI_RBUTTONDBLCLK, row, col, wnd );
        return( true );
    case EV_MOUSE_RELEASE_R:
        ProcessMouseReleaseDrag( ui_ev, GUI_RBUTTONUP, row, col );
        return( true );
    case EV_MOUSE_DRAG_R:
        if( GUICurrWnd != GUIMouseWnd ) {
            /* got drag without press first */
            ProcessMousePress( EV_MOUSE_PRESS_R, GUI_LBUTTONDOWN, row, col, new_curr_wnd );
        }
        /* fall through */
    case EV_MOUSE_MOVE:
        ProcessMousePos( GUI_MOUSEMOVE, row, col, wnd );
        return( true );
    case EV_MOUSE_RELEASE:
        ProcessMouseReleaseDrag( ui_ev, GUI_LBUTTONUP, row, col );
        return( true );
    case EV_MOUSE_DRAG:
        if( GUICurrWnd != GUIMouseWnd ) {
            /* got drag without press first */
            ProcessMousePress( EV_MOUSE_PRESS, GUI_LBUTTONDOWN, row, col, new_curr_wnd );
        }
        ProcessMouseReleaseDrag( ui_ev, GUI_MOUSEMOVE, row, col );
        return( true );
    case EV_MOUSE_PRESS_R:
        ProcessMousePress( ui_ev, GUI_RBUTTONDOWN, row, col, new_curr_wnd );
        return( true );
    case EV_MOUSE_PRESS:
        ProcessMousePress( ui_ev, GUI_LBUTTONDOWN, row, col, new_curr_wnd );
        return( true );
    case EV_MOUSE_DCLICK:
        ProcessMousePress( ui_ev, GUI_LBUTTONDBLCLK, row, col, new_curr_wnd );
        return( true );
    case EV_NO_EVENT:
        break;
    case EV_SCROLL_UP:
    case EV_SCROLL_DOWN:
    case EV_SCROLL_PAGE_UP:
    case EV_SCROLL_PAGE_DOWN:
    case EV_SCROLL_LEFT:
    case EV_SCROLL_RIGHT:
    case EV_SCROLL_LEFT_PAGE:
    case EV_SCROLL_RIGHT_PAGE:
        if( GUICurrWnd != NULL ) {
            ProcessScrollEvent( ui_ev );
            return( true );
        }
        break;
    case EV_SCROLL_VERTICAL:
        if( ( GUICurrWnd->vgadget != NULL ) && (GUICurrWnd->style & GUI_VSCROLL_EVENTS) ) {
            DoScrollDrag( GUICurrWnd->vgadget, prev, diff );
        } else {
            GUIDirtyWhole( GUICurrWnd );
        }
        return( true );
    case EV_SCROLL_HORIZONTAL:
        if( ( GUICurrWnd->hgadget != NULL ) && (GUICurrWnd->style & GUI_HSCROLL_EVENTS) ) {
            DoScrollDrag( GUICurrWnd->hgadget, prev, diff );
        } else {
            GUIDirtyWhole( GUICurrWnd );
        }
        return( true );
    case EV_MENU_INITPOPUP:
        ProcessInitPopupEvent();
        return( true );
#if 0
    case EV_BACKGROUND_RESIZE:
        {
            gui_window          *root;
            root = GUIGetRootWindow();
            if( root != NULL ) {
                GUIZoomWnd( root, GUI_NONE );
            }
        }
        return( true );
#endif
    default:
        if( IS_CTLEVENT( ui_ev ) ) {
            if( !GUIMDIProcessEvent( ui_ev ) ) {
                menu_window = GUIGetMenuWindow();
                if( menu_window != NULL ) {
                    id = EV2ID( ui_ev );
                    GUIEVENT( menu_window, GUI_CLICKED, &id );
                }
            }
            return( true );
        }
        break;
    }
    return( true );
}
