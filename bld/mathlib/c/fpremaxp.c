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


#include <math.h>
#include <float.h>
#include <limits.h>

#include "variety.h"
#include "xfloat.h"

void __fprem( double x, double y, int *quot, double *rem ) {
    int sign_x;
    int sign_y;
    int q;

    if( y == 0.0 ) {                                // if modulus is zero
        *quot = 0;                                  // set quot to zero
        *rem = 0.0;                                 // set rem to zero
        return;
    }

    sign_x = 1;
    if( x < 0.0 ) {
        sign_x = -1;
        x = -x;
    }
    sign_y = 1;
    if( y < 0.0 ) {
        sign_y = -1;
        y = -y;
    }
    q = 0;
    while( x >= y ) {
        int ex;
        double mx = frexp( x, &ex );
        int ey;
        double my = frexp( y, &ey );
        double ny;

        #if 0   // use this code when modf doesn't call __fprem
        if( ( ex - ey ) <= DBL_MAX_EXP ) {
            double d;
            double ipart;

            d = x / y;
            modf( d, &ipart );
            q = ipart;
            x -= ipart * y;
            break;
        }
        #endif
        if( mx >= my ) {
            ny = ldexp( my, ex );
        } else {
            --ex;
            ny = ldexp( my, ex );
        }
        ex -= ey;
        if( ex <= ( sizeof( *quot ) * CHAR_BIT ) ) {
            q += 1 << ex;
        }
        x -= ny;
    }
    if( sign_x == -1 ) {
        x = -x;
    }
    *rem = x;
    if( sign_x != sign_y ) {
        q = -q;
    }
    *quot = q;
}
