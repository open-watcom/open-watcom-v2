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


#include "plusplus.h"

#include <float.h>

#include "cgback.h"
#include "cgbackut.h"
#include "typesig.h"
#include "ctexcept.h"
#include "carve.h"
#include "ring.h"
#include "initdefs.h"

struct cond_label               // COND_LABEL -- label for conditional DTOR
{   COND_LABEL* next;           // - next in list
    SE* se;                     // - state entry
    label_handle label;         // - label for state
};


static carve_t carveCOND_LABEL;     // allocations for COND_LABEL


static COND_LABEL* condLabelFind// LOCATE SAVED CONDITIONAL LABEL
    ( COND_LABEL** a_ring       // - addr[ ring of labels ]
    , SE* se )                  // - state entry
{
    COND_LABEL* curr;           // - current label
    COND_LABEL* lab;            // - label for state entry

    lab = NULL;
    RingIterBeg( *a_ring, curr ) {
        if( se == curr->se ) {
            lab = curr;
            break;
        }
    } RingIterEnd( curr )
    return lab;
}


label_handle CondLabelAdd       // ADD A CONDITIONAL LABEL
    ( COND_LABEL** a_ring       // - addr[ ring of labels ]
    , SE* se )                  // - state entry
{
    COND_LABEL* lab;            // - conditional label

    lab = condLabelFind( a_ring, se );
    if( lab == NULL ) {
        lab = RingCarveAlloc( carveCOND_LABEL, a_ring );
        lab->se = se;
        lab->label = BENewLabel();
    }
    return lab->label;
}


SE* CondLabelNext               // FIND STATE ENTRY FOR NEXT COND. LABEL
    ( COND_LABEL** a_ring       // - addr[ ring of labels ]
    , SE* se )                  // - default state entry
{
    COND_LABEL* curr;           // - conditional label for current state

    RingIterBeg( *a_ring, curr ) {
        if( se == NULL
         || ( NULL != curr->se
           && curr->se->base.state_var >= se->base.state_var ) ) {
            se = curr->se;
        }
    } RingIterEnd( curr )
    return se;
}


static void cgCondLabel         // CODE-GENERATE CONDITIONAL LABEL
    ( COND_LABEL** a_ring       // - addr[ ring of labels ]
    , COND_LABEL* lab )         // - conditional label
{
    RingPrune( a_ring, lab );
    CgLabel( lab->label );
    CarveFree( carveCOND_LABEL, lab );
}


bool CondLabelEmit              // EMIT CONDITIONAL LABEL IF REQ'D
    ( COND_LABEL** a_ring       // - addr[ ring of labels ]
    , SE* se )                  // - state entry at current position
{
    COND_LABEL* lab;            // - conditional label for current state
    bool retn;                  // - return: true ==> label emitted

    lab = condLabelFind( a_ring, se );
    if( lab == NULL ) {
        retn = false;
    } else {
        cgCondLabel( a_ring, lab );
        retn = true;
    }
    return retn;
}


void CondLabelsEmit(            // EMIT ANY REMAINING CONDITIONAL LABELS
    COND_LABEL** a_ring )       // - addr[ ring of labels ]
{
    COND_LABEL* lab;            // - conditional label pushed

    RingIterBegSafe( *a_ring, lab ) {
        cgCondLabel( a_ring, lab );
    } RingIterEndSafe( lab )
}


static void condLabelInit(      // INITIALIZATION FOR CGBKCLAB
    INITFINI* defn )            // - definition
{
    defn = defn;
    carveCOND_LABEL     = CarveCreate( sizeof( COND_LABEL ),        16 );
}


static void condLabelFini(      // COMPLETION FOR CGBKCLAB
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carveCOND_LABEL );
}


INITDEFN( cond_label, condLabelInit, condLabelFini )
