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


#include "gdefn.h"
#if !defined( _DEFAULT_WINDOWS )
#include <malloc.h>
#include "stkavail.h"


struct seg_entry {
    struct line_entry   line;
    short               end_pt;
    struct seg_entry    *link;
    short               delete;
};


static short            NumMinima;
static short            *MinList;
static struct seg_entry *LineList;
static struct seg_entry *FreeList;
static int              StackSize;
static char *           Stack;


static short NextPoint( short i, short dir, short n,
                        struct xycoord _WCI86FAR *points )
//===================================================

//  Find the next point after i, which has a different y value.

{
    short               p;
    short               y;

    y = points[ i ].ycoord;
    for( p = i + dir;; p += dir  ) {
        if( p == n ) {  // p %= n
            p = 0;
        } else if ( p < 0 ) {
            p = n - 1;
        }
        if( p == i ) {      // back to the start
            break;
        }
        if( points[ p ].ycoord != y ) {
            break;
        }
    }
    return( p );
}


static void CalcMinima( short n, struct xycoord _WCI86FAR *points )
//============================================================

//  Detect and store away all of the minimas. Sort the minima's
//  by y value.

{
    short               curr_y;
    short               i;
    short               j;
    short               start;
    short               next;
    short               prev;
    short               min;

    NumMinima = 0;
    start = NextPoint( 0, 1, n, points );
    i = start;
    do {
        next = NextPoint( i, 1, n, points );
        prev = NextPoint( i, -1, n, points );
        curr_y = points[ i ].ycoord;
        if( points[ prev ].ycoord > curr_y && points[ next ].ycoord > curr_y ) {
            // found a minima, add to min list, sort by y value
            if( ( NumMinima + 1 ) * sizeof( short ) > StackSize ) {
                _ErrorStatus = _GRINSUFFICIENTMEMORY;   // need room for 1 more
                NumMinima = 0;  // signal error
                return;
            }
            for( min = 0; min < NumMinima; ++min ) {
                if( curr_y < points[ MinList[ min ] ].ycoord ) {
                    // shift rest of list up
                    for( j = NumMinima; j > min; --j ) {
                        MinList[ j ] = MinList[ j - 1 ];
                    }
                    break;
                }
            }
            MinList[ min ] = i;
            ++NumMinima;
        }
        i = next;
    } while( next != start );
    if( NumMinima == 0 ) {
        _ErrorStatus = _GRINVALIDPARAMETER;     // no minima found
    }
}


static void OrderLines( void )
//======================

//  Order the line segments by x value.

{
    struct seg_entry    *curr;
    struct seg_entry    *next;
    struct seg_entry    *prev;
    char                swap;
    struct seg_entry    temp;   // dummy entry for start of list

    temp.link = LineList;   // place start of list in dummy so that
    do {                    // we can do swaps more easily
        swap = 0;
        prev = &temp;
        for( ;; ) {
            curr = prev->link;
            if( curr == NULL ) {
                break;
            }
            next = curr->link;
            if( next == NULL ) {
                break;
            }
            if( curr->line.curr_x > next->line.curr_x ) {
                prev->link = next;
                curr->link = next->link;
                next->link = curr;
                swap = 1;
            }
            prev = curr;
        }
    } while( swap );
    LineList = temp.link;       // reload start of list
}


static short AddLine( short p1, short p2, struct xycoord _WCI86FAR *points )
//=====================================================================

{
    struct seg_entry    *segment;

    segment = FreeList;         // get element from free list
    if( segment == NULL ) {
        _ErrorStatus = _GRINSUFFICIENTMEMORY;
        return( FALSE );
    }
    FreeList = FreeList->link;
    _LineInit( points[ p1 ].xcoord, points[ p1 ].ycoord,
               points[ p2 ].xcoord, points[ p2 ].ycoord, &segment->line );
    segment->end_pt = p2;
    segment->delete = FALSE;
    segment->link = LineList;
    LineList = segment;
    return( TRUE );
}


static short AddMinima( short y, short n, struct xycoord _WCI86FAR *points )
//=====================================================================

{
    short               p;
    short               prev;
    short               next;
    short               p2;

    p = MinList[ 0 ];
    for( ;; ) {
        prev = NextPoint( p, -1, n, points );
        p2 = prev + 1;
        if( p2 == n ) {
            p2 = 0;
        }
        if( !AddLine( p2, prev, points ) ) {
            return( FALSE );
        }
        next = NextPoint( p, 1, n, points );
        p2 = next - 1;
        if( p2 < 0 ) {
            p2 = n - 1;
        }
        if( !AddLine( p2, next, points ) ) {
            return( FALSE );
        }
        --NumMinima;
        if( NumMinima == 0 ) {
            break;
        }
        ++MinList;              // advance list
        p = MinList[ 0 ];
        if( points[ p ].ycoord != y ) {     // check for more on this line
            break;
        }
    }
    OrderLines();
    return( TRUE );
}


