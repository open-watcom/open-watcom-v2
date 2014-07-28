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
*    Translated from Fortran and based on specfun
*    http://www.netlib.org/specfun/gamma
*
*  ========================================================================
*
* Description:  Returns the value of the Gamma function
*
****************************************************************************/

#include "variety.h"
#include <math.h>
#include <float.h>
#include "pi.h"

#define P1  -1.71618513886549492533811
#define P2   2.47656508055759199108314E+1
#define P3  -3.79804256470945635097577E+2
#define P4   6.29331155312818442661052E+2
#define P5   8.66966202790413211295064E+2
#define P6  -3.14512729688483675254357E+4
#define P7  -3.61444134186911729807069E+4
#define P8   6.64561438202405440627855E+4

#define Q1  -3.08402300119738975254353E+1
#define Q2   3.15350626979604161529144E+2
#define Q3  -1.01515636749021914166146E+3
#define Q4  -3.10777167157231109440444E+3
#define Q5   2.25381184209801510330112E+4
#define Q6   4.75584627752788110767815E+3
#define Q7  -1.34659959864969306392456E+5
#define Q8  -1.15132259675553483497211E+5

#define C1  -1.910444077728E-03
#define C2   8.4171387781295E-04
#define C3  -5.952379913043012E-04
#define C4   7.93650793500350248E-04
#define C5  -2.777777777777681622553E-03
#define C6   8.333333333333333331554247E-02
#define C7   5.7083835261E-03

#define XBIG    171.624
#define XMININ    2.23E-308
#define XINF      _INFINITY
#define XNAN      NAN


_WMRTLINK double tgamma(double x)
{
int i, n;
int parity;
double fact;
double y, y1, ysq, z;
double res;
double summation;
double xnum, xden;
    
    parity = 0;
    fact = 1.0;
    n = 0;
    y = x;
    
    if(isnan(x) || x == -INFINITY)
        return XNAN;
    
    /* Argument is negative */
    if(y < 0)
    {
        y = -x;
        y1 = trunc(y);
        res = y-y1;
        if(res != 0) 
        {
            if(y1 != trunc(0.5*y1)*2.0) 
                parity = 1;
            fact = -PI / sin(PI*res);
            y += 1.0;
        }
        else 
            return XNAN;
    }
    
    /* Argument < eps */
    if(y < DBL_EPSILON) {
        if(y >= XMININ)
            res = 1.0 / y;
        else
            return XINF;
    }
    else if(y < 12.0)
    {   
        y1 = y;
        if(y < 1.0)
        {
            z = y;
            y += 1.0;
        }
        else
        {
            n = (int)y - 1;
            y -= (double)n;
            z = y - 1.0;
        }
        
        xnum = 0.0;
        xnum = (xnum + P1)*z;
        xnum = (xnum + P2)*z;
        xnum = (xnum + P3)*z;
        xnum = (xnum + P4)*z;
        xnum = (xnum + P5)*z;
        xnum = (xnum + P6)*z;
        xnum = (xnum + P7)*z;
        xnum = (xnum + P8)*z;
        
        xden = 1.0;
        xden = xden*z + Q1;
        xden = xden*z + Q2;
        xden = xden*z + Q3;
        xden = xden*z + Q4;
        xden = xden*z + Q5;
        xden = xden*z + Q6;
        xden = xden*z + Q7;
        xden = xden*z + Q8;
        
        res = xnum  / xden + 1.0;
        
        if(y1 < y) 
            res = res / y1;
        else if(y1 > y) {
            for(i=0; i < n; i++) {
                res = res * y;
                y = y + 1.0;
            }
        }
        
    }
    else
    {
        if(y <= XBIG)
        {
            ysq = y * y;
            summation = C7;
            summation = summation / ysq + C1;
            summation = summation / ysq + C2;
            summation = summation / ysq + C3;
            summation = summation / ysq + C4;
            summation = summation / ysq + C5;
            summation = summation / ysq + C6;
            
            summation = summation/y - y + LnSqrt2PI;
            summation += (y-0.5)*log(y);
            res = exp(summation);
        }
        else
            return XINF;
    }
    
    if(parity == 1)
        res = -res;
        
    if(fact != 1.0)
        res = fact/res;
    
    return res;
}
