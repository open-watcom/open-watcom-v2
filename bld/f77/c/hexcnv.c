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
* Description:  Convert hex string to binary
*
****************************************************************************/

#include "ftnstd.h"
#include "hexcnv.h"

#include <string.h>
#include <ctype.h>

static const char __FAR HexDigits[ 16 ] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};


char    Hex( char data ) {
//========================

    if( isdigit( data ) == 0 ) {
        data += 9;
    }
    data &= 0x0f;
    return( data );
}


uint    HSToB( char *src, uint src_len, char *dst ) {
//===================================================

    uint        length;

    length = 0;
    if( ( src_len % 2 ) != 0 ) {
        if( isxdigit( *src ) == 0 ) return( 0 );
        *dst = Hex( *src );
        ++dst;
        ++length;
        ++src;
        --src_len;
    }
    while( src_len != 0 ) {
        if( isxdigit( *src ) == 0 ) return( length );
        *dst = Hex( *src ) * 0x10;
        ++src;
        --src_len;
        if( isxdigit( *src ) == 0 ) return( length );
        *dst += Hex( *src );
        ++src;
        --src_len;
        ++dst;
        ++length;
    }
    return( length );
}


char    *BToHS( char *mem, int length, char *fmt_buf ) {
//==============================================================

    char        data;

    for(;;) {
        if( --length < 0 ) break;
        data = *mem;
        ++mem;
        *fmt_buf = HexDigits[( data >> 4 ) & 0x0f];
        ++fmt_buf;
        if( --length < 0 ) break;
        *fmt_buf = HexDigits[data & 0x0f];
        ++fmt_buf;
    }
    *fmt_buf = NULLCHAR;
    return( fmt_buf );
}
