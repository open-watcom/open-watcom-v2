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


#define __NEED_SYSTEM_HEADER

#include "cpplib.h"
#include "rtexcept.h"
#include "exc_pr.h"
#include <setjmp.h>

typedef unsigned (*p_handler)
    ( FsExcRec*                 // - exception record
    , RW_DTREG*                 // - current R/W block
    , FsCtxRec*                 // - context record
    , unsigned                  // - dispatch context
    );

struct __EXC_INFO               // EXCEPTION INFORMATION
{   RO_DTREG* ro;               // - R/O block
    RW_DTREG* rw;               // - R/W block
    unsigned est_frame[8];      // - establisher frame
    _CONTEXT ctx;               // - context for current function
    PD_DISP_CTX dctx;           // - dispatcher context, for convenience
    unsigned in_func;           // - used to figure if in function
    PData* pd;                  // - PData for function
};


int lastPgmCtr                  // TEST IF LAST PROGRAM CTR WHEN WALKING STACK
    ( void* pc )                // - program ctr
{
    long pcl =  (long)pc & 0xfffffffC;
    return pcl == 0 || pcl == 0xfffffffC;
}


static int procSetsFP           // TEST IF PROCEDURE SETS FP
    ( PData* pdata )            // - procedure descriptor
{
    int retn;                   // - return: 1 ==> fp used
    RISC_INS* ins;              // - an instruction

    if( pdata->entry == pdata->endpr ) {
        retn = 0;
    } else {
        ins = (RISC_INS*)pdata->endpr;
        ins = (RISC_INS*)( (char*)ins - RISC_INS_SIZE );
        if( *ins == RISC_MOV_SP_FP ) {
            retn = 1;
        } else {
            retn = 0;
        }
    }
    return retn;
}


static void* getProcFrame       // GET PROCEDURE FRAME
    ( PData* pdata              // - procedure descriptor
    , PD_DISP_CTX* dctx )       // - dispatch context
{
    void* frame;                // - frame ptr.

#if 0
    if( procSetsFP( pdata ) ) {
        frame = (void*)dctx->fp_actual;
    } else {
        frame = (void*)dctx->sp_actual;
    }
#else
    if( dctx->pdata != pdata ) {
        GOOF_EXC( "getProcFram: dctx->pdata != pdata" );
    }
    // AFS: this function seems to change a lot and it very brittle;
    // is it possible to use 'sp' from the handler?
    frame = dctx->fp_alternate;
#endif
    return frame;
}


static void getProcInfo         // GET EXCEPTION INFO FOR PROCEDURE
    ( __EXC_INFO* inf           // - exception info
    , PD_DISP_CTX* dctx )       // - dispatcher context for procedure
{
    void* frame;                // - frame ptr.

    PData* pd = dctx->pdata;
    inf->pd = pd;
    inf->ro = (RO_DTREG*)pd->exc_data;
    frame = getProcFrame( pd, dctx );
    inf->rw = (RW_DTREG*)( (char*)frame + inf->ro->fun.rw_offset );
    if( inf->rw->base.ro != inf->ro ) {
        GOOF_EXC( "getProcInfo: rw->ro != ro" );
    }
}


static __EXC_INFO* setPdata     // SET PDATA INFORMATION
    ( __EXC_INFO* info )        // - exception info
{
    info->dctx.pdata = (PData*)RtlLookupFunctionEntry( info->dctx.pc );
    return info;
}


static void nextExcInfo         // MOVE AHEAD TO NEXT RTN. IN CALL CHAIN
    ( __EXC_INFO* info )        // - exception info
{
    info->dctx.pc = RtlVirtualUnwind( info->dctx.pc
                                    , info->dctx.pdata
                                    , &info->ctx
                                    , &info->in_func
                                    , info->est_frame
                                    , 0 );
    setPdata( info );
}


static void initExcInfo         // INITIALIZE __EXC_INFO
    ( __EXC_INFO* info )        // - exception info
{
#ifndef NDEBUG
    memset( info, 0xdd, sizeof( *info ) );
#endif
    info->dctx.ctx = &info->ctx;
    RtlCaptureContext( info->dctx.ctx );
    info->dctx.pc = GetCtxReg( &info->ctx, Ra );
    info->in_func = 1;
    info = setPdata( info );
    nextExcInfo( info );
}


