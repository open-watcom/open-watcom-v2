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
* Description:  RISC style memchr().
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include "riscstr.h"


void *memchr( const void *buf, int c, size_t n )
/**********************************************/
{
    UINT *              dw;
    UINT                dword, cdword, tmpdword;
    INT                 len = 0;
    int                 offset = OFFSET(buf);
    INT                 cShl8, cShl16, cShl24;
    char *              bufP = (char*)buf;

    if( n == 0 )  return( NULL );

    /*** Scan any bytes up to a 4-byte alignment ***/
    while( offset != 0  &&  n >= 1 ) {
        if( *bufP == c )  return( bufP );
        bufP++;
        offset = (offset+1) & 0x03;     /* offset = (offset+1) % 4 */
        n--;
        len++;
    }

    /*** Initialize locals ***/
    c &= BYTE1;
    cShl8 = c << 8;
    cShl16 = c << 16;
    cShl24 = c << 24;
    cdword = cShl24 | cShl16 | cShl8 | c;
    dw = (UINT*)bufP;                   /* already rounded */

    /*** Scan in aligned 4-byte groups ***/
    for( ;; ) {
        dword = *dw++;
        if( n < BYTES_PER_WORD )  break;
        tmpdword = dword ^ cdword;
        if( GOT_NIL(tmpdword) ) {
            if( !CHR1(tmpdword) )  return( (char*)buf+len );
            if( !CHR2(tmpdword) )  return( (char*)buf+len+1 );
            if( !CHR3(tmpdword) )  return( (char*)buf+len+2 );
            /* otherwise */        return( (char*)buf+len+3 );
        }
        len += BYTES_PER_WORD;
        n -= BYTES_PER_WORD;
    }

    /*** Scan the last byte(s) in the string ***/
    if( n >= 1 ) {
        tmpdword = CHR1(dword);
        if( tmpdword == c )  return( (char*)buf+len );
        n--;
    }

    if( n >= 1 ) {
        tmpdword = CHR2(dword);
        if( tmpdword == cShl8 )  return( (char*)buf+len+1 );
        n--;
    }

    if( n >= 1 ) {
        tmpdword = CHR3(dword);
        if( tmpdword == cShl16 )  return( (char*)buf+len+2 );
    }

    return( NULL );
}
