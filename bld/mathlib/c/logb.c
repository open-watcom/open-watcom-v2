/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (C) 1993 by Sun Microsystems, Inc.
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
*    Based on FDLIBM
*
*    Developed at SunSoft, a Sun Microsystems, Inc. business.
*    Permission to use, copy, modify, and distribute this
*    software is freely granted, provided that this notice 
*    is preserved.
*
*  ========================================================================
*
* Description:  Retrieves the exponent from a floating-point number and 
*               returns the value as a floating-point number.
*
****************************************************************************/

#include "variety.h"
#include <math.h>
#include <float.h>
#include "xfloat.h"

_WMRTLINK double logb( double x )
{
    i4              lx, ix;
    float_double    fdx;
    
    fdx.u.value = x;
    
    ix = fdx.u.word[1] & (u4)0x7fffffff;    /* high |x| */
    lx = fdx.u.word[0];                     /* low x */

    if( (ix | lx) == 0 ) 
        return( -_INFINITY );
    if( ix == (u4)0x7ff00000 && lx == 0 ) 
        return( _INFINITY );

    if( ix >= (u4)0x7ff00000 ) 
        return( x );
        
    if( (ix >>= 20) == 0 ) {                /* IEEE 754 logb */
        return( -1022.0 );
    } else {
        return( (double)( ix - 1023 ) );
    }
}
