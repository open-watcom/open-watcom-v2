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


#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <dos.h>
#include "bool.h"
#include "tinyio.h"
#include "heap.h"
#include "dbgdefn.h"
#include "dbgdata.h"


#define ROUND_DOWN_SIZE_TO_PARA( __x )  ((__x)>>4)
#define ROUND_UP_SIZE_TO_PARA( __x )    (((__x)+15)>>4)

extern __segment        LastSeg;
extern void             *SyMemBeg;
extern void             *SyMemEnd;

#pragma aux __GrowSeg __modify[];
int __GrowSeg( __segment seg, unsigned int amount )
{
    unsigned        num_of_paras;   /* number of paragraphs desired   */
    unsigned        new_heaplen;
    unsigned int    old_heaplen;
    unsigned int    old_num_of_paras;
    FRLPTR( seg )   pfree;
    FRLPTR( seg )   pnew;

    old_heaplen = BHEAP( seg )->len;
    if( old_heaplen == 0 )
        return( 0 );    /* indicate failed to grow the segment */
    /* if not already 64K */
    amount += TAG_SIZE;
    if( amount < TAG_SIZE )
        amount = /*0x....ffff*/ ~0U;
    if( amount < _amblksiz )
        amount = _amblksiz;
    num_of_paras = ROUND_UP_SIZE_TO_PARA( amount );
    if( num_of_paras == 0 )
        num_of_paras = PARAS_IN_64K;
    old_num_of_paras = ROUND_DOWN_SIZE_TO_PARA( old_heaplen );
    num_of_paras += old_num_of_paras;
    /*
        We shouldn't extend segments to 64k if we are not going to
        use the space for this allocation.  In protected-mode
        environments, it should be possible to extend segments
        later on when we know we can use the space.
    */
    if( num_of_paras > PARAS_IN_64K )
        num_of_paras = PARAS_IN_64K;

/* modification start */
    if( LastSeg != seg ) {
        if( seg <= FP_SEG( SyMemEnd ) && seg >= FP_SEG( SyMemBeg ) ) {
            return( 0 );
        } else if ( _IsOn( SW_REMOTE_LINK ) ) {
            if( TINY_ERROR( TinySetBlock( num_of_paras, seg ) ) ) {
                return( 0 );
            }
        }
    } else if( LastSeg + num_of_paras < LastSeg
                || LastSeg + num_of_paras > FP_SEG( SyMemEnd ) ) {
        return( 0 );
    }
/* modification end */

    new_heaplen = num_of_paras << 4;
    BHEAP( seg )->len = new_heaplen;        /* put in new heap length */
    pfree = BHEAP( seg )->freehead.prev.nptr;
    if( NEXT_BLK( pfree ) != old_heaplen - TAG_SIZE * 2 ) {
        /* last free entry not at end of the heap */
        /* add a new free entry to end of list */
        pnew = (FRLPTR( seg ))( old_heaplen - TAG_SIZE * 2 );
        pnew->prev.nptr = pfree;
        pnew->next.nptr = pfree->next.nptr;
        pfree->next.nptr = pnew;
        BHEAP( seg )->freehead.prev.nptr = pnew;
        BHEAP( seg )->numfree++;
        pfree = pnew;
    }
    pfree->len = new_heaplen - FP_OFF( pfree ) - TAG_SIZE * 2;
    if( BHEAP( seg )->largest_blk < pfree->len )
        BHEAP( seg )->largest_blk = pfree->len;
    SET_HEAP_END( seg, new_heaplen - 2 * TAG_SIZE );
    return( 1 );                /* indicate segment was grown */
}
