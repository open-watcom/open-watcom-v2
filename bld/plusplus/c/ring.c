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
* Description:  Ring support.
*
****************************************************************************/

/*
    Notes:  (1) These functions are not type-safe; they act on structures
                that are assumed to contain the ring pointer in the first
                position of the structure.

            (2) A ring is located by its header pointer, which is NULL to
                when the ring is empty.  Otherwise, it points at the last
                element in the ring.  Each element points to the next
                element (the last points to the first).

            (3) The traversal routines (RingWalk...) are written so that
                the next element is located before the associated routine
                is called; this means that the routine can free the current
                element without side effects.
*/

#include "plusplus.h"
#include "ring.h"

typedef struct ring RING;
struct ring                     // model of a ring
{   RING *next;                 // - points to next
};


//************************************************************************
// NOTE:: the following uses stack technology (see stack.h)
//***********************************************************************


void * StackCarveAlloc(         // CARVER ALLOC AND PUSH STACK
    carve_t carver,             // - carving control
    void *hdr )                 // - addr[ stack hdr ]
{
    void    **stack_hdr = hdr;
    void** element = CarveAlloc( carver );
    *element = *stack_hdr;
    *stack_hdr = (void*)element;
    return (void*)element;
}
