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


#include <string.h>
#include "riscstr.h"


void *memcpy( void *dest, const void *src, size_t n )
/***************************************************/
{
    void *              destStart = dest;
    UINT *              dwSrc;
    UINT *              dwDest;
    UINT                dword, tmpdword;
    int                 shl, shr;
    int                 offset = OFFSET(dest);
    char *              destP = (char*)dest;
    char *              srcP = (char*)src;

    if( n <= 0 )  return( destStart );

    /*** Copy any unaligned bytes at the start ***/
    while( offset != 0  &&  n >= 1 ) {
        *destP++ = *srcP++;
        offset = (offset+1) & 0x03;     /* offset = (offset+1) % 4 */
        n--;
    }

    /*** Initialize locals ***/
    dwSrc = ROUND(srcP);
    dwDest = (UINT*)destP;                      /* already rounded */
    shr = OFFSET(srcP) * 8;
    shl = INT_SIZE - shr;
    if( shr != 0 ) {
        dword = *dwSrc++;
    }

    /*** Copy in aligned 4-byte groups ***/
    for( ;; ) {
        /*** Form the dword containing the next four bytes ***/
        if( shr == 0 ) {
            tmpdword = *dwSrc++;
        } else {
            tmpdword = dword >> shr;
            dword = *dwSrc++;
            tmpdword |= (dword << shl);
        }
        if( n < BYTES_PER_WORD )  break;

        /*** Copy the bytes ***/
        *dwDest++ = tmpdword;
        n -= BYTES_PER_WORD;
    }

    /*** Copy any remaining unaligned bytes at the end ***/
    if( n == 1 ) {                              /* copy 1 byte */
        dword = *dwDest & ~(BYTE1);
        tmpdword &= (BYTE1);
        *dwDest = dword | tmpdword;
    } else if( n == 2 ) {                       /* copy 2 bytes */
        dword = *dwDest & ~(BYTE1|BYTE2);
        tmpdword &= (BYTE1|BYTE2);
        *dwDest = dword | tmpdword;
    } else if( n == 3 ) {                       /* copy 3 bytes */
        dword = *dwDest & ~(BYTE1|BYTE2|BYTE3);
        tmpdword &= (BYTE1|BYTE2|BYTE3);
        *dwDest = dword | tmpdword;
    }

    return( destStart );
}
