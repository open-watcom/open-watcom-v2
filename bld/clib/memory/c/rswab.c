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
#include <string.h>
#include "riscstr.h"


void swab( char *src, char *dest, int n )
/***************************************/
{
    UINT *              dwSrc = ROUND(src);
    UINT *              dwDest = ROUND(dest);
    int                 shlSrc, shrSrc, shlDest, shrDest;
    UINT                dwordSrc, tmpdwordSrc, dwordDest, tmpdwordDest;
    int                 leftovers;
    char                ch;

    /*** Initialize locals ***/
    n = n / 2;                          /* number of pairs to process */
    if( n <= 0 )  return;
    shrSrc = OFFSET(src) * 8;           /* shift right = offset * 8 */
    shrDest = OFFSET(dest) * 8;
    shlSrc = INT_SIZE - shrSrc;         /* shift left = 32 - shift right */
    shlDest = INT_SIZE - shrDest;

    if( shrSrc != 0 ) {
        dwordSrc = *dwSrc++;
    }
    if( shrDest != 0 ) {
        dwordDest = *dwDest;
    }

    /*** Swap in 4-byte groups ***/
    for( leftovers=0; n>=BYTES_PER_WORD/2; n-=BYTES_PER_WORD/2 ) {
        /*** Extract the next few characters from the source string ***/
        if( shrSrc == 0 ) {
            tmpdwordSrc = *dwSrc++;
        } else {
            tmpdwordSrc = dwordSrc >> shrSrc;
            dwordSrc = *dwSrc++;
            tmpdwordSrc |= (dwordSrc << shlSrc);
        }
        tmpdwordSrc = SWAP_BYTES(tmpdwordSrc);

        /*** Write out the bytes, swapped ***/
        if( shrDest == 0 ) {
            *dwDest++ = tmpdwordSrc;
        } else {
            tmpdwordDest = (dwordDest<<shlDest) >> shlDest;
            dwordDest = tmpdwordSrc;
            tmpdwordDest |= dwordDest << shrDest;
            *dwDest++ = tmpdwordDest;
            dwordDest >>= shlDest;
            leftovers = 1;
        }

        src += CHARS_PER_WORD;
        dest += CHARS_PER_WORD;
    }

    /*** Handle any leftovers from the previous loop ***/
    if( leftovers ) {
        tmpdwordDest = ((*dwDest)>>shrDest) << shrDest;
        tmpdwordDest |= dwordDest;
        *dwDest = tmpdwordDest;
    }

    /*** Handle any trailing bytes ***/
    while( n > 0 ) {
        ch = src[0];
        dest[0] = src[1];
        dest[1] = ch;
        src += 2;
        dest += 2;
        n--;
    }
}
