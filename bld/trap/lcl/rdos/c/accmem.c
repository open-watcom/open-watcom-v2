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
#include <stdlib.h>
#include "stdrdos.h"
#include "debug.h"


trap_retval ReqRead_mem( void )
{
    void                *data;
    read_mem_req        *acc;
    struct TDebug       *obj;
    struct TDebugThread *thread = 0;

    acc = GetInPtr( 0 );
    data = ( void * ) GetOutPtr( 0 );

    obj = GetCurrentDebug();
        if (obj)
        thread = obj->CurrentThread;

    if( thread )
        return( ReadMem(    thread, 
                            acc->mem_addr.segment, 
                            acc->mem_addr.offset, 
                            data,
                            acc->len ) );
    else
        return( 0 );
}

trap_retval ReqWrite_mem( void )
{
    void                *data;
    int                 len;
    write_mem_req       *acc;
    write_mem_ret       *ret;
    struct TDebug       *obj;
    struct TDebugThread *thread = 0;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    len = GetTotalSize() - sizeof( *acc );
    data = ( void * ) GetInPtr( sizeof( *acc ) );

    ret->len = 0;

    obj = GetCurrentDebug();
        if (obj)
        thread = obj->CurrentThread;

    if( thread )
        ret->len = WriteMem(thread, 
                            acc->mem_addr.segment, 
                            acc->mem_addr.offset, 
                            data,
                            len );

    return( sizeof( *ret ) );
}

trap_retval ReqChecksum_mem( void )
{
    long                offset;
    int                 segment;
    WORD                length;
    WORD                value;
    DWORD               sum;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;
    struct TDebug       *obj;
    struct TDebugThread *thread = 0;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    length = acc->len;
    sum = 0;

    obj = GetCurrentDebug();
    if( obj )
        thread = obj->CurrentThread;

    if( thread ) {
        offset = acc->in_addr.offset;
        segment = acc->in_addr.segment;
        while( length != 0 ) {
            ReadMem( thread, segment, offset, (char *)&value, sizeof( value ) );
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
