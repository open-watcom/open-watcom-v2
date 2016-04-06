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
#include "guigadgt.h"
#include "guixinit.h"
#include "guixwind.h"


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
    return( !GUI_WND_MINIMIZED( wnd ) && ( wnd->style & GUI_VISIBLE ) &&
        ( ( gadget->end - gadget->start + 1 ) >= MIN_GADGET_SIZE ) &&
        ( gadget->total_size > gadget->page_size ) &&
        ( ( wnd == GUICurrWnd ) || ( wnd->parent == NULL ) ||
        ( GUIGetWindowStyles() & ( GUI_INACT_GADGETS | GUI_INACT_SAME ) ) ) );
}

bool GUIDrawGadgetLine( p_gadget gadget )
{
    if( GUIGetScrollOffset() > 0 ) {
        return( ( gadget->end - gadget->start + 1 ) >= MIN_GADGET_SIZE );
    }
    return( false );
}

static void SetScrollAttrs( gui_window *wnd, ATTR *scroll_bar,
                            ATTR *scroll_icon )
{
    bool                active;
    gui_draw_char       offset;

    *scroll_bar = UIData->attrs[ATTR_SCROLL_BAR];
    *scroll_icon = UIData->attrs[ATTR_SCROLL_ICON];
    active = ( wnd == GUICurrWnd ) || ( wnd->parent == NULL );
    offset = 0;
    if( active ) {
        UIData->attrs[ATTR_SCROLL_BAR] = wnd->colours[GUI_FRAME_ACTIVE];
        UIData->attrs[ATTR_SCROLL_ICON] = wnd->colours[GUI_FRAME_ACTIVE];
    } else {
        if( !(GUIGetWindowStyles() & GUI_INACT_SAME ) ) {
            offset = GUI_INACTIVE_OFFSET;
        }
        UIData->attrs[ATTR_SCROLL_BAR] = wnd->colours[GUI_FRAME_INACTIVE];
        UIData->attrs[ATTR_SCROLL_ICON] = wnd->colours[GUI_FRAME_INACTIVE];
    }
    VertScrollFrame[0] = GUIGetCharacter( GUI_VERT_SCROLL + offset );
    HorzScrollFrame[0] = GUIGetCharacter( GUI_HOR_SCROLL + offset);
    SliderChar[0] = GUIGetCharacter( GUI_SCROLL_SLIDER + offset);
    LeftPoint[0] = GUIGetCharacter( GUI_LEFT_SCROLL_ARROW + offset);
    RightPoint[0] = GUIGetCharacter( GUI_RIGHT_SCROLL_ARROW + offset);
    UpPoint[0] = GUIGetCharacter( GUI_UP_SCROLL_ARROW + offset);
    DownPoint[0] =  GUIGetCharacter( GUI_DOWN_SCROLL_ARROW + offset);
}

static void ResetScrollAttrs( ATTR scroll_bar, ATTR scroll_icon )
{
    UIData->attrs[ATTR_SCROLL_BAR] = scroll_bar;
    UIData->attrs[ATTR_SCROLL_ICON] = scroll_icon;
    VertScrollFrame[0] = GUIGetCharacter( GUI_DIAL_VERT_SCROLL );
    UpPoint[0] = GUIGetCharacter( GUI_DIAL_UP_SCROLL_ARROW );
    DownPoint[0] =  GUIGetCharacter( GUI_DIAL_DOWN_SCROLL_ARROW );
}


void GUISetShowGadget( p_gadget gadget, bool show, bool set, int pos )
{
    ATTR        scroll_bar;
    ATTR        scroll_icon;
    gui_window  *wnd;

    wnd = (gui_window *)(gadget->win);
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

static EVENT CheckGadget( p_gadget gadget, EVENT ev, EVENT scroll_ev,
                          int *prev, int *diff )
{
    *prev = gadget->pos;
    ev = uigadgetfilter( ev, gadget );
    if( ev == scroll_ev ) {
        *diff = gadget->pos - *prev;
    }
    return( ev );
}

EVENT GUIGadgetFilter( gui_window *wnd, EVENT ev, int *prev, int *diff )
{
    ATTR        scroll_bar;
    ATTR        scroll_icon;
    bool        set;

    set = false;
    if( GUIUseGadget( wnd, wnd->hgadget ) ) {
        set = true;
        SetScrollAttrs( wnd, &scroll_bar, &scroll_icon );
        ev = CheckGadget( wnd->hgadget, ev, EV_SCROLL_HORIZONTAL, prev, diff );
    }
    if( ev != EV_NO_EVENT && ev != EV_SCROLL_HORIZONTAL ) {
        if( GUIUseGadget( wnd, wnd->vgadget ) ) {
            if( !set ) {
                SetScrollAttrs( wnd, &scroll_bar, &scroll_icon );
                set = true;
            }
            ev = CheckGadget( wnd->vgadget, ev, EV_SCROLL_VERTICAL, prev, diff );
        }
    }
    if( set ) {
        ResetScrollAttrs( scroll_bar, scroll_icon );
    }
    return( ev );
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
                      p_gadget *gadget_ptr, gui_scroll_styles style )
{
    p_gadget    gadget;

    if( !( wnd->style & GUI_VISIBLE ) ) {
        *gadget_ptr = NULL;
        return( true );
    }

    gadget = (p_gadget)GUIMemAlloc( sizeof( a_gadget ) );
    if( gadget == NULL ) {
        *gadget_ptr = NULL;
        return( false );
    }
    *gadget_ptr = gadget;
    gadget->win = &wnd->screen;
    gadget->dir = dir;
    GUIInitGadget( gadget, start, length, anchor );
    gadget->total_size = gadget->page_size;
    if( dir == VERTICAL ) {
        gadget->forward = EV_SCROLL_DOWN;
        gadget->backward = EV_SCROLL_UP;
        gadget->pageforward = EV_SCROLL_PAGE_DOWN;
        gadget->pagebackward = EV_SCROLL_PAGE_UP;
        if( style & GUI_VDRAG ) {
            gadget->slider = EV_SCROLL_VERTICAL;
        } else {
            gadget->slider = EV_NO_EVENT;
        }
        if( style & GUI_VTRACK ) {
            gadget->flags = GADGET_TRACK;
        } else {
            gadget->flags = GADGET_NONE;
        }
    } else {
        gadget->forward = EV_SCROLL_RIGHT;
        gadget->backward = EV_SCROLL_LEFT;
        gadget->pageforward = EV_SCROLL_RIGHT_PAGE;
        gadget->pagebackward = EV_SCROLL_LEFT_PAGE;
        if( style & GUI_HDRAG ) {
            gadget->slider = EV_SCROLL_HORIZONTAL;
        } else {
            gadget->slider = EV_NO_EVENT;
        }
        if( style & GUI_HTRACK ) {
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

