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


#ifndef __RING_H__

#ifndef RING_H_RECURSE
#define RING_H_RECURSE
#include "ring.h"
#undef RINGHNAME
#define RINGHNAME( name ) Ring##name
#define __RING_H__
#else
#undef RINGHNAME
#define RINGHNAME( name ) Ring2##name
#endif

// PROTOTYPES:

#include "carve.h"

void *RINGHNAME(Alloc) (        // ALLOCATE AND APPEND NEW ELEMENT
    void *hdr,                  // - addr( ring header )
    size_t size )               // - size of entry to be allocated
;
void* RINGHNAME(CarveAlloc) (   // CARVER ALLOC AND APPEND AN ENTRY
    carve_t carver,             // - carving control
    void *hdr )                 // - addr( ring header )
;
void RINGHNAME(CarveFree) (     // CARVER FREE ALL ELEMENTS IN A RING
    carve_t carver,             // - carving control
    void *hdr )                 // - addr( ring header )
;
void RINGHNAME(Append) (        // APPEND ELEMENT TO RING
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be appended
;
unsigned RINGHNAME(Count) (     // COUNT ELEMENTS IN A RING
    void *hdr )                 // - ring hdr
;
void RINGHNAME(Dealloc) (       // DE-ALLOCATE A RING ELEMENT
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be de-allocated
;
void RINGHNAME(Free) (          // FREE ALL ELEMENTS IN A RING
    void *hdr )                 // - addr( ring header )
;
void* RINGHNAME(Promote) (      // PROMOTE ELEMENT TO START OF RING
    void *hdr,                  // - addr( ring header )
    void *elt,                  // - element to be promoted
    void *prv )                 // - element just before element
;
void RINGHNAME(Insert) (        // INSERT ELEMENT INTO RING
    void *hdr,                  // - addr( ring header )
    void *element,              // - element to be inserted
    void *insert )              // - insertion point (or NULL for start)
;
void *RINGHNAME(Lookup) (       // LOOKUP IN A RING (also used for walks)
    void *hdr,                  // - ring hdr
    bool (*compare_rtn)         // - comparison routine
        ( void *element,        // - - element
          void *comparand ),    // - - comparand
    void *comparand )           // - comparand
;
void *RINGHNAME(Pop) (          // PRUNE FIRST ELEMENT IN THE RING
    void *hdr )                 // - addr( ring header )
;
void *RINGHNAME(Last) (         // RETURN LAST ELEMENT IN THE RING
    void *hdr )
;
void *RINGHNAME(First) (        // RETURN FIRST ELEMENT IN THE RING
    void *hdr )
;
void *RINGHNAME(Pred) (         // FIND PREDECESSOR ELEMENT IN A RING
    void *hdr,                  // - ring header
    void *element )             // - element
;
void *RINGHNAME(Prune) (        // PRUNE ELEMENT FROM A RING
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be pruned
;
void *RINGHNAME(PruneWithPrev) ( // PRUNE ELEMENT FROM A RING (PREV ELT AVAILABLE)
    void *hdr,                  // - addr( ring header )
    void *element,              // - element to be pruned
    void *prv )                 // - element just before element
;
void* RINGHNAME(Push) (         // INSERT ELEMENT AT START OF RING
    void *hdr,                  // - addr( ring header )
    void *element )             // - element to be pushed
;
void RINGHNAME(Walk) (          // TRAVERSE RING
    void *hdr,                  // - ring header
    void (*rtn)                 // - traversal routine
        (void * curr) )         // - - passed current element
;
void *RINGHNAME(Step) (         // STEP ALONG ELEMENTS (NULL -> e1 -> e2 -> NULL)
    void *hdr,                  // - ring header
    void *elt )                 // - curr element (NULL to start)
;
#endif
