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

#include <float.h>

#include "cgback.h"
#include "cgbackut.h"
#include "typesig.h"
#include "ctexcept.h"
#include "carve.h"
#include "ring.h"
#include "initdefs.h"

#ifndef NDEBUG
    #include "errdefns.h"
    #include <stdio.h>
    #include "dbg.h"
    #include "toggle.h"
    #include "pragdefn.h"
#endif

enum                                    // INDICES FOR STATE-TABLE COMMANDS
{   DTOR_CMD_INDEX_VBASE                // - virtual base
,   DTOR_CMD_INDEX_DBASE                // - direct base
,   DTOR_CMD_INDEX_SETSV                // - set sv (must be last)
};


static carve_t carveCMD_SET_SV;         // allocations for CMD_SET_SV
static carve_t carveCMD_TEST_FLAG;      // allocations for CMD_TEST_FLAG
static carve_t carveCMD_TRY;            // allocations for CMD_TRY
static carve_t carveCMD_FN_EXC;         // allocations for CMD_FN_EXC
static carve_t carveCMD_COMPONENT;      // allocations for CMD_COMPONENT
static carve_t carveCMD_ARRAY_INIT;     // allocations for CMD_ARRAY_INIT
static carve_t carveCMD_DEL_1;          // allocations for CMD_DEL_1
static carve_t carveCMD_DEL_2;          // allocations for CMD_DEL_2
static carve_t carveCMD_DEL_1_ARRAY;    // allocations for CMD_DEL_1_ARRAY
static carve_t carveCMD_DEL_2_ARRAY;    // allocations for CMD_DEL_2_ARRAY
static carve_t carveCMD_CTOR_TEST;      // allocations for CMD_CTOR_TEST

static CMD_SET_SV* ringCmdsSetSv;       // commands: SET_SV
static CMD_TEST_FLAG* ringCmdsTestFlag; // commands: TEST_FLAG
static CMD_COMPONENT* ringCmdsComponent;// commands: COMPONENT (function)
static CMD_TRY* ringCmdsTry;            // commands: TRY
static CMD_FN_EXC* ringCmdsFnExc;       // commands: FN_EXC
static CMD_DEL_1* ringCmdsDel1;         // commands: DEL_1
static CMD_DEL_2* ringCmdsDel2;         // commands: DEL_2
static CMD_DEL_1* ringCmdsDel1Array;    // commands: DEL_1_ARRAY
static CMD_DEL_2* ringCmdsDel2Array;    // commands: DEL_2_ARRAY
static CMD_ARRAY_INIT* ringCmdsArrayInit;//commands: array initialization
static CMD_CTOR_TEST* ringCmdsCtorTest; // commands: CTOR_TEST


static void* stateTableCmdAlloc(// ALLOCATE STATE-TABLE CMD
    carve_t allocation,         // - allocation
    void* hdr )                 // - ring hdr
{
    CMD_BASE* cmd;

    cmd = RingCarveAlloc( allocation, hdr );
    cmd->emitted = FALSE;
    cmd->sym = NULL;
    return cmd;
}


static void* stateTableCmdAllocVar( // ALLOCATE STATE-TABLE CMD, SET VAR
    carve_t allocation,         // - allocation
    void* hdr,                  // - ring hdr
    target_size_t size )        // - size of command
{
    CMD_BASE* cmd;

    cmd = stateTableCmdAlloc( allocation, hdr );
    cmd->sym = CgVarRo( size, SC_STATIC, NULL );
    return cmd;
}


static SYMBOL stateTableCmdName(// CREATE COMDEF VARIABLE FOR DTOR_CMD
    unsigned index )            // - command index
{
    return CgVarRo( 1, SC_PUBLIC, CppNameStateTableCmd( index ) );
}


