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
    #include "dbg.h"
    #include "toggle.h"
    #include "pragdefn.h"
#endif

enum                                    // INDICES FOR STATE-TABLE COMMANDS
{   DTOR_CMD_INDEX_VBASE                // - virtual base
,   DTOR_CMD_INDEX_DBASE                // - direct base
,   DTOR_CMD_INDEX_SETSV                // - set sv (must be last)
};

#define ONLY_DTC_CMDS

#define pick(e,se,cmd,ring) static carve_t carveCMD_ ## e; // allocations for CMD_ ## e
#include "_dtccarv.h"
#undef pick

#define pick(e,se,cmd,ring) static CMD_ ## e *ringCmds ## ring; // commands: e
#include "_dtccarv.h"
#undef pick

// Following definitions and routine must correspond with appropriate items
// declaration in C++ run-time library
// see the AlignPad... macros in RTEXCEPT.H and CPPLIB.H
//
#if ( _CPU == 8086 )
    #define DG_ALIGN 2
#else
    #define DG_ALIGN 4
#endif

#define CMD_SIZE DG_ALIGN

static void DgAlignPad(         // INSERT PADDING IN A STRUCTURE
    unsigned total )            // - number of bytes emitted so far
{
    unsigned left;

    left = ( ( total + DG_ALIGN - 1 ) & ( - DG_ALIGN ) ) - total;
    if( left > 0 ) {
        DgUninitBytes( left );
    }
}

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


static SYMBOL stateTableCmdName(    // CREATE COMDEF VARIABLE FOR DTOR_CMD
    unsigned index )                // - command index
{
    return CgVarRo( CMD_SIZE + CgbkInfo.size_offset, SC_PUBLIC, CppNameStateTableCmd( index ) );
}


SYMBOL CgCmdFnExc(              // GET SYMBOL FOR FN-EXCEPTION SPEC. COMMAND
    SE* se )                    // - state entry in state table
{
    TYPE_SIG_ENT* sigs;         // - ring of signatures
    CMD_FN_EXC* cmd;            // - command

    sigs = BeTypeSigEntsCopy( se->fn_exc.sigs );
    cmd = stateTableCmdAllocVar( carveCMD_FN_EXC, &ringCmdsFnExc,
                    CMD_SIZE + CgbkInfo.size_offset + RingCount( sigs ) * CgbkInfo.size_data_ptr );
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
        cmd = stateTableCmdAllocVar( carveCMD_TEST_FLAG, &ringCmdsTestFlag, CMD_SIZE + 3 * CgbkInfo.size_offset );
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

    DbgVerify( UNDEF_AREL != se->component.obj->offset, "CgCmdComponent -- no offset" );
    cmd = stateTableCmdAllocVar( carveCMD_COMPONENT, &ringCmdsComponent,
                    CMD_SIZE + 2 * CgbkInfo.size_offset + CgbkInfo.size_code_ptr );
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

    DbgVerify( UNDEF_AREL != se->array_init.reg->offset, "cgAddCmdArrayInit -- no offset" );
    cmd = stateTableCmdAllocVar( carveCMD_ARRAY_INIT, &ringCmdsArrayInit, CMD_SIZE + CgbkInfo.size_offset );
    cmd->reg = se->array_init.reg;
    return cmd->base.sym;
}


SYMBOL CgCmdDlt1(               // GET SYMBOL FOR DTC_DLT_1
    SE* se )                    // - state entry in state table
{
    CMD_DLT_1* cmd;             // - command for operation
    CMD_DLT_1* cur;             // - current command

    DbgVerify( UNDEF_AREL != se->dlt_1.offset
             , "CgCmdDlt1 -- no offset for DLT_1" );
    cmd = NULL;
    RingIterBeg( ringCmdsDlt1, cur ) {
        if( cur->op_del == se->dlt_1.op_del && cur->offset == se->dlt_1.offset ) {
            cmd = cur;
            break;
        }
    } RingIterEnd( cur )
    if( cmd == NULL ) {
        cmd = stateTableCmdAllocVar( carveCMD_DLT_1, &ringCmdsDlt1,
                        CMD_SIZE + CgbkInfo.size_offset + CgbkInfo.size_code_ptr );
        cmd->op_del = se->dlt_1.op_del;
        cmd->offset = CgOffsetRw( se->dlt_1.offset );
    }
    return cmd->base.sym;
}


