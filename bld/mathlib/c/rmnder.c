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
* Description: Computes the remainder of the floating point division 
*              operation x/y.  Value will be negative if fmod(x, y)>y/2. 
*
****************************************************************************/

#include "variety.h"
#include <math.h>
#include "_matherr.h"

_WMRTLINK double remainder(double x, double y)
{
double r;

    if(__math_errhandling_flag != 0 && 
       ((isinf(x) && !isnan(y)) || (y == 0 && !isnan(x))))
    {
        __reporterror(DOMAIN, __func__, x, y, NAN);
        return NAN;
    }   

    r = fmod(x, y);
    if(r > y/2.0)
        return -r;
    else
        return r;
}