SYMBOL CgCmdFnExc(              // GET SYMBOL FOR FN-EXCEPTION SPEC. COMMAND
    SE* se )                    // - state entry in state table
{
    TYPE_SIG_ENT* sigs;         // - ring of signatures
    CMD_FN_EXC* cmd;            // - command

    sigs = BeTypeSigEntsCopy( se->fn_exc.sigs );
    cmd = stateTableCmdAllocVar
                    ( carveCMD_FN_EXC
                    , &ringCmdsFnExc
                    , 2 +
                      CgbkInfo.size_offset +
                      RingCount( sigs ) * CgbkInfo.size_data_ptr );
    cmd->sigs = sigs;
    return cmd->base.sym;
}


SYMBOL CgCmdTestFlag(           // GET SYMBOL FOR TEST_FLAG CMD TO BE GEN'ED
    SE* se )                    // - state entry in state table
{
    CMD_TEST_FLAG* cmd;         // - command for entry
    CMD_TEST_FLAG* curr;        // - current command
    STATE_VAR sv_true;          // - true state
    STATE_VAR sv_false;         // - false state

    cmd = NULL;
    sv_true  = SeStateVar( se->test_flag.se_true  );
    sv_false = SeStateVar( se->test_flag.se_false );
    RingIterBeg( ringCmdsTestFlag, curr ) {//{
        if(  se->test_flag.index == curr->index
          && sv_true             == curr->state_var_true
          && sv_false            == curr->state_var_false ) {
            cmd = curr;
            break;
        }
    } RingIterEnd( curr )
    if( cmd == NULL ) {
        cmd = stateTableCmdAllocVar( carveCMD_TEST_FLAG
                                   , &ringCmdsTestFlag
                                   , 2 + 3 * CgbkInfo.size_offset );
        cmd->index           = se->test_flag.index;
        cmd->state_var_true  = sv_true;
        cmd->state_var_false = sv_false;
    }
    return cmd->base.sym;
}


SYMBOL CgCmdSetSv(              // GET SYMBOL FOR SET_SV CMD TO BE GEN'ED
    SE* se )                    // - state entry in state table
{
    CMD_SET_SV* cmd;            // - command for entry
    CMD_SET_SV* curr;           // - current command
    STATE_VAR state;            // - state for set_sv

    cmd = NULL;
    state = SeStateVar( se->set_sv.se );
    RingIterBeg( ringCmdsSetSv, curr ) {
        if( state == curr->state_var ) {
            cmd = curr;
            break;
        }
    } RingIterEnd( curr )
    if( cmd == NULL ) {
        cmd = stateTableCmdAlloc( carveCMD_SET_SV, &ringCmdsSetSv );
        cmd->state_var = state;
        cmd->base.sym = stateTableCmdName( state + DTOR_CMD_INDEX_SETSV );
    }
    return cmd->base.sym;
}


// note: the code disabled does not work because it checks offset; it should
//       check the offset relative to the R/W block, because that it what
//       is generated ultimately.
//
// fix in future by computing the relative offset and using it
//
//
SYMBOL CgCmdComponent(          // GET SYMBOL FOR DTC_COMP... COMMAND
    SE* se )                    // - state entry in state table
{
    CMD_COMPONENT* cmd;         // - command

    DbgVerify( UNDEF_AREL != se->component.obj->offset
             , "CgCmdComponent -- no offset" );
    cmd = stateTableCmdAllocVar( carveCMD_COMPONENT
                               , &ringCmdsComponent
                               , 1 );
    cmd->obj = se->component.obj;
    cmd->dtor = se->component.dtor;
    cmd->offset = se->component.offset;
    cmd->cmd_type = se->base.se_type;
    return cmd->base.sym;
}


SYMBOL CgCmdArrayInit(          // GET SYMBOL FOR DTC_ARRAY_INIT COMMAND
    SE* se )                    // - state entry in state table
{
    CMD_ARRAY_INIT* cmd;        // - command

    DbgVerify( UNDEF_AREL != se->array_init.reg->offset
             , "cgAddCmdArrayInit -- no offset" );
    cmd = stateTableCmdAllocVar( carveCMD_ARRAY_INIT
                               , &ringCmdsArrayInit
                               , 1 );
    cmd->reg = se->array_init.reg;
    return cmd->base.sym;
}


