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


#error nothing in this module is ever referenced (94/05/10 AFS)
#include <float.h>

#include "plusplus.h"
#include "cgfront.h"
#include "cgback.h"
#include "carve.h"
#include "codegen.h"
#include "cgbackut.h"
#include "ring.h"
#include "initdefs.h"

#ifndef NDEBUG
    #include "dbg.h"
    #include "toggle.h"
#endif


typedef struct vftgen VFTGEN;


struct vftgen {                 // VFTGEN -- VFT to be generated
    VFTGEN* next;               // - next in ring
    SYMBOL sym;                 // - symbol for table
};


static VFTGEN* vfts;            // allocated VFT's
static carve_t carve_gen;       // allocation control


static void vftAdd(             // ADD A VFT ENTRY
    SYMBOL vft )                // - symbol to be added
{
    VFTGEN* gen;                // - entry for VFT

    gen = RingCarveAlloc( carve_gen, &vfts );
    gen->sym = vft;
}


static VFTGEN* findVftGen(      // LOCATE VFTGEN FOR SYMBOL
    SYMBOL vft )                // - VFT symbol
{
    VFTGEN* retn;               // - entry to be returned
    VFTGEN* curr;               // - current entry

    retn = NULL;
    RingIterBeg( vfts, curr ) {
        if( curr->sym == vft ) {
            retn = curr;
            break;
        }
    } RingIterEnd( curr );
    return retn;
}


void BeVftRef(                  // REFERENCE TO VFT
    SYMBOL vft )                // - VFT symbol
{
    if( NULL == findVftGen( vft ) ) {
        vftAdd( vft );
    }
}


bool BeVftReqd(                 // CHECK IF VFT REQUIRED
    SYMBOL vft )                // - the VFT
{
    return NULL != findVftGen( vft );
}


static void init(               // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    vfts = NULL;
    carve_gen = CarveCreate( sizeof( VFTGEN ), 16 );
}


static void fini(               // COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carve_gen );
}


INITDEFN( vft_ctl, init, fini );
