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


_WCRTLINK short _WCI86FAR _CGRAPH _rectangle( short fill, short x1, short y1,
/*==========================*/ short x2, short y2 )

/* This routine draws of fills a rectangle whose opposite corners are
   defined by ( x1, y1 ) and ( x2, y2 ), in viewport coordinates.   */

{
    short           success;

    if( _GrProlog() ) {
        success = _L2rectangle( fill, _VtoPhysX( x1 ), _VtoPhysY( y1 ),
                                      _VtoPhysX( x2 ), _VtoPhysY( y2 ) );
        _GrEpilog();
    } else {
        success = 0;
    }
    return( success );
}

Entry( _RECTANGLE, _rectangle ) // alternate entry-point


short _WCI86FAR _L2rectangle( short fill, short x1, short y1,
/*====================*/ short x2, short y2 )

/* This routine draws or fills a rectangle whose opposite corners are
   defined by ( x1, y1 ) and ( x2, y2 ), in physical coordinates.   */

{
    int count;

    if( fill == _GBORDER ) {
        if (_PlotAct == 1) {
            count  = _L2line( x1, y1, x1, y2 );
            x1 = (x1 < x2) ? (x1 + 1) : (x1 - 1);
            count += _L2line( x1, y2, x2, y2 );
            y2 = (y2 < y1) ? (y2 + 1) : (y2 - 1);
            count += _L2line( x2, y2, x2, y1 );
            x2 = (x2 < x1) ? (x2 + 1) : (x2 - 1);
            count += _L2line( x2, y1, x1, y1 );
        } else {
            count  = _L2line( x1, y1, x1, y2 );
            count += _L2line( x1, y2, x2, y2 );
            count += _L2line( x2, y2, x2, y1 );
            count += _L2line( x2, y1, x1, y1 );
        }
        _RefreshWindow();
        /*
         * If all 4 line segments returned 0 then there was no output at all.
         */
        if ((_ErrorStatus == _GRCLIPPED) && (count == 0)) {
            _ErrorStatus = _GRNOOUTPUT;
        }
    } else {
//      _PaRf_x = x1;               /* should make sure first that  */
//      _PaRf_y = y1;               /* x1 < x2 and y1 < y2          */
        if( _L0BlockClip( &x1, &y1, &x2, &y2 ) == 0 ) {
            _L1Block( x1, y1, x2, y2 );
        } else {
            _ErrorStatus = _GRNOOUTPUT;
        }
    }
    return( TRUE );
}
