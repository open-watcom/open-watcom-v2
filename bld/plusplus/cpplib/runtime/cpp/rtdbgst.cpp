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


#ifndef NDEBUG

#include "cpplib.h"

#include <stdio.h>
#include <stdarg.h>

#include "rtexcept.h"
#include "exc_pr.h"


static void dumpDtorCmd( RW_DTREG*, DTOR_CMD* );


enum FT                         // types of formatting
{   FT_RW                       // - R/W header
,   FT_RO                       // - R/O header
,   FT_STATE                    // - state table
,   FT_FLAGS                    // - flags
,   FT_BYTE                     // - byte
,   FT_TEXT                     // - text
,   FT_PTR                      // - data ptr
,   FT_RTN                      // - code ptr
,   FT_OFF                      // - offset
,   FT_DTOFF                    // - (dtor,offset) list
,   FT_DTADDR                   // - (dtor,addr) list
,   FT_TSIG                     // - type signature
,   FT_TSIGS                    // - type signature list
,   FT_INDENT                   // - new line, indent
,   FT_CONT                     // - continue
,   FT_END                      // - end
};


static void dumpTitle(          // DUMP A TITLE
    const char* title )         // - the title
{
    printf( "%s\n\n", title );
}


static unsigned indent = 1;     // # of indentations


static void dump(               // FORMATTED DUMP
    enum FT ft, ... )           // - FT formatting
{
    va_list args;               // - for variable arguments
    rboolean done;              // - TRUE ==> done formatting
    size_t blk_type;            // - type of block
    RW_DTREG* rw;               // - R/W header

