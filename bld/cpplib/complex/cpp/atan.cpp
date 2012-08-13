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

_WPRTLINK Complex atan( const Complex &z ) {
/*****************************************/
// Arctan of a complex number.
// From "Complex Variables and Applications" (pg. 63).
//    atan( z ) = i/2 * log( (i + z)/(i - z) )
//              = i/2 * log( (i + (z.r + z.i*i))/(i - (z.r + z.i*i)) )
//              = i/2 * log( (z.r + (1+z.i)*i)/(-z.r + (1-z.i)*i) )
//
// let a = z.r, b = z.i
//                           (a + (1 + b)*i)/(-a + (1 - b)*i)
//
// or:   a+(1+b)*i     -a-(1-b)*i     - a^2 + 1 - b^2 - (1 - b + 1 + b )*a*i
//     ------------ * ------------ = ----------------------------------------
//      -a+(1-b)*i     -a-(1-b)*i              a^2 + b^2 - 2*b + 1
//
//                                    1 - a^2 - b^2 - 2*a*i
//                                 = -----------------------
//                                     a^2 + b^2 - 2*b + 1
//
// Note: a^2 + b^2 == (a+b)^2 - 2ab
//                                    1 - (a+b)^2 + 2*a*b - 2*a*i
//                                 = -----------------------------
//                                     (a+b)^2 - 2*a*b - 2*b + 1
//
    dcomplex    value;
    double      apb2 = (z.real()+z.imag())*(z.real()+z.imag());
    double      _2ab = 2.0*z.real()*z.imag();

    value = _IF_C16Div( 1.0 - apb2 + _2ab, -2.0*z.real(),
                    apb2 - _2ab - 2.0*z.imag() + 1.0, 0.0 );
    value = _IF_CDLOG( value.realpart, value.imagpart );
    return Complex( -value.imagpart / 2, value.realpart / 2 );
}

