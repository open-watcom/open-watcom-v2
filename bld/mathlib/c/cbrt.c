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
*    Based on FDLIBM
*    Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
*
*    Developed at SunSoft, a Sun Microsystems, Inc. business.
*    Permission to use, copy, modify, and distribute this
*    software is freely granted, provided that this notice 
*    is preserved.
*
*  ========================================================================
*
* Description:  Return cube root of x
*
****************************************************************************/

#include "variety.h"
#include <math.h>
#include "xfloat.h"

static const i4 
    B1 = 715094163, /* B1 = (682-0.03306235651)*2**20 */
    B2 = 696219795; /* B2 = (664-0.03306235651)*2**20 */

static const double
    C =  5.42857142857142815906e-01, /* 19/35     = 0x3FE15F15, 0xF15F15F1 */
    D = -7.05306122448979611050e-01, /* -864/1225 = 0xBFE691DE, 0x2532C834 */
    E =  1.41428571428571436819e+00, /* 99/70     = 0x3FF6A0EA, 0x0EA0EA0F */
    F =  1.60714285714285720630e+00, /* 45/28     = 0x3FF9B6DB, 0x6DB6DB6E */
    G =  3.57142857142857150787e-01; /* 5/14      = 0x3FD6DB6D, 0xB6DB6DB7 */


_WMRTLINK double cbrt( double x ) 
{
    float_double    fdx, fdt;
	u4 				hx;
    double          r,s,w;
    i4              sign;

    fdx.u.value = x;
	hx = fdx.u.word[1];         /* high word of x */
	sign = hx & 0x80000000;     /* sign = sign(x) */
	hx ^= (u4)sign;             /* hx <- |hx| ) */

    if( hx >= 0x7FF00000 )           /* cbrt(NaN,INF) is itself */
        return( x );

    if( (hx | fdx.u.word[0]) == 0 )  /* cbrt(0) is itself */
        return( x );

	fdx.u.word[1] = hx;	            /* fdx <- |x| */
    
    fdt.u.value = 0.0;
    
    /* rough cbrt to 5 bits */
    if( hx < 0x00100000 ) {                 /* subnormal number */
        fdt.u.word[1]= 0x43500000;          /* set t= 2**54 */
        fdt.u.value *= fdx.u.value; 
        fdt.u.word[1] = fdt.u.word[1] / 3 + B2;
    } else {
        fdt.u.word[1] = hx / 3 + B1;
    }

    /* new cbrt to 23 bits, may be implemented in single precision */
    r = fdt.u.value * fdt.u.value / fdx.u.value;
    s = C + r * fdt.u.value;
    fdt.u.value *= G + F / ( s + E + D / s );   

    /* chopped to 20 bits and make it larger than cbrt(x) */ 
    fdt.u.word[0] = 0; 
    fdt.u.word[1] += 1;

    /* one step newton iteration to 53 bits with error less than 0.667 ulps */
    s = fdt.u.value * fdt.u.value;          /* t*t is exact */
    r = fdx.u.value / s;
    w = fdt.u.value + fdt.u.value;
    r = ( r - fdt.u.value ) / ( w + r );    /* r-s is exact */
    fdt.u.value = fdt.u.value + fdt.u.value * r;

    /* restore the sign bit */
    fdt.u.word[1] |= sign;
        
    return( fdt.u.value );
}
