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


#include <stddef.h>
#include "i64.h"

/*
        Be careful of overlap between input parameters and output results.
*/

#define L       I64LO32
#define H       I64HI32

void I32ToI64( signed_32 in, signed_64 *res )
{
    res->u._32[L] = in;
    res->u._32[H] = (in < 0) ? -1L : 0L;
}

void U32ToU64( unsigned_32 in, unsigned_64 *res )
{
    res->u._32[L] = in;
    res->u._32[H] = 0;
}

void U64Neg( const unsigned_64 *a, unsigned_64 *res )
{
    res->u._32[L] = ~a->u._32[L];
    res->u._32[H] = ~a->u._32[H];
    if( ++res->u._32[L] == 0 ) ++res->u._32[H];
}

void U64Add( const unsigned_64 *a, const unsigned_64 *b, unsigned_64 *res )
{
    unsigned_32 new_value;

    res->u._32[H] = a->u._32[H] + b->u._32[H];
    new_value = a->u._32[L] + b->u._32[L];
    if( new_value < a->u._32[L] ) ++res->u._32[H];
    res->u._32[L] = new_value;
}

void U64IncDec( unsigned_64 *a, signed_32 i )
{
    unsigned_32 new_value;

    new_value = a->u._32[L] + i;
    if( i >= 0 ) {
        if( new_value < a->u._32[L] ) ++a->u._32[H];
    } else {
        if( new_value > a->u._32[L] ) --a->u._32[H];
    }
    a->u._32[L] = new_value;
}

void U64Sub( const unsigned_64 *a, const unsigned_64 *b, unsigned_64 *res )
{
    unsigned_64 tmp;

    U64Neg( b, &tmp );
    U64Add( a, &tmp, res );
}

void U64Mul( const unsigned_64 *a, const unsigned_64 *b, unsigned_64 *res )
{
    unsigned_64         tmp_a;
    unsigned_64         tmp_b;

    tmp_a = *a;
    tmp_b = *b;

    res->u._32[L] = 0;
    res->u._32[H] = 0;
    while( tmp_b.u._32[L] != 0 || tmp_b.u._32[H] != 0 ) {
        if( tmp_b.u._32[L] & 1 ) U64Add( &tmp_a, res, res );
        U64ShiftL( &tmp_a, 1, &tmp_a );
        U64ShiftR( &tmp_b, 1, &tmp_b );
    }
}

void U64Div( const unsigned_64 *a, const unsigned_64 *b,
                unsigned_64 *div, unsigned_64 *rem )
{
    unsigned_64 tmp_a;
    unsigned_64 tmp_b;
    int         count;
    unsigned_32 tmp;

    if( a->u._32[H] == 0 && b->u._32[H] == 0 ) {
        /* both fit in 32-bit, use hardware divide */
        tmp = a->u._32[L] / b->u._32[L];
        if( rem != NULL ) {
            rem->u._32[L] = a->u._32[L] % b->u._32[L];
            rem->u._32[H] = 0;
        }
        div->u._32[L] = tmp;
        div->u._32[H] = 0;
    } else {
        tmp_a = *a;
        tmp_b = *b;
        div->u._32[L] = 0;
        div->u._32[H] = 0;
        count = 0;
        for( ;; ) {
            if( tmp_b.u.sign.v ) break;
            if( U64Cmp( &tmp_a, &tmp_b ) <= 0 ) break;
            U64ShiftL( &tmp_b, 1, &tmp_b );
            ++count;
        }
        while( count >= 0 ) {
            U64ShiftL( div, 1, div );
            if( U64Cmp( &tmp_a, &tmp_b ) >= 0 ) {
                div->u._32[L] |= 1;
                U64Sub( &tmp_a, &tmp_b, &tmp_a );
            }
            U64Shift( &tmp_b, 1, &tmp_b );
            --count;
        }
        if( rem != NULL ) *rem = tmp_a;
    }
}

void I64Div( const signed_64 *a, const signed_64 *b,
                signed_64 *div,    signed_64 *rem )
{
    unsigned_64 tmp_a;
    unsigned_64 tmp_b;
    int         neg;
    int         remneg;

    neg = 0;
    remneg = 0;
    if( a->u.sign.v ){
        U64Neg( a, &tmp_a );
        neg ^= 1;
        remneg = 1;
    }else{
        tmp_a = *a;
    }
    if( b->u.sign.v ){
        U64Neg( b, &tmp_b );
        neg ^= 1;
    }else{
        tmp_b = *b;
    }
    U64Div( &tmp_a, &tmp_b, div, rem );
    if( neg ){
        U64Neg( div, div );
    }
    if( remneg && (rem != NULL) ){
        U64Neg( rem, rem );
    }
}

int U64Cmp( const unsigned_64 *a, const unsigned_64 *b )
{
    if( a->u._32[H] > b->u._32[H] ) return( +1 );
    if( a->u._32[H] < b->u._32[H] ) return( -1 );
    if( a->u._32[L] > b->u._32[L] ) return( +1 );
    if( a->u._32[L] < b->u._32[L] ) return( -1 );
    return( 0 );
}

int U64Test( const unsigned_64 *a )
{
    if( a->u._32[H] > 0 ) return( +1 );
    if( a->u._32[L] > 0 ) return( +1 );
    return( 0 );
}

int I64Cmp( const signed_64 *a, const signed_64 *b )
{
    signed_32 ah;
    signed_32 bh;

    ah = a->u._32[H];
    bh = b->u._32[H];
    if( ah > bh ) return( +1 );
    if( ah < bh ) return( -1 );
    if( a->u._32[L] > b->u._32[L] ) return( +1 );
    if( a->u._32[L] < b->u._32[L] ) return( -1 );
    return( 0 );
}

