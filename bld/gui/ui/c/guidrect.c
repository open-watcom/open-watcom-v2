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
#include "guiscale.h"
#include "guixutil.h"

/*
 * GUIWndDirtyRect -- tell the user interface that one rectangle of the
 *                   contents of window wnd are bad.
 */

void GUIWndDirtyRect( gui_window * wnd, gui_rect * rect )
{
    SAREA       area;

    GUIScaleToScreenRectR( rect, &area );

    /* adjust for scrolling */
    if( ( wnd->vgadget != NULL ) && !GUI_VSCROLL_EVENTS_SET( wnd ) ) {
        if( area.row < wnd->vgadget->pos ) {
            if( ( area.row + area.height ) < wnd->vgadget->pos ) {
                return; // rect entirely above visible area;
            } else {
                /* only bottom portion of rect visible */
                area.height -= ( wnd->vgadget->pos - area.row );
                area.row = 0;
            }
        } else {
            area.row -= wnd->vgadget->pos;
        }
    }
    if( ( wnd->hgadget != NULL ) && !GUI_HSCROLL_EVENTS_SET( wnd ) ) {
        if( area.col < wnd->hgadget->pos ) {
            if( ( area.col + area.width ) < wnd->hgadget->pos ) {
                return; // rect entirely to left of visible area;
            } else {
                /* only right portion of rect visible */
                area.width -= ( wnd->hgadget->pos - area.col );
                area.col = 0;
            }
        } else {
            area.col -= wnd->hgadget->pos;
        }
    }

    /* adjust top left to inside client area */
    area.row += wnd->use.row;
    area.col += wnd->use.col;

    /* ensure we're only redrawing in the client area */

    if( area.row > ( wnd->use.row + wnd->use.height ) ) {
        /* area entirely below visible area */
        return;
    }
    if( area.col > ( wnd->use.col + wnd->use.width ) ) {
        /* area entirely to right of visible area */
        return;
    }
    if( ( area.row + area.height ) > ( wnd->use.row + wnd->use.height ) ) {
        /* area goes past bottom of visible area - clip */
        area.height = wnd->use.row + wnd->use.height - area.row;
    }
    if( ( area.col + area.width ) > ( wnd->use.col + wnd->use.width ) ) {
        /* area goes past right of visible area - clip */
        area.width = wnd->use.col + wnd->use.width - area.col;
    }
    GUIDirtyArea( wnd, &area );
}
