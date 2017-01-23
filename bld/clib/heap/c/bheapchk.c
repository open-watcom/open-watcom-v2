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
#include "heap.h"
#include "heapacc.h"


static int checkFreeList( unsigned long *free_size, __segment req_seg )
{
    __segment                   curr_seg;
    XBPTR( freelist, curr_seg ) curr_frl;
    unsigned long               total_size;

    total_size = 0;
    for( curr_seg = (req_seg == _NULLSEG ? __bheapbeg : req_seg); curr_seg != _NULLSEG; curr_seg = curr_heap->nextseg ) {
        for( curr_frl = (XBPTR( freelist, curr_seg ))curr_heap->freehead.next; FP_OFF( curr_frl ) != offsetof( heapblk, freehead ); curr_frl = (XBPTR( freelist, curr_seg ))curr_frl->next ) {
            total_size += curr_frl->len;
        }
        if( req_seg != _NULLSEG ) {
            break;
        }
    }
    *free_size = total_size;
    return( _HEAPOK );
}

static int checkFree( farfrlptr p )
{
    __segment               seg;
    XBPTR( freelist, seg )  prev;
    XBPTR( freelist, seg )  next;

    seg = FP_SEG( p );
    prev = (XBPTR( freelist, seg ))p->prev;
    next = (XBPTR( freelist, seg ))p->next;
    if( prev->next != FP_OFF( p ) || next->prev != FP_OFF( p ) ) {
        return( _HEAPBADNODE );
    }
    if( ((XBPTR( freelist, seg ))prev->prev)->next != FP_OFF( prev ) || ((XBPTR( freelist, seg ))next->next)->prev != FP_OFF( next ) ) {
        return( _HEAPBADNODE );
    }
    return( _HEAPOK );
}

_WCRTLINK int _bheapchk( __segment seg )
{
    struct _heapinfo    hi;
    int                 heap_status;
    unsigned long       free_size;

    if( seg == _DGroup() )
        return( _nheapchk() );
    _AccessFHeap();
    heap_status = checkFreeList( &free_size, seg );
    if( heap_status == _HEAPOK ) {
        hi._pentry = NULL;
        while( (heap_status = __HeapWalk( &hi, (seg == _NULLSEG ? __bheapbeg : seg), seg )) == _HEAPOK ) {
            if( hi._useflag == _FREEENTRY ) {
                heap_status = checkFree( hi._pentry );
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
    }
    _ReleaseFHeap();
    return( heap_status );
}
