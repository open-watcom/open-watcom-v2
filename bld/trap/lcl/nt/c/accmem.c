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
#include "stdnt.h"

/*
 * getRealBase - get real base address, based on segment/offset
 */
static DWORD getRealBase( WORD seg, DWORD base, DWORD *limit )
{
#if defined( MD_axp ) || defined( MD_ppc )

    *limit = -( DWORD )1;
    return( base );

#elif defined( MD_x86 )

    DWORD       realbase;
    LDT_ENTRY   ldt;
    thread_info *ti;
    DWORD       lim;
    DWORD       selbase;

    if( seg != FlatDS && seg != FlatCS ) {
        ti = FindThread( DebugeeTid );
        if( GetThreadSelectorEntry( ti->thread_handle, seg, &ldt ) ) {
            lim = 1 + ( DWORD ) ldt.LimitLow +
                ( ( DWORD ) ldt.HighWord.Bits.LimitHi << 16L );
            if( ldt.HighWord.Bits.Granularity ) {
                lim *= 0x1000L;
            }
            if( !ldt.HighWord.Bits.Default_Big ) {
                base = ( DWORD ) ( WORD ) base;
            }
            selbase = ( DWORD ) ldt.BaseLow +
                ( ( DWORD ) ldt.HighWord.Bytes.BaseMid << 16L ) +
                ( ( DWORD ) ldt.HighWord.Bytes.BaseHi << 24L );
            realbase = base + selbase;
            *limit = lim + selbase;
        } else {
            realbase = 0;
            *limit = 0;
        }
    } else {
        *limit = ( DWORD ) - 1L;
        realbase = base;
    }
    return( realbase );

#else

    #error getRealBase not configured

#endif
}

/*
 * ReadMem - read some memory
 */
DWORD ReadMem( WORD seg, DWORD base, LPVOID buff, DWORD size )
{
    LONG        bytes;
    DWORD       limit;
#ifdef DEBUGGING_THIS_DAMN_WIN95_PROBLEM
    static bool first = TRUE;
    DWORD       oldbase;
#endif

    if( DebugeePid == 0 ) {
        return( 0 );
    }
#ifdef DEBUGGING_THIS_DAMN_WIN95_PROBLEM
    oldbase = base;
#endif
    base = getRealBase( seg, base, &limit );
    if( base > limit ) {
        limit = base;
    }
    if( base + size < base ) { // wants to wrap segment
        size = ( ( DWORD ) ~0 ) - base;
    }
    if( limit != ( DWORD ) - 1L ) {
        if( base + size > limit ) {
            size = limit - base;
        }
    }
    if( size > 2048 )
        size = 2048;
#ifdef DEBUGGING_THIS_DAMN_WIN95_PROBLEM
    FILE        *io;
    io = fopen( "t.t", "a+" );
    fprintf( io, "%4.4x:%8.8x, base=%8.8x, limit=%8.8x, size=%d\n",
                seg, oldbase,
                base, limit, size );
    fclose( io );
#endif
    ReadProcessMemory( ProcessInfo.process_handle, ( LPVOID ) base, buff,
                        size, ( LPDWORD )&bytes );
#ifdef DEBUGGING_THIS_DAMN_WIN95_PROBLEM
    if( first ) {
        remove( "t.t" );
        first = FALSE;
    }
    if( bytes != size ) {
        io = fopen( "t.t", "a+" );
        fprintf( io, "got=%d\n", bytes );
        fclose( io );
    }
#endif
    if( bytes < 0 ) {
        bytes = 0;
    }
    return( bytes );
}

/*
 * WriteMem - write some memory
 */
DWORD WriteMem( WORD seg, DWORD base, LPVOID buff, DWORD size )
{
    LONG    bytes;
    DWORD   limit;

    if( DebugeePid == 0 ) {
        return( 0 );
    }
    base = getRealBase( seg, base, &limit );
    if( limit != ( DWORD ) - 1L ) {
        if( base + size > limit ) {
            size = limit - base;
        }
    }
    WriteProcessMemory( ProcessInfo.process_handle, ( LPVOID )base, buff,
                        size, ( LPDWORD ) & bytes );
    GetLastError();
    if( bytes < 0 ) {
        bytes = 0;
    }
    return( bytes );

}

unsigned ReqRead_mem( void )
{
    WORD            seg;
    DWORD           offset;
    DWORD           length;
    LPSTR           data;
    read_mem_req    *acc;

    acc = GetInPtr( 0 );

    if( DebugeePid == 0 ) {
        return( 0 );
    }

    seg = acc->mem_addr.segment;
    offset = acc->mem_addr.offset;
    length = acc->len;
    data = ( LPSTR ) GetOutPtr( 0 );

    length = ReadMem( seg, offset, data, length );
    return( length );
}

unsigned ReqWrite_mem( void )
{
    WORD            seg;
    DWORD           offset;
    DWORD           length;
    LPSTR           data;
    write_mem_req   *acc;
    write_mem_ret   *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ret->len = 0;
    if( DebugeePid == 0 ) {
        return( sizeof( *ret ) );
    }

    seg = acc->mem_addr.segment;
    offset = acc->mem_addr.offset;
    length = GetTotalSize() - sizeof( *acc );
    data = ( LPSTR ) GetInPtr( sizeof( *acc ) );

    ret->len = WriteMem( seg, offset, data, length );
    return( sizeof( *ret ) );
}

unsigned ReqChecksum_mem( void )
{
    DWORD               offset;
    WORD                length;
    WORD                value;
    WORD                segment;
    DWORD               sum;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    length = acc->len;
    sum = 0;
    if( DebugeePid ) {
        offset = acc->in_addr.offset;
        segment = acc->in_addr.segment;
        while( length != 0 ) {
            ReadMem( segment, offset, &value, sizeof( value ) );
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
