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


#include header
#include "ringfns.h"

typedef struct ring RING;
struct ring                     // model of a ring
{   RING *next;                 // - points to next
};

#ifndef NDEBUG
static void verifyNotInRing( RING *ring, RING *elt )
{
    RING *curr;

    RingIterBeg( ring, curr ) {
        if( curr == elt ) {
            _FatalAbort( "RingFns: trying to insert element twice into a ring" );
        }
    } RingIterEnd( curr )
}
#else
#define verifyNotInRing( h, r )
#endif


void *RingFirst(                // RETURN FIRST ELEMENT OF RING
    void *hdr )                 // - ring header
{
    RING *last;

    last = hdr;
    if( last == NULL ) {
        return( last );
    }
    return( last->next );
}


void RingAppend(                // APPEND ELEMENT TO RING
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be appended
{
    RING **rhdr;                // - ring header
    RING *relement;             // - ring element
    RING *lelement;             // - last ring element, before appending

    rhdr = hdr;
    relement = element;
    verifyNotInRing( *rhdr, relement );
    lelement = *rhdr;
    if( lelement == NULL ) {
        relement->next = relement;
    } else {
        relement->next = lelement->next;
        lelement->next = relement;
    }
    *rhdr = relement;
}


void* RingPromote(              // PROMOTE ELEMENT TO START OF RING
    void *hdr,                  // - addr( ring header )
    void *elt,                  // - element to be promoted
    void *prv )                 // - element just before element
{
    RING **rhdr;
    RING *last;
    RING *prev;
    RING *element;

    rhdr = hdr;
    prev = prv;
    element = elt;
    last = *rhdr;
    if( prev == NULL || last == prev ) {
        /* already at front */
        return element;
    }
    if( last != element ) {
        /* delete */
        prev->next = element->next;
        /* insert at front */
        element->next = last->next;
        last->next = element;
    } else {
        /* last element in ring; rotate */
        last = prev;
    }
    *(void **)hdr = last;
    return element;
}


void RingInsert(                // INSERT ELEMENT INTO RING
    void *hdr,                  // - addr( ring header )
    void *element,              // - element to be inserted
    void *insert )              // - insertion point (or NULL for start)
{
    RING **rhdr;                // - ring header
    RING *relement;             // - ring element, to be inserted
    RING *ielement;             // - ring element, insertion point
    RING *lelement;             // - last ring element, before appending

    rhdr = hdr;
    relement = element;
    verifyNotInRing( *rhdr, relement );
    ielement = insert;
    lelement = *rhdr;
    if( ( lelement == NULL ) || ( lelement == ielement ) ) {
        RingAppend( hdr, element );
    } else if( ielement == NULL ) {  // insert at start of ring
        relement->next = lelement->next;
        lelement->next = relement;
    } else {
        relement->next = ielement->next;
        ielement->next = relement;
    }
}


#ifndef NDEBUG
void RingWalk(                  // TRAVERSE RING
    void *hdr,                  // - ring header
    void (*rtn)                 // - traversal routine
        ( void * curr ) )       // - - passed current element
{
    RING *relement;             // - ring element
    RingIterBegSafe( hdr, relement ) {
            (*rtn)( relement );
    } RingIterEndSafe( relement )
}
#endif


void *RingPred(                 // FIND PREVIOUS ELEMENT IN A RING
    void *hdr,                  // - ring header
    void *element )             // - element
{
    RING *rhdr;                 // - ring header
    RING *pred;                 // - previous element
    RING *next;                 // - next element

    rhdr = hdr;
    if( rhdr == NULL ) {
        pred = NULL;
    } else {
        for( pred = rhdr; ; ) {
            next = pred->next;
            if( element == next ) break;
            pred = next;
            if( pred == rhdr ) {
                pred = NULL;
                break;
            }
        }
    }
    return pred;
}

void *RingPruneWithPrev(        // PRUNE ELEMENT FROM A RING (PREV ELT AVAILABLE)
    void *hdr,                  // - addr( ring header )
    void *element,              // - element to be pruned
    void *prv )                 // - element just before element
{
    RING **rhdr;                // - addr( ring header )
    RING *relement;             // - element to be pruned
    RING *prev;                 // - previous element

    rhdr = hdr;
    relement = element;
    prev = prv;
    if( prev == NULL ) {
        prev = *rhdr;
    }
    prev->next = relement->next;
    if( prev == relement ) {
        *rhdr = NULL;
    } else {
        if( *rhdr == relement ) {
            *rhdr = prev;
        }
    }
    // this is necessary so that it is clear this element isn't part of
    // a ring anymore
    relement->next = NULL;
    return( relement );
}


void *RingPrune(                // PRUNE ELEMENT FROM A RING
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be pruned
{
    RING **rhdr;                // - addr( ring header )
    RING *relement;             // - element to be pruned
    RING *prev;                 // - previous element

    rhdr = hdr;
    relement = element;
    prev = RingPred( *rhdr, relement );
    return( RingPruneWithPrev( hdr, element, prev ) );
}


void* RingPush(                 // INSERT ELEMENT AT START OF RING
    void *_hdr,                 // - addr( ring header )
    void *element )             // - element to be pushed
{
    void **hdr = _hdr;          // - addr( ring header )
    RING *last;                 // - last element
    RING *relement;             // - element to be pruned

    last = *hdr;
    relement = element;
    verifyNotInRing( last, relement );
    if( last == NULL ) {
        relement->next = relement;
        *hdr = relement;
    } else {
        relement->next = last->next;
        last->next = relement;
    }
    return relement;
}


void *RingTop(                  // RETURN FIRST ELEMENT IN THE RING
    void *_hdr )                // - addr( ring header )
{
    void **hdr = _hdr;          // - addr( ring header )
    RING *last;                 // - last element
    RING *first;                // - first element

    first = NULL;
    last = *hdr;
    if( last != NULL ) {
        first = last->next;
    }
    return( first );
}


void *RingPop(                  // PRUNE FIRST ELEMENT IN THE RING
    void *_hdr )                // - addr( ring header )
{
    void **hdr = _hdr;          // - addr( ring header )
    RING *last;                 // - last element
    RING *first;                // - first element

    first = NULL;
    last = *hdr;
    if( last != NULL ) {
        first = last->next;
        if( first == last ) {
            *hdr = NULL;
        } else {
            last->next = first->next;
        }
        // this is necessary so that it is clear this element isn't part of
        // a ring anymore
        first->next = NULL;
    }
    return( first );
}


void *RingConcatAppend(         // APPEND A RING
    void *hdr,                  // - ring header
    void *hdr2 )                // - second ring header
{
    RING *last1 = hdr;
    RING *last2 = hdr2;
    RING *first1;

    if( last1 != NULL ) {
        if( last2 != NULL ) {
            first1 = last1->next;
            last1->next = last2->next;
            last2->next = first1;
            last1 = last2;
        }
    } else {
        last1 = last2;
    }
    return( last1 );
}


void *RingLookup(               // LOOKUP IN A RING
    void *hdr,                  // - ring hdr
    boolean (*compare_rtn)      // - comparison routine
        ( void *element,        // - - element
          const void *comparand ),// - - comparand
    const void *comparand )     // - comparand
{
    void* curr;                 // - current element
    void* found;                // - located element

    found = NULL;
    RingIterBeg( hdr, curr ) {
        if( (*compare_rtn)( curr, comparand ) ) {
            found = curr;
            break;
        }
    } RingIterEnd( curr );
    return found;
}


int RingCount(                  // COUNT ELEMENTS IN A RING
    void *hdr )                 // - ring hdr
{
    int count;                  // - number elements
    RING *curr;                 // - current element

    count = 0;
    RingIterBeg( hdr, curr ) {
        ++ count;
    } RingIterEnd( curr )
    return count;
}


void *RingStep(                 // STEP ALONG ELEMENTS (NULL -> e1 -> e2 -> NULL)
    void *hdr,                  // - ring header
    void *elt )                 // - curr element (NULL to start)
{
    RING *rhdr;                 // - ring hdr
    RING *relt;                 // - ring element

    rhdr = hdr;
    if( elt == NULL ) {
        /* start traversal */
        if( rhdr != NULL ) {
            elt = rhdr->next;
        }
    } else {
        relt = elt;
        if( relt != rhdr ) {
            elt = relt->next;
        } else {
            elt = NULL;
        }
    }
    return( elt );
}


void *RingAlloc(                // ALLOCATE AND APPEND NEW ELEMENT
    void *hdr,                  // - addr( ring header )
    size_t size )               // - size of entry to be allocated
{
    void *new_element;          // - allocated element

    new_element = (void *) _MemoryAllocate( size );
    RingAppend( hdr, new_element );
    return( new_element );
}


void RingDealloc(               // DE-ALLOCATE A RING ELEMENT
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be de-allocated
{
    RingPrune( hdr, element );
    _MemoryFree( element );
}


void RingFree(                  // FREE ALL ELEMENTS IN A RING
    void *_hdr )                // - addr( ring header )
{
    void **hdr = _hdr;
    void *elt;

    RingIterBegSafe( *hdr, elt ) {
        /* modify ring in an atomic manner */
        _MemoryFree( elt );
    } RingIterEndSafe( elt );
    *hdr = NULL;
}
