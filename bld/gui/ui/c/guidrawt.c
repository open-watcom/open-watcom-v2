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
#include "guix.h"
#include <string.h>
#include "walloca.h"

/*
 * GUIXDrawText -- draw text
 */

void GUIXDrawText( gui_window *wnd, const char *text, size_t length, gui_coord *in_pos,
                  gui_attr attr, gui_ord extentx, bool draw_extent )
{
    int         vscroll;        /* vertical scroll adjust amount */
    int         hscroll;        /* horizontal scroll adjust amount */
    size_t      my_length;      /* actual length of text (may be > length) */
    gui_coord   my_pos;         /* pos in screen coords */
    int         pos;            /* position to draw on VSCREEN */
    int         col;            /* index into string */
    gui_coord   extent;
    SAREA       area;
    int         width;
    int         frame_adjust;

    if( attr >= wnd->num_attrs ) {
        return;
    }
    if( !( wnd->style & GUI_VISIBLE ) ) {
         return;
    }

    if( wnd->style & GUI_NOFRAME ) {
        frame_adjust = 0;
    } else {
        frame_adjust = 1;
    }

    my_pos.x = in_pos->x;
    my_pos.y = in_pos->y;
    GUIScaleToScreenR( &my_pos );

    /* adjust for scrolling */
    vscroll = 0;
    if( ( wnd->vgadget != NULL ) && ( !GUI_VSCROLL_EVENTS_SET( wnd ) ) ) {
        vscroll += wnd->vgadget->pos;
    }
    hscroll = 0;
    if( ( wnd->hgadget != NULL ) && !GUI_HSCROLL_EVENTS_SET( wnd ) ) {
        hscroll += wnd->hgadget->pos;
    }

    if( ( my_pos.y - vscroll + frame_adjust ) < frame_adjust ) {
        /* row to draw is not visible with vertical scrolling */
        return;
    }

    if( text != NULL ) {
        my_length = strlen( text );
        if( my_length < length ) {
            length = my_length;
        }
    } else {
        my_length = 0;
    }

    /* if text shows even with scrolling */
    if( ( my_pos.x + length ) > hscroll ) {
        pos = frame_adjust;  /* position on VSCREEN */
        col = 0;             /* index into curr string */

        /* start of text in dirty region */
        if( ( ( wnd->dirty.col - 1 ) <= ( my_pos.x - hscroll ) ) &&
            ( my_pos.x >= hscroll ) ) {
            pos += ( my_pos.x - hscroll );
        } else {
            /* start of text to left of dirty region */
            pos += wnd->dirty.col - 1;
            if( my_pos.x < hscroll ) {
                /* start of text scrolled off screen */
                col    += ( hscroll - my_pos.x + wnd->dirty.col - 1 );
                length -= ( hscroll - my_pos.x + wnd->dirty.col - 1 );
            } else {
                /* start of text visible but to left of dirty region */
                col    += ( wnd->dirty.col  - 1 + hscroll - my_pos.x );
                length -= ( wnd->dirty.col  - 1 + hscroll - my_pos.x );
            }
        }
        /* should deal with decreasing length due to text off screen
            to right */
        if( ( length > 0 ) &&
            ( ( col - hscroll ) < ( wnd->dirty.col + wnd->dirty.width ) ) ) {
            if( ( pos + length ) > ( wnd->dirty.col + wnd->dirty.width ) ) {
                length = wnd->dirty.col + wnd->dirty.width - pos;
            }
            if( length > 0 ) {
                char        *p;
                const char  *cp;

                for( cp = text; cp < text+col; cp += GUICharLen( *cp ) ) ;
                if( cp != text + col ) {
                    p = alloca( length );
                    cp = memcpy( p, text+col, length );
                    p[0] = ' ';
                }
                uivtextput( &wnd->screen, my_pos.y - vscroll + frame_adjust,
                            pos, wnd->colours[attr], cp, length );
            } else {
                length = 0;
            }
        } else {
            length = 0;
        }
    } else {
        pos = -length + 1; /* so (pos+length) will be 1 for drawing extent */
    }
    if( draw_extent ) {
        if( extentx == GUI_NO_COLUMN ) {
            /* this is the most that will be covered.  It will be adjust for
               starting position and dirty rect */
            extentx = wnd->use.width;
        } else {
            /* record total width user wants to cover, adjusting for
             * portion not visible due to scrolling scrolling
             */
            extent.x = extentx + in_pos->x;
            GUIScaleToScreenR( &extent );
            extentx = extent.x - hscroll;
        }

        if( ( pos + length ) <= extentx ) {
            area.row = my_pos.y - vscroll + frame_adjust;
            area.height = 1;
            area.col = pos + length;
            /* adjust left border for dirty area */
            if( area.col < wnd->dirty.col ) {
                area.col = wnd->dirty.col;
            }
            width = extentx - area.col + 1;
            /* adjust right border for dirty area */
            if( ( area.col + width ) > ( wnd->dirty.col + wnd->dirty.width ) ) {
                width = wnd->dirty.width + wnd->dirty.col - area.col;
            }
            if( width > 0 )  {
                area.width = width;
                uivfill( &wnd->screen, area, wnd->colours[attr], ' ' );
            }
        }
    }
}

void GUIXDrawTextRGB( gui_window *wnd, const char *text, size_t length, gui_coord *pos,
                      gui_rgb fore, gui_rgb back, gui_ord extentx,
                      bool draw_extent )
{
    wnd = wnd;
    text = text;
    length = length;
    pos = pos;
    fore = fore;
    back = back;
    extentx = extentx;
    draw_extent = draw_extent;
}

