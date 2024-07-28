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


#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "cfloati.h"

static  int     Adder( int a, int b )
/***********************************/
{
    return( a + b );
}

static  int     Suber( int a, int b )
/***********************************/
{
    return( a - b );
}

int     CFOrder( cfloat *f1, cfloat *f2 )
/****************************************
 * compare absolute value of f1 and f2
 *
 * return value
 *
 *      = 0     | f1 | = | f2 |
 *      > 0     | f1 | > | f2 |
 *      < 0     | f1 | < | f2 |
 */
{
    int         i;
    int         diff;

    diff = f1->exp - f2->exp;
    if( diff == 0 ) {
        diff = f1->len - f2->len;
        for( i = 0; i < f1->len && i < f2->len; i++ ) {
            if( f1->mant[i] != f2->mant[i] ) {
                diff = (unsigned char)f1->mant[i] - (unsigned char)f2->mant[i];
                break;
            }
        }
    }
    return( diff );
}

static  int     Max( int a, int b )
/*********************************/
{
    if( b > a )
        return( b );
    return( a );
}


static  int     Min( int a, int b )
/*********************************/
{
    if( b < a )
        return( b );
    return( a );
}

static  cfloat  *CSSum( cfhandle h, cfloat *f1, cfloat *f2, int (*arith)( int, int ) )
/************************************************************************************/
{
    int         carry;
    int         pos;
    int         length;
    int         f1left;
    int         f2left;
    int         farleft;
    int         f1right;
    int         f2right;
    int         farright;
    cfloat      *result;

    f1left = f1->exp;
    f2left = f2->exp;
    f1right = f1left - f1->len;
    f2right = f2left - f2->len;
    farleft = Max( f1left, f2left );
    farright = Min( f1right, f2right );
    length = farleft - farright + 1;           /* result length + extra digit*/
    pos = farright + 1;
    result = CFAlloc( h, length );
    result->exp = farleft + 1;
    result->len = length;
    carry = 0;
    length--;
    while( pos <= farleft ) {
        if( pos > f1right
          && pos <= f1left ) {
            carry += CFAccess( f1, f1left - pos );
        }
        if( pos > f2right
          && pos <= f2left ) {
            carry = arith( carry, CFAccess( f2, f2left - pos ) );
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
    result->sign = f1->sign;
    /*
     * normalize result
     */
    CFClean( result );
    return( result );
}


cfloat  *CFAdd( cfhandle h, cfloat *f1, cfloat *f2 )
/**************************************************/
{
    int         ord;

    switch( f1->sign + 3 * f2->sign ) {
    case -4:
    case  4:
        return( CSSum( h, f1, f2, &Adder ) );
    case -3:                 /* f1 is zero*/
    case  3:
        return( CFCopy( h, f2 ) );
    case -2:                 /* different signs*/
    case  2:                 /* different signs*/
        ord = CFOrder( f1, f2 );
        if( ord < 0 ) {
            return( CSSum( h, f2, f1, &Suber ) );   /* | f1 | < | f2 | */
        }
        if( ord > 0 ) {
            return( CSSum( h, f1, f2, &Suber ) );   /* | f1 | > | f2 | */
        }
        return( CFAlloc( h, 1 ) );                  /* | f1 | = | f2 | */
    case -1:                 /* f2 is zero*/
    case  1:                 /* f2 is zero*/
        return( CFCopy( h, f1 ) );
    case  0:
        return( CFAlloc( h, 1 ) );
    }
    return( NULL ); // shut up compiler
}

cfloat  *CFSub( cfhandle h, cfloat *f1, cfloat *f2 )
/**************************************************/
{
    cfloat      *result;
    int         ord;

    switch( f1->sign + 3 * f2->sign ) {
    case -4:
    case  4:
        ord = CFOrder( f1, f2 );
        if( ord < 0 ) {
            result = CSSum( h, f2, f1, &Suber );    /* | f1 | < | f2 | */
            CFNegate( result );
            return( result );
        }
        if( ord > 0 ) {
            return( CSSum( h, f1, f2, &Suber ) );   /* | f1 | > | f2 | */
        }
        return( CFAlloc( h, 1 ) );                  /* | f1 | = | f2 | */
    case -3:                 /* f1 is zero*/
    case  3:                 /* f1 is zero*/
        result = CFCopy( h, f2 );
        CFNegate( result );
        return( result );
    case -2:                 /* different signs*/
    case  2:                 /* different signs*/
        return( CSSum( h, f1, f2, &Adder ) );
    case -1:                 /* f2 is zero*/
    case  1:                 /* f2 is zero*/
        return( CFCopy( h, f1 ) );
    case  0:
        return( CFAlloc( h, 1 ) );
    }
    return( NULL ); // shut up compiler
}

void    CFNegate( cfloat *f )
/***************************/
{
    f->sign = -f->sign;
}

int     CFCompare( cfloat *f1, cfloat *f2 )
/*****************************************/
{
    int     cmp;

    if( f1->sign < f2->sign ) {
        return( -1 );
    }
    if( f1->sign > f2->sign ) {
        return( 1 );
    }
    /*
     * f1 sign = f2 sign
     */
    cmp = CFOrder( f1, f2 );
    if( cmp == 0 )              /* | f1 | = | f2 | */
        return( 0 );
    if( cmp > 0 ) {             /* | f1 | > | f2 | */
        return( f1->sign );
    }
    return( -1 * f1->sign );    /* | f1 | < | f2 | */
}

int     CFTest( cfloat *f )
/*************************/
{
    return( f->sign );
}

int     CFExp( cfloat *f )
/************************/
{
    return( f->exp );
}

int     CFAccess( cfloat *f, int index )
/**************************************/
{
    return( f->mant[index] - '0' );
}

void    CFDeposit( cfloat *f, int index, int data )
/*************************************************/
{
    f->mant[index] = (char)data + '0';
}

void    CFClean( cfloat *f )
/***************************
 * normalize number data
 */
{
    int         headindex;
    char        *head;
    int         new_len;

    for( new_len = f->len; new_len > 0; --new_len ) {
        if( f->mant[new_len - 1] != '0' )
            break;
        if( new_len == 1 ) { /* it's zero!*/
            f->exp = 1;
            f->sign = 0;
            f->len = 1;
            f->mant[1] = NULLCHAR;
            return;
        }
    }
    f->mant[new_len] = NULLCHAR;
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
    f->mant[new_len] = NULLCHAR;
}
