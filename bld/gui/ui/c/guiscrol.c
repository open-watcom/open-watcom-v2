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

    if( gadget != NULL ) {
        wnd = (gui_window *)(gadget->vs);
        if( wnd != NULL ) {
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
}

static void InitScroll( p_gadget gadget, guix_ord scr_pos )
{
    if( scr_pos > ( gadget->total_size - gadget->page_size ) ) {
        scr_pos = gadget->total_size - gadget->page_size;
    }
    GUISetShowGadget( gadget, true, true, scr_pos );
}

static void SetScroll( p_gadget gadget, guix_ord scr_pos )
{
   GUIScroll( (int)scr_pos - gadget->pos, gadget );
}

static void Scrl( p_gadget gadget, gui_ord scroll_pos, void (*fn)( p_gadget, guix_ord ) )
{
    guix_ord    scr_pos;

    if( gadget != NULL ) {
        if( gadget->dir == VERTICAL ) {
            scr_pos = GUIScaleToScreenV( scroll_pos );
        } else {
            scr_pos = GUIScaleToScreenH( scroll_pos );
        }
        if( ( scr_pos == 0 ) && ( scroll_pos != 0 ) ) {
            scr_pos++;
        }
        (*fn)( gadget, scr_pos );
    }
}

static void ScrlText( p_gadget gadget, guix_ord scroll_pos, void (*fn)( p_gadget, guix_ord ) )
{
    if( gadget != NULL ) {
        (*fn)( gadget, scroll_pos );
    }
}

/*
 * GUIInitHScroll - init the horizontal scroll position
 */

void GUIAPI GUIInitHScroll( gui_window * wnd, gui_ord hscroll_pos )
{
    Scrl( wnd->hgadget, hscroll_pos, &InitScroll );
}

/*
 * GUIInitHScrollCol - init the horizontal scroll position
 */

void GUIAPI GUIInitHScrollCol( gui_window * wnd, gui_text_ord hscroll_pos )
{
    ScrlText( wnd->hgadget, hscroll_pos, &InitScroll );
}

/*
 * GUIInitVScroll - init the vertical scroll position
 */

void GUIAPI GUIInitVScroll( gui_window * wnd, gui_ord vscroll_pos )
{
    Scrl( wnd->vgadget, vscroll_pos, &InitScroll );
}

/*
 * GUIInitVScrollRow - init the vertical scroll position
 */

void GUIAPI GUIInitVScrollRow( gui_window * wnd, gui_text_ord vscroll_pos )
{
    ScrlText( wnd->vgadget, vscroll_pos, &InitScroll );
}

/*
 * GUISetHScroll - set the horizontal scroll position
 */

void GUIAPI GUISetHScroll( gui_window * wnd, gui_ord hscroll_pos )
{
    Scrl( wnd->hgadget, hscroll_pos, &SetScroll );
}

/*
 * GUISetHScrollCol - set the horizontal scroll position
 */

void GUIAPI GUISetHScrollCol( gui_window * wnd, gui_text_ord hscroll_pos )
{
    ScrlText( wnd->hgadget, hscroll_pos, &SetScroll );
}

/*
 * GUISetVScroll - set the vertical scroll position
 */

void GUIAPI GUISetVScroll( gui_window * wnd, gui_ord vscroll_pos )
{
    Scrl( wnd->vgadget, vscroll_pos, &SetScroll );
}

/*
 * GUISetVScrollRow - set the vertical scroll position
 */

void GUIAPI GUISetVScrollRow( gui_window * wnd, gui_text_ord vscroll_pos )
{
    ScrlText( wnd->vgadget, vscroll_pos, &SetScroll );
}
