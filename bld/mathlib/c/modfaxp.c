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
* Description:  Floating-point modulo routine.
*
****************************************************************************/


#include "variety.h"
#include <float.h>
#include "xfloat.h"
#include "mathlib.h"

_WMRTLINK double modf( double x, double *iptr )
{
    double      frac;
    long_double ld;
    int         exp;

    ld.value = x;
    switch( __LDClass( &ld ) ) {
    case __ZERO:
        frac = 0.0;
        *iptr = 0.0;
        break;
    case __DENORMAL:
        frac = x;
        *iptr = 0.0;
        break;
    case __NAN:
    case __INFINITY:
        frac = 0.0;
        *iptr = x;
        break;
    case __NONZERO:
        exp = ((ld.word[1] >> 20)&0x7FF) - 0x3FE;
        if( exp < 0 ) {
            frac = x;
            *iptr = 0.0;
        } else if( exp > DBL_MANT_DIG ) {
            frac = 0.0;
            *iptr = x;
        } else {
            int quot;
            __fprem( x, 1.0, &quot, &frac );
            *iptr = x - frac;
        }
        break;
    }
    return( frac );
}
