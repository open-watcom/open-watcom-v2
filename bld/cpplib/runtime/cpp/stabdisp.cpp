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



static size_t typesigIndex(     // FIND BASE-1 INDEX OF MATCHING TYPE SIGNATURE
    DISPATCH_EXC* dispatch,     // - dispatch control
    size_t count,               // - number of elements
    RT_TYPE_SIG* tsigs )        // - type signatures
{
    size_t index;               // - base-1 index
    THROW_RO* throw_ro;         // - throw R/O block
    rboolean zero_thrown;       // - true ==> zero was thrown
    unsigned thr_ctr;           // - throw ctr.
    unsigned ctr;               // - testing ctr.
    RT_TYPE_SIG tsig;           // - current type signature, in tsigs
    RT_TYPE_SIG thr_sig;        // - current type signature, from throw

    throw_ro = dispatch->ro;
    zero_thrown = dispatch->zero;
    for( ctr = 0; ctr < count; ++ctr ) {
        tsig = tsigs[ ctr ];
        if( tsig == NULL ) {
            index = ctr + 1;
            goto done;
        }
        for( thr_ctr = 0; thr_ctr < throw_ro->count; ++thr_ctr ) {
            dispatch->cnv_try = &throw_ro->cnvs[ thr_ctr ];
            thr_sig = dispatch->cnv_try->signature;
            if( CPPLIB( ts_equiv )( tsig, thr_sig, zero_thrown ) ) {
                index = ctr + 1;
                goto done;
            }
        }
    }
    index = 0;
done:
    return index;
}


static rboolean dispatchableFnExc(// CHECK IF FUNCTION EXCEPTION SPEC. DISPATCH
    DISPATCH_EXC *dispatch,     // - dispatch control
    DTOR_CMD* cmd )             // - DTOR_CMD_FN_EXC command
{
    size_t index = typesigIndex( dispatch
                               , cmd->fn_exc.count
                               , cmd->fn_exc.sigs );
    rboolean retn = ( index == 0 )
                || ( cmd->fn_exc.sigs[ index - 1 ] == NULL );
    if( retn ) {
        dispatch->try_cmd = cmd;
    }
    return retn;
}


static rboolean dispatchableCatch(// CHECK IF DISPATCHABLE CATCH
    DISPATCH_EXC *dispatch,     // - dispatch control
    DTOR_CMD* cmd )             // - DTOR_CMD_TRY command
{
    rboolean retn;              // - return: true ==> dispatachable
    size_t index;               // - index of match catch (base-1)

    index = typesigIndex( dispatch
                        , cmd->try_cmd.count
                        , cmd->try_cmd.sigs );
    if( index == 0 ) {
        retn = false;
    } else {
        -- index;
        dispatch->try_cmd = cmd;
        dispatch->catch_no = index;
        retn = true;
    }
    return retn;
}


static DISPATCHABLE dispatchable( // DETERMINE WHERE CATCHABLE WITHIN FUNCTION
    DISPATCH_EXC *dispatch,     // - dispatch information
    RW_DTREG *rw )              // - current read/write
{
    _RTCTL rt_ctl               // - R/T control
        ( dispatch->rtc->thr );
    STAB_TRAVERSE traverse;     // - traversal control
    RO_STATE* state;            // - current state entry
    DISPATCHABLE retn;          // - return: dispatchability
    DTOR_CMD* cmd;              // - command pointer
    ACTIVE_EXC *exc;            // - active exception
    rboolean rethrow;           // - true ==> re-throw from top block

    if( dispatch->rethrow ) {
        exc = dispatch->exc;
        if( exc == NULL ) {
            GOOF_EXC( "dispatchable: missing caught exception" );
        }
        dispatch->ro = exc->throw_ro;
        dispatch->zero = exc->zero_thrown;
        rethrow = true;
    } else {
        rethrow = false;
    }
    rt_ctl.setRwRo( rw );
    CPPLIB( stab_trav_init )( &traverse, &rt_ctl );
    for( ; ; CPPLIB( stab_trav_next )( &traverse ) ) {
        state = CPPLIB( stab_trav_move )( &traverse );
        if( state == NULL ) {
            retn = DISPATCHABLE_NONE;
            break;
        }
        if( state->dtor != NULL ) continue;
        cmd = state->u.cmd_addr;
        switch( cmd->base.code ) {
          case DTC_CATCH :
            if( rethrow ) {
                if( exc->cat_try == TryFromCatch( cmd ) ) {
                    dispatch->popped = true;
                }
                rethrow = false;
            }
            continue;
          case DTC_FN_EXC :
            if( dispatchableFnExc( dispatch, cmd ) ) {
                // exception violates fn-exc spec.
                retn = DISPATCHABLE_FNEXC;
                break;
            }
            continue;
          case DTC_TRY :
            if( dispatchableCatch( dispatch, cmd ) ) {
                retn = DISPATCHABLE_CATCH;
                break;
            }
            continue;
          default :
            continue;
        }
        dispatch->state_var = traverse.state_var;
        break;
    }
    return retn;
}


