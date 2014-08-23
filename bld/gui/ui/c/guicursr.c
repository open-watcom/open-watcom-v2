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

/*
 * GUISetCursorPos
 */

bool GUISetCursorPos( gui_window *wnd, gui_point *point )
{
    gui_point   pt;

    if( wnd->style & GUI_CURSOR ) {
        pt = *point;
        GUIScaleToScreenRPt( &pt );
        if( ( wnd->hgadget != NULL ) && !GUI_HSCROLL_EVENTS_SET( wnd ) ) {
            pt.x -= wnd->hgadget->pos;
        }
        if( ( wnd->vgadget != NULL ) && !GUI_VSCROLL_EVENTS_SET( wnd ) ) {
            pt.y -= wnd->vgadget->pos;
        }
        if( ( pt.x >= 0 ) && ( pt.y >= 0 ) && ( pt.y < wnd->use.height ) &&
            ( pt.x < ( wnd->use.width  ) ) ) {
            wnd->screen.row = pt.y + wnd->use.row;
            wnd->screen.col = pt.x + wnd->use.col;
            return( GUISetCursor( wnd ) );
        }
    }
    return( false );
}

bool GUIGetCursorPos( gui_window *wnd, gui_point *point )
{
    if( ( point == NULL ) || !( wnd->style & GUI_CURSOR ) ) {
        return( false );
    }
    point->x = wnd->screen.col - wnd->use.col;
    point->y = wnd->screen.row - wnd->use.col;
    if( ( wnd->hgadget != NULL ) && !GUI_HSCROLL_EVENTS_SET( wnd ) ) {
        point->x += wnd->hgadget->pos;
    }
    if( ( wnd->vgadget != NULL ) && !GUI_VSCROLL_EVENTS_SET( wnd ) ) {
        point->y += wnd->vgadget->pos;
    }
    GUIScreenToScaleRPt( point );
    return( true );
}

bool GUIGetCursorType( gui_window *wnd, gui_char_cursor *cursor )
{
    if( !( wnd->style & GUI_CURSOR ) | ( cursor == NULL ) ) {
        return( false );
    }
    switch( wnd->screen.cursor ) {
    case C_OFF :
        *cursor = GUI_NO_CURSOR;
        break;
    case C_NORMAL :
        *cursor = GUI_NORMAL_CURSOR;
        break;
    case C_INSERT :
        *cursor = GUI_INSERT_CURSOR;
        break;
    default :
        return( false );
    }
    return( true );
}

bool GUISetCursorType( gui_window *wnd, gui_char_cursor cursor )
{
    int type;

    switch( cursor ) {
    case GUI_NO_CURSOR :
        type = C_OFF;
        break;
    case GUI_NORMAL_CURSOR :
        type = C_NORMAL;
        break;
    case GUI_INSERT_CURSOR :
        type = C_INSERT;
        break;
    default :
        return( false );
    }
    wnd->screen.cursor = type;
    return( GUISetCursor( wnd ) );
}
