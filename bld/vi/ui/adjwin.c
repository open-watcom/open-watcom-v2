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
#include "dosx.h"

static char_info    *tmpImage;

/*
 * drawTmpBorder - display border
 */
static void drawTmpBorder( int color, windim x1, windim y1, windim x2, windim y2 )
{
    windim              i;
    size_t              tl, bl, tr, br, k;
    windim              height, width;
    char_info           what = {0, 0};

    height = y2 - y1 + 1;
    width = x2 - x1 + 1;
    tl = x1 + y1 * EditVars.WindMaxWidth;
    tr = x2 + y1 * EditVars.WindMaxWidth;
    bl = x1 + y2 * EditVars.WindMaxWidth;
    br = x2 + y2 * EditVars.WindMaxWidth;
    what.cinfo_attr = MAKE_ATTR( NULL, color, BLACK );

    /*
     * corners
     */
    what.cinfo_char = WindowBordersNG[WB_TOPLEFT];
    WRITE_SCREEN( Scrn[tl], what );
    what.cinfo_char = WindowBordersNG[WB_TOPRIGHT];
    WRITE_SCREEN( Scrn[tr], what );
    what.cinfo_char = WindowBordersNG[WB_BOTTOMLEFT];
    WRITE_SCREEN( Scrn[bl], what );
    what.cinfo_char = WindowBordersNG[WB_BOTTOMRIGHT];
    WRITE_SCREEN( Scrn[br], what );

    /*
     * sides, top and bottom
     */
    what.cinfo_char = WindowBordersNG[WB_LEFTSIDE];
    k = EditVars.WindMaxWidth;
    for( i = 1; i < height - 1; i++) {
        WRITE_SCREEN( Scrn[tr + k], what );
        WRITE_SCREEN( Scrn[tl + k], what );
#ifdef __VIO__
        MyVioShowBuf( tr + k, 1 );
        MyVioShowBuf( tl + k, 1 );
#endif
        k += EditVars.WindMaxWidth;
    }
    what.cinfo_char = WindowBordersNG[WB_TOPBOTTOM];
    for( i = 1; i < width - 1; i++ ) {
        WRITE_SCREEN( Scrn[bl + i], what );
    }
    for( i = 1; i < width - 1; i++ ) {
        WRITE_SCREEN( Scrn[tl + i], what );
    }
#ifdef __VIO__
    MyVioShowBuf( tl, width );
    MyVioShowBuf( bl, width );
#endif

} /* drawTmpBorder */

/*
 * swapTmp - exchange border and tmpImage
 */
static void swapTmp( char_info _FAR *src, char_info _FAR *dest, windim x1, windim y1, windim x2, windim y2 )
{
    windim  i;
    size_t  tl, bl, tr, br, k;
    windim  height, width;

    height = y2 - y1 + 1;
    width = x2 - x1 + 1;
    tl = x1 + y1 * EditVars.WindMaxWidth;
    tr = x2 + y1 * EditVars.WindMaxWidth;
    bl = x1 + y2 * EditVars.WindMaxWidth;
    br = x2 + y2 * EditVars.WindMaxWidth;

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
    k = EditVars.WindMaxWidth;
    for( i = 1; i < height - 1; i++ ) {
        WRITE_SCREEN2( dest[tr + k], src[tr + k] );
        WRITE_SCREEN2( dest[tl + k], src[tl + k] );
#ifdef __VIO__
        MyVioShowBuf( tr + k, 1 );
        MyVioShowBuf( tl + k, 1 );
#endif
        k += EditVars.WindMaxWidth;
    }
    for( i = 1; i < width - 1; i++ ) {
        WRITE_SCREEN2( dest[bl + i], src[bl + i] );
        WRITE_SCREEN2( dest[tl + i], src[tl + i] );
    }
#ifdef __VIO__
    MyVioShowBuf( tl, width );
    MyVioShowBuf( bl, width );
#endif

} /* swapTmp */


/*
 * dickWithAWindow - resize window based on keys/mouse
 */
