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
* Description:  Window moving and resizing.
*
****************************************************************************/


#include "guiwind.h"
#include "guismove.h"
#include "guisize.h"

static resize_dir Direction;

/* statics for mouse move and resize */
static gui_point MoveResizeAnchor = { 0, 0 };

/* statics for keyboard move and resize */
static gui_window *MoveSizeWnd = NULL;
static gui_point   MoveSizeChange       = { 0, 0 };

/*
 * StartMoveResize -- start move or resize for the given window
 */

static void StartMoveResize( gui_window *wnd, resize_dir dir )
{
    SAREA       area;

    area = wnd->vs.area;
    area.width--;
    area.height--;
    uimenudisable( true );
    uibandinit( area, WNDATTR( wnd, GUI_FRAME_RESIZE ) );
    Direction = dir;
}

/*
 * GUIStartMoveResize -- start mouse move or resize
 */

bool GUIStartMoveResize( gui_window *wnd, ORD row, ORD col, resize_dir dir )
{
    MoveResizeAnchor.x = col;
    MoveResizeAnchor.y = row;
    StartMoveResize( wnd, dir );
    return( true );
}

/*
 * GUIStartKeyboardMoveResize
 */

void GUIStartKeyboardMoveResize( gui_window *wnd, bool move )
{
    MoveSizeWnd = wnd;
    MoveSizeChange.x = 0;
    MoveSizeChange.y = 0;
    if( move ) {
        StartMoveResize( wnd, RESIZE_NONE );
    } else {
        StartMoveResize( wnd, RESIZE_DOWN | RESIZE_RIGHT );
    }
}

/*
 * DoMoveResize -- do move or resize, with mouse or keyboard
 */

static void DoMoveResize( gui_window *wnd, int delta_x, int delta_y, bool finish, gui_point *adjust )
{
    SAREA       area;
    SAREA       new;
    int         start_delta_x;
    int         start_delta_y;
    gui_flags   flags;

    start_delta_x = delta_x;
    start_delta_y = delta_y;
    area = wnd->vs.area;
    new = area;
    if( Direction == RESIZE_NONE ) {
        if( delta_y < 0 && -delta_y > area.row ) {
            new.row = 0;
        } else {
            new.row = area.row + delta_y;
        }
        if( delta_x < 0 && -delta_x > area.col ) {
            new.col = 0;
        } else {
            new.col = area.col + delta_x;
        }
        new.width = area.width;
        new.height = area.height;
    } else { /* resize */
        if( Direction & RESIZE_RIGHT ) {
            new.col = area.col;
            if( delta_x < 0 && -delta_x > area.width ) {
                new.width = 0;
            } else {
                new.width = area.width + delta_x;
            }
        } else if( Direction & RESIZE_LEFT ) {
            if( delta_x < 0 ) {
                if( -delta_x > area.col ) {
                    delta_x = -area.col;
                }
            } else if( delta_x > area.width ) {
                delta_x = area.width;
            }
            new.col = area.col + delta_x;
            new.width = area.width - delta_x;
        } else {
            new.col = area.col;
            new.width = area.width;
        }
        if( Direction & RESIZE_DOWN ) {
            new.row = area.row;
            if( delta_y < 0 && -delta_y > area.height ) {
                new.height = 0;
            } else {
                new.height = area.height + delta_y;
            }
        } else if( Direction & RESIZE_UP ) {
            if( delta_y < 0 ) {
                if( -delta_y > area.row ) {
                    delta_y = -area.row;
                }
            } else if( delta_y > area.height ) {
                delta_y = area.height;
            }
            new.row = area.row + delta_y;
            new.height = area.height - delta_y;
        } else {
            new.row = area.row;
            new.height = area.height;
        }
    }
    GUICheckArea( &new, Direction );
    if( Direction == RESIZE_NONE ) {
        delta_x = new.col - wnd->vs.area.col;
        delta_y = new.row - wnd->vs.area.row;
        GUICheckMove( wnd, &delta_y, &delta_x );
        new.row = wnd->vs.area.row + delta_y;
        new.col = wnd->vs.area.col + delta_x;
    } else { /* resize */
        GUICheckResizeAreaForChildren( wnd, &new, Direction );
        GUICheckResizeAreaForParent( wnd, &new, Direction );
        delta_x = new.width - area.width;
        delta_y = new.height - area.height;
    }
    if( adjust != NULL ) {
        adjust->x = delta_x - start_delta_x;
        adjust->y = delta_y - start_delta_y;
    }
    new.width --;
    new.height --;
    uibandmove( new );
    new.width ++;
    new.height ++;
    if( finish ) {
        uimenudisable( false );
        uibandfini();
        if( GUI_WND_MAXIMIZED( wnd ) ) {
            flags = MAXIMIZED;
        } else {
            flags = NONE;
        }
        GUIWndMoveSize( wnd, &new, flags, Direction );
    }
}

/*
 * MoveResizeCancel -- cancel a move or resize
 */

static void MoveResizeCancel( void )
{
    uimenudisable( false );
    uibandfini();
}

/*
 * GUIDoKeyboardMoveResize
 */

bool GUIDoKeyboardMoveResize( ui_event ui_ev )
{
    bool        finish;
    bool        cancel;
    gui_point   adjust;

    if( MoveSizeWnd == NULL ) {
        return( false );
    }
    finish = false;
    cancel = false;
    switch( ui_ev ) {
    case EV_CURSOR_UP:
        MoveSizeChange.y--;
        break;
    case EV_CURSOR_DOWN:
        MoveSizeChange.y++;
        break;
    case EV_CURSOR_RIGHT:
        MoveSizeChange.x++;
        break;
    case EV_CURSOR_LEFT:
        MoveSizeChange.x--;
        break;
    case EV_ESCAPE:
        MoveResizeCancel();
        cancel = true;
        /* fall through */
    case EV_ENTER:
        finish = true;
        break;
    default:
        return( true );
    }
    if( !cancel ) {
        DoMoveResize( MoveSizeWnd, MoveSizeChange.x, MoveSizeChange.y, finish, &adjust );
        MoveSizeChange.x += adjust.x;
        MoveSizeChange.y += adjust.y;
    }
    if( finish ) {
        MoveSizeWnd = NULL;
    }
    return( true );
}

/*
 * GUIDoMoveResize -- do a mouse move or resize
 */

bool GUIDoMoveResize( gui_window *wnd, int row, int col, ui_event ui_ev, gui_point *adjust )
{
    bool        finish;
    int         delta_x;
    int         delta_y;

    delta_x = col - MoveResizeAnchor.x;
    delta_y = row - MoveResizeAnchor.y;
    finish = ( ui_ev == EV_MOUSE_RELEASE ) || ( ui_ev == EV_MOUSE_RELEASE_R ) || ( ui_ev == EV_ENTER );
    DoMoveResize( wnd, delta_x, delta_y, finish, adjust );
    return( true );
}

/*
 * GUIDoMoveResizeCheck -- check if event affects mouse move or resize
 */

bool GUIDoMoveResizeCheck( gui_window * wnd, ui_event ui_ev, ORD row, ORD col )
{
    switch( ui_ev ) {
    case EV_ENTER:
        GUIDoMoveResize( wnd, row, col, ui_ev, NULL );
        break;
    case EV_ESCAPE:
        MoveResizeCancel();
        break;
    default:
        return( false );
    }
    return( true );
}