SYMBOL CgCmdDlt1Array(          // GET SYMBOL FOR DTC_DLT_1
    SE* se )                    // - state entry in state table
{
    CMD_DLT_1_ARRAY* cmd;       // - command for operation
    CMD_DLT_1_ARRAY* cur;       // - current command

    DbgVerify( UNDEF_AREL != se->dlt_1_array.offset
             , "CgCmdDlt1 -- no offset for DLT_1" );
    cmd = NULL;
    RingIterBeg( ringCmdsDlt1Array, cur ) {
        if( cur->op_del == se->dlt_1_array.op_del && cur->offset == se->dlt_1_array.offset ) {
            cmd = cur;
            break;
        }
    } RingIterEnd( cur )
    if( cmd == NULL ) {
        cmd = stateTableCmdAllocVar( carveCMD_DLT_1_ARRAY, &ringCmdsDlt1Array,
                        CMD_SIZE + CgbkInfo.size_offset + CgbkInfo.size_code_ptr );
        cmd->op_del = se->dlt_1_array.op_del;
        cmd->offset = CgOffsetRw( se->dlt_1_array.offset );
    }
    return cmd->base.sym;
}


SYMBOL CgCmdDlt2(               // GET SYMBOL FOR DTC_DLT_2
    SE* se )                    // - state entry in state table
{
    CMD_DLT_2* cmd;             // - command for operation
    CMD_DLT_2* cur;             // - current command

    DbgVerify( UNDEF_AREL != se->dlt_2.offset
             , "CgCmdDlt2 -- no offset for DLT_2" );
    cmd = NULL;
    RingIterBeg( ringCmdsDlt2, cur ) {
        if( cur->op_del == se->dlt_2.op_del
         && cur->offset == se->dlt_2.offset
         && cur->size   == se->dlt_2.size ) {
            cmd = cur;
            break;
        }
    } RingIterEnd( cur )
    if( cmd == NULL ) {
        cmd = stateTableCmdAllocVar( carveCMD_DLT_2, &ringCmdsDlt2,
                        CMD_SIZE + 2 * CgbkInfo.size_offset + CgbkInfo.size_code_ptr );
        cmd->op_del = se->dlt_2.op_del;
        cmd->offset = CgOffsetRw( se->dlt_2.offset );
        cmd->size   = se->dlt_2.size;
    }
    return cmd->base.sym;
}


SYMBOL CgCmdDlt2Array(          // GET SYMBOL FOR DTC_DLT_2
    SE* se )                    // - state entry in state table
{
    CMD_DLT_2_ARRAY* cmd;       // - command for operation
    CMD_DLT_2_ARRAY* cur;       // - current command

    DbgVerify( UNDEF_AREL != se->dlt_2_array.offset
             , "CgCmdDlt2 -- no offset for DLT_2" );
    cmd = NULL;
    RingIterBeg( ringCmdsDlt2Array, cur ) {
        if( cur->op_del == se->dlt_2_array.op_del
         && cur->offset == se->dlt_2_array.offset
         && cur->size   == se->dlt_2_array.size ) {
            cmd = cur;
            break;
        }
    } RingIterEnd( cur )
    if( cmd == NULL ) {
        cmd = stateTableCmdAllocVar( carveCMD_DLT_2_ARRAY, &ringCmdsDlt2Array,
                        CMD_SIZE + 2 * CgbkInfo.size_offset + CgbkInfo.size_code_ptr );
        cmd->op_del = se->dlt_2_array.op_del;
        cmd->offset = CgOffsetRw( se->dlt_2_array.offset );
        cmd->size   = se->dlt_2_array.size;
    }
    return cmd->base.sym;
}


SYMBOL CgCmdTry(                // GET SYMBOL FOR TRY BLOCK
    SE* se )                    // - state entry in state table
{
    TYPE_SIG_ENT* sigs;         // - ring of type signatures
    CMD_TRY* cmd;               // - command

    DbgVerify( UNDEF_AREL != se->try_blk.try_impl->offset_jmpbuf, "cgGenerateCmdsTry -- no offset for jmpbuf" );
    DbgVerify( UNDEF_AREL != se->try_blk.try_impl->offset_var, "cgGenerateCmdsTry -- no offset for var" );
    sigs = BeTypeSigEntsCopy( se->try_blk.sigs );
    cmd = stateTableCmdAllocVar( carveCMD_TRY, &ringCmdsTry,
                    CMD_SIZE + 4 * CgbkInfo.size_offset + RingCount( sigs ) * CgbkInfo.size_data_ptr );
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

    cmd = stateTableCmdAllocVar( carveCMD_CTOR_TEST, &ringCmdsCtorTest, CMD_SIZE + CgbkInfo.size_offset );
    cmd->flag_no = se->ctor_test.flag_no;
    return cmd->base.sym;
}


#define cgGenerateCmdCode( code ) DGInteger( code, TY_UINT_1 )


