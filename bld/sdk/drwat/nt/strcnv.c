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


#include "i64.h"
#include "ctype.h"
#include <windows.h>

static char * eatSpace( char *str )
{
    while( *str != '\0' ) {
        if( !isspace(*str) ) {
            break;
        }
    str++;
    }
    return( str );
}

BOOL StrToU64( char *str, unsigned_64 *u64, BOOL neg )
{
    unsigned_64 r64;
    unsigned_64 v64;
    int         value;
    int         radix;

    str = eatSpace( str );
    U32ToU64( 0, u64 );
    if( neg == TRUE ) {
        if( *str == '-') {
            str++;
        } else {
            if( *str == '+' ) {
                str++;
                neg = FALSE;
            }
        }
    }
    str = eatSpace( str );
    switch( *str ){
    case '\0':
        return( FALSE );
    case '0':
        str++;
        if( tolower(*str) == 'x' ) {
            radix = 16;
            str++;
        } else {
            radix = 8;
        }
        break;
    default:
        radix = 10;
    }
    U32ToU64( radix, &r64 );
    while( *str != '\0' ) {
        if( isdigit( *str ) ) {
            value = *str - '0';
        } else {
            value = 10 + tolower( *str ) - 'a';
        }
        if( value < 0 || value >= radix ) {
            return ( FALSE );
        }
        U32ToU64( value, &v64 );
        U64Mul( u64, &r64, u64 );
        U64Add( &v64, u64, u64 );
        str++;
    }
    if( neg == TRUE) {
        U64Neg( u64, u64 );
    }
    return( TRUE );
}
