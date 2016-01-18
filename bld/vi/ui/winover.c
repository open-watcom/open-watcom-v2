/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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

/*
 * ResetOverlap - set so no overlap of window
 */
void ResetOverlap( window *w )
{
    window_id   *over;
    window_id   *whoover;
    int         i, j;

    AccessWindow( w );
    over = w->overlap;
    whoover = w->whooverlapping;

    for( j = w->y1; j <= w->y2; j++ ) {
        for( i = w->x1; i <= w->x2; i++ ) {
            *over++ = NO_WINDOW;
            *whoover++ = NO_WINDOW;
        }
    }

    for( i = 0; i < w->height; i++ ) {
        w->overcnt[i] = 0;
    }
    ReleaseWindow( w );

} /* ResetOverlap */

/*
 * MarkOverlap - mark who a "new" window has overlapped
 */
void MarkOverlap( window_id wn )
{
    window      *w, *wo;
    int         i, j, k;
    window_id   *whoover;
    window_id   *img;

    w = WINDOW_FROM_ID( wn );
    AccessWindow( w );
    whoover = w->whooverlapping;

    for( j = w->y1; j <= w->y2; j++ ) {
        img = &ScreenImage[w->x1 + j * EditVars.WindMaxWidth];
        for( i = w->x1; i <= w->x2; i++ ) {
            /*
             * if there is a character under us,
             * mark the window it belongs to as being overlapped,
             * and mark us as overlapping it
             */
            if( !BAD_ID( *img ) ) {
                wo = WINDOW_FROM_ID( *img );
                AccessWindow( wo );
                k = (i - wo->x1) + (j - wo->y1) * wo->width;
                wo->overlap[k] = wn;
                wo->overcnt[j - wo->y1]++;
                ReleaseWindow( wo );
            }
            *whoover = *img;
            *img = wn;
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
    window              *w, *wo, *o;
    int                 i, j, k, l;
    window_id           *whoover;
    window_id           *over;
    window_id           *img;
    char_info           _FAR *scr;
    unsigned            oscr;

    if( EditFlags.Quiet ) {
        scrflag = false;
    }
    w = WINDOW_FROM_ID( wn );
    AccessWindow( w );
    whoover = w->whooverlapping;
    over = w->overlap;
    scr = NULL;
    for( j = w->y1; j <= w->y2; j++ ) {
        oscr = w->x1 + j * EditVars.WindMaxWidth;
        if( scrflag ) {
            scr = &Scrn[oscr];
        }
        img = &ScreenImage[oscr];
        for( i = w->x1; i <= w->x2; i++ ) {

            /*
             * if we are over someone, then reset the screen
             * with the proper information
             *
             * if we are not over someone, check for over us
             */
            if( !BAD_ID( *whoover ) ) {
                wo = WINDOW_FROM_ID( *whoover );
                AccessWindow( wo );
                k = (i - wo->x1) + (j - wo->y1) * wo->width;
                /*
                 * if we are being overlapped at the same
                 * spot, then point the guy overlapping us
                 * at the guy we are overlapping
                 *
                 * otherwise, mark the guy we are overlapping
                 * as not being overlapped, and restore his
                 * text to the screen
                 */
                if( !BAD_ID( *over ) ) {
                    o = WINDOW_FROM_ID( *over );
                    AccessWindow( o );
                    l = (i - o->x1) + (j - o->y1) * o->width;
                    o->whooverlapping[l] = *whoover;
                    wo->overlap[k] = *over;
                    ReleaseWindow( o );
                } else {
                    wo->overlap[k] = NO_WINDOW;
                    wo->overcnt[j - wo->y1]--;
                    if( scrflag ) {
                        WRITE_SCREEN( *scr, wo->text[k] );
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
                if( !BAD_ID( *over ) ) {
                    o = WINDOW_FROM_ID( *over );
                    AccessWindow( o );
                    l = (i - o->x1) + (j - o->y1) * o->width;
                    o->whooverlapping[l] = NO_WINDOW;
                    ReleaseWindow( o );
                } else {
                    if( scrflag ) {
                        WRITE_SCREEN( *scr, WindowNormalAttribute );
                    }
                    *img = NO_WINDOW;
                }
            }
            img++;
            over++;
            whoover++;
            if( scrflag ) {
                scr++;
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
bool TestOverlap( window_id wn )
{
    window      *w;
    int         i;

    w = WINDOW_FROM_ID( wn );
    for( i = 0; i < w->height; i++ ) {
        if( w->overcnt[i] ) {
            return( true );
        }
    }

    return( false );

} /* TestOverlap */

/*
 * TestVisible - test if a window is visible at all
 */
bool TestVisible( window *w )
{
    int i;

    for( i = 0; i < w->height; i++ ) {
        if( w->overcnt[i] != w->width ) {
            return( true );
        }
    }

    return( false );

} /* TestVisible */

/*
 * WindowIsVisible - check if given window id is visible
 */
bool WindowIsVisible( window_id id )
{
    window      *w;

    w = WINDOW_FROM_ID( id );
    return( TestVisible( w ) );

} /* WindowIsVisible */

/*
 * WhoIsUnder - determine who is under a given x,y, and return the real x,y
 */
window_id WhoIsUnder( int *x, int *y )
{
    window_id   id;
    window      *w;
    int         win_x, win_y;

    id = ScreenImage[(*x) + (*y) * EditVars.WindMaxWidth];
    if( !BAD_ID( id ) ) {
        w = WINDOW_FROM_ID( id );
        win_x = (*x) - w->x1;
        win_y = (*y) - w->y1;
        *x = win_x;
        *y = win_y;
    }
    return( id );

} /* WhoIsUnder */