    va_start( args, ft );
    for( done = FALSE; ! done; ft = va_arg( args, enum FT ) ) {
        switch( ft ) {
          case FT_RW :
          { const char* text;   // - text
            text = va_arg( args, char* );
            rw = va_arg( args, RW_DTREG* );
            blk_type = rw->base.ro->base.reg_type;
            dump( FT_TEXT, "\nBLK-RW:"
                , FT_PTR, text, rw
#ifdef RW_REGISTRATION
                , FT_PTR, "prev", rw->base.prev
#endif
                , FT_PTR, "ro", rw->base.ro
                , FT_OFF, "state_var", rw->base.state_var
                , FT_CONT );
          } break;
          case FT_RO :
          { const char* text;   // - text
            RO_DTREG* ro;       // - R/O header
            text = va_arg( args, char* );
            ro = va_arg( args, RO_DTREG* );
            dump( FT_TEXT, "\nBLK-RO:"
                , FT_OFF, "type", ro->base.reg_type
                , FT_CONT );
          } break;
          case FT_FLAGS :
          { const char* text;   // - text
            uint_8 *ptr;        // - pointer to flags
            unsigned count;     // - offset
            unsigned bits;      // - # bits left in byte
            uint_8 byte;        // - contains flags (being shifted)
            text = va_arg( args, char* );
            count = va_arg( args, unsigned );
            ptr = va_arg( args, uint_8* );
            printf( " %s=", text );
            for( bits = 0; count > 0; --count, --bits, byte >>= 1 ) {
                if( bits == 0 ) {
                    byte = *ptr++;
                    bits = 8;
                }
                printf( byte & 1 ? "1" : "0" );
            }
          } break;
          case FT_BYTE :
          { const char* text;   // - text
            uint_8 byte;        // - byte
            text = va_arg( args, char* );
            byte = va_arg( args, uint_8 );
            printf( " %s=%x", text, byte );
          } break;
          case FT_TEXT :
          { const char* text;   // - text
            text = va_arg( args, char* );
            printf( " %s", text );
          } break;
          case FT_PTR :
          { const char* text;   // - text
            void *ptr;          // - pointer to data
            text = va_arg( args, char* );
            ptr = va_arg( args, void* );
            printf( " %s=%x", text, ptr );
          } break;
          case FT_RTN :
          { const char* text;   // - text
            pFUNVOIDVOID rtn;   // - routine
            text = va_arg( args, char* );
            rtn = va_arg( args, pFUNVOIDVOID );
            printf( " %s=%x", text, rtn );
          } break;
          case FT_OFF :
          { const char* text;   // - text
            unsigned offset;    // - offset
            text = va_arg( args, char* );
            offset = va_arg( args, unsigned );
            printf( " %s=%x", text, offset );
          } break;
          case FT_TSIG :
          { RT_TYPE_SIG sig;    // - type signature
            THROBJ type;        // - type of signature
            sig = va_arg( args, RT_TYPE_SIG );
            if( sig == NULL ) {
                type = THROBJ_ANYTHING;
            } else {
                type = sig->hdr.type;
            }
            dump( FT_INDENT
                , FT_PTR, "type-sig", sig
                , FT_BYTE, "type", type
                , FT_CONT );
            switch( type ) {
              case THROBJ_REFERENCE :
                sig = CPPLIB( ts_refed )( sig );
                dump( FT_PTR, "ref-indirect", sig
                    , FT_CONT );
                break;
              case THROBJ_PTR_CLASS :
                sig = CPPLIB( ts_pnted )( sig );
                dump( FT_PTR, "ptr-class", sig
                    , FT_CONT );
                break;
              case THROBJ_PTR_SCALAR :
                if( sig->base.indirect ) {
                    sig = CPPLIB( ts_pnted )( sig );
                    dump( FT_PTR, "ptr-scalar", sig
                        , FT_CONT );
                }
                break;
              case THROBJ_CLASS :
              case THROBJ_CLASS_VIRT :
              case THROBJ_SCALAR :
              case THROBJ_ANYTHING :
              case THROBJ_VOID_STAR :
              case THROBJ_PTR_FUN :
                break;
            }
            switch( type ) {
              case THROBJ_CLASS :
              case THROBJ_CLASS_VIRT :
                dump( FT_RTN, "ctor-def", sig->clss.ctor
                    , FT_RTN, "ctor-copy", sig->clss.copyctor
                    , FT_RTN, "dtor", sig->clss.dtor
                    , FT_CONT
                    , FT_OFF, "size", sig->clss.size
                    , FT_PTR, "name", sig->clss.name
                    , FT_TEXT, sig->clss.name
                    , FT_CONT );
                break;
              case THROBJ_PTR_SCALAR :
                if( sig->base.indirect ) {
                    dump( FT_PTR, "ptr-scalar-indirect", sig->indirected.sig
                        , FT_CONT );
                } else {
                    dump( FT_OFF, "ptr-scalar size", sig->scalar.size
                        , FT_CONT );
                }
                break;
              case THROBJ_SCALAR :
                dump( FT_OFF, "scalar size", sig->scalar.size
                    , FT_PTR, "name", sig->scalar.name
                    , FT_TEXT, sig->scalar.name
                    , FT_CONT );
                break;
              case THROBJ_ANYTHING :
                printf( " ..." );
                break;
              case THROBJ_VOID_STAR :
                dump( FT_OFF, "void* size", sig->scalar.size
                    , FT_CONT );
                break;
              case THROBJ_PTR_FUN :
                dump( FT_OFF, "ptr-fun size", sig->scalar.size
                    , FT_PTR, "name", sig->scalar.name
                    , FT_TEXT, sig->scalar.name
                    , FT_CONT );
                break;
            }
          } break;
          case FT_TSIGS :
          { unsigned ctr;       // - counter
            RT_TYPE_SIG *sig;   // - type signature list
            ++indent;
            for( ctr = va_arg( args, unsigned ),
                 sig = va_arg( args, RT_TYPE_SIG* )
               ; ctr > 0
               ; --ctr, ++sig  ) {
                dump( FT_TSIG, *sig
                    , FT_CONT );
            }
            --indent;
          } break;
          case FT_STATE :       // state table
          { RO_STATE* state;    // - current state
            DTOR_CMD* cmd;      // - command from state table
            size_t index;       // - index
            state = va_arg( args, RO_STATE* );
            dump( FT_PTR, "STATE TABLE", state
                , FT_END );
            for( index = 1; ; ++state, ++index ) {
                if( state->dtor == 0 ) {
                    cmd = state->u.cmd_addr;
                    if( cmd == 0 ) break;
                    dump( FT_OFF,    "index", index,
                          FT_PTR,    "cmd",   cmd,
                          FT_CONT );
                    dumpDtorCmd( rw, cmd );
                } else {
                    dump( FT_OFF,   "index",    index,
                          FT_RTN,   "dtor",     state->dtor,
                          FT_CONT );
                    switch( blk_type ) {
                      case DTRG_FUN :
                        dump( FT_OFF,   "offset",   state->u.data_offset,
                              FT_PTR,   "addr",
                                        PointOffset( rw
                                                   , state->u.data_offset ),
                              FT_END );
                        break;
                      case DTRG_STATIC_INITLS :
                        dump( FT_PTR,   "addr",     state->u.data_addr,
                              FT_END );
                        break;
                      case DTRG_ARRAY :
                        dump( FT_END );
                        break;
                    }
                }
            }
          } break;
          case FT_INDENT :
          { unsigned ctr;       // - counter
            printf( "\n" );
            for( ctr = indent; ctr > 0; --ctr ) {
                printf( "    " );
            }
          } break;
          case FT_CONT :
            done = TRUE;
            break;
          case FT_END :
            printf( "\n" );
            fflush( stdout );
            done = TRUE;
            break;
        }
    }
    va_end( args );
}


