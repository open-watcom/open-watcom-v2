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


#if defined( _M_I86 )
    extern __segment setup_ds( __segment );
    #pragma aux setup_ds = \
                "push ax" \
                "mov ax,ds" \
                "pop ds" \
                parm [ax] value [ax];
    #define setup_segment( _x ) _x = setup_ds( _x );
#else
    #define setup_segment( _x ) (void)(_x = _x);
#endif

//
// input:
//      size    - #bytes to allocate
//      segment - 16bit Intel data selector containing heap
//      offset  - address of heap control block
//                if 16bit Intel -> offset within segment
//                else           -> absolute pointer value
//
// output:
//      result  - address of allocated storage or zero on failure
//                if 16bit Intel -> offset within segment
//                else           -> absolute pointer value
//
unsigned __MemAllocator( unsigned size, __segment segment, unsigned offset )
{
    frlptr  result;

    result = 0;                                 // assume the worst
    setup_segment( segment );                   // setup DS for 16bit Intel

    if( size != 0 ) {                           // quit if size is zero
        unsigned    new_size;

        new_size = size + TAG_SIZE + ROUND_SIZE;// round up size
        if( new_size >= size ) {                // quit if overflowed
            heapblkp    _WCI86NEAR *heap;
            unsigned    largest;

            heap = (heapblkp _WCI86NEAR *)offset;
            size = new_size & ~ROUND_SIZE;      // make size even
            largest = heap->largest_blk;
            if( size < FRL_SIZE ) {
                size = FRL_SIZE;
            }
            if( size <= largest ) {             // quit if size too big
                frlptr      pcur;
                unsigned    len;

                pcur = heap->rover;             // start at rover
                largest = heap->b4rover;
                if( size <= largest ) {         // check size with rover
                    pcur = heap->freehead.next; // start at beginning
                    largest = 0;                // reset largest block size
                }
                for( ;; ) {                     // search free list
                    len = pcur->len;
                    if( size <= len ) {         // found one
                        break;
                    }
                    if( len > largest ) {       // update largest block size
                        largest = len;
                    }
                    pcur = pcur->next;          // advance to next entry
                    if( pcur ==                 // if back at start
                        (frlptr)&(heap->freehead)) {
                        heap->largest_blk = largest;    // update largest
                        setup_segment( segment );       // 16bit Intel restore
                        return( (unsigned)result );     // return 0
                    }
                }
                heap->b4rover = largest;        // update rover size
                heap->numalloc++;               // udpate allocation count
                len -= size;                    // compute leftover size
                if( len >= FRL_SIZE ) {         // if leftover big enough
                                                // split into two chunks
                    frlptr pprev;               // before current
                    frlptr pnext;               // after current
                    frlptr pnew;                // start of new piece

                    pnew = (frlptr)((PTR)pcur + size);
                    heap->rover = pnew;         // update rover
                    pnew->len = len;            // set new size
                    pcur->len = size;           // reset current size
                    pprev = pcur->prev;         // update next/prev links
                    pnew->prev = pprev;
                    pnext = pcur->next;
                    pnew->next = pnext;
                    pprev->next = pnew;
                    pnext->prev = pnew;
                } else {                        // just use this chunk
                    frlptr pprev;               // before current
                    frlptr pnext;               // after current

                    heap->numfree--;            // 1 fewer entries in free list
                    pprev = pcur->prev;
                    heap->rover = pprev;        // update rover
                    pnext = pcur->next;         // update next/prev links
                    pprev->next = pnext;
                    pnext->prev = pprev;
                }
                pcur->len |= 1;                 // mark as allocated
                                                // get pointer to user area
                result = (frlptr)((PTR)pcur + TAG_SIZE);
            }
        }
    }
    setup_segment( segment );                   // 16bit Intel restore
    return( (unsigned)result );
}

