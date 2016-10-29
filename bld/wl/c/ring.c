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


#include "linkstd.h"
#include "ring.h"
#include "alloc.h"

#ifdef PARAM2
#define RINGNAME( name ) Ring2##name
#else
#define RINGNAME( name ) Ring##name
#endif

typedef struct ring RING;
struct ring                     // model of a ring
{
#ifdef PARAM2
        void *filler;
#endif
        RING *next;                 // - points to next
};

// following assume that ring will not be modified

#define RingIterBeg( h, i ) \
    if( (i = h) != 0 ) { \
        RING* _T = i; \
        do { \
            i = i->next;

#define RingIterEnd( i ) \
        } while( i != _T ); \
    }

// following allow ring to be modified

#define RingIterBegSafe( h, i ) \
    if( (i = h) != 0 ) { \
        RING* _T = i; \
        RING* _N = _T->next; \
        do { \
            i = _N; \
            _N = i->next;

#define RingIterEndSafe( i ) \
        } while( i != _T ); \
    }

#ifndef NDEBUG
static void verifyNotInRing( RING *ring, RING *elt )
{
    RING *curr;

    RingIterBeg( ring, curr ) {
        if( curr == elt ) {
            LnkFatal( "trying to insert element twice into a ring" );
        }
    } RingIterEnd( curr )
}
#else
#define verifyNotInRing( h, r )
#endif


void RINGNAME(Append) (         // APPEND ELEMENT TO RING
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


void* RINGNAME(Promote) (       // PROMOTE ELEMENT TO START OF RING
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
    *(RING **)hdr = last;
    return element;
}


void RINGNAME(Insert) (         // INSERT ELEMENT INTO RING
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
        RINGNAME(Append)( hdr, element );
    } else if( ielement == NULL ) {  // insert at start of ring
        relement->next = lelement->next;
        lelement->next = relement;
    } else {
        relement->next = ielement->next;
        ielement->next = relement;
    }
}


void RINGNAME(Walk) (           // TRAVERSE RING
    void *hdr,                  // - ring header
    ringwalk_fn *rtn )          // - traversal routine
{
#if 0
    RING *rhdr;                 // - ring header
    RING *relement;             // - ring element
    RING *nelement;             // - next ring element

    if( hdr != NULL ) {
        rhdr = hdr;
        nelement = rhdr->next;
        do {
            relement = nelement;
            nelement = nelement->next;
            (*rtn)( relement ) );
        } while( relement != rhdr );
    }
#else
    RING *relement;             // - ring element
    RingIterBegSafe( hdr, relement ) {
            (*rtn)( relement );
    } RingIterEndSafe( relement )
#endif
}


void * RINGNAME(Pred)(          // FIND PREVIOUS ELEMENT IN A RING
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
    return( pred );
}

void *RINGNAME(PruneWithPrev) ( // PRUNE ELEMENT FROM A RING (PREV ELT AVAILABLE)
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
    relement->next = NULL;
    return( relement );
}