static void dumpObjRegistration(// DUMP RW_DTREG_OBJ, RO_DTREG_OBJ
    RW_DTREG_OBJECT* rw,        // - R/W block
    unsigned offset )           // - offset of component
{
    dump( FT_PTR,   "RW_OBJ",   rw,
          FT_PTR,   "object",   rw->object,
          FT_BYTE,  "cdtor",    rw->cdtor,
          FT_PTR,   "component", PointOffset( rw->object, offset ),
          FT_END );
}


static void dumpArrayInit(      // DUMP ARRAY INITIALIZATION
    RT_ARRAY_INIT* init )       // - R/W block
{
    dump( FT_PTR,   "ARRAY-INIT",   init
        , FT_PTR,   "array",        init->array
        , FT_OFF,   "index",        init->index
        , FT_TSIG,                  init->sig
        , FT_END );
}


static void dumpCmdComponent(   // DUMP A COMPONENT COMMAND
    RW_DTREG* rw,               // - current R/W table
    DTOR_CMD* cmd,              // - current command
    const char* name )          // - command name
{
    RW_DTREG_OBJECT* init;      // - initialization structure

    init = PointUsingOffset( RW_DTREG_OBJECT
                           , rw
                           , cmd->component.offset_init );
    dump( FT_TEXT, name
        , FT_OFF,  "off-init",     cmd->component.offset_init
        , FT_OFF,  "off-obj",      cmd->component.offset_object
        , FT_RTN,  "dtor",         cmd->component.dtor
        , FT_PTR,  "add_init",     init
        , FT_END );
    if( CPPLIB( cmd_active )( rw, cmd ) ) {
        ++indent;
        dumpObjRegistration( init, cmd->component.offset_object );
        --indent;
    }
}