static void ExtendLines( short y, short n, struct xycoord _WCI86FAR *points )
//======================================================================

//  Replace line segments ending at the current y value with their
//  upward extension.

{
    struct seg_entry    *line;
    short               p;
    short               p1;
    short               p2;
    short               end_x;
    short               old_left;
    short               old_right;

    for( line = LineList; line != NULL; line = line->link ) {
        p = line->end_pt;
        if( points[ p ].ycoord == y ) {
            // current line ends at this point, so make sure the
            // line doesn't extend past the ending x value
            end_x = points[ p ].xcoord;
            if( line->line.sdx > 0 ) {
                line->line.right_x = end_x;
            } else {
                line->line.left_x = end_x;
            }
            p2 = NextPoint( p, -1, n, points );
            if( points[ p2 ].ycoord > y ) {
                p1 = p2 + 1;
                if( p1 == n ) {
                    p1 = 0;
                }
            } else {
                p2 = NextPoint( p, 1, n, points );
                if( points[ p2 ].ycoord > y ) {
                    p1 = p2 - 1;
                    if( p1 < 0 ) {
                        p1 = n - 1;
                    }
                } else {    // no upward extension
                    line->delete = TRUE;    // mark line to be deleted
                    continue;
                }
            }
            // Replace the current line segment with the line segment
            // from p1 to p2. Preserve the current left and right extensions.
            old_left = line->line.left_x;
            old_right = line->line.right_x;
            _LineInit( points[ p1 ].xcoord, points[ p1 ].ycoord,
                       points[ p2 ].xcoord, points[ p2 ].ycoord, &line->line );
            line->end_pt = p2;
            if( old_left < line->line.left_x ) {
                line->line.left_x = old_left;
            }
            if( old_right > line->line.right_x ) {
                line->line.right_x = old_right;
            }
        }
    }
}


static void PlotBetween( short y )
//================================

{
    struct seg_entry    *curr;
    struct seg_entry    *next;
    short               left_x;
    short               right_x;

    for( curr = LineList; curr != NULL; ) {
        next = curr->link;
        left_x = min( curr->line.left_x, next->line.left_x );
        right_x = max( curr->line.right_x, next->line.right_x );
        _L1ClipFill( left_x, right_x, y );
        curr = next->link;
    }
}


static void UpdateLines( void )
//=======================

//  Delete lines with no upward extension. These lines were previously
//  flagged by ExtendLines().

{
    struct seg_entry    *curr;
    struct seg_entry    *next;
    struct seg_entry    *prev;
    short               curr_x;
    short               re_sort;

    curr_x = 0;
    re_sort = FALSE;
    for( curr = LineList; curr != NULL; ) {
        next = curr->link;
        if( curr->delete ) {
            if( curr == LineList ) {
                LineList = next;
            } else {
                prev->link = next;
            }
            curr->link = FreeList;      // free element by adding to free list
            FreeList = curr;
        } else {
            _LineMove( &curr->line );
            if( curr->line.curr_x < curr_x ) {
                re_sort = TRUE;     // need to re-sort the line segments
            }
            curr_x = curr->line.curr_x;
            prev = curr;
        }
        curr = next;
    }
    if( re_sort ) {
        OrderLines();
    }
}


static short InitLineList( void )
//=========================

{
    short               max_lines;
    short               i;

    Stack += NumMinima * sizeof( short );
    StackSize -= NumMinima * sizeof( short );
    max_lines = StackSize / sizeof( struct seg_entry );
    if( max_lines < 2 ) {   // need at least 2, since we have 2 for each min
        _ErrorStatus = _GRINSUFFICIENTMEMORY;
        return( FALSE );
    }
    LineList = NULL;
    FreeList = (struct seg_entry *) Stack;  // initialize free list
    for( i = 0; i < max_lines - 1; ++i ) {
        FreeList[ i ].link = &FreeList[ i + 1 ];
    }
    FreeList[ max_lines - 1 ].link = NULL;
    return( TRUE );
}

#elif defined( __OS2__ )
    #include <limits.h>
#endif


short _L1FillArea( short n, struct xycoord _WCI86FAR *points )
//=======================================================

