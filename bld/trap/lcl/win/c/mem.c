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


#include <stdio.h>
#include <dos.h>
#include "stdwin.h"

void PushAll( void );
void PopAll( void );
#pragma aux PushAll = 0x60;
#pragma aux PopAll = 0x61 modify[dx ax];

/*
 * ReadMem - read some memory, using toolhelp or wdebug.386
 */
DWORD ReadMem( WORD sel, DWORD off, LPSTR buff, DWORD size )
{
    DWORD       rc;

    if( WDebug386 ) {
        rc = CopyMemory( FP_SEG(buff), FP_OFF(buff), sel, off,  size );
    } else {
        if( DebugeeTask == NULL ) return( 0 );
        rc = MemoryRead( sel, off, buff, size );
    }
    return( rc );

} /* ReadMem */

/*
 * WriteMem - write some memory, using toolhelp or wdebug.386
 */
DWORD WriteMem( WORD sel, DWORD off, LPSTR buff, DWORD size )
{
    DWORD       rc;

    if( WDebug386 ) {
        rc = CopyMemory( sel, off, FP_SEG(buff), FP_OFF(buff), size );
    } else {
        if( DebugeeTask == NULL ) return( 0 );
        rc = MemoryWrite( sel, off, buff, size );
    }
    return( rc );

} /* WriteMem */

unsigned ReqRead_mem( void )
{
    read_mem_req        *acc;
    LPVOID              data;
    unsigned            len;

    acc = GetInPtr(0);
    data = GetOutPtr(0);

    len = ReadMem( acc->mem_addr.segment, acc->mem_addr.offset, data, acc->len );
    return( len );
}

unsigned ReqWrite_mem( void )
{
    DWORD               len;
    LPVOID              data;
    write_mem_req       *acc;
    write_mem_ret       *ret;

    acc = GetInPtr(0);
    data = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr(0);
    len = GetTotalSize() - sizeof( *acc );

    ret->len = WriteMem( acc->mem_addr.segment, acc->mem_addr.offset, data, len );
    return( sizeof( *ret ) );
}

unsigned ReqChecksum_mem( void )
{
    DWORD       offset;
    WORD        length,value;
    DWORD       sum;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);

    length = acc->len;
    sum = 0;
    if( DebugeeTask != NULL ) {
        offset = acc->in_addr.offset;
        while( length != 0 ) {
            ReadMem( acc->in_addr.segment, offset, (LPVOID)&value, 2 );
            sum += value & 0xff;
            offset++;
            length--;
            if( length != 0 ) {
                sum += value >> 8;
                offset++;
                length--;
            }
        }
    }
    ret->result = sum;
    return( sizeof( *ret ) );
}