static void dumpDtorCmd(        // DUMP DTOR COMMAND
    RW_DTREG* rw,               // - current R/W table
    DTOR_CMD* cmd )             // - current command
{
    switch( cmd->base.code ) {
      case DTC_CTOR_TEST :
        dump( FT_TEXT, "CTOR_TEST"
            , FT_OFF,  "index", cmd->ctor_test.index
            , FT_END );
        break;
      case DTC_COMP_MEMB :
        dumpCmdComponent( rw, cmd, "COMP_MEMB" );
        break;
      case DTC_COMP_DBASE :
        dumpCmdComponent( rw, cmd, "COMP_DBASE" );
        break;
      case DTC_COMP_VBASE :
        dumpCmdComponent( rw, cmd, "COMP_VBASE" );
        break;
      case DTC_ACTUAL_DBASE :
        dumpCmdComponent( rw, cmd, "ACTUAL_DBASE" );
        break;
      case DTC_ACTUAL_VBASE :
        dumpCmdComponent( rw, cmd, "ACTUAL_VBASE" );
        break;
      case DTC_SET_SV :
        dump( FT_TEXT, "SET_SV"
            , FT_OFF,  "state_var", cmd->set_sv.state_var
            , FT_END );
        break;
      case DTC_TRY :
        dump( FT_TEXT, "TRY"
            , FT_OFF,  "state", cmd->try_cmd.state
            , FT_OFF,  "jmpbuf", cmd->try_cmd.jmp_buf
            , FT_PTR,  "addr", PointOffset( rw, cmd->try_cmd.jmp_buf )
            , FT_OFF,  "offset", cmd->try_cmd.offset
            , FT_OFF,  "count", cmd->try_cmd.count
            , FT_TSIGS, cmd->try_cmd.count, cmd->try_cmd.sigs
            , FT_END );
        break;
      case DTC_CATCH :
        dump( FT_TEXT, "CATCH"
            , FT_PTR, "try", TryFromCatch( cmd )
            , FT_END );
        break;
      case DTC_FN_EXC :
        dump( FT_TEXT, "FN_EXC"
            , FT_OFF,  "count", cmd->fn_exc.count
            , FT_TSIGS, cmd->fn_exc.count, cmd->fn_exc.sigs
            , FT_END );
        break;
      case DTC_TEST_FLAG :
        dump( FT_TEXT, "TEST_FLAG"
            , FT_OFF,  "index", cmd->test_flag.index
            , FT_OFF,  "sv-true", cmd->test_flag.state_var_true
            , FT_OFF,  "sv-false", cmd->test_flag.state_var_false
            , FT_END );
        break;
      case DTC_ARRAY_INIT :
      { RT_ARRAY_INIT* init;        // - run-time array initialization
        init = PointUsingOffset( RT_ARRAY_INIT, rw, cmd->array_init.offset );
        dump( FT_TEXT, "ARRAY_INIT"
            , FT_OFF,  "offset", cmd->array_init.offset
            , FT_PTR,  "addr", init
            , FT_END );
        if( CPPLIB( cmd_active )( rw, cmd ) ) {
            dumpArrayInit( init );
        }
      } break;
      case DTC_DLT_1 :
        dump( FT_TEXT, "DLT_1"
            , FT_OFF,  "offset", cmd->delete_1.offset
            , FT_RTN,  "op-delete", cmd->delete_1.op_del
            , FT_CONT );
        if( CPPLIB( cmd_active )( rw, cmd ) ) {
            dump( FT_PTR, "object", PointOffset( rw, cmd->delete_1.offset )
                , FT_END );
        } else {
            dump( FT_END );
        }
        break;
      case DTC_DLT_1_ARRAY :
        dump( FT_TEXT, "DLT_1_ARRAY"
            , FT_OFF,  "offset", cmd->delete_1.offset
            , FT_RTN,  "op-delete", cmd->delete_1.op_del
            , FT_CONT );
        if( CPPLIB( cmd_active )( rw, cmd ) ) {
            dump( FT_PTR, "object", PointOffset( rw, cmd->delete_1.offset )
                , FT_END );
        } else {
            dump( FT_END );
        }
        break;
      case DTC_DLT_2 :
        dump( FT_TEXT, "DLT_2"
            , FT_OFF,  "offset", cmd->delete_2.offset
            , FT_RTN,  "op-delete", cmd->delete_2.op_del
            , FT_OFF,  "size", cmd->delete_2.size
            , FT_CONT );
        if( CPPLIB( cmd_active )( rw, cmd ) ) {
            dump( FT_PTR, "object", PointOffset( rw, cmd->delete_2.offset )
                , FT_END );
        } else {
            dump( FT_END );
        }
        break;
      case DTC_DLT_2_ARRAY :
        dump( FT_TEXT, "DLT_2_ARRAY"
            , FT_OFF,  "offset", cmd->delete_2.offset
            , FT_RTN,  "op-delete", cmd->delete_2.op_del
            , FT_OFF,  "size", cmd->delete_2.size
            , FT_CONT );
        if( CPPLIB( cmd_active )( rw, cmd ) ) {
            dump( FT_PTR, "object", PointOffset( rw, cmd->delete_2.offset )
                , FT_END );
        } else {
            dump( FT_END );
        }
        break;
      default :
        dump( FT_TEXT, "**** UNRECOGNIZED COMMAND ****"
            , FT_END );
        break;
    }
}


