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
#include <string.h>
#include "watcom.h"
#include "machtype.h"
#include "bitman.h"

/*
 * get bits from unbyte aligned src and put in aligned dst
 */
void BitGet( unsigned_8 *dst, unsigned_8 *src, unsigned short start,
        unsigned char size )
{
    int             extra;
    int             i;
    int             left_shift;
    int             right_shift;
    unsigned_8      mask;

    right_shift = start % BITS_PER_BYTE;
    left_shift = BITS_PER_BYTE - right_shift;
    extra = size % BITS_PER_BYTE;
    size /= BITS_PER_BYTE;
    if( extra ) {
        memset( dst, 0, size + 1 );
    } else {
        memset( dst, 0, size );
    }
    src += ( start / BITS_PER_BYTE );
    for( i = 0; i < size; i++ ) {
        dst[i] = ( src[i] >> right_shift ) | ( src[i + 1] << left_shift );
    }

    if( extra ) {
        mask = 0xFF;
        mask >>= ( BITS_PER_BYTE - extra );
        dst[size] |= ( ( src[size] >> right_shift ) & mask );
    }
}

/*
 * get bits from byte aligned src and put in unbyte aligned dst
 */
void BitPut(unsigned_8 *dst, unsigned short start, unsigned_8 *src,
        unsigned char size )
{
    unsigned_8      lmask;
    unsigned_8      rmask;
    int             rshift;
    int             extra;
    int             i;

    dst += ( start / BITS_PER_BYTE );
    start %= BITS_PER_BYTE;
    rshift = BITS_PER_BYTE - start;
    lmask = rmask = 0xFF;
    lmask <<= start;
    if( size < rshift ){
        rmask >>= ( rshift - size );
        size = 0;
    } else {
        size -= rshift;
    }
    *dst &= ~( lmask & rmask );
    *dst |= ( rmask & ( *src << start ) );
    if( size != 0 ) {
        dst++;
        extra = size % BITS_PER_BYTE;
        size /= BITS_PER_BYTE;
        for( i = 0; i < size; i++ ) {
            dst[i] = ( ( src[i] >> rshift ) | ( src[i + 1] << start ) );
        }
        if( extra != 0 ){
            rmask = 0xFF;
            rmask >>= ( BITS_PER_BYTE - extra );
            dst[size] &= ~rmask;
            dst[size] |= ( rmask & ( src[size] >> rshift ) );
        }
    }
}
