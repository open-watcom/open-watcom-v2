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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include "mathlib.h"


_WMRTLINK dcomplex _IF_C16PowI( double a, double b, intstar4 i )
//==============================================================
{
// Complex power to an integer exponent.

    double      t;
    dcomplex    arg1;
    dcomplex    arg2;
    dcomplex    res;

    if( ( a == 0 ) && ( b == 0 ) ) {
        if( i > 0 ) {
            res.realpart = 0;
            res.imagpart = 0;
            return( res );
        }
        arg1.realpart = a;
        arg1.imagpart = b;
        arg2.realpart = i;
        arg2.imagpart = 0;
        return( __qmath2err( FUNC_POW | M_DOMAIN | V_ZERO, &arg1, &arg2 ) );
    } else {
        if( i < 0 ) {
            i = -i;
            t = a*a + b*b;
            a = a / t;
            b = -b / t;
        }
        res.realpart = 1;
        res.imagpart = 0;
        while( i > 0 ) {
            if( i % 2 == 0 ) {
                t = a;
                a = a*a - b*b;
                b = 2*t*b;
                i = i / 2;
            } else {
                t = res.realpart;
                res.realpart = res.realpart*a - res.imagpart*b;
                res.imagpart = t*b + res.imagpart*a;
                --i;
            }
        }
        return( res );
    }
}
