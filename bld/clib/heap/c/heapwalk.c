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


static int verifyHeapList( __segment seg )
{
    /* make sure list of heaps is a doubly-linked NULL terminated list */
    __segment   curr_seg;
    __segment   next_seg;
    __segment   prev_seg;

    /* check previous heaps end in NULL */
    for( curr_seg = seg; ; curr_seg = prev_seg ) {
        prev_seg = HBPTR( curr_seg )->prevseg;
        if( prev_seg == seg ) {
            return( _HEAPBADBEGIN );
        }
        if( prev_seg == _NULLSEG )
            break;
        if( HBPTR( prev_seg )->nextseg != curr_seg ) {
            return( _HEAPBADBEGIN );
        }
    }
    /* check next heaps end in NULL */
    for( curr_seg = seg; ; curr_seg = next_seg ) {
        next_seg = HBPTR( curr_seg )->nextseg;
        if( next_seg == seg ) {
            return( _HEAPBADBEGIN );
        }
        if( next_seg == _NULLSEG )
            break;
        if( HBPTR( next_seg )->prevseg != curr_seg ) {
            return( _HEAPBADBEGIN );
        }
    }
    return( _HEAPOK );
}

int __HeapWalk( struct _heapinfo *entry, __segment seg, unsigned one_heap )
{
    __segment   curr_seg;
    __segment   next_seg;
    __segment   prev_seg;
    farfrlptr   p;
    farfrlptr   q;

    if( seg == _NULLSEG )
        return( _HEAPEMPTY );
    p = entry->_pentry;
    if( p != FAR_NULL ) {
        seg = FP_SEG( p );
    } else if( one_heap == 0 ) {
        /* we are starting a multi-heap walk */
        if( verifyHeapList( curr_seg ) != _HEAPOK ) {
            return( _HEAPBADBEGIN );
        }
    }
    for( curr_seg = seg; ; curr_seg = next_seg ) {
        prev_seg = HBPTR( curr_seg )->prevseg;
        next_seg = HBPTR( curr_seg )->nextseg;
        if( prev_seg != _NULLSEG ) {
            if( HBPTR( prev_seg )->nextseg != curr_seg || prev_seg == next_seg ) {
                return( _HEAPBADBEGIN );
            }
        }
        if( next_seg != _NULLSEG ) {
            if( HBPTR( next_seg )->prevseg != curr_seg ) {
                return( _HEAPBADBEGIN );
            }
        }
        if( p == FAR_NULL ) {
            if( HBPTR( curr_seg )->freehead.len != 0 )
                return( _HEAPBADBEGIN );
            p = MK_FP( curr_seg, sizeof( heapblk ) );
        } else {    /* advance to next entry */
            q = (farfrlptr)( FP_SEG( p ) :> NEXT_BLK( p ) );
            if( q <= p )
                return( _HEAPBADNODE );
            p = q;
            if( HBPTR( curr_seg )->heaplen != 0 && p->len > HBPTR( curr_seg )->heaplen ) {
                return( _HEAPBADNODE );
            }
        }
        if( !IS_BLK_END( p ) )
            break;
        if( next_seg == _NULLSEG || one_heap != 0 ) {
            entry->_useflag = _USEDENTRY;
            entry->_size    = 0;
            entry->_pentry  = FAR_NULL;
            return( _HEAPEND );
        }
        p = FAR_NULL;
    }
    entry->_pentry  = p;
    entry->_useflag = _FREEENTRY;
    entry->_size    = GET_BLK_SIZE( p );
    if( IS_BLK_INUSE( p ) ) {
        entry->_useflag = _USEDENTRY;
    }
    return( _HEAPOK );
}
