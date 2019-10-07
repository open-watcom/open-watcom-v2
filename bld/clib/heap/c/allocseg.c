/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Memory block allocator.
*                (16-bit code only)
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#if defined(__OS2__)
    #include <dos.h>
    #define INCL_DOSMEMMGR
    #include <wos2.h>
#elif defined(__QNX__)
    #include <sys/types.h>
    #include <sys/seginfo.h>
#elif defined(__WINDOWS__)
    #include <dos.h>
    #include <windows.h>
#else   /* __DOS__ */
    #include <dos.h>
    #include "tinyio.h"
#endif
#include "rtdata.h"
#include "heap.h"

#define FIRST_FRL(s)        ((freelist __based(s) *)(BHEAP(s) + 1))

#if defined(__QNX__)
extern unsigned         __qnx_alloc_flags;
#endif

__segment __AllocSeg( unsigned int amount )
{
    unsigned    num_of_paras;       /* number of paragraphs desired   */
    unsigned    heaplen;
    __segment   seg;
#if defined(__OS2__)
#elif defined(__QNX__)
    unsigned    rc;
#elif defined(__WINDOWS__)
#else   /* __DOS__ */
    tiny_ret_t  rc;
#endif

    if( !__heap_enabled )
        return( _NULLSEG );
    /*               heapinfo + frl + frl,                    end tags */
    if( amount > - ( sizeof( heapblk ) + sizeof( freelist ) + TAG_SIZE * 2 ) ) {
        return( _NULLSEG );
    }
    /*        heapinfo + frl,  allocated blk,  end tags */
    amount += sizeof( heapblk ) + TAG_SIZE + TAG_SIZE * 2;
    if( amount < _RWD_amblksiz )
        amount = _RWD_amblksiz;
    num_of_paras = __ROUND_UP_SIZE_TO_PARA( amount );
    if( num_of_paras == 0 )
        num_of_paras = PARAS_IN_64K;
#if defined(__OS2__)
    seg = _NULLSEG;
    if( DosAllocSeg( num_of_paras << 4, (PSEL)&seg, 0 ) )
        return( _NULLSEG );
#elif defined(__QNX__)
    rc = qnx_segment_alloc_flags( ((long)num_of_paras) << 4, __qnx_alloc_flags );
    if( rc == (unsigned)-1 )
        return( _NULLSEG );
    seg = (__segment)rc;
#elif defined(__WINDOWS__)
    {
        HANDLE hmem;
        LPSTR px;

        hmem = GlobalAlloc( __win_alloc_flags, ((long)num_of_paras) << 4 );
        if( hmem == NULL )
            return( _NULLSEG );
        px = GlobalLock( hmem );
        if( px == NULL ) {
            GlobalFree( hmem );
            return( _NULLSEG );
        }
        seg = _FP_SEG( px );
    }
#else   /* __DOS__ */
    rc = TinyAllocBlock( num_of_paras );
    if( TINY_ERROR( rc ) ) {
        return( _NULLSEG );
    }
    seg = TINY_INFO( rc );
#endif
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
