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
* Description:  Conversion from string to tbyte format (long double)
*                it doesn't depend on processor/math coprocessor type
*
****************************************************************************/

/*
    It use 96bit x 96bit -> 192bit multiply for converting decimal to
       80-bit extended double.
    Math operation is on 96-bit wide operands (mantisa).
*/

#include <string.h>
#include <ctype.h>
#include "bool.h"
#include "tbyte.h"
#include "watcom.h"

typedef unsigned __int64 u64;
typedef unsigned_32 u32;

typedef union {
    u64 m64[3];
    u32 m32[6];
} u192;

typedef union {
    u32 m32[3];
} u96;

typedef struct {
    u32 m32[3];
    unsigned short e;
} ELD;

#define EXPONENT_BIAS 0x3FFF

#define SET_SIGN(x,y) if( y < 0 ) x->e |= 0x8000
#define U96ISNOTZERO(x) (x.m32[0] || x.m32[1] || x.m32[2])

#define MAX_EXP_INDEX 13

static ELD tab_plus_exp[MAX_EXP_INDEX] = {
    { { 0x00000000UL, 0x00000000UL, 0xA0000000UL }, 0x4002 }, // 1e1L
    { { 0x00000000UL, 0x00000000UL, 0xC8000000UL }, 0x4005 }, // 1e2L
    { { 0x00000000UL, 0x00000000UL, 0x9C400000UL }, 0x400C }, // 1e4L
    { { 0x00000000UL, 0x00000000UL, 0xBEBC2000UL }, 0x4019 }, // 1e8L
    { { 0x00000000UL, 0x04000000UL, 0x8E1BC9BFUL }, 0x4034 }, // 1e16L
    { { 0xF0200000UL, 0x2B70B59DUL, 0x9DC5ADA8UL }, 0x4069 }, // 1e32L
    { { 0x3CBF6B71UL, 0xFFCFA6D5UL, 0xC2781F49UL }, 0x40D3 }, // 1e64L
    { { 0xC66F336BUL, 0x80E98CDFUL, 0x93BA47C9UL }, 0x41A8 }, // 1e128L
    { { 0xDDBB9018UL, 0x9DF9DE8DUL, 0xAA7EEBFBUL }, 0x4351 }, // 1e256L
    { { 0xCC655C4BUL, 0xA60E91C6UL, 0xE319A0AEUL }, 0x46A3 }, // 1e512L
    { { 0x650D3D17UL, 0x81750C17UL, 0xC9767586UL }, 0x4D48 }, // 1e1024L
    { { 0xA74D28B1UL, 0xC53D5DE4UL, 0x9E8b3B5DUL }, 0x5A92 }, // 1e2048L
    { { 0xC94C14F7UL, 0x8A20979AUL, 0xC4605202UL }, 0x7525 }  // 1e4096L
};

static ELD tab_minus_exp[MAX_EXP_INDEX] = {
    { { 0xCCCCCCCDUL, 0xCCCCCCCCUL, 0xCCCCCCCCUL }, 0x3FFB }, // 1e-1L
    { { 0x3D70A3D7UL, 0x70A3D70AUL, 0xA3D70A3DUL }, 0x3FF8 }, // 1e-2L
    { { 0xD3C36113UL, 0xE219652BUL, 0xD1B71758UL }, 0x3FF1 }, // 1e-4L
    { { 0xFDC20D2AUL, 0x8461CEFCUL, 0xABCC7711UL }, 0x3FE4 }, // 1e-8L
    { { 0x4C2EBE65UL, 0xC44DE15BUL, 0xE69594BEUL }, 0x3FC9 }, // 1e-16L
    { { 0x67DE18E7UL, 0x453994BAUL, 0xCFB11EADUL }, 0x3F94 }, // 1e-32L
    { { 0x3F2398CCUL, 0xA539E9A5UL, 0xA87FEA27UL }, 0x3F2A }, // 1e-64L
    { { 0xAC7CB3D9UL, 0x64BCE4A0UL, 0xDDD0467CUL }, 0x3E55 }, // 1e-128L
    { { 0xFA911122UL, 0x637A1939UL, 0xC0314325UL }, 0x3CAC }, // 1e-256L
    { { 0x7132D2E4UL, 0xDB23D21CUL, 0x9049EE32UL }, 0x395A }, // 1e-512L
    { { 0x87A600A6UL, 0xDA57C0BDUL, 0xA2A682A5UL }, 0x32B5 }, // 1e-1024L
    { { 0x4925110FUL, 0x34362DE4UL, 0xCEAE534FUL }, 0x256B }, // 1e-2048L
    { { 0x2DE37E46UL, 0xD2CE9FDEUL, 0xA6DD04C8UL }, 0x0AD8 }  // 1e-4096L
};

