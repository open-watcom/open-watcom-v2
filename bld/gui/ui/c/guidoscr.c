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
#include "guiwhole.h"
#include <stdlib.h>

static void DoScroll( gui_window *wnd, int diff, a_gadget_direction dir, int start, int end )
{
    SAREA           area;
    ORD             *min;
    unsigned short  *max;
    int             xdiff;
    int             ydiff;

    COPYRECTX( wnd->use, area );
    if( dir == VERTICAL ) {
        min = &area.row;
        max = &area.height;
        xdiff = 0;
        ydiff = -diff;
        wnd->flags |= VSCROLL_INVALID;
    } else {
        min = &area.col;
        max = &area.width;
        xdiff = -diff;
        ydiff = 0;
        wnd->flags |= HSCROLL_INVALID;
    }
    if( start > -1 ) {
        if( start > ( *max - 1  ) ) {
            start = *max - 1;
        }
        *min += start;
        *max -= start;
    }
    if( end > -1 ) {
        end++;
        if( ( end - start ) > *max ) {
            end = *max + start;
        }
        if( ( end - start ) < *max ) {
            *max = end - start;
        }
    }
    if( !uiisdbcs() || dir != HORIZONTAL ) {
        if( abs( diff ) < *max ) {  /* some info preserved */
            if( diff > 0 ) {
                /* scrolled down/left */
                *min += diff;
                *max -= diff;
                uivmoveblock( &wnd->vs, area, ydiff, xdiff );
                *min += ( *max - diff );
                *max = diff;
            } else {
                /* scrolled up/right */
                *max += diff;
                uivmoveblock( &wnd->vs, area, ydiff, xdiff );
                *max = -diff;
            }
        }
    }
    wnd->flags |= CONTENTS_INVALID;
    COPYRECTX( area, wnd->dirty );
    GUIWndUpdate( wnd );
}

void GUIAPI GUIDoHScroll( gui_window *wnd, int cols )
{
    DoScroll( wnd, cols, HORIZONTAL, -1, -1 );
}

void GUIAPI GUIDoVScroll( gui_window *wnd, int rows )
{
    DoScroll( wnd, rows, VERTICAL, -1, -1 );
}

void GUIAPI GUIDoHScrollClip( gui_window *wnd, int cols, int start, int end )
{
    DoScroll( wnd, cols, HORIZONTAL, start, end );
}

void GUIAPI GUIDoVScrollClip( gui_window *wnd, int rows, int start, int end )
{
    DoScroll( wnd, rows, VERTICAL, start, end );
}
