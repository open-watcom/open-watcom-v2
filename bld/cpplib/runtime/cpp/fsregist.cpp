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
#include "exc_pr.h"

#ifdef FS_REGISTRATION
uint_8 *findHandler( uint_8 *h ) {
    while( h[0] == 0xff && h[1] == 0x25 ) {
        // skip jmp [0xhhhhhhhh] instructions (these are injected by the linker)
        h = (uint_8*) ( **((void***)(h+2)) );
    }
    return( h );
}
#endif


extern "C"
void CPPLIB( dispatch_dummy )   // CREATE DUMMY DISPATCH BLOCK
    ( DISPATCH_EXC* dispatch    // - dispatch block
    , _RTCTL* rtc )             // - R/T control
{
    dispatch->rtc = rtc;
    dispatch->zero = false;
    dispatch->rethrow = 0;
    dispatch->non_watcom = 0;
    dispatch->ro = 0;
    dispatch->try_cmd = 0;
    dispatch->exc = 0;
    dispatch->fs_last = 0;
}


extern "C"
THREAD_CTL* CPPLIB( fs_lookup ) // LOOK THRU FS ENTRIES FOR LAST, THREAD_CTL
    ( RW_DTREG** a_last )       // - addr[ ptr to last WATCOM entry ]
{
    _RTCTL rt_ctl( 0 );         // - fake R/T control
    FsExcRec excrec;            // - system exception record
    DISPATCH_EXC dispatch;      // - dispatch control
    RW_DTREG* last;             // - last WATCOM R/W block
    RW_DTREG *ctl;              // R/W block (based fs:0)
    THREAD_CTL* retn;           // PGM THREAD (first WATCOM .EXE, .DLL active)
    THREAD_CTL* base;           // PGM THREAD (call base)

#ifdef SYSIND_REGISTRATION
    #define LIST_END 0          // - link-list ending
#else
    #define LIST_END (void*)(-1L)// - link-list ending
#endif

    CPPLIB( dispatch_dummy )( &dispatch, &rt_ctl );
    excrec.code = EXCREC_CODE;
    excrec.flags = EXCREC_FLAGS;
    excrec.rec = 0;
    excrec.addr = 0;
    excrec.parm_count = EXCREC_PARM_COUNT;
    excrec.object = 0;
    excrec.dispatch = &dispatch;
    last = 0;
    retn = 0;
    for( ctl = FsTop(); ctl != LIST_END; ctl = ctl->base.prev ) {
#ifdef FS_REGISTRATION
        uint_8* handler = findHandler( (uint_8*)ctl->base.handler );
        if( handler[5]  == 'W'
         && handler[6]  == 'A'
         && handler[7]  == 'T'
         && handler[8]  == 'C'
         && handler[9]  == 'O'
         && handler[10] == 'M' ) {
#endif
            if( EXC_HAND_CATCH
                    == (*ctl->base.handler)( &excrec, 0, 0, 0 ) ) {
                last = ctl;
                base = dispatch.rtc->thr;
                if( base->flags.executable ) {
                    retn = base;
                }
            }
#ifdef FS_REGISTRATION
        }
#endif
    }
    if( retn == 0 ) {
        retn = dispatch.rtc->thr;
        if( retn == 0 ) {
            retn = &_RWD_ThreadData;
        }
    }
    *a_last = last;
    return retn;
}


