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
#include "widechar.h"
#include "libsupp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <wchar.h>
#include "xfloat.h"

#define NEGATIVE_NUMBER         0x01
#define NEGATIVE_EXPONENT       0x02
#define DIGITS_PRESENT          0x04
#define DOT_FOUND               0x08

enum {
    _ZERO       = 0,
    _NONZERO,
    _UNDERFLOW,
    _OVERFLOW
};

#ifdef _LONG_DOUBLE_
 #define MAX_DIGITS     19
#else
 #define MAX_DIGITS     18
#endif

#if !(defined(_LONG_DOUBLE_) || defined(__WIDECHAR__))
void __ZBuf2LD( char _WCNEAR *buf, long_double _WCNEAR *ld )
{
    int         i;
    int         n;
    long        high;
    long        low;
    CHAR_TYPE   *ptr;

    ptr = (CHAR_TYPE *)buf;
    n = __F_NAME(strlen,wcslen)( ptr );
    high = 0;
    for( i = 0; i <= 8; i++ ) {         // collect high 9 significant digits
        if( n <= 9 ) break;
        --n;
        high = high * 10 + (*ptr++ - '0');
    }
    low = 0;
    for( i = 0; i <= 8; i++ ) {         // collect low 9 significant digits
        if( n == 0 ) break;
        --n;
        low = low * 10 + (*ptr++ - '0');
    }
    if( high == 0 && low == 0 ) {
        ld->value = 0.0;
    } else {
        ld->value = (double)high * 1e9 + (double)low;
    }
}
#endif

_WMRTLINK int __F_NAME(__Strtold,__wStrtold)( const CHAR_TYPE *bufptr,
                                    long_double *pld,
                                    CHAR_TYPE **endptr )
/*******************************************************/
{
    CHAR_TYPE   chr;
    int         sigdigits;
    int         fdigits;
    int         exponent;
    const CHAR_TYPE *oldptr;
    long_double ld;
    CHAR_TYPE   digitflag;
    char        flags;
    CHAR_TYPE   buffer[20];

    oldptr = bufptr;
    for(;;) {
        chr = *bufptr;
        if( chr != ' ' && ( chr < '\t' || chr > '\r' ) ) break;
        ++bufptr;
    }
    flags = 0;
    if( chr == '+' ) {
        ++bufptr;
    } else if( chr == '-' ) {
        flags |= NEGATIVE_NUMBER;
        ++bufptr;
    }
    sigdigits = 0;
    fdigits = 0;
    digitflag = '0';
    for(;;) {
        chr = *bufptr++;
        if( chr == '.' ) {
            if( flags & DOT_FOUND ) break;
            flags |= DOT_FOUND;
        } else {
            if( chr < '0' || chr > '9' ) break;
            if( flags & DOT_FOUND ) ++fdigits;
            digitflag |= chr;
            if( digitflag != '0' ) {        /* if a significant digit */
                if( sigdigits < MAX_DIGITS ) {
                    buffer[ sigdigits ] = (CHAR_TYPE)chr;
                }
                ++sigdigits;
            }
            flags |= DIGITS_PRESENT;
        }
    }
    exponent = 0;
    if( flags & DIGITS_PRESENT ) {
        if( chr == 'e' || chr == 'E' ) {
            oldptr = bufptr - 1;
            chr = *bufptr;
            if( chr == '+' ) {
                ++bufptr;
            } else if( chr == '-' ) {
                flags |= NEGATIVE_EXPONENT;
                ++bufptr;
            }
            flags &= ~ DIGITS_PRESENT;
            for( ;; ) {
                chr = *bufptr;              /* don't increment bufptr yet */
                if( chr < '0' || chr > '9' )  break;
                if( exponent < 1000 ) {
                    exponent = exponent * 10 + chr - '0';
                }
                flags |= DIGITS_PRESENT;
                ++bufptr;                           /* 27-nov-89 */
            }
            if( flags & NEGATIVE_EXPONENT ) {
                exponent = -exponent;
            }
            if( (flags & DIGITS_PRESENT) == 0 ) {
                bufptr = oldptr;
            }
        } else {    /* digits found, but no e or E */
            --bufptr;
        }
        oldptr = bufptr;
    }
    if( endptr != NULL ) *endptr = (CHAR_TYPE *)oldptr;
    exponent -= fdigits;
    if( sigdigits > MAX_DIGITS ) {
        exponent += sigdigits - MAX_DIGITS;
        sigdigits = MAX_DIGITS;
    }
    while( sigdigits > 0 ) {
        if( buffer[ sigdigits - 1 ] != '0' )  break;
        ++exponent;
        --sigdigits;
    }
    if( sigdigits == 0 ) {          /* number is 0.0   07-mar-91 */
#ifdef _LONG_DOUBLE_
        pld->exponent  = 0;
        pld->high_word = 0;
        pld->low_word  = 0;
#else
        pld->word[0] = 0;
        pld->word[1] = 0;
#endif
        return( _ZERO );                // indicate number is 0
    } else {
        buffer[ sigdigits ] = '\0';
        #ifdef __WIDECHAR__
        {
            // convert wide string of digits to skinny string of digits
            char tmp[20];
            wcstombs( tmp, buffer, sigdigits+1 );
            __ZBuf2LD( (char _WCNEAR *)tmp, (long_double _WCNEAR *)&ld );
        }
        #else
            __ZBuf2LD( (char _WCNEAR *)&buffer[0], (long_double _WCNEAR *)&ld );
        #endif
        if( exponent != 0 ) {
            _LDScale10x( (long_double _WCNEAR *)&ld, exponent );
        }
        if( flags & NEGATIVE_NUMBER ) {
#ifdef _LONG_DOUBLE_
            ld.exponent |= 0x8000;
#else
            ld.word[1] |= 0x80000000;
#endif
        }
    }
#ifdef _LONG_DOUBLE_
    pld->exponent  = ld.exponent;
    pld->high_word = ld.high_word;
    pld->low_word  = ld.low_word;
#else
    pld->word[0] = ld.word[0];
    pld->word[1] = ld.word[1];
#endif
    if( (exponent+sigdigits-1) > 308 ) {          /* overflow */
        return( _OVERFLOW );
    } else if( (exponent+sigdigits-1) < -308 ) {  /* underflow */
        return( _UNDERFLOW );
    }
    return( _NONZERO );                 // indicate number is non-zero
}