static int cmp_u96_max( u96 *x )
/**********************************************
    compare u96 with maximum value before u96
    overflow after multiply by 10
*/
{
    if( x->m32[2] > 0x19999999UL ) {
        return 1;
    } else if( x->m32[2] < 0x19999999UL ) {
        return -1;
    } else if( x->m32[1] > 0x99999999UL ) {
        return 1;
    } else if( x->m32[1] < 0x99999999UL ) {
        return -1;
    } else if( x->m32[0] > 0x99999998UL ) {
        return 1;
    } else if( x->m32[0] < 0x99999998UL ) {
        return -1;
    } else {
        return 0;
    }
}

static int add_check_u96_overflow( u96 *x, unsigned int c)
/**********************************************
    test u96 overflow after multiply by 10
    add one decimal digit to u96
*/
{
    u64 cy;
    int i;

    if( cmp_u96_max(x) > 0 ) {
        return 1;
    } else {
        cy = c;
        for( i = 0; i < 3; i++ ) {
            cy += (u64)x->m32[i] * 10;
            x->m32[i] = (u32)cy;
            cy >>= 32;
        }
        return 0;
    }
}

static int bitsize32(u32 x)
/**********************************************
    calculate bitsize for u32
*/
{
    int i;

    for( i = 32; i > 0 ; i-- ) {
        if( x & 0x80000000U ) break;
        x <<= 1;
    }
    return i;
}

static int bitsize64(u64 x)
/**********************************************
    calculate bitsize for u64
*/
{
    int i;

    for( i = 64; i > 0 ; i-- ) {
        if( x & 0x8000000000000000ULL ) break;
        x <<= 1;
    }
    return i;
}

static int U96LD(u96 *op, ELD *res)
/**************************************************
    convert u96 into internal extended long double
*/
{
    int bs;
    int shft;

    memcpy(res, op, sizeof(u96));
    bs = bitsize32(res->m32[2]) + 64;
    if( bs == 64 ) {
        res->m32[2] = res->m32[1];
        res->m32[1] = res->m32[0];
        res->m32[0] = 0;
        bs = bitsize32(res->m32[2]) + 32;
    }
    if( bs == 32 ) {
        res->m32[2] = res->m32[1];
        res->m32[1] = res->m32[0];
        res->m32[0] = 0;
        bs = bitsize32(res->m32[2]);
    }
    if( bs == 0 ) {
        res->e = 0;
    } else {
        res->e = (unsigned short)( bs - 1 + EXPONENT_BIAS );
        bs %= 32;
        if( bs ) {
            shft = 32 - bs;
            res->m32[2] <<= shft;
            res->m32[2] |= res->m32[1] >> bs;
            res->m32[1] <<= shft;
            res->m32[1] |= res->m32[0] >> bs;
            res->m32[0] <<= shft;
        }
    }
    return 0;
}

static int normalize(u192 *res)
/**************************************************
    normalize internal extended long double u192
    return exponent shift
*/
{
    int shft;
    int bs;
    int bs1;

    bs = bitsize64(res->m64[2]) + 128;
    if( bs == 128 ) {
        res->m64[2] = res->m64[1];
        res->m64[1] = res->m64[0];
        res->m64[0] = 0;
        bs = bitsize64(res->m64[2]) + 64;
    }
    if( bs == 64 ) {
        res->m64[2] = res->m64[1];
        res->m64[1] = res->m64[0];
        res->m64[0] = 0;
        bs = bitsize64(res->m64[2]);
    }
    if( bs == 0 ) {
        return 0;
    }
    bs1 = bs % 64;
    if (bs1) {
        shft = 64 - bs1;
        res->m64[2] <<= shft;
        res->m64[2] |= res->m64[1] >> bs1;
        res->m64[1] <<= shft;
        res->m64[1] |= res->m64[0] >> bs1;
        res->m64[0] <<= shft;
    }
    return bs - 192;
}

static int add192(u192 *res, u64 x, int pos)
/**************************************************
    add u64 to u192 on u32 position
*/
{
    u64 cy;
    int i;

    cy = (u32)x;
    for( i = pos; i < 6; i++ ) {
        cy += res->m32[i];
        res->m32[i] = (u32)cy;
        cy >>= 32;
    }
    cy = x >> 32;
    for( i = pos + 1; i < 6; i++ ) {
        cy += res->m32[i];
        res->m32[i] = (u32)cy;
        cy >>= 32;
    }
    return 0;
}

