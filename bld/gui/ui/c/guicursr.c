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
#include "guixutil.h"

/*
 * GUISetCursorPos
 */

bool GUIAPI GUISetCursorPos( gui_window *wnd, const gui_point *point )
{
    guix_ord    scr_x;
    guix_ord    scr_y;

    if( wnd->style & GUI_CURSOR ) {
        scr_x = GUIScaleToScreenH( point->x );
        scr_y = GUIScaleToScreenV( point->y );
        if( GUI_DO_HSCROLL( wnd ) ) {
            scr_x -= wnd->hgadget->pos;
        }
        if( GUI_DO_VSCROLL( wnd ) ) {
            scr_y -= wnd->vgadget->pos;
        }
        if( ( scr_y < wnd->use.height ) && ( scr_x < wnd->use.width ) ) {
            wnd->vs.cursor_row = scr_y + wnd->use.row;
            wnd->vs.cursor_col = scr_x + wnd->use.col;
            return( GUISetCursor( wnd ) );
        }
    }
    return( false );
}

bool GUIAPI GUIGetCursorPos( gui_window *wnd, gui_point *point )
{
    guix_ord    scr_x;
    guix_ord    scr_y;

    if( ( point == NULL ) || (wnd->style & GUI_CURSOR) == 0 ) {
        return( false );
    }
    scr_x = wnd->vs.cursor_col - wnd->use.col;
    scr_y = wnd->vs.cursor_row - wnd->use.col;
    if( GUI_DO_HSCROLL( wnd ) ) {
        scr_x += wnd->hgadget->pos;
    }
    if( GUI_DO_VSCROLL( wnd ) ) {
        scr_y += wnd->vgadget->pos;
    }
    point->x = GUIScreenToScaleH( scr_x );
    point->y = GUIScreenToScaleV( scr_y );
    return( true );
}

bool GUIAPI GUIGetCursorType( gui_window *wnd, gui_cursor_type *cursor )
{
    if( (wnd->style & GUI_CURSOR) == 0 || ( cursor == NULL ) ) {
        return( false );
    }
    switch( wnd->vs.cursor_type ) {
    case C_OFF:
        *cursor = GUI_NO_CURSOR;
        break;
    case C_NORMAL:
        *cursor = GUI_NORMAL_CURSOR;
        break;
    case C_INSERT:
        *cursor = GUI_INSERT_CURSOR;
        break;
    default:
        return( false );
    }
    return( true );
}

bool GUIAPI GUISetCursorType( gui_window *wnd, gui_cursor_type cursor )
{
    CURSOR_TYPE type;

    switch( cursor ) {
    case GUI_NO_CURSOR:
        type = C_OFF;
        break;
    case GUI_NORMAL_CURSOR:
        type = C_NORMAL;
        break;
    case GUI_INSERT_CURSOR:
        type = C_INSERT;
        break;
    default:
        return( false );
    }
    wnd->vs.cursor_type = type;
    return( GUISetCursor( wnd ) );
}
