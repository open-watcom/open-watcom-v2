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
* Description:  Implementation of near _heapmin() and _nheapmin().
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"
#if defined(__WINDOWS_286__) || defined(__NT__)
    #include <windows.h>
#elif defined(__WINDOWS_386__)
    #include "windpmi.h"
#endif
#if defined(__OS2__)
    #include <wos2.h>
#endif
#if defined(__RDOS__)
    #include <rdos.h>
#endif
#if defined(__CALL21__)
    #include "tinyio.h"
#endif


#if defined(__SMALL_DATA__)

_WCRTLINK int _heapshrink( void )
{
    return( _nheapshrink() );
}

_WCRTLINK int _heapmin( void )
{
    return( _nheapshrink() );
}

#endif

_WCRTLINK int _nheapmin( void )
{
    return( _nheapshrink() );
}

#if defined(__WARP__)        || \
    defined(__WINDOWS__)     || \
    defined(__NT__)          || \
    defined(__CALL21__)      || \
    defined(__RDOS__)
static int __ReturnMemToSystem( mheapptr heap )
{
    mheapptr    next_heap;

    next_heap = heap->next;
  #if defined(__WARP__)
    if( DosFreeMem( (PBYTE)heap ) )
        return( -1 );
  #elif defined(__NT__)
    //if( LocalFree( (HLOCAL)heap ) != NULL ) return( -1 );
    if( !VirtualFree( (LPVOID)heap, 0, MEM_RELEASE ) )
        return -1;
  #elif defined(__WINDOWS_386__)
    if( DPMIFree( (unsigned long)heap ) != 0 )
        return( -1 );
  #elif defined(__WINDOWS_286__)
    if( LocalFree( (HLOCAL)heap ) != NULL )
        return( -1 );
  #elif defined(__CALL21__)
    // No way to free storage under OSI
    if( heap )
        return( -1 );
  #elif defined(__RDOS__)
    RdosFreeMem( heap );
  #endif
    if( __MiniHeapRover == heap ) {  // Update rovers
        if( next_heap != NULL ) {
            __MiniHeapRover = next_heap;
        } else {
            __MiniHeapRover = __nheapbeg;
            __LargestSizeB4MiniHeapRover = 0;
        }
    }
    // Re-test rover; if we freed the only mini-heap, we might end up
    // pointing back to it
    if( __MiniHeapRover == heap ) {
        __MiniHeapRover = NULL;
    }
    if( __MiniHeapFreeRover == heap ) {
        __MiniHeapFreeRover = NULL;
    }
    return( 0 ); // success
}

static void __ReleaseMiniHeap( mheapptr heap )
{
    mheapptr    prev_heap;
    mheapptr    next_heap;

    prev_heap = heap->prev;
    next_heap = heap->next;
    if( __ReturnMemToSystem( heap ) == 0 ) {
        if( prev_heap == NULL ) {
            __nheapbeg = next_heap;
        } else {
            prev_heap->next = next_heap;
        }
        if( next_heap != NULL ) {
            next_heap->prev = prev_heap;
        }
    } //else: do not unlink if the memory cannot be freed successfully
}

#endif

_WCRTLINK int _nheapshrink( void )
{
    mheapptr    heap;
#if !defined(__WARP__)        && \
    !defined(__WINDOWS__)     && \
    !defined(__NT__)          && \
    !defined(__CALL21__)      && \
    !defined(__RDOS__)
    // Shrink by adjusting _curbrk

    frlptr      last_free;
    frlptr      end_tag;
    unsigned    new_brk;

    _AccessNHeap();
  #if defined(__DOS_EXT__)
    if( !__IsCtsNHeap() ) {
  #endif
        if( __nheapbeg == NEAR_NULL ) {
            _ReleaseNHeap();
            return( 0 ); // No near heap, can't shrink
        }
        /* Goto the end of miniheaplist (if there's more than 1 blk) */
        for( heap = __nheapbeg; heap->next != NEAR_NULL; heap = heap->next )
            ;
        /* check that last free block is at end of heap */
        last_free = heap->freehead.prev;
        end_tag = (frlptr)NEXT_BLK( last_free );
        if( !IS_BLK_END( end_tag ) ) {
            _ReleaseNHeap();
            return( 0 );
        }
        if( end_tag != (frlptr)NEXT_BLK( heap ) ) {
            _ReleaseNHeap();
            return( 0 );
        }
  #if defined(__DOS_EXT__)
        // only shrink if we can shave off at least 4k
        if( last_free->len < 0x1000 ) {
            _ReleaseNHeap();
            return( 0 );
        }
  #else
        if( last_free->len <= sizeof( freelistp ) ) {
            _ReleaseNHeap();
            return( 0 );
        }
  #endif
        /* make sure there hasn't been an external change in _curbrk */
        if( sbrk( 0 ) != (void _WCNEAR *)BLK2CPTR( end_tag ) ) {
            _ReleaseNHeap();
            return( 0 );
        }
        /* calculate adjustment factor */
        if( heap->len - last_free->len > sizeof( miniheapblkp ) ) {
            // this miniheapblk is still being used
  #if defined(__DOS_EXT__)
            frlptr new_last_free;
            new_last_free = (frlptr)( __ROUND_UP_SIZE_4K( (unsigned)last_free ) - TAG_SIZE );
            if( new_last_free == last_free ) {
  #endif
                // remove entire entry
                heap->len -= last_free->len;
                --heap->numfree;
                // Relink the freelist entries, and update the rover
                heap->freehead.prev = last_free->prev;
                last_free->prev->next = &heap->freehead;
                if( heap->rover == last_free ) {
                    heap->rover = last_free->prev;
                }
  #if defined(__DOS_EXT__)
            } else {
                // just shrink the last free entry
                heap->len -= last_free->len;
                last_free->len = (PTR)new_last_free - (PTR)last_free;
                heap->len += last_free->len;
                last_free = new_last_free;
            }
  #endif
            SET_BLK_END( last_free );
            new_brk = (unsigned)BLK2CPTR( last_free );
        } else {
            // we can remove this miniheapblk
            if( heap->prev ) { // Not the first miniheapblk
                heap->prev->next = NEAR_NULL;
                new_brk = (unsigned)heap;//->prev + (unsigned)heap->prev->len;
            } else { // Is the first miniheapblk
                new_brk = (unsigned)__nheapbeg;
                __nheapbeg = NEAR_NULL;
            }
            // Update rover info
            if( __MiniHeapRover == heap ) {
                __MiniHeapRover = __nheapbeg;
                __LargestSizeB4MiniHeapRover = 0;
            }
        }

        if( __brk( new_brk ) == (void _WCNEAR *)-1 ) {
            _ReleaseNHeap();
            return( -1 );
        }
        _ReleaseNHeap();
        return( 0 );
  #if defined(__DOS_EXT__)
    }
    __FreeDPMIBlocks(); // For RSI/zero-base and Intel CB
    _ReleaseNHeap();
    return( 0 );
  #endif
#else
    // Shrink by releasing mini-heaps
    {
        mheapptr next_heap;

        _AccessNHeap();
        for( heap = __nheapbeg; heap != NULL; heap = next_heap ) {
            next_heap = heap->next;
            if( heap->len - sizeof( miniheapblkp ) == (heap->freehead.prev)->len ) {
                __ReleaseMiniHeap( heap );
            }
        }
        _ReleaseNHeap();
        return( 0 );
    }
#endif
}
