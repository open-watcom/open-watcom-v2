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
*    Translated from Fortran and based on specfun
*    http://www.netlib.org/specfun/erf
*
*  ========================================================================
*
* Description:  Error function and complimentary error function of x
*
****************************************************************************/

#include "variety.h"
#include <math.h>
#include "pi.h"

#define THRESH  0.46875E0

#define XINF    1.79E308
#define XNEG   -26.628E0
#define XSMALL  1.11E-16
#define XBIG   26.543E0
#define XHUGE   6.71E7
#define XMAX    2.53E307

/* Coefficients for approximation to erf in first interval */
#define A1      3.16112374387056560E00
#define A2      1.13864154151050156E02
#define A3      3.77485237685302021E02
#define A4      3.20937758913846947E03
#define A5      1.85777706184603153E-1

#define B1      2.36012909523441209E01
#define B2      2.44024637934444173E02
#define B3      1.28261652607737228E03
#define B4      2.84423683343917062E03

/* Coefficients for approximation to  erfc  in second interval */
#define C1      5.64188496988670089E-1
#define C2      8.88314979438837594E0
#define C3      6.61191906371416295E01
#define C4      2.98635138197400131E02
#define C5      8.81952221241769090E02
#define C6      1.71204761263407058E03
#define C7      2.05107837782607147E03
#define C8      1.23033935479799725E03
#define C9      2.15311535474403846E-8

#define D1      1.57449261107098347E01
#define D2      1.17693950891312499E02
#define D3      5.37181101862009858E02
#define D4      1.62138957456669019E03
#define D5      3.29079923573345963E03
#define D6      4.36261909014324716E03
#define D7      3.43936767414372164E03
#define D8      1.23033935480374942E03

/* Coefficients for approximation to  erfc  in third interval */
#define P1      3.05326634961232344E-1
#define P2      3.60344899949804439E-1
#define P3      1.25781726111229246E-1
#define P4      1.60837851487422766E-2
#define P5      6.58749161529837803E-4
#define P6      1.63153871373020978E-2

#define Q1      2.56852019228982242E00
#define Q2      1.87295284992346047E00
#define Q3      5.27905102951428412E-1
#define Q4      6.05183413124413191E-2
#define Q5      2.33520497626869185E-3

static double calerf(double x, int jint)
{
double y, ysq;
double xnum, xden, result;
double del;

    y = fabs(x);
    
    /* Evaluate  erf  for  |X| <= 0.46875 */
    if(y <= THRESH) 
    {
        ysq = 0;
        if(y > XSMALL)
            ysq = y*y;
        xnum = A5*ysq;
        xden = ysq;
        
        xnum = (xnum + A1)*ysq; xden = (xden + B1)*ysq;
        xnum = (xnum + A2)*ysq; xden = (xden + B2)*ysq;
        xnum = (xnum + A3)*ysq; xden = (xden + B3)*ysq;
        
        result = x * (xnum + A4) / (xden + B4);
        
        if(jint != 0)
            result = 1.0 - result;
        if(jint == 2)
            result = exp(ysq) * result;
        
        return result;
    }
    /* Evaluate  erfc  for 0.46875 <= |X| <= 4.0 */
    else if(y <= 4.0) 
    {
        xnum = C9 * y;
        xden = y;
        
        xnum = (xnum + C1) * y; xden = (xden + D1) * y;
        xnum = (xnum + C2) * y; xden = (xden + D2) * y;
        xnum = (xnum + C3) * y; xden = (xden + D3) * y;
        xnum = (xnum + C4) * y; xden = (xden + D4) * y;
        xnum = (xnum + C5) * y; xden = (xden + D5) * y;
        xnum = (xnum + C6) * y; xden = (xden + D6) * y;
        xnum = (xnum + C7) * y; xden = (xden + D7) * y;
        
        result = (xnum + C8) / (xden + D8);
        
        if(jint != 2) 
        {
            ysq = floor(y*16.0) / 16.0;
            del = (y - ysq) * (y + ysq);
            result = exp(-ysq*ysq) * exp(-del) * result;
        }
    }
    /* Evaluate  erfc  for |X| > 4.0 */
    else
    {
        result = 0.0;
        if(y >= XBIG) 
        {
            if(jint != 2 || y >= XMAX)
                goto signcorrect;
            if(y >= XHUGE) 
            {
                result = OnebySqrtPI / y;
                goto signcorrect;
            }
        }
        ysq = 1.0 / (y*y);
        
        xnum = P6*ysq;
        xden = ysq;
        
        xnum = (xnum + P1)  * ysq; xden = (xden + Q1)  * ysq;
        xnum = (xnum + P2)  * ysq; xden = (xden + Q2)  * ysq;
        xnum = (xnum + P3)  * ysq; xden = (xden + Q3)  * ysq;
        xnum = (xnum + P4)  * ysq; xden = (xden + Q4)  * ysq;
        
        result = ysq * (xnum + P5) / (xden + Q5);
        result = (OnebySqrtPI - result) / y;
        if(jint != 2) 
        {
            ysq = floor(y*16.0)/16.0;
            del = (y-ysq)*(y+ysq);
            result = exp(-ysq*ysq) * exp(-del) * result;
        }
        
    }

signcorrect:
    if(jint == 0) 
    {
        result = (0.5 - result) + 0.5;
        if(x < 0)
            result = -result;
    } 
    else if(jint == 1)
    {
        if(x < 0)
            result = 2.0 - result;
    }
    else
    {
        if(x < 0) 
        {
            if(x < XNEG)
            {
                result = XINF;
            }
            else
            {
                ysq = ceil(16.0*x) / 16.0;
                del = (x-ysq) * (x+ysq);
                y = exp(ysq*ysq) * exp(del);
                result = (y + y) - result;
            }
        }
    }
    return result;
}

_WMRTLINK double erf(double x)
{
    return calerf(x, 0);
}

_WMRTLINK double erfc(double x)
{
    return calerf(x, 1);
}
