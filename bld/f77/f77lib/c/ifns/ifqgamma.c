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
// IFQGAMMA     : gamma function for extended PRECISION argument
//

#include "fmath.h"
#include <float.h>

#include "ftnstd.h"
#include "ifenv.h"

static  const extended __FAR  p[9] = { -5.966047488753637e01,
                          5.864023793062003e01,
                         -1.364106217165365e03,
                         -8.117569271425580e02,
                         -1.569414683149179e04,
                         -1.525979925758372e04,
                         -7.264059615964330e04,
                         -8.972275718101010e-01,
                          3.349618189847578e00 };
static  const extended __FAR  q[8] = {  4.103991474182904e02,
                         -2.262590291514875e03,
                          2.494325576714903e03,
                          2.362106244383048e04,
                         -5.741873227396418e04,
                         -7.257239715408240e04,
                         -9.491399521686949e00,
                         -3.255006939455704e01 };
static  const extended __FAR  p4[7] = {  8.40596949829e-04,
                          -5.9523334141881e-04,
                           7.9365078409227e-04,
                          -2.777777777769526e-03,
                           8.333333333333333e-02,
                           9.189385332046727e-01,
                          -1.7816839846e-03 };
static  const extended __FAR  xinf  = { LDBL_MAX };
static  const extended __FAR  xmin  = { LDBL_MIN };
static  const extended __FAR  big   = { 171.0e0 };

extern  extended          QINT(extended);
extern  extended          QMOD(extended,extended);


extended  __gamma( extended arg, const extended __FAR *my_inf ) {
//=========================================================

    bool        mflag;
    int         i;
    int         j;
    extended      sign;
    extended      y;
    extended      t;
    extended      r;
    extended      top;
    extended      den;
    extended      a;
    extended      b;

    mflag = FALSE;
    t = arg;
    if( t <= 0.0 ) { // argument is negative
        mflag = TRUE;
        t = -t;
        r = QINT( t );
        sign = 1.0;
        if( QMOD( r, 2.0 ) == 0.0 ) {
            sign = -1.0;
        }
        r = t - r;
        if( r == 0.0 ) {
            if( sign == -1.0 ) {
                return( -*my_inf );
            } else {
                return( *my_inf );
            }
        }
    }
    // evaluate approximation for gamma(t), t > xmin
    if( t <= 12.0 ) {
        i = t;
        a = 1.0;
        if( i == 0 ) { // 0.0 < t < 1.0
            a = a / ( t * ( t + 1.0 ) );
            t = t + 2.0;
        } else if( i == 1 ) { // 1.0 <= t < 2.0
            a = a / t;
            t = t + 1.0;
        } else if( i > 2 ) { // 3.0 <= t <= 12.0
            for( j = 3; j <= i; ++j ) {
                t = t - 1.0;
                a = a * t;
            }
        }
        top = p[7] * t + p[8];
        den = t + q[7];
        for( j = 0; j <= 6; ++j ) {
            top = top * t + p[j];
            den = den * t + q[j];
        }
        y = ( top / den ) * a;
        if( mflag ) {
            y = r/y;
        }
        return( y );
    } else { // t > 12.0
        top = log( t );
        top = t * ( top - 1.0 ) -.5 * top;
        t = 1.0 / t;
        b = t * t;
        a = p4[6];
        for( j = 0; j <= 4; ++j ) {
            a = a * b + p4[j];
        }
        y = a * t + p4[5] + top;
        y = exp( y );
        if( mflag ) {
            y = r/y;
        }
        return( y );
    }
}


extended  QGAMMA( extended arg ) {
//============================

    if( fabs( arg ) <= xmin ) {
        if( arg <= 0.0 ) {
            return( -xinf );
        } else {
            return( xinf );
        }
    } else if( fabs( arg ) >= big ) {
        return( xinf );
    }
    return( __gamma( arg, &xinf ) );
}

extended XQGAMMA( extended *arg ) {
//=============================

    return( QGAMMA( *arg ) );
}
