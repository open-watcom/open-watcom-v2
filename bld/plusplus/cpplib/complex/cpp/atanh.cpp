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

_WPRTLINK Complex atanh( const Complex &z ) {
/******************************************/
// Hyperbolic arctangent of a complex number.
// From "Complex Variables and Applications" (pg. 63).
//    atanh( z ) = log( (1 + z)/(1 - z) )/2
//               = log( (1 + z.r + z.i*i)/(1 - z.r - z.i*i) )/2
//
// let a = z.r, b = z.i
//                      (1 + a + b*i) / ( 1 - a - b*i )
//
// or:  1+a +b*i     1-a +b*i     1 - a^2 - b^2 + 2*b*i
//     ---------- * ---------- = -----------------------
//      1-a -b*i     1-a +b*i      1 - 2*a + a^2 + b^2
//
// Note: a*a + b*b == (a+b)*(a+b) - 2ab
//
//                                1 - (a+b)^2 + 2*a*b + 2*b*i
//                             = -----------------------------
//                                 1 - 2*a + (a+b)^2 - 2*a*b
//

    dcomplex    value;
    double      apb2 = (z.real()+z.imag())*(z.real()+z.imag());
    double      _2ab = 2.0*z.real()*z.imag();

    value = _IF_C16Div( 1.0 - apb2 + _2ab, 2.0*z.imag(),
                    1.0 - 2.0*z.real() + apb2 - _2ab, 0.0 );
    value = _IF_CDLOG( value.realpart, value.imagpart );
    return Complex( value.realpart / 2, value.imagpart / 2 );
}
