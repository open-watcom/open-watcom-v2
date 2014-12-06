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


#include "farptrs.h"
#include "dbgdefn.h"
#include "tinyio.h"


typedef struct {
    char        chain;  /* 'M' memory block, 'Z' is last in chain */
    unsigned    owner;  /* 0x0000 ==> free, otherwise psp address */
    unsigned    size;   /* in paragraphs, not including header  */
} dos_mem_block;

#define CHECK_FILE  "___CHK.MEM"
#define MEMORY_BLOCK 'M'
#define END_OF_CHAIN 'Z'
#define NEXT_BLOCK( curr )  MK_FP( (FP_SEG( curr ) + (curr)->size + 1), 0 )
#define PUT_ITEM( item ) (TinyFarWrite( hdl, &(item), \
                         sizeof( item ) ) == sizeof( item ))

static      char *ChkFile;

static void Cleanup()
{
    TinyDelete( ChkFile );
}


bool CheckPointMem( unsigned max, char *f_buff )
{
    dos_mem_block   __far *mem;
    dos_mem_block   __far *start;
    dos_mem_block   __far *end;
    dos_mem_block   __far *next;
    dos_mem_block   __far *chk;
    tiny_ret_t      ret;
    tiny_handle_t   hdl;
    unsigned        size;
    unsigned        bytes;
    unsigned        psp;
    char            *p;

    if( max == 0 ) return( FALSE );
    ChkFile = f_buff;
    psp = TinyGetPSP();
    start = MK_FP( psp - 1, 0 );
    while( start->owner == psp ) {
        if( start->chain == END_OF_CHAIN ) return( FALSE );
//        start = NEXT_BLOCK( start );
        start = MK_FP( (FP_SEG( start ) + (start)->size + 1), 0 );
    }
    mem = start;
    for( ;; ) {
        if( mem->owner == 0 && mem->size >= max ) return( FALSE );
        if(  mem->chain == END_OF_CHAIN ) break;
//        mem = NEXT_BLOCK( mem );
        mem = MK_FP( (FP_SEG( mem ) + (mem)->size + 1), 0 );
    }
//    end = NEXT_BLOCK( mem );
      end = MK_FP( (FP_SEG( mem ) + (mem)->size + 1), 0 );
    size = FP_SEG( end ) - FP_SEG( start );
    if( size < 0x1000 ) return( FALSE );
    *f_buff++ = TinyGetCurrDrive() + 'A';
    *f_buff++ = ':';
    *f_buff++ = '\\';
    ret = TinyFarGetCWDir( (char __far *)f_buff, 0 );
    if( ret < 0 ) return( FALSE );
    while( *f_buff != 0 ) ++f_buff;
    if( f_buff[-1] == '\\' ) {
        --f_buff;
    } else {
        *f_buff++ = '\\';
    }
    for( p = CHECK_FILE; *f_buff = *p; ++p, ++f_buff ) {}
    ret = TinyCreate( ChkFile, TIO_NORMAL );
    if( ret < 0 ) return( FALSE );
    hdl = ret;
    if( size > max ) size = max;
    chk = MK_FP( FP_SEG( end ) - size - 1, 0 );
    mem = start;
    for( ;; ) {
//        next = NEXT_BLOCK( mem );
        next = MK_FP( (FP_SEG( mem ) + (mem)->size + 1), 0 );
        if( FP_SEG( next ) > FP_SEG( chk ) ) break;
        mem = next;
    }
    if( !PUT_ITEM( mem ) || !PUT_ITEM( *mem ) || !PUT_ITEM( chk ) ) {
        TinyClose( hdl );
        Cleanup();
        return( FALSE );
    }
    next = chk;
    while( FP_SEG( next ) < FP_SEG( end ) ) {
        size = FP_SEG( end ) - FP_SEG( next );
        if( size >= 0x1000 ) size = 0x0800;
        bytes = size << 4;
        if( TinyFarWrite( hdl, next, bytes) != bytes) {
            TinyClose( hdl );
            Cleanup();
            return( FALSE );
        }
        next = MK_FP( FP_SEG( next ) + size, 0 );
    }
    TinyClose( hdl );
    mem->chain = MEMORY_BLOCK;
    mem->size = FP_SEG( chk ) - FP_SEG( mem ) - 1;
    chk->size = FP_SEG( end ) - FP_SEG( chk ) - 1;
    chk->chain = END_OF_CHAIN;
    chk->owner = 0;
    return( TRUE );
}


void CheckPointRestore()
{
    dos_mem_block   __far *chk;
    tiny_ret_t      ret;
    tiny_handle_t   hdl;

    ret = TinyOpen( ChkFile, TIO_READ );
    if( ret < 0 ) return;
    hdl = ret;

    TinyFarRead( hdl, &chk, sizeof( chk ) );
    TinyFarRead( hdl, chk, sizeof( *chk ) );
    TinyFarRead( hdl, &chk, sizeof( chk ) );
    while( TinyFarRead( hdl, chk, 0x8000 ) == 0x8000 ) {
        chk = MK_FP( FP_SEG( chk ) + 0x800, 0 );
    }
    TinyClose( hdl );
    Cleanup();
}
