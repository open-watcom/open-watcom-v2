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
#include <complex>
#include "cplx.h"

_WPRTLINK Complex tan( const Complex &z ) {
/****************************************/
// Tangent of a complex number.
// From "Complex Variables and Applications" (pg. 51).
//    tan( z ) = sin( z ) / cos( z )
//             = ((exp(iz) - exp(-iz))/2i)/((exp(iz) + exp(-iz))/2)
//             = (exp(iz) - exp(-iz))/(i*(exp(iz) + exp(-iz)))
#if 0
    dcomplex result;
    dcomplex exp_iz;
    dcomplex exp_neg_iz;

    exp_iz     = CDEXP( z.imag(), -z.real() );
    exp_neg_iz = C16Div( 1.0, 0.0, exp_iz.realpart, exp_iz.imagpart );
    result     = C16Div( exp_iz.realpart - exp_neg_iz.realpart,
                         exp_iz.imagpart - exp_neg_iz.imagpart,
                         exp_iz.imagpart + exp_neg_iz.imagpart,
                       -(exp_iz.realpart + exp_neg_iz.realpart) );
    Complex c_result( result.realpart, result.imagpart );
#elif 0
    double   sin_r;
    double   cos_r;
    double   exp_i;
    double   sin_r_by_exp_i;
    double   cos_r_by_exp_i;
    double   sin_r_times_exp_i;
    double   cos_r_times_exp_i;
    dcomplex result;

    sin_r = sin( z.real() );
    cos_r = cos( z.real() );
    exp_i = exp( z.imag() );
    sin_r_by_exp_i    = sin_r/exp_i;
    cos_r_by_exp_i    = cos_r/exp_i;
    sin_r_times_exp_i = sin_r*exp_i;
    cos_r_times_exp_i = cos_r*exp_i;
    result = C16Div( cos_r_by_exp_i - cos_r_times_exp_i,
                     sin_r_by_exp_i + sin_r_times_exp_i,
                    -sin_r_by_exp_i + sin_r_times_exp_i,
                     cos_r_by_exp_i + cos_r_times_exp_i );
    Complex c_result( result.realpart, result.imagpart );
#endif

// From Maple:
//    tan( a+ib ):
//        real part = sin(2a)  / (cos(2a) + cosh(2b))
//        imag part = sinh(2a) / (cos(2a) + cosh(2b))

    double  two_r;
    double  two_i;
    double  denom;


    two_r = z.real() * 2.0;
    two_i = z.imag() * 2.0;
    denom = cos( two_r ) + cosh( two_i );
    return Complex( sin( two_r ) / denom, sinh( two_i ) / denom );
}
