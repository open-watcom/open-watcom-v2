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
#include "guiscrol.h"
#include "guixutil.h"
#include "guixloop.h"
#include "guiwhole.h"
#include "guiscale.h"
#include "guigadgt.h"
#include "guixwind.h"


/*
 * GUIScroll -- scroll the GUICurrWnd the given amount
 */

void GUIScroll( int change, p_gadget gadget )
{
    gui_window  *wnd;
    bool        range_set;
    int         pos;

    wnd = (gui_window *)gadget->win;

    if( ( wnd != NULL ) && ( gadget != NULL )  ) {
        if( ( gadget->pos + change ) < 0 ) {
            pos = 0;
        } else {
            pos = gadget->pos + change;
        }
        if( pos != gadget->pos ) {
            if( gadget->dir == VERTICAL ) {
                range_set = GUI_VRANGE_SET( wnd );
            } else {
                range_set = GUI_HRANGE_SET( wnd );
            }
            if( range_set ) {
                if( ( gadget->total_size != 0 ) &&
                    ( pos > ( gadget->total_size - gadget->page_size ) ) ) {
                    pos = gadget->total_size - gadget->page_size;
                    if( pos < 0 ) {
                        pos = 0;
                    }
                    change = pos - gadget->pos;
                }
            } else {
                gadget->total_size = gadget->page_size + pos;
            }
            GUISetShowGadget( gadget, true, true, pos );
            if( gadget->dir == VERTICAL ) {
                GUIDoVScroll( wnd, change );
            } else {
                GUIDoHScroll( wnd, change );
            }
        }
    }
}

static void InitScroll( p_gadget gadget, int pos )
{
    if( pos < 0 ) {
        return;
    }
    if( pos > ( gadget->total_size - gadget->page_size ) ) {
        pos = gadget->total_size - gadget->page_size;
    }
    if( pos < 0 ) {
        pos = 0;
    }
    GUISetShowGadget( gadget, true, true, pos );
}


static void Scrl( p_gadget gadget, gui_ord scroll_pos, bool scale,
                  void (*fn)( p_gadget, int ) )
{
    gui_coord coord;
    gui_ord   pos;

    if( gadget != NULL ) {
        if( scale ) {
            if( gadget->dir == VERTICAL ) {
                coord.y  = scroll_pos;
            } else {
                coord.x = scroll_pos;
            }
            GUIScaleToScreenR( &coord );
            if( gadget->dir == VERTICAL ) {
                pos  = coord.y;
            } else {
                pos = coord.x;
            }
            if( ( pos == 0 ) && ( scroll_pos != 0 ) ) {
                pos++;
            }
            scroll_pos = pos;
        }
        (*fn)( gadget, scroll_pos );
    }
}

static void SetScroll( p_gadget gadget, gui_ord pos )
{
   GUIScroll( (int)pos - gadget->pos, gadget );
}

/*
 * GUIInitHScroll - init the horizontal scroll position
 */

void GUIInitHScroll( gui_window * wnd, gui_ord hscroll_pos )
{
    Scrl( wnd->hgadget, hscroll_pos, true, &InitScroll );
}

/*
 * GUIInitHScrollCol - init the horizontal scroll position
 */

void GUIInitHScrollCol( gui_window * wnd, int hscroll_pos )
{
    Scrl( wnd->hgadget, hscroll_pos, false, &InitScroll );
}

/*
 * GUIInitVScroll - init the vertical scroll position
 */

void GUIInitVScroll( gui_window * wnd, gui_ord vscroll_pos )
{
    Scrl( wnd->vgadget, vscroll_pos, true, &InitScroll );
}

/*
 * GUIInitVScrollRow - init the vertical scroll position
 */

void GUIInitVScrollRow( gui_window * wnd, int vscroll_pos )
{
    Scrl( wnd->vgadget, vscroll_pos, false, &InitScroll );
}

/*
 * GUISetHScroll - set the horizontal scroll position
 */

void GUISetHScroll( gui_window * wnd, gui_ord hscroll_pos )
{
    Scrl( wnd->hgadget, hscroll_pos, true, &SetScroll );
}

/*
 * GUISetHScrollCol - set the horizontal scroll position
 */

void GUISetHScrollCol( gui_window * wnd, int hscroll_pos )
{
    Scrl( wnd->hgadget, hscroll_pos, false, &SetScroll );
}

/*
 * GUISetVScroll - set the vertical scroll position
 */

void GUISetVScroll( gui_window * wnd, gui_ord vscroll_pos )
{
    Scrl( wnd->vgadget, vscroll_pos, true, &SetScroll );
}

/*
 * GUISetVScrollRow - set the vertical scroll position
 */

void GUISetVScrollRow( gui_window * wnd, int vscroll_pos )
{
    Scrl( wnd->vgadget, vscroll_pos, false, &SetScroll );
}