SYMBOL CgCmdDel1(               // GET SYMBOL FOR DTC_DEL_1
    SE* se )                    // - state entry in state table
{
    CMD_DEL_1* cmd;             // - command for operation
    CMD_DEL_1* cur;             // - current command

    DbgVerify( UNDEF_AREL != se->del_1.offset
             , "CgCmdDel1 -- no offset for DLT_1" );
    cmd = NULL;
    RingIterBeg( ringCmdsDel1, cur ) {
        if( cur->op_del == se->del_1.op_del
         && cur->offset == se->del_1.offset ) {
            cmd = cur;
            break;
        }
    } RingIterEnd( cur )
    if( cmd == NULL ) {
        cmd = stateTableCmdAllocVar( carveCMD_DEL_1
                                   , &ringCmdsDel1
                                   , 1 );
        cmd->op_del = se->del_1.op_del;
        cmd->offset = CgOffsetRw( se->del_1.offset );
    }
    return cmd->base.sym;
}


SYMBOL CgCmdDel1Array(          // GET SYMBOL FOR DTC_DEL_1
    SE* se )                    // - state entry in state table
{
    CMD_DEL_1_ARRAY* cmd;       // - command for operation
    CMD_DEL_1_ARRAY* cur;       // - current command

    DbgVerify( UNDEF_AREL != se->del_1_array.offset
             , "CgCmdDel1 -- no offset for DLT_1" );
    cmd = NULL;
    RingIterBeg( ringCmdsDel1Array, cur ) {
        if( cur->op_del == se->del_1_array.op_del
         && cur->offset == se->del_1_array.offset ) {
            cmd = cur;
            break;
        }
    } RingIterEnd( cur )
    if( cmd == NULL ) {
        cmd = stateTableCmdAllocVar( carveCMD_DEL_1_ARRAY
                                   , &ringCmdsDel1Array
                                   , 1 );
        cmd->op_del = se->del_1_array.op_del;
        cmd->offset = CgOffsetRw( se->del_1_array.offset );
    }
    return cmd->base.sym;
}


SYMBOL CgCmdDel2(               // GET SYMBOL FOR DTC_DEL_2
    SE* se )                    // - state entry in state table
{
    CMD_DEL_2* cmd;             // - command for operation
    CMD_DEL_2* cur;             // - current command

    DbgVerify( UNDEF_AREL != se->del_2.offset
             , "CgCmdDel2 -- no offset for DLT_2" );
    cmd = NULL;
    RingIterBeg( ringCmdsDel2, cur ) {
        if( cur->op_del == se->del_2.op_del
         && cur->offset == se->del_2.offset
         && cur->size   == se->del_2.size ) {
            cmd = cur;
            break;
        }
    } RingIterEnd( cur )
    if( cmd == NULL ) {
        cmd = stateTableCmdAllocVar( carveCMD_DEL_2
                                   , &ringCmdsDel2
                                   , 1 );
        cmd->op_del = se->del_2.op_del;
        cmd->offset = CgOffsetRw( se->del_2.offset );
        cmd->size   = se->del_2.size;
    }
    return cmd->base.sym;
}


SYMBOL CgCmdDel2Array(          // GET SYMBOL FOR DTC_DEL_2
    SE* se )                    // - state entry in state table
{
    CMD_DEL_2_ARRAY* cmd;       // - command for operation
    CMD_DEL_2_ARRAY* cur;       // - current command

    DbgVerify( UNDEF_AREL != se->del_2_array.offset
             , "CgCmdDel2 -- no offset for DLT_2" );
    cmd = NULL;
    RingIterBeg( ringCmdsDel2Array, cur ) {
        if( cur->op_del == se->del_2_array.op_del
         && cur->offset == se->del_2_array.offset
         && cur->size   == se->del_2_array.size ) {
            cmd = cur;
            break;
        }
    } RingIterEnd( cur )
    if( cmd == NULL ) {
        cmd = stateTableCmdAllocVar( carveCMD_DEL_2_ARRAY
                                   , &ringCmdsDel2Array
                                   , 1 );
        cmd->op_del = se->del_2_array.op_del;
        cmd->offset = CgOffsetRw( se->del_2_array.offset );
        cmd->size   = se->del_2_array.size;
    }
    return cmd->base.sym;
}


