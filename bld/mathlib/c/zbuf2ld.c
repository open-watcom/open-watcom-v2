/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  String to double/long_double conversion.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "watcom.h"
#include "xfloat.h"


#if !defined(_LONG_DOUBLE_)
void __ZBuf2LD( buf_stk_ptr buf, ld_stk_ptr ld )
{
    int             i;
    int             n;
    long            high;
    long            low;
    const char      *s;

    s = (const char *)buf;
    n = strlen( s );
    high = 0;
    #define DWORD_MAX_DIGITS    9
    for( i = 0; i < DWORD_MAX_DIGITS; i++ ) {   // collect high 9 significant digits
        if( n <= DWORD_MAX_DIGITS )
            break;
        --n;
        high = high * 10 + (*s++ - '0');
    }
    low = 0;
    for( i = 0; i < DWORD_MAX_DIGITS; i++ ) {   // collect low 9 significant digits
        if( n == 0 )
            break;
        --n;
        low = low * 10 + (*s++ - '0');
    }
    #undef DWORD_MAX_DIGITS
    if( high == 0 && low == 0 ) {
        ld->u.value = 0.0;
    } else {
        ld->u.value = (double)high * 1e9 + (double)low;
    }
}
#endif

static uint_32 xdigit2bin( char chr )
{
    uint_32     value;

    value = chr - '0';
    if( value > 9 ) {
        value = chr - 'A' + 10;
        if( value > 15 ) {
            value = chr - 'a' + 10;
        }
    }
    return( value );
}

void __ZXBuf2LD( buf_stk_ptr buf, ld_stk_ptr ld, int *exponent )
{
    int             i;
    int             n;
    int_32          exp;
    uint_32         high;
    uint_32         low;
    const char      *s;

    s = (const char *)buf;
    n = strlen( s );
    exp = *exponent;
    high = 0;
    #define DWORD_MAX_DIGITS    8
    for( i = 0; i < DWORD_MAX_DIGITS; i++ ) {   /* collect high 8 significant hex digits */
        if( n == 0 )
            break;
        --n;
        high |= xdigit2bin( *s++ ) << (28 - i * 4);
        exp += 4;
    }
    low = 0;
    for( i = 0; i < DWORD_MAX_DIGITS; i++ ) {   /* collect low 8 significant hex digits */
        if( n == 0 )
            break;
        --n;
        low |= xdigit2bin( *s++ ) << (28 - i * 4);
        exp += 4;
    }
    #undef DWORD_MAX_DIGITS

    /* Flush significand to the left */
    while( !(high & 0x80000000) ) {
        high <<= 1;
        if( low & 0x80000000 )
            high |= 1;
        low <<= 1;
        --exp;
    }

    /* Perform final conversion to binary */
    --exp;
#ifdef _LONG_DOUBLE_
    ld->low_word  = low;
    ld->high_word = high;
    ld->exponent  = 0x3FFF + exp;                   /* bias is 16383 (0x3FFF) */
#else
    /* The msb is implied and not stored. Shift the significand left once more */
    high <<= 1;
    if( low & 0x80000000 )
        high |= 1;
    low <<= 1;

    ld->u.word[I64LO32]  = (high << (32 - 12)) | (low >> 12);
    ld->u.word[I64HI32]  = high >> 12;
    ld->u.word[I64HI32] |= (0x3FF + exp) << (32 - 12); /* bias is 1023 (0x3FF) */
#endif
    *exponent = exp;
}
