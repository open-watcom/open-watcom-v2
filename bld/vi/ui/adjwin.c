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
#include "keys.h"
#include "colors.h"
#include "dosx.h"

static char *tmpImage;

/*
 * drawTmpBorder - display border
 */
static void drawTmpBorder( int color, int x1, int y1, int x2, int y2 )
{
    int                 i,tl,bl,tr,br,height,width,k;
    char_info           what;
    char_info           _FAR *scr;

    height = y2-y1+1;
    width = x2-x1+1;
    tl = x1 + y1*WindMaxWidth;
    tr = x2 + y1*WindMaxWidth;
    bl = x1 + y2*WindMaxWidth;
    br = x2 + y2*WindMaxWidth;
    what.attr = MAKE_ATTR( NULL, color, BLACK );

    /*
     * corners
     */
    scr = (char_info _FAR *) Scrn;
    what.ch = WindowBordersNG[WB_TOPLEFT];
    WRITE_SCREEN( scr[tl], what );
    what.ch = WindowBordersNG[WB_TOPRIGHT];
    WRITE_SCREEN( scr[tr], what );
    what.ch = WindowBordersNG[WB_BOTTOMLEFT];
    WRITE_SCREEN( scr[bl], what );
    what.ch = WindowBordersNG[WB_BOTTOMRIGHT];
    WRITE_SCREEN( scr[br], what );

    /*
     * sides, top and bottom
     */
    what.ch = WindowBordersNG[WB_LEFTSIDE];
    k = WindMaxWidth;
    for( i=1;i<height-1;i++) {
        WRITE_SCREEN( scr[tr+k], what );
        WRITE_SCREEN( scr[tl+k], what );
#ifdef __VIO__
        MyVioShowBuf( (unsigned) sizeof( char_info ) * (tr+k), 1 );
        MyVioShowBuf( (unsigned) sizeof( char_info ) * (tl+k), 1 );
#endif
        k += WindMaxWidth;
    }
    what.ch = WindowBordersNG[WB_TOPBOTTOM];
    for( i=1;i<width-1;i++ ) {
        WRITE_SCREEN( scr[bl+i], what );
    }
    for( i=1;i<width-1;i++ ) {
        WRITE_SCREEN( scr[tl+i], what );
    }
#ifdef __VIO__
    MyVioShowBuf( (unsigned) sizeof( char_info ) * tl, width );
    MyVioShowBuf( (unsigned) sizeof( char_info ) * bl, width );
#endif
} /* drawTmpBorder */

/*
 * swapTmp - exchange border and tmpImage
 */
static void swapTmp( char_info _FAR *src, char_info _FAR *dest, int x1, int y1, int x2, int y2 )
{
    int i,tl,bl,tr,br,height,width,k;

    height = y2-y1+1;
    width = x2-x1+1;
    tl = x1 + y1*WindMaxWidth;
    tr = x2 + y1*WindMaxWidth;
    bl = x1 + y2*WindMaxWidth;
    br = x2 + y2*WindMaxWidth;

    /*
     * corners
     */
    WRITE_SCREEN2( dest[tl], src[tl] );
    WRITE_SCREEN2( dest[tr], src[tr] );
    WRITE_SCREEN2( dest[bl], src[bl] );
    WRITE_SCREEN2( dest[br], src[br] );

    /*
     * sides, top and bottom
     */
    k = WindMaxWidth;
    for( i=1;i<height-1;i++) {
        WRITE_SCREEN2( dest[tr+k], src[ tr+k ] );
        WRITE_SCREEN2( dest[tl+k], src[ tl+k ] );
#ifdef __VIO__
        MyVioShowBuf( (unsigned) sizeof( char_info ) * (tr+k), 1 );
        MyVioShowBuf( (unsigned) sizeof( char_info ) * (tl+k), 1 );
#endif
        k += WindMaxWidth;
    }
    for( i=1;i<width-1;i++ ) {
        WRITE_SCREEN2( dest[bl+i], src[ bl+i ] );
        WRITE_SCREEN2( dest[tl+i], src[ tl+i ] );
    }
#ifdef __VIO__
    MyVioShowBuf( (unsigned) sizeof( char_info ) * tl, width );
    MyVioShowBuf( (unsigned) sizeof( char_info ) * bl, width );
#endif

} /* swapTmp */


/*
 * dickWithAWindow - resize window based on keys/mouse
 */
