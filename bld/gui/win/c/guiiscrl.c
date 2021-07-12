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
 * InitScroll -- Init the scrolling position for the window
 */

static void InitScroll( gui_window *wnd, int bar, guix_ord pos )
{
    guix_ord old;
    guix_ord max;

    if( GUIScrollOn( wnd, bar ) ) {
        if( pos >= 0 ) {
            old = GUIGetScrollPos( wnd, bar );
            max = GUIGetScrollRange( wnd, bar );
            if( pos > max ) {
                pos = max;
            }
            if( pos < 0 ) {
                pos = 0;
            }
            if( old != pos ) {
                GUISetScrollPos( wnd, bar, pos, true );
            }
        }
    }
}

/*
 * GUIInitVScrollRow -- Init the vertical scrolling position for the window
 */

void GUIAPI GUIInitVScrollRow( gui_window *wnd, gui_text_ord vscroll_pos )
{
    InitScroll( wnd, SB_VERT, GUIFromTextY( vscroll_pos, wnd ) );
}

/*
 * GUIInitHScrollCol -- Init the horizontal scrolling position for the window
 */

void GUIAPI GUIInitHScrollCol( gui_window *wnd, gui_text_ord hscroll_pos )
{
    InitScroll( wnd, SB_HORZ, GUIFromTextX( hscroll_pos, wnd ) );
}

/*
 * GUIInitVScroll -- Init the vertical scrolling position for the window
 */

void GUIAPI GUIInitVScroll( gui_window *wnd, gui_ord vscroll_pos )
{
    guix_ord    scr_y;

    scr_y = GUIScaleToScreenV( vscroll_pos );
    if( ( vscroll_pos != 0 ) && ( scr_y == 0 ) ) {
        scr_y++;
    }
    InitScroll( wnd, SB_VERT, scr_y );
}

/*
 * GUIInitHScroll -- Init the horizontal scrolling position for the window
 */

void GUIAPI GUIInitHScroll( gui_window *wnd, gui_ord hscroll_pos )
{
    guix_ord    scr_x;

    scr_x = GUIScaleToScreenH( hscroll_pos );
    if( ( hscroll_pos != 0 ) && ( scr_x == 0 ) ) {
        scr_x++;
    }
    InitScroll( wnd, SB_HORZ, scr_x );
}
