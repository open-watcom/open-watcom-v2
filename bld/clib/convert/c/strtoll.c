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
* Description:  Implementation of strtoll() - convert string to long long.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#else
    #include <ctype.h>
#endif
#include <limits.h>
#include "rtdata.h"
#include "rterrno.h"
#include "thread.h"

/* This is heavily based on strtol() implementation; however this code needs
 * to use 64-bit arithmetic and there is little need to drag in all the
 * supporting code for people who just need 16- or 32-bit string to integer
 * conversion.
 */

/*
 * this table is the largest value that can safely be multiplied
 * by the associated base without overflowing
 */
static unsigned long long nearly_overflowing[] = {
    ULLONG_MAX / 2,  ULLONG_MAX / 3,  ULLONG_MAX / 4,  ULLONG_MAX / 5,
    ULLONG_MAX / 6,  ULLONG_MAX / 7,  ULLONG_MAX / 8,  ULLONG_MAX / 9,
    ULLONG_MAX / 10, ULLONG_MAX / 11, ULLONG_MAX / 12, ULLONG_MAX / 13,
    ULLONG_MAX / 14, ULLONG_MAX / 15, ULLONG_MAX / 16, ULLONG_MAX / 17,
    ULLONG_MAX / 18, ULLONG_MAX / 19, ULLONG_MAX / 20, ULLONG_MAX / 21,
    ULLONG_MAX / 22, ULLONG_MAX / 23, ULLONG_MAX / 24, ULLONG_MAX / 25,
    ULLONG_MAX / 26, ULLONG_MAX / 27, ULLONG_MAX / 28, ULLONG_MAX / 29,
    ULLONG_MAX / 30, ULLONG_MAX / 31, ULLONG_MAX / 32, ULLONG_MAX / 33,
    ULLONG_MAX / 34, ULLONG_MAX / 35, ULLONG_MAX / 36
};

static int radix_value( CHAR_TYPE c )
{
    if( c >= STRING( '0' ) && c <= STRING( '9' ) )
        return( c - STRING( '0' ) );
    c = __F_NAME(tolower,towlower)( (UCHAR_TYPE)c );
    if( c >= STRING( 'a' ) &&  c <= STRING( 'i' ) )
        return( c - STRING( 'a' ) + 10 );
    if( c >= STRING( 'j' ) &&  c <= STRING( 'r' ) )
        return( c - STRING( 'j' ) + 19 );
    if( c >= STRING( 's' ) &&  c <= STRING( 'z' ) )
        return( c - STRING( 's' ) + 28 );
    return( 37 );
}

#define hexstr(p) (p[0] == STRING( '0' ) && (p[1] == STRING( 'x' ) || p[1] == STRING( 'X' )))

static unsigned long long int _stoll( const CHAR_TYPE *nptr, CHAR_TYPE **endptr, int base, int who )
{
    const CHAR_TYPE         *p;
    const CHAR_TYPE         *startp;
    int                     digit;
    unsigned long long int  value;
    unsigned long long int  prev_value;
    CHAR_TYPE               sign;
    char                    overflow;   /*overflow is used as a flag so it does not
                                         *need to be of type CHAR_TYPE */

    if( endptr != NULL )
        *endptr = (CHAR_TYPE *)nptr;
    p = nptr;
    while( __F_NAME(isspace,iswspace)( (UCHAR_TYPE)*p ) )
        ++p;
    sign = *p;
    if( sign == STRING( '+' ) || sign == STRING( '-' ) )
        ++p;
    if( base == 0 ) {
        if( hexstr( p ) ) {
            base = 16;
        } else if( *p == STRING( '0' ) ) {
            base = 8;
        } else {
            base = 10;
        }
    }
    if( base < 2 || base > 36 ) {
        _RWD_errno = EDOM;
        return( 0 );
    }
    if( base == 16 ) {
        if( hexstr( p ) ) {
            p += 2;    /* skip over '0x' */
        }
    }
    startp = p;
    overflow = 0;
    value = 0;
    for( ;; ) {
        digit = radix_value( *p );
        if( digit >= base )
            break;
        if( value > nearly_overflowing[base - 2] )
            overflow = 1;
        prev_value = value;
        value = value * base + digit;
        if( value < prev_value )
            overflow = 1;
        ++p;
    }
    if( p == startp )
        p = nptr;
    if( endptr != NULL )
        *endptr = (CHAR_TYPE *)p;
    if( who == 1 ) {
        if( value >= 0x8000000000000000 ) {
            if( value == 0x8000000000000000 && sign == STRING( '-' ) ) {
                ;  /* OK */
            } else {
                overflow = 1;
            }
        }
    }
    if( overflow ) {
        _RWD_errno = ERANGE;
        if( who == 0 )
            return( ULLONG_MAX );
        if( sign == STRING( '-' ) )
            return( LLONG_MIN );
        return( LLONG_MAX );
    }
    if( sign == STRING( '-' ) )
        value = - value;
    return( value );
}


_WCRTLINK unsigned long long int __F_NAME(strtoull,wcstoull)( const CHAR_TYPE *nptr, CHAR_TYPE **endptr, int base )
{
    return( _stoll( nptr, endptr, base, 0 ) );
}


_WCRTLINK long long int __F_NAME(strtoll,wcstoll)( const CHAR_TYPE *nptr, CHAR_TYPE **endptr, int base )
{
    return( _stoll( nptr, endptr, base, 1 ) );
}

/* Assuming that intmax_t is equal to long long and uintmax_t to unsigned long long */
_WCRTLINK uintmax_t __F_NAME(strtoumax,wcstoumax)( const CHAR_TYPE *nptr, CHAR_TYPE **endptr, int base )
{
    return( _stoll( nptr, endptr, base, 0 ) );
}

_WCRTLINK intmax_t __F_NAME(strtoimax,wcstoimax)( const CHAR_TYPE *nptr, CHAR_TYPE **endptr, int base )
{
    return( _stoll( nptr, endptr, base, 1 ) );
}