static bool cgGenerateCmdBase(      // EMIT BASE FOR COMMAND
    CMD_BASE* base,                 // - base for command
    DTC_KIND code )                 // - code for command
{
    bool genning;                   // - TRUE ==> genning entry

    if( base->emitted ) {
        genning = FALSE;
    } else {
#ifndef NDEBUG
        if( PragDbgToggle.dump_stab ) {
            printf( "CMD[%p]: ", base->sym );
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
                printf( "DTC_SET_SV state=%d\n", curr->state_var );
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
                VBUF vbuf;
                switch( curr->cmd_type ) {
                  case DTC_COMP_VBASE :   code = "DTC_COMP_VBASE"; break;
                  case DTC_COMP_DBASE :   code = "DTC_COMP_DBASE"; break;
                  case DTC_ACTUAL_VBASE : code = "DTC_ACTUAL_VBASE"; break;
                  case DTC_ACTUAL_DBASE : code = "DTC_ACTUAL_DBASE"; break;
                  case DTC_COMP_MEMB  :   code = "DTC_COMP_MEMB "; break;
                  default:                code = "*****BAD******"; break;
                }
                printf( "%s obj-offset=%x var-offset=%x %s\n"
                      , code
                      , curr->obj->offset
                      , curr->offset
                      , DbgSymNameFull( curr->dtor, &vbuf ) );
                VbufFree( &vbuf );
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
                printf( "DTC_ARRAY_INIT offset=%x\n", curr->reg->offset );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->reg->offset );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsDlt1( // EMIT DELETE-1 CMDS
    void )
{
    CMD_DLT_1* curr;            // - current command

    RingIterBegSafe( ringCmdsDlt1, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_DLT_1 ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                VBUF vbuf;
                printf( "DTC_DLT_1 offset=%x %s\n"
                      , curr->offset
                      , DbgSymNameFull( curr->op_del, &vbuf ) );
                VbufFree( &vbuf );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->offset );
            DgPtrSymCode( curr->op_del );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsDlt1Array( // EMIT DELETE-1-ARRAY CMDS
    void )
{
    CMD_DLT_1* curr;            // - current command

    RingIterBegSafe( ringCmdsDlt1Array, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_DLT_1_ARRAY ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                VBUF vbuf;
                printf( "DTC_DLT_1_ARRAY offset=%x %s\n"
                      , curr->offset
                      , DbgSymNameFull( curr->op_del, &vbuf ) );
                VbufFree( &vbuf );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->offset );
            DgPtrSymCode( curr->op_del );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsDlt2( // EMIT DELETE-2 CMDS
    void )
{
    CMD_DLT_2* curr;            // - current command

    RingIterBegSafe( ringCmdsDlt2, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_DLT_2 ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                VBUF vbuf;
                printf( "DTC_DLT_2 offset=%x size=%x %s\n"
                      , curr->offset
                      , curr->size
                      , DbgSymNameFull( curr->op_del, &vbuf ) );
                VbufFree( &vbuf );
            }
#endif
            DgAlignPad( sizeof( DTOR_CMD_CODE ) );
            DgOffset( curr->offset );
            DgPtrSymCode( curr->op_del );
            DgOffset( curr->size );
        }
    } RingIterEndSafe( curr )
}


static void cgGenerateCmdsDlt2Array( // EMIT DELETE-2-ARRAY CMDS
    void )
{
    CMD_DLT_2* curr;            // - current command

    RingIterBegSafe( ringCmdsDlt2Array, curr ) {
        if( cgGenerateCmdBase( &curr->base, DTC_DLT_2_ARRAY ) ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                VBUF vbuf;
                printf( "DTC_DLT_2_ARRAY offset=%x size=%x %s\n"
                      , curr->offset
                      , curr->size
                      , DbgSymNameFull( curr->op_del, &vbuf ) );
                VbufFree( &vbuf );
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
                printf( "DTC_CTOR_TEST %d\n", curr->flag_no );
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
    #define pick(e,se,cmd,ring) cgGenerateCmds ## ring();
    #include "_dtccarv.h"
    #undef pick
}


static void cgCmdInit(          // INITIALIZATION FOR CGBKCMDS.C
    INITFINI* def )             // - init/fini definition
{
    def = def;

    #define pick(e,se,cmd,ring) ringCmds ## ring = NULL;
    #include "_dtccarv.h"
    #undef pick

    #define pick(e,se,cmd,ring) carveCMD_ ## e = CarveCreate( sizeof( CMD_ ## e ), cmd );
    #include "_dtccarv.h"
    #undef pick
}


static void cgCmdFini(          // COMPLETION FOR CGBKCMDS.C
    INITFINI* def )             // - init/fini definition
{
    def = def;

    #define pick(e,se,cmd,ring) CarveDestroy( carveCMD_ ## e );
    #include "_dtccarv.h"
    #undef pick
}

#undef ONLY_DTC_CMDS

INITDEFN( cg_cmds, cgCmdInit, cgCmdFini )
