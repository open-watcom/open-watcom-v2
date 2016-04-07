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
* Description:  Conversion of floating-point values to strings.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "fltsupp.h"


#define NDIG            8

// this is defined in float.h
#if LDBL_MAX_10_EXP == 308
#undef LDBL_MAX_10_EXP
#define LDBL_MAX_10_EXP 4096
#else
#error LDBL_MAX_10_EXP has changed from 308
#endif

/* We must use more than DBL_DIG/LDBL_DIG, otherwise we lose precision! */
#define DBL_CVT_DIGITS      20
#define LDBL_CVT_DIGITS     23

// this manufactures a value from macros defined in float.h
#define __local_glue( __x, __y ) __x ## __y
#define local_glue( __x, __y ) __local_glue( __x, __y )
#define ONE_TO_DBL_MAX_10_EXP local_glue( 1e, DBL_MAX_10_EXP )
#define ONE_TO_DBL_MIN_10_EXP 1e307


#if defined( __386__ )
 char _WCNEAR *Fmt8Digits( unsigned long value, char *p );
 #pragma aux    Fmt8Digits = \
        "       push    ecx"\
        "       push    edx"\
        "       call    fmt8"\
        "       jmp     short L4"\
        "fmt8:  mov     ecx,10000"\
        "       sub     edx,edx"\
        "       cmp     eax,ecx"\
        "       xchg    eax,edx"\
        "       jb      short L1"\
        "       xchg    eax,edx"\
        "       div     ecx"\
        "L1:    push    edx"\
        "       call    fmt4"\
        "       pop     eax"\
        "fmt4:  mov     ecx,100"\
        "       sub     edx,edx"\
        "       cmp     eax,ecx"\
        "       xchg    eax,edx"\
        "       jb      short L2"\
        "       xchg    eax,edx"\
        "       div     cx"\
        "L2:    push    edx"\
        "       call    fmt2"\
        "       pop     eax"\
        "fmt2:  mov     cl,10"\
        "       cmp     al,cl"\
        "       xchg    al,ah"\
        "       jb      short L3"\
        "       xchg    al,ah"\
        "       div     cl"\
        "L3:    add     ah,'0'"\
        "       add     al,'0'"\
        "       mov     [ebx],al"\
        "       inc     ebx"\
        "       mov     [ebx],ah"\
        "       inc     ebx"\
        "       ret"\
        "L4:    pop     edx"\
        "       pop     ecx"\
        "       xor     al,al"\
        "       mov     [ebx],al"\
    parm caller [eax] [ebx] value [ebx];
#elif defined( _M_I86 )
 char _WCNEAR *Fmt8Digits( unsigned long value, char *p );
 #pragma aux    Fmt8Digits = \
        "       push    cx"\
        "       call    fmt8"\
        "       jmp     short L4"\
        "fmt8:  mov     cx,10000"\
        "       div     cx"\
        "       push    dx"\
        "       call    fmt4"\
        "       pop     ax"\
        "fmt4:  mov     cx,100"\
        "       sub     dx,dx"\
        "       cmp     ax,cx"\
        "       xchg    ax,dx"\
        "       jb      short L2"\
        "       xchg    ax,dx"\
        "       div     cx"\
        "L2:    push    dx"\
        "       call    fmt2"\
        "       pop     ax"\
        "fmt2:  mov     cl,10"\
        "       cmp     al,cl"\
        "       xchg    al,ah"\
        "       jb      short L3"\
        "       xchg    al,ah"\
        "       div     cl"\
        "L3:    add     ax,3030h"\
        "       mov     ss:[bx],ax"\
        "       inc     bx"\
        "       inc     bx"\
        "       ret"\
        "L4:    pop     cx"\
        "       xor     al,al"\
        "       mov     ss:[bx],al"\
    parm caller [dx ax] [bx] value [bx];
#else
static unsigned long IntPow10[] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
};

static char _WCNEAR *Fmt8Digits( unsigned long value, char *p )
{
    int                 i;
    int                 digit;
    unsigned long       pow10;

    for( i = NDIG - 1; i != 0; i-- ) {
        digit = '0';
        pow10 = IntPow10[i];
        while( value >= pow10 ) {
            value -= pow10;
            ++digit;
        }
        *p++ = digit;
    }
    *p++ = value + '0';
    *p = '\0';
    return( p );
}
#endif

