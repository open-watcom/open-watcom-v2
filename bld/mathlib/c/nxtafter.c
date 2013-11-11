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
* Description:  Return the next machine floating-point number of x in the
*	            direction toward y.
*
****************************************************************************/

#include <math.h>
#include "xfloat.h"

_WMRTLINK double nextafter(double x, double y)
{
	u4 hx,hy,ix,iy;
	u4 lx,ly;
    
    float_double fdx;
    float_double fdy;
    
    fdx.u.value = x;
    fdy.u.value = y;

	hx = fdx.u.word[1];		/* high word of x */
	lx = fdx.u.word[0];		/* low  word of x */
	hy = fdy.u.word[1];		/* high word of y */
	ly = fdy.u.word[0];		/* low  word of y */
	ix = hx & ((u4)0x7fffffff);	/* |x| */
	iy = hy & ((u4)0x7fffffff);	/* |y| */

	if(((ix>=((u4)0x7ff00000)) && ((ix-((u4)0x7ff00000)) | lx)!=0) ||   /* x is nan */ 
	   ((iy>=((u4)0x7ff00000)) && ((iy-((u4)0x7ff00000)) | ly)!=0))     /* y is nan */ 
    {
        return x+y;
    }
    
	if(x==y) 
        return x;		/* x=y, return x */
        
	if((ix|lx) == 0) 			/* x == 0 */
    {
	    fdx.u.word[1] = hy&((u4)0x80000000);	/* return +-minsubnormal */
	    fdx.u.word[0] = 1;
	    y = fdx.u.value*fdx.u.value;
        
	    if(y==fdx.u.value) 
            return y; 
        else 
            return fdx.u.value;	/* raise underflow flag */
	}
	
    if(hx>=0) 				/* x > 0 */
    {
	    if(hx > hy || ((hx == hy) && (lx > ly)))  	/* x > y, x -= ulp */
        {
		    if(lx==0) 
                hx -= 1;
		    lx -= 1;
	    } 
        else 				/* x < y, x += ulp */
        {
    		lx += 1;
		    if(lx==0) 
                hx += 1;
	    }
	} 
    else 				/* x < 0 */
    {
	    if(hy >= 0 || hx > hy || ((hx == hy) && (lx > ly))) /* x < y, x -= ulp */
        {
		    if(lx==0) 
                hx -= 1;
		    lx -= 1;
	    } 
        else 				/* x > y, x += ulp */
        {
		    lx += 1;
		    if(lx==0) 
                hx += 1;
	    }
	}
    
	hy = hx & ((u4)0x7ff00000);
    
	if(hy >= ((u4)0x7ff00000))
        return x+x;	/* overflow  */
	
    if(hy<((u4)0x00100000)) 		/* underflow */
    {
	    y = x*x;
	    if(y!=x) 		/* raise underflow flag */
        {
		    fdy.u.word[1] = hx; 
            fdy.u.word[0] = lx;
		    return fdy.u.value;
	    }
	}
	
    fdx.u.word[1] = hx;
    fdx.u.word[0] = lx;
    
	return fdx.u.value;
}
