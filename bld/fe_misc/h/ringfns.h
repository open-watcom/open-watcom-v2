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
* Description:  Definitions for ring support. See comments in ring.c for
*               a list of implicit assumptions.
*
****************************************************************************/


#ifndef __RINGFNS_H__
#define __RINGFNS_H__


#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// PROTOTYPES:

void *RingAlloc(                // ALLOCATE AND APPEND NEW ELEMENT
    void *hdr,                  // - addr( ring header )
    size_t size )               // - size of entry to be allocated
;
void RingAppend(                // APPEND ELEMENT TO RING
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be appended
;
int RingCount(                  // COUNT ELEMENTS IN A RING
    void *hdr )                 // - ring hdr
;
void RingDealloc(               // DE-ALLOCATE A RING ELEMENT
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be de-allocated
;
void RingFree(                  // FREE ALL ELEMENTS IN A RING
    void *hdr )                 // - addr( ring header )
;
void* RingPromote(              // PROMOTE ELEMENT TO START OF RING
    void *hdr,                  // - addr( ring header )
    void *elt,                  // - element to be promoted
    void *prv )                 // - element just before element
;
void RingInsert(                // INSERT ELEMENT INTO RING
    void *hdr,                  // - addr( ring header )
    void *element,              // - element to be inserted
    void *insert )              // - insertion point (or NULL for start)
;
void *RingLookup(               // LOOKUP IN A RING
    void *hdr,                  // - ring hdr
    bool (*compare_rtn)         // - comparison routine
        ( void *element,        // - - element
          const void *comparand ),// - - comparand
    const void *comparand )     // - comparand
;
void *RingPop(                  // PRUNE FIRST ELEMENT IN THE RING
    void *hdr )                 // - addr( ring header )
;
void *RingTop(                  // RETURN FIRST ELEMENT IN THE RING
    void *hdr )                 // - addr( ring header )
;
void *RingPred(                 // FIND PREDECESSOR ELEMENT IN A RING
    void *hdr,                  // - ring header
    void *element )             // - element
;
void *RingPrune(                // PRUNE ELEMENT FROM A RING
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be pruned
;
void *RingPruneWithPrev(        // PRUNE ELEMENT FROM A RING (PREV ELT AVAILABLE)
    void *hdr,                  // - addr( ring header )
    void *element,              // - element to be pruned
    void *prv )                 // - element just before element
;
void* RingPush(                 // INSERT ELEMENT AT START OF RING
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be pushed
;
void *RingConcatAppend(         // APPEND A RING
    void *hdr,                  // - ring header
    void *hdr2 )                // - second ring header
;
#ifndef NDEBUG
void RingWalk(                  // TRAVERSE RING
    void *hdr,                  // - ring header
    void (*rtn)                 // - traversal routine
        ( void * curr  ) )      // - - passed current element
;
#endif
void *RingStep(                 // STEP ALONG ELEMENTS (NULL -> e1 -> e2 -> NULL)
    void *hdr,                  // - ring header
    void *elt )                 // - curr element (NULL to start)
;

void *RingFirst(                // RETURN FIRST ELEMENT OF RING
    void *hdr )                 // - ring header
;

// void *RingLast( void *hdr );
#define RingLast( h )           ((void*)(h))

// ( p - prev ) returns new head
#define RingRotateToFront( p ) ((void*)(p))

// following assume that ring will not be modified

#define RingIterBeg( h, i ) \
    if( 0 != ((i) = (h)) ) { \
        void* _T = (void*)(i); \
        do { \
            *(void**)&(i) = * (void**)(i);

#define RingIterEnd( i ) \
        } while( (void*)(i) != _T ); \
    }

// want to walk pair of rings at the same time

#define RingIterBegPair( h1, i1, h2, i2 ) \
    if( ( 0 != ((i1) = (h1)) )  && ( 0 != ((i2) = (h2)) ) ) { \
        void* _T1 = (void*)(i1); \
        void* _T2 = (void*)(i2); \
        do { \
            *(void**)&(i1) = * (void**)(i1); \
            *(void**)&(i2) = * (void**)(i2);

#define RingIterEndPair( i1, i2 ) \
        } while( ( (void*)(i1) != _T1 ) && ( (void*)(i2) != _T2 ) ); \
    }


// use the following to traverse up to and including
// ring item 'to'

#define RingIterBegFrom( from, i ) \
    if( 0 != ((i) = (from)) ) { \
        for(;;) { \

#define RingIterEndTo( i, to ) \
            if( (i) == (to) ) break;\
            *(void**)&(i) = * (void**)(i); \
        } \
    }

// following allow ring to be modified

#define RingIterBegSafe( h, i ) \
    { (i) = (h); if( (i) ) { \
        void* _T = (void*)(i); \
        void* _N = * (void**) _T; \
        do { \
            *(void**)&(i) = _N; \
            _N = * (void**) (i);

#define RingIterEndSafe( i ) \
        } while( (void*)(i) != _T ); \
    }}

// Ring Thread Support
// (rings that use a specific field rather than assume where the next pointer is)
// ( h - header lvalue, e - element, f - field name )
#define _RingThreadAppend( h, e, f ) \
    if( (h) ) { \
        (e)->f = (h)->f; (h)->f = (e); (h) = (e); \
    } else { \
        (e)->f = (e); (h) = (e); \
    }

/*
    Use as follows:
    T *head;
    T *stop;
    T *curr;

    stop = _RingThreadStart( head );
    curr = NULL;
    for(;;) {
        curr = _RingThreadNext( stop, curr, link_field_name );
        if( curr == NULL ) break;
    }
*/
#define _RingThreadStart( h ) (h)
#define _RingThreadNext( s, c, f ) \
    ((s) ? ( (c) ? ((c)==(s) ? 0 : (c)->f ) : (s)->f ) : 0 )

#define _RingThreadIterBeg( h, i, f ) \
    if( 0 != ((i) = (h)) ) { \
        void* _T = (void*)(i); \
        do { \
            (i) = (i)->f;

#define _RingThreadIterEnd( i ) \
        } while( (void*)(i) != _T ); \
    }

#define _RingThreadIterBegSafe( h, i, f ) \
    { (i) = (h); if( (i) ) { \
        void* _T = (void*)(i); \
        void* _N = (void*)(i)->f; \
        do { \
            *(void**)&(i) = _N; \
            _N = (i)->f;

#define _RingThreadIterEndSafe( i ) \
        } while( (void*)i != _T ); \
    }}

// ( h - head, f - field, e - var to receive element
#define _RingThreadPopIntoVar( h, f, e ) \
    if( (h) ) { \
        (e) = (h)->f; \
        if( (h) == (e) ) { \
            (h) = 0; \
        } else { \
            (h)->f = (e)->f; \
        } \
    } else { \
        (e) = 0; \
    }

// ( h - head, e - elt, p - prev, f - field )
#define _RingThreadPruneWithPrev( h, e, p, f ) \
    { \
        if( !(p) ) { \
            (p) = (h); \
        } \
        (p)->f = (e)->f; \
        if( (p) == (e) ) { \
            (h) = 0; \
        } else { \
            if( (h) == (e) ) { \
                (h) = (p); \
            } \
        } \
        (e)->f = 0; \
    }

// ( h - head, e - elt, i - insert, f - field )
#define _RingThreadInsert( h, e, i, f ) \
    { \
        if( !(i) ) { \
            (i) = (h); \
        } \
        if( (h) ) { \
            (e)->f = (i)->f; \
            (i)->f = (e); \
            if( (h) == (i) ) { \
                (h) = (i); \
            } \
        } else { \
            (e)->f = (e); \
            (h) = (e); \
        } \
    }

// ( p - prev ) returns new head
#define _RingThreadRotateToFront( p ) (p)


#ifdef __cplusplus
};
#endif

#endif