#ifdef _LONG_DOUBLE_
// Intel supports long double
#define E8_EXP          0x4019
#define E8_HIGH         0xBEBC2000
#define E8_LOW          0x00000000
#define E16_EXP         0x4034
#define E16_HIGH        0x8E1BC9BF
#define E16_LOW         0x04000000

static long_double LDPowTable[] = {
    { 0x00000000, 0xA0000000, 0x4002 }, // 1e1L
    { 0x00000000, 0xC8000000, 0x4005 }, // 1e2L
    { 0x00000000, 0x9C400000, 0x400C }, // 1e4L
    { 0x00000000, 0xBEBC2000, 0x4019 }, // 1e8L
    { 0x04000000, 0x8E1BC9BF, 0x4034 }, // 1e16L
    { 0x2B70B59E, 0x9DC5ADA8, 0x4069 }, // 1e32L
    { 0xFFCFA6D5, 0xC2781F49, 0x40D3 }, // 1e64L
    { 0x80E98CE0, 0x93BA47C9, 0x41A8 }, // 1e128L
    { 0x9DF9DE8E, 0xAA7EEBFB, 0x4351 }, // 1e256L
    { 0xA60E91C7, 0xE319A0AE, 0x46A3 }, // 1e512L
    { 0x81750C17, 0xC9767586, 0x4D48 }, // 1e1024L
    { 0xC53D5DE5, 0x9E8b3B5D, 0x5A92 }, // 1e2048L
    { 0x8A20979B, 0xC4605202, 0x7525 }, // 1e4096L
    { 0x00000000, 0x80000000, 0x7FFF }, // infinity
};

static void CalcScaleFactor( ld_arg factor, int n )
{
    long_double *pow;
    long_double tmp;

    if( n >= 8192 ) {
        n = 8192;               // set to infinity multiplier
    }
    for( pow = LDPowTable; n > 0; n >>= 1, ++pow ) {
        if( n & 1 ) {
            tmp.exponent  = pow->exponent;
            tmp.high_word = pow->high_word;
            tmp.low_word  = pow->low_word;
            __FLDM( factor, &tmp, factor );
        }
    }
}

static void _do_LDScale10x( ld_arg ld, int scale )
{
    long_double factor;

    if( scale != 0 ) {
#if defined( _LONG_DOUBLE_ ) && defined( __FPI__ )
        unsigned short _8087cw = __Get87CW();
        __Set87CW( _8087cw | _PC_64 ); // make sure extended precision is on
#endif
        factor.exponent  = 0x3FFF;              // set factor = 1.0
        factor.high_word = 0x80000000;
        factor.low_word  = 0x00000000;
        if( scale < 0 ) {
            CalcScaleFactor( &factor, -scale );
            __FLDD( ld, &factor, ld );
        } else {
            CalcScaleFactor( &factor, scale );
            __FLDM( ld, &factor, ld );
        }
#if defined( _LONG_DOUBLE_ ) && defined( __FPI__ )
        __Set87CW( _8087cw );       // restore control word
#endif
    }
}

void _LDScale10x( ld_arg ld, int scale )
{
    if( scale > LDBL_MAX_10_EXP ) {
        _do_LDScale10x( ld, LDBL_MAX_10_EXP );
        scale -= LDBL_MAX_10_EXP;
    } else if( scale < -LDBL_MAX_10_EXP ) {
        _do_LDScale10x( ld, -LDBL_MAX_10_EXP );
        scale += LDBL_MAX_10_EXP;
    }
    _do_LDScale10x( ld, scale );
}

#else           /* 'long double' is same as 'double' */

static double Pow10Table[] = {
    1e1, 1e2, 1e4, 1e8, 1e16, 1e32, 1e64, 1e128, 1e256,
};

