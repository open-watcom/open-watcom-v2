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


#include <float.h>
#include <stdlib.h>

#include "plusplus.h"
#include "cgfront.h"
#include "cgback.h"
#include "memmgr.h"
#include "errdefns.h"
#include "codegen.h"
#include "cgbackut.h"
#include "ring.h"
#include "initdefs.h"
#include "dbg.h"
#include "rtfuncod.h"

static STAB_CTL fstab;          // function state table instance
static STAB_DEFN fStabDefn;     // function state table definition
static target_offset_t rw_offset_fun; // offset of R/W var. in function

#ifndef NDEBUG
    #include <stdio.h>
    #include "toggle.h"
    #include "pragdefn.h"


void FstabDump()                // DEBUG ONLY: DUMP FUNCTION STATE TABLE
{
    DbgDumpStateTable( &fstab );
}

void DbgSetState(               // DEBUG ONLY: PRINT STATE VALUE SET, IF REQ'D
    const char* msg,            // - message text
    void* se_ent )              // - state entry
{

    if( PragDbgToggle.dump_stab ) {
        SE* se = (SE*)se_ent;   // - state entry
        printf( "***** set state %d [%s]\n"
              , SeStateVar( se )
              , msg );
    }
}

#endif


target_offset_t CgOffsetRw(     // COMPUTE OFFSET FROM R/W REGISTRATION
    target_offset_t offset )    // - offset within stack
{
    return offset - rw_offset_fun;
}


SE* FstabCtorTest(              // ALLOCATE CTOR-TEST COMMAND
    FN_CTL* fctl )              // - function being emitted
{
    SE* se = SeAlloc( DTC_CTOR_TEST );
    se->ctor_test.flag_no = FnCtlCondFlagCtor( fctl );
    return FstabAdd( se );
}


SE* FstabPrevious(              // GET PREVIOUS STATE ENTRY
    SE* se )                    // - starting entry
{
    return StabCtlPrevious( &fstab, se );
}


SE* FstabPrecedes(              // GET PRECEDING STATE ENTRY
    SE* se )                    // - starting entry
{
    return StabCtlPrecedes( &fstab, se );
}


SE* FstabActualPosn(            // GET ACTUAL POSITION IN STATE TABLE
    void )
{
    return StateTableActualPosn( &fstab );
}


SE* FstabAdd(                   // ADD STATE ENTRY TO STATE TABLE
    SE* se )                    // - state entry
{
    return StateTableAdd( se, &fstab );
}


boolean FstabHasStateTable(     // DETERMINE IF STATE TABLE BEING GEN'ED
    void )
{
    return fStabDefn.ro != NULL;
}


SE* FstabPosnGened(             // GET GENNED POSITION IF REQUIRED
    SE* src,                    // - source entry
    SE* tgt )                   // - target entry
{
    return StabCtlPosnGened( &fstab, src, tgt );
}


SE* FstabSetSvSe(               // ADD SET_SV FOR SE TO STATE TABLE
    SE* tgt )                   // - state entry
{
    tgt = SeSetSvPosition( tgt );
    if( ( fStabDefn.state_table != NULL )
      &&( tgt != FstabCurrPosn() ) ) {
        SE* se = SeAlloc( DTC_SET_SV );
        tgt = FstabPosnGened( se, tgt );
        DbgVerify( NULL == tgt || ! se->base.gen || tgt->base.gen
                 , "FstabSetSvSe -- base.gen mismatch" );
        se->set_sv.se = tgt;
        FstabAdd( se );
    }
    return tgt;
}


void FstabInit(                 // INITIALIZE FUNCTION STATE TABLE
    void )
{
    StabCtlInit( &fstab, &fStabDefn );
    StabDefnInit( &fStabDefn, DTRG_FUN );
}


