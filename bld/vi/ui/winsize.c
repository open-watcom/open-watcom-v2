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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "vi.h"
#include "keys.h"
#include "win.h"
#include "dc.h"

/*
 * ResizeWindow - give a window a new size
 */
int ResizeWindow( window_id wn, int x1, int y1, int x2, int y2,
                        int scrflag )
{
    wind        *tmp,*w;
    int         bt,k;
    // char     *txt,*tptr;
//    char      *ot;
//    int       i, j;

    w = AccessWindow( wn );

    if( !ValidDimension( x1, y1, x2, y2, w->has_border ) ) {
        ReleaseWindow( w );
        return( ERR_WIND_INVALID );
    }

    tmp = AllocWindow( x1, y1, x2, y2, w->has_border, w->border_color1,
                        w->border_color2, w->text_color, w->background_color );
    tmp->id = wn;
    tmp->has_gadgets = w->has_gadgets;
    // txt = MemAlloc( w->width+1 );
    //tptr = txt;

    RestoreOverlap( wn, scrflag );

    Windows[wn] = tmp;
    tmp->accessed = TRUE;
    ResetOverlap( tmp );
    MarkOverlap( wn );

    /*
     * display the new text
     */
    k = 1;
    bt = (int) w->has_border;
    ClearWindow( wn );
    if( w->title != NULL ) {
        WindowTitle( wn, w->title );
    } else {
        DrawBorder( wn );
    }
#if 0
    for( j=bt;j<w->height-bt;j++ ) {

        ot = &(w->text[(j*w->width)*sizeof(char_info)]);
        for( i=bt;i<w->width-bt;i++ ) {
            *txt++ = ot[i*sizeof(char_info)];
        }
        *txt = 0;
        DisplayLineInWindow( wn,k++,tptr );
        txt = tptr;
    }
#else
    DCResize( CurrentInfo );
    DCDisplayAllLines();
    DCUpdate();
#endif

    FreeWindow( w );

    ReleaseWindow( tmp );
    return( ERR_NO_ERR );

} /* ResizeWindow */

/*
 * ResizeWindowRelative - resize current window with relative shifts
 */
int ResizeWindowRelative( window_id wn, int x1, int y1, int x2, int y2,
                int scrflag )
{
    wind        *w;
    int         rc;

    w = Windows[ wn ];
    rc = ResizeWindow( wn, w->x1+x1,w->y1+y1,w->x2+x2,w->y2+y2, scrflag );
    return( rc );

} /* ResizeWindowRelative */

/*
 * getMinSlot - find a minimize slot
 */
static int getMinSlot( void )
{
    int i;

    for( i=0;i<MAX_MIN_SLOTS;i++ ) {
        if( MinSlots[i] == 0 ) {
            MinSlots[i] = 1;
            return( i+1 );
        }
    }
    return( 0 );

} /* getMinSlot */

#define MIN_WIN_WIDTH   11
/*
 * MinimizeCurrentWindow - put next window into next minimize slot
 */
int MinimizeCurrentWindow( void )
{
    int i,j;
    int minx1,miny1;
    int rc;

    i = getMinSlot();
    if( !i ) {
        return( ERR_NO_ERR );
    }
    miny1 = WindMaxHeight-8;
    minx1 = 0;
    for( j=1;j<i;j++ ) {
        minx1 += MIN_WIN_WIDTH;
        if( minx1 >= WindMaxWidth-MIN_WIN_WIDTH ) {
            miny1 -= 3;
            if( miny1 < 0 ) {
                miny1 = WindMaxHeight-7;
            }
            minx1 = 0;
        }
    }
    rc = CurrentWindowResize( minx1, miny1,minx1+MIN_WIN_WIDTH-1,miny1+2 );
    WindowAuxUpdate( CurrentWindow, WIND_INFO_MIN_SLOT, i );
    return( rc );

} /* MinimizeCurrentWindow */

/*
 * MaximizeCurrentWindow - make current window full screen
 */
int MaximizeCurrentWindow( void )
{
    int rc;

    if( EditFlags.LineNumbers ) {
        if( EditFlags.LineNumsOnRight ) {
            rc = CurrentWindowResize( editw_info.x1, editw_info.y1,
                        editw_info.x2-LineNumWinWidth, editw_info.y2 );
        } else {
            rc = CurrentWindowResize( editw_info.x1+LineNumWinWidth,
                        editw_info.y1, editw_info.x2, editw_info.y2 );
        }
    } else {
        rc = CurrentWindowResize( editw_info.x1, editw_info.y1,
                        editw_info.x2, editw_info.y2 );
    }
    return( rc );

} /* MaximizeCurrentWindow */
