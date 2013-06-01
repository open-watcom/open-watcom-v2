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


inline
static void initFsExcRec(       // INITIALIZE FsExcRec
    void *object,               // - address of object
    FsExcRec* rec,              // - the record
    DISPATCH_EXC* dispatch )    // - dispatch info
{
    rec->code = EXCREC_CODE;
    rec->flags = EXCREC_FLAGS;
    rec->rec = 0;
    rec->addr = 0;
    rec->parm_count = EXCREC_PARM_COUNT;
    rec->object = object;
    rec->dispatch = dispatch;
}


inline
static void dispatch_init(      // INITIALIZE DISPATCH BLOCK
    DISPATCH_EXC *dispatch,     // - dispatch control
    THROW_RO *throw_ro,         // - thrown R/O block
    rboolean is_zero,           // - TRUE ==> thrown object is zero constant
    _RTCTL* rtc )               // - R/T control
{
    dispatch->ro = throw_ro;
    dispatch->rtc = rtc;
    dispatch->zero = is_zero;
    ThreadLookup( &dispatch->fs_last );
    dispatch->try_cmd = NULL;
    dispatch->exc = rtc->thr->excepts;
    if( 0 == throw_ro ) {
        dispatch->rethrow = TRUE;
    } else {
        dispatch->rethrow = FALSE;
    }
    dispatch->popped = FALSE;
    dispatch->non_watcom = FALSE;
    dispatch->fnexc_skip = NULL;
    dispatch->srch_ctl = rtc->thr->exc_pr;
}


void CPPLIB( exc_setup )        // SETUP DISPATCH, EXCEPTION RECORDS
    ( DISPATCH_EXC* disp        // - dispatch record
    , THROW_RO* thr_ro          // - throw r/o block
    , rboolean is_zero          // - TRUE ==> thrown object is zero constant
    , _RTCTL* rt_ctl            // - R/T control
    , void* object              // - thrown object
    , FsExcRec* rec )           // - exception record
{
    dispatch_init( disp, thr_ro, is_zero, rt_ctl );
    initFsExcRec( object, rec, disp );
}
