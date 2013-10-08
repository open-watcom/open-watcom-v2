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
* Description:  Returns  x*(2**n) computed by exponent manipulation rather 
*               than by actually performing an exponentiation or a 
*               multiplication.
*
****************************************************************************/

#include <math.h>
#include "xfloat.h"

static const double
two54   =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
twom54  =  5.55111512312578270212e-17, /* 0x3C900000, 0x00000000 */
huge   = 1.0e+300,
tiny   = 1.0e-300;

_WMRTLINK double scalbn (double x, int n)
{
    int  k,hx,lx;
    
    float_double fdx;
    
    fdx.u.value = x;
    
	hx = fdx.u.word[1];
	lx = fdx.u.word[0];
    
    /* extract exponent */
    k = (hx & 0x7ff00000) >> 20;
    
    if (k==0) 				/* 0 or subnormal x */
    {
        if ((lx|(hx&0x7fffffff))==0) /* +-0 */
            return x; 
        x *= two54; 
        fdx.u.value = x;
        hx = fdx.u.word[1];
        k = ((hx & 0x7ff00000) >> 20) - 54;
        if (n< -50000) 
            return tiny*x; 	/*underflow*/
    }
    if (k==0x7ff) 
        return x+x;		/* NaN or Inf */
        
    k = k+n; 
    if (k >  0x7fe) 
        return huge*copysign(huge,x); /* overflow  */
        
    if (k > 0) 				/* normal result */
    {
        fdx.u.word[1] = (hx & 0x800fffff) | (k<<20);
        return fdx.u.value;
    }
    
    if (k <= -54)
    {
        if (n > 50000) 	/* in case integer overflow in n+k */
            return huge*copysign(huge,x);	/*overflow*/
        else 
            return tiny*copysign(tiny,x); 	/*underflow*/
    }
    
    k += 54;				/* subnormal result */
    
    fdx.u.word[1] = (hx & 0x800fffff) | (k << 20);
    
    return fdx.u.value*twom54;
}
