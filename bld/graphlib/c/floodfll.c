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


_WCRTLINK short _WCI86FAR _CGRAPH _floodfill( short x, short y, short stop_color )
/*=================================================================

   This function fills an area starting at ( x, y ), in viewport coordinates,
   and continuing until the stop_color is met.  */

{
    short               success;        /* successful fill flag */

    if( _GrProlog() ) {
        success = _L2floodfill( _VtoPhysX( x ), _VtoPhysY( y ), stop_color );
        _GrEpilog();
    } else {
        success = 0;
    }
    return( success );
}

Entry( _FLOODFILL, _floodfill ) // alternate entry-point


short _WCI86FAR _L2floodfill( short x, short y, short stop_color )
/*===========================================================

   This function fills an area starting at ( x, y ) and continuing until
   the stop_color is met. All values are in physical coordinates.   */

{
    return( _L1Paint( stop_color, x, y ) );
}
