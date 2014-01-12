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


static RO_STATE* nextDtorPosn(  // POSITION TO NEXT DTOR
    STAB_TRAVERSE *traverse )   // - traversal control information
{
    DTOR_CMD* cmd;              // - destruction command
    RO_STATE* state;            // - current entry

    state = CPPLIB( stab_trav_move )( traverse );
    if( state != NULL
     && state->dtor == NULL
     && traverse->state_var != traverse->bound ) {
        cmd = state->u.cmd_addr;
        switch( cmd->base.code ) {
          case DTC_CTOR_TEST :
          case DTC_TEST_FLAG :
          case DTC_SET_SV :
          default :
            GOOF_EXC( "nextDtorPosn: bad DTC_" );
          case DTC_CATCH :
          case DTC_TRY :
          case DTC_FN_EXC :
#ifndef RT_EXC_ENABLED
            GOOF_EXC( "nextDtorPosn: -xs code in -xd library" );
#endif
          case DTC_ACTUAL_VBASE :
          case DTC_ACTUAL_DBASE :
          case DTC_COMP_VBASE :
          case DTC_COMP_DBASE :
          case DTC_COMP_MEMB :
          case DTC_ARRAY_INIT :
          case DTC_DLT_1 :
          case DTC_DLT_2 :
          case DTC_DLT_1_ARRAY :
          case DTC_DLT_2_ARRAY :
            break;
        }
    }
    return state;
}


static void dtorComponent(      // DESTRUCT A COMPONENT
    DTOR_CMD* cmd,              // - command for component
    unsigned cdtor,             // - CDTOR parameter
    _RTCTL* rtc,                // - R/T control
    unsigned mask )             // - mask to inhibit destruction
{
    RW_DTREG_OBJECT* rw_obj;    // - R/W block for component

    rw_obj = PointUsingOffset( RW_DTREG_OBJECT
                             , rtc->rw
                             , cmd->component.offset_init );
    if( 0 == ( rw_obj->cdtor & mask ) ) {
        (*cmd->component.dtor)( PointOffset( rw_obj->object
                                           , cmd->component.offset_object )
                              , cdtor );
    }
}


#ifdef RT_EXC_ENABLED
static void dtorCaughtException // DESTRUCT AND FREE EXCEPTION AT CATCH
    ( _RTCTL* rtc               // - R/T control
    , RW_DTREG *rw              // - R/W for block
    , DTOR_CMD* cmd )           // - command within it
{
    ACTIVE_EXC* active = CPPLIB( find_active )( rtc, rw, cmd );
    CPPLIB( dtor_free_exc )( active, rtc );
}
#endif


#ifdef RT_EXC_ENABLED
static void dtorFnexcException  // DESTRUCT AND FREE EXCEPTION AT FNEXC
    ( _RTCTL* rtc               // - R/T control
    , RW_DTREG *rw              // - R/W for block
    , DTOR_CMD* cmd )           // - command within it
{
    for( ; ; ) {
        ACTIVE_EXC* active = CPPLIB( find_active )( rtc, rw, cmd );
        if( NULL == active ) break;
        CPPLIB( dtor_free_exc )( active, rtc );
    }
}
#endif