static int multiply(ELD *op1, ELD *op2, ELD *res)
/**************************************************
    multiply u96 by u96 into u96
    normalize and round result
*/
{
    u64 x1;
    u192 r1;
    long exp;

    exp = (long)(op1->e & 0x7fff) + (long)(op2->e & 0x7fff) - EXPONENT_BIAS + 1;
    r1.m64[0] = (u64)(op1->m32[0]) * (u64)(op2->m32[0]);
    r1.m64[1] = (u64)(op1->m32[1]) * (u64)(op2->m32[1]);
    r1.m64[2] = (u64)(op1->m32[2]) * (u64)(op2->m32[2]);
    x1 = (u64)(op1->m32[1]) * (u64)(op2->m32[0]);
    add192(&r1, x1, 1);
    x1 = (u64)(op1->m32[0]) * (u64)(op2->m32[1]);
    add192(&r1, x1, 1);
    x1 = (u64)(op1->m32[0]) * (u64)(op2->m32[2]);
    add192(&r1, x1, 2);
    x1 = (u64)(op1->m32[2]) * (u64)(op2->m32[0]);
    add192(&r1, x1, 2);
    x1 = (u64)(op1->m32[1]) * (u64)(op2->m32[2]);
    add192(&r1, x1, 3);
    x1 = (u64)(op1->m32[2]) * (u64)(op2->m32[1]);
    add192(&r1, x1, 3);
    exp += normalize(&r1);
    // round result
    if( r1.m32[2] & 0x80000000U ) {
        if( r1.m32[5] == 0xffffffffU && r1.m32[4] == 0xffffffffU && r1.m32[3] == 0xffffffffU ) {
            r1.m32[3] = 0;
            r1.m32[4] = 0;
            r1.m32[5] = 0x80000000U;
            exp++;
        } else {
            x1 = 1L;
            add192(&r1, x1, 3);
        }
    }
    res->m32[0] = r1.m32[3];
    res->m32[1] = r1.m32[4];
    res->m32[2] = r1.m32[5];
    res->e = (unsigned short)exp;
    return 0;
}

static int TB_create(u96 *value, long exponent, TB_LD *ld)
/**************************************************
    create tbyte/long double from u96 value and
    decimal exponent, round result
*/
{
    ELD *tabExp;
    int i;
    ELD res;

    if( exponent < 0 ) {
        exponent = -exponent;
        tabExp = tab_minus_exp;
    } else {
        tabExp = tab_plus_exp;
    }
    U96LD(value, &res);
    for( i = 0; i < MAX_EXP_INDEX; i++) {
        if ( exponent & 1 ) {
            multiply(&res, tabExp + i, &res);
        }
        exponent >>= 1;
        if( exponent == 0 ) break;
    }
    if( exponent != 0 ) {
        // exponent overflow
    }
    ld->e = res.e;
    ld->m = res.m32[1] + ((u64)res.m32[2] << 32) ;
    // round result
    if(res.m32[0] & 0x80000000U) {
        if( ld->m == 0xffffffffffffffffULL ) {
            ld->m = 0x8000000000000000ULL;
            ld->e++;
        } else {
            ld->m++;
        }
    }
    return 0;
}

TB_LD * strtotb(char *p, TB_LD * ld, bool negative)
/**************************************************
    convert string into tbyte/long double
    set result sign
*/
{
    int              sign = +1;
    int              exp_sign = +1;
    long             exp_value;
    int              overflow;
    long             exp1;
    long             exponent;
    long             exponent_tmp;
    u96              value;
    u96              value_tmp;

    while ( isspace(*p) )
        p++;
    switch (*p) {
    case '-':
        sign = -1;
    case '+':
        p++;
    default :
        break;
    }
    if( negative ) {
        sign = (sign > 0) ? -1 : +1;
    }
    memset(&value, 0, sizeof(value));
    memset(&value_tmp, 0, sizeof(value_tmp));
    exponent = 0;
    exp1 = 0;
    exponent_tmp = 0;
    overflow = 0;
    while ( (unsigned int)(*p - '0') < 10u ) {
        if( overflow ) {
            exponent_tmp++;
            exp1++;
        } else {
            if( add_check_u96_overflow(&value_tmp, *p - '0') ) {
                overflow = 1;
                exponent_tmp++;
                exp1++;
            } else if( *p != '0' ) {
                memcpy(&value, &value_tmp, sizeof(value));
                exp1 = 0;
            } else if( U96ISNOTZERO(value) ) {
                exp1++;
            }
        }
        p++;
    }
    exponent = exp1;
    if ( *p == '.' ) {
        p++;
        while ( (unsigned int)(*p - '0') < 10u ) {
            if( overflow == 0 ) {
                if( add_check_u96_overflow(&value_tmp, *p - '0') ) {
                    overflow = 1;
                } else {
                    exponent_tmp--;
                    if( *p != '0' ) {
                        memcpy(&value, &value_tmp, sizeof(value));
                        exponent = exponent_tmp;
                    }
                }
            }
            p++;
        }
    }
    exp_value   = 0;
    if ( (*p | 0x20) == 'e' ) {
        switch (*++p) {
        case '-':
            exp_sign = -1;
        case '+': p++;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            break;
        default :
            ld->m = 0;
            ld->e = 0;
            SET_SIGN(ld, sign);
            return ld;
        }
        while ( (unsigned int)(*p - '0') < 10u )
            exp_value = 10 * exp_value + (*p++ - '0');
        if( exp_sign < 0 ) {
            exp_value = -exp_value;
        }
    }
    exp_value += exponent;
    TB_create(&value, exp_value, ld);
    SET_SIGN(ld, sign);
    return ld;
}
