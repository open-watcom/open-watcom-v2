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
* Description:  Check DOS memory blocks for consistency.
*
****************************************************************************/


#include <stdlib.h>
#include <i86.h>
#include "bool.h"
#include "tinyio.h"
#include "doschk.h"


#define FILE_BUFFER_SIZE    0x8000

#define MEMORY_BLOCK        'M'
#define END_OF_CHAIN        'Z'
#define MCB_PTR(curr)       ((dos_mem_block __based( curr ) *)0)
#define NEXT_MCB(curr)      (curr + MCB_PTR( curr )->size + 1)
#define PUT_ITEM(item)                     (TinyFarWrite( hdl, &(item), sizeof( item ) ) == sizeof( item ))

#include "pushpck1.h"
typedef struct {
    char            chain;  /* 'M' memory block, 'Z' is last in chain */
    unsigned short  owner;  /* 0x0000 ==> free, otherwise psp address */
    unsigned short  size;   /* in paragraphs, not including header  */
} dos_mem_block;
#include "poppck.h"

static char             *ChkFile;

static void Cleanup( tiny_handle_t hdl )
{
    TinyClose( hdl );
    TinyDelete( ChkFile );
}

static tiny_handle_t chkOpen( char *f_buff, unsigned psp )
{
    tiny_ret_t      rc;
    char            c;
    int             i;
    tiny_handle_t   hdl;

    *f_buff++ = TinyGetCurrDrive() + 'A';
    *f_buff++ = ':';
    *f_buff++ = '\\';
    rc = TinyFarGetCWDir( f_buff, 0 );
    if( TINY_ERROR( rc ) )
        return( -1 );
    while( *f_buff != 0 )
        ++f_buff;
    if( f_buff[-1] == '\\' ) {
        --f_buff;
    } else {
        *f_buff++ = '\\';
    }
    *f_buff++ = 'S';
    *f_buff++ = 'W';
    *f_buff++ = 'A';
    *f_buff++ = '0';
    for( i = 0; i < 4; ++i ) {
        if( (c = (psp & 0xF)) > 9 )
            c += 'A' - '9' - 1;
        *f_buff++ = c + '0';
        psp >>= 4;
    }
    *f_buff = '\0';
    hdl = -1;
    for( c = 'A'; c <= 'Z'; ++c ) {
        f_buff[-6] = c;
        if( TINY_ERROR( TinyAccess( ChkFile, 0 ) ) ) {
            rc = TinyCreateNew( ChkFile, TIO_NORMAL );
            if( TINY_OK( rc ) ) {
                hdl = TINY_INFO( rc );
                break;
            }
        }
    }
    return( hdl );
}

static void chkClose( tiny_handle_t hdl )
{
    TinyClose( hdl );

} /* chkClose */

static bool chkWrite( tiny_handle_t hdl, __segment buff, unsigned *size )
{
    tiny_ret_t      rc;
    unsigned        bytes;

    if( *size >= 0x1000 ) {
        *size = FILE_BUFFER_SIZE >> 4;
    }
    bytes = *size << 4;
    rc = TinyFarWrite( hdl, MK_FP( buff, 0 ), bytes );
    if( TINY_OK( rc ) && TINY_INFO( rc ) == bytes ) {
        return( true );
    }
    return( false );

} /* chkWrite */

static bool chkRead( tiny_handle_t hdl, __segment *buff )
{
    tiny_ret_t      rc;

    rc = TinyFarRead( hdl, MK_FP( *buff, 0 ), FILE_BUFFER_SIZE );
    if( TINY_OK( rc ) && TINY_INFO( rc ) == FILE_BUFFER_SIZE ) {
        *buff += FILE_BUFFER_SIZE >> 4;
        return( true );
    }
    return( false );

} /* chkRead */

int CheckPointMem( unsigned max, char *f_buff )
{
    __segment       mem;
    __segment       start;
    __segment       end;
    __segment       next;
    __segment       chk;
    tiny_handle_t   hdl;
    unsigned        size;
    unsigned        psp;

    if( max == 0 )
        return( 0 );
    ChkFile = f_buff;
    psp = TinyGetPSP();
    for( start = psp - 1; MCB_PTR( start )->owner == psp; start = NEXT_MCB( start ) ) {
        if( MCB_PTR( start )->chain == END_OF_CHAIN ) {
            return( 0 );
        }
    }
    for( mem = start; ; mem = NEXT_MCB( mem ) ) {
        if( MCB_PTR( mem )->owner == 0 && MCB_PTR( mem )->size >= max )
            return( 0 );
        if(  MCB_PTR( mem )->chain == END_OF_CHAIN ) {
            break;
        }
    }
    end = NEXT_MCB( mem );
    size = end - start;
    if( size < 0x1000 )
        return( 0 );

    if( size > max )
        size = max;
    chk = end - size - 1;
    for( mem = start; ; mem = next ) {
        next = NEXT_MCB( mem );
        if( next > chk ) {
            break;
        }
    }

    hdl = chkOpen( f_buff, psp );
    if( hdl == -1 ) {
        return( 0 );
    }

    if( !PUT_ITEM( mem ) || !PUT_ITEM( *MCB_PTR( mem ) ) || !PUT_ITEM( chk ) ) {
        Cleanup( hdl );
        return( 0 );
    }
    for( next = chk; next < end; next += size ) {
        size = end - next;
        if( !chkWrite( hdl, next, &size ) ) {
            Cleanup( hdl );
            return( 0 );
        }
    }
    chkClose( hdl );

    MCB_PTR( mem )->chain = MEMORY_BLOCK;
    MCB_PTR( mem )->size = chk - mem - 1;
    MCB_PTR( chk )->size = end - chk - 1;
    MCB_PTR( chk )->chain = END_OF_CHAIN;
    MCB_PTR( chk )->owner = 0;
    return( 1 );
}

void CheckPointRestore( void )
{
    __segment       chk;
    tiny_ret_t      rc;
    tiny_handle_t   hdl;

    rc = TinyOpen( ChkFile, TIO_READ );
    if( TINY_OK( rc ) ) {
        hdl = TINY_INFO( rc );
        TinyFarRead( hdl, &chk, sizeof( chk ) );
        TinyFarRead( hdl, MCB_PTR( chk ), sizeof( *MCB_PTR( chk ) ) );
        TinyFarRead( hdl, &chk, sizeof( chk ) );
        while( chkRead( hdl, &chk ) )
            ;
        Cleanup( hdl );
    }
}
