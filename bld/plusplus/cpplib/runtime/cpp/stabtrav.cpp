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


extern "C"
RO_STATE* CPPLIB( stab_entry )  // GET ENTRY FOR STATE VARIABLE
    ( RO_DTREG* ro              // - R/O entry
    , RT_STATE_VAR state_var )  // - state variable
{
    RO_STATE* state;            // - NULL or state entry

    if( state_var == 0 ) {
        state = NULL;
    } else {
        state = ro->fun.state_table;
        state = &state[ state_var - 1 ];
    }
    return state;
}


extern "C"
void CPPLIB( stab_trav_comp )   // POINT STATE-TABLE ENTRY BEYOND COMPONENT
    ( STAB_TRAVERSE* ctl )      // - control for travsersal
{
    ctl->state_var -= 2;
}


static RO_STATE* travPosn       // POSITION TO STATE ENTRY
    ( STAB_TRAVERSE* ctl        // - control for traversal
    , RT_STATE_VAR state_var )  // - state value for state entry
{
    RO_STATE* state;            // - next state entry
    RW_DTREG* rw;               // - current R/W entry

    for( ; ; ) {
        rw = ctl->rtc->rw;
        state = CPPLIB( stab_entry )( ctl->rtc->ro, state_var );
        if( state_var == ctl->bound ) break;
        if( state->dtor != NULL ) break;
        DTOR_CMD* cmd = state->u.cmd_addr;
        switch( cmd->base.code ) {
          case DTC_SET_SV :
            state_var = cmd->set_sv.state_var;
            continue;
          case DTC_TEST_FLAG :
            if( CPPLIB( bit_test )( rw->fun.flags
                                  , cmd->test_flag.index ) ) {
                state_var = cmd->test_flag.state_var_true;
            } else {
                state_var = cmd->test_flag.state_var_false;
            }
            continue;
          case DTC_CTOR_TEST :
            if( CPPLIB( bit_test )( rw->fun.flags
                                  , cmd->ctor_test.index ) ) {
                if( ctl->unwinding ) {
                    CPPLIB( bit_off )( rw->fun.flags, cmd->ctor_test.index );
                }
                state_var -= 1;
                break;
            } else {
                state_var -= 1;
                if( state_var == ctl->bound ) break;
                state_var -= 1;
                continue;
            }
          case DTC_TRY :
          case DTC_CATCH :
          case DTC_FN_EXC :
          case DTC_COMP_VBASE :
          case DTC_COMP_DBASE :
          case DTC_COMP_MEMB :
          case DTC_ARRAY_INIT :
          case DTC_DLT_1 :
          case DTC_DLT_2 :
          case DTC_DLT_1_ARRAY :
          case DTC_DLT_2_ARRAY :
            break;
          default :
            GOOF_EXC( "travPosn: bad DTC_" );
        }
        break;
    }
    ctl->state_var = state_var;
    return state;
}


extern "C"
void CPPLIB( stab_trav_init )   // INITIALIZE STATE-TABLE TRAVERSAL
    ( STAB_TRAVERSE* ctl        // - control for traversal
    , _RTCTL* rtc )             // - R/T control
{
    RW_DTREG *rw;               // - R/W control block

    ctl->rtc = rtc;
    rw = rtc->rw;
    ctl->state_var = rw->base.state_var;
    ctl->bound = 0;
    ctl->unwinding = FALSE;
    ctl->obj_type = OBT_OBJECT;
    travPosn( ctl, ctl->state_var );
}


extern "C"
RO_STATE* CPPLIB( stab_trav_next )// POINT AT NEXT STATE-TABLE ENTRY
    ( STAB_TRAVERSE* ctl )      // - control for traversal
{
    RO_STATE* state;            // - next state entry
    RW_DTREG* rw;               // - current R/W entry
    RT_STATE_VAR state_var;     // - current state variable

    rw = ctl->rtc->rw;
    if( rw == NULL ) {
        state = NULL;
    } else {
        state_var = ctl->state_var;
        if( state_var > 0 ) {
            RO_DTREG* ro = ctl->rtc->ro;
            if( ro->base.reg_type == DTRG_FUN ) {
                state = CPPLIB( stab_entry )( ro, state_var );
                if( state->dtor == NULL ) {
                    DTOR_CMD* cmd = state->u.cmd_addr;
                    if( cmd->base.code == DTC_CATCH ) {
                        cmd = TryFromCatch( cmd );
                        state_var = cmd->try_cmd.state;
                    } else {
                        -- state_var;
                    }
                } else {
                    --state_var;
                }
            } else {
                -- state_var;
            }
        }
        state = travPosn( ctl, state_var );
    }
    return state;
}


extern "C"
RO_STATE* CPPLIB( stab_trav_move)( // MOVE TO NEXT ACTUAL POSITION
    STAB_TRAVERSE *traverse )   // - traversal control information
{
    return travPosn( traverse, traverse->state_var );
}
