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


//
// DIVC16          : complex division runtime routine
//

#include "fmath.h"

#include "ftnstd.h"
#include "rtenv.h"


dcomplex C16Div( double a, double b, double c, double d ) {
//=========================================================

// Divide two double precision complex numbers and return the
// imaginary part of the result.
// ( a, b )/( c, d ) = (( a, b )/( d + c*c/d )) * ( c/d, -1 )

    double      quo;
    double      tmp;
    dcomplex    res;

    if( fabs( d ) < fabs( c ) ) {
        tmp = a;        // convert to (-b,a)/(-d,c)
        a = - b;
        b = tmp;
        tmp = c;
        c = -d;
        d = tmp;
    }
    quo = c / d;
    tmp = quo * c + d;
    a /= tmp;
    b /= tmp;
    c = quo;
    res.realpart = a * c + b;
    res.imagpart = b * c - a;
    return( res );
}
