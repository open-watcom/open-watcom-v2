/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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


#include "cfloati.h"


static  cfloat  *SDMul( cfhandle h, cfloat *f, int plyer, int fleft, int fexp, int byexp )
/****************************************************************************************/
{
    int         sum;
    int         exp;
    cfloat      *result;
    int         resexp;
    int         respos;

    sum = 0;
    exp = fexp;
    resexp = fexp + byexp - 1;
    respos = fleft - fexp + 1;
    result = CFAlloc( h, respos + 1 );
    result->len = respos + 1;
    result->exp = fleft + byexp;
    while( exp <= fleft ) {
        sum += CFAccess( f, f->exp - exp++ ) * plyer;
        CFDeposit( result, respos--, sum % 10 );
        sum /= 10;
        resexp++;
    }
    CFDeposit( result, respos, sum % 10 );
    result->sign = f->sign;
    /*
     * normalize result
     */
    CFClean( result );
    return( result );
}

cfloat  *CFMul( cfhandle h, cfloat *f1, cfloat *f2 )
/**************************************************/
{
    cfloat      *result;
    cfloat      *temp;
    cfloat      *sum;
    int         f1left;
    int         f1exp;
    int         f2exp;
    int         f2ptr;
    signed char sgn;

    sgn = f1->sign * f2->sign;
    result = CFAlloc( h, 1 );
    if( sgn != 0 ) {
        f1left = f1->exp;
        f1exp = f1left - f1->len + 1;
        f2exp = f2->exp - f2->len + 1;
        f2ptr = f2->len - 1;
        while( f2ptr >= 0 ) {
            temp = SDMul( h, f1, CFAccess( f2, f2ptr-- ),
                f1left, f1exp, f2exp++ );
            sum = CFAdd( h, result, temp );
            CFFree( h, temp );
            CFFree( h, result );
            result = sum;
        }
        result->sign = sgn;
    }
    return( result );
}
