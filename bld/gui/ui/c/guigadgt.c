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
#include "guigadgt.h"
#include "guixinit.h"
#include "guixwind.h"
#include "guidraw.h"


static  int             ScrollOffset    =       1;

/*
 * GUIUseGadget -- return true if the gadget should be drawn.  To draw,
 *                 the window must be visible, not minimized and it
 *                 must be large enough to draw the gadget.  Also, the
 *                 window must be the current one, this must be a top
 *                 level window or styles must be set such that
 *                 we are drawing gadgets on inactive windows.
 */

bool GUIUseGadget( gui_window *wnd, p_gadget gadget )
{
    if( gadget == NULL ) {
        return( false );
    }
    return( !GUI_WND_MINIMIZED( wnd ) && (wnd->style & GUI_VISIBLE) &&
        ( ( gadget->end - gadget->start + 1 ) >= MIN_GADGET_SIZE ) &&
        ( gadget->total_size > gadget->page_size ) &&
        ( ( wnd == GUICurrWnd ) || ( wnd->parent == NULL ) ||
        (GUIGetWindowStyles() & (GUI_INACT_GADGETS | GUI_INACT_SAME)) ) );
}

bool GUIDrawGadgetLine( p_gadget gadget )
{
    if( GUIGetScrollOffset() > 0 ) {
        return( ( gadget->end - gadget->start + 1 ) >= MIN_GADGET_SIZE );
    }
    return( false );
}

static void SetScrollAttrs( gui_window *wnd, ATTR *scroll_bar, ATTR *scroll_icon )
{
    bool                active;
    int                 inactive;

    active = ( wnd == GUICurrWnd ) || ( wnd->parent == NULL );
    inactive = 0;
    if( active ) {
        *scroll_bar  = uisetattr( ATTR_SCROLL_BAR,  WNDATTR( wnd, GUI_FRAME_ACTIVE ) );
        *scroll_icon = uisetattr( ATTR_SCROLL_ICON, WNDATTR( wnd, GUI_FRAME_ACTIVE ) );
    } else {
        if( (GUIGetWindowStyles() & GUI_INACT_SAME) == 0 ) {
            inactive = 1;
        }
        *scroll_bar  = uisetattr( ATTR_SCROLL_BAR,  WNDATTR( wnd, GUI_FRAME_INACTIVE ) );
        *scroll_icon = uisetattr( ATTR_SCROLL_ICON, WNDATTR( wnd, GUI_FRAME_INACTIVE ) );
    }
    VertScrollFrame[0] = DRAWC( VERT_SCROLL, inactive );
    HorzScrollFrame[0] = DRAWC( HOR_SCROLL, inactive );
    SliderChar[0] = DRAWC( SCROLL_SLIDER, inactive );
    LeftPoint[0] = DRAWC( LEFT_SCROLL_ARROW, inactive );
    RightPoint[0] = DRAWC( RIGHT_SCROLL_ARROW, inactive );
    UpPoint[0] = DRAWC( UP_SCROLL_ARROW, inactive );
    DownPoint[0] =  DRAWC( DOWN_SCROLL_ARROW, inactive );
}

static void ResetScrollAttrs( ATTR scroll_bar, ATTR scroll_icon )
{
    uisetattr( ATTR_SCROLL_BAR,  scroll_bar );
    uisetattr( ATTR_SCROLL_ICON, scroll_icon );
    VertScrollFrame[0] = DRAWC1( DIAL_VERT_SCROLL );
    UpPoint[0] = DRAWC1( DIAL_UP_SCROLL_ARROW );
    DownPoint[0] =  DRAWC1( DIAL_DOWN_SCROLL_ARROW );
}


void GUISetShowGadget( p_gadget gadget, bool show, bool set, int pos )
{
    ATTR        scroll_bar;
    ATTR        scroll_icon;
    gui_window  *wnd;

    wnd = (gui_window *)(gadget->vs);
    if( set ) {
        uisetgadgetnodraw( gadget, pos );
    }
    if( !show || !GUIUseGadget( wnd, gadget ) ) {
        return;
    }
    SetScrollAttrs( wnd, &scroll_bar, &scroll_icon );
    uishowgadget( gadget );
    ResetScrollAttrs( scroll_bar, scroll_icon );
}

