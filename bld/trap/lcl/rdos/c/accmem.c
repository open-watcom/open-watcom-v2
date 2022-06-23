/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


trap_retval TRAP_CORE( Read_mem )( void )
{
    read_mem_req        *acc;
    struct TDebug       *obj;

    acc = GetInPtr( 0 );
    obj = GetCurrentDebug();
    if( obj != NULL )
        return( ReadMem( obj, acc->mem_addr.segment, acc->mem_addr.offset, GetOutPtr( 0 ), acc->len ) );
    return( 0 );
}

trap_retval TRAP_CORE( Write_mem )( void )
{
    void                *data;
    write_mem_req       *acc;
    write_mem_ret       *ret;
    struct TDebug       *obj;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->len = 0;
    data = GetInPtr( sizeof( *acc ) );
    obj = GetCurrentDebug();
    if( obj != NULL )
        ret->len = WriteMem( obj, acc->mem_addr.segment, acc->mem_addr.offset,
                            data, GetTotalSizeIn() - sizeof( *acc ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Checksum_mem )( void )
{
    DWORD               offset;
    WORD                segment;
    size_t              len;
    WORD                value;
    DWORD               sum;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;
    struct TDebug       *obj;

    sum = 0;
    obj = GetCurrentDebug();
    if( obj != NULL ) {
        acc = GetInPtr( 0 );
        offset = acc->in_addr.offset;
        segment = acc->in_addr.segment;
        for( len = acc->len; len > 0; ) {
            ReadMem( obj, segment, offset, &value, sizeof( value ) );
            sum += value & 0xff;
            offset++;
            len--;
            if( len > 0 ) {
                sum += value >> 8;
                offset++;
                len--;
            }
        }
    }
    ret = GetOutPtr( 0 );
    ret->result = sum;
    return( sizeof( *ret ) );
}
