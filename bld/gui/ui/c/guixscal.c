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


#define COPYAREATORECT( area, rect ) {                                  \
        (rect).x = (area).col; (rect).y = (area).row;                   \
        (rect).width = (area).width; (rect).height = (area).height; }

#define COPYRECTTOAREA( rect, area ) {                                  \
        (area).row = (rect).y; (area).col = (rect).x;                   \
        (area).width = (rect).width; (area).height = (rect).height; }


/* Information Function */

void GUIGetScreenArea( SAREA * area )
{
    gui_rect rect;

    GUIGetScreen( &rect );
    COPYRECTTOAREA( rect, *area );
}

/* Conversion routines */

/*
 * ConvertRect -- from == SCREEN,  to == SCALE  implies area -> rect
 *                from == SCALE,   to == SCREEN implies rect -> area
 */

static bool ConvertRectArea( SAREA *area, gui_rect *rect, gui_coord_systems from,
                             gui_coord_systems to, bool rel )
{
    gui_rect    my_rect;
    gui_rect    *convert;

    if( from == SCREEN ) {
        /* copy area info to destination (rect), point convert at this */
        COPYAREATORECT( *area, *rect );
        convert = rect;
    } else {
        /* copy rect to static (my_rect) so rect isn't changed */
        /* point convert at my_rect */
        COPYRECT( *rect, my_rect );
        convert = &my_rect;
    }

    if( !GUIConvertRect( from, to, convert, rel ) ) {
        return( false );
    }

    /* put my_rect into info to return */
    if( to == SCREEN ) {
        /* copy convert to area */
        COPYRECTTOAREA( *convert, *area );
    }
    return( true );
}

/* User Routines */

bool GUIScreenToScaleRect( SAREA *area, gui_rect *rect )
{
    return( ConvertRectArea( area, rect, SCREEN, SCALE, false ) );
}

bool GUIScreenToScaleRectR( SAREA *area, gui_rect *rect )
{
    return( ConvertRectArea( area, rect, SCREEN, SCALE, true ) );
}

bool GUIScaleToScreenRect( gui_rect *rect, SAREA *area )
{
    return( ConvertRectArea( area, rect, SCALE, SCREEN, false ) );
}

bool GUIScaleToScreenRectR( gui_rect *rect, SAREA *area )
{
    return( ConvertRectArea( area, rect, SCALE, SCREEN, true ) );
}
