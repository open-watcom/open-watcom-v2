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
#include "guisdef.h"
#include "guiscale.h"

static gui_rect scale_data[NUM_COORD_SYSTEMS];

/*
 * GUISetScale -- Set the user defined scale
 */

void GUISetScale( gui_rect * rect )
{
    COPYRECT( *rect, scale_data[SCALE] );
}

/*
 * GUIGetScale -- Get the user defined scale
 */

void GUIGetScale( gui_rect * rect )
{
    COPYRECT( scale_data[SCALE], *rect )
}

/*
 * GUISetScreen -- set the screen coordinates
 */

void GUISetScreen( gui_ord xmin, gui_ord ymin, gui_ord width, gui_ord height )
{
    scale_data[SCREEN].x = xmin;
    scale_data[SCREEN].y = ymin;
    scale_data[SCREEN].width = width;
    scale_data[SCREEN].height = height;
}

/*
 * GUIGetScreen -- get the screen coordinates
 */

void GUIGetScreen( gui_rect * rect )
{
    COPYRECT( scale_data[SCREEN], *rect )
}

/*
 * GUIConvert -- convert a gui_coord from one coordinate system to another
 */

bool GUIConvert( gui_coord_systems from, gui_coord_systems to, gui_coord *coord,
                 bool rel )
{
    if( !rel ) {
        coord->x -= scale_data[from].x;
        coord->y -= scale_data[from].y;
    }
    coord->x = GUIMulDiv( int, coord->x, scale_data[to].width,  scale_data[from].width );
    coord->y = GUIMulDiv( int, coord->y, scale_data[to].height, scale_data[from].height );
    if( !rel ) {
        coord->x += scale_data[to].x;
        coord->y += scale_data[to].y;
    }
    return( true );
}

/*
 * GUIConvertRect -- convert a rect from one coordinate system to another
 */

bool GUIConvertRect( gui_coord_systems from, gui_coord_systems to, gui_rect * rect, bool rel )
{
    if( !GUIConvert( from, to, (gui_coord *)rect, rel ) ) {
        return( false );
    }
    return( GUIConvert( from, to, (gui_coord *)(&(rect->width)), true ) );
}

/*
 * GUIConvertPoint -- convert a point from one coordinate system to another
 */

void GUIConvertPoint( gui_coord_systems from, gui_coord_systems to, gui_point * point )
{
    point->x = GUIMulDiv( int, point->x, scale_data[to].width, scale_data[from].width );
    point->y = GUIMulDiv( int, point->y, scale_data[to].height, scale_data[from].height );
}

/* Routines Used by lower levels of GUI library */

bool GUIScaleToScreen( gui_coord *coord )
{
    return( GUIConvert( SCALE, SCREEN, coord, false ) );
}

bool GUIScaleToScreenR( gui_coord *coord )
{
    return( GUIConvert( SCALE, SCREEN, coord, true ) );
}

bool GUIScreenToScale( gui_coord *coord )
{
    return( GUIConvert( SCREEN, SCALE, coord, false ) );
}

bool GUIScreenToScaleR( gui_coord *coord )
{
    return( GUIConvert( SCREEN, SCALE, coord, true ) );
}

void GUIScaleToScreenPointR( gui_point *point )
{
    GUIConvertPoint( SCALE, SCREEN, point );
}

void GUIScreenToScalePointR( gui_point *point )
{
    GUIConvertPoint( SCREEN, SCALE, point );
}
