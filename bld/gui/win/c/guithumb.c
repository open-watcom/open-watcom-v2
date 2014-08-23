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
#include "guixutil.h"
#include "guiscrol.h"

/*
 * SetScrollThumb -- Set the scrolling thumb for the window
 */

static void SetScrollThumb( gui_window * wnd, int percent, int bar )
{
    int         old;
    int         new;
    bool        range_set;

    if( GUIScrollOn( wnd, bar ) ) {
        old = GUIGetScrollPos( wnd, bar );
        if( percent < 0 ) {
            percent = 0;
        }
        if( percent > 100 ) {
            percent = 100;
        }
        if( bar == SB_VERT ) {
            range_set = GUI_VRANGE_SET( wnd );
        } else {
            range_set = GUI_HRANGE_SET( wnd );
        }
        if( range_set ) {
            new = (int)( (long)GUIGetScrollRange( wnd, bar ) *
                         (long)percent / (long)100 );
        } else {
            new = percent;
            GUISetScrollRange( wnd, bar, 0, 100, true );
        }
        if( old != new ) {
            GUISetScrollPos( wnd, bar, new, true );
        }
    }
}

/*
 * GUISetVScrollThumb -- Set the vertical scrolling thumb for the window
 */

void GUISetVScrollThumb( gui_window * wnd, int percent )
{
    SetScrollThumb( wnd, percent, SB_VERT );
}

/*
 * GUISetHScrollThumb -- Set the horizontal scrolling thumb for the window
 */

void GUISetHScrollThumb( gui_window * wnd, int percent )
{
    SetScrollThumb( wnd, percent, SB_HORZ );
}
