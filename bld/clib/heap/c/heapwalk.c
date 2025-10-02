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
* Description:  far and based heap walk function implementation
*               (16-bit code only)
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include "heap.h"


static int verifyHeapList( __segment start )
{
    /* make sure list of heaps is a doubly-linked NULL terminated list */
    __segment   seg;
    __segment   next_seg;
    __segment   prev_seg;

    /* check previous heaps end in NULL */
    for( seg = start; ; seg = prev_seg ) {
        prev_seg = BHEAP( seg )->prev.segm;
        if( prev_seg == start ) {
            return( _HEAPBADBEGIN );
        }
        if( prev_seg == _NULLSEG )
            break;
        if( BHEAP( prev_seg )->next.segm != seg ) {
            return( _HEAPBADBEGIN );
        }
    }
    /* check next heaps end in NULL */
    for( ; ; seg = next_seg ) {
        next_seg = BHEAP( seg )->next.segm;
        if( next_seg == start ) {
            return( _HEAPBADBEGIN );
        }
        if( next_seg == _NULLSEG )
            break;
        if( BHEAP( next_seg )->prev.segm != seg ) {
            return( _HEAPBADBEGIN );
        }
    }
    return( _HEAPOK );
}

int __HeapWalk( struct _heapinfo *entry, __segment seg, __segment one_heap )
{
    __segment       next_seg;
    __segment       prev_seg;
    FRLPTR( seg )   frl;
    FRLPTR( seg )   frl_next;

    if( seg == _NULLSEG )
        return( _HEAPEMPTY );
    frl = entry->_pentry;
    if( frl != NULL ) {
        seg = _FP_SEG( entry->_pentry );
    } else if( one_heap == _NULLSEG ) {
        /* we are starting a multi-heap walk */
        if( verifyHeapList( seg ) != _HEAPOK ) {
            return( _HEAPBADBEGIN );
        }
    }
    for( ; ; seg = next_seg ) {
        prev_seg = BHEAP( seg )->prev.segm;
        next_seg = BHEAP( seg )->next.segm;
        if( prev_seg != _NULLSEG ) {
            if( BHEAP( prev_seg )->next.segm != seg || prev_seg == next_seg ) {
                return( _HEAPBADBEGIN );
            }
        }
        if( next_seg != _NULLSEG ) {
            if( BHEAP( next_seg )->prev.segm != seg ) {
                return( _HEAPBADBEGIN );
            }
        }
        if( frl == NULL ) {
            if( BHEAP( seg )->freehead.len != 0 )
                return( _HEAPBADBEGIN );
            frl = (FRLPTR( seg ))sizeof( heapblk );
        } else {    /* advance to next entry */
            frl_next = (FRLPTR( seg ))NEXT_BLK_A( frl );
            if( frl_next <= frl )
                return( _HEAPBADNODE );
            frl = frl_next;
            if( BHEAP( seg )->len != 0 && (tag)frl > BHEAP( seg )->len ) {
                return( _HEAPBADNODE );
            }
        }
        if( !IS_BLK_END( frl ) )
            break;
        if( next_seg == _NULLSEG || one_heap != _NULLSEG ) {
            entry->_useflag = _USEDENTRY;
            entry->_size    = 0;
            entry->_pentry  = NULL;
            return( _HEAPEND );
        }
        frl = NULL;
    }
    entry->_pentry  = frl;
    entry->_useflag = _FREEENTRY;
    entry->_size    = GET_BLK_SIZE( frl );
    if( IS_BLK_INUSE( frl ) ) {
        entry->_useflag = _USEDENTRY;
    }
    return( _HEAPOK );
}
