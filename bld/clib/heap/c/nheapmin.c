/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#elif defined(__DOS__)
    #include "tinyio.h"
#endif


#if defined(__DOS_EXT__)
#define MINIMAL_LEN     0x1000
#else
#define MINIMAL_LEN     (sizeof( freelist ) + 1)
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

#if defined(__OS2__) && !defined(_M_I86) || defined(__WINDOWS__) || defined(__NT__) || \
    defined(__CALL21__) || defined(__RDOS__) || defined(__DOS_EXT__)

static int __ReturnMemToSystem( heapblk_nptr heap )
{
  #if defined(__OS2__) && !defined(_M_I86)
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
    heapblk_nptr    heap;
    heapblk_nptr    next_heap;
    heapblk_nptr    prev_heap;

    // Shrink by releasing mini-heaps

    for( heap = __nheapbeg; heap != NULL; heap = next_heap ) {
        next_heap = heap->next.nptr;
        // see if the last free entry has the full size of the block ( - overhead )
        // if it is then we can give this block back to the system
        if( heap->len - sizeof( heapblk ) == (heap->freehead.prev.nptr)->len ) {
            prev_heap = heap->prev.nptr;
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
#if defined(__OS2__) && !defined(_M_I86) || defined(__WINDOWS__) || defined(__NT__) || \
    defined(__CALL21__) || defined(__RDOS__)
#else
    heapblk_nptr    heap;
    freelist_nptr   last_free;
    freelist_nptr   end_tag;
    unsigned        new_brk;
#endif

    // Shrink by adjusting _curbrk

    _AccessNHeap();
#if defined(__OS2__) && !defined(_M_I86) || defined(__WINDOWS__) || defined(__NT__) || \
    defined(__CALL21__) || defined(__RDOS__)
    rc = __nheapshrink();
#else
  #if defined(__DOS_EXT__)
    if( _IsRationalZeroBase() || _IsCodeBuilder() ) {
        rc = __nheapshrink();
    } else {
  #endif
        rc = 0;
        if( __nheapbeg != NULL ) {
            /* Goto the end of miniheaplist (if there's more than 1 blk) */
            for( heap = __nheapbeg; heap->next.nptr != NULL; heap = heap->next.nptr )
                ;
            /* check that last free block is at end of heap */
            last_free = heap->freehead.prev.nptr;
            end_tag = (freelist_nptr)NEXT_BLK( last_free );
            if( IS_BLK_END( end_tag ) && end_tag == (freelist_nptr)NEXT_BLK( heap )
                // only shrink if we can shave off at MINIMAL_LEN
              && last_free->len >= MINIMAL_LEN
                /* make sure there hasn't been an external change in _curbrk */
              && sbrk( 0 ) == (void_nptr)BLK2CSTG( end_tag ) ) {
                /* calculate adjustment factor */
                if( heap->len - last_free->len > sizeof( heapblk ) ) {
                    // this miniheapblk is still being used
#if defined(__DOS_EXT__)
                    freelist_nptr new_last_free;
                    new_last_free = (freelist_nptr)( __ROUND_UP_SIZE_4K( (unsigned)last_free ) - TAG_SIZE );
                    if( new_last_free == last_free ) {
#endif
                        // remove entire entry
                        heap->len -= last_free->len;
                        --heap->numfree;
                        // Relink the freelist entries, and update the rover
                        heap->freehead.prev.nptr = last_free->prev.nptr;
                        last_free->prev.nptr->next.nptr = &heap->freehead;
                        if( heap->rover.nptr == last_free ) {
                            heap->rover.nptr = last_free->prev.nptr;
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
                    new_brk = (unsigned)BLK2CSTG( last_free );
                } else {
                    // this miniheapblk is not used, we can remove it
                    if( heap->prev.nptr != NULL ) { // Not the first miniheapblk
                        new_brk = (unsigned)heap;//->prev + (unsigned)heap->prev->len;
                    } else { // Is the first miniheapblk
                        new_brk = (unsigned)__nheapbeg;
                    }
                    // Unlink and update rover info
                    __UnlinkNHeap( heap, heap->prev.nptr, heap->next.nptr );
                }
                if( __brk( new_brk ) == (void_nptr)-1 ) {
                    rc = -1;
                }
            }
        }
  #if defined(__DOS_EXT__)
    }
  #endif
#endif
    _ReleaseNHeap();
    return( rc );
}