static void dickWithAWindow( window_id wid, bool topcorner, bool move, bool *doresize,
                             winarea *wd, bool mouse )
{
    vi_key      key;
    bool        done = false;
    windim      x1, x2, y1, y2;
    windim      nx1, nx2, ny1, ny2;
    int         mrow = 0;
    int         mcol = 0;
    windim      dx, dy;
    int         bclr;
    window      *w;

    /*
     * get original window dimensions, save copy of old border
     */
    if( EditFlags.Quiet ) {
        return;
    }
    if( mouse ) {
        DisplayMouse( false );
    }
    *doresize = false;
    w = WINDOW_FROM_ID( wid );
    tmpImage = MemAlloc( EditVars.WindMaxWidth * EditVars.WindMaxHeight * sizeof( char_info ) );
    x1 = w->area.x1;
    x2 = w->area.x2;
    y1 = w->area.y1;
    y2 = w->area.y2;
    if( move ) {
        bclr = EditVars.MoveColor;
    } else {
        bclr = EditVars.ResizeColor;
    }
    swapTmp( Scrn, tmpImage, x1, y1, x2, y2 );
    drawTmpBorder( bclr, x1, y1, x2, y2 );

    /*
     * engage in resizing
     */
    for( ;; ) {

        /*
         * get keyboard info
         */
        if( mouse ) {
            mrow = MouseRow;
            mcol = MouseCol;
            DisplayMouse( true );
        }
        for( ;; ) {
            key = GetNextEvent( true );
            dy = dx = 0;
            if( key == VI_KEY( ESC ) ) {
                done = true;
            } else {
                if( !mouse ) {
                    switch( key ) {
                    case VI_KEY( UP ):
                        dy = -1;
                        topcorner = false;
                        break;
                    case VI_KEY( DOWN ):
                        dy = +1;
                        topcorner = false;
                        break;
                    case VI_KEY( LEFT ):
                        dx = -1;
                        topcorner = false;
                        break;
                    case VI_KEY( RIGHT ):
                        dx = +1;
                        topcorner = false;
                        break;
                    case VI_KEY( SHIFT_UP ):
                        dy = -1;
                        topcorner = true;
                        break;
                    case VI_KEY( SHIFT_DOWN ):
                        dy = +1;
                        topcorner = true;
                        break;
                    case VI_KEY( SHIFT_LEFT ):
                        dx = -1;
                        topcorner = true;
                        break;
                    case VI_KEY( SHIFT_RIGHT ):
                        dx = +1;
                        topcorner = true;
                        break;
                    }
                } else {
                    if( key == VI_KEY( MOUSEEVENT ) ) {
                        if( LastMouseEvent == MOUSE_DRAG ) {
                            break;
                        } else if( LastMouseEvent == MOUSE_RELEASE ) {
                            done = true;
                        }
                    }
                }
            }
            if( dx != 0 || dy != 0 || done ) {
                break;
            }
        }
        if( mouse ) {
            DisplayMouse( false );
        }

        /*
         * check if we should exit
         */
        if( done ) {
            swapTmp( tmpImage, Scrn, x1, y1, x2, y2 );
            MemFree( tmpImage );
            if( *doresize ) {
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
                dy = MouseRow - mrow;
                dx = MouseCol - mcol;
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
        if( ValidDimension( nx1, ny1, nx2, ny2, w->has_border ) ) {
            swapTmp( tmpImage, Scrn, x1, y1, x2, y2 );
            x1 = nx1; x2 = nx2; y1 = ny1; y2 = ny2;
            swapTmp( Scrn, tmpImage, x1, y1, x2, y2 );
            drawTmpBorder( bclr, x1, y1, x2, y2 );
            *doresize = true;
        }

    }

} /* dickWithAWindow */

/*
 * dickWithCurrentWindow - set up to play with current window
 */
static vi_rc dickWithCurrentWindow( bool topcorner, bool move, bool mouse )
{
    bool        resize;
    winarea     w;
    vi_rc       rc;

    resize = false;
    w.x1 = 0;
    w.y1 = 0;
    w.x2 = 0;
    w.y2 = 0;
    dickWithAWindow( current_window_id, topcorner, move, &resize, &w, mouse );
    if( resize ) {
        rc = ResizeCurrentWindow( w.x1, w.y1, w.x2, w.y2 );
        if( mouse ) {
            DisplayMouse( true );
        }
        return( rc );
    }
    return( ERR_NO_ERR );

} /* dickWithCurrentWindow */

/*
 * routines to resize/move current window
 */
vi_rc ResizeCurrentWindowWithKeys( void ) { return( dickWithCurrentWindow( false, false, false ) ); }
vi_rc MoveCurrentWindowWithKeys( void ) { return( dickWithCurrentWindow( false, true, false ) ); }
vi_rc ResizeCurrentWindowWithMouse( void ) { return( dickWithCurrentWindow( false, false, true ) ); }
vi_rc MoveCurrentWindowWithMouse( void ) { return( dickWithCurrentWindow( false, true, true ) ); }
