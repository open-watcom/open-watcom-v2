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
#include "vi.h"
#include "win.h"

/*
 * ResetOverlap - set so no overlap of window
 */
void ResetOverlap( wind *w )
{
    char        *over,*whoover;
    int         i,j;

    AccessWindow( w->id );
    over = w->overlap;
    whoover = w->whooverlapping;

    for( j=w->y1; j<=w->y2; j ++ ) {
        for( i=w->x1; i<=w->x2; i++ ) {
            *over++ = NO_CHAR;
            *whoover++ = NO_CHAR;
        }
    }

    for( i=0;i<w->height;i++) {
        w->overcnt[i] = 0;
    }
    ReleaseWindow( w );

} /* ResetOverlap */

/*
 * MarkOverlap - mark who a "new" window has overlapped
 */
void MarkOverlap( window_id wn )
{
    wind        *w,*wo;
    int         i,j,k;
    char        *whoover,*img;

    w = AccessWindow( wn );
    whoover = w->whooverlapping;

    for( j=w->y1; j<=w->y2; j ++ ) {
        img = &ScreenImage[ w->x1 + j* WindMaxWidth ];
        for( i=w->x1; i<=w->x2; i++ ) {
            /*
             * if there is a character under us,
             * mark the window it belongs to as being overlapped,
             * and mark us as overlapping it
             */
            if( *img != NO_CHAR ) {
                wo = AccessWindow( *img );
                k = (i - wo->x1) + (j - wo->y1)*wo->width;
                wo->overlap[k] = (char) wn;
                wo->overcnt[ j - wo->y1 ]++;
                ReleaseWindow( wo );
            }
            *whoover = *img;
            *img = (char) wn;
            img++;
            whoover++;
        }
    }
    ReleaseWindow( w );

} /* MarkOverlap */

/*
 * RestoreOverlap - restore overlap information from a window that is
 *                  "going away" - either relocating or dying
 */
void RestoreOverlap( window_id wn, bool scrflag )
{
    wind                *w,*wo,*o;
    int                 i,j,k,l;
    char                *whoover,*over,*img;
    char_info           _FAR *scr;
#ifdef __VIO__
    unsigned            oscr;
#endif

    if( EditFlags.Quiet ) {
        scrflag = FALSE;
    }
    w = AccessWindow( wn );
    whoover = w->whooverlapping;
    over = w->overlap;

    for( j=w->y1; j<=w->y2; j ++ ) {
        if( scrflag) {
            scr = (char_info _FAR *) &Scrn[ (w->x1 + j* WindMaxWidth)*
                                                sizeof(char_info) ];
#ifdef __VIO__
            oscr = (unsigned) ((char *)scr - Scrn);
#endif
        }
        img = &ScreenImage[ w->x1 + j* WindMaxWidth ];
        for( i=w->x1; i<=w->x2; i++ ) {

            /*
             * if we are over someone, then reset the screen
             * with the proper information
             *
             * if we are not over someone, check for over us
             */
            if( *whoover != NO_CHAR ) {
                wo = AccessWindow( *whoover );
                k = (i - wo->x1) + (j - wo->y1)*wo->width;
                /*
                 * if we are being overlapped at the same
                 * spot, then point the guy overlapping us
                 * at the guy we are overlapping
                 *
                 * otherwise, mark the guy we are overlapping
                 * as not being overlapped, and restore his
                 * text to the screen
                 */
                if( *over != NO_CHAR ) {
                    o = AccessWindow( *over );
                    l = (i - o->x1) + (j - o->y1)*o->width;
                    o->whooverlapping[l] = *whoover;
                    wo->overlap[k] = *over;
                    ReleaseWindow( o );
                } else {
                    wo->overlap[k] = NO_CHAR;
                    wo->overcnt[ j - wo->y1 ]--;
                    if( scrflag ) {
                        WRITE_SCREEN( *scr, ((char_info *)wo->text)[k] );
                    }
                    *img = *whoover;
                }
                ReleaseWindow( wo );
            } else {
                /*
                 * we are not overlapping anyone, so
                 * see if anyone is overlapping us;
                 * if so, reset them to be not overlapping
                 * anyone
                 *
                 * if not, clear the screen
                 */
                if( *over != NO_CHAR ) {
                    o = AccessWindow( *over );
                    l = (i - o->x1) + (j - o->y1)*o->width;
                    o->whooverlapping[l] = NO_CHAR;
                    ReleaseWindow( o );
                } else {
                    if( scrflag ) {
                        WRITE_SCREEN( *scr, WindowNormalAttribute );
                    }
                    *img = NO_CHAR;
                }
            }
            img++;
            over++;
            whoover++;
            if( scrflag) {
                scr ++;
            }
        }
#ifdef __VIO__
        if( scrflag ) {
            MyVioShowBuf( oscr, w->width );
        }
#endif

    }
    ReleaseWindow( w );

} /* RestoreOverlap */

/*
 * TestOverlap - test if window is overlapped at all
 */
bool TestOverlap( window_id wn  )
{
    wind        *w;
    int         i;

    w = Windows[ wn ];
    for( i=0;i<w->height;i++) {
        if( w->overcnt[i] ) {
            return( TRUE );
        }
    }

    return( FALSE );

} /* TestOverlap */

/*
 * TestVisible - test if a window is visible at all
 */
bool TestVisible( wind *w  )
{
    int i;

    for( i=0;i<w->height;i++) {
        if( w->overcnt[i] != w->width ) {
            return( TRUE );
        }
    }

    return( FALSE );

} /* TestVisible */

/*
 * WindowIsVisible - check if given window id is visible
 */
bool WindowIsVisible( window_id id )
{
    wind        *w;

    w = Windows[ id ];
    return( TestVisible( w ) );

} /* WindowIsVisible */

/*
 * WhoIsUnder - determine who is under a given x,y, and return the real x,y
 */
window_id WhoIsUnder( int *x, int *y )
{
    window_id   id;
    wind        *w;
    int         win_x, win_y;

    id = ScreenImage[ (*x) + (*y) * WindMaxWidth ];
    if( id == NO_CHAR ) {
        return( id );
    }
    w = Windows[ id ];
    win_x = (*x) - w->x1;
    win_y = (*y) - w->y1;
    *x = win_x;
    *y = win_y;
    return( id );

} /* WhoIsUnder */
