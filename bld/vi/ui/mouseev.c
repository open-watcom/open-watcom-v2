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
#ifdef __WATCOMC__
    #include <conio.h>
#endif
#include "mouse.h"
#include "win.h"

static mouse_hook  *hookHead;

/*
 * GetMousePosInfo - get position of mouse in window
 */
window_id GetMousePosInfo( int *win_x, int *win_y )
{
    *win_x = MouseCol;
    *win_y = MouseRow;
    return( WhoIsUnder( win_x, win_y ) );

} /* GetMousePosInfo */

/*
 * TestMouseEvent - make sure mouse event should be sent back
 */
bool TestMouseEvent( bool usemouse )
{
    int         win_x, win_y;
    window_id   id;
    bool        rc;

    if( hookHead == NULL ) {
        return( usemouse );
    }

    id = GetMousePosInfo( &win_x, &win_y );

    rc = hookHead->cb( id, win_x, win_y );
    if( !usemouse ) {
        return( false );
    }
    return( rc );

} /* TestMouseEvent */

/*
 * PushMouseEventHandler - set up callbacks for receiving mouse events
 */
void PushMouseEventHandler( mouse_callback cb )
{
    mouse_hook          *mh;

    mh = MemAlloc( sizeof( mouse_hook ) );
    mh->cb = cb;
    mh->next = hookHead;
    hookHead = mh;

} /* PushMouseEventHandler */

/*
 * PopMouseEventHandler - pop current event handler
 */
void PopMouseEventHandler( void )
{
    mouse_hook  *mh;

    mh = hookHead;
    if( mh != NULL ) {
        hookHead = hookHead->next;
        MemFree( mh );
#ifndef NDEBUG
    } else {
        Message1( "Popped null mouse event handler!!!" );
        while( getchar() != ' ' ) {
            ;
        }
#endif
    }

} /* PopMouseEventHandler */
