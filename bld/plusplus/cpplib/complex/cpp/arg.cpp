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


#include "variety.h"
#include <complex.h>
#include <math.h>

#define PI     3.141592653589793

_WPRTLINK double arg( const Complex &z ) {
/***************************************/
// The angle of the complex vector z.
// In the 1st and 4th quadrants:
//      angle( z ) = asin( z.i/sqrt(z.r*z.r + z.i*z.i) )
// In the 2nd quadrant:
//      angle( z ) = pi - (above (positive value))
// In the 3rd quadrant:
//      angle( z ) = -pi - (above (negative value))
// (Derived from basic principles.)
// The angle of the (0, 0) is 0.
    double mag_squared;
    double angle;

    mag_squared = z.real()*z.real() + z.imag()*z.imag();
    if( mag_squared == 0.0 ) {
        return( 0.0 );
    }
    angle = asin( z.imag() / sqrt( mag_squared ) );
    if( z.real() < 0.0 ) {                    // 2nd or 3rd quadrant?
        angle = -angle;
        if( z.imag() >= 0 ) {                   //   2nd!
            angle += PI;
        } else {                           //   3rd!
            angle -= PI;
        }
    }
    return( angle );
}
