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


#ifndef _UTF8SUPP_H
#define _UTF8SUPP_H

// add other UTF8 encodings
#if !defined(_UTF8_JAVA)
#define _UTF8_JAVA
#endif

#ifdef _UTF8_JAVA

#if defined( UNDER_PALM_OS ) && defined( __GNUC__ )
    #define __attribute__( a )  __attribute__(( a ))
#else
    #define __attribute__( a )
#endif


#ifdef _UTF8_ENCODE
static unsigned UTF8Encode( char *dest, unsigned c )
        __attribute__( section( "ULRT3" ) );

static unsigned UTF8Encode( char *dest, unsigned c ) {
    if( c >= 0x0001 && c <= 0x007f ) {
        dest[0] = (char)c;
        return( 1 );
    }
    if( c <= 0x07ff ) {
        // 0x0000 is encoded as two bytes
        dest[0] = (char)( 0xc0 | (( c >> 6 ) & 0x1f));
        dest[1] = (char)( 0x80 | ( c & 0x3f));
        return( 2 );
    }
    dest[0] = (char)( 0xe0 | (( c >> 12 ) & 0x0f));
    dest[1] = (char)( 0x80 | (( c >> 6 ) & 0x03f));
    dest[2] = (char)( 0x80 | ( c & 0x03f));
    return( 3 );
}
#endif

#ifdef _UTF8_DECODE
static unsigned UTF8Decode( char const *dest, unsigned *c )
        __attribute__( section( "ULRT3" ) );

static unsigned UTF8Decode( char const *dest, unsigned *c ) {
    unsigned char const *p = (unsigned char const *)dest;
    unsigned char c0, c1, c2;

    c0 = p[0];
    if( c0 <= 0x7f ) {
        *c = c0;
        return( 1 );
    }
    c1 = p[1];
    if( c0 < 0xe0 ) {
        *c = (( c0 & 0x1f ) << 6 ) | ( c1 & 0x3f );
        return( 2 );
    }
    c2 = p[2];
    *c = (( c0 & 0x0f ) << 12 ) | (( c1 & 0x3f ) << 6 ) | ( c2 & 0x3f );
    return( 3 );
}
#endif

#ifdef _UTF8_CHECK
static unsigned UTF8Check( char const *dest, unsigned len )
        __attribute__( section( "ULRT3" ) );

// returns 0 if the buffer doesn't contain a valid UTF8 encoding prefix
// that uses all the characters provided
static unsigned UTF8Check( char const *dest, unsigned len ) {
    unsigned char const *p = (unsigned char const *)dest;

    switch( len ) {
    case 1:
        if(( p[0] & 0x80 ) != 0x00 ) {
            break;
        }
        return 1;
    case 2:
        if(( p[0] & 0xe0 ) != 0xc0 ) {
            break;
        }
        if(( p[1] & 0xc0 ) != 0x80 ) {
            break;
        }
        return 1;
    case 3:
        if(( p[0] & 0xf0 ) != 0xe0 ) {
            break;
        }
        if(( p[1] & 0xc0 ) != 0x80 ) {
            break;
        }
        if(( p[2] & 0xc0 ) != 0x80 ) {
            break;
        }
        return 1;
    }
    return 0;
}

/* test code
#include <assert.h>
#define _UTF8_CHECK
#define _UTF8_DECODE
#include "utf8supp.h"

main() {
    unsigned c;
    char buff[3];
    int i,j,k;

    for( i = 0; i < 0x80; ++i ) {
        buff[0] = i;
        assert( UTF8Check( buff, 1 ) != 0 );
        assert( UTF8Decode( buff, &c ) == 1 );
        assert( c < 0x80 );
    }
    for( i = 0xc0; i <= 0xdf; ++i ) {
        buff[0] = i;
        for( j = 0x80; j <= 0xbf; ++j ) {
            buff[1] = j;
            assert( UTF8Check( buff, 2 ) != 0 );
            assert( UTF8Decode( buff, &c ) == 2 );
            assert( c < 0x0800 );
        }
    }
    for( i = 0xe0; i <= 0xef; ++i ) {
        buff[0] = i;
        for( j = 0x80; j <= 0xbf; ++j ) {
            buff[1] = j;
            for( k = 0x80; k <= 0xbf; ++k ) {
                buff[2] = k;
                assert( UTF8Check( buff, 3 ) != 0 );
                assert( UTF8Decode( buff, &c ) == 3 );
                assert( c <= 0xffff );
            }
        }
    }
    for( i = 0; i < 0x100; ++i ) {
        buff[0] = i;
        if( i < 0x80 ) {
            assert( UTF8Check( buff, 1 ) != 0 );
        } else {
            assert( UTF8Check( buff, 1 ) == 0 );
            if( 0xc0 <= i && i <= 0xdf ) {
                for( j = 0; j < 0x100; ++j ) {
                    buff[1] = j;
                    if( 0x80 <= j && j <= 0xbf ) {
                        assert( UTF8Check( buff, 2 ) != 0 );
                        for( k = 0; k < 0x100; ++k ) {
                            assert( UTF8Check( buff, 3 ) == 0 );
                        }
                    } else {
                        assert( UTF8Check( buff, 2 ) == 0 );
                    }
                }
            } else if( 0xe0 <= i && i <= 0xef ) {
                for( j = 0; j < 0x100; ++j ) {
                    buff[1] = j;
                    if( 0x80 <= j && j <= 0xbf ) {
                        assert( UTF8Check( buff, 2 ) == 0 );
                        for( k = 0; k < 0x100; ++k ) {
                            buff[2] = k;
                            if( 0x80 <= k && k <= 0xbf ) {
                                assert( UTF8Check( buff, 3 ) != 0 );
                            } else {
                                assert( UTF8Check( buff, 3 ) == 0 );
                            }
                        }
                    } else {
                        assert( UTF8Check( buff, 2 ) == 0 );
                    }
                }
            } else {
                for( j = 0; j < 0x100; ++j ) {
                    buff[1] = j;
                    assert( UTF8Check( buff, 2 ) == 0 );
                    for( k = 0; k < 0x100; ++k ) {
                        buff[2] = k;
                        assert( UTF8Check( buff, 3 ) == 0 );
                    }
                }
            }
        }
    }
    puts( "PASS" );
}
*/

#endif

#endif

#endif
