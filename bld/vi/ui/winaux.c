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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "vi.h"
#include "win.h"

/*
 * WindowAuxInfo - get info about a window
 */
int WindowAuxInfo( window_id wn, int type )
{
    wind        *w;
    int         rc;

    if( wn == -1 ) {
        return( 1 );
    }

    w = Windows[ wn ];
    switch( type ) {
    case WIND_INFO_X1: rc = w->x1; break;
    case WIND_INFO_Y1: rc = w->y1; break;
    case WIND_INFO_X2: rc = w->x2; break;
    case WIND_INFO_Y2: rc = w->y2; break;
    case WIND_INFO_TEXT_LINES: rc = w->text_lines; break;
    case WIND_INFO_TEXT_COLS: rc = w->text_cols; break;
    case WIND_INFO_HEIGHT: rc = w->height; break;
    case WIND_INFO_WIDTH: rc = w->width; break;
    case WIND_INFO_TEXT_COLOR: rc = w->text_color; break;
    case WIND_INFO_BACKGROUND_COLOR: rc = w->background_color; break;
    case WIND_INFO_HAS_BORDER: rc = (int) w->has_border; break;
    case WIND_INFO_BORDER_COLOR1: rc = w->border_color1; break;
    case WIND_INFO_BORDER_COLOR2: rc = w->border_color2; break;
    case WIND_INFO_HAS_SCROLL_GADGETS: rc = w->has_scroll_gadgets; break;
    }

    return( rc );

} /* WindowAuxInfo */

/*
 * WindowAuxUpdate - update stuff for a window
 */
void WindowAuxUpdate( window_id wn, int type, int data )
{
    wind *w;

    if( wn == -1 ) {
        return;
    }

    w = Windows[ wn ];
    switch( type ) {
    case WIND_INFO_MIN_SLOT: w->min_slot = (char) data; break;
    case WIND_INFO_TEXT_COLOR: w->text_color = data; break;
    case WIND_INFO_BACKGROUND_COLOR: w->background_color = data; break;
    case WIND_INFO_BORDER_COLOR1: w->border_color1 = data; break;
    case WIND_INFO_BORDER_COLOR2: w->border_color2 = data; break;
    case WIND_INFO_HAS_SCROLL_GADGETS: w->has_scroll_gadgets = data; break;
    }

} /* WindowAuxUpdate */
