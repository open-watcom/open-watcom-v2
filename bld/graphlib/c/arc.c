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


_WCRTLINK short _WCI86FAR _CGRAPH _arc( short x1, short y1, short x2, short y2,
/*====================*/ short x3, short y3, short x4, short y4 )

/* This function draws an elliptical arc.  The virtual ellipse is defined by
   the rectangle whose opposite corners are ( x1, y1 ) and ( x2, y2 ).  The
   intersections of the vectors from the center of the ellipse to the points
   ( x3, y3 ) and ( x4, y4 ) define the start and end points respectively
   where the arc is drawn in a counter clockwise direction. */

{
    short           success;

    if( _GrProlog() ) {
        success = _L2arc( _VtoPhysX( x1 ), _VtoPhysY( y1 ),
                          _VtoPhysX( x2 ), _VtoPhysY( y2 ),
                          _VtoPhysX( x3 ), _VtoPhysY( y3 ),
                          _VtoPhysX( x4 ), _VtoPhysY( y4 ) );
        if( success ) {
            _moveto( _GetLogX( _ArcInfo.end.xcoord ),
                     _GetLogY( _ArcInfo.end.ycoord ) );
        }
        _GrEpilog();
    } else {
        success = 0;
    }
    return( success );
}

Entry( _ARC, _arc ) // alternate entry-point


#define sgn( x )    ( x == 0 ? 0 : ( x < 0 ? -1 : 1 ) )


_WCRTLINK short _WCI86FAR _CGRAPH _getarcinfo( struct xycoord _WCI86FAR *start,
/*===========================*/ struct xycoord _WCI86FAR *end,
                                struct xycoord _WCI86FAR *fillpoint )

/* This function returns the starting and ending points of the last arc
   or pie drawn. The points are in viewport coordinates. The fillpoint
   is an inside point that can be used by floodfill to fill a pie slice.
   This is not used in WATCOM C Graphics which uses a specialized fill
   routine for pie slices.  */

{
    short               success;
    short               x1, y1;
    short               x2, y2;
    short               x3, y3;
    long                xprod;

    success = ( _ErrorStatus == _GROK );
    if( success ) {
        start->xcoord = _GetLogX( _ArcInfo.start.xcoord );
        start->ycoord = _GetLogY( _ArcInfo.start.ycoord );
        end->xcoord = _GetLogX( _ArcInfo.end.xcoord );
        end->ycoord = _GetLogY( _ArcInfo.end.ycoord );

        x1 = _ArcInfo.start.xcoord - _ArcInfo.centre.xcoord;
        y1 = _ArcInfo.start.ycoord - _ArcInfo.centre.ycoord;
        x2 = _ArcInfo.end.xcoord - _ArcInfo.centre.xcoord;
        y2 = _ArcInfo.end.ycoord - _ArcInfo.centre.ycoord;
        x3 = 3 * ( x1 + x2 ) / 8;
        y3 = 3 * ( y1 + y2 ) / 8;
        xprod = (long) x1 * y2 - (long) x2 * y1;
        if( xprod == 0 ) {
            if( x1 != x2 || y1 != y2 ) {    // 180 degrees apart
                x3 = sgn( y1 );     // offset from centre
                y3 = -sgn( x1 );
            }
        } else if( xprod > 0 ) {    // more than 180
            x3 = -x3;
            y3 = -y3;
        }
        fillpoint->xcoord = _GetLogX( x3 + _ArcInfo.centre.xcoord );
        fillpoint->ycoord = _GetLogY( y3 + _ArcInfo.centre.ycoord );
    }
    return( success );
}

Entry( _GETARCINFO, _getarcinfo ) // alternate entry-point


short _WCI86FAR _L2arc( short x1, short y1, short x2, short y2,
/*==============*/ short x3, short y3, short x4, short y4 )

/* This function draws an elliptical arc.  The virtual ellipse is defined by
   the rectangle whose opposite corners are ( x1, y1 ) and ( x2, y2 ). The
   intersections of the vectors from the center of the ellipse to the points
   ( x3, y3 ) and ( x4, y4 ) define the start and end points respectively
   where the arc is drawn in a counter clockwise direction. All values
   are in physical coordinates. */

{
    _L1Arc( _GBORDER, x1, y1, x2, y2, x3, y3, x4, y4 );
    return( _ErrorStatus == _GROK );
}
