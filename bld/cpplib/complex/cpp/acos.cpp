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
#include "cplx.h"

_WPRTLINK Complex acos( const Complex &z ) {
/*****************************************/
// Arccos of a complex number.
// From "Complex Variables and Applications" (pg. 62).
//    acos( z ) = -i log( z + sqrt( z*z - 1 ) )
//              = -i log( z + sqrt( (z + 1) * (z - 1) ) )
//  I'm not sure why, but both Borland C++ and Maple seem to give
//  answers only in the 1st and 4th quadrants, so any answer in the
//  2nd or 3rd quadrant is changed to the 4th or 1st quadrant by
//  negating both parts of the complex number, since
//    cos x == cos -x
    dcomplex value;

    value = _IF_C16Mul( z.real() + 1.0, z.imag(), z.real() - 1.0, z.imag() );
    value = _IF_CDSQRT( value.realpart, value.imagpart );
    value = _IF_CDLOG( value.realpart + z.real(), value.imagpart + z.imag() );
    Complex c_result( value.imagpart, -value.realpart );

// Any negative real part means 2nd or 3rd quadrant.
// If the real part is zero and the imaginary part is negative, the
// vector is between the 3rd and 4th quadrants (angle -pi/2), so convert
// to the 1st quadrant (angle pi/2)

    if( c_result.real() < 0.0
            ||  (c_result.real() == 0.0  &&  c_result.imag() < 0.0) ) {
        c_result = -c_result;
    }
    return( c_result );
}
