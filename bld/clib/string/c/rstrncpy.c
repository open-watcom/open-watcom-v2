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
* Description:  Implementation of strncpy() for RISC architectures.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <string.h>
#include "riscstr.h"


CHAR_TYPE *__F_NAME(strncpy,wcsncpy)( CHAR_TYPE *dest, const CHAR_TYPE *src,
                                      size_t n )
/**************************************************************************/
{
    CHAR_TYPE           *destStart = dest;
    UINT                *dwSrc;
    UINT                *dwDest;
    UINT                dword, tmpdword;
    int                 shl, shr;
    int                 offset = OFFSET(dest);

#ifdef __WIDECHAR__
    if( offset % 2  ||  OFFSET(src) % 2 )
        return( __simple_wcsncpy( dest, src, n ) );
#endif

    if( n == 0 )  return( destStart );

    /*** Copy any unaligned bytes at the start (align dest pointer) ***/
    while( offset != 0  &&  n >= 1 ) {
        *dest++ = *src++;
        offset = MOD_BYTES_PER_WORD(offset+CHARSIZE); /* find new offset */
        n--;
    }

    /*** Initialize locals ***/
    dwSrc = ROUND(src);                 /* round down to dword */
    dwDest = (UINT*)dest;               /* already rounded */
    shr = OFFSET(src) * 8;
    shl = INT_SIZE - shr;
    if( shr != 0 ) {
        dword = *dwSrc++;
    }

    /*** Copy in aligned 4-byte groups ***/
    for( ; n >= CHARS_PER_WORD; n -= CHARS_PER_WORD ) {
        /*** Form the dword containing the next four characters ***/
        if( shr == 0 ) {
            tmpdword = *dwSrc++;
        } else {
            tmpdword = dword >> shr;
            dword = *dwSrc++;
            tmpdword |= (dword << shl);
        }

        /*** Copy the characters unless the string has ended ***/
        if( GOT_NIL(tmpdword) ) {
            break;
        } else {
            *dwDest++ = tmpdword;
        }
    }

    /*** Either end of string reached or n is too small ***/
    dest = (CHAR_TYPE*)dwDest;
    if( shr == 0 ) {
        src = (CHAR_TYPE*)dwSrc;
    } else {
        src = (CHAR_TYPE*)dwSrc - (shl/8);
    }
    while( n >= 1  &&  *src != '\0' ) {
        *dest++ = *src++;
        n--;
    }

    /*** If necessary, pad with more null bytes ***/
    if( n > 0 ) {
        memset( dest, '\0', n * CHARSIZE );
    }

    return( destStart );
}
