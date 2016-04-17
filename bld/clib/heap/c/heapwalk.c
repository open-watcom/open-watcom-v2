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

static int verifyHeapList( unsigned seg )
{
    /* make sure list of heaps is a doubly-linked NULL terminated list */
    struct heapblk _WCFAR *prev_heap;
    struct heapblk _WCFAR *next_heap;
    struct heapblk _WCFAR *curr;
    unsigned next_seg;
    unsigned prev_seg;

    curr = MK_FP( seg, 0 );
    /* check previous heaps end in NULL */
    for(;;) {
        prev_seg = curr->prevseg;
        if( prev_seg == seg ) {
            return( _HEAPBADBEGIN );
        }
        prev_heap = MK_FP( prev_seg, 0 );
        if( prev_heap == NULL ) break;
        if( prev_heap->nextseg != FP_SEG( curr ) ) {
            return( _HEAPBADBEGIN );
        }
        curr = prev_heap;
    }
    /* check next heaps end in NULL */
    for(;;) {
        next_seg = curr->nextseg;
        if( next_seg == seg ) {
            return( _HEAPBADBEGIN );
        }
        next_heap = MK_FP( next_seg, 0 );
        if( next_heap == NULL ) break;
        if( next_heap->prevseg != FP_SEG( curr ) ) {
            return( _HEAPBADBEGIN );
        }
        curr = next_heap;
    }
    return( _HEAPOK );
}

int __HeapWalk( struct _heapinfo *entry, __segment seg, unsigned all_segs )
    {
        struct heapblk _WCFAR *prev_heap;
        struct heapblk _WCFAR *next_heap;
        struct heapblk _WCFAR *pheap;
        farfrlptr p;
        farfrlptr q;

        if( seg == 0 )  return( _HEAPEMPTY );
        p = entry->_pentry;
        if( p != NULL ) {
            seg = FP_SEG(p);
        } else if( all_segs == 0 ) {
            /* we are starting a multi-heap walk */
            if( verifyHeapList( seg ) != _HEAPOK ) {
                return( _HEAPBADBEGIN );
            }
        }
        for(;;) {
            pheap = MK_FP( seg, 0 );
            prev_heap = MK_FP( pheap->prevseg, 0 );
            next_heap = MK_FP( pheap->nextseg, 0 );
            if( prev_heap != NULL ) {
                if( prev_heap->nextseg != seg || prev_heap == next_heap ) {
                    return( _HEAPBADBEGIN );
                }
            }
            if( next_heap != NULL ) {
                if( next_heap->prevseg != seg ) {
                    return( _HEAPBADBEGIN );
                }
            }
            if( p == NULL ) {
                if( pheap->freehead.len != 0 )  return( _HEAPBADBEGIN );
                p = MK_FP( FP_SEG(pheap), sizeof(struct heapblk) );
            } else {    /* advance to next entry */
                q = (farfrlptr)((FARPTR)p + (p->len & ~1));
                if( q <= p )  return( _HEAPBADNODE );
                p = q;
                if( pheap->heaplen != 0 && (tag)p > pheap->heaplen ) {
                    return( _HEAPBADNODE );
                }
            }
            if( p->len != END_TAG ) break;
            seg = pheap->nextseg;
            if( seg == 0  ||  all_segs != 0 ) {
                entry->_useflag = _USEDENTRY;
                entry->_size    = 0;
                entry->_pentry  = NULL;
                return( _HEAPEND );
            }
            p = NULL;
        }
        entry->_pentry  = p;
        entry->_useflag = _FREEENTRY;
        entry->_size    = p->len & ~1;
        if( p->len & 1 ) {
            entry->_useflag = _USEDENTRY;
        }
        return( _HEAPOK );
    }