//
// input:
//      pointer - address of block to free
//                if 16bit Intel -> offset within segment
//                else           -> absolute pointer value
//      segment - 16bit Intel data selector containing heap
//      offset  - address of heap control block
//                if 16bit Intel -> offset within segment
//                else           -> absolute pointer value
//
// output:
//      none
//
void __MemFree( unsigned pointer, __segment segment, unsigned offset )
{
    setup_segment( segment );                   // setup DS for 16bit Intel

    if( pointer != 0 ) {                        // quit if pointer is zero
        frlptr pfree;

        pfree = (frlptr)(pointer - TAG_SIZE);
        if( pfree->len & 1 ) {                  // quit if storage is free
            heapblkp    _WCI86NEAR *heap;
            frlptr      pnext;
            frlptr      pprev;
            frlptr      ptr;
            unsigned    len;

            heap = (heapblkp _WCI86NEAR *)offset;
            do {                                // this allows break statement
                unsigned average;
                unsigned numfree;

                // look at next block to try and coalesce
                len = pfree->len & ~1;          // get next block
                pnext = (frlptr)((PTR)pfree + len);
                if( (pnext->len & 1) == 0 ) {   // if it is free
                    len += pnext->len;          // include the length
                    pfree->len = len;           // update pfree length
                    if( pnext == heap->rover ) {    // check for rover
                        heap->rover = pfree;    // update rover
                    }
                    pprev = pnext->prev;        // fixup next/prev links
                    pnext = pnext->next;
                    pprev->next = pnext;
                    pnext->prev = pprev;
                    heap->numfree--;            // reduce numfree
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
                pnext = heap->rover;            // get rover
                if( pfree < pnext ) {           // where is pfree?
                                                // pfree is before rover
                    if( pfree > pnext->prev ) { // where is pfree?
                                                // pfree is next to rover
                        break;                  // proceed to coalesce code
                    }
                    pnext = heap->freehead.next;    // get start of free list
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
                    pnext = (frlptr)&(heap->freehead);
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

                numfree = heap->numfree;
                average = heap->numalloc / (numfree+1);
                if( average < numfree ) {

                    // There are lots of allocated blocks and lots of free
                    // blocks.  On average we should find a free block
                    // quickly by following the allocated blocks, but the
                    // worst case can be very bad.  So, we try scanning the
                    // allocated blocks and give up once we have looked at
                    // twice the average.

                    unsigned worst;
                    worst = heap->numalloc - numfree;
                    average *= 2;               // give up after this many
                    if( worst <= numfree ) {
                        average = UINT_MAX;     // we won't give up loop
                    }
                                                // point at next allocated
                    pnext = (frlptr)((PTR)pfree + pfree->len);
                    for(;;) {
                        len = pnext->len;
                        if( len & 1 ) {         // pnext is allocated
                            if( len != END_TAG ) {  // check for end TAG
                                len &= ~1;          // advance pnext
                                pnext = (frlptr)((PTR)pnext + len);
                                average--;
                                if( !average ) {    // give up search
                                    break;
                                }
                            } else {
                                break;          // stop at end tag
                            }
                        } else {
                            // break twice!
                            goto found_it;      // we have the spot
                        }
                    }
                }

                // when all else fails, search the free list
                pnext = heap->rover;            // begin at rover
                if( pfree < pnext ) {           // is pfree before rover?
                                                // then begin at start
                    pnext = heap->freehead.next;
                }
                for(;;) {
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
            ptr = (frlptr)((PTR)pprev + pprev->len);
            if( ptr == pfree ) {                // are they adjacent?
                                                // coalesce pprev and pfree
                len += pprev->len;              // udpate len
                pprev->len = len;
                if( heap->rover == pfree ) {    // check rover impact
                    heap->rover = pprev;        // update rover
                }
                pfree = pprev;                  // now work with coalesced blk
            } else {
                heap->numfree++;                // one more free entry
                pfree->next = pnext;            // update next/prev entries
                pfree->prev = pprev;
                pprev->next = pfree;
                pnext->prev = pfree;
            }
            heap->numalloc--;                   // one fewer allocated

            if( pfree < heap->rover ) {         // check rover impact
                if( len > heap->b4rover ) {     // is len bigger than b4rover
                    heap->b4rover = len;        // then update b4rover
                }
            }

            if( len > heap->largest_blk ) {     // check largest block
                heap->largest_blk = len;
            }
        }
    }

    setup_segment( segment );                   // 16bit Intel restore
}
