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


_WCRTLINK grcolor _WCI86FAR _CGRAPH _getpixel_w( double x, double y )
/*==================================================

   This function queries the color of the pixel at ( x, y ), in
   window coordinates.  */

{
    grcolor         colour;

    if( _GrProlog() ) {
        colour = _L1GetDot( _WtoPhysX( x ), _WtoPhysY( y ) );
        _GrEpilog();
    } else {
        colour = -1;
    }
    return( colour );
}

Entry( _GETPIXEL_W, _getpixel_w ) // alternate entry-point


_WCRTLINK grcolor _WCI86FAR _CGRAPH _getpixel_w_87( double x, double y )
/*===================================================*/

{
    return( _getpixel_w( x, y ) );
}

Entry( _GETPIXEL_W_87, _getpixel_w_87 ) // alternate entry-point


_WCRTLINK grcolor _WCI86FAR _CGRAPH _setpixel_w( double x, double y )
/*==================================================

   This routine sets the pixel at the point ( x, y ), in window coords. */

{
    grcolor             old_colour;         /* previous colour at (x,y) */

    if( _GrProlog() ) {
        old_colour = _L1PutDot( _WtoPhysX( x ), _WtoPhysY( y ) );
        _GrEpilog();
    } else {
        old_colour = -1;
    }
    return( old_colour );
}

Entry( _SETPIXEL_W, _setpixel_w ) // alternate entry-point


_WCRTLINK grcolor _WCI86FAR _CGRAPH _setpixel_w_87( double x, double y )
/*===================================================*/

{
    return( _setpixel_w( x, y ) );
}

Entry( _SETPIXEL_W_87, _setpixel_w_87 ) // alternate entry-point
