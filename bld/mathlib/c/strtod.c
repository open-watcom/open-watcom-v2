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
#include "widechar.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#ifdef __WIDECHAR__
    #include <wchar.h>
#endif
#include "watcom.h"
#include "clibsupp.h"
#include "xfloat.h"


typedef enum {
    NEGATIVE_NUMBER   = 0x01,
    NEGATIVE_EXPONENT = 0x02,
    DIGITS_PRESENT    = 0x04,
    DOT_FOUND         = 0x08,
    HEX_FOUND         = 0x10,
    NAN_FOUND         = 0x20,
    INFINITY_FOUND    = 0x40,
    INVALID_SEQ       = 0x80
} flt_flags;

enum {
    _ZERO       = 0,
    _ISNAN,
    _ISINF,
    _NONZERO,
    _UNDERFLOW,
    _OVERFLOW,
    _NEGATIVE   = 0x80
};

/* Maximum hex digits */
#define MAX_HEX_DIGITS  16

#ifdef _LONG_DOUBLE_
 #define MAX_DIGITS     19
#else
 #define MAX_DIGITS     18
#endif

/* Size of buffer holding significant digits. Must be large enough
 * to support the highest precision type.
 */
#define MAX_SIG_DIG     32

#if MAX_SIG_DIG < MAX_DIGITS
    #error MAX_SIG_DIG is too small
#endif