boolean FstabSetup(             // SETUP FUNCTION STATE TABLE
    CGFILE* file_ctl,           // - current file information
    FN_CTL* fctl )              // - current file generation information
{
    boolean retn;               // - TRUE ==> state table will be genned
    unsigned flag_bytes;        // - number of flag bytes

    if( 0 == CgNonThunkDepth( fctl ) && ! SymIsThunk( fctl->func ) ) {
        fstab.marked_posn = NULL;
        flag_bytes = ( file_ctl->cond_flags + 7 ) / 8;
        if( file_ctl->state_table && file_ctl->stab_gen ) {
#ifndef NDEBUG
            if( PragDbgToggle.dump_stab ) {
                printf( "State Table for Function: %x\n"
                      , &fStabDefn.state_table );
            }
#endif
//          if( DtmTabular( fctl ) ) {
                fStabDefn.ro = CgVarRo( 1, SC_STATIC, NULL );
                fstab.rw = CgVarRw( flag_bytes + CgbkInfo.size_rw_base
                                  , SC_AUTO );
                AutoRelRegister( fstab.rw, &rw_offset_fun );
                if( ! fctl->is_dtor ) {
                    CgFunRegister( fctl, fstab.rw, fStabDefn.ro );
                }
                retn = TRUE;
//          } else {
//              CondInfoDirectFlags( flag_bytes );
//              fStabDefn.ro = NULL;
//              retn = FALSE;
//          }
        } else {
            CondInfoDirectFlags( flag_bytes );
            fStabDefn.ro = NULL;
            retn = FALSE;
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


static unsigned offsetStateVar( // GET OFFSET OF STATE VAR. IN R/W BLOCK
    void )
{
#if _CPU == _AXP
    return CgbkInfo.size_data_ptr;
#else
    return 2 * CgbkInfo.size_data_ptr + CgbkInfo.size_fs_hand;
#endif
}


static cg_name assignStateVar(  // EMIT CODE TO ASSIGN STATE VARIABLE
    SE* se )                    // - NULL or state entry to be set
{
    DbgSetState( "direct", se );
    return CgAssignStateVar( fstab.rw, se, offsetStateVar() );
}


void FstabAssignStateVar(       // EMIT EXPRESSION TO ASSIGN STATE VARIABLE
    SE* se )                    // - NULL or state entry to be set
{
    FstabMarkedPosnSet( se );
    if( NULL != fstab.rw ) {
        CGDone( assignStateVar( se ) );
    }
}


static boolean emitSv(          // TEST IF SV TO BE UPDATED
    FN_CTL* fctl )              // - function being emitted
{
    return BlkPosnUseStab() && DtmTabular( fctl );
}


cg_name FstabEmitStateVar(      // EMIT CODE TO SET STATE VARIABLE, IF REQ'D
    SE* se,                     // - NULL or state entry to be set
    FN_CTL* fctl )              // - function being emitted
{
    cg_name expr;               // - emitted expression

    se = SeSetSvPosition( se );
    FstabMarkedPosnSet( se );
    if( emitSv( fctl ) ) {
        expr = assignStateVar( se );
    } else {
        expr = NULL;
    }
    return expr;
}


cg_name FstabEmitStateVarPatch( // EMIT CODE TO PATCH STATE VARIABLE, IF REQ'D
    patch_handle* a_handle,     // - addr[ handle ]
    FN_CTL* fctl )              // - function being emitted
{
    cg_name expr;               // - emitted expression

    if( emitSv( fctl ) ) {
        patch_handle handle = BEPatch();
        cg_type type = CgTypeOffset();
        *a_handle = handle;
        expr = CGPatchNode( handle, type );
        expr = CGLVAssign( CgSymbolPlusOffset( fstab.rw, offsetStateVar() )
                         , expr
                         , type );
    } else {
        expr = NULL;
    }
    return expr;
}


void FstabEmitStateVarExpr(     // EMIT EXPR'N TO SET STATE VARIABLE, IF REQ'D
    SE* se,                     // - NULL or state entry to be set
    FN_CTL* fctl )              // - function being emitted
{
    cg_name expr;               // - expression

    expr = FstabEmitStateVar( se, fctl );
    if( expr != NULL ) {
        CGDone( expr );
    }
}


SE* FstabFindAuto(              // FIND AUTO VAR ENTRY IN STATE TABLE
    SYMBOL auto_var )           // - the auto variable
{
    SE* se_auto;                // - SE for auto
    SE* se_curr;                // - current SE

    se_auto = NULL;
    RingIterBeg( fStabDefn.state_table, se_curr ) {
        if( se_curr->base.se_type == DTC_SYM_AUTO
         && se_curr->sym_auto.sym == auto_var ) {
            se_auto = se_curr;
            break;
        }

    } RingIterEnd( se_curr );
    return se_auto;
}


void FstabSetDtorState(         // SET STATE VAR. FOR DTOR
    SE* se,                     // - state entry being DTOR'ed
    FN_CTL* fctl )              // - file information
{
    SE* prev = FstabPrevious( se );
    FstabEmitStateVarExpr( prev, fctl );
    FstabSetSvSe( prev );
}


boolean FstabGenerate(          // GENERATE FUNCTION STATE TABLE
    void )
{
    return StabGenerate( &fstab );
}


#if 0
void FstabFree(                 // FREE FUNCTION STATE TABLE
    void )
{
    StabCtlFreeStateTable( &fstab );
}
#endif


SE* FstabCurrPosn(              // GET CURRENT STATE ENTRY FOR FUNCTION
    void )
{
    return StateTableCurrPosn( &fstab );
}


void FstabRemove(               // REMOVE LAST STATE ENTRY
    void )
{
    StabCtlRemove( &fstab );
}


SYMBOL FstabRw(                 // GET R/W SYMBOL FOR FUNCTION STATE TABLE
    void )
{
    return fstab.rw;
}

void FstabRegister(             // REGISTER FUNCTION
    FN_CTL* fctl )              // - function control
{
    if( fStabDefn.ro != NULL ) {
        if( 0 == CgBackInlinedDepth() ) {
            CgFunRegister( fctl, fstab.rw, fStabDefn.ro );
        }
#if 0
        if( fctl->is_dtor ) {
            SE* se = BlkPosnCurr();
            FstabAssignStateVar( se );
        }
#endif
    }
}

void FstabDeRegister(           // DE-REGISTER FUNCTION
    FN_CTL* fctl )              // - function control
{
#if _CPU == _AXP
    fctl->deregistered = TRUE;
#else
    if( ! fctl->deregistered ) {
        if( 0 == CgBackInlinedDepth() && fStabDefn.ro != NULL ) {
            CgFunDeregister( fstab.rw );
        }
        fctl->deregistered = TRUE;
    }
#endif
}


SE* FstabMarkedPosn(            // GET MARKED POSITION
    void )
{
    return fstab.marked_posn;
}


SE* FstabMarkedPosnSet(         // SET MARKED POSITION
    SE* se )                    // - new position
{
#ifndef NDEBUG
    if( PragDbgToggle.dump_stab ) {
        printf( "--- update marked position = %x\n", se );
    }
#endif
    fstab.marked_posn = se;
    return se;
}


void FstabPrune(                // PRUNE END OF STATE TABLE
    SE* end )                   // - actual ending position
{
    StabCtlPrune( end, &fstab );
}


SE* FstabTestFlag(              // CREATE TEST-FLAG ENTRY
    unsigned flag_no,           // - flag #
    SE* se_true,                // - entry when true
    SE* se_false )              // - entry when false
{
    SE* se;                     // - new entry

    se = SeAlloc( DTC_TEST_FLAG );
    se->test_flag.index = flag_no;
    se->test_flag.se_true = FstabPosnGened( se, se_true );
    se->test_flag.se_false = FstabPosnGened( se, se_false );
    DbgVerify( NULL == se->test_flag.se_false
               || ! se->base.gen
               || se->test_flag.se_false->base.gen
             , "FstabTestFlag: DTC_TEST_FLAG (false) base.gen mismatch" );
    DbgVerify( NULL == se->test_flag.se_true
               || ! se->base.gen
               || se->test_flag.se_true->base.gen
             , "FstabTestFlag: DTC_TEST_FLAG (true) base.gen mismatch" );
    return se;
}


#if _CPU == _AXP
// SPECIFIC CODE FOR SYSTEMS USING PROCEDURE DESCRIPTORS (ALPHA)


SYMBOL FstabExcHandler(         // ALPHA: SET EXCEPTION HANDLER
    void )
{
    SYMBOL retn;                // - exception handler

    if( fStabDefn.ro == NULL ) {
        retn = NULL;
    } else {
        retn = RunTimeCallSymbol( RTF_PD_HANDLER );
    }
    return retn;
}


SYMBOL FstabExcData(            // ALPHA: SET EXCEPTION DATA
    void )
{
    return fStabDefn.ro;
}


SYMBOL FstabExcRw(              // ALPHA: GET R/W DATA SYMBOL
    void )
{
    return fstab.rw;
}

#endif
