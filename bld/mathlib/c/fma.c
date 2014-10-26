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
* Description: Performs a fake fused multiply-add in long double in an
*              attempt to improve accuracy
*
****************************************************************************/

#include "variety.h"
#include <math.h>
#include "_matherr.h"

_WMRTLINK double fma(double a, double b, double c)
{
long double product, res;

    if(__math_errhandling_flag != 0) {
        if((a==0.0 && isinf(b)) || (b==0.0 && isinf(a))) {
            __reporterror(DOMAIN, __func__, a, b, NAN);
            return NAN;
        }
    }
    
    /* Not strictly an error condition, but a predefined result */
    if(isnan(a) || isnan(b) || isnan(c))
        return NAN;
    
    /* Check product versus c for error handling */
    product = ((long double)a)*((long double)b);
    
    if(isinf(product) && isinf(c) && _FLSign(product) != _FDSign(c)) {
        if(__math_errhandling_flag != 0)
            __reporterror(DOMAIN, __func__, a, b, NAN);
        return NAN;
    }
    
    res = product + (long double)c;
    
    /* If our result is the same as either the product or the additive
     * argument and neither was zero, we've experienced catastrophic
     * cancelation.  We should report a partial loss of significance.
     */
    if(__math_errhandling_flag != 0 && 
       ((res == product && product != 0.0) || 
        (res == (long double)c && c != 0.0)))
    {
        __reporterror(PLOSS, __func__, a, b, (double)res);
    }
    
    return (double)res;
}
