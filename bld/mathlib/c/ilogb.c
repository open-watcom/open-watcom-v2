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
* Description:  Retrieves the exponent from a floating-point number and 
*               returns the value as an integer.
*
****************************************************************************/

#include <math.h>
#include "xfloat.h"

_WMRTLINK int ilogb(double x)
{
	u4 hx,lx,ix;
    
    float_double fdx;
    
    fdx.u.value = x;

	hx  = (fdx.u.word[1]) & ((u4)0x7fffffff);	    /* high word of x */
	if(hx<0x00100000) {
	    lx = fdx.u.word[0];
        
	    if((hx|lx)==0) 
		    return (int)((u4)0x80000001);	/* ilogb(0) = 0x80000001 */
	    else			/* subnormal x */
        {
		    if(hx==0) 
            {
		        for (ix = -1043; lx>0; lx<<=1) ix -=1;
		    } else {
		        for (ix = -1022,hx<<=11; hx>0; hx<<=1) ix -=1;
		    }
        }
	    return ix;
	}
	else if (hx < ((u4)0x7ff00000)) 
        return (hx>>20)-1023;
	else 
        return (int)((u4)0x7fffffff);
}
