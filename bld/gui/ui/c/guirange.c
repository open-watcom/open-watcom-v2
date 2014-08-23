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
#include "guigadgt.h"

/*
 * SetScrollRange
 */

static void SetScrollRange( p_gadget gadget, gui_ord range )
{
    if( gadget != NULL ) {
        if( gadget->total_size != range ) {
            gadget->total_size = range;
            GUISetShowGadget( gadget, true, true, gadget->pos );
        }
    }
}

/*
 * GUISetHScrollRange
 */

void GUISetHScrollRange( gui_window * wnd, gui_ord range )
{
    gui_coord coord;

    if( wnd != NULL ) {
        coord.x  = range;
        GUIScaleToScreenR( &coord );
        SetScrollRange( wnd->hgadget, coord.x );
        wnd->flags |= SETHRANGE;
    }
}

/*
 * GUISetVScrollRange
 */

void GUISetVScrollRange( gui_window * wnd, gui_ord range )
{
    gui_coord coord;

    if( wnd != NULL ) {
        coord.y  = range;
        GUIScaleToScreenR( &coord );
        SetScrollRange( wnd->vgadget, coord.y );
        wnd->flags |= SETVRANGE;
    }
}

/*
 * GUISetHScrollRangeCols
 */

void GUISetHScrollRangeCols( gui_window * wnd, gui_ord range )
{
    if( wnd != NULL ) {
        SetScrollRange( wnd->hgadget, range );
        wnd->flags |= SETHRANGE;
    }
}

/*
 * GUISetVScrollRangeRows
 */

void GUISetVScrollRangeRows( gui_window * wnd, gui_ord range )
{

    if( wnd != NULL ) {
        SetScrollRange( wnd->vgadget, range );
        wnd->flags |= SETVRANGE;
    }
}

/*
 * GUIGetVScrollRangeRows
 */

gui_ord GUIGetVScrollRangeRows( gui_window * wnd )
{
    if( wnd->vgadget != NULL ) {
        return( wnd->vgadget->total_size );
    } else {
        return( GUI_NO_ROW );
    }
}

gui_ord GUIGetVScrollRange( gui_window *wnd )
{
    gui_coord   coord;

    coord.y = GUIGetVScrollRangeRows( wnd );
    if( coord.y != GUI_NO_ROW ) {
        GUIScreenToScaleR( &coord );
        return( coord.y );
    } else {
        return( GUI_NO_ROW );
    }
}

/*
 * GUIGetHScrollRangeCols
 */

gui_ord GUIGetHScrollRangeCols( gui_window * wnd )
{
    if( wnd->hgadget != NULL ) {
        return( wnd->hgadget->total_size );
    } else {
        return( GUI_NO_COLUMN );
    }
}

gui_ord GUIGetHScrollRange( gui_window *wnd )
{
    gui_coord   coord;

    coord.x = GUIGetHScrollRangeCols( wnd );
    if( coord.x != GUI_NO_COLUMN ) {
        GUIScreenToScaleR( &coord );
        return( coord.x );
    } else {
        return( GUI_NO_COLUMN );
    }
}
