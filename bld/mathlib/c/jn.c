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
* Description:  Implementation of jn() Bessel function.
*
****************************************************************************/


#include "variety.h"
#include <math.h>
#include "pdiv.h"

#define ACC     40.0
#define BIGNO   1.0e10
#define BIGNI   1.0e-10

_WMRTLINK double jn( int n, double x )
/************************************/
{
    int     j, jsum, m;
    double  ax, bj, bjm, bjp, sum, tox, ans;

    ax = fabs( x );
    if( ax == 0.0 ) {
        return( 0.0 );
    }
    tox = PDIV( 2.0, x );
    if( ax > (double)n ) { /* Use upwards recurrence from j0 and j1 */
        bjm = j0( x );
        bj  = j1( x );
        for( j = 1; j < n; j++ ) {
            bjp = j * tox * bj - bjm;
            bjm = bj;
            bj = bjp;
        }
        ans = bj;
    } else {                /* Use downwards recurrence from an even m */
        m = 2 * ( (n + (int)sqrt( ACC * n )) / 2 );
        jsum = 0;
        bjp = 0.0;
        ans = 0.0;
        sum = 0.0;
        bj = 1.0;
        for( j = m; j > 0; j-- ) {
            bjm = j * tox * bj - bjp;
            bjp = bj;
            bj = bjm;
            if( fabs(bj) > BIGNO ) { /* renormalize to prevent overflows */
                bj  *= BIGNI;
                bjp *= BIGNI;
                ans *= BIGNI;
                sum *= BIGNI;
            }
            if( jsum ) {
                sum += bj;          /* accumulate the sum */
            }
            jsum = ! jsum;          /* change 0 to !0 and vice-versa */
            if( j == n ) {
                ans = bjp;          /* save the unnormalized answer */
            }
        }
        sum = 2.0 * sum - bj;
        ans = PDIV( ans, sum );
    }
    if( x < 0.0  &&  n % 2 == 1 ) {
        ans = -ans;
    }
    return( ans );
}
