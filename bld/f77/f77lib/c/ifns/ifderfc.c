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
// IFDERFC      : 1 - error function for DOUBLE PRECISION argument
//

#include "fmath.h"

#include "ftnstd.h"
#include "ifenv.h"

extern const struct {
    double p[5];
    double q[4];
    double p1[9];
    double q1[8];
    double p2[6];
    double q2[5];
    double xmin;
    double xlarge;
    double xbig;
    double sqrpi;
    double pt477;
    double _4pt0;
} __FAR __derfdat;      // defined in ifderfdt.c


double  DERFC( double arg ) {
//===========================

    int         i;
    int         isw;
    double      res;
    double      xsq;
    double      xnum;
    double      xden;
    double      xi;

    isw = 1;
    if( arg < 0.0 ) {
        isw = -1;
        arg = -arg;
    }
    if( arg < __derfdat.pt477 ) {
        // abs(arg) < .477, evaluate approximation for erfc()
        if( arg >= __derfdat.xmin ) {
            xsq = arg*arg;
            xnum = __derfdat.p[3]*xsq+__derfdat.p[4];
            xden = xsq+__derfdat.q[3];
            for( i = 0; i <= 2; ++i ) {
                xnum = xnum*xsq+__derfdat.p[i];
                xden = xden*xsq+__derfdat.q[i];
            }
            res = arg*xnum/xden;
        } else {
            res = arg*__derfdat.p[3]/__derfdat.q[3];
        }
        if( isw == -1 ) {
            res = -res;
        }
        res = 1 - res;
    } else if( arg <= __derfdat._4pt0 ) {
        // .477 <= abs(arg) <= 4.0 evaluate approximation for erfc()
        xsq = arg*arg;
        xnum = __derfdat.p1[7]*arg+__derfdat.p1[8];
        xden = arg+__derfdat.q1[7];
        for( i = 0; i <= 6; ++i ) {
           xnum = xnum*arg+__derfdat.p1[i];
           xden = xden*arg+__derfdat.q1[i];
        }
        res = xnum/xden;
        res = res*exp(-xsq);
        if( isw == -1 ) {
            res = 2.0 - res;
        }
    } else if( ( isw > 0 ) || ( arg < __derfdat.xlarge ) ) {
        if( ( isw > 0 ) && ( arg > __derfdat.xbig ) ) {
            return( 0.0 );
        }
        // 4.0 < abs(arg), evaluate minimax approximation for erfc()
        xsq = arg*arg;
        xi = 1.0/xsq;
        xnum = __derfdat.p2[4]*xi+__derfdat.p2[5];
        xden = xi+__derfdat.q2[4];
        for( i = 0; i <= 3; ++i ) {
           xnum = xnum*xi+__derfdat.p2[i];
           xden = xden*xi+__derfdat.q2[i];
        }
        res = (__derfdat.sqrpi+xi*xnum/xden)/arg;
        res = res*exp(-xsq);
        if( isw == -1 ) {
            res = 2.0 - res;
        }
    } else {
        res = 2.0;
    }
    return( res );
}

double XDERFC( double *arg ) {
//============================

    return( DERFC( *arg ) );
}