static void dumpRwRoBlk(        // DUMP R/W, R/O BLOCK
    RW_DTREG* rw,               // - R/W block
    RO_DTREG* ro )              // - R/O block
{

    switch( ro->base.reg_type ) {
      case DTRG_FUN :
        dump( FT_RW,    "RW_FUN",   rw,
              FT_BYTE,  "flags",    rw->fun.flags[0],
              FT_RO,    "RO_FUN",   ro,
              FT_STATE,             ro->fun.state_table,
              FT_END );
        break;
#if 0
      case DTRG_ARRAY :
        dump( FT_RW,    "RW_ARR",   rw,
              FT_PTR,   "array",    rw->array.array,
              FT_RO,    "RO_ARR",   ro,
              FT_TSIG,              ro->array.sig,
              FT_STATE,             ro->array.state_table,
              FT_END );
        break;
      case DTRG_STATIC_INITLS :
        dump( FT_RW,    "RW_INIT_LS", rw,
              FT_PTR,   "object",     rw->init_ls.object,
              FT_RO,    "RO_INIT_LS", ro,
              FT_STATE,               ro->init_ls.state_table,
              FT_END );
        break;
#endif
      default :
        printf( "*** UNKNOWN BLOCKS *** RW=%x RO=%x\n\n", rw, ro );
        break;
    }
    printf( "\n" );
}


#ifdef FS_REGISTRATION
static void dumpFsBlock(        // DUMP NON-WATCOM FS BLOCK
    RW_DTREG* rw )              // - R/W block
{
    dump( FT_PTR, "\nBLK-FS:", rw
        , FT_PTR, "prev", rw->base.prev
        , FT_RTN, "handler", rw->base.handler
        , FT_END );
}
#endif


#ifdef PD_REGISTRATION
static void dumpPdata           // DUMP PDATA BLOCK
    ( PData* p )                // block
{
    dump( FT_PTR,   "\nPDATA:", p
        , FT_RTN,   "entry",    p->entry
        , FT_RTN,   "end",      p->end
        , FT_RTN,   "handler",  p->exc
        , FT_PTR,   "data",     p->exc_data
        , FT_RTN,   "endpr",    p->endpr
        , FT_END );
}
#endif


static void dumpDtorList        // DUMP REGISTRATION LIST
    ( const char* title         // - title
#ifdef RW_REGISTRATION
    , RW_DTREG* list            // - list to be dumped
#endif
    )
{
    indent = 0;
    dumpTitle( title );
#ifdef FS_REGISTRATION
    {
        for( ; list != (void*)-1L; list = list->base.prev ) {
            if( list->base.handler == & CPPLIB( fs_handler ) ) {
                dumpRwRoBlk( list, list->base.ro );
            } else {
                dumpFsBlock( list );
            }
        }
    }
#elif defined( PD_REGISTRATION )
    {
        CPPLIB( pd_dump_rws )( &dumpRwRoBlk, &dumpPdata );
    }
#else
    {
        for( ; list != NULL; list = list->base.prev ) {
            dumpRwRoBlk( list, list->base.ro );
        }
    }
#endif
}


static void dumpExc(            // DUMP EXCEPTION BLOCK
    ACTIVE_EXC* exc )           // - current exception
{
    dump( FT_PTR    ,"\nEXCEPTION"  ,exc
        , FT_PTR    ,"prev"         ,exc->prev
        , FT_PTR    ,"throw"        ,exc->throw_ro
        , FT_BYTE   ,"state"        ,exc->state
        , FT_BYTE   ,"fnexc-state"  ,exc->fnexc_state
        , FT_BYTE   ,"zero"         ,exc->zero_thrown
        , FT_PTR    ,"extra"        ,exc->extra_object
        , FT_PTR    ,"\n exc_area"  ,exc->exc_area
        , FT_PTR    ,"cat_try"      ,exc->cat_try
        , FT_PTR    ,"dispatch"     ,exc->dispatch
        , FT_PTR    ,"rw"           ,exc->rw
        , FT_TSIG                   ,exc->sig
        , FT_END );
}


