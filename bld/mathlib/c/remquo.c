/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (C) 2002 by  Red Hat, Incorporated.
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
*  Some portions developed by Red Hat, Incorporated.  Permission to use, 
*  copy, modify, and distribute this software is freely granted, provided 
*  that this notice is preserved.
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

_WMRTLINK double remquo(double x, double y, int *quo)
{
int signx, signy, signres;
double x_over_y;

    *quo = 0;

    signx = _FDSign(x);
    signy = _FDSign(y);

    signres = (signx ^ signy) ? -1 : 1;
    x_over_y = fabs(x / y);

    *quo = signres * (lrint(x_over_y) & 0x7f);

    return remainder(x,y);
}
