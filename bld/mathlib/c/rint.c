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
* Description:  Rounds the argument to a nearby integer without possibility
*               of an exception base don the current rounding direction
*
* Notes: This function should throw an exception if the result of rounding 
*        will be either inexact or overflow the double.  It currently does 
*        neither.
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <math.h>
#include <fenv.h>

_WMRTLINK double rint(double x)
{
int roundmode;

    roundmode = fegetround();
    switch(roundmode) {
        case FE_DOWNWARD:
            return floor(x);
        case FE_UPWARD:
            return ceil(x);
        case FE_TOWARDZERO:
            if(x < 0)
                return ceil(x);
            else
                return floor(x);
        case FE_TONEAREST:
        default:
            return round(x);
    }

}
