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


#include "vi.h"
#include "mouse.h"
#include "win.h"


#define MOUSE_LEFT_BUTTON          0
#define MOUSE_RIGHT_BUTTON         1
#define MOUSE_MIDDLE_BUTTON        2

static long         mouseTime;
static int          lastButton;
static windim       oldRow = -1;
static windim       oldCol = -1;
static viattr_t     oldAttr;
static bool         mouseOn;
static bool         mouseRepeat;

/*
 * getButton - return an integer representing the mouse button
 */
static int getButton( int status )
{
    status &= MOUSE_ANY_BUTTON_DOWN;
    if( status == MOUSE_LEFT_BUTTON_DOWN ) {
        return( MOUSE_LEFT_BUTTON );
    } else if( status == MOUSE_RIGHT_BUTTON_DOWN ) {
        return( MOUSE_RIGHT_BUTTON );
    } else {
        return( MOUSE_MIDDLE_BUTTON );
    }

} /* getButton */

static vi_mouse_event mapButtonEvents( vi_mouse_event me, int button )
{
    if( EditFlags.LeftHandMouse ) {
        if( button == MOUSE_LEFT_BUTTON ) {
            button = MOUSE_RIGHT_BUTTON;
        } else if( button == MOUSE_RIGHT_BUTTON ) {
            button = MOUSE_LEFT_BUTTON;
        }
    }
    if( button != MOUSE_LEFT_BUTTON ) {
        if( me == MOUSE_PRESS ) {
            me = (button == MOUSE_RIGHT_BUTTON) ? MOUSE_PRESS_R : MOUSE_PRESS_M;
        } else if( me == MOUSE_RELEASE ) {
            me = (button == MOUSE_RIGHT_BUTTON) ? MOUSE_RELEASE_R : MOUSE_RELEASE_M;
        } else if( me == MOUSE_DCLICK ) {
            me = (button == MOUSE_RIGHT_BUTTON) ? MOUSE_DCLICK_R : MOUSE_DCLICK_M;
        } else if( me == MOUSE_HOLD ) {
            me = (button == MOUSE_RIGHT_BUTTON) ? MOUSE_HOLD_R : MOUSE_HOLD_M;
        } else if( me == MOUSE_DRAG ) {
            me = (button == MOUSE_RIGHT_BUTTON) ? MOUSE_DRAG_R : MOUSE_DRAG_M;
        } else if( me == MOUSE_REPEAT ) {
            me = (button == MOUSE_RIGHT_BUTTON) ? MOUSE_REPEAT_R : MOUSE_REPEAT_M;
        }
    }
    return( me );
}

/*
 * GetMouseEvent - get a mouse event
 */
vi_mouse_event GetMouseEvent( void )
{
#ifdef __CURSES__
    return( MOUSE_NONE );
#else
    int             status;
    windim          row, col;
    bool            moved;
    vi_mouse_event  me;
    int             button;
    int             diff;

    if( !EditFlags.UseMouse ) {
        return( MOUSE_NONE );
    }

    PollMouse( &status, &row, &col );

    // in a windowed OS values can be beyond screen boundries
    // this confuses us so disallow it.
    if( row > EditVars.WindMaxHeight - 1 )
        row = EditVars.WindMaxHeight - 1;
    if( row < 0 )
        row = 0;
    if( col > EditVars.WindMaxWidth - 1 )
        col = EditVars.WindMaxWidth - 1;
    if( col < 0 )
        col = 0;

    moved = (row != MouseRow || col != MouseCol);
    diff = (status ^ MouseStatus) & MOUSE_ANY_BUTTON_DOWN;

    me = MOUSE_NONE;
    button = 0;
    if( moved ) {
        lastButton = -1;
        if( MouseStatus & MOUSE_ANY_BUTTON_DOWN ) {
            button = getButton( status );
            me = MOUSE_DRAG;
        } else {
            me = MOUSE_MOVE;
            mouseOn = true;
        }
    } else if( diff & MOUSE_ANY_BUTTON_DOWN ) {
        if( (diff & status) == diff ) {
            if( getButton( diff ) == lastButton &&
                (ClockTicks - mouseTime) < EditVars.MouseDoubleClickSpeed ) {
                me = MOUSE_DCLICK;
            } else {
                me = MOUSE_PRESS;
                lastButton = getButton( diff );
            }
        } else {
            me = MOUSE_RELEASE;
        }
        button = getButton( diff );
        MouseStatus = status;
        mouseRepeat = false;
        mouseTime = ClockTicks;
        mouseOn = true;
    } else if( status & MOUSE_ANY_BUTTON_DOWN ) {
        button = getButton( status );
        if( !mouseRepeat ){
            if( ClockTicks - mouseTime > EditVars.MouseRepeatStartDelay ) {
                me = MOUSE_REPEAT;
                mouseRepeat = true;
                mouseTime = ClockTicks;
            }
        } else if( ClockTicks - mouseTime > EditVars.MouseRepeatDelay ) {
            me = MOUSE_REPEAT;
            mouseTime = ClockTicks;
        }
    }
    MouseRow = row;
    MouseCol = col;

    return( mapButtonEvents( me, button ) );
#endif

} /* GetMouseEvent */

/*
 * drawMouseCursor - draw mouse at specified row, col
 */
static void drawMouseCursor( windim row, windim col )
{
    size_t      oscr;

    if( mouseOn ) {
        oscr = row * EditVars.WindMaxWidth + col;
        oldAttr = Scrn[oscr].cinfo_attr;
        if( EditFlags.Monocolor ) {
            Scrn[oscr].cinfo_attr = (oldAttr & 0x79) ^ 0x71;
        } else {
            Scrn[oscr].cinfo_attr = (oldAttr & 0x7f) ^ 0x77;
        }
        oldRow = row;
        oldCol = col;
#ifdef __VIO__
        MyVioShowBuf( oscr, 1 );
#endif
    }

} /* drawMouseCursor */

/*
 * eraseMouseCursor - erase the mouse, if it is visible
 */
static void eraseMouseCursor( void )
{
    size_t      oscr;

    if( mouseOn && oldRow >= 0 ) {
        oscr = oldRow * EditVars.WindMaxWidth + oldCol;
        Scrn[oscr].cinfo_attr = oldAttr;
#ifdef __VIO__
        MyVioShowBuf( oscr, 1 );
#endif
    }

} /* eraseMouseCursor */

/*
 * RedrawMouse - redraw the mouse, at specified position
 */
void RedrawMouse( windim row, windim col )
{
    if( oldRow == row && oldCol == col || EditFlags.HasSystemMouse ) {
        return;
    }
    eraseMouseCursor();
    drawMouseCursor( row, col );

} /* RedrawMouse */

/*
 * DisplayMouse - control whether mouse is visible or not
 */
bool DisplayMouse( bool flag )
{
    bool    lastmouse;

    lastmouse = mouseOn;
    if( EditFlags.HasSystemMouse ) {
        return( lastmouse );
    }
    if( flag ) {
        if( oldRow == -1 ) {
            mouseOn = true;
            drawMouseCursor( MouseRow, MouseCol );
        } else {
            if( !mouseOn ) {
                mouseOn = true;
                drawMouseCursor( oldRow, oldCol );
            }
        }
    } else {
        eraseMouseCursor();
        oldRow = -1;
        oldCol = -1;
        mouseOn = false;
    }
    return( lastmouse );

} /* DisplayMouse */
