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


#include "cfloat.h"

extern  cfloat  *       CFAlloc(int);
extern  int             CFAccess(cfloat*,int);
extern  cfloat  *       CFAdd(cfloat*,cfloat*);
extern  void            CFFree(cfloat*);
extern  void            CFDeposit(cfloat*,int,int);
extern  void            CFClean(cfloat*);

static  cfloat  *SDMul( cfloat *op1, int plyer, int op1left,
                        int op1exp, int byexp ) {
/*******************************************************************/

    int         sum;
    int         exp;
    cfloat      *result;
    int         resexp;
    int         respos;

    sum = 0;
    exp = op1exp;
    resexp = op1exp + byexp - 1;
    respos = op1left - op1exp + 1;
    result = CFAlloc( respos + 1 );
    result->len = respos + 1;
    result->exp = op1left + byexp;
    while( exp <= op1left ) {
        sum += CFAccess( op1, op1->exp - exp++ ) * plyer;
        CFDeposit( result, respos--, sum % 10 );
        sum /= 10;
        resexp++;
    }
    CFDeposit( result, respos, sum % 10 );
    result->sign = op1->sign;          /* by convention*/
    CFClean( result );
    return( result );
}

extern  cfloat  *CFMul( cfloat *op1, cfloat *op2 ) {
/**************************************************/

    cfloat      *result;
    cfloat      *temp;
    cfloat      *sum;
    int         op1left;
    int         op1exp;
    int         op2exp;
    int         op2ptr;
    int         sgn;

    sgn = op1->sign * op2->sign;
    result = CFAlloc( 1 );
    if( sgn != 0 ) {
        op1left = op1->exp;
        op1exp = op1left - op1->len + 1;
        op2exp = op2->exp - op2->len + 1;
        op2ptr = op2->len - 1;
        while( op2ptr >= 0 ) {
            temp = SDMul( op1, CFAccess( op2, op2ptr-- ),
                op1left, op1exp, op2exp++ );
            sum = CFAdd( result, temp );
            CFFree( temp );
            CFFree( result );
            result = sum;
        }
        result->sign = sgn;
    }
    return( result );
}

