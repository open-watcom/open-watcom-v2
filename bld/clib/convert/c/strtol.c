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
* Description:  Implementation of strtol() - convert string to long.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#else
    #include <ctype.h>
#endif
#include <limits.h>
#include "bool.h"
#include "rtdata.h"
#include "rterrno.h"
#include "thread.h"

/*
 * this table is the largest value that can safely be multiplied
 * by the associated base without overflowing
 */
static unsigned long nearly_overflowing[] = {
    ULONG_MAX / 2,  ULONG_MAX / 3,  ULONG_MAX / 4,  ULONG_MAX / 5,
    ULONG_MAX / 6,  ULONG_MAX / 7,  ULONG_MAX / 8,  ULONG_MAX / 9,
    ULONG_MAX / 10, ULONG_MAX / 11, ULONG_MAX / 12, ULONG_MAX / 13,
    ULONG_MAX / 14, ULONG_MAX / 15, ULONG_MAX / 16, ULONG_MAX / 17,
    ULONG_MAX / 18, ULONG_MAX / 19, ULONG_MAX / 20, ULONG_MAX / 21,
    ULONG_MAX / 22, ULONG_MAX / 23, ULONG_MAX / 24, ULONG_MAX / 25,
    ULONG_MAX / 26, ULONG_MAX / 27, ULONG_MAX / 28, ULONG_MAX / 29,
    ULONG_MAX / 30, ULONG_MAX / 31, ULONG_MAX / 32, ULONG_MAX / 33,
    ULONG_MAX / 34, ULONG_MAX / 35, ULONG_MAX / 36
};

#define hexstr(p) (p[0] == STRING( '0' ) && (p[1] == STRING( 'x' ) || p[1] == STRING( 'X' )))


static int radix_value( CHAR_TYPE c )
{
    if( c >= STRING( '0' ) &&  c <= STRING( '9' ) )
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


static unsigned long int _stol( const CHAR_TYPE *nptr, CHAR_TYPE **endptr, int base, bool who )
{
    const CHAR_TYPE         *p;
    const CHAR_TYPE         *startp;
    int                     digit;
    unsigned long int       value;
    unsigned long int       prev_value;
    bool                    minus;
    bool                    overflow;   /*overflow is used as a flag so it does not
                                         *need to be of type CHAR_TYPE */

    if( endptr != NULL )
        *endptr = (CHAR_TYPE *)nptr;
    p = nptr;
    while( __F_NAME(isspace,iswspace)( (UCHAR_TYPE)*p) )
        ++p;
    minus = false;
    switch( *p ) {
    case STRING( '-' ):
        minus = true;
        // fall down
    case STRING( '+' ):
        ++p;
        break;
    }
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
    overflow = false;
    value = 0;
    for( ;; ) {
        digit = radix_value( *p );
        if( digit >= base )
            break;
        if( value > nearly_overflowing[base - 2] )
            overflow = true;
        prev_value = value;
        value = value * base + digit;
        if( value < prev_value )
            overflow = true;
        ++p;
    }
    if( p == startp )
        p = nptr;
    if( endptr != NULL )
        *endptr = (CHAR_TYPE *)p;
    if( who ) {
        if( value >= 0x80000000 ) {
            if( value == 0x80000000 && minus ) {
                ;  /* OK */
            } else {
                overflow = true;
            }
        }
    }
    if( overflow ) {
        _RWD_errno = ERANGE;
        if( !who )
            return( ULONG_MAX );
        if( minus )
            return( LONG_MIN );
        return( LONG_MAX );
    }
    if( minus )
        value = - value;
    return( value );
}


_WCRTLINK unsigned long int __F_NAME(strtoul,wcstoul)( const CHAR_TYPE *nptr, CHAR_TYPE **endptr, int base )
{
    return( _stol( nptr, endptr, base, false ) );
}


_WCRTLINK long int __F_NAME(strtol,wcstol)( const CHAR_TYPE *nptr, CHAR_TYPE **endptr, int base )
{
    return( _stol( nptr, endptr, base, true ) );
}