static void dickWithAWindow( int wn, bool topcorner, bool move, int *doresize,
                                windim *wd, bool mouse )
{
    int         key;
    bool        done=FALSE;
    int         x1,x2,y1,y2,nx1,nx2,ny1,ny2;
    int         mrow,mcol;
    int         dx,dy,bclr;
    wind        *cwd;

    /*
     * get original window dimensions, save copy of old border
     */
    if( EditFlags.Quiet ) {
        return;
    }
    if( mouse ) {
        DisplayMouse( FALSE );
    }
    *doresize = FALSE;
    cwd = Windows[ wn ];
    tmpImage = MemAlloc( WindMaxWidth * WindMaxHeight * sizeof(char_info) );
    x1 = cwd->x1;
    x2 = cwd->x2;
    y1 = cwd->y1;
    y2 = cwd->y2;
    if( move ) {
        bclr = MoveColor;
    } else {
        bclr = ResizeColor;
    }
    swapTmp( (char_info _FAR *) Scrn, (char_info _FAR *) tmpImage,x1,y1,x2,y2 );
    drawTmpBorder( bclr,x1,y1,x2,y2 );

    /*
     * engage in resizing
     */
    while( TRUE ) {

        /*
         * get keyboard info
         */
        if( mouse ) {
            mrow = MouseRow;
            mcol = MouseCol;
            DisplayMouse( TRUE );
        }
        while( 1 ) {
            key = GetNextEvent( TRUE );
            dy = dx = 0;
            if( key == VI_KEY( ESC ) ) {
                done = TRUE;
            } else {
                if( !mouse ) {
                    switch( key ) {
                    case VI_KEY( UP ):
                        dy = -1;
                        topcorner = FALSE;
                        break;
                    case VI_KEY( DOWN ):
                        dy = +1;
                        topcorner = FALSE;
                        break;
                    case VI_KEY( LEFT ):
                        dx = -1;
                        topcorner = FALSE;
                        break;
                    case VI_KEY( RIGHT ):
                        dx = +1;
                        topcorner = FALSE;
                        break;
                    case VI_KEY( SHIFT_UP ):
                        dy = -1;
                        topcorner = TRUE;
                        break;
                    case VI_KEY( SHIFT_DOWN ):
                        dy = +1;
                        topcorner = TRUE;
                        break;
                    case VI_KEY( SHIFT_LEFT ):
                        dx = -1;
                        topcorner = TRUE;
                        break;
                    case VI_KEY( SHIFT_RIGHT ):
                        dx = +1;
                        topcorner = TRUE;
                        break;
                    }
                } else {
                    if( key == VI_KEY( MOUSEEVENT ) ) {
                        if( LastMouseEvent == MOUSE_DRAG ) {
                            break;
                        } else if( LastMouseEvent == MOUSE_RELEASE ) {
                            done = TRUE;
                        }
                    }
                }
            }
            if( dx != 0 || dy != 0 || done ) {
                break;
            }
        }
        if( mouse ) {
            DisplayMouse( FALSE );
        }

        /*
         * check if we should exit
         */
        if( done ) {
            swapTmp( (char_info _FAR *) tmpImage, (char_info _FAR *) Scrn,x1,y1,x2,y2 );
            MemFree( tmpImage );
            if( (*doresize) ) {
                wd->x1 = x1;
                wd->x2 = x2;
                wd->y1 = y1;
                wd->y2 = y2;
            }
            return;
        }

        /*
         * adjust position
         */
        nx1 = x1; nx2 = x2; ny1 = y1; ny2 = y2;
        if( mouse ) {
            if( move ) {
                dy = MouseRow-mrow;
                dx = MouseCol-mcol;
                ny2 += dy;
                ny1 += dy;
                nx1 += dx;
                nx2 += dx;
            } else if( topcorner ) {
            } else {
                ny2 = MouseRow;
                nx2 = MouseCol;
            }
        } else if( dx || dy ) {
            if( move ) {
                ny2 += dy;
                ny1 += dy;
                nx1 += dx;
                nx2 += dx;
            } else if( topcorner ) {
                ny2 += dy;
                nx1 += dx;
            } else {
                ny1 += dy;
                nx2 += dx;
            }
        } else {
            continue;
        }

        /*
         * do the resize
         */
        if( ValidDimension( nx1, ny1, nx2, ny2, cwd->has_border  ) ) {
            swapTmp( (char_info _FAR *) tmpImage, (char_info _FAR *) Scrn,x1,y1,x2,y2 );
            x1 = nx1; x2 = nx2; y1 = ny1; y2 = ny2;
            swapTmp( (char_info _FAR *) Scrn, (char_info _FAR *) tmpImage,x1,y1,x2,y2 );
            drawTmpBorder( bclr,x1,y1,x2,y2 );
            *doresize = TRUE;
        }

    }

} /* dickWithAWindow */

/*
 * dickWithCurrentWindow - set up to play with current window
 */
static int dickWithCurrentWindow( bool topcorner, bool move, bool mouse )
{
    int         resize;
    windim      w;
    int         rc;

    dickWithAWindow( CurrentWindow, topcorner, move, &resize, &w, mouse );
    if( resize ) {
        rc = CurrentWindowResize( w.x1,w.y1,w.x2,w.y2 );
        if( mouse ) {
            DisplayMouse( TRUE );
        }
        return( rc );
    }
    return( ERR_NO_ERR );

} /* dickWithCurrentWindow */

/*
 * routines to resize/move current window
 */
int ResizeCurrentWindowWithKeys( void ) { return( dickWithCurrentWindow( FALSE, FALSE, FALSE ) ); }
int MoveCurrentWindowWithKeys( void ) { return( dickWithCurrentWindow( FALSE, TRUE, FALSE ) ); }
int ResizeCurrentWindowWithMouse( void ) { return( dickWithCurrentWindow( FALSE, FALSE, TRUE ) ); }
int MoveCurrentWindowWithMouse( void ) { return( dickWithCurrentWindow( FALSE, TRUE, TRUE ) ); }
