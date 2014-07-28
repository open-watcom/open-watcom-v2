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
// POWC32          : complex power run-time routine
//

#include "fmath.h"

#include "ftnstd.h"
#include "rtenv.h"
#include "errcod.h"
#include "ifenv.h"
#include "fmthcode.h"

/*
extern  extended          QSIN(extended);
extern  extended          QCOS(extended);
extern  extended          QLOG(extended);
extern  extended          QEXP(extended);
extern  extended          QATAN2(extended,extended);
*/
dcomplex C16Pow( double a, double b, double c, double d );

xcomplex C32Pow( extended a, extended b, extended c, extended d ) {
//=========================================================

// Return the real part of the result of taking one extended precision
// complex number to the power of another.
// ( a, b ) ** ( c, d ) = exp( ( c,d ) * log( a,b ) )

    dcomplex    arg1 = C16Pow( a, b, c, d );
    xcomplex    arg2;
    arg2.realpart = arg1.realpart;
    arg2.imagpart = arg1.imagpart;
    return( arg2 );  // kill this when extendeds are enabled

/*      UNCOMMENT_TOKEN
    extended      u;
    extended      v;
    xcomplex    arg1;
    xcomplex    arg2;
    xcomplex    res;

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
        u = QLOG( a*a + b*b ) / 2;
        v = QATAN2( b, a );
        res.realpart = QEXP( u*c - v*d );
        res.imagpart = res.realpart;
        u = u*d + v*c;
        res.realpart *= QCOS( u );
        res.imagpart *= QSIN( u );
        return( res );
    }
    */
}