#if !(defined(_LONG_DOUBLE_) || defined(__WIDECHAR__))
void __ZBuf2LD( buf_stk_ptr buf, ld_stk_ptr ld )
{
    int             i;
    int             n;
    long            high;
    long            low;
    const CHAR_TYPE *s;

    s = (CHAR_TYPE *)buf;
    n = __F_NAME(strlen,wcslen)( s );
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

static unsigned char xdigit2bin( CHAR_TYPE chr )
{
    unsigned char   value;

    value = chr - '0';
    if( value > 9 ) {
        value = chr - 'A' + 10;
        if( value > 15 ) {
            value = chr - 'a' + 10;
        }
    }
    return( value );
}

static void __ZXBuf2LD( buf_stk_ptr buf, ld_stk_ptr ld, int *exponent )
{
    int             i;
    int             n;
    int32_t         exp;
    uint32_t        high;
    uint32_t        low;
    const CHAR_TYPE *s;

    s = (const CHAR_TYPE *)buf;
    n = __F_NAME(strlen,wcslen)( s );
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

/* Determine what sort of subject sequence, if any, is provided, and parse
 * NaNs and Infinity.
 */
static flt_flags subject_seq( const CHAR_TYPE *s, const CHAR_TYPE **endptr )
{
    CHAR_TYPE           chr;
    flt_flags           flags = 0;

    /* Skip whitespace */
    for( chr = *s; isspace( chr ); chr = *++s )
        ;

    /* Parse optional sign */
    if( chr == '+' ) {
        ++s;
    } else if( chr == '-' ) {
        flags |= NEGATIVE_NUMBER;
        ++s;
    }

    if( *s == 'n' || *s == 'N' ) {
        /* Could be a NaN */
        if( (*++s == 'a' || *s == 'A')
          && (*++s == 'n' || *s == 'N') ) {
            /* Yup, it is. Skip the optional digit/nondigit sequence */
            flags |= NAN_FOUND;
            if( *++s == '(' ) {
                const CHAR_TYPE *p;

                p = ++s;
                while( isalnum( *p ) || *p == '_' )
                    ++p;

                if( *p == ')' ) {
                    s = ++p;    /* only update s if valid sequence found */
                }
            }
        } else {
            flags = INVALID_SEQ;
        }
    } else if ( *s == 'i' || *s == 'I' ) {
        /* Could be infinity */
        if( (*++s == 'n' || *s == 'N')
          && (*++s == 'f' || *s == 'F') ) {
            const CHAR_TYPE     *p;

            /* Yup, it is. See if it's the long form */
            flags |= INFINITY_FOUND;
            p = s;
            if( (*++p == 'i' || *p == 'I')
              && (*++p == 'n' || *p == 'N')
              && (*++p == 'i' || *p == 'I')
              && (*++p == 't' || *p == 'T')
              && (*++p == 'y' || *p == 'Y') ) {

                s = p;
            }
            ++s;
        } else {
            flags = INVALID_SEQ;
        }
    } else if( s[0] == '0' && (s[1] == 'x' || s[1] == 'X') ) {
        /* We've been hexed! */
        flags |= HEX_FOUND;
        s += 2;
    }
    *endptr = s;

    return( flags );
}

/* Parse a decimal and a hexadecimal floating-point number
 * collect siginficant digits into buffer and determine exponent
 * power of ten for decimal form
 * power of two for hexadecimal form
 */
static flt_flags parse_float( char hex, const CHAR_TYPE *input, CHAR_TYPE *buffer,
                                const CHAR_TYPE **endptr, int *exp, int *sig )
{
    CHAR_TYPE           chr;
    flt_flags           flags     = 0;
    int                 sigdigits = 0;
    int                 fdigits   = 0;
    int                 exponent  = 0;
    CHAR_TYPE           digitflag = '0';
    const CHAR_TYPE     *s = input;

    /* Parse the mantissa */
    for( ;; ) {
        chr = *s++;
        if( chr == '.' ) {  // <- needs changing for locale support
            if( flags & DOT_FOUND )
                break;
            flags |= DOT_FOUND;
        } else {
            if( hex ) {
                if( !isxdigit( chr ) ) {
                    break;
                }
            } else {
                if( !isdigit( chr ) ) {
                    break;
                }
            }
            if( flags & DOT_FOUND ) {
                ++fdigits;
            }
            digitflag |= chr;
            if( digitflag != '0' ) {        /* if a significant digit */
                if( sigdigits < MAX_SIG_DIG ) {
                    buffer[sigdigits] = chr;
                }
                ++sigdigits;
            }
            flags |= DIGITS_PRESENT;
        }
    }

    /* Parse the optional exponent */
    if( flags & DIGITS_PRESENT ) {
        int     digit_exp_size;
        int     max_exponent;
        int     max_digits;

        if( !hex && ( chr == 'e' || chr == 'E' )
          || hex && ( chr == 'p' || chr == 'P' ) ) {
            const CHAR_TYPE *p;

            p = s - 1;
            chr = *s;
            if( chr == '+' ) {
                ++s;
            } else if( chr == '-' ) {
                flags |= NEGATIVE_EXPONENT;
                ++s;
            }
            flags &= ~DIGITS_PRESENT;
            max_exponent = ( hex ) ? 10000 : 2000;
            for( ;; ) {
                chr = *s;                   /* don't increment s yet */
                if( !isdigit( chr ) )
                    break;
                if( exponent < max_exponent ) {
                    exponent = exponent * 10 + chr - '0';
                }
                flags |= DIGITS_PRESENT;
                ++s;
            }
            if( flags & NEGATIVE_EXPONENT ) {
                exponent = -exponent;
            }
            if( !(flags & DIGITS_PRESENT) ) {
                s = p;
            }
        } else {    /* digits found, but no e or E */
            --s;
        }

        max_digits = ( hex ) ? MAX_HEX_DIGITS : MAX_DIGITS;
        digit_exp_size = ( hex ) ? 4 : 1;

        exponent -= fdigits * digit_exp_size;

        if( sigdigits > max_digits ) {
            exponent += sigdigits - max_digits * digit_exp_size;
            sigdigits = max_digits;
        }

        while( sigdigits > 0 ) {
            if( buffer[sigdigits - 1] != '0' )
                break;
            exponent += digit_exp_size;
            --sigdigits;
        }
    } else {
        s = input;  /* not a number (could be just dot) */
    }

    *exp = exponent;
    *sig = sigdigits;
    *endptr = s;
    return( flags );
}


/*
 * Note: The only external user of __Strtold() is currently
 *       the C front end
 */

_WMRTLINK int __F_NAME(__Strtold,__wStrtold)( const CHAR_TYPE *bufptr,
                                              long_double *pld,
                                              CHAR_TYPE **endptr )
/********************************************************************/
{
    int                 sigdigits;
    int                 exponent;
    const CHAR_TYPE     *cur_ptr;
    long_double         ld;
    flt_flags           flags;
    CHAR_TYPE           buffer[MAX_SIG_DIG + 1];
    buf_stk_ptr         tmpbuf;
    int                 rc;
    int                 neg;
    char                hex;

    cur_ptr = bufptr;
    flags   = subject_seq( bufptr, &cur_ptr );
    bufptr  = cur_ptr;

    hex = ( flags & HEX_FOUND );
    neg = ( flags & NEGATIVE_NUMBER ) ? _NEGATIVE : 0;

    if( flags & (INFINITY_FOUND | NAN_FOUND) ) {
#ifdef _LONG_DOUBLE_
        pld->exponent  = 0x7FFF;
        pld->high_word = 0;
        pld->low_word  = 0;
#else
        pld->u.word[I64LO32] = 0x7FF;
        pld->u.word[I64HI32] = 0;
#endif
        if( flags & NAN_FOUND ) {
#ifdef _LONG_DOUBLE_
            pld->high_word |= 0x80000000;
#else
            pld->u.word[I64HI32] |= 0x00080000;
#endif
            rc = _ISNAN;
        } else {
            rc = _ISINF;
        }

        if( flags & NEGATIVE_NUMBER ) {
#ifdef _LONG_DOUBLE_
            pld->exponent |= 0x8000;
#else
            pld->u.word[I64HI32] |= 0x80000000;
#endif
        }
        if( endptr != NULL )
            *endptr = (CHAR_TYPE *)cur_ptr;
        return( rc | neg );
    }

    flags |= parse_float( hex, bufptr, buffer, &cur_ptr, &exponent, &sigdigits );
    if( endptr != NULL ) {
        *endptr = (CHAR_TYPE *)cur_ptr;
    }

    if( sigdigits == 0 ) {          /* number is 0.0 */
#ifdef _LONG_DOUBLE_
        pld->exponent  = 0;
        pld->high_word = 0;
        pld->low_word  = 0;
#else
        pld->u.word[I64LO32] = 0;
        pld->u.word[I64HI32] = 0;
#endif
        return( _ZERO | neg );      /* indicate zero */
    } else {
#ifdef __WIDECHAR__
        char    tmp[MAX_SIG_DIG + 1];
#endif
        buffer[sigdigits] = '\0';
#ifdef __WIDECHAR__
        // convert wide string of digits to skinny string of digits
        wcstombs( tmp, buffer, sizeof( tmp ) );
        tmpbuf = &tmp;
#else
        tmpbuf = &buffer;
#endif
        if( hex ) {
            __ZXBuf2LD( tmpbuf, &ld, &exponent );
        } else {
            __ZBuf2LD( tmpbuf, &ld );
            if( exponent != 0 ) {
                _LDScale10x( &ld, exponent );
            }
        }
        if( flags & NEGATIVE_NUMBER ) {
#ifdef _LONG_DOUBLE_
            ld.exponent |= 0x8000;
#else
            ld.u.word[I64HI32] |= 0x80000000;
#endif
        }
    }
#ifdef _LONG_DOUBLE_
    pld->exponent  = ld.exponent;
    pld->high_word = ld.high_word;
    pld->low_word  = ld.low_word;
    if( ( exponent + sigdigits - 1 ) > 4932 ) {         /* overflow */
        return( _OVERFLOW | neg );
    } else if( ( exponent + sigdigits - 1 ) < -4932 ) { /* underflow */
        return( _UNDERFLOW | neg );
    }
#else
    pld->u.word[0] = ld.u.word[0];
    pld->u.word[1] = ld.u.word[1];
    if( ( exponent + sigdigits - 1 ) > 308 ) {          /* overflow */
        return( _OVERFLOW | neg );
    } else if( ( exponent + sigdigits - 1 ) < -308 ) {  /* underflow */
        return( _UNDERFLOW | neg );
    }
#endif
    return( _NONZERO );                 // indicate number is non-zero
}


_WMRTLINK double __F_NAME(strtod,wcstod)( const CHAR_TYPE *bufptr, CHAR_TYPE **endptr )
/*************************************************************************************/
{
    int         rc, type;
    double      x;
    long_double ld;

    rc = __F_NAME(__Strtold,__wStrtold)( bufptr, &ld, endptr );
    type = rc & ~_NEGATIVE;
    if( type == _ZERO || type == _ISNAN || type == _ISINF ) {
        if( type == _ZERO ) {
            x = 0.0;
        } else if( type == _ISNAN ) {
            x = _NAN;
        } else {
            x = _INF;
        }
        if( rc & _NEGATIVE ) {
            x = -x;
        }
    } else {
#ifdef _LONG_DOUBLE_
        int     exponent;

        exponent = ld.exponent & 0x7FFF;
        if( exponent >= 0x3FFF + 0x400 ) {      // if exponent too large
            __set_ERANGE();                     // - overflow
            if( rc & _NEGATIVE ) {
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
                __iLDFD( &ld, &x );
                // retest for underflow given that we are really
                // returning a double
                d.value = x;
                if( x == 0.0 || (d.word[1] & 0x7FF00000) == 0 ) {
                    __set_ERANGE();
                }
            }
        } else {
            __iLDFD( &ld, &x );
            // retest for overflow or underflow given that we are really
            // returning a double
            if( type == _OVERFLOW ) {
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
        if( type == _OVERFLOW ) {               // if exponent too large
            __set_ERANGE();                     // - overflow
            if( ld.u.word[1] & 0x80000000 ) {
                x = - HUGE_VAL;
            } else {
                x = HUGE_VAL;
            }
        } else if( (ld.u.word[1] & 0x7FFFFFFF) == 0 && ld.u.word[0] == 0 ) {
            /* underflow */
            __set_ERANGE();
            x = 0.0;
        } else {
            x = ld.u.value;
        }
#endif
    }
    return( x );
}
