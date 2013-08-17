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


_WCRTLINK short _WCI86FAR _CGRAPH _polygon( short fill, short numpts,
/*========================*/ struct xycoord _WCI86FAR *points )

/* This routine draws a polygon specified by the array points[].  It also
   fills the regions of this polygon based on the fill parameter. */

{
    short               i;
    short               success;

    if( _GrProlog() ) {
        for( i = 0; i < numpts; ++i ) {                /* translate points */
            points[i].xcoord = _VtoPhysX( points[i].xcoord );
            points[i].ycoord = _VtoPhysY( points[i].ycoord );
        }
        success = _L2polygon( fill, numpts, points );
        for( i = 0; i < numpts; ++i ) {                /* translate back */
            points[i].xcoord = _GetLogX( points[i].xcoord );
            points[i].ycoord = _GetLogY( points[i].ycoord );
        }
        _GrEpilog();
    }
    return( success );
}

Entry1( _POLYGON, _polygon ) // alternate entry-point


short _WCI86FAR _L2polygon( short fill, short numpts,
/*==================*/ struct xycoord _WCI86FAR *points )

/* This routine draws or fills a polygon specified by the array points[].
   The points are in physical coordinates.  */

{
    short i;
    short x1, y1, x2, y2;
    int   count = 0;

    if( numpts < 3 ) {
        _ErrorStatus = _GRINVALIDPARAMETER;
        return( 0 );
    }
    if( fill == _GFILLINTERIOR ) {
        return( _L1FillArea( numpts, points ) );
    } else {
        x1 = points[numpts-1].xcoord;
        y1 = points[numpts-1].ycoord;
        for( i = 0; i < numpts; i++ ) {
            x2 = points[i].xcoord;
            y2 = points[i].ycoord;

            if( y1 < y2 ) {
                count += _L1Line( x1, y1, x2, y2 );
            } else {
                count += _L1Line( x2, y2, x1, y1 );
            }

            x1 = x2;
            y1 = y2;
        }
        _RefreshWindow();

        /*
         * If ALL lines were completely clipped then set _GRNOOUTPUT.
         */
        if ((_ErrorStatus == _GRCLIPPED) && (count == 0)) {
            _ErrorStatus = _GRNOOUTPUT;
        }

        return( 1 );
    }
}
