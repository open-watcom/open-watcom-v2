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
* Description:  Rounds the argument to a nearby integer based on the 
*               current rounding direction
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <math.h>
#include <fenv.h>
#include <limits.h>
#include "_matherr.h"

double __rint_checked(double x, double limit, const char *funcname)
{
double fres;

    if(__math_errhandling_flag != 0 && 
       (isinf(x) || isnan(x)))
    {
        __reporterror(DOMAIN, funcname, x, 0, 0);
        return 0;
    } 

    fres = rint(x);
    
    if(__math_errhandling_flag != 0 && 
       (fabs(fres) > limit)) {
        __reporterror(DOMAIN, funcname, x, 0, 0);
        return 0;
    } 
    
    return fres;
}

_WMRTLINK long lrint(double x)
{
    return (long)__rint_checked(x, LONG_MAX, __func__);
}

_WMRTLINK long long llrint(double x)
{    
    return (long long)__rint_checked(x, LONG_MAX, __func__);
}
