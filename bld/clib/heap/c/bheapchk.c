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


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include <i86.h>
#include "heap.h"
#include "heapacc.h"

static int checkFreeList( unsigned long *free_size, __segment req_seg )
{
    farfrlptr curr;
    unsigned short seg;
    struct heapblk _WCFAR *p;
    unsigned long total_size;

    total_size = 0;
    seg = req_seg;
    if( seg == _NULLSEG )  seg = __bheap;
    for( ; seg ;) {
        p = MK_FP( seg, 0 );
        curr = MK_FP( seg, p->freehead.next );
        for(;;) {
            if( FP_OFF(curr) == offsetof(struct heapblk, freehead) ) break;
            total_size += curr->len;
            curr = MK_FP( seg, curr->next );
        }
        if( req_seg != _NULLSEG )  break;
        seg = p->nextseg;
    }
    *free_size = total_size;
    return( _HEAPOK );
}

static int checkFree( farfrlptr p )
{
    unsigned short seg;
    farfrlptr prev;
    farfrlptr next;
    farfrlptr prev_prev;
    farfrlptr next_next;

    seg = FP_SEG( p );
    prev = MK_FP( seg, p->prev );
    next = MK_FP( seg, p->next );
    if( prev->next != FP_OFF(p) || next->prev != FP_OFF(p) ) {
        return( _HEAPBADNODE );
    }
    prev_prev = MK_FP( seg, prev->prev );
    next_next = MK_FP( seg, next->next );
    if( prev_prev->next != FP_OFF(prev) || next_next->prev != FP_OFF(next) ) {
        return( _HEAPBADNODE );
    }
    return( _HEAPOK );
}

_WCRTLINK int _bheapchk( __segment seg )
{
    struct _heapinfo    hi;
    int                 heap_status;
    unsigned long       free_size;

    if( seg == _DGroup() ) return( _nheapchk() );
    _AccessFHeap();
    heap_status = checkFreeList( &free_size, seg );
    if( heap_status == _HEAPOK ) {
        hi._pentry = NULL;
        for(;;) {
            heap_status = __HeapWalk( &hi, seg==_NULLSEG ? __bheap:seg, seg);
            if( heap_status != _HEAPOK ) break;
            if( hi._useflag == _FREEENTRY ) {
                heap_status = checkFree( hi._pentry );
                if( heap_status != _HEAPOK ) break;
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
    }
    _ReleaseFHeap();
    return( heap_status );
}
