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


#include <stdlib.h>
#include "mbqnx.h"


#define MY_MB_LEN_MAX   ( sizeof(wchar_t) * 3 / 2 )
/*  If wchar_t is long,  MY_MB_LEN_MAX will be 6.
 *  If wchar_t is short, MY_MB_LEN_MAX will be 3.
 *  It should be equal to MB_LEN_MAX anyway.
 */


int wctomb( char *s, wchar_t wc )
{
    if( s != NULL ) {
        if( wc < 0x80 ) {
            *s = wc;
            return( 1 );
        } else {
            if( sizeof(wchar_t) == 2 ) {
                /* Explicitly optimised for 16-bit wchar_t */
                if( wc < 0x800 ) {
                    s[0] = 0xC0 | wc >> 6;
                    s[1] = 0x80 | wc & 0x3F;
                    return( 2 );
                } else {
                    s[2] = 0x80 | wc & 0x3F;
                    s[1] = 0x80 | ( wc >>= 6 ) & 0x3F;
                    s[0] = 0xE0 | wc >> 6;
                    return( 3 );
                }
            } else {
                /* wchar_t is 32 bit wide (this would also work with 16 bits) */
                char *  dst = s;
                int     rv;
                {   /* The block seems to help the compiler optimise */
                    wchar_t mask = ~0ul << 5+6;
                    while( mask & wc ) {
                        mask <<= 5;
                        ++dst;
                    }
                }
                if( ( rv = ++dst - s ) < MY_MB_LEN_MAX ) {
                    wchar_t     mask;
                    do {
                        *dst = 0x80 | ( wc & 0x3F );
                        wc >>= 6;
                    } while( --dst != s );
                    mask = 0x7F >> ++rv;
                    *dst = wc & mask | ( ~mask << 1 );
                    return( rv );
                }
                return( -1 );
            }
        }
    } else {
        return( 0 ); /* Not state dependent encoding */
    }
}