_WMRTLINK double __F_NAME(strtod,wcstod)( const CHAR_TYPE *bufptr, CHAR_TYPE **endptr )
/*************************************************************************************/
{
    int         rc;
    double      x;
    long_double ld;

    rc = __F_NAME(__Strtold,__wStrtold)( bufptr, &ld, endptr );
    if( rc == _ZERO ) {
        x = 0.0;
    } else {
#ifdef _LONG_DOUBLE_
        int exponent;
        exponent = ld.exponent & 0x7FFF;
        if( exponent >= 0x3FFF + 0x400 ) {      // if exponent too large
            __set_ERANGE();                     // - overflow
            if( ld.exponent & 0x8000 ) {
                x = - HUGE_VAL;
            } else {
                x = HUGE_VAL;
            }
        } else if( exponent < 0x3FFF - 0x3FF - 51 ) {  /* underflow */
            if( exponent < 0x3FFF - 0x3FFF - 52 ) {
                __set_ERANGE(); // we're sure it's zero
                x = 0.0;
            } else {
                union {
                    double          value;      // - double value
                    unsigned long   word[2];    // - so we can access bits
                } d;
                // not sure if it's really going to give back zero
                __iLDFD( (long_double _WCNEAR *)&ld, (double _WCNEAR *)&x );
                // retest for underflow given that we are really
                // returning a double
                d.value = x;
                if( x == 0.0 || (d.word[1] & 0x7FF00000) == 0 ) {
                    __set_ERANGE();
                }
            }
        } else {
            __iLDFD( (long_double _WCNEAR *)&ld, (double _WCNEAR *)&x );
            // retest for overflow or underflow given that we are really
            // returning a double
            if( rc == _OVERFLOW ) {
                __set_ERANGE();
            } else {
                union {
                    double          value;      // - double value
                    unsigned long   word[2];    // - so we can access bits
                } d;
                d.value = x;
                // if exponent is all zeros (denormal)
                if( (d.word[1] & 0x7FF00000) == 0 ) {
                    __set_ERANGE();
                }
                // if exponent is all ones (infinity or NAN)
                if( (d.word[1] & 0x7FF00000) == 0x7FF00000 ) {
                    __set_ERANGE();
                }
            }
        }
#else
        if( rc == _OVERFLOW ) {                 // if exponent too large
            __set_ERANGE();                     // - overflow
            if( ld.word[1] & 0x80000000 ) {
                x = - HUGE_VAL;
            } else {
                x = HUGE_VAL;
            }
        } else if( (ld.word[1] & 0x7FFFFFFF) == 0 && ld.word[0] == 0 ) {
            /* underflow */
            __set_ERANGE();
            x = 0.0;
        } else {
            x = ld.value;
        }
#endif
    }
    return( x );
}

