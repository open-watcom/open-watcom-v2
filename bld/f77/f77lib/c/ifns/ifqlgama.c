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
// IFQLGAMA     : log of gamma function for extended PRECISION argument
//

#include "fmath.h"
#include <float.h>

#include "ftnstd.h"
#include "ifenv.h"
#include "ifqlgama.h"


static  const extended __FAR  p1[9] = {  6.304933722864032e02,
                           1.389482659233250e02,
                          -2.331861065739548e03,
                          -2.651470392943388e03,
                          -8.953073589022869e02,
                          -9.229503102917111e01,
                          -1.940352203312667e00,
                           4.368019694395194e00,
                           1.279153645893113e02 };
static  const extended __FAR  p2[9] = {  1.071722590306920e04,
                           6.527047912184606e04,
                           1.176398389569621e05,
                          -9.726314581896472e03,
                          -1.266094622188023e05,
                          -5.393468741199669e04,
                          -3.895916159676326e03,
                           5.397392180667399e00,
                           5.334390026324024e02 };
static  const extended __FAR  p3[9] = { -6.114039864945718e07,
                          -5.588132821261888e08,
                          -9.078970022444525e08,
                           3.662935130796460e09,
                           4.658700336821218e09,
                          -4.570725249206307e09,
                          -2.220833171087439e09,
                          -1.474322990113017e04,
                          -1.959850795570400e06 };
static  const extended __FAR  p4[7] = {  8.40596949829e-04,
                          -5.9523334141881e-04,
                           7.9365078409227e-04,
                          -2.777777777769526e-03,
                           8.333333333333333e-02,
                           9.189385332046727e-01,
                          -1.7816839846e-03 };
static  const extended __FAR  q1[8] = { 6.689575153359349e02,
                          2.419887329355996e03,
                          3.354196974608081e03,
                          1.860416170944268e03,
                          3.944307810159532e02,
                          2.682132440551618e01,
                          3.440812622259858e-01,
                          5.948212550303777e01 };
static  const extended __FAR  q2[8] = { 5.314589562326176e03,
                          5.493654949398033e04,
                          2.205757574602192e05,
                          3.602313576600391e05,
                          2.273446951911101e05,
                          4.560612434396495e04,
                          1.702062439974796e03,
                          1.670328399370593e02 };
static  const extended __FAR  q3[8] = { -5.636057205056241e05,
                          -2.691827587118628e07,
                          -4.411606716771217e08,
                          -2.774890551941383e09,
                          -6.579874397740792e09,
                          -4.980644951174248e09,
                          -6.677373781427094e08,
                          -2.722530175870899e03 };
static  const extended __FAR  xinf  = { LDBL_MAX };
static  const extended __FAR  pi    = { 3.141592653589793e0 };
static  const extended __FAR  eps   = { LDBL_EPSILON };
static  const extended __FAR  big   = { 1.28118e305 };

extern  extended          QINT(extended);
extern  extended          QMOD(extended,extended);


extended  __lgamma( extended arg, const extended __FAR *my_inf ) {
//================================================================

    bool        mflag;
    int         j;
    extended      sign;
    extended      y;
    extended      t;
    extended      r;
    extended      top;
    extended      den;
    extended      a;
    extended      b;

    mflag = false;
    t = arg;
    if( t < 0.0 ) { // argument is negative
        mflag = true;
        t = -t;
        r = QINT( t );
        sign = 1.0;
        if( QMOD( t, 2.0 ) == 0.0 ) {
            sign = -1.0;
        }
        r = t - r;
        if( r == 0.0 ) {
            return( *my_inf );
        }
        // argument is not a negative integer
        r = pi / sin( r * pi ) * sign;
        t = t + 1.0;
        r = log( fabs( r ) );
    }
    // evaluate approximation for ln(gamma(t)), t > 0.0
    if( t > 12.0 ) {
        top = log( t );
        top = t * ( top - 1.0 ) - .5 * top;
        t = 1.0 / t;
        y = top;
        if( t >= eps ) {
            b = t * t;
            a = p4[6];
            for( j = 0; j <= 4; ++j ) {
                a = a * b + p4[j];
            }
            y = a * t + p4[5] + top;
        }
        if( mflag ) {
            y = r - y;
        }
        return( y );
    } else if( t > 4.0 ) {
        top = p3[7] * t + p3[8];
        den = t + q3[7];
        for( j = 0; j <= 6; ++j ) {
            top = top * t + p3[j];
            den = den * t + q3[j];
        }
        y = top / den;
        if( mflag ) {
            y = r - y;
        }
        return( y );
    } else if( t >= 1.5e0 ) {
        b = t - 1.0;
        top = p2[7] * t + p2[8];
        den = t + q2[7];
        a = b - 1.0;
        for( j = 0; j <= 6; ++j ) {
            top = top * t + p2[j];
            den = den * t + q2[j];
        }
        y = ( top / den ) * a;
        if( mflag ) {
            y = r - y;
        }
        return( y );
    } else {
        if( t >= .5 ) {
            top = t - .5;
            b = 0.0;
            a = top - .5;
        } else {
            b = -log( t );
            a = t;
            t = t + 1.0;
            if( a < eps ) {
                return( b );
            }
        }
        top = p1[7] * t + p1[8];
        den = t + q1[7];
        for( j = 0; j <= 6; ++j ) {
            top = top * t + p1[j];
            den = den * t + q1[j];
        }
        y = ( top / den ) * a + b;
        if( mflag ) {
            y = r - y;
        }
        return( y );
    }
}


extended  QLGAMA( extended arg ) {
//================================

    if( fabs( arg ) >= big ) return( xinf );
    if( arg == 0.0 ) return( xinf );
    return( __lgamma( arg, &xinf ) );
}

extended XQLGAMA( extended *arg ) {
//=================================

    return( QLGAMA( *arg ) );
}