void _LDScale10x( ld_arg ld, int scale )
{
    double      factor;
    double      *pow;
    int         n;

    if( scale != 0 ) {
        n = scale;
        if( scale < 0 ) n = -n;
        if( n > DBL_MAX_10_EXP ) {
            if( scale < 0 ) {
                ld->value /= ONE_TO_DBL_MIN_10_EXP;
                n += DBL_MIN_10_EXP;
            } else {
                ld->value *= ONE_TO_DBL_MAX_10_EXP;
                n -= DBL_MAX_10_EXP;
            }
        }
        factor = 1.0;
        for( pow = Pow10Table; n > 0; n >>= 1, ++pow ) {
            if( n & 1 ) {
                factor *= *pow;
            }
        }
        if( scale < 0 ) {
            ld->value /= factor;
        } else {
            ld->value *= factor;
        }
    }
}

#endif

static void DoFFormat( CVT_INFO *cvt, char *p, int nsig, int xexp, char *buf )
{
    int         i;
    int         ndigits;

    ndigits = cvt->ndigits;
    ++xexp;
    i = 0;
    if( cvt->flags & G_FMT ) {
        if( nsig < ndigits && !(cvt->flags & F_DOT) ) {
            ndigits = nsig;
        }
        ndigits -= xexp;
        if( ndigits < 0 ) {
            ndigits = 0;
        }
    }
    if( xexp <= 0 ) {   // digits only to right of '.'
        if( !(cvt->flags & F_CVT) ) {
            buf[i++] = '0';
            if( ndigits > 0 || (cvt->flags & F_DOT) ) {
                buf[i++] = '.';
            }
        }
        cvt->n1 = i;
        if( ndigits < -xexp ) {
            xexp = - ndigits;
        }
        cvt->decimal_place = xexp;
        cvt->nz1 = -xexp;
//      for( n = -xexp; n > 0; --n ) buf[i++] = '0';
        ndigits += xexp;
        if( ndigits < nsig ) {
            nsig = ndigits;
        }
        memcpy( &buf[i], p, nsig );
        i += nsig;
        cvt->n2 = nsig;
        cvt->nz2 = ndigits - nsig;
//      for( n = ndigits - nsig; n > 0; --n ) buf[i++] = '0';
    } else if( nsig < xexp ) {  // zeros before '.'
        memcpy( buf, p, nsig );
        i += nsig;
        cvt->n1 = nsig;
        cvt->nz1 = xexp - nsig;
        cvt->decimal_place = xexp;
//      for( n = xexp - nsig; n > 0; --n ) buf[i++] = '0';
        if( !(cvt->flags & F_CVT) ) {
            if( ndigits > 0 || (cvt->flags & F_DOT) ) {
                buf[i++] = '.';
                cvt->n2 = 1;
            }
        }
        cvt->nz2 = ndigits;
//      for( n = ndigits; n > 0; --n ) buf[i++] = '0';
    } else {                    // enough digits before '.'
        memcpy( buf, p, xexp );
        cvt->decimal_place = xexp;
        i += xexp;
        nsig -= xexp;
        if( !(cvt->flags & F_CVT) ) {
            if( ndigits > 0 || (cvt->flags & F_DOT) ) {
                buf[i++] = '.';
            }
        } else if( buf[0] == '0' ) {    // ecvt or fcvt with 0.0
            cvt->decimal_place = 0;
        }
        if( ndigits < nsig ) {
            nsig = ndigits;
        }
        memcpy( &buf[i], p + xexp, nsig );
        i += nsig;
        cvt->n1 = i;
        cvt->nz1 = ndigits - nsig;
//      for( n = ndigits - nsig; n > 0; --n ) buf[i++] = '0';
    }
    buf[i] = '\0';
}

