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
* Description:  Implementation of strcpy() for RISC architectures.
*
****************************************************************************/


#include "widechar.h"
#include <string.h>
#include "riscstr.h"


CHAR_TYPE *__F_NAME(strcpy,wcscpy)( CHAR_TYPE *dest, const CHAR_TYPE *src )
/*************************************************************************/
{
    CHAR_TYPE           *destStart = dest;
    UINT                *dwSrc;
    UINT                *dwDest;
    UINT                dword, tmpdword;
    CHAR_TYPE           ch;
    int                 shl, shr;
    int                 offset = OFFSET(dest);

#ifdef __WIDECHAR__
    if( offset % 2  ||  OFFSET(src) % 2 )
        return( __simple_wcscpy( dest, src ) );
#endif

    /*** Copy any unaligned bytes at the start (align dest pointer) ***/
    while( offset != 0 ) {
        ch = *src;
        if( ch == NULLCHAR )
            return( destStart );
        *dest++ = ch;
        src++;
        offset = MOD_BYTES_PER_WORD(offset + CHARSIZE); /* find new offset */
    }

    /*** Initialize locals ***/
    dwSrc = ROUND(src);
    dwDest = (UINT*)dest;               /* already rounded */
    shr = OFFSET(src) * 8;
    shl = INT_SIZE - shr;
    if( shr != 0 ) {
        dword = *dwSrc++;
    }

    /*** Copy in aligned 4-byte groups ***/
    for( ;; ) {
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

    /*** There's a null char somewhere in the last dword ***/
#ifdef __WIDECHAR__
    if( !CHR1(tmpdword) ) {                 /* copy 1 char */
        dword = *dwDest & ~(CHR1MASK);
        tmpdword &= (CHR1MASK);
        *dwDest = dword | tmpdword;
    } else {                                /* copy 2 chars */
        *dwDest = tmpdword;
    }
#else
    if( CHR1(tmpdword) == '\0' ) {          /* copy 1 byte */
        dword = *dwDest & ~(BYTE1);
        tmpdword &= (BYTE1);
        *dwDest = dword | tmpdword;
    } else if( CHR2(tmpdword) == '\0' ) {   /* copy 2 bytes */
        dword = *dwDest & ~(BYTE1|BYTE2);
        tmpdword &= (BYTE1|BYTE2);
        *dwDest = dword | tmpdword;
    } else if( CHR3(tmpdword) == '\0' ) {   /* copy 3 bytes */
        dword = *dwDest & ~(BYTE1|BYTE2|BYTE3);
        tmpdword &= (BYTE1|BYTE2|BYTE3);
        *dwDest = dword | tmpdword;
    } else {                                /* copy 4 bytes */
        *dwDest = tmpdword;
    }
#endif

    return( destStart );
}
