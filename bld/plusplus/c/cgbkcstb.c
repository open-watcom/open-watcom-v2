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
#include "memmgr.h"
#include "errdefns.h"
#include "codegen.h"
#include "cgbackut.h"
#include "ring.h"
#include "toggle.h"
#include "initdefs.h"

static carve_t carve_call_stab; // carve control: CALL_STAB


#ifndef NDEBUG

#include <stdio.h>
#include "pragdefn.h"

static void __dump( const char* text, CALL_STAB* cstb )
{
    if( PragDbgToggle.dump_stab ) {
        printf( "CALL_STAB[%x] %s handle(%x) se(%x) has_cd_arg(%d) cd_arg(%x)\n"
              , cstb
              , text
              , cstb->handle
              , cstb->se
              , cstb->has_cd_arg
              , cstb->cd_arg );
    }
}

#else

    #define __dump(a,b)

#endif


CALL_STAB* CallStabAlloc(       // ALLOCATE CALL_STAB
    call_handle handle,         // - handle for call
    FN_CTL* fctl )              // - function hosting the call
{
    CALL_STAB* cstb;            // - call information

    cstb = RingCarveAlloc( carve_call_stab, &fctl->expr_calls );
    cstb->handle = handle;
    cstb->se = FstabMarkedPosn();
    cstb->has_cd_arg = FALSE;
    cstb->cd_arg = 0;
    CgCdArgUsed( handle );
    __dump( "allocate", cstb );
    return cstb;
}


static CALL_STAB* callStabEntry(// GET CALL_STAB FOR A HANDLE
    call_handle handle )        // - handle for call
{
    FN_CTL* fctl;               // - top function information
    CALL_STAB* curr;            // - call information (current)
    CALL_STAB* retn;            // - call information (returned)

    retn = NULL;
    if( 0 != handle ) {
        fctl = FnCtlTop();
        RingIterBeg( fctl->expr_calls, curr ) {
            if( curr->handle == handle ) {
                retn = curr;
                break;
            }
        } RingIterEnd( curr );
    }
    return retn;
}


boolean CallStabCdArgGet(       // GET CD-ARG FOR A CALL
    call_handle handle,         // - handle for call
    unsigned *a_cd_arg )        // - addr[ value for CD-ARG ]
{
    CALL_STAB* cstb;            // - call information
    boolean retn;               // - TRUE ==> have CDTOR arg.

    cstb = callStabEntry( handle );
    if( cstb != NULL && cstb->has_cd_arg ) {
        *a_cd_arg = cstb->cd_arg;
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return retn;
}


unsigned CallStabCdArgSet(      // SET CD-ARG FOR A CALL
    call_handle handle,         // - handle for call
    unsigned cd_arg )           // - value for CD-ARG
{
    CALL_STAB* cstb;            // - call information

    cstb = callStabEntry( handle );
    if( cstb != NULL ) {
        cstb->has_cd_arg = TRUE;
        cstb->cd_arg = cd_arg;
    }
    return cd_arg;
}


void CallStabFree(              // FREE CALL_STAB
    FN_CTL* fctl,               // - function hosting the call
    CALL_STAB* cstb )           // - call information
{
    RingPrune( &fctl->expr_calls, cstb );
    CarveFree( carve_call_stab, cstb );
}


SE* CallStabStateTablePosn(     // GET STATE-TABLE POSITION AT CALL POINT
    call_handle handle )        // - handle for inline call
{
    CALL_STAB* curr;            // - current CALL_STAB entry

    curr = callStabEntry( handle );
    DbgVerify( curr, "CallStabStateTablePosn -- no active call" );
    __dump( "inline", curr );
    return curr->se;
}


static void callStabInit(       // INITIALIZATION FOR CALL_STAB
    INITFINI* defn )            // - definition
{
    defn = defn;
    carve_call_stab = CarveCreate( sizeof( CALL_STAB ), 16 );
}


static void callStabFini(       // COMPLETION FOR CALL_STAB
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carve_call_stab );
}


INITDEFN( call_stab, callStabInit, callStabFini )