static void DoEFormat( CVT_INFO *cvt, char *p, int nsig, int xexp, char *buf )
{
    int         i;
    int         n;
    int         ndigits;        // number of digits after decimal place
    int         width;

    ndigits = cvt->ndigits;
    if( cvt->scale <= 0 ) {
        ndigits += cvt->scale;  // decrease number of digits after decimal
    } else {
        ndigits -= cvt->scale;  // adjust number of digits (see fortran spec)
        ndigits++;
    }
    i = 0;
    if( cvt->flags & G_FMT ) {  // fixup for 'G'
        // for 'G' format, ndigits is the number of significant digits
        // cvt->scale should be 1 indicating 1 digit before decimal place
        // so decrement ndigits to get number of digits after decimal place
/* JBS 25-may-98  - changed to model what DoFFormat did */
//      if( nsig < ndigits )  ndigits = nsig;
        if( nsig < ndigits && !(cvt->flags & F_DOT) ) {
            ndigits = nsig;
        }
        --ndigits;
        if( ndigits < 0 ) {
            ndigits = 0;
        }
    }
    if( cvt->scale <= 0 ) {
        buf[i++] = '0';
    } else {
        n = cvt->scale;
        if( n > nsig ) {
            n = nsig;
        }
        memcpy( &buf[i], p, n );        // put in leading digits
        i += n;
        p += n;
        nsig -= n;
        if( n < cvt->scale ) {          // put in zeros if required
            n = cvt->scale - n;
            memset( &buf[i], '0', n );
            i += n;
        }
    }
    cvt->decimal_place = i;
    if( !(cvt->flags & F_CVT) ) {
        if( ndigits > 0 || (cvt->flags & F_DOT) ) {
            buf[i++] = '.';
        }
    }
    if( cvt->scale < 0 ) {
        n = - cvt->scale;
        memset( &buf[i], '0', n );
        i += n;
    }
    if( ndigits > 0 ) {                 // put in fraction digits
        if( ndigits < nsig ) {
            nsig = ndigits;
        }
        if( nsig != 0 ) {
            memcpy( &buf[i], p, nsig );
            i += nsig;
        }
        cvt->n1 = i;
        cvt->nz1 = ndigits - nsig;
//      for( n = cvt->ndigits - nsig; n > 0; --n ) buf[i++] = '0';
    }
    if( cvt->expchar != '\0' ) {
        buf[i++] = cvt->expchar;
    }
    if( xexp >= 0 ) {
        buf[i++] = '+';
    } else {
        buf[i++] = '-';
        xexp = - xexp;
    }
    width = cvt->expwidth;
    switch( width ) {
    case 0:                             // width unspecified
        if( xexp >= 1000 ) {
            width = 4;
        } else {
            width = 3;
        }
        break;
    case 1:
        if( xexp >= 10 ) {
            width = 2;
        }
    case 2:
        if( xexp >= 100 ) {
            width = 3;
        }
    case 3:
        if( xexp >= 1000 ) {
            width = 4;
        }
    }
    cvt->expwidth = width;              // pass back width actually used
    if( width >= 4 ) {
        n = 0;
        if( xexp >= 1000 ) {
            n = xexp / 1000;
            xexp -= n * 1000;
        }
        buf[i++] = n + '0';
    }
    if( width >= 3 ) {
        n = 0;
        if( xexp >= 100 ) {
            n = xexp / 100;
            xexp -= n * 100;
        }
        buf[i++] = n + '0';
    }
    if( width >= 2 ) {
        n = 0;
        if( xexp >= 10 ) {
            n = xexp / 10;
            xexp -= n * 10;
        }
        buf[i++] = n + '0';
    }
    buf[i++] = xexp + '0';
    cvt->n2 = i - cvt->n1;
    buf[i] = '\0';
}

#define STK_BUF_SIZE    64              // size of stack buffer required
                                        // if long double and NO_TRUNC is on.

/* NB: Just like _EFG_Format(), the following assumes ASCII character  encoding */

