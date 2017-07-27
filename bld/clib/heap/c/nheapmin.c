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
#elif defined(__OS2__)
    #include <wos2.h>
#elif defined(__RDOS__)
    #include <rdos.h>
#elif defined(__CALL21__) || defined(__DOS_EXT__)
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

#if defined(__WARP__) || defined(__WINDOWS__) || defined(__NT__) || \
    defined(__CALL21__) || defined(__RDOS__) || defined(__DOS_EXT__)

int __ReturnMemToSystem( mheapptr heap )
{
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
    if( heap != NULL )
        return( -1 );
  #elif defined(__DOS_EXT__)
    dpmi_hdr    *dpmi = BLK2DPMI( heap );
    if( dpmi->dos_seg_value == 0 ) {    // if DPMI block
        TinyDPMIFree( dpmi->dpmi_handle );
    } else {                            // else DOS block below 1MB
        TinyFreeBlock( dpmi->dos_seg_value );
    }
  #elif defined(__RDOS__)
    RdosFreeMem( heap );
  #endif
    return( 0 ); // success
}

int __nheapshrink( void )
{
    mheapptr    heap;
    mheapptr    next_heap;
    mheapptr    prev_heap;

    // Shrink by releasing mini-heaps

    for( heap = __nheapbeg; heap != NULL; heap = next_heap ) {
        next_heap = heap->next;
        if( heap->len - sizeof( miniheapblkp ) == (heap->freehead.prev)->len ) {
            prev_heap = heap->prev;
            if( __ReturnMemToSystem( heap ) == 0 ) {
                __UnlinkNHeap( heap, prev_heap, next_heap );
            } else {
                // do not unlink if the memory cannot be freed successfully
            }
        }
    }
    return( 0 );
}

#endif

_WCRTLINK int _nheapshrink( void )
{
    int         rc;
#if defined(__WARP__) || defined(__WINDOWS__) || defined(__NT__) || \
    defined(__CALL21__) || defined(__RDOS__)
#else
    mheapptr    heap;
    frlptr      last_free;
    frlptr      end_tag;
    unsigned    new_brk;
#endif

    // Shrink by adjusting _curbrk

    _AccessNHeap();
#if defined(__WARP__) || defined(__WINDOWS__) || defined(__NT__) || \
    defined(__CALL21__) || defined(__RDOS__)
    rc = __nheapshrink();
#else
  #if defined(__DOS_EXT__)
    if( _IsRationalZeroBase() || _IsCodeBuilder() ) {
        rc = __nheapshrink();
    } else {
  #endif
        rc = 0;
        if( __nheapbeg == NULL ) {
            _ReleaseNHeap();
            return( 0 ); // No near heap, can't shrink
        }
        /* Goto the end of miniheaplist (if there's more than 1 blk) */
        for( heap = __nheapbeg; heap->next != NULL; heap = heap->next )
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
  #else
        if( last_free->len <= sizeof( freelistp ) ) {
  #endif
            _ReleaseNHeap();
            return( 0 );
        }
        /* make sure there hasn't been an external change in _curbrk */
        if( sbrk( 0 ) != (void_nptr)BLK2CPTR( end_tag ) ) {
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
            if( heap->prev != NULL ) { // Not the first miniheapblk
                new_brk = (unsigned)heap;//->prev + (unsigned)heap->prev->len;
            } else { // Is the first miniheapblk
                new_brk = (unsigned)__nheapbeg;
            }
            // Unlink and update rover info
            __UnlinkNHeap( heap, heap->prev, heap->next );
        }

        if( __brk( new_brk ) == (void_nptr)-1 ) {
            _ReleaseNHeap();
            return( -1 );
        }
  #if defined(__DOS_EXT__)
    }
  #endif
#endif
    _ReleaseNHeap();
    return( rc );
}
