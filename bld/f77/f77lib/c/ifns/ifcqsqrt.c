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
// IFCQSQRT     : square root function for COMPLEX*20 argument
//

#include "fmath.h"

#include "ftnstd.h"
#include "ifenv.h"


xcomplex        CQSQRT( extended rp, extended ip ) {
//==============================================

// Return the complex square root of "arg".

    extended      r;
    extended      t;
    extended      x;
    extended      y;
    xcomplex    result;

    // treat the imaginary number as a vector [magnitude m,angle 2*theta]
    // square root is equal to [magnitude sqrt(m),angle theta]

    if( ip == 0.0 ) {
        if( rp < 0.0 ) {
            // arg is (a,0) where a is negative
            result.realpart = 0.0;
            result.imagpart = sqrt( -rp );
        } else {
            // arg is (a,0) where a is positive
            result.realpart = sqrt( rp );
            result.imagpart = 0.0;
        }
    } else if( rp == 0.0 ) {
        if( ip < 0.0 ) {
            // arg is (0,a) where a is negative
            t = sqrt( (-ip) / 2 );
            result.imagpart = -t;
        } else {
            // arg is (0,a) where a is positive
            t = sqrt( ip / 2 );
            result.imagpart = t;
        }
        result.realpart = t;
    } else {
        x = fabs( rp );
        y = fabs( ip );
        if( x >= y ) {
            r = y / x;
            t = sqrt( x ) * sqrt( ( 1 + sqrt( 1 + r * r ) ) / 2 );
        } else {
            r = x / y;
            t = sqrt( y ) * sqrt( ( r + sqrt( 1 + r * r ) ) / 2 );
        }
        if( rp >= 0.0 ) {
            result.realpart = t;
            result.imagpart = ip / ( 2 * t );
        } else {
            if( ip >= 0 ) {
                result.imagpart = t;
            } else {
                result.imagpart = -t;
            }
            result.realpart = ip / ( 2 * result.imagpart );
        }
    }
    return( result );
}

xcomplex        _CQSQRT( xcomplex *arg ) {
//========================================

    return( CQSQRT( arg->realpart, arg->imagpart ) );
}