int I64Test( const signed_64 *a )
{
    signed_32 ah;

    ah = a->u._32[H];

    if( ah > 0 ) return( +1 );
    if( ah < 0 ) return( -1 );
    if( a->u._32[L] > 0 ) return( +1 );
    return( 0 );
}


void I64ShiftR( const signed_64 *a, unsigned shift, signed_64 *res )
{
    unsigned_32       save;
    signed_32         tmp;

    shift &= 0x3f;
    if( shift < 32 ){
        tmp = a->u._32[H];
        save = tmp;
        save <<= 1;  // incase shift == 0
        res->u._32[H] = tmp >> shift;          //arithmetic shift
        res->u._32[L] = a->u._32[L] >> shift;
        res->u._32[L] |= save << (31-shift);
    }else{
        shift -= 32;
        tmp = a->u._32[H];
        res->u._32[H] = (tmp < 0) ? -1L : 0L;//sign extend
        res->u._32[L] = tmp >> shift;
    }
}

void U64ShiftR( const unsigned_64 *a, unsigned shift, unsigned_64 *res )
{
    unsigned_32       save;

    shift &= 0x3f;
    if( shift < 32 ){
        save = a->u._32[H];
        save <<= 1;  // incase shift == 0
        res->u._32[H] = a->u._32[H] >> shift;
        res->u._32[L] = a->u._32[L] >> shift;
        res->u._32[L] |= save << (31-shift);
    }else{
        shift -= 32;
        save = a->u._32[H];
        res->u._32[H] = 0L;//vacate
        res->u._32[L] = save >> shift;
    }
}

void U64ShiftL( const unsigned_64 *a, unsigned shift, unsigned_64 *res )
{
    unsigned_32       save;

    shift &= 0x3f;
    if( shift < 32 ){
        save = a->u._32[L];
        save >>= 1;  // incase shift == 0
        res->u._32[L] = a->u._32[L] << shift;
        res->u._32[H] = a->u._32[H] << shift;
        res->u._32[H] |= save >> (31-shift);
    }else{
        shift -= 32;
        save = a->u._32[L];
        res->u._32[L] = 0L;//vacate
        res->u._32[H] = save << shift;
    }
}

void U64Shift( const unsigned_64 *a, int shift, unsigned_64 *res )
{
    if( shift < 0 ) {
        /* left shift */
        shift = -shift;
        U64ShiftL( a, shift, res );
    } else {
        /* right shift */
        U64ShiftR( a, shift, res );
    }
}

#ifdef _U64_C_ROUTINES
int U64Cnv10( unsigned_64 *res, char c )
{ //res = res*10+c res source/dest c value <= 9
    unsigned_64 tmp8;
    unsigned_64 tmp2;
    unsigned_32 save;
    int         over;

    tmp8 = *res;
    tmp2 = tmp8;
    over = 0;
    if( tmp8.u._32[H] & 0xE0000000 ){ // shift by 3 will overflow
        over = 1;
    }
    if( tmp8.u._32[H] & 0x80000000 ){ // shift by 1 will overflow
        over = 1;
    }
    save = tmp8.u._32[L];
    tmp8.u._32[L] <<= 3;
    tmp8.u._32[H] <<= 3;
    tmp8.u._32[H] |= save >> (32-3);
    save = tmp2.u._32[L];
    tmp2.u._32[L] <<= 1;
    tmp2.u._32[H] <<= 1;
    tmp2.u._32[H] |= save >> (32-1);
    save = tmp8.u._32[L] + tmp2.u._32[L];
    tmp2.u._32[L] = save;
    if( save < tmp8.u._32[L] ){ // add carry;
        ++tmp8.u._32[H];
        if( tmp8.u._32[H]  == 0 ){
            over = 1;
        }
    }
    save = tmp8.u._32[H] + tmp2.u._32[H];
    tmp2.u._32[H] = save;
    if( save < tmp8.u._32[H] ){
        over = 1;
    }
    save = tmp2.u._32[L] + c;
    tmp2.u._32[L] = save;
    if( save < (unsigned)c ){ // add carry;
        ++tmp2.u._32[H];
        if( tmp2.u._32[H]  == 0 ){
            over = 1;
        }
    }
    *res = tmp2;
    return( over );
}

int U64Cnv8( unsigned_64 *res, char c )
{ //res = res*8+c res source/dest c value <= 7
    unsigned_64 tmp8;
    unsigned_32 save;
    int         over;

    over = 0;
    tmp8 = *res;
    if( tmp8.u._32[H] & 0xE0000000 ){ // shift by 3 will overflow
        over = 1;
    }
    save = tmp8.u._32[L];
    tmp8.u._32[L] <<= 3;
    tmp8.u._32[H] <<= 3;
    tmp8.u._32[H] |= save >> (32-3);
    tmp8.u._32[L] |= c;
    *res = tmp8;
    return( over );
}

int U64Cnv16( unsigned_64 *res, char c )
{ //res = res*16+c res source/dest c value <= 15
    unsigned_64 tmp16;
    unsigned_32 save;
    int         over;

    over = 0;
    tmp16 = *res;
    if( tmp16.u._32[H] & 0xF0000000 ){ // shift by 4 will overflow
        over = 1;
    }
    save = tmp16.u._32[L];
    tmp16.u._32[L] <<= 4;
    tmp16.u._32[H] <<= 4;
    tmp16.u._32[H] |= save >> (32-4);
    tmp16.u._32[L] |= c;
    *res = tmp16;
    return( over );
}
#endif
