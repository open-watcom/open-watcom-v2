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
#include "tinyio.h"
#include "dosheap.h"
#include "dbgswtch.h"

extern struct heapstart         *LastSeg;
extern void                     *SyMemBeg;
extern void                     *SyMemEnd;

#pragma aux __GrowSeg modify[];
unsigned __GrowSeg( unsigned short seg, unsigned int amount )
    {
        unsigned n;             /* number of paragraphs desired   */
        unsigned int old_heaplen;
        unsigned int old_heap_paras;
        struct heapblk __far *p;
        struct freelist __far *pfree;
        struct freelist __far *pnew;
        tag __far *last_tag;

        p = (struct heapblk __far *)MK_FP( seg, 0 );
        old_heaplen = p->heaplen;
        if( old_heaplen != 0 ) {                /* if not already 64K */
            amount += sizeof(tag);                      /* 25-feb-91 */
            if( amount < sizeof(tag) ) amount = ~0;
            if( amount < _amblksiz )  amount = _amblksiz;
            n = ( amount + 0x0f ) >> 4;
            if( n == 0 )  n = PARAS_IN_64K;     /* 23-may-89 */
            old_heap_paras = old_heaplen >> 4;
            n += old_heap_paras;
            /*
                We shouldn't extend segments to 64k if we are not going to
                use the space for this allocation.  In protected-mode
                environments, it should be possible to extend segments
                later on when we know we can use the space.
            */
            if( n > PARAS_IN_64K ) n = PARAS_IN_64K;



            if( LastSeg != MK_FP( seg, 0 ) ) {
                if( seg <= FP_SEG( SyMemEnd ) && seg >= FP_SEG( SyMemBeg ) ) {
                    return( 0 );
                } else if ( _IsOn( SW_REMOTE_LINK ) ) {
                    if( TINY_ERROR( TinySetBlock( n, seg ) ) ) return( 0 );
                }
            } else if( FP_SEG( LastSeg ) + n < FP_SEG( LastSeg )
                        || FP_SEG( LastSeg ) + n > FP_SEG( SyMemEnd ) ) {
                return( 0 );
            }



            p->heaplen = n << 4;        /* put in new heap length */
            pfree = MK_FP( seg, p->freehead.prev );
            if( FP_OFF(pfree) + pfree->len != old_heaplen - sizeof(tag)*2 ) {
                /* last free entry not at end of the heap */
                /* add a new free entry to end of list */
                pnew = MK_FP( seg, old_heaplen - sizeof(tag)*2 );
                pnew->prev = FP_OFF(pfree);
                pnew->next = pfree->next;
                pfree->next = FP_OFF(pnew);
                p->freehead.prev = FP_OFF(pnew);
                p->numfree++;
                pfree = pnew;
            }
            pfree->len = p->heaplen - FP_OFF(pfree) - sizeof(tag)*2;
            if( pfree->len > p->largest_blk )  p->largest_blk = pfree->len;
            last_tag = MK_FP( seg, p->heaplen - sizeof(tag)*2 );
            *last_tag = END_TAG;
            last_tag[1] = 0;            /* link to next piece of near heap */
            return( 1 );                /* indicate segment was grown */
        }
        return( 0 );    /* indicate failed to grow the segment */
    }
