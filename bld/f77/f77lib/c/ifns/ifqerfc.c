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
// IFQERFC      : 1 - error function for extended PRECISION argument
//

#include "fmath.h"

#include "ftnstd.h"
#include "ifenv.h"

extern const struct {
    extended p[5];
    extended q[4];
    extended p1[9];
    extended q1[8];
    extended p2[6];
    extended q2[5];
    extended xmin;
    extended xlarge;
    extended xbig;
    extended sqrpi;
    extended pt477;
    extended _4pt0;
} __FAR __qerfdat;      // defined in ifderfdt.c


extended  QERFC( extended arg ) {
//===========================

    int         i;
    int         isw;
    extended      res;
    extended      xsq;
    extended      xnum;
    extended      xden;
    extended      xi;

    isw = 1;
    if( arg < 0.0 ) {
        isw = -1;
        arg = -arg;
    }
    if( arg < __qerfdat.pt477 ) {
        // abs(arg) < .477, evaluate approximation for erfc()
        if( arg >= __qerfdat.xmin ) {
            xsq = arg*arg;
            xnum = __qerfdat.p[3]*xsq+__qerfdat.p[4];
            xden = xsq+__qerfdat.q[3];
            for( i = 0; i <= 2; ++i ) {
                xnum = xnum*xsq+__qerfdat.p[i];
                xden = xden*xsq+__qerfdat.q[i];
            }
            res = arg*xnum/xden;
        } else {
            res = arg*__qerfdat.p[3]/__qerfdat.q[3];
        }
        if( isw == -1 ) {
            res = -res;
        }
        res = 1 - res;
    } else if( arg <= __qerfdat._4pt0 ) {
        // .477 <= abs(arg) <= 4.0 evaluate approximation for erfc()
        xsq = arg*arg;
        xnum = __qerfdat.p1[7]*arg+__qerfdat.p1[8];
        xden = arg+__qerfdat.q1[7];
        for( i = 0; i <= 6; ++i ) {
           xnum = xnum*arg+__qerfdat.p1[i];
           xden = xden*arg+__qerfdat.q1[i];
        }
        res = xnum/xden;
        res = res*exp(-xsq);
        if( isw == -1 ) {
            res = 2.0 - res;
        }
    } else if( ( isw > 0 ) || ( arg < __qerfdat.xlarge ) ) {
        if( ( isw > 0 ) && ( arg > __qerfdat.xbig ) ) {
            return( 0.0 );
        }
        // 4.0 < abs(arg), evaluate minimax approximation for erfc()
        xsq = arg*arg;
        xi = 1.0/xsq;
        xnum = __qerfdat.p2[4]*xi+__qerfdat.p2[5];
        xden = xi+__qerfdat.q2[4];
        for( i = 0; i <= 3; ++i ) {
           xnum = xnum*xi+__qerfdat.p2[i];
           xden = xden*xi+__qerfdat.q2[i];
        }
        res = (__qerfdat.sqrpi+xi*xnum/xden)/arg;
        res = res*exp(-xsq);
        if( isw == -1 ) {
            res = 2.0 - res;
        }
    } else {
        res = 2.0;
    }
    return( res );
}

extended XQERFC( extended *arg ) {
//============================

    return( QERFC( *arg ) );
}
