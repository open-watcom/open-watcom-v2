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


short _WCI86FAR _CGRAPH _floodfill_w( double x, double y, short stop_color )
/*=====================================================================

   This function fills an area starting at ( x, y ), in window coordinates,
   and continuing until the stop_color is met. */

{
    short               success;        /* successful fill flag */

    if( _GrProlog() ) {
        success = _L2floodfill( _WtoPhysX( x ), _WtoPhysY( y ), stop_color );
        _GrEpilog();
    } else {
        success = 0;
    }
    return( success );
}

Entry( _FLOODFILL_W, _floodfill_w ) // alternate entry-point


short _WCI86FAR _CGRAPH _floodfill_w_87( double x, double y, short stop_color )
/*======================================================================*/

{
    return( _floodfill_w( x, y, stop_color ) );
}

Entry( _FLOODFILL_W_87, _floodfill_w_87 ) // alternate entry-point
