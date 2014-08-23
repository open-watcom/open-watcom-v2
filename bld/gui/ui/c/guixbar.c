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

bool GUIDrawBar( gui_window *wnd, gui_ord row, gui_ord start, gui_ord width,
                 gui_bar_styles bstyle, gui_attr attr, bool selected )
{
    gui_text_metrics    metrics;
    gui_rect            rect;
    bool                ret;

    bstyle = bstyle;

    if( wnd == NULL ) {
        return( false );
    }

    GUIGetTextMetrics( wnd, &metrics );
    rect.x      = start;
    rect.y      = row * metrics.avg.y;
    rect.width  = width;
    rect.height = metrics.avg.y;

    if( width > 0 && rect.width == 0 ) {
        rect.width = 1;
    }

    if( selected ) {
        attr = GUI_BACKGROUND;
    }

    ret = GUIFillBar( wnd, &rect, attr );

    return( ret );
}


bool GUIDrawBarGroup( gui_window *wnd, gui_ord row, gui_ord start,
                      gui_ord width1, gui_ord width2, gui_bar_styles bstyle,
                      gui_attr attr1, gui_attr attr2, bool selected )
{
    bool        ret;

    if( width1 == 0 ) {
        ret = true;
    } else {
        ret = GUIDrawBar( wnd, row, start, width1, bstyle, attr1, selected );
    }
    if( ret ) {
        ret = GUIDrawBar( wnd, row, start+width1, width2, bstyle,
                          attr2, selected );
    }
    return( ret );
}

