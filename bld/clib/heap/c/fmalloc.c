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
* Description:  Implementation of far malloc() and _fmalloc().
*               (16-bit code only)
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include "extfunc.h"
#include "heapacc.h"
#include "heap.h"
#include "rtinit.h"


#if defined(_M_IX86)
    #pragma aux (__outside_CLIB) __fmemneed;
#endif

__segment __fheapbeg = _NULLSEG;        /* head of segment list in far heap */
__segment __fheapRover = _NULLSEG;      /* segment to start search at */

//   The following variable contains the size of the largest block in a
//   segment between __fheapbeg and __fheapRover. If we are trying to allocate
//   a block of memory larger than this value then we can start at search
//   in the segment indicated by __fheapRover, otherwise we must start our
//   search at the head of the segment list __fheapbeg.

unsigned int   __LargestSizeB4Rover = 0;/* size of largest block */

#if defined(__BIG_DATA__)

_WCRTLINK void *malloc( size_t amount )
{
    return( _fmalloc( amount ) );
}

#endif

_WCRTLINK void_fptr _fmalloc( size_t amt )
{
    unsigned    size;
    void_bptr   cstg;
    __segment   seg;
    __segment   prev_seg;

    if( amt == 0 || amt > - ( sizeof( heapblk ) + TAG_SIZE * 2 ) ) {
        return( NULL );
    }

    // Try to determine which segment to begin allocating from.
    // first, round up the amount
    size = __ROUND_UP_SIZE_HEAP( amt );
    if( size < FRL_SIZE ) {
        size = FRL_SIZE;
    }

    prev_seg = _NULLSEG;
    _AccessFHeap();
    for( ;; ) {
        if( size > __LargestSizeB4Rover ) {
            seg = __fheapRover;
        } else {
            __LargestSizeB4Rover = 0;   // force value to be updated
            seg = __fheapbeg;
        }
        for( ;; ) {
            if( seg == _NULLSEG ) {
                seg = __AllocSeg( amt );
                if( seg == _NULLSEG )
                    break;
                if( __fheapbeg == _NULLSEG ) {
                    __fheapbeg = seg;
                } else {
                    BHEAP( seg )->prev.segm = prev_seg;
                    BHEAP( prev_seg )->next.segm = seg;
                }
            }
            for( ;; ) {
                __fheapRover = seg;
                cstg = __MemAllocator( amt, seg, 0 );
                if( cstg != NULL )
                    goto release_heap;
                if( __GrowSeg( seg, amt ) == 0 ) {
                    break;
                }
            }
            if( __LargestSizeB4Rover < BHEAP( seg )->largest_blk  ) {
                __LargestSizeB4Rover = BHEAP( seg )->largest_blk;
            }
            prev_seg = seg;
            seg = BHEAP( seg )->next.segm;
        }
        if( __fmemneed( amt ) == 0 ) {
            break;
        }
    }
    if( seg == _NULLSEG ) {
        cstg = _nmalloc( amt );
        if( cstg != NULL ) {
            seg = _DGroup();
        }
    }
release_heap:
    _ReleaseFHeap();
    return( _MK_FP( seg, cstg ) );
}

#if defined(__DOS__) && defined(__BIG_DATA__)
static void ___nheapgrow( void )
{
    /* near heap growing routine in large data models. */
    /* grow near heap to 64K */
    _nheapgrow();
}

AXI( ___nheapgrow, INIT_PRIORITY_PROGRAM - 8 )
#endif
