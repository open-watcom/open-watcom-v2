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

_WPRTLINK Complex asin( const Complex &z ) {
/*****************************************/
// Arcsin of a complex number.
// From "Complex Variables and Applications" (pg. 62).
//    asin( z ) = -i log( iz + sqrt( 1 - z*z ) )
//              = -i log( iz + sqrt( (1 - z) * (1 + z) ) )
//              = -i log( z + sqrt( 1 + (z.i - z.r)*(z.i + z.r) - 2*z.r*z.i*i ) )
    dcomplex    value;

    value = _IF_CDSQRT( 1.0 + (z.imag()-z.real())*(z.imag()+z.real()),
                    -2.0*z.real()*z.imag() );
    value = _IF_CDLOG( value.realpart - z.imag(), value.imagpart + z.real() );
    return Complex( value.imagpart, -value.realpart );
}
