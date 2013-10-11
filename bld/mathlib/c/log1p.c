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
* Description:  Computes the natural logarithm of 1+x.
*
****************************************************************************/

#include "xfloat.h"

static const double
ln2_hi  =  6.93147180369123816490e-01,	/* 3fe62e42 fee00000 */
ln2_lo  =  1.90821492927058770002e-10,	/* 3dea39ef 35793c76 */
two54   =  1.80143985094819840000e+16,  /* 43500000 00000000 */
Lp1 = 6.666666666666735130e-01,  /* 3FE55555 55555593 */
Lp2 = 3.999999999940941908e-01,  /* 3FD99999 9997FA04 */
Lp3 = 2.857142874366239149e-01,  /* 3FD24924 94229359 */
Lp4 = 2.222219843214978396e-01,  /* 3FCC71C5 1D8E78AF */
Lp5 = 1.818357216161805012e-01,  /* 3FC74664 96CB03DE */
Lp6 = 1.531383769920937332e-01,  /* 3FC39A09 D078C69F */
Lp7 = 1.479819860511658591e-01;  /* 3FC2F112 DF3E5244 */

static double zero = 0.0;

_WMRTLINK double log1p(double x)
{
	double hfsq,f,c,s,z,R;
	int k,hx,hu,ax;

    float_double fdx;
    float_double fdu;

    fdx.u.value = x;

	hx = fdx.u.word[1];		        /* high word of x */
	ax = hx&0x7fffffff;

	k = 1;
	if (hx < 0x3FDA827A) 			/* x < 0.41422  */
    {
	    if(ax>=0x3ff00000)  		/* x <= -1.0 */
        {
		    if(x==-1.0)             /* log1p(-1)=+inf */
                return -two54/zero; 
		    else 	                /* log1p(x<-1)=NaN */
                return (x-x)/(x-x);
	    }
        
	    if(ax<0x3e200000) 			/* |x| < 2**-29 */
        {
		    if(two54+x>zero && ax<0x3c900000)			/* raise inexact */ /* |x| < 2**-54 */
                return x;
		    else
		        return x - x*x*0.5;
	    }
        
	    if(hx>0||hx<=((int)0xbfd2bec3))                 /* -0.2929<x<0.41422 */
        {
            k=0;
            f=x;
            hu=1;
        }	
	} 
    
	if (hx >= 0x7ff00000) 
        return x+x;
	
    if(k != 0) 
    {
	    if(hx<0x43400000) 
        {
		    fdu.u.value  = 1.0 + x;
            hu = fdu.u.word[1];
	        k  = (hu >> 20) - 1023;
	        c  = (k>0) ? 1.0-(fdu.u.value-x) : x-(fdu.u.value-1.0);   /* correction term */
		    c /= fdu.u.value;
	    }
        else
        {
		    fdu.u.value  = x;
            hu = fdu.u.word[1];
	        k  = (hu>>20) - 1023;
		    c  = 0;
	    }
	    
        fdu.u.word[1] &= 0x000fffff;
	    
        if(fdu.u.word[1] < 0x6a09e) 
        {
	        fdu.u.word[1] = hu|0x3ff00000;	/* normalize u */
	    }
        else
        {
	        k += 1; 
	        fdu.u.word[1] = hu|0x3fe00000;	/* normalize u/2 */
	        hu = (0x00100000-hu)>>2;
	    }
	    f = fdu.u.value-1.0;
	}
    
	hfsq=0.5*f*f;
	if(hu==0)                   /* |f| < 2**-20 */
    {	
	    if(f==zero) 
        {
            if(k==0) 
                return zero;  
			else 
            {
                c += k*ln2_lo; 
                return k*ln2_hi+c;
            }
        }
	    
        R = hfsq*(1.0-0.66666666666666666*f);
	    
        if(k==0) 
            return f-R; 
        else
            return k*ln2_hi-((R-(k*ln2_lo+c))-f);
	}
    
 	s = f/(2.0+f); 
	z = s*s;
	R = z*(Lp1+z*(Lp2+z*(Lp3+z*(Lp4+z*(Lp5+z*(Lp6+z*Lp7))))));
	if(k==0) 
        return f-(hfsq-s*(hfsq+R)); 
    else
	    return k*ln2_hi-((hfsq-(s*(hfsq+R)+(k*ln2_lo+c)))-f);
}
