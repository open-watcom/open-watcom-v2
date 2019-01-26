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


#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <dos.h>
#include "bool.h"
#include "tinyio.h"
#include "heap.h"
#include "dbgdefn.h"
#include "dbgdata.h"


#define ROUND_DOWN_SIZE_TO_PARA( __x )  ((__x)>>4)
#define ROUND_UP_SIZE_TO_PARA( __x )    (((__x)+15)>>4)

#define FIRST_FRL(s)    ((freelist __based(s) *)(BHEAP(s) + 1))

extern __segment        LastSeg;
extern void             *SyMemBeg;
extern void             *SyMemEnd;

#pragma aux __AllocSeg __modify[];
__segment __AllocSeg( unsigned int amount )
{
    unsigned    num_of_paras;       /* number of paragraphs desired   */
    unsigned    heaplen;
    __segment   seg;
    tiny_ret_t  rc;

    /*               heapinfo + frl + frl,                    end tags */
    if( amount > - ( sizeof( heapblk ) + sizeof( freelist ) + TAG_SIZE * 2 ) ) {
        return( _NULLSEG );
    }
    /*        heapinfo + frl,  allocated blk,  end tags */
    amount += sizeof( heapblk ) + TAG_SIZE + TAG_SIZE * 2;
    if( amount < _amblksiz )
        amount = _amblksiz;
    num_of_paras = ROUND_UP_SIZE_TO_PARA( amount );
    if( num_of_paras == 0 )
        num_of_paras = PARAS_IN_64K;

/* modification start */
    if( LastSeg == _NULLSEG ) {
        seg = FP_SEG( SyMemBeg );
    } else if( BHEAP( LastSeg )->len == 0 ) {
        seg = LastSeg + 0x1000;
    } else {
        seg = LastSeg + ROUND_DOWN_SIZE_TO_PARA( BHEAP( LastSeg )->len );
    }
    if( seg + num_of_paras > FP_SEG( SyMemEnd ) || seg + num_of_paras < seg ) {
        if( _IsOn( SW_REMOTE_LINK ) ) {
            rc = TinyAllocBlock( num_of_paras );
            if( TINY_ERROR( rc ) ) {
                return( _NULLSEG );
            }
            seg = TINY_INFO( rc );
        } else {
            return( _NULLSEG );
        }
    } else {
        LastSeg = seg;
    }
/* modification end */

    heaplen = num_of_paras << 4;
    BHEAP( seg )->len = heaplen;
    BHEAP( seg )->prev.segm = _NULLSEG;
    BHEAP( seg )->next.segm = _NULLSEG;
    BHEAP( seg )->rover.offs = sizeof( heapblk );
    BHEAP( seg )->b4rover = 0;
    BHEAP( seg )->numalloc = 0;
    BHEAP( seg )->numfree = 1;
    BHEAP( seg )->freehead.len = 0;
    BHEAP( seg )->freehead.prev.offs = sizeof( heapblk );
    BHEAP( seg )->freehead.next.offs = sizeof( heapblk );
    BHEAP( seg )->largest_blk = heaplen - sizeof( heapblk ) - 2 * TAG_SIZE;
    FIRST_FRL( seg )->len = heaplen - sizeof( heapblk ) - 2 * TAG_SIZE;
    FIRST_FRL( seg )->prev.offs = offsetof( heapblk, freehead );
    FIRST_FRL( seg )->next.offs = offsetof( heapblk, freehead );
    SET_HEAP_END( seg, heaplen - 2 * TAG_SIZE );
    return( seg );          /* return allocated segment */
}

/*
 * these stubs prevent the memory manager from trying to allocate near
 * memory
 */

heapblk_nptr __near __nheapbeg;

void __near *_nmalloc( size_t amt )
{
    /* unused parameters */ (void)amt;

    return( 0 );
}

void _nfree( void __near *p )
{
    /* unused parameters */ (void)p;
}

void _nheapgrow( void )
{
}

void __near *_nexpand( void __near *p, size_t size )
{
    /* unused parameters */ (void)p; (void)size;

    return( NULL );
}