static ui_event CheckGadget( p_gadget gadget, ui_event ui_ev, ui_event scroll_ui_ev, int *prev, int *diff )
{
    *prev = gadget->pos;
    ui_ev = uigadgetfilter( ui_ev, gadget );
    if( ui_ev == scroll_ui_ev ) {
        *diff = gadget->pos - *prev;
    }
    return( ui_ev );
}

ui_event GUIGadgetFilter( gui_window *wnd, ui_event ui_ev, int *prev, int *diff )
{
    ATTR        scroll_bar;
    ATTR        scroll_icon;
    bool        set;

    set = false;
    if( GUIUseGadget( wnd, wnd->hgadget ) ) {
        set = true;
        SetScrollAttrs( wnd, &scroll_bar, &scroll_icon );
        ui_ev = CheckGadget( wnd->hgadget, ui_ev, EV_SCROLL_HORIZONTAL, prev, diff );
    }
    if( ui_ev != EV_NO_EVENT && ui_ev != EV_SCROLL_HORIZONTAL ) {
        if( GUIUseGadget( wnd, wnd->vgadget ) ) {
            if( !set ) {
                SetScrollAttrs( wnd, &scroll_bar, &scroll_icon );
                set = true;
            }
            ui_ev = CheckGadget( wnd->vgadget, ui_ev, EV_SCROLL_VERTICAL, prev, diff );
        }
    }
    if( set ) {
        ResetScrollAttrs( scroll_bar, scroll_icon );
    }
    return( ui_ev );
}

void GUIInitGadget( p_gadget gadget, ORD start, ORD length, ORD anchor )
{
    gadget->start = start;
    gadget->end = start + length - 1;
    gadget->page_size = length;
    gadget->anchor = anchor + 1;
    if( GUIGetScrollOffset() > 0 ) {
        if( ( gadget->end - gadget->start ) >= MIN_GADGET_SIZE ) {
            gadget->end -= GUIGetScrollOffset();
        }
    }
}

bool GUICreateGadget( gui_window *wnd, a_gadget_direction dir,
                      ORD anchor, ORD start, ORD length,
                      p_gadget *gadget_ptr, gui_scroll_styles scroll_style )
{
    p_gadget    gadget;

    if( (wnd->style & GUI_VISIBLE) == 0 ) {
        *gadget_ptr = NULL;
        return( true );
    }

    gadget = (p_gadget)GUIMemAlloc( sizeof( a_gadget ) );
    if( gadget == NULL ) {
        *gadget_ptr = NULL;
        return( false );
    }
    *gadget_ptr = gadget;
    gadget->vs = &wnd->vs;
    gadget->dir = dir;
    GUIInitGadget( gadget, start, length, anchor );
    gadget->total_size = gadget->page_size;
    if( dir == VERTICAL ) {
        gadget->forward = EV_SCROLL_DOWN;
        gadget->backward = EV_SCROLL_UP;
        gadget->pageforward = EV_SCROLL_PAGE_DOWN;
        gadget->pagebackward = EV_SCROLL_PAGE_UP;
        if( scroll_style & GUI_VDRAG ) {
            gadget->slider = EV_SCROLL_VERTICAL;
        } else {
            gadget->slider = EV_NO_EVENT;
        }
        if( scroll_style & GUI_VTRACK ) {
            gadget->flags = GADGET_TRACK;
        } else {
            gadget->flags = GADGET_NONE;
        }
    } else {
        gadget->forward = EV_SCROLL_RIGHT;
        gadget->backward = EV_SCROLL_LEFT;
        gadget->pageforward = EV_SCROLL_RIGHT_PAGE;
        gadget->pagebackward = EV_SCROLL_LEFT_PAGE;
        if( scroll_style & GUI_HDRAG ) {
            gadget->slider = EV_SCROLL_HORIZONTAL;
        } else {
            gadget->slider = EV_NO_EVENT;
        }
        if( scroll_style & GUI_HTRACK ) {
            gadget->flags = GADGET_TRACK;
        } else {
            gadget->flags = GADGET_NONE;
        }
    }
    uisetgadgetnodraw( gadget, 0 );
    return( true );
}

int GUIGetScrollOffset( void )
{
    return( ScrollOffset );
}
