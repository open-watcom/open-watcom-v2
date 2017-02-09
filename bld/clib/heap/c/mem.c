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
* Description:  Heart of the heap manager. Do not break
*               unless you have a death wish.
*
****************************************************************************/


#include "variety.h"
#include <limits.h>
#include <malloc.h>
#include "heap.h"


#define HEAP    ((XBPTR(heapblkp, seg))(heap))
#define FRLPTR  XBPTR(freelistp, seg)

//
// input:
//      size    - #bytes to allocate
//      seg     - 16bit Intel data selector containing heap
//      heap    - address of heap control block
//                if 16bit Intel -> offset within segment
//                else           -> absolute pointer value
//
// output:
//      cstg    - "C" address of allocated storage or zero on failure
//                if 16bit Intel -> offset within segment
//                else           -> absolute pointer value
//
unsigned __MemAllocator( unsigned req_size, __segment seg, unsigned heap )
{
    unsigned    cstg;

    cstg = 0;                                   // assume the worst
    if( req_size != 0 ) {                       // quit if size is zero
        unsigned    size;

        size = __ROUND_UP_SIZE( req_size + TAG_SIZE, ROUND_SIZE );// round up size
        if( size >= req_size ) {                // quit if overflowed
            unsigned    largest;

            largest = HEAP->largest_blk;
            if( size < FRL_SIZE ) {
                size = FRL_SIZE;
            }
            if( size <= largest ) {             // quit if size too big
                FRLPTR      pcur;
                unsigned    len;

                pcur = HEAP->rover;             // start at rover
                largest = HEAP->b4rover;
                if( size <= largest ) {         // check size with rover
                    pcur = HEAP->freehead.next; // start at beginning
                    largest = 0;                // reset largest block size
                }
                for( ;; ) {                     // search free list
                    len = pcur->len;
                    if( size <= len ) {         // found one
                        break;
                    }
                    if( largest < len ) {       // update largest block size
                        largest = len;
                    }
                    pcur = pcur->next;          // advance to next entry
                                                // if back at start
                    if( pcur == (FRLPTR)&(HEAP->freehead) ) {
                        HEAP->largest_blk = largest;    // update largest
                        return( cstg );         // return 0
                    }
                }
                HEAP->b4rover = largest;        // update rover size
                HEAP->numalloc++;               // udpate allocation count
                len -= size;                    // compute leftover size
                if( len >= FRL_SIZE ) {         // if leftover big enough
                                                // split into two chunks
                    FRLPTR pprev;               // before current
                    FRLPTR pnext;               // after current
                    FRLPTR pnew;                // start of new piece

                    pnew = (FRLPTR)((PTR)pcur + size);
                    HEAP->rover = pnew;         // update rover
                    pnew->len = len;            // set new size
                    pcur->len = size;           // reset current size
                    pprev = pcur->prev;         // update next/prev links
                    pnew->prev = pprev;
                    pnext = pcur->next;
                    pnew->next = pnext;
                    pprev->next = pnew;
                    pnext->prev = pnew;
                } else {                        // just use this chunk
                    FRLPTR pprev;               // before current
                    FRLPTR pnext;               // after current

                    HEAP->numfree--;            // 1 fewer entries in free list
                    pprev = pcur->prev;
                    HEAP->rover = pprev;        // update rover
                    pnext = pcur->next;         // update next/prev links
                    pprev->next = pnext;
                    pnext->prev = pprev;
                }
                SET_BLK_INUSE( pcur );          // mark as allocated
                                                // get pointer to user area
                cstg = BLK2CPTR( pcur );
            }
        }
    }
    return( cstg );
}

