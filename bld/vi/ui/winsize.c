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
#include "win.h"
#include "dc.h"

/*
 * ResizeWindow - give a window a new size
 */
vi_rc ResizeWindow( window_id wn, int x1, int y1, int x2, int y2, bool scrflag )
{
    wind        *oldw;
//    int         bt, k;
//    char        *txt, *tptr;
//    char        *ot;
//    int         i, j;

    oldw = AccessWindow( wn );

    if( !ValidDimension( x1, y1, x2, y2, oldw->has_border ) ) {
        ReleaseWindow( oldw );
        return( ERR_WIND_INVALID );
    }
    RestoreOverlap( wn, scrflag );

    AllocWindow( wn, x1, y1, x2, y2, oldw->has_border, oldw->has_gadgets, true,
            oldw->border_color1, oldw->border_color2, oldw->text_color, oldw->background_color );
    MarkOverlap( wn );

    /*
     * display the new text
     */
    ClearWindow( wn );
    if( oldw->title != NULL ) {
        WindowTitle( wn, oldw->title );
    } else {
        DrawBorder( wn );
    }
    DCResize( CurrentInfo );
    DCDisplayAllLines();
    DCUpdate();

    FreeWindow( oldw );

    ReleaseWindow( Windows[wn] );
    return( ERR_NO_ERR );

} /* ResizeWindow */

/*
 * ResizeWindowRelative - resize current window with relative shifts
 */
vi_rc ResizeWindowRelative( window_id wn, int x1, int y1, int x2, int y2, bool scrflag )
{
    wind        *w;
    vi_rc       rc;

    w = Windows[wn];
    rc = ResizeWindow( wn, w->x1 + x1, w->y1 + y1, w->x2 + x2, w->y2 + y2, scrflag );
    return( rc );

} /* ResizeWindowRelative */

/*
 * getMinSlot - find a minimize slot
 */
static int getMinSlot( void )
{
    int i;

    for( i = 0; i < MAX_MIN_SLOTS; i++ ) {
        if( MinSlots[i] == 0 ) {
            MinSlots[i] = 1;
            return( i + 1 );
        }
    }
    return( 0 );

} /* getMinSlot */

#define MIN_WIN_WIDTH   11

/*
 * MinimizeCurrentWindow - put next window into next minimize slot
 */
vi_rc MinimizeCurrentWindow( void )
{
    int     i, j;
    int     minx1, miny1;
    vi_rc   rc;

    i = getMinSlot();
    if( !i ) {
        return( ERR_NO_ERR );
    }
    miny1 = EditVars.WindMaxHeight - 8;
    minx1 = 0;
    for( j = 1; j < i; j++ ) {
        minx1 += MIN_WIN_WIDTH;
        if( minx1 >= EditVars.WindMaxWidth - MIN_WIN_WIDTH ) {
            miny1 -= 3;
            if( miny1 < 0 ) {
                miny1 = EditVars.WindMaxHeight - 7;
            }
            minx1 = 0;
        }
    }
    rc = CurrentWindowResize( minx1, miny1, minx1 + MIN_WIN_WIDTH - 1, miny1 + 2 );
    WindowAuxUpdate( CurrentWindow, WIND_INFO_MIN_SLOT, i );
    return( rc );

} /* MinimizeCurrentWindow */

/*
 * MaximizeCurrentWindow - make current window full screen
 */
vi_rc MaximizeCurrentWindow( void )
{
    vi_rc   rc;

    if( EditFlags.LineNumbers ) {
        if( EditFlags.LineNumsOnRight ) {
            rc = CurrentWindowResize( editw_info.x1, editw_info.y1, editw_info.x2 - EditVars.LineNumWinWidth, editw_info.y2 );
        } else {
            rc = CurrentWindowResize( editw_info.x1 + EditVars.LineNumWinWidth, editw_info.y1, editw_info.x2, editw_info.y2 );
        }
    } else {
        rc = CurrentWindowResize( editw_info.x1, editw_info.y1, editw_info.x2, editw_info.y2 );
    }
    return( rc );

} /* MaximizeCurrentWindow */
