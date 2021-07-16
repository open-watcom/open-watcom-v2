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


static struct {
    gui_ord         x;
    gui_ord         y;
    gui_ord         width;
    gui_ord         height;
} scale_data;

static struct {
    guix_ord        x;
    guix_ord        y;
    guix_ord        width;
    guix_ord        height;
} screen_data;


/*
 * GUISetScale -- Set the user defined scale
 */

void GUIAPI GUISetScale( const gui_rect *rect )
{
    COPYRECT( *rect, scale_data );
}

/*
 * GUIGetScale -- Get the user defined scale
 */

void GUIAPI GUIGetScale( gui_rect *rect )
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

void GUIAPI GUIGetScreen( gui_rect *rect )
{
    COPYRECT( screen_data, *rect )
}

static guix_ord ConvertToScreenH( gui_ord ord )
{
    return( GUIMulDiv( guix_ord, ord, screen_data.width, scale_data.width ) );
}

static guix_ord ConvertToScreenV( gui_ord ord )
{
    return( GUIMulDiv( guix_ord, ord, screen_data.height, scale_data.height ) );
}

static gui_ord ConvertFromScreenH( guix_ord ord )
{
    return( GUIMulDiv( gui_ord, ord, scale_data.width, screen_data.width ) );
}

static gui_ord ConvertFromScreenV( guix_ord ord )
{
    return( GUIMulDiv( gui_ord, ord, scale_data.height, screen_data.height ) );
}

/*
 * ConvertToScreenRect -- convert a gui_coord from one coordinate system to another
 */

static bool ConvertToScreenRect( const gui_rect *rect, guix_rect *screen_rect, bool rel )
{
    if( rel ) {
        screen_rect->s_x = ConvertToScreenH( rect->x );
        screen_rect->s_y = ConvertToScreenV( rect->y );
    } else {
        screen_rect->s_x = ConvertToScreenH( rect->x - scale_data.x ) + screen_data.x;
        screen_rect->s_y = ConvertToScreenV( rect->y - scale_data.y ) + screen_data.y;
    }
    screen_rect->s_width = ConvertToScreenH( rect->width );
    screen_rect->s_height = ConvertToScreenV( rect->height );
    return( true );
}

/*
 * ConvertFromScreenRect -- convert a gui_coord from one coordinate system to another
 */

static bool ConvertFromScreenRect( const guix_rect *screen_rect, gui_rect *rect, bool rel )
{
    if( rel ) {
        rect->x = ConvertFromScreenH( screen_rect->s_x );
        rect->y = ConvertFromScreenV( screen_rect->s_y );
    } else {
        rect->x = ConvertFromScreenH( screen_rect->s_x - screen_data.x ) + scale_data.x;
        rect->y = ConvertFromScreenV( screen_rect->s_y - screen_data.y ) + scale_data.y;
    }
    rect->width = ConvertFromScreenH( screen_rect->s_width );
    rect->height = ConvertFromScreenV( screen_rect->s_height );
    return( true );
}

/* Routines Used by lower levels of GUI library */

bool GUIScreenToScaleRect( const guix_rect *screen_rect, gui_rect *rect )
{
    return( ConvertFromScreenRect( screen_rect, rect, false ) );
}

bool GUIScreenToScaleRectR( const guix_rect *screen_rect, gui_rect *rect )
{
    return( ConvertFromScreenRect( screen_rect, rect, true ) );
}

bool GUIScaleToScreenRect( const gui_rect *rect, guix_rect *screen_rect )
{
    return( ConvertToScreenRect( rect, screen_rect, false ) );
}

bool GUIScaleToScreenRectR( const gui_rect *rect, guix_rect *screen_rect )
{
    return( ConvertToScreenRect( rect, screen_rect, true ) );
}

guix_ord GUIScaleToScreenH( gui_ord ord )
{
    return( ConvertToScreenH( ord ) );
}

guix_ord GUIScaleToScreenV( gui_ord ord )
{
    return( ConvertToScreenV( ord ) );
}

guix_ord GUIScaleToScreenX( gui_ord ord )
{
    return( ConvertToScreenH( ord - scale_data.x ) + screen_data.x );
}

guix_ord GUIScaleToScreenY( gui_ord ord )
{
    return( ConvertToScreenV( ord - scale_data.y ) + screen_data.y );
}

gui_ord GUIScreenToScaleH( guix_ord screen_ord )
{
    return( ConvertFromScreenH( screen_ord ) );
}

gui_ord GUIScreenToScaleV( guix_ord screen_ord )
{
    return( ConvertFromScreenV( screen_ord ) );
}

gui_ord GUIScreenToScaleX( guix_ord screen_ord )
{
    return( ConvertFromScreenH( screen_ord - screen_data.x ) + scale_data.x );
}

gui_ord GUIScreenToScaleY( guix_ord screen_ord )
{
    return( ConvertFromScreenV( screen_ord - screen_data.y ) + scale_data.y );
}