extern "C"
DISPATCHABLE CPPLIB( dispatchable )(// TEST IF R/W BLOCK IS DISPATCHABLE
    DISPATCH_EXC *dispatch,     // - dispatch control
    RW_DTREG* rw )              // - R/W block: search block
{
    DISPATCHABLE dispatch_type; // - type of dispatch

    // check if r/w on stack
    if( rw == 0 ) {
        CPPLIB( corrupted_stack )();
    }

    dispatch->rw = rw;
    dispatch->rtc->setRwRo( rw );
    RO_DTREG* ro = dispatch->rtc->ro;
    switch( ro->base.reg_type ) {
      default :
        CPPLIB( corrupted_stack )();
      case DTRG_FUN :
        dispatch_type = dispatchable( dispatch, rw );
        break;
      case DTRG_STATIC_INITLS :
      case DTRG_ARRAY :
      case DTRG_OBJECT :
        dispatch_type = DISPATCHABLE_NONE;
        break;
    }
    return dispatch_type;
}


#if 0
// future support for catch(...) to catch anything

static DISPATCHABLE catch_any(  // DETERMINE WHERE CATCH(...) WITHIN FUNCTION
    DISPATCH_EXC *dispatch,     // - dispatch information
    RW_DTREG *rw )              // - current read/write
{
    _RTCTL rt_ctl               // - R/T control
        ( dispatch->rtc->thr );
    STAB_TRAVERSE traverse;     // - traversal control
    RO_STATE* state;            // - current state entry
    DISPATCHABLE retn;          // - return: dispatchability
    DTOR_CMD* cmd;              // - command pointer

    rt_ctl.setRwRo( rw );
    CPPLIB( stab_trav_init )( &traverse, &rt_ctl );
    for( ; ; CPPLIB( stab_trav_next )( &traverse ) ) {
        state = CPPLIB( stab_trav_move )( &traverse );
        if( state == NULL ) {
            retn = DISPATCHABLE_NONE;
            break;
        }
        if( state->dtor != NULL ) continue;
        cmd = state->u.cmd_addr;
        if( cmd->base.code == DTC_TRY ) {
            if( 0 == cmd->try_cmd.sigs[ cmd->try_cmd.count - 1 ] ) {
                dispatch->non_watcom = true;
                retn = DISPATCHABLE_CATCH;
                break;
            }
        }
        dispatch->state_var = traverse.state_var;
        break;
    }
    return retn;
}


extern "C"
DISPATCHABLE CPPLIB( catch_any )// TEST IF R/W BLOCK IS DISPATCHABLE FOR ...
    ( FsExcRec*                 // - exception record
    , RW_DTREG* rw )            // - R/W block: search block
{
    _RTCTL rt_ctl;              // - R/T control
    DISPATCHABLE dispatch_type; // - type of dispatch
    DISPATCH_EXC dispatch;      // - dispatch control

    // check if r/w on stack
    if( rw == 0 ) {
        CPPLIB( corrupted_stack )();
    }

    CPPLIB( dispatch_dummy )( &dispatch, &rt_ctl );
    dispatch.rw = rw;
    dispatch.rtc->setRwRo( rw );
    RO_DTREG* ro = dispatch.rtc->ro;
    switch( ro->base.reg_type ) {
      default :
        CPPLIB( corrupted_stack )();
      case DTRG_FUN :
        dispatch_type = catch_any( &dispatch, rw );
        break;
      case DTRG_STATIC_INITLS :
      case DTRG_ARRAY :
      case DTRG_OBJECT :
        dispatch_type = DISPATCHABLE_NONE;
        break;
    }
    return dispatch_type;
}
#endif
