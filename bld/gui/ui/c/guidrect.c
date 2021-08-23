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
#include "guixutil.h"

/*
 * GUIWndDirtyRect -- tell the user interface that one rectangle of the
 *                   contents of window wnd are bad.
 */

void GUIAPI GUIWndDirtyRect( gui_window *wnd, const gui_rect *rect )
{
//    SAREA       area;
    guix_rect   scr_rect;

    GUIScaleToScreenRectR( rect, &scr_rect );

    /* adjust for scrolling */
    if( GUI_DO_VSCROLL( wnd ) ) {
        if( scr_rect.s_y < wnd->vgadget->pos ) {
            if( ( scr_rect.s_y + scr_rect.s_height ) < wnd->vgadget->pos ) {
                return; // rect entirely above visible area;
            } else {
                /* only bottom portion of rect visible */
                scr_rect.s_height -= ( wnd->vgadget->pos - scr_rect.s_y );
                scr_rect.s_y = 0;
            }
        } else {
            scr_rect.s_y -= wnd->vgadget->pos;
        }
    }
    if( GUI_DO_HSCROLL( wnd ) ) {
        if( scr_rect.s_x < wnd->hgadget->pos ) {
            if( ( scr_rect.s_x + scr_rect.s_width ) < wnd->hgadget->pos ) {
                return; // rect entirely to left of visible area;
            } else {
                /* only right portion of rect visible */
                scr_rect.s_width -= ( wnd->hgadget->pos - scr_rect.s_x );
                scr_rect.s_x = 0;
            }
        } else {
            scr_rect.s_x -= wnd->hgadget->pos;
        }
    }

    /* adjust top left to inside client area */
    scr_rect.s_y += wnd->use.row;
    scr_rect.s_x += wnd->use.col;

    /* ensure we're only redrawing in the client area */

    if( scr_rect.s_y > ( wnd->use.row + wnd->use.height ) ) {
        /* area entirely below visible area */
        return;
    }
    if( scr_rect.s_x > ( wnd->use.col + wnd->use.width ) ) {
        /* area entirely to right of visible area */
        return;
    }
    if( ( scr_rect.s_y + scr_rect.s_height ) > ( wnd->use.row + wnd->use.height ) ) {
        /* area goes past bottom of visible area - clip */
        scr_rect.s_height = wnd->use.row + wnd->use.height - scr_rect.s_y;
    }
    if( ( scr_rect.s_x + scr_rect.s_width ) > ( wnd->use.col + wnd->use.width ) ) {
        /* area goes past right of visible area - clip */
        scr_rect.s_width = wnd->use.col + wnd->use.width - scr_rect.s_x;
    }
    GUIDirtyArea( wnd, &scr_rect );
}
