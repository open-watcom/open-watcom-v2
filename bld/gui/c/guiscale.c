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


#define COPYRECT( old, new ) {                                  \
        (new).x = (old).x; (new).y = (old).y;                   \
        (new).width = (old).width; (new).height = (old).height; }

static struct {
    gui_ord         x;
    gui_ord         y;
    gui_ord         width;
    gui_ord         height;
} scale_data;

static struct {
    gui_screen_ord  x;
    gui_screen_ord  y;
    gui_screen_ord  width;
    gui_screen_ord  height;
} screen_data;


/*
 * GUISetScale -- Set the user defined scale
 */

void GUISetScale( gui_rect * rect )
{
    COPYRECT( *rect, scale_data );
}

/*
 * GUIGetScale -- Get the user defined scale
 */

void GUIGetScale( gui_rect * rect )
{
    COPYRECT( scale_data, *rect )
}

/*
 * GUISetScreen -- set the screen coordinates
 */

void GUISetScreen( gui_ord xmin, gui_ord ymin, gui_ord width, gui_ord height )
{
    screen_data.x = xmin;
    screen_data.y = ymin;
    screen_data.width = width;
    screen_data.height = height;
}

/*
 * GUIGetScreen -- get the screen coordinates
 */

void GUIGetScreen( gui_rect * rect )
{
    COPYRECT( screen_data, *rect )
}

static gui_screen_ord ConvertToScreenH( gui_ord ord )
{
    return( GUIMulDiv( gui_screen_ord, ord, screen_data.width, scale_data.width ) );
}

static gui_screen_ord ConvertToScreenV( gui_ord ord )
{
    return( GUIMulDiv( gui_screen_ord, ord, screen_data.height, scale_data.height ) );
}

static gui_screen_ord ConvertToScreenX( gui_ord ix, bool rel )
{
    gui_screen_ord ox;

    if( !rel ) {
        ix -= scale_data.x;
    }
    ox = ConvertToScreenH( ix );
    if( !rel ) {
        ox += screen_data.x;
    }
    return( ox );
}

static gui_screen_ord ConvertToScreenY( gui_ord iy, bool rel )
{
    gui_screen_ord oy;

    if( !rel ) {
        iy -= scale_data.y;
    }
    oy = ConvertToScreenV( iy );
    if( !rel ) {
        oy += screen_data.y;
    }
    return( oy );
}

static gui_ord ConvertFromScreenH( gui_screen_ord ord )
{
    return( GUIMulDiv( gui_ord, ord, scale_data.width, screen_data.width ) );
}

static gui_ord ConvertFromScreenV( gui_screen_ord ord )
{
    return( GUIMulDiv( gui_ord, ord, scale_data.height, screen_data.height ) );
}

static gui_ord ConvertFromScreenX( gui_screen_ord ix, bool rel )
{
    gui_ord ox;

    if( !rel ) {
        ix -= screen_data.x;
    }
    ox = ConvertFromScreenH( ix );
    if( !rel ) {
        ox += scale_data.x;
    }
    return( ox );
}

static gui_ord ConvertFromScreenY( gui_screen_ord iy, bool rel )
{
    gui_ord oy;

    if( !rel ) {
        iy -= screen_data.y;
    }
    oy = ConvertFromScreenV( iy );
    if( !rel ) {
        oy += scale_data.y;
    }
    return( oy );
}

/*
 * ConvertToScreen -- convert a gui_coord from one coordinate system to another
 */

static bool ConvertToScreenCoord( gui_coord *coord, gui_screen_coord *screen_coord, bool rel )
{
    screen_coord->x = ConvertToScreenX( coord->x, rel );
    screen_coord->y = ConvertToScreenY( coord->y, rel );
    return( true );
}

/*
 * ConvertFromScreen -- convert a gui_coord from one coordinate system to another
 */

static bool ConvertFromScreenCoord( gui_screen_coord *screen_coord, gui_coord *coord, bool rel )
{
    coord->x = ConvertFromScreenX( screen_coord->x, rel );
    coord->y = ConvertFromScreenY( screen_coord->y, rel );
    return( true );
}

/*
 * ConvertToScreenRect -- convert a gui_coord from one coordinate system to another
 */

static bool ConvertToScreenRect( gui_rect *rect, gui_screen_rect *screen_rect, bool rel )
{
    screen_rect->s_x = ConvertToScreenX( rect->x, rel );
    screen_rect->s_y = ConvertToScreenY( rect->y, rel );
    screen_rect->s_width = ConvertToScreenH( rect->width );
    screen_rect->s_height = ConvertToScreenV( rect->height );
    return( true );
}

/*
 * ConvertFromScreenRect -- convert a gui_coord from one coordinate system to another
 */

static bool ConvertFromScreenRect( gui_screen_rect *screen_rect, gui_rect *rect, bool rel )
{
    rect->x = ConvertFromScreenX( screen_rect->s_x, rel );
    rect->y = ConvertFromScreenY( screen_rect->s_y, rel );
    rect->width = ConvertFromScreenH( screen_rect->s_width );
    rect->height = ConvertFromScreenV( screen_rect->s_height );
    return( true );
}

/* Routines Used by lower levels of GUI library */

bool GUIScreenToScaleRect( gui_screen_rect *screen_rect, gui_rect *rect )
{
    return( ConvertFromScreenRect( screen_rect, rect, false ) );
}

bool GUIScreenToScaleRectR( gui_screen_rect *screen_rect, gui_rect *rect )
{
    return( ConvertFromScreenRect( screen_rect, rect, true ) );
}

bool GUIScaleToScreenRect( gui_rect *rect, gui_screen_rect *screen_rect )
{
    return( ConvertToScreenRect( rect, screen_rect, false ) );
}

bool GUIScaleToScreenRectR( gui_rect *rect, gui_screen_rect *screen_rect )
{
    return( ConvertToScreenRect( rect, screen_rect, true ) );
}

bool GUIScaleToScreen( gui_coord *coord )
{
    return( ConvertToScreenCoord( coord, coord, false ) );
}

bool GUIScaleToScreenR( gui_coord *coord )
{
    return( ConvertToScreenCoord( coord, coord, true ) );
}

gui_screen_ord GUIScaleToScreenH( gui_ord ord )
{
    return( ConvertToScreenH( ord ) );
}

gui_screen_ord GUIScaleToScreenV( gui_ord ord )
{
    return( ConvertToScreenV( ord ) );
}

bool GUIScreenToScale( gui_coord *coord )
{
    return( ConvertFromScreenCoord( coord, coord, false ) );
}

bool GUIScreenToScaleR( gui_coord *coord )
{
    return( ConvertFromScreenCoord( coord, coord, true ) );
}

gui_ord GUIScreenToScaleH( gui_screen_ord screen_ord )
{
    return( ConvertFromScreenH( screen_ord ) );
}

gui_ord GUIScreenToScaleV( gui_screen_ord screen_ord )
{
    return( ConvertFromScreenV( screen_ord ) );
}
