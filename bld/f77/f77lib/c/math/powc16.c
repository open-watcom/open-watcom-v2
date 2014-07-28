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
// POWC16          : complex power run-time routine
//

#include "fmath.h"

#include "ftnstd.h"
#include "rtenv.h"
#include "errcod.h"
#include "ifenv.h"
#include "fmthcode.h"

extern  double          DSIN(double);
extern  double          DCOS(double);
extern  double          DLOG(double);
extern  double          DEXP(double);
extern  double          DATAN2(double,double);


dcomplex C16Pow( double a, double b, double c, double d ) {
//=========================================================

// Return the real part of the result of taking one double precision
// complex number to the power of another.
// ( a, b ) ** ( c, d ) = exp( ( c,d ) * log( a,b ) )

    double      u;
    double      v;
    dcomplex    arg1;
    dcomplex    arg2;
    dcomplex    res;

    if( ( a == 0 ) && ( b == 0 ) ) {
        if( d == 0 ) {
            if( c > 0 ) {
                res.realpart = 0;
                res.imagpart = 0;
                return( res );
            }
        }
        arg1.realpart = a;
        arg1.imagpart = b;
        arg2.realpart = c;
        arg2.imagpart = d;
        return( __qmath2err( FP_FUNC_POW | M_DOMAIN | V_ZERO, &arg1, &arg2 ) );
    } else {
        u = DLOG( a*a + b*b ) / 2;
        v = DATAN2( b, a );
        res.realpart = DEXP( u*c - v*d );
        res.imagpart = res.realpart;
        u = u*d + v*c;
        res.realpart *= DCOS( u );
        res.imagpart *= DSIN( u );
        return( res );
    }
}
