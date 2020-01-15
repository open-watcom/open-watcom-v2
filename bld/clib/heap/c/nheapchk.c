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
* Description:  Implementation of near _heapchk() and _nheapchk().
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"

freelist_nptr __nheapchk_current;

static int checkFreeList( size_t *free_size )
{
    freelist_nptr   frl;
    freelist_nptr   end_frl;
    size_t          new_size;
    size_t          free_list_size;
    heapblk_nptr    heap;

    free_list_size = 0;
    for( heap = __nheapbeg; heap != NULL; heap = heap->next.nptr ) {
        /* check that the free list is a doubly linked ring */
        __nheapchk_current = frl = heap->freehead.next.nptr;
        /* make sure we start off on the right track */
        if( frl->prev.nptr == NULL || frl->prev.nptr < &(heap->freehead) || (PTR)frl->prev.nptr > (PTR)NEXT_BLK( heap ) ) {
            return( _HEAPBADNODE );
        }
        if( frl->prev.nptr->next.nptr != frl ) {
            return( _HEAPBADNODE );
        }
        end_frl = frl;
        do {
            /* loop invariant: frl->prev->next == frl */
            /* are we still in a ring if we move to frl->next? */
            /* nb. this check is sufficient to ensure that we will never cycle */
            if( frl->next.nptr == NULL || frl->next.nptr < &(heap->freehead) || (PTR)frl->next.nptr > (PTR)NEXT_BLK( heap ) ) {
                return( _HEAPBADNODE );
            }
            if( frl->next.nptr->prev.nptr != frl ) {
                return( _HEAPBADNODE );
            }
            /* is entry allocated? */
            if( IS_BLK_INUSE( frl ) ) {
                return( _HEAPBADNODE );
            }
            new_size = free_list_size + frl->len;
            if( new_size < free_list_size ) {
                /* this is a case where we do not know where memory is corrupted */
                return( _HEAPBADNODE );
            }
            free_list_size = new_size;
            __nheapchk_current = frl = frl->next.nptr;
        } while( frl != end_frl );
    }
    *free_size = free_list_size;
    return( _HEAPOK );
}

static int checkFree( freelist_nptr frl )
{
    freelist_nptr next;
    freelist_nptr prev;

    __nheapchk_current = frl;
    if( IS_BLK_INUSE( frl ) ) {
        return( _HEAPBADNODE );
    }
    next = frl->next.nptr;
    prev = frl->prev.nptr;
    if( next == NULL || prev == NULL ) {
        return( _HEAPBADNODE );
    }
    if( next->prev.nptr != frl || prev->next.nptr != frl ) {
        return( _HEAPBADNODE );
    }
    if( next->next.nptr == NULL || prev->prev.nptr == NULL ) {
        return( _HEAPBADNODE );
    }
    if( next->next.nptr->prev.nptr != next || prev->prev.nptr->next.nptr != prev ) {
        return( _HEAPBADNODE );
    }
    return( _HEAPOK );
}

#if defined(__SMALL_DATA__)
_WCRTLINK int _heapchk( void )
{
    return( _nheapchk() );
}
#endif

_WCRTLINK int _nheapchk( void )
{
    struct _heapinfo    hi;
    int                 heap_status;
    size_t              free_size;

    _AccessNHeap();
    heap_status = checkFreeList( &free_size );
    if( heap_status != _HEAPOK ) {
        _ReleaseNHeap();
        return( heap_status );
    }
    hi._pentry = NULL;
    while( (heap_status = __NHeapWalk( &hi, __nheapbeg )) == _HEAPOK ) {
        if( hi._useflag == _FREEENTRY ) {
            heap_status = checkFree( FAR2NEAR( void, hi._pentry ) );
            if( heap_status != _HEAPOK )
                break;
            free_size -= hi._size;
        }
    }
    if( free_size != 0 ) {
        heap_status = _HEAPBADNODE;
    } else if( heap_status == _HEAPBADPTR ) {
        heap_status = _HEAPBADNODE;
    } else if( heap_status == _HEAPEND ) {
        heap_status = _HEAPOK;
    }
    _ReleaseNHeap();
    return( heap_status );
}