SYMBOL CgCmdTry(                // GET SYMBOL FOR TRY BLOCK
    SE* se )                    // - state entry in state table
{
    TYPE_SIG_ENT* sigs;         // - ring of type signatures
    CMD_TRY* cmd;               // - command

    DbgVerify( UNDEF_AREL != se->try_blk.try_impl->offset_jmpbuf
             , "cgGenerateCmdsTry -- no offset for jmpbuf" );
    DbgVerify( UNDEF_AREL != se->try_blk.try_impl->offset_var
             , "cgGenerateCmdsTry -- no offset for var" );
    sigs = BeTypeSigEntsCopy( se->try_blk.sigs );
    cmd = stateTableCmdAllocVar
                    ( carveCMD_TRY
                    , &ringCmdsTry
                    , 2 + 2 +
                      3 * CgbkInfo.size_offset +
                      RingCount( sigs ) * CgbkInfo.size_data_ptr );
    cmd->state = SeStateVar( FstabPrevious( se ) );
    cmd->offset_var = CgOffsetRw( se->try_blk.try_impl->offset_var );
    cmd->offset_jmpbuf = CgOffsetRw( se->try_blk.try_impl->offset_jmpbuf );
    cmd->sigs = sigs;
    return cmd->base.sym;
}


SYMBOL CgCmdCtorTest(           // GET SYMBOL FOR CTOR-TEST COMMAND
    SE* se )                    // - state entry
{
    CMD_CTOR_TEST* cmd;         // - command

    cmd = stateTableCmdAllocVar( carveCMD_CTOR_TEST
                               , &ringCmdsCtorTest
                               , 1 + CgbkInfo.size_offset );
    cmd->flag_no = se->ctor_test.flag_no;
    return cmd->base.sym;
}


#define cgGenerateCmdCode( code ) DGInteger( code, TY_UINT_1 )


static boolean cgGenerateCmdBase(  // EMIT BASE FOR COMMAND
    CMD_BASE* base,             // - base for command
    uint_8 code )               // - code for command
{
    boolean genning;            // - TRUE ==> genning entry

    if( base->emitted ) {
        genning = FALSE;
    } else {
#ifndef NDEBUG
        if( PragDbgToggle.dump_stab ) {
            printf( "CMD[%x]: ", base->sym );
        }
#endif
        base->emitted = TRUE;
        CgBackGenLabelInternal( base->sym );
#if 1 // this kludge allows old run-time systems to work
        switch( code ) {
          case DTC_ACTUAL_DBASE :
          case DTC_ACTUAL_VBASE :
            code = DTC_COMP_DBASE;
            break;
        }
#endif
        cgGenerateCmdCode( code );
        genning = TRUE;
    }
    return genning;
}


