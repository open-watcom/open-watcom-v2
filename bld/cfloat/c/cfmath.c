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


#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "cfloati.h"

static  int     Adder( int a, int b ) {
/*************************************/

    return( a + b );
}

static  int     Suber( int a, int b ) {
/*************************************/

    return( a - b );
}

int     CFOrder( cfloat *float1, cfloat *float2 ) {
/*************************************************/

    int         index;
    int         diff;

    if( float1->exp > float2->exp ) return( 1 );
    if( float1->exp < float2->exp ) return( -1 );
    index = 0;
    for(;;) {
        if( index >= float1->len ) break;
        if( index >= float2->len ) return( 1 );
        diff = CFAccess( float1, index ) - CFAccess( float2, index );
        if( diff < 0 ) return( -1 );
        if( diff > 0 ) return( 1 );
        index++;
    }
    if( index < float2->len ) return( -1 );
    return( 0 );        /* | float1 | == | float2 |*/
}

static  int     Max( int a, int b ) {
/***********************************/

    if( b > a ) return( b );
    return( a );
}


static  int     Min( int a, int b ) {
/***********************************/

    if( b < a ) return( b );
    return( a );
}

static  cfloat  *CSSum( cfloat *op1, cfloat *op2, int (*arith)( int, int ) )
/***************************************************************************/
{
    int         carry;
    int         pos;
    int         length;
    int         op1left;
    int         op2left;
    int         farleft;
    int         op1right;
    int         op2right;
    int         farright;
    cfloat      *result;

    op1left = op1->exp;
    op2left = op2->exp;
    op1right = op1left - op1->len;
    op2right = op2left - op2->len;
    farleft = Max( op1left, op2left );
    farright = Min( op1right, op2right );
    length = farleft - farright + 1;           /* result length + extra digit*/
    pos = farright + 1;
    result = CFAlloc( length );
    result->exp = farleft + 1;
    result->len = length;
    carry = 0;
    length--;
    while( pos <= farleft ) {
        if( pos > op1right && pos <= op1left ) {
            carry += CFAccess( op1, op1left - pos );
        }
        if( pos > op2right && pos <= op2left ) {
            carry = arith( carry, CFAccess( op2, op2left - pos ) );
        }
        if( carry < 0 ) {
            CFDeposit( result, length, carry + 10 );
            carry = -1;
        } else if( carry > 9 ) {
            CFDeposit( result, length, carry - 10 );
            carry = 1;
        } else {
            CFDeposit( result, length, carry );
            carry = 0;
        }
        pos++;
        length--;
    }
    CFDeposit( result, length, carry );
    result->sign = op1->sign;
    CFClean( result );
    return( result );
}



cfloat  *CFAdd( cfloat *op1, cfloat *op2 ) {
/******************************************/

    int         ord;

    switch( op1->sign + 3 * op2->sign ) {
    case -4:
    case  4:
        return( CSSum( op1, op2, &Adder ) );
    case -3:                 /* Op1 is zero*/
    case  3:
        return( CFCopy( op2 ) );
    case -2:                 /* different signs*/
    case  2:                 /* different signs*/
        ord = CFOrder( op1, op2 );
        if( ord == -1 ) {
            return( CSSum( op2, op1, &Suber ) );  /* | op1 | < | op2 |*/
        } else if( ord == 1 ) {
            return( CSSum( op1, op2, &Suber ) );  /* | op1 | > | op2 |*/
        } else {
            return( CFAlloc( 1 ) );
        }
    case -1:                 /* Op2 is zero*/
    case  1:                 /* Op2 is zero*/
        return( CFCopy( op1 ) );
    case  0:
        return( CFAlloc( 1 ) );
    }
    return( NULL ); // shut up compiler
}

cfloat  *CFSub( cfloat *op1, cfloat *op2 ) {
/******************************************/

    cfloat      *result;
    int         ord;

    switch( op1->sign + 3 * op2->sign ) {
    case -4:
    case  4:
        ord = CFOrder( op1, op2 );
        if( ord == -1 ) {
            result = CSSum( op2, op1, &Suber );        /* | op1 | < | op2 |*/
            CFNegate( result );
            return( result );
        } else if( ord == 1 ) {
            return( CSSum( op1, op2, &Suber ) );        /* | op1 | > | op2 |*/
        } else {
            return( CFAlloc( 1 ) );
        }
    case -3:                 /* Op1 is zero*/
    case  3:                 /* Op1 is zero*/
        result = CFCopy( op2 );
        CFNegate( result );
        return( result );
    case -2:                 /* different signs*/
    case  2:                 /* different signs*/
        return( CSSum( op1, op2, &Adder ) );
    case -1:                 /* Op2 is zero*/
    case  1:                 /* Op2 is zero*/
        return( CFCopy( op1 ) );
    case  0:
        return( CFAlloc( 1 ) );
    }
    return( NULL ); // shut up compiler
}

void    CFNegate( cfloat *f ) {
/*****************************/

    f->sign = -f->sign;
}

int     CFCompare( cfloat *op1, cfloat *op2 ) {
/*********************************************/

    if( op1->sign < op2->sign ) {
        return( -1 );
    } else if( op1->sign > op2->sign ) {
        return( 1 );
    } else {
        return( op1->sign * CFOrder( op1, op2 ) );
    }
}

int     CFTest( cfloat *f ) {
/***************************/

    return( f->sign );
}

int     CFAccess( cfloat *f, int index ) {
/****************************************/

    return( *(f->mant + index) - '0' );
}

void    CFDeposit( cfloat *f, int index, int data ) {
/***************************************************/

    *(f->mant + index) = (char)data + '0';
}

void    CFClean( cfloat *f ) {
/****************************/

    int         headindex;
    char        *head;
    int         new_len;

    for( new_len = f->len; new_len > 0; --new_len ) {
        if( *(f->mant + new_len - 1) != '0' )
            break;
        if( new_len == 1 ) { /* it's zero!*/
            f->exp = 1;
            f->sign = 0;
            f->len = 1;
            *(f->mant + 1) = NULLCHAR;
            return;
        }
    }
    *(f->mant + new_len) = NULLCHAR;
    headindex = 0;
    head = f->mant;
    while( *head == '0' ) {
        head++;
        headindex++;
    }
    if( headindex > 0 ) {
        new_len -= headindex;
        memmove( f->mant, head, new_len );
        f->exp -= headindex;
    }
    if( new_len > CF_MAX_PREC ) {
        new_len = CF_MAX_PREC;
    }
    f->len = new_len;
    *(f->mant + new_len) = NULLCHAR;
}