extern "C"
FSREGAPI unsigned CPPLIB( fs_handler_rtn ) // HANDLER FOR FS REGISTRATIONS
    ( FsExcRec* rec_exc         // - exception record
    , RW_DTREG* rw              // - current R/W block
    , FsCtxRec*                 // - context record
    , unsigned                  // - dispatch context
    )
{
    unsigned retn;              // - return code

#ifdef RT_EXC_ENABLED
    if( 0 == rw ) {
        // rw == 0 only when called from pgm_thread
        THREAD_CTL* ctl = &_RWD_ThreadData;
        rec_exc->dispatch->rtc->thr = ctl;
        retn = EXC_HAND_CATCH;
    } else if( rec_exc->flags & EXC_TYPE_UNWIND_NORMAL ) {
        CPPLIB( destruct_internal )( 0, rw );
        retn = EXC_HAND_CONTINUE;
    } else if( rec_exc->flags & EXC_TYPE_UNWIND_EXIT ) {
        if( rw->base.ro->base.reg_type == DTRG_STATIC_INITLS ) {
            // always unwind static initialization
            CPPLIB( destruct_internal )( 0, rw );
        }
        retn = EXC_HAND_CONTINUE;
    } else if( EXCREC_CODE_WATCOM == ( EXCREC_CODE_MASK & rec_exc->code ) ) {
        // WATCOM EXCEPTION
        DISPATCHABLE type;      // - type of dispatchability
        DISPATCH_EXC* dispatch; // - dispatch control
        dispatch = rec_exc->dispatch;
        if( dispatch->fnexc_skip == rw ) {
            dispatch->fnexc_skip = NULL;
        }
        _EXC_PR* srch_ctl;
        for( srch_ctl = dispatch->srch_ctl
           ; NULL != srch_ctl && srch_ctl->_rw == rw
           ; srch_ctl = srch_ctl->_prev ) {
            if( NULL == dispatch->fnexc_skip ) {
                switch( srch_ctl->_state ) {
                  case EXCSTATE_UNEXPECTED :
                  case EXCSTATE_BAD_EXC :
                    dispatch->fnexc_skip
                        = ((_EXC_PR_FNEXC*)srch_ctl)->_fnexc_skip;
                    continue;
                }
                break;
            }
        }
        if( NULL != srch_ctl && srch_ctl->_rw == rw ) {
            type = DISPATCHABLE_STOP;
        } else if( NULL == dispatch->fnexc_skip ) {
            type = CPPLIB( dispatchable )( dispatch, rw );
        } else {
            type = DISPATCHABLE_NONE;
        }
        if( DISPATCHABLE_NONE == type ) {
            if( rw == dispatch->fs_last ) {
                type = DISPATCHABLE_NO_CATCH;
                dispatch->type = type;
                retn = EXC_HAND_CATCH;
            } else {
                retn = EXC_HAND_CONTINUE;
            }
        } else {
            dispatch->type = type;
            retn = EXC_HAND_CATCH;
        }
    } else {
#if 0
// future support for catch(...) to catch anything
        // not WATCOM throw / re-throw
        DISPATCHABLE type = CPPLIB( catch_any )( rec_exc, rw );
        if( DISPATCHABLE_NONE == type ) {
            retn = EXC_HAND_CONTINUE;
        } else {
            retn = EXC_HAND_CATCH;
        }
#else
        retn = EXC_HAND_CONTINUE;
#endif
    }
#else
    rw = rw;
    rec_exc = rec_exc;
    retn = EXC_HAND_CONTINUE;
#endif
    return retn;
}


#ifdef SYSIND_REGISTRATION


extern "C"
void CPPLIB( unwind_global )    // GLOBAL UNWIND ROUTINE
    ( RW_DTREG* rw              // - bounding R/W block
    , uint_32                   // - return address (not used)
    , FsExcRec* excrec )        // - exception record
{
    RW_DTREG* curr;             // - current R/W block
    THREAD_CTL* ctl;            // - thread control

    excrec->flags = EXC_TYPE_UNWIND_NORMAL;
    ctl = excrec->dispatch->rtc->thr;
    for( curr = ctl->registered; curr != rw; ) {
        CPPLIB( fs_handler_rtn )( excrec, curr, NULL, 0 );
        curr = curr->base.prev;
        ctl->registered = curr;
    }
}

#ifdef RT_EXC_ENABLED

extern "C"
void CPPLIB( raise_exception )  // RAISE AN EXCEPTION
    ( FsExcRec* excrec )        // - exception record
{
    RW_DTREG* curr;             // - current R/W block
    THREAD_CTL* ctl;            // - thread control
    unsigned retn;              // - search return

    ctl = excrec->dispatch->rtc->thr;
    for( curr = ctl->registered; ; curr = curr->base.prev ) {
        retn = CPPLIB( fs_handler_rtn )( excrec, curr, NULL, 0 );
        if( retn != EXC_HAND_CONTINUE ) break;
    }
}


#endif  // RT_EXC_ENABLED

#endif  // SYSIND_REGISTRATION