static void cgGenerateCmdsSetSv(// EMIT SET_SV COMMANDS
    void )
{
    CMD_SET_SV* curr;           // - current command

    RingIterBegSafe( ringCmdsSetSv, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_SET_SV ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                printf( "DTC_SET_SV state=%d\n"
                      , curr->state_var );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->state_var );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsTestFlag(// EMIT TEST_FLAG COMMANDS
    void )
{
    CMD_TEST_FLAG* curr;        // - current command

    RingIterBegSafe( ringCmdsTestFlag, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_TEST_FLAG ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                printf( "DTC_TEST_FLAG index=%d true=%d false=%d\n"
                      , curr->index
                      , curr->state_var_true
                      , curr->state_var_false );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->index );
            DgOffset( curr->state_var_true );
            DgOffset( curr->state_var_false );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsTry(  // EMIT TRY COMMANDS
    void )
{
    CMD_TRY* curr;              // - current command

    RingIterBegSafe( ringCmdsTry, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_TRY ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                printf( "DTC_CATCH %x state=%x buf=%x var=%x count="
                      , DTC_TRY
                      , curr->state
                      , curr->offset_jmpbuf
                      , curr->offset_var );
            }
#endif
            cgGenerateCmdCode( DTC_CATCH );
            DgAlignPad( 2 * sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->state );
            DgOffset( curr->offset_jmpbuf );
            DgOffset( curr->offset_var );
            BeGenTypeSigEnts( curr->sigs );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsFnExc(// EMIT FN_EXC CMDS
    void )
{
    CMD_FN_EXC* curr;           // - current command

    RingIterBegSafe( ringCmdsFnExc, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_FN_EXC ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                printf( "DTC_FN_EXC count=" );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            BeGenTypeSigEnts( curr->sigs );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsComponent(// EMIT COMPONENT CMDS
    void )
{
    CMD_COMPONENT* curr;        // - current command

    RingIterBegSafe( ringCmdsComponent, curr ) {
        if( cgGenerateCmdBase( &curr->base, curr->cmd_type ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                const char* code;
                switch( curr->cmd_type ) {
                  case DTC_COMP_VBASE : code = "DTC_COMP_VBASE"; break;
                  case DTC_COMP_DBASE : code = "DTC_COMP_DBASE"; break;
                  case DTC_ACTUAL_VBASE : code = "DTC_ACTUAL_VBASE"; break;
                  case DTC_ACTUAL_DBASE : code = "DTC_ACTUAL_DBASE"; break;
                  case DTC_COMP_MEMB  : code = "DTC_COMP_MEMB "; break;
                  default:              code = "*****BAD******"; break;
                }
                printf( "%s obj-offset=%x var-offset=%x %s\n"
                      , code
                      , curr->obj->offset
                      , curr->offset
                      , DbgSymNameFull( curr->dtor ) );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->obj->offset );
            DgOffset( curr->offset );
            DgPtrSymCode( curr->dtor );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsArrayInit( // EMIT ARRAY-INIT CMDS
    void )
{
    CMD_ARRAY_INIT* curr;       // - current command

    RingIterBegSafe( ringCmdsArrayInit, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_ARRAY_INIT ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                printf( "DTC_ARRAY_INIT offset=%x\n"
                      , curr->reg->offset );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->reg->offset );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsDel1( // EMIT DELETE-1 CMDS
    void )
{
    CMD_DEL_1* curr;            // - current command

    RingIterBegSafe( ringCmdsDel1, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_DLT_1 ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                printf( "DTC_DLT_1 offset=%x %s\n"
                      , curr->offset
                      , DbgSymNameFull( curr->op_del ) );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->offset );
            DgPtrSymCode( curr->op_del );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsDel1Array( // EMIT DELETE-1-ARRAY CMDS
    void )
{
    CMD_DEL_1* curr;            // - current command

    RingIterBegSafe( ringCmdsDel1Array, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_DLT_1_ARRAY ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                printf( "DTC_DLT_1_ARRAY offset=%x %s\n"
                      , curr->offset
                      , DbgSymNameFull( curr->op_del ) );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->offset );
            DgPtrSymCode( curr->op_del );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsDel2( // EMIT DELETE-2 CMDS
    void )
{
    CMD_DEL_2* curr;            // - current command

    RingIterBegSafe( ringCmdsDel2, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_DLT_2 ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                printf( "DTC_DLT_2 offset=%x size=%x %s\n"
                      , curr->offset
                      , curr->size
                      , DbgSymNameFull( curr->op_del ) );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->offset );
            DgPtrSymCode( curr->op_del );
            DgOffset( curr->size );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsDel2Array( // EMIT DELETE-2-ARRAY CMDS
    void )
{
    CMD_DEL_2* curr;            // - current command

    RingIterBegSafe( ringCmdsDel2Array, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_DLT_2_ARRAY ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                printf( "DTC_DLT_2_ARRAY offset=%x size=%x %s\n"
                      , curr->offset
                      , curr->size
                      , DbgSymNameFull( curr->op_del ) );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->offset );
            DgPtrSymCode( curr->op_del );
            DgOffset( curr->size );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsCtorTest( // EMIT CTOR_TEST CMDS
    void )
{
    CMD_CTOR_TEST* curr;        // - current command

    RingIterBegSafe( ringCmdsCtorTest, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_CTOR_TEST ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                printf( "DTC_CTOR_TEST %d\n"
                      , curr->flag_no );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->flag_no );
        }
    } RingIterEndSafe( curr )
}


void CgCmdsGenerate(            // GENERATE DTOR CMD.S
    void )
{
    cgGenerateCmdsSetSv();
    cgGenerateCmdsTestFlag();
    cgGenerateCmdsComponent();
    cgGenerateCmdsTry();
    cgGenerateCmdsFnExc();
    cgGenerateCmdsArrayInit();
    cgGenerateCmdsDel1();
    cgGenerateCmdsDel2();
    cgGenerateCmdsDel1Array();
    cgGenerateCmdsDel2Array();
    cgGenerateCmdsCtorTest();
}


static void cgCmdInit(          // INITIALIZATION FOR CGBKCMDS.C
    INITFINI* def )             // - init/fini definition
{
    def = def;
    ringCmdsSetSv = NULL;
    ringCmdsTestFlag = NULL;
    ringCmdsComponent = NULL;
    ringCmdsTry = NULL;
    ringCmdsFnExc = NULL;
    ringCmdsDel1 = NULL;
    ringCmdsDel2 = NULL;
    ringCmdsDel1Array = NULL;
    ringCmdsDel2Array = NULL;
    ringCmdsArrayInit = NULL;
    ringCmdsCtorTest = NULL;
    carveCMD_TRY         = CarveCreate( sizeof( CMD_TRY ),           4  );
    carveCMD_FN_EXC      = CarveCreate( sizeof( CMD_FN_EXC ),        4  );
    carveCMD_SET_SV      = CarveCreate( sizeof( CMD_SET_SV ),        8  );
    carveCMD_TEST_FLAG   = CarveCreate( sizeof( CMD_TEST_FLAG ),     16 );
    carveCMD_COMPONENT   = CarveCreate( sizeof( CMD_COMPONENT ),     32 );
    carveCMD_ARRAY_INIT  = CarveCreate( sizeof( CMD_ARRAY_INIT ),    8  );
    carveCMD_DEL_1       = CarveCreate( sizeof( CMD_DEL_1 ),         4  );
    carveCMD_DEL_2       = CarveCreate( sizeof( CMD_DEL_2 ),         4  );
    carveCMD_DEL_1_ARRAY = CarveCreate( sizeof( CMD_DEL_1_ARRAY ),   4  );
    carveCMD_DEL_2_ARRAY = CarveCreate( sizeof( CMD_DEL_2_ARRAY ),   4  );
    carveCMD_CTOR_TEST   = CarveCreate( sizeof( CMD_CTOR_TEST ),     1  );
}


static void cgCmdFini(          // COMPLETION FOR CGBKCMDS.C
    INITFINI* def )             // - init/fini definition
{
    def = def;
    CarveDestroy( carveCMD_TRY );
    CarveDestroy( carveCMD_FN_EXC );
    CarveDestroy( carveCMD_SET_SV );
    CarveDestroy( carveCMD_TEST_FLAG );
    CarveDestroy( carveCMD_COMPONENT );
    CarveDestroy( carveCMD_ARRAY_INIT );
    CarveDestroy( carveCMD_DEL_1 );
    CarveDestroy( carveCMD_DEL_2 );
    CarveDestroy( carveCMD_DEL_1_ARRAY );
    CarveDestroy( carveCMD_DEL_2_ARRAY );
    CarveDestroy( carveCMD_CTOR_TEST );
}


INITDEFN( cg_cmds, cgCmdInit, cgCmdFini )
