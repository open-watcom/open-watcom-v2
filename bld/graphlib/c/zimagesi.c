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


long _WCI86FAR _CGRAPH _imagesize_w( double x1, double y1, double x2, double y2 )
/*==========================================================================

   This routine returns the size of buffer needed to store the picture
   in the rectangle defined by ( x1, y1 ) and ( x2, y2 ), in window
   coordinates. */

{
    if( _GraphMode() ) {
        return( _L2imagesize( _WtoPhysX( x1 ), _WtoPhysY( y1 ),
                              _WtoPhysX( x2 ), _WtoPhysY( y2 ) ) );
    } else {
        return( 0 );
    }
}

Entry( _IMAGESIZE_W, _imagesize_w ) // alternate entry-point


long _WCI86FAR _CGRAPH _imagesize_wxy( struct _wxycoord _WCI86FAR * p1,
/*=============================*/ struct _wxycoord _WCI86FAR * p2 )

/* This routine returns the size of buffer needed to store the picture
   in the rectangle defined by "p1" and "p2", in window coordinates.    */

{
    return( _imagesize_w( p1->wx, p1->wy, p2->wx, p2->wy ) );
}

Entry( _IMAGESIZE_WXY, _imagesize_wxy ) // alternate entry-point


long _WCI86FAR _CGRAPH _imagesize_w_87( double x1, double y1, double x2, double y2 )
/*===========================================================================*/

{
    return( _imagesize_w( x1, y1, x2, y2 ) );
}

Entry( _IMAGESIZE_W_87, _imagesize_w_87 ) // alternate entry-point
