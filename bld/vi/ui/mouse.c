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


#include <stdio.h>
#include <stdlib.h>
#include "vi.h"
#include "mouse.h"
#include "win.h"

static long mouseTime;
static int lastButton;
static int oldRow=-1;
static int oldCol=-1;
static char oldAttr;
static bool mouseOn;
static bool mouseRepeat;

/*
 * getButton - return an integer representing the mouse button
 */
static int getButton( int status )
{
    status &= MOUSE_ANY_BUTTON_DOWN;
    if( status == MOUSE_LEFT_BUTTON_DOWN ) {
        return( 0 );
    } else if( status == MOUSE_RIGHT_BUTTON_DOWN ) {
        return( 1 );
    } else {
        return( 2 );
    }

} /* getButton */

/*
 * GetMouseEvent - get a mouse event
 */
int GetMouseEvent( void )
{
#ifdef __CURSES__
    return( -1 );
#else
    int         status;
    int         row,col;
    bool        moved;
    int         me;
    int         button;
    int         diff;

    if( !EditFlags.UseMouse ) {
        return( -1 );
    }

    PollMouse( &status, &row, &col );

    // in a windowed OS values can be beyond screen boundries
    // this confuses us so disallow it.
    row = max( row, 0 );
    col = max( col, 0 );
    row = min( row, WindMaxHeight-1 );
    col = min( col, WindMaxWidth-1 );

    moved = ( row != MouseRow || col != MouseCol );
    diff = (status ^ MouseStatus) & MOUSE_ANY_BUTTON_DOWN;

    me = -1;
    if( moved ) {
        lastButton = -1;
        if( MouseStatus & MOUSE_ANY_BUTTON_DOWN ){
            button = getButton( status );
            me = MOUSE_DRAG;
        } else {
            me = MOUSE_MOVE;
            mouseOn = TRUE;
        }
    } else if( diff & MOUSE_ANY_BUTTON_DOWN ) {
        if( (diff & status) == diff ) {
            if( getButton( diff ) == lastButton &&
                (ClockTicks - mouseTime) < MouseDoubleClickSpeed ) {
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
        mouseRepeat = FALSE;
        mouseTime  = ClockTicks;
        mouseOn = TRUE;
    } else if( status & MOUSE_ANY_BUTTON_DOWN ) {
        button = getButton( status );
        if( !mouseRepeat ){
            if( ClockTicks - mouseTime > MouseRepeatStartDelay ){
                me = MOUSE_REPEAT;
                mouseRepeat = TRUE;
                mouseTime = ClockTicks;
            }
        } else if( ClockTicks - mouseTime > MouseRepeatDelay ){
            me = MOUSE_REPEAT;
            mouseTime = ClockTicks;
        }
    }
    if( EditFlags.LeftHandMouse ) {
        if( button == 0 ) {
            button = 1;
        } else if( button == 1 ) {
            button = 0;
        }
    }

    if( me >= 0 && me != MOUSE_MOVE ) {
        me += button*6;
    }
    MouseRow = row;
    MouseCol = col;
    return( me );
#endif

} /* GetMouseEvent */

/*
 * drawMouseCursor - draw mouse at specified row, col
 */
static void drawMouseCursor( int row, int col )
{
    char_info   _FAR *ptr;

    if( mouseOn ) {
        ptr = (char_info _FAR *) &Scrn[ sizeof( char_info ) *((row)*WindMaxWidth + col) ];
        oldAttr = ptr->attr;
        if( EditFlags.Monocolor ) {
            ptr->attr = (oldAttr & 0x79) ^ 0x71;
        } else {
            ptr->attr = (oldAttr & 0x7f) ^ 0x77;
        }
        oldRow = row;
        oldCol = col;
#ifdef __VIO__
        MyVioShowBuf( (char _FAR *) ptr - Scrn, 1 );
#endif
    }

} /* drawMouseCursor */

/*
 * eraseMouseCursor - erase the mouse, if it is visible
 */
static void eraseMouseCursor( void )
{
    char_info   _FAR *ptr;

    if( mouseOn && oldRow >= 0 ) {
        ptr = (char_info _FAR *) &Scrn[ sizeof( char_info ) *((oldRow)*WindMaxWidth + oldCol) ];
        ptr->attr = oldAttr;
#ifdef __VIO__
        MyVioShowBuf( (char _FAR *) ptr - Scrn, 1 );
#endif
    }

} /* eraseMouseCursor */

/*
 * RedrawMouse - redraw the mouse, at specified position
 */
void RedrawMouse( int row, int col )
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
int DisplayMouse( int flag )
{
    int lastmouse;

    lastmouse = mouseOn;
    if( EditFlags.HasSystemMouse ) {
        return( lastmouse );
    }
    if( flag ) {
        if( oldRow == -1 ) {
            mouseOn = TRUE;
            drawMouseCursor( MouseRow, MouseCol );
        } else {
            if( !mouseOn ) {
                mouseOn = TRUE;
                drawMouseCursor( oldRow, oldCol );
            }
        }
    } else {
        eraseMouseCursor();
        oldRow = -1;
        oldCol = -1;
        mouseOn = FALSE;
    }
    return( lastmouse );

} /* DisplayMouse */