void *RINGNAME(Prune) (         // PRUNE ELEMENT FROM A RING
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be pruned
{
    RING **rhdr;                // - addr( ring header )
    RING *relement;             // - element to be pruned
    RING *prev;                 // - previous element

    rhdr = hdr;
    relement = element;
    prev = RINGNAME(Pred)( *rhdr, relement );
    return( RINGNAME(PruneWithPrev)( hdr, element, prev ) );
}


void* RINGNAME(Push) (          // INSERT ELEMENT AT START OF RING
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be pushed
{
    RING **rhdr;                // - addr( ring header )
    RING *last;                 // - last element
    RING *relement;             // - element to be pruned

    rhdr = hdr;
    last = *rhdr;
    relement = element;
    verifyNotInRing( last, relement );
    if( last == NULL ) {
        relement->next = relement;
        *rhdr = relement;
    } else {
        relement->next = last->next;
        last->next = relement;
    }
    return relement;
}

void * RINGNAME(Last) (         // RETURN LAST ELEMENT IN THE RING
    void *hdr )                 // - ring header
{
    return hdr;
}

void * RINGNAME(First) (        // RETURN FIRST ELEMENT IN THE RING
    void *hdr )                 // - ring header
{
    return ((RING *)hdr)->next;
}

void * RINGNAME(Pop) (          // PRUNE FIRST ELEMENT IN THE RING
    void *hdr )                 // - addr( ring header )
{
    RING **rhdr;                // - addr( ring header )
    RING *last;                 // - last element
    RING *first;                // - first element

    rhdr = hdr;
    first = NULL;
    last = *rhdr;
    if( last != NULL ) {
        first = last->next;
        if( first == last ) {
            *rhdr = NULL;
        } else {
            last->next = first->next;
        }
    }
    return( first );
}


void * RINGNAME(Lookup) (       // LOOKUP IN A RING
    void *hdr,                  // - ring hdr
    ringcompare_fn *rtn,        // - comparison routine
    void *comparand )           // - comparand
{
    RING *rhdr;                 // - ring hdr
    RING *curr;                 // - current element

    if( hdr == NULL ) {
        curr = NULL;
    } else {
        rhdr = hdr;
        curr = rhdr;
        for( ; ; ) {
            curr = curr->next;
            if( (*rtn)( curr, comparand ) ) break;
            if( curr == rhdr ) {
                curr = NULL;
                break;
            }
        };
    }
    return( curr );
}


unsigned RINGNAME(Count) (      // COUNT ELEMENTS IN A RING
    void *hdr )                 // - ring hdr
{
    unsigned count;             // - number elements
    RING *curr;                 // - current element

    count = 0;
    RingIterBeg( hdr, curr ) {
        ++count;
    } RingIterEnd( curr )
    return count;
}

void *RINGNAME(Alloc) (         // ALLOCATE AND APPEND NEW ELEMENT
    void *hdr,                  // - addr( ring header )
    size_t size )               // - size of entry to be allocated
{
    void *new_element;          // - allocated element

    _ChkAlloc( new_element, size );
    RINGNAME(Append)( hdr, new_element );
    return( new_element );
}


void RINGNAME(Dealloc) (        // DE-ALLOCATE A RING ELEMENT
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be de-allocated
{
    RINGNAME(Prune)( hdr, element );
    _LnkFree( element );
}


void RINGNAME(Free) (           // FREE ALL ELEMENTS IN A RING
    void *hdr )                 // - addr( ring header )
{
    void *elt;

    for(;;) {
        /* modify ring in an atomic manner */
        elt = RINGNAME(Pop)( hdr );
        if( elt == NULL ) break;
        _LnkFree( elt );
    }
}



//************************************************************************
// NOTE:: the following use carving technology
//***********************************************************************

#include "carve.h"


void* RINGNAME(CarveAlloc) (    // CARVER ALLOC AND APPEND AN ENTRY
    carve_t carver,             // - carving control
    void *hdr )                 // - addr( ring header )
{
    void *elt;

    elt = CarveAlloc( carver );
    RINGNAME(Append)( hdr, elt );
    return elt;
}


void RINGNAME(CarveFree) (      // CARVER FREE ALL ELEMENTS IN A RING
    carve_t carver,             // - carving control
    void *hdr )                 // - addr( ring header )
{
    void *elt;

    for(;;) {
        elt = RINGNAME(Pop)( hdr );
        if( elt == NULL ) break;
        CarveFree( carver, elt );
    }
}


void *RINGNAME(Step) (  // STEP ALONG ELEMENTS (NULL -> e1 -> e2 -> NULL)
    void *hdr,          // - ring header
    void *elt )         // - curr element (NULL to start)
{
    RING *rhdr;         // - ring hdr
    RING *relt;         // - ring element

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