static void destruct_traverse   // DESTRUCTION FOR BLK UNTIL STATE REACHED
    ( RT_STATE_VAR state_var    // - state variable
    , _RTCTL* rtc               // - R/T control
    , RW_DTREG *rw )            // - R/W for block
{
    STAB_TRAVERSE traversal;    // - traversal information
    RO_STATE* next_state;       // - next state entry
    RO_STATE* curr_state;       // - current state entry
    RO_DTREG* ro;               // - R/O table for entry
    void* obj_addr;             // - points at object
    pFUNdtor dtor;              // - DTOR for object
    DTOR_CMD* cmd;              // - is array command
    RT_STATE_VAR curr_state_var;// - current state variable

    rtc = rtc->setRwRo( rw );
    CPPLIB( stab_trav_init )( &traversal, rtc );
    traversal.unwinding = true;
    traversal.bound = state_var;
    next_state = nextDtorPosn( &traversal );
    ro = rtc->ro;
    for( ; ; ) {
        if( traversal.state_var == state_var ) break;
        if( traversal.state_var < state_var ) {
            GOOF_EXC( "destruct_traverse: underflow sv" );
        }
        curr_state_var = traversal.state_var;
        curr_state = next_state;
        CPPLIB( stab_trav_next )( &traversal );
        next_state = nextDtorPosn( &traversal );
        rw->base.state_var = traversal.state_var;
        dtor = curr_state->dtor;
        if( dtor == NULL ) {
            cmd = curr_state->u.cmd_addr;
            switch( cmd->base.code ) {
              default :
                GOOF_EXC( "destruct_traverse: invalid command" );
              case DTC_ARRAY :
              { RT_ARRAY_INIT* array_init;
                array_init = PointUsingOffset( RT_ARRAY_INIT
                                             , rw
                                             , cmd->array.offset );
                CPPLIB( dtor_array )( array_init->array
                                    , cmd->array.count
                                    , cmd->array.sig );
              } break;
              case DTC_CATCH :
#ifdef RT_EXC_ENABLED
                cmd = TryFromCatch( cmd );
                dtorCaughtException( rtc, rw, cmd );
#else
                GOOF_EXC( "destruct_traverse: -xs code in -xd library" );
#endif
                break;
              case DTC_FN_EXC :
#ifdef RT_EXC_ENABLED
                dtorFnexcException( rtc, rw, cmd );
#else
                GOOF_EXC( "destruct_traverse: -xs code in -xd library" );
#endif
                break;
              case DTC_TRY :
#ifndef RT_EXC_ENABLED
                GOOF_EXC( "destruct_traverse: -xs code in -xd library" );
#endif
                break;
              case DTC_COMP_VBASE :
                dtorComponent( cmd
                             , DTOR_COMPONENT
                             , rtc
                             , DTOR_IGNORE_COMPS | DTOR_COMPONENT );
                break;
              case DTC_ACTUAL_VBASE :
              case DTC_ACTUAL_DBASE :
              case DTC_COMP_DBASE :
                dtorComponent( cmd
                             , DTOR_COMPONENT
                             , rtc
                             , DTOR_IGNORE_COMPS );
                break;
              case DTC_COMP_MEMB :
                dtorComponent( cmd
                             , DTOR_NULL
                             , rtc
                             , DTOR_IGNORE_COMPS );
                break;
              case DTC_ARRAY_INIT :
              { RT_ARRAY_INIT* ctl;         // - array-init. block
                ctl = PointUsingOffset( RT_ARRAY_INIT
                                      , rw
                                      , cmd->array_init.offset );
                CPPLIB( dtor_array )( ctl->array, ctl->index, ctl->sig );
              } break;
              case DTC_DLT_1 :
              { void** object_ptr;          // - addr[ pointer to object ]
                object_ptr = PointUsingOffset( void*
                                             , rw
                                             , cmd->delete_1.offset );
                ( *cmd->delete_1.op_del )( *object_ptr );
              } break;
              case DTC_DLT_1_ARRAY :
              { void** object_ptr;          // - addr[ pointer to object ]
                object_ptr = PointUsingOffset( void*
                                             , rw
                                             , cmd->delete_1.offset );
                ARRAY_STORAGE* array_ptr = ArrayStorageFromArray( *object_ptr );
                ( *cmd->delete_1.op_del )( array_ptr );
              } break;
              case DTC_DLT_2 :
              { void** object_ptr;          // - addr[ pointer to object ]
                object_ptr = PointUsingOffset( void*
                                             , rw
                                             , cmd->delete_2.offset );
                ( *cmd->delete_2.op_del )( *object_ptr, cmd->delete_2.size );
              } break;
              case DTC_DLT_2_ARRAY :
              { void** object_ptr;          // - addr[ pointer to object ]
                object_ptr = PointUsingOffset( void*
                                             , rw
                                             , cmd->delete_2.offset );
                ARRAY_STORAGE* array_ptr = ArrayStorageFromArray( *object_ptr );
                ( *cmd->delete_2.op_del )( array_ptr
                                         , cmd->delete_2.size
                                           * array_ptr->element_count
                                          + sizeof( array_ptr->element_count )
                                         );
              } break;
            }
        } else {
            obj_addr = PointUsingOffset( void
                                       , rw
                                       , curr_state->u.data_offset );
            (*dtor)( obj_addr, DTOR_NULL );
        }
    }
    rw->base.state_var = state_var;
}


extern "C"
void CPPLIB( destruct_internal )// DESTRUCTION FOR BLK UNTIL STATE REACHED
    ( RT_STATE_VAR state_var    // - state variable
    , RW_DTREG *rw )            // - R/W for block
{
#ifdef RW_REGISTRATION
    _RTCTL rt_ctl;              // - R/T control

    destruct_traverse( state_var, &rt_ctl, rw );
#else
    CPPLIB( destruct )( rw, state_var );
#endif
}

#ifdef RW_REGISTRATION
extern "C"
_WPRTLINK
void CPPLIB( destruct_all )(    // R/T CALL -- destruct remainder of block
    void )
{
    CPPLIB( destruct )( 0 );
}
#else
extern "C"
_WPRTLINK
void CPPLIB( destruct_all )(    // R/T CALL -- destruct remainder of block
    RW_DTREG* rw )              // - active r/w entry
{
    CPPLIB( destruct )( rw, 0 );
}
#endif


#ifdef RW_REGISTRATION
extern "C"
_WPRTLINK
void CPPLIB( destruct )(        // R/T CALL -- destruct up to state variable
    RT_STATE_VAR state_var )    // - value of state variable
{
    _RTCTL rt_ctl;              // - R/T control

    destruct_traverse( state_var, &rt_ctl, RwTop( rt_ctl.thr ) );
}
#else
extern "C"
_WPRTLINK
void CPPLIB( destruct )(        // R/T CALL -- destruct up to state variable
    RW_DTREG* rw,               // - active r/w entry
    RT_STATE_VAR state_var )    // - value of state variable
{
    _RTCTL rt_ctl;              // - R/T control

    destruct_traverse( state_var, &rt_ctl, rw );
}
#endif


#ifndef NDEBUG

extern "C"                      // DETERMINES IF COMMAND ACTIVE, SO DEBUGGING
int CPPLIB( cmd_active )(       // INFO CAN BE DISPLAYED ABOUT IT
    RW_DTREG* rw,               // - active r/w entry
    DTOR_CMD* cmd )             // - command in question
{
    int retn;                   // - true ==> command was active
    _RTCTL rt_ctl;              // - R/T control
    STAB_TRAVERSE traversal;    // - traversal information
    RO_STATE* curr_state;       // - current state entry

    rt_ctl.setRwRo( rw );
    CPPLIB( stab_trav_init )( &traversal, &rt_ctl );
    for( ; ; ) {
        curr_state = nextDtorPosn( &traversal );
        if( traversal.state_var == 0 ) {
            retn = 0;
            break;
        }
        CPPLIB( stab_trav_next )( &traversal );
        if( curr_state->dtor == NULL ) {
            if( cmd == curr_state->u.cmd_addr ) {
                retn = 1;
                break;
            }
        }
    }
    return retn;
}

#endif
