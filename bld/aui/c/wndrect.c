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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "auipvt.h"

static void RoundToPixel( gui_coord *new, gui_coord *pixel_size )
{
    gui_coord   old;

    old = *new;
    GUITruncToPixel( new );
    if( old.x - new->x >= pixel_size->x / 2 ) new->x += pixel_size->x;
    if( old.y - new->y >= pixel_size->y / 2 ) new->y += pixel_size->y;
}


static void RoundRect( gui_rect *rect )
{
    gui_coord   pixel_size;
    gui_coord   tmp;
    gui_coord   top_left;
    gui_coord   bot_rite;

    pixel_size.x = WndMax.x;
    pixel_size.y = WndMax.y;
    GUITruncToPixel( &pixel_size );
    tmp.x = pixel_size.x - 1;
    tmp.y = pixel_size.y - 1;
    GUITruncToPixel( &tmp );
    pixel_size.x -= tmp.x;
    pixel_size.y -= tmp.y;

    top_left.x = rect->x;
    top_left.y = rect->y;
    RoundToPixel( &top_left, &pixel_size );

    bot_rite.x = rect->x + rect->width;
    bot_rite.y = rect->y + rect->height;
    RoundToPixel( &bot_rite, &pixel_size );

    rect->x = top_left.x;
    rect->y = top_left.y;
    rect->width = bot_rite.x - top_left.x;
    rect->height = bot_rite.y - top_left.y;
}


extern void WndPosToRect( wnd_posn *posn, gui_rect *rect, gui_coord *scale )
{

    rect->x = posn->x * scale->x;
    rect->y = posn->y * scale->y;
    rect->width = posn->width * scale->x;
    rect->height = posn->height * scale->y;
    RoundRect( rect );
}

extern void WndRectToPos( gui_rect *rect, wnd_posn *posn, gui_coord *scale )
{

    posn->x = (float)rect->x / scale->x;
    posn->y = (float)rect->y / scale->y;
    posn->width = (float)rect->width / scale->x;
    posn->height = (float)rect->height / scale->y;
}

