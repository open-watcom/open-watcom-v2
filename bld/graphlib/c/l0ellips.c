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
* Description:  Draw an ellipse.
*
****************************************************************************/


#include "gdefn.h"


void _L0Ellipse( short x1, short y1, short x2, short y2,
                  void (*plot)( short, short, short ) )
//=======================================================================

// This function calculates the points on the ellipse with opposite
// corners (x1,y1) and (x2,y2) and calls the given plot function for
// each point. Refer to "Programmer's Guide to PC and PS/2 Video
// Systems" by R. Wilton for the inspiration for this routine.

{
    short               a, b, t;
    short               x_axis_width, y_axis_width;
    short               x_end, y_end;
    short               x, y;                   // current point
    long                as, bs, as2, bs2;       // a^2, b^2, 2a^2, 2b^2
    long                d, dx, dy;              // control values

    if( x1 > x2 ) {         // ensure x1 < x2
        t = x1;
        x1 = x2;
        x2 = t;
    }
    if( y1 > y2 ) {         // ensure y1 < y2
        t = y1;
        y1 = y2;
        y2 = t;
    }

    a = ( x2 - x1 + 1 ) / 2 - 1;
    b = ( y2 - y1 + 1 ) / 2 - 1;
    if( a < 0 || b < 0 ) {
        _ErrorStatus = _GRINVALIDPARAMETER;
        return;             // invalid ellipse
    }
    x_axis_width = y2 - y1 - 2*b - 1;   // either 0 or 1
    y_axis_width = x2 - x1 - 2*a - 1;
    as = (long) a * a;
    bs = (long) b * b;
    as2 = as + as;
    bs2 = bs + bs;

    // 1st quadrant : move from (a,0) to (0,b)

    if( x_axis_width != 0 ) {   // this point is in quad 0 or 2
        ( *plot )( x2, y1 + b + 1, 0 );
        ( *plot )( x2, y2 - b - 1, 2 );     // really (x1,y1+b+1)
    }

    x = x2;
    y = y1 + b;
    x_end = x2 - a;
    y_end = y1;

    dx = 0;
    dy = a * bs2;
    d = as + bs / 4 - a * bs;
    if( as + bs <= a * bs2 && b > 1 ) {     // if its not a thin ellipse
        do {
            ( *plot )( x, y, 4 );
            if( d > 0L ) {
                dy -= bs2;
                d -= dy;
                --x;
            }
            dx += as2;
            d += dx + as;
            --y;
        } while( dy > dx );
    }
    d += 3 * ( bs - as ) / 4 - ( dx + dy ) / 2;
    for( ;; ) {
        ( *plot )( x, y, 4 );
        if( x == x_end ) {          // last x-value
            if( y != y_end ) {      // plot rest of y-values
                do {
                    --y;
                    ( *plot )( x, y, 4 );
                } while ( y != y_end );
            }
            break;
        }
        if( d <= 0L ) {
            dx += as2;
            d += dx;
            --y;
        }
        dy -= bs2;
        d -= dy - bs;
        --x;
    }
    if( y_axis_width != 0 ) {   // this point is in quad 0 or 3
        ( *plot )( x2 - a - 1, y1, 1 );     // really (x1+a+1,y1)
        ( *plot )( x1 + a + 1, y1, 3 );     // really (x1+a+1,y2)
    }
}
