/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2014 Open Watcom contributors. 
*    All Rights Reserved.
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
* Description:  Computes the exponential of x minus 1.  More accurate than
*               exp(x)-1 for small x.
*
* Notes: Uses a power series and probably isn't wildly efficient...
*
* Author: J. Armstrong
****************************************************************************/

#include "variety.h"
#include <math.h>

#if defined(_M_I86)
 #define MAX_FACTORIAL   8
#else
 #define MAX_FACTORIAL   12
#endif

_WMRTLINK double expm1(double x)
{
double y;
double last, ret;
int i;
unsigned int denom;
int signx;

    signx = signbit(x);
    if(signx != 0)
        y = fabs(x);
    else
        y = x;
        
    /* Special value checks:
     *   x = +- 0.0 -> return  0.0
     *   x = -inf   -> return -1.0
     *   x = +inf   -> return inf
     *   x =  NaN   -> return NaN
     */
    if(y == 0.0 || isnan(x))
        return x;
    else if(isinf(y)) {
        if(signx != 0)
            return -1.0;
        else
            return x;
    }
    
    /* If magnitude is big enough, just calculate it */
    if(y > 0.675)
    {
        ret = exp(x) - 1.0;
    }
    /* For negative, a power series or taylor series can't be used
     * directly
     */
    else if(signx != 0)
    {
        ret = -expm1(y) / exp(y);
    }
    else
    {
        denom = 1; i = 1;
        ret = x;
        last = 0.0;
        while(last != ret && i <= MAX_FACTORIAL) 
        {
            last = ret;
            
            y *= x;
            denom *= (++i);
            ret += y / (double)denom;
        }
    }
    
    return ret;
}
