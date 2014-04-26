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
#include <string.h>
#include "riscstr.h"


void *memset( void *buf, int c, size_t n )
/****************************************/
{
#if USE_INT64
    RISC_DATA_LOCALREF;
#endif
    char *              bufStart = buf;
    char *              dest = buf;
    UINT *              dw;
    UINT                dword, cdword;
    int                 offset = OFFSET(dest);

    /*** Set any bytes up to a 4-byte alignment ***/
    while( offset > 0 ) {
        if( n >= 1 ) {
            *dest++ = c;
            n--;
        } else {
            return( bufStart );
        }
        offset = (offset + 1) & (sizeof(UINT) - 1);
    }

    /*** Initialize locals ***/
    c &= BYTE1;
    cdword = (c<<24) | (c<<16) | (c<<8) | c;
    dw = (UINT*)dest;                           /* already rounded */

    /*** Write in aligned 4-byte groups ***/
    for( ;; ) {
        if( n < BYTES_PER_WORD )  break;
        *dw++ = cdword;
        n -= BYTES_PER_WORD;

        if( n < BYTES_PER_WORD )  break;
        *dw++ = cdword;
        n -= BYTES_PER_WORD;
    }

    /*** Write in last dword ***/
    if( n == 1 ) {                              /* copy 1 byte */
        dword = *dw & ~(BYTE1);
        cdword &= (BYTE1);
        *dw = dword | cdword;
    } else if( n == 2 ) {                       /* copy 2 bytes */
        dword = *dw & ~(BYTE1|BYTE2);
        cdword &= (BYTE1|BYTE2);
        *dw = dword | cdword;
    } else if( n == 3 ) {                       /* copy 3 bytes */
        dword = *dw & ~(BYTE1|BYTE2|BYTE3);
        cdword &= (BYTE1|BYTE2|BYTE3);
        *dw = dword | cdword;
    }

    return( bufStart );
}