extern "C"
_WPRTLINK
unsigned CPPLIB( pd_handler_rtn )  // HANDLER FOR FS REGISTRATIONS
    ( FsExcRec* rec_exc         // - exception record
    , void* sp                  // - frame pointer function entry
    , _CONTEXT*                 // - context record
    , PD_DISP_CTX* dctx         // - dispatch context
    )
{
    unsigned retn;              // - return code
    __EXC_INFO info;            // - procedure exception information

    if( 0 == sp ) {
        // sp == 0 only when called from pd_lookup
        THREAD_CTL* ctl = &_RWD_ThreadData;
        retn = unsigned( ctl );
    } else {
        getProcInfo( &info, dctx );
        if( rec_exc->flags & EXC_TYPE_UNWIND_NORMAL ) {
            if( rec_exc->parm_count != 1
             || rec_exc->object     != sp
             || EXCREC_CODE_SETJMP  != ( EXCREC_CODE_MASK & rec_exc->code )
              ) {
                CPPLIB( destruct_internal )( 0, info.rw );
            }
            retn = EXC_HAND_CONTINUE;
        } else if( rec_exc->flags & EXC_TYPE_UNWIND_EXIT ) {
            if( info.ro->base.reg_type == DTRG_STATIC_INITLS ) {
                // always unwind static initialization
                CPPLIB( destruct_internal )( 0, info.rw );
            }
            retn = EXC_HAND_CONTINUE;
        } else if( EXCREC_CODE_WATCOM
                == ( EXCREC_CODE_MASK & rec_exc->code ) ) {
            // WATCOM C++ Exception raised
#if 0
            DISPATCHABLE type = CPPLIB( dispatchable )( rec_exc->dispatch
                                                      , info.rw );
            DISPATCH_EXC* dispatch = rec_exc->dispatch;
            if( DISPATCHABLE_NONE == type ) {
                if( info.rw == dispatch->fs_last ) {
                    type = DISPATCHABLE_NO_CATCH;
                    dispatch->type = type;
                    retn = EXC_HAND_CATCH;
                } else {
                    retn = EXC_HAND_CONTINUE;
                }
            } else {
                dispatch->pdata = info.pd;
                dispatch->type = type;
                retn = EXC_HAND_CATCH;
            }
#else
            DISPATCHABLE type;      // - type of dispatchability
            DISPATCH_EXC* dispatch; // - dispatch control
            dispatch = rec_exc->dispatch;
            if( dispatch->fnexc_skip == info.rw ) {
                dispatch->fnexc_skip = NULL;
            }
            for( _EXC_PR* srch_ctl = dispatch->srch_ctl
               ; NULL != srch_ctl && srch_ctl->_rw == info.rw
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
            if( NULL != srch_ctl && srch_ctl->_rw == info.rw ) {
                type = DISPATCHABLE_STOP;
            } else if( NULL == dispatch->fnexc_skip ) {
                type = CPPLIB( dispatchable )( dispatch, info.rw );
            } else {
                type = DISPATCHABLE_NONE;
            }
            if( DISPATCHABLE_NONE == type ) {
                if( info.rw == dispatch->fs_last ) {
                    type = DISPATCHABLE_NO_CATCH;
                    dispatch->type = type;
                    retn = EXC_HAND_CATCH;
                } else {
                    retn = EXC_HAND_CONTINUE;
                }
            } else {
                dispatch->pdata = info.pd;
                dispatch->type = type;
                retn = EXC_HAND_CATCH;
            }
#endif
        } else {
            // not WATCOM throw / re-throw
            retn = EXC_HAND_CONTINUE;
        }
    }
    return retn;
}

extern "C" void* CPPLIB( PdCtx )// R/T Support (Assembler)
    ( void*                     // - context for function
    , void* );                  // - save area

extern "C"
void CPPLIB( PdUnwind )         // UNWIND USING PROCEDURE DESCRIPTORS
    ( FsExcRec* exc_rec )       // - exception record
{
    DISPATCH_EXC* dispatch;     // - dispatch control
    RW_DTREG* rw;               // - R/W block being dispatched
    void* frame;                // - frame for rtn. (SP or FP)
    void* pc;                   // - PC for continuation (after the setjmp)
    _CONTEXT ctx;               // - context for function
    unsigned save_area[3*2];    // - save area

    RtlCaptureContext( &ctx );
    pc = CPPLIB( PdCtx )( &ctx, save_area );
    if( pc ) {
        dispatch = exc_rec->dispatch;
        rw = dispatch->rw;
        frame = (void*)( (char*)rw - rw->base.ro->fun.rw_offset );
        if( procSetsFP( dispatch->pdata ) ) {
            RtlUnwindRfp( frame, pc, exc_rec, save_area );
        } else {
            RtlUnwind( frame, pc, exc_rec, save_area );
        }
    }
}


static int isWatcomHandler      // FIGURE OUT IF WATCOM HANDLER
    ( __EXC_INFO* info )        // - exception info
{
    int retn;
    if( info->in_func
     && 0 != info->dctx.pdata ) {
        unsigned* hand = (unsigned*)info->dctx.pdata->exc;
        if( 0 != hand
         && hand[1] == 0x2B544157           // "WAT+"
         && hand[2] == 0x4D4F432B ) {       // "+COM"
#if 1
            if( procSetsFP( info->dctx.pdata ) ) {
                info->dctx.fp_actual = GetCtxReg( &info->ctx, Fp );
            } else {
                info->dctx.fp_actual = GetCtxReg( &info->ctx, Sp );
            }
#else
            info->dctx.sp_actual = GetCtxReg( &info->ctx, Sp );
            info->dctx.fp_actual = GetCtxReg( &info->ctx, Fp );
#endif
            info->dctx.fp_alternate = info->dctx.fp_actual;
            getProcInfo( info, &info->dctx );
            retn = 1;
        } else {
            retn = 0;
        }
    } else {
        retn = 0;
    }
    return retn;
}


extern "C"
THREAD_CTL* CPPLIB( pd_lookup ) // LOOK THRU PD ENTRIES FOR LAST, THREAD_CTL
    ( RW_DTREG** a_last )       // - addr[ ptr to last WATCOM entry ]
{
    __EXC_INFO info;            // - exception info. for frame
    RW_DTREG* last;             // - last R/W active
    THREAD_CTL* base;           // - PGM THREAD (call base)
    THREAD_CTL* retn;           // - PGM THREAD (first WATCOM .EXE, .DLL active)

    initExcInfo( &info );
    last = 0;
    retn = 0;
    for( ; ! lastPgmCtr( info.dctx.pc ); nextExcInfo( &info ) ) {
        if( isWatcomHandler( &info ) ) {
            last = info.rw;
            p_handler handler = (p_handler)info.dctx.pdata->exc;
            base = (THREAD_CTL*)handler( 0, 0, 0, 0 );
            if( retn == 0
             || base->flags.executable ) {
                retn = base;
            }
        }
    }
    *a_last = last;
    if( retn == 0 ) {
        retn = &_RWD_ThreadData;
    }
    return retn;
}


extern "C"
RW_DTREG* CPPLIB( pd_top )      // LOOK THRU PD ENTRIES FOR FIRST R/W ENTRY
    ( void )
{
    __EXC_INFO info;            // - exception info. for frame
    RW_DTREG* first;            // - first entry

    initExcInfo( &info );
    first = 0;
    for( ; ! lastPgmCtr( info.dctx.pc ); nextExcInfo( &info ) ) {
        if( isWatcomHandler( &info ) ) {
            first = info.rw;
            break;
        }
    }
    return first;
}


#ifndef NDEBUG

// DEBUGGING SUPPORT

extern "C"
void CPPLIB( pd_dump_rws )      // DEBUGGING -- DUMP R/W, R/O data structure
    ( register void (*dump_rw)( RW_DTREG*// - watcom block
                     , RO_DTREG* )
    , void (*dump_pd)( PData* ) )// - non-watcom block
{
    __EXC_INFO info;            // - exception info. for frame
    RW_DTREG* first;            // - first entry

    initExcInfo( &info );
    first = 0;
    for( ; ! lastPgmCtr( info.dctx.pc ); nextExcInfo( &info ) ) {
        if( isWatcomHandler( &info ) ) {
            dump_rw( info.rw, info.ro );
        } else {
            dump_pd( info.dctx.pdata );
        }
    }
}
#endif