FLTSUPPFUNC void __cvtld( long_double *pld, CVT_INFO *cvt, char *buf )
{
    int         i;
    int         n;
    int         nsig;
    int         xexp;
    char        *p;
    char        drop;
    long        value;
    long_double ld;
    auto char   stkbuf[STK_BUF_SIZE];
    int         maxsize;
#if defined( _LONG_DOUBLE_ ) && defined( __FPI__ )
    unsigned short _8087cw = __Get87CW();
    __Set87CW( _8087cw | _PC_64 );  // make sure extended precision is used
#endif

    cvt->sign = 0;
#ifdef _LONG_DOUBLE_
    ld.exponent  = pld->exponent;
    ld.high_word = pld->high_word;
    ld.low_word  = pld->low_word;
    if( ld.exponent & 0x8000 ) {
        cvt->sign = -1;
    }
    ld.exponent &= 0x7FFF;          // make number positive
#else
    ld.value = pld->value;
    if( ld.word[1] & 0x80000000 ) {
        cvt->sign = -1;
    }
    ld.word[1] &= 0x7FFFFFFF;               // make number positive
#endif
    cvt->n1  = 0;
    cvt->nz1 = 0;
    cvt->n2  = 0;
    cvt->nz2 = 0;
    cvt->decimal_place = 0;
    value = 0;
    switch( __LDClass( &ld ) ) {
    case __ZERO:
    case __DENORMAL:
        cvt->sign = 0;                  // force sign to +0.0
        xexp = 0;
        break;
    case __NAN:
        buf[0] = 'n'; buf[1] = 'a'; buf[2] = 'n'; buf[3] = '\0';
        goto nan_inf;
    case __INFINITY:
        buf[0] = 'i'; buf[1] = 'n'; buf[2] = 'f'; buf[3] = '\0';
nan_inf:
        if( cvt->flags & IN_CAPS ) {
            unsigned long _WCUNALIGNED  *text;

            /* Uppercase entire four-char ASCII string in one go */
            text = (unsigned long *)buf;
            *text &= ~0x20202020;
        }
        cvt->flags |= IS_INF_NAN;   /* may need special handling */
        cvt->n1 = 3;
        goto end_cvt;
    case __NONZERO:
        // what if number is denormal?  should normalize it
/*
    Estimate the position of the decimal point by estimating 1 + log10(x).
    Compute approximate value of log10(x) by multiplying the exponent
    by 30103 and dividing by 100000, since log10(x) = log2(x) * log10(2)
    where log10(2) = .30103 approximately.
*/
#ifdef _LONG_DOUBLE_
        xexp = ld.exponent - 0x3FFE;
#else
        xexp = (ld.word[1] >> 20) - 0x3FE;
#endif
        xexp = xexp * 30103L / 100000L;
        xexp -= NDIG / 2;
        if( xexp != 0 ) {
            if( xexp < 0 ) {                    // must scale up
                xexp = - ((-xexp + (NDIG / 2 - 1)) & ~(NDIG / 2 - 1));
                _LDScale10x( &ld, -xexp );
            } else /*if( xexp > 0 )*/ {         // must scale down
#ifdef _LONG_DOUBLE_
                if( ld.exponent < E8_EXP ||
                   (ld.exponent == E8_EXP && ld.high_word < E8_HIGH) ) {
                    // number is < 1e8
                    xexp = 0;
                } else if( ld.exponent < E16_EXP ||
                   ((ld.exponent == E16_EXP &&
                   (ld.high_word <  E16_HIGH ||
                   (ld.high_word == E16_HIGH && ld.low_word < E16_LOW)))) ) {
                    // number is < 1e16
                    long_double tmp;
                    long_double tmp2;

                    tmp.exponent  = E8_EXP;             // tmp = 1e8L
                    tmp.high_word = E8_HIGH;
                    tmp.low_word  = E8_LOW;
                    __FLDD( &ld, &tmp, &tmp2 );
                    value = __LDI4( &tmp2 );
                    __I4LD( value, &tmp2 );
                    __FLDM( &tmp2, &tmp, &tmp );
                    __FLDS( &ld, &tmp, &ld );
                    xexp = 8;
#else
                if( ld.value < 1e8 ) {
                    xexp = 0;
                } else if( ld.value < 1e16 ) {
                    value = (long)(ld.value / 1e8);
                    ld.value -= (double)value * 1e8;
                    xexp = 8;
#endif
                } else {                // scale number down
                    xexp &= ~(NDIG / 2 - 1);
                    _LDScale10x( &ld, -xexp );
                }
            }
        }
        break;
    }
    if( cvt->flags & F_FMT ) {
        n = cvt->ndigits + xexp + 2 + NDIG;
        if( cvt->scale > 0 ) {
            n += cvt->scale;
        }
    } else {
        n = cvt->ndigits + 4 + NDIG / 2;    // need at least this for rounding
    }

    maxsize = DBL_CVT_DIGITS;
#ifdef _LONG_DOUBLE_
    if( cvt->flags & LONG_DOUBLE ) {        // number is long double
        maxsize = LDBL_CVT_DIGITS;
    }
#endif
    if( cvt->flags & NO_TRUNC ) {
        maxsize *= 2;
    }
    maxsize += (NDIG / 2);
    if( n > maxsize ) {
        n = maxsize;
    }

    // convert ld into string of digits
    // put in leading '0' in case we round 99...99 to 100...00
    stkbuf[0] = '0';
    stkbuf[1] = '\0';
    p = &stkbuf[1];
    i = 0;
    while( n > 0 ) {
        n -= NDIG;
        if( value == 0 ) {
#ifdef _LONG_DOUBLE_
            if( (ld.exponent & 0x7FFF) == 0 )
                break;
            value = __LDI4( &ld );
            if( n > 0 ) {
                long_double     tmp;

                __I4LD( value, &tmp );
                __FLDS( &ld, &tmp, &ld );
                tmp.exponent  = E8_EXP;         // tmp = 1e8L
                tmp.high_word = E8_HIGH;
                tmp.low_word  = E8_LOW;
                __FLDM( &ld, &tmp, &ld );
            }
#else
            if( (ld.word[1] & 0x7FF00000) == 0 )
                break;
            value = ld.value;
            if( n > 0 ) {
                ld.value = (ld.value - (double)value) * 1e8;
            }
#endif
        }
        p = Fmt8Digits( value, p );
        i += 8;
        value = 0;
    }
    n = i;                              // get number of characters in buf
    p = &stkbuf[1];
    xexp += NDIG - 1;
    while( *p == '0' ) {                // skip over leading zeros
        --n;
        --xexp;
        ++p;
    }
    nsig = cvt->ndigits;
    if( cvt->flags & F_FMT ) {
        xexp += cvt->scale;
        nsig += xexp + 1;
    } else if( cvt->flags & E_FMT ) {
        if( cvt->scale > 0 ) {
            ++nsig;
        } else {
            nsig += cvt->scale;
        }
        xexp = xexp + 1 - cvt->scale;
    }
    if( nsig >= 0 ) {           // round and strip trailing zeros
        if( nsig > n ) {
            nsig = n;
        }

        maxsize = DBL_CVT_DIGITS;
#ifdef _LONG_DOUBLE_
        if( cvt->flags & LONG_DOUBLE ) {    // number is long double
            maxsize = LDBL_CVT_DIGITS;
        }
#endif
        if( cvt->flags & NO_TRUNC ) {
            maxsize *= 2;
        }
        if( nsig > maxsize ) {
            nsig = maxsize;
        }

        drop = '0';
        if( n > nsig && p[nsig] >= '5' ) {
            drop = '9';
        }
        i = nsig;
        while( p[--i] == drop ) {
            --nsig;
        }
        if( drop == '9' ) {
            ++p[i];             // round up
        }
        if( i < 0 ) {           // repeating 9's rounded up to 10000...
            --p;
            ++nsig;
            ++xexp;
        }
    }
    if( nsig <= 0 ) {
        nsig = 1;
        xexp = 0;                               // 21-apr-95
        stkbuf[0] = '0';
        cvt->sign = 0;
        p = stkbuf;
    }
    if( (cvt->flags & F_FMT) || ((cvt->flags & G_FMT) &&
        ((xexp >= -4 && xexp < cvt->ndigits) || (cvt->flags & F_CVT)) ) ) {
        DoFFormat( cvt, p, nsig, xexp, buf );
    } else {
        DoEFormat( cvt, p, nsig, xexp, buf );
    }
end_cvt:;
#if defined( _LONG_DOUBLE_ ) && defined( __FPI__ )
    __Set87CW( _8087cw );               // restore old control word
#endif
}

#if defined( __MAKE_DLL_MATHLIB )
_type_EFG_cvtld *__get_EFG_cvtld( void )
{
    return( __cvtld );
}
#endif

_WMRTLINK void __LDcvt( long_double *pld, CVT_INFO *cvt, char *buf )
{
    __cvtld( pld, cvt, buf );
}
