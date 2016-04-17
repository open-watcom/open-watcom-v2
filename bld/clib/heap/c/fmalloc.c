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
* Description:  Implementation of far malloc() and _fmalloc().
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

__segment __fheap = _NULLSEG;          /* head of segment list in far heap */
__segment __fheapRover = _NULLSEG;     /* segment to start search at */

//   The following variable contains the size of the largest block in a
//   segment between __fheap and __fheapRover. If we are trying to allocate
//   a block of memory larger than this value then we can start at search
//   in the segment indicated by __fheapRover, otherwise we must start our
//   search at the head of the segment list __fheap.

unsigned int   __LargestSizeB4Rover = 0;/* size of largest block */


#if defined(__BIG_DATA__)

_WCRTLINK void *malloc( size_t amount )
{
    return( _fmalloc( amount ) );
}

#endif


_WCRTLINK void _WCFAR *_fmalloc( size_t amt )
{
    unsigned    size;
    unsigned    offset;
    __segment   seg;
    __segment   prev_seg;
    heapblk     _WCFAR *p;

    if( amt == 0 || amt > - ( sizeof( heapblk ) + TAG_SIZE * 2 ) ) {
        return( (void _WCFAR *)NULL );
    }

    // Try to determine which segment to begin allocating from.
    // first, round up the amount
    size = (amt + TAG_SIZE + ROUND_SIZE) & ~ROUND_SIZE;
    if( size < FRL_SIZE ) {
        size = FRL_SIZE;
    }

    _AccessFHeap();
    for( ;; ) {
        if( size > __LargestSizeB4Rover ) {
            seg = __fheapRover;
        } else {
            __LargestSizeB4Rover = 0;   // force value to be updated
            seg = __fheap;
        }
        for( ;; ) {
            if( seg == _NULLSEG ) {
                seg = __AllocSeg( amt );
                if( seg == _NULLSEG )
                    break;
                if( __fheap == _NULLSEG ) {
                    __fheap = seg;
                } else {
                    p->nextseg = seg;
                    p = MK_FP( seg, 0 );
                    p->prevseg = prev_seg;
                }
            }
            for( ;; ) {
                __fheapRover = seg;
                offset = __MemAllocator( amt, seg, 0 );
                if( offset != 0 )
                    goto release_heap;
                if( __GrowSeg( seg, amt ) == 0 ) {
                    break;
                }
            }
            prev_seg = seg;
            p = MK_FP( seg, 0 );
            if( p->largest_blk > __LargestSizeB4Rover ) {
                __LargestSizeB4Rover = p->largest_blk;
            }
            seg = p->nextseg;
        }
        if( __fmemneed( amt ) == 0 ) {
            break;
        }
    }
    if( seg == _NULLSEG ) {
        offset = (unsigned)_nmalloc( amt );
        if( offset != 0 ) {
            seg = _DGroup();
        }
    }
release_heap:
    _ReleaseFHeap();
    return( MK_FP( seg, offset ) );
}

#if defined(_M_I86) && defined(__DOS__) && defined(__BIG_DATA__)
static void ___nheapgrow( void )
{
    /* near heap growing routine in large data models. */
    /* grow near heap to 64K */
    _nheapgrow();
}

AXI( ___nheapgrow, INIT_PRIORITY_PROGRAM - 8 )
#endif