{
#if defined( _DEFAULT_WINDOWS )
    WPI_PRES            dc;
    HBITMAP             bm;
    HBRUSH              brush;
    HBRUSH              old_brush;
    HPEN                pen;
    HPEN                old_pen;
    HRGN                temprgn;
    WPI_COLOUR          color;
    WPI_RECT            clip_rect;
    short               clipy1, clipy2;
  #if defined( __WINDOWS__ )
    HRGN                Refresh;
  #else
    WPI_POINT*          stack;
    short               i;
    WPI_RECTDIM         minx, miny;
    WPI_RECTDIM         maxx, maxy;
  #endif
#else
    short               y;
    short               next_min;
#endif

    short               success;

#if defined( _DEFAULT_WINDOWS )
    dc = _Mem_dc;

// Do the clipping
    temprgn = _ClipRgn;
    clipy1 = _wpi_cvth_y( _CurrState->clip_def.ymin, _GetPresHeight() );
    clipy2 = _wpi_cvth_y( _CurrState->clip_def.ymax + 1, _GetPresHeight() );
    _wpi_setintwrectvalues( &clip_rect,
                           _CurrState->clip_def.xmin, clipy1,
                           _CurrState->clip_def.xmax + 1, clipy2 );
    _ClipRgn = _wpi_createrectrgn( dc, &clip_rect );
    _wpi_getclipbox( dc, &clip_rect);
    _wpi_selectcliprgn( dc, _ClipRgn );
    _wpi_deletecliprgn( dc, temprgn );

// setup before drawing
    color = _Col2RGB( _CurrColor );
    pen = _wpi_createpen( PS_NULL, 0, color );

    if( _HaveMask == 0 ) {
        brush = _wpi_createsolidbrush( color );
    } else {
        // if a mask is defined, convert it to bitmap
        bm = _Mask2Bitmap( dc, &_FillMask );
        brush = _wpi_createpatternbrush( bm );
    }

    old_pen = _wpi_selectpen( dc, pen );
    old_brush = _wpi_selectbrush( dc, brush );
  #if defined( __OS2__ )
    minx = miny = LONG_MAX;
    maxx = maxy = LONG_MIN;
    stack = _MyAlloc( n * sizeof( WPI_POINT ) );
    for( i = 0; i < n; i++ ) {
        stack[i].x = points[i].xcoord;
        stack[i].y = _wpi_cvth_y( points[i].ycoord, _GetPresHeight() );
        if( minx > points[i].xcoord ) {
            minx = points[i].xcoord;
        }
        if( miny > points[i].ycoord ) {
            miny = points[i].ycoord;
        }
        if( maxx < points[i].xcoord ) {
            maxx = points[i].xcoord;
        }
        if( maxy < points[i].ycoord ) {
            maxy = points[i].ycoord;
        }
    }
    _wpi_polygon( dc, stack, n );
    _MyFree( stack );
  #else
    _wpi_polygon( dc, (LPPOINT)points, n );
  #endif

// Clean up afterwards
    _wpi_getoldbrush( dc, old_brush );
    _wpi_deletebrush( brush );

    if( _HaveMask != 0 ) {
        _wpi_deletebitmap( bm );
    }

    _wpi_getoldpen( dc, old_pen );
    _wpi_deletepen( pen );

    temprgn = _ClipRgn;
    _ClipRgn = _wpi_createrectrgn( dc, &clip_rect );
    _wpi_selectcliprgn( dc, _ClipRgn );
    _wpi_deletecliprgn( dc, temprgn );

// Update the window
  #if defined( __WINDOWS__ )
    Refresh = CreatePolygonRgn( (LPPOINT)points, n, ALTERNATE );
    OffsetRgn( Refresh, -_BitBlt_Coord.xcoord, -_BitBlt_Coord.ycoord );
    InvalidateRgn( _CurrWin, Refresh, 0 );
  #else
    _MyInvalidate( minx, miny, maxx, maxy );
    _wpi_invalidaterect( _CurrWin, NULL, 0 );
  #endif
    _RefreshWindow();
  #if defined( __WINDOWS__ )
    DeleteObject( Refresh );
  #endif
    success = TRUE;

#else
    StackSize = _RoundUp( _stackavail() - 0x100 );  // obtain memory from stack
    if( StackSize > 0 ) {
        Stack = __alloca( StackSize );
    } else {
        _ErrorStatus = _GRINSUFFICIENTMEMORY;
        return( FALSE );
    }
    MinList = (short *)Stack;
    CalcMinima( n, points );
    if( NumMinima == 0 ) {
        return( FALSE );
    }
    if( !InitLineList() ) {
        return( FALSE );
    }
    y = points[ MinList[ 0 ] ].ycoord;
    next_min = y;
    _StartDevice();
    for( ; ; ++y ) {
        if( y == next_min ) {
            success = AddMinima( y, n, points );
            if( !success ) {
                break;
            }
            if( NumMinima == 0 ) {
                next_min = y - 1;
            } else {
                next_min = points[ MinList[ 0 ] ].ycoord;
            }
        }
        if( LineList == NULL ) {
            break;
        }
        ExtendLines( y, n, points );
        PlotBetween( y );
        UpdateLines();
    }
    _ResetDevice();
#endif
    return( success );
}