//
// input:
//      cstg    - "C" address of block to free
//                if 16bit Intel -> offset within segment
//                else           -> absolute pointer value
//      seg     - 16bit Intel data selector containing heap
//      heap    - address of heap control block
//                if 16bit Intel -> offset within segment
//                else           -> absolute pointer value
//
// output:
//      none
//
void __MemFree( unsigned cstg, __segment seg, unsigned heap )
{
    if( cstg != 0 ) {                           // quit if pointer is zero
        FRLPTR pfree;

        pfree = (FRLPTR)CPTR2BLK( cstg );
        if( IS_BLK_INUSE( pfree ) ) {           // quit if storage is free
            FRLPTR      pnext;
            FRLPTR      pprev;
            FRLPTR      ptr;
            unsigned    len;

            do {                                // this allows break statement
                unsigned average;
                unsigned numfree;

                // look at next block to try and coalesce
                len = GET_BLK_SIZE( pfree );    // get next block
                pnext = (FRLPTR)((PTR)pfree + len);
                if( !IS_BLK_INUSE( pnext ) ) {  // if it is free
                    len += pnext->len;          // include the length
                    pfree->len = len;           // update pfree length
                    if( pnext == HEAP->rover ) {    // check for rover
                        HEAP->rover = pfree;    // update rover
                    }
                    pprev = pnext->prev;        // fixup next/prev links
                    pnext = pnext->next;
                    pprev->next = pnext;
                    pnext->prev = pprev;
                    HEAP->numfree--;            // reduce numfree
                    break;                      // proceed to coalesce code
                }

                // following block is not free
                // we must now try to figure out where pfree
                // is in relation to the entries in the free list
                pfree->len = len;               // remove allocated marker

                // check a few special places
                // see if pfree is:
                // - just before or just after the rover
                // - at the very beginning or very end of the heap
                pnext = HEAP->rover;            // get rover
                if( pfree < pnext ) {           // where is pfree?
                                                // pfree is before rover
                    if( pfree > pnext->prev ) { // where is pfree?
                                                // pfree is next to rover
                        break;                  // proceed to coalesce code
                    }
                    pnext = HEAP->freehead.next;    // get start of free list
                    if( pfree < pnext ) {       // where is pfree?
                                                // pfree is at start of list
                        break;                  // proceed to coalesce code
                    }
                } else {                        // pfree is after rover
                    pnext = pnext->next;        // pnext is after rover
                    if( pfree < pnext ) {       // where is pfree?
                                                // pfree is just after rover
                        break;                  // proceed to coalesce code
                    }
                                                // get end of free list
                    pnext = (FRLPTR)&(HEAP->freehead);
                    pprev = pnext->prev;
                    if( pfree > pprev ) {       // where is pfree?
                                                // pfree is at end of list
                        break;                  // proceed to coalesce code
                    }
                }

                // Calculate the average number of allocated blocks we may
                // have to skip until we expect to find a free block.  If
                // this number is less than the total number of free blocks,
                // chances are that we can find the correct position in the
                // free list by scanning ahead for a free block and linking
                // this free block before the found free block.  We protect
                // ourself against the degenerate case where there is an
                // extremely long string of allocated blocks by limiting the
                // number of blocks we will search to twice the calculated
                // average.

                numfree = HEAP->numfree;
                average = HEAP->numalloc / (numfree+1);
                if( average < numfree ) {
                    unsigned    worst;

                    // There are lots of allocated blocks and lots of free
                    // blocks.  On average we should find a free block
                    // quickly by following the allocated blocks, but the
                    // worst case can be very bad.  So, we try scanning the
                    // allocated blocks and give up once we have looked at
                    // twice the average.

                    worst = HEAP->numalloc - numfree;
                    average *= 2;               // give up after this many
                    if( worst <= numfree ) {
                        average = UINT_MAX;     // we won't give up loop
                    }
                                                // point at next allocated
                    pnext = (FRLPTR)NEXT_BLK( pfree );
                    for( ;; ) {
                        if( IS_BLK_END( pnext ) )   // check for end TAG
                            break;              // stop at end tag
                        if( IS_BLK_INUSE( pnext ) ) {   // pnext is allocated
                            pnext = (FRLPTR)NEXT_BLK_A( pnext );
                            average--;
                            if( average == 0 ) {    // give up search
                                break;
                            }
                        } else {
                            // break twice!
                            goto found_it;      // we have the spot
                        }
                    }
                }

                // when all else fails, search the free list
                pnext = HEAP->rover;            // begin at rover
                if( pfree < pnext ) {           // is pfree before rover?
                                                // then begin at start
                    pnext = HEAP->freehead.next;
                }
                for( ;; ) {
                    if( pfree < pnext ) {       // if pfree before pnext
                        break;                  // we found it
                    }
                    pnext = pnext->next;        // advance pnext

                    if( pfree < pnext ) {       // if pfree before pnext
                        break;                  // we found it
                    }
                    pnext = pnext->next;        // advance pnext

                    if( pfree < pnext ) {       // if pfree before pnext
                        break;                  // we found it
                    }
                    pnext = pnext->next;        // advance pnext
                }
            } while( 0 );                       // only do once

found_it:
            // if we are here, then we found the spot
            pprev = pnext->prev;                // setup pprev

            // pprev, pfree, pnext are all setup
            len = pfree->len;
                                                // check pprev and pfree
            ptr = (FRLPTR)NEXT_BLK( pprev );
            if( ptr == pfree ) {                // are they adjacent?
                                                // coalesce pprev and pfree
                len += pprev->len;              // udpate len
                pprev->len = len;
                if( HEAP->rover == pfree ) {    // check rover impact
                    HEAP->rover = pprev;        // update rover
                }
                pfree = pprev;                  // now work with coalesced blk
            } else {
                HEAP->numfree++;                // one more free entry
                pfree->next = pnext;            // update next/prev entries
                pfree->prev = pprev;
                pprev->next = pfree;
                pnext->prev = pfree;
            }
            HEAP->numalloc--;                   // one fewer allocated

            if( pfree < HEAP->rover ) {         // check rover impact
                if( HEAP->b4rover < len ) {     // is len bigger than b4rover
                    HEAP->b4rover = len;        // then update b4rover
                }
            }
            if( HEAP->largest_blk < len ) {     // check largest block
                HEAP->largest_blk = len;
            }
        }
    }
}
