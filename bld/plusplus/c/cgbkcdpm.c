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
#include "cgfront.h"
#include "cgback.h"
#include "codegen.h"
#include "cgbackut.h"
#include "carve.h"
#include "ring.h"
#include "initdefs.h"

typedef struct                  // cdtor_entry -- info for CDTOR value
{   void* next;                 // - next in ring
    unsigned value;             // - value of CDTOR
    call_handle handle;         // - handle for call
} cdtor_entry;

static carve_t carver_cdtors;   // carver for CDTOR entries
static cdtor_entry* ring_cdtors;// ring of pending entries


static void init(               // module initialization
    INITFINI* defn )
{
    defn = defn;
    carver_cdtors = CarveCreate( sizeof( cdtor_entry ), 8 );
}


static void fini(               // module completion
    INITFINI* defn )
{
    defn = defn;
    RingCarveFree( carver_cdtors, &ring_cdtors );
    CarveDestroy( carver_cdtors );
}


INITDEFN( cg_cdtor, init, fini );


void CgCdArgDefine(             // DEFINE CDOPT VALUE
    unsigned value )            // - cdopt value
{
    cg_name expr;               // - expression under construction
    cg_type type;               // - expression type
    cdtor_entry* cd_entry;      // - cdopt entry

    type = CgTypeOffset();
    expr = CgOffset( value );
    cd_entry = RingCarveAlloc( carver_cdtors, &ring_cdtors );
    cd_entry->value = value;
    cd_entry->handle = CallStackTopHandle();
    CgExprPush( expr, type );
}


static void processCdtor(       // PROCESS A CDTOR ENTRY IF POSSIBLE
    call_handle handle,         // - handle for call
    bool direct )               // - TRUE ==> direct call
{
    cdtor_entry* curr;          // - current entry
    cdtor_entry* prev;          // - previous entry

    prev = NULL;
    RingIterBeg( ring_cdtors, curr ) {
        if( curr->handle == handle ) {
            if( direct ) {
                CallStabCdArgSet( handle, curr->value );
            }
            RingPruneWithPrev( &ring_cdtors, curr, prev );
            CarveFree( carver_cdtors, curr );
            break;
        }
        prev = curr;
    } RingIterEnd( curr );
}


void CgCdArgUsed(               // USE A CALL-HANDLE DIRECTLY
    call_handle handle )        // - handle for call
{
    processCdtor( handle, TRUE );
}


void CgCdArgRemove(             // REMOVE CDTOR ENTRY FOR A CALL-HANDLE
    call_handle handle )        // - handle for call
{
    processCdtor( handle, FALSE );
}
