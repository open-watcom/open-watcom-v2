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
#include <stdlib.h>
#include "mbqnx.h"


#define MY_MB_LEN_MAX   ( sizeof(wchar_t) * 3 / 2 )
/*  If wchar_t is long,  MY_MB_LEN_MAX will be 6.
 *  If wchar_t is short, MY_MB_LEN_MAX will be 3.
 *  It should be equal to MB_LEN_MAX anyway.
 */


int mbtowc( wchar_t *pwc, const char *s, size_t n )
{
    unsigned char       c;

    if( s != NULL ) {
        if( ( c = *s ) != '\0' ) {
            if( n > 0 ) {
                if( (signed char)( c = *s ) > 0 ) {
                    if( pwc != NULL ) *pwc = c;
                    return( 1 );
                }
                if( sizeof(wchar_t) == 2 ) {
                    /* Explicitly optimised for 16-bit wchar_t */
                    if( ( c & 0xE0 ) == 0xC0 ) {
                        unsigned char   d;
                        if( n < 2  ||  ((d=s[1]^0x80)&0xC0) != 0 ) {
                            /* return -1; */
                            goto neg1;
                        }
                        if( pwc != NULL ) *pwc = ((c&0x1F) << 6) | d;
                        return( 2 );
                    }
                    if( ( c & 0xF0 ) == 0xE0 ) {
                        unsigned char   d, e;
                        if( n < 3                                   ||
                            ( (d = *++s^0x80) & 0xC0 ) != 0         ||
                            ( (e = *++s^0x80) & 0xC0 ) != 0 ) {
                            /* return -1; */
                            goto neg1;
                        }
                        if( pwc != NULL ) *pwc = ( ( ((c&0x1F) << 6)|d ) << 6)|e;
                        return( 3 );
                    }
                } else {
#if 0
    /* this section won't compile with 16-bit wchar_t */
                    /* wchar_t is 32 bit wide (this would also work with 16 bits) */
                    int         nb;
                    wchar_t     val;
                    if( n > MY_MB_LEN_MAX ) {
                        n = MY_MB_LEN_MAX;
                    }
                    nb = 0;
                    val = c;
                    while( c & 0x40 ) {
                        unsigned char   b;
                        if( ( (b=*++s^0x80) & 0xC0 ) != 0 ) {
                            /* return -1; */
                            goto neg1;
                        }
                        val = val << 6|b;
                        if( ++nb > n ) {
                            /* return -1; */
                            goto neg1;
                        }
                        c <<= 1;
                    }
                    if( nb == 0 ) { /* Valid values for nb are 1...5 */
                        /* return -1; */
                        goto neg1;
                    }
                    if( pwc != NULL ) {
                        #define MASK( nb ) ~( ~0uL << 5 * nb + 6 )
                        static const wchar_t masks[5] = {
                            MASK( 1 ),  MASK( 2 ),
                            MASK( 3 ),  MASK( 4 ),
                            MASK( 5 )
                        };
                        *pwc = val & masks[ nb - 1 ];
                    }
                    return nb+1;
#endif
                }
            }
            neg1: return( -1 );
        }
        if( pwc != NULL ) {
            *pwc = c;
        }
    }
    return( 0 ); /* Not state dependent encoding */
}
