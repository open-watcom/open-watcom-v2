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


#include "cpplib.h"
#include "rtexcept.h"
extern "C" {
    #include "ljmphdl.h"
};


#ifndef SYSIND_REGISTRATION
    #error not system-independent registration
#endif


// The following provides protection against longjmp's.  The routine is
// called at the start of a longjmp to fix up the run-time data structure
// in case there are stacked exceptions or function state tables.
//
static void
#ifdef M_PC_INTEL
__pragma( "__arg_convention" )
#endif
longjmpDtoring                  // longjmp INTERFACE
    ( void __far *stack_bound ) // - bound for stack
{
    _RTCTL rt_ctl;              // - R/T control
    DISPATCH_EXC dispatch;      // - dispatch control
    RW_DTREG* rw;               // - R/W block: search block
    RW_DTREG* last;             // - R/W block: last block
    auto FsExcRec excrec;       // - system exception record

    // locate the R/W block for the corresponding setjmp
    //  last == NULL: all must be popped (setjmp is before everything)
    //  last != NULL: pop all blocks before "last"
    //      - setjmp is located:
    //          (a) in routine without registration called by last rtn
    //          (b) in last C++ rtn (we compiled destruction code
    //              after the setjmp to ensure the state is always the
    //              state before the setjmp)
    //
    CPPLIB( exc_setup )( &dispatch, 0, FALSE, &rt_ctl, 0, &excrec );
    rw = rt_ctl.thr->registered;
    last = rw;
    for( ; ; rw = rw->base.prev ) {
        if( rw == NULL ) {
            if( NULL == last ) {
                // nothing registered
                return;
            } else {
                // everything must be popped
                last = rw;
                dispatch.state_var = 0;
                break;
            }
        }
        last = rw;
        if( rw >= stack_bound ) {
            // unwindStack will do nothing for last
            dispatch.state_var = last->base.state_var;
            break;
        }
    }
    dispatch.rw = last;
    FS_UNWIND_GLOBAL( dispatch.rw, NULL, &excrec );
}


// The handler can be in a DLL, so we need to extract its address via
// a function call.
//
extern "C"
_WPRTLINK
pFUNVOIDVOID CPPLIB( lj_handler ) // GET ADDRESS OF longjmp HANDLER
    ( void )
{
    return (pFUNVOIDVOID)&longjmpDtoring;
}