static void dumpExcs(           // DUMP EXCEPTIONS LIST
    void )
{
    THREAD_CTL *thr;            // - thread-specific information
    ACTIVE_EXC* exc;            // - current exception

    thr = PgmThread();
    dumpTitle( "Pending Exceptions:" );
    if( NULL == thr->excepts ) {
        printf( "    *** No active exceptions\n" );
    } else {
        for( exc = thr->excepts; exc != NULL; exc = exc->prev ) {
            dumpExc( exc );
        }
    }
    dump( FT_PTR, "\nThread Control Block:", thr
        , FT_BYTE, "\n  terminated",   thr->flags.terminated
        , FT_PTR,  "\n  excepts:",     thr->excepts
        , FT_RTN,  "unexpected",       thr->unexpected
        , FT_RTN,  "terminate",        thr->terminate
        , FT_RTN,  "\n  new_handler",  thr->new_handler
        , FT_RTN,  "_new_handler",     thr->_new_handler
        , FT_PTR,  "exc_pr",           thr->exc_pr
        , FT_END );
    if( NULL != thr->exc_pr ) {
        puts( "\n_EXC_PR blocks\n" );
        for( _EXC_PR* excpr = thr->exc_pr
           ; excpr != NULL
           ; excpr = excpr->_prev ) {
            dump( FT_PTR,   "_EXC_PR",  excpr
                , FT_BYTE,  "_type",    excpr->_type
                , FT_PTR,   "rw",       excpr->_rw
                , FT_PTR,   "rtctl",    excpr->_rtc
                , FT_PTR,   "prev",     excpr->_prev
                , FT_BYTE,  "state",    excpr->_state
                , FT_CONT );
            switch( excpr->_type ) {
              default :
                dump( FT_TEXT,  "***BAD TYPE***"
                    , FT_END );
                break;
              case EXCPR_BASIC :
                dump( FT_END );
                break;
              case EXCPR_FREE :
              {
                _EXC_PR_FREE* p = (_EXC_PR_FREE*)excpr;
                dump( FT_TEXT,  "FREE"
                    , FT_PTR,   "_exc",     p->_exc
                    , FT_END );
                break;
              }
              case EXCPR_DTOR :
              {
                _EXC_PR_DTOR* p = (_EXC_PR_DTOR*)excpr;
                dump( FT_TEXT,  "DTOR"
                    , FT_PTR,   "_exc",     p->_exc
                    , FT_END );
                break;
              }
              case EXCPR_FNEXC :
              {
                _EXC_PR_FNEXC* p = (_EXC_PR_FNEXC*)excpr;
                dump( FT_TEXT,  "FNEXC"
                    , FT_PTR,   "_exc",     p->_exc
                    , FT_PTR,   "_skip",    p->_fnexc_skip
                    , FT_END );
                break;
              }
            }
        }
    }
}


extern "C"
void CPPLIB( DbgRtDumpModuleDtor )( // DUMP MODULE DTOR BLOCKS
    void )
{
#ifdef RW_REGISTRATION
    THREAD_CTL *thr;            // - thread-specific information

    thr = PgmThread();
    dumpDtorList( "\nStatic Initialization List:", _RWD_ModuleInit );
#else
    printf( "\nStatic Initialization List:\n" );
    RW_DTREG* list             // - list to be dumped
        = _RWD_ModuleInit;
    for( ; list != (void*)-1L; list = list->init_ls_st.base.prev ) {
        dumpRwRoBlk( list, list->base.ro );
    }
#endif
    printf( "\n" );
    fflush( stdout );
}


extern "C"
void CPPLIB( DbgRtDumpAutoDtor )( // DUMP REGISTRATION BLOCKS
    void )
{
#ifdef RW_REGISTRATION
    THREAD_CTL *thr;            // - thread-specific information

    thr = PgmThread();
    dumpDtorList( "\nRegistration List:", RwTop( thr ) );
#else
    dumpDtorList( "\nRegistration List:" );
#endif
    dumpExcs();
    printf( "\n" );
    fflush( stdout );
}

#endif


#ifdef PD_REGISTRATION

extern "C"
void __DumpPdata()
{
    PData* p = (PData*)0x430000;
    PData* l = 0;
    printf( "PDATA at 430000\n\n" );
    for( ; p->entry < p->end; ++p ) {
        if( p <= l ) {
            printf( "Out of order\n" );
        }
        printf( "%8x %8x %8x %8x %8x %8x "
              , p->entry
              , p->end
              , p->exc
              , p->exc_data
              , p->endpr
              , (char*)p->endpr - (char*)p->entry );
        if( p->endpr < p->entry
         || p->endpr >= p->end ) {
            printf( "BAD" );
        }
        printf( "\n" );
        l = p;
    }
    fflush( stdout );
}
#endif

extern "C" {

    #include "..\..\..\c\dbgio.c"

};
