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

frlptr __nheapchk_current;

static int checkFreeList( size_t *free_size )
{
    frlptr      curr_frl;
    frlptr      end_frl;
    size_t      new_size;
    size_t      free_list_size;
    mheapptr    mhp;

    free_list_size = 0;
    for( mhp = __nheapbeg; mhp != NULL; mhp = mhp->next ) {
        /* check that the free list is a doubly linked ring */
        __nheapchk_current = curr_frl = mhp->freehead.next;
        /* make sure we start off on the right track */
        if( (curr_frl->prev == NEAR_NULL) ||
            (curr_frl->prev < &(mhp->freehead)) ||
            (((PTR)curr_frl->prev) > (((PTR)mhp) + mhp->len)) ) {
            return( _HEAPBADNODE );
        }
        if( curr_frl->prev->next != curr_frl ) {
            return( _HEAPBADNODE );
        }
        end_frl = curr_frl;
        do {
            /* loop invariant: curr_frl->prev->next == curr_frl */
            /* are we still in a ring if we move to curr_frl->next? */
            /* nb. this check is sufficient to ensure that we will never cycle */
            if( (curr_frl->next == NEAR_NULL) ||
                (curr_frl->next < &(mhp->freehead)) ||
                (((PTR)curr_frl->next) > (((PTR)mhp) + mhp->len)) ) {
                return( _HEAPBADNODE );
            }
            if( curr_frl->next->prev != curr_frl ) {
                return( _HEAPBADNODE );
            }
            /* is entry allocated? */
            if( IS_BLK_INUSE( curr_frl ) ) {
                return( _HEAPBADNODE );
            }
            new_size = free_list_size + curr_frl->len;
            if( new_size < free_list_size ) {
                /* this is a case where we do not know where memory is corrupted */
                return( _HEAPBADNODE );
            }
            free_list_size = new_size;
            __nheapchk_current = curr_frl = curr_frl->next;
        } while( curr_frl != end_frl );
    }
    *free_size = free_list_size;
    return( _HEAPOK );
}

static int checkFree( frlptr p )
{
    frlptr next;
    frlptr prev;

    __nheapchk_current = p;
    if( IS_BLK_INUSE( p ) ) {
        return( _HEAPBADNODE );
    }
    next = p->next;
    prev = p->prev;
    if( next == NEAR_NULL || prev == NEAR_NULL ) {
        return( _HEAPBADNODE );
    }
    if( next->prev != p || prev->next != p ) {
        return( _HEAPBADNODE );
    }
    if( next->next == NEAR_NULL || prev->prev == NEAR_NULL ) {
        return( _HEAPBADNODE );
    }
    if( next->next->prev != next || prev->prev->next != prev ) {
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
    hi._pentry = FAR_NULL;
    for( ;; ) {
        heap_status = __NHeapWalk( &hi, __nheapbeg );
        if( heap_status != _HEAPOK )
            break;
        if( hi._useflag == _FREEENTRY ) {
            heap_status = checkFree( (frlptr)hi._pentry );
            if( heap_status != _HEAPOK )
                break;
            free_size -= hi._size;
        }
    }
    if( free_size != 0 ) {
        heap_status = _HEAPBADNODE;
    } else if( heap_status == _HEAPBADPTR ) {
        heap_status = _HEAPBADNODE;
    } else {
        if( heap_status == _HEAPEND ) {
            heap_status = _HEAPOK;
        }
    }
    _ReleaseNHeap();
    return( heap_status );
}
