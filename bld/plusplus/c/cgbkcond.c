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
#include "pstk.h"
#include "initdefs.h"
#include "cgdbg.h"

static PSTK_CTL stack_cond_blks;    // stack: conditional DTOR blocks
static carve_t carveInfo;           // conditional dtor block
static SYMBOL dtor_cond_sym;        // conditional flags, direct DTORing

typedef struct {                // INFO FOR A CONDITION
    unsigned offset;            // - offset of conditional bit
    patch_handle handle_set;    // - patch handle: set
    patch_handle handle_clr;    // - patch handle: clr
    uint_8 mask_set;            // - mask used for setting
    uint_8 mask_clr;            // - mask used for clearing
    unsigned :0;                // - alignment
    SE* posn_last;              // - last significant position
    SE* posn_true;              // - position when flag set
    SE* posn_false;             // - position when flag clr
} COND_STK;

#ifndef NDEBUG
    #include <stdio.h>
    #include "dbg.h"
    #include "toggle.h"
    #include "pragdefn.h"

    static void _Dump( COND_STK* cond, const char* msg )
    {
        if( PragDbgToggle.dump_stab ) {
            printf( "COND_STK[%x]: flag(%d) %s\n"
                    "  last(%x) true(%x) false(%x)\n"
                    "  handle_set(%x) handle_clr(%x) mask_set(%x) mask_clr(%x)\n"
                  , cond
                  , cond->offset
                  , msg
                  , cond->posn_last
                  , cond->posn_true
                  , cond->posn_false
                  , cond->handle_set
                  , cond->handle_clr
                  , cond->mask_set
                  , cond->mask_clr );
        }
    }
#else
    #define _Dump( a, b )
#endif


static SE* callBackCurrent(     // SET UP CURRENT POSITION
    COND_STK* info )            // - top entry
{
    SE* posn;                   // - current position

    posn = FstabCurrPosn();
    info->posn_last = posn;
    return posn;
}


static void callBackTrue(       // CALL-BACK: start of TRUE block
    void* data )                // - COND_STK entry
{
    COND_STK* info = data;      // - COND_STK entry
    SE* posn;                   // - current position

    posn = callBackCurrent( info );
    info->posn_true = posn;
    info->posn_false = posn;
    _Dump( info, "CallBack(TRUE)" );
}


static void callBackFalse(      // CALL-BACK: start of FALSE block
    void* data )                // - COND_STK entry
{
    COND_STK* info = data;      // - COND_STK entry
    SE* posn;                   // - last position

    posn = info->posn_last;
    info->posn_false = callBackCurrent( info );
    FstabSetSvSe( posn );
    _Dump( info, "CallBack(FALSE)" );
}


static void patchMask(          // PATCH A MASK
    patch_handle handle,        // - NULL or handle
    uint_8 mask )               // - mask
{
    if( NULL != handle ) {
        BEPatchInteger( handle, mask );
        BEFiniPatch( handle );
    }
}

static void callBackFini(       // COMPLETE CALL-BACK
    COND_STK* cond )            // - entry to be completed
{
    patchMask( cond->handle_set, cond->mask_set );
    patchMask( cond->handle_clr, cond->mask_clr );
    CarveFree( carveInfo, cond );
}


static void callBackEnd(        // CALL-BACK: end of condition block
    void* data )                // - COND_STK entry
{
    COND_STK* cond = data;      // - COND_STK entry
    SE* posn;                   // - current position

    _Dump( cond, "CallBack(END)" );
    posn = FstabCurrPosn();
#if 0
    if( posn == cond->posn_true
     && posn == cond->posn_false ) {
        cond->mask_set = 0;
        cond->mask_clr = 0xFF;
        BlkPosnTempBegSet( posn );
    } else {
#else
    {
#endif
        SE* test = FstabTestFlag( cond->offset
                                , cond->posn_last
                                , posn );
        FstabAdd( test );
        BlkPosnTempBegSet( test );
    }
    callBackFini( cond );
}


static void callBackNewCtorBeg( // CALL-BACK: start of new ctor
    void* data )                // - COND_STK entry
{
    COND_STK* cond = data;      // - COND_STK entry
    SE* posn;                   // - current position
    SE* se;                     // - new test_flag entry

    posn = callBackCurrent( cond );
    DbgVerify( NULL != posn, "callBackNewCtorBeg -- no delete SE" );
    se = FstabTestFlag( cond->offset, posn, FstabPrevious( posn ) );
    cond->posn_true = se;
    FstabAdd( se );
    BlkPosnTempBegSet( se );
}


static void callBackNewCtorEnd( // CALL-BACK: end of new ctor
    void* data )                // - COND_STK entry
{
    COND_STK* cond = data;      // - COND_STK entry

    _Dump( cond, "CallBack(END-NEW_CTOR)" );
    if( cond->posn_true == FstabActualPosn() ) {
        FstabRemove();
        cond->mask_set = 0;
        cond->mask_clr = 0xFF;
    }
    callBackFini( cond );
};


void CondInfoPush(              // PUSH COND_INFO STACK
    FN_CTL* fctl )              // - function control
{
    COND_STK* stk = CarveAlloc( carveInfo );
    stk->offset = FnCtlCondFlagNext( fctl );
    stk->handle_set = NULL;
    stk->handle_clr = NULL;
    stk->mask_set = 0;
    stk->mask_clr = 0xFF;
    stk->posn_last = 0;
    stk->posn_true = 0;
    stk->posn_false = 0;
    PstkPush( &stack_cond_blks, stk );
    _Dump( stk, "PUSH" );
}


void CondInfoPop(               // POP COND_INFO STACK
    void )
{
    COND_STK* stk = PstkPopElement( &stack_cond_blks );
    stk = stk;
    _Dump( stk, "POP" );
}


void CondInfoSetup(             // SETUP UP CONDITIONAL INFORMATION
    unsigned index,             // - index of flag
    COND_INFO* cond,            // - conditional information
    FN_CTL* fctl )              // - function information
{
    unsigned flag_offset;       // - offset within flags vector

    fctl = fctl;
    flag_offset = index >> 3;
    cond->mask = 0x01 << ( index & 7 );
    cond->sym = FstabRw();
    if( cond->sym == NULL ) {
        cond->sym = dtor_cond_sym;
        cond->offset = flag_offset;
    } else {
        cond->offset = flag_offset + CgbkInfo.size_rw_base;
    }
}


static cg_name condSet(         // SET/RESET FLAG
    unsigned index,             // - index of flag
    boolean set_flag,           // - TRUE ==> set the flag; FALSE ==> clear
    FN_CTL* fctl )              // - function information
{
    cg_name op_flg;             // - expression for flag setting
    cg_name op_mask;            // - mask operand
    COND_INFO cond;             // - conditional information

    CondInfoSetup( index, &cond, fctl );
    op_flg = CgSymbolPlusOffset( cond.sym, cond.offset );
    if( set_flag ) {
        op_mask = CGInteger( cond.mask, T_UINT_1 );
        op_flg = CGLVPreGets( O_OR, op_flg, op_mask, T_UINT_1 );
    } else {
        op_mask = CGInteger( 0xFF - cond.mask, T_UINT_1 );
        op_flg = CGLVPreGets( O_AND, op_flg, op_mask, T_UINT_1 );
    }
    return op_flg;
}


void CondInfoSetFlag(           // SET FLAG FOR CONDITIONAL DTOR BLOCK
    FN_CTL* fctl,               // - function control
    boolean set_flag )          // - TRUE ==> set the flag; FALSE ==> clear
{
    COND_STK* stk;              // - conditional entry
    cg_name op_flg;             // - expression for flag setting
    cg_name op_mask;            // - mask operand
    COND_INFO cond;             // - conditional information
    patch_handle patch;         // - handle for patch
    unsigned opcode;            // - opcode for set/clr

    stk = PstkTopElement( &stack_cond_blks );
    CondInfoSetup( stk->offset, &cond, fctl );
    patch = BEPatch();
    op_mask = CGPatchNode( patch, T_UINT_1 );
    if( set_flag ) {
        stk->mask_set = cond.mask;
        stk->handle_set = patch;
        opcode = O_OR;
    } else {
        stk->mask_clr = 0xFF - cond.mask;
        stk->handle_clr = patch;
        opcode = O_AND;
    }
    op_flg = CgSymbolPlusOffset( cond.sym, cond.offset );
    op_flg = CGLVPreGets( opcode, op_flg, op_mask, T_UINT_1 );
    CgExprPush( op_flg, T_POINTER );
}


void CondInfoSetCtorTest(       // SET/RESET FLAG FOR CTOR-TEST
    FN_CTL* fctl,               // - function control
    boolean set_flag )          // - TRUE ==> set the flag; FALSE ==> clear
{
    CGDone( condSet( FnCtlCondFlagCtor( fctl ), set_flag, fctl ) );
}


void CondInfoDirectFlags(       // SET FOR DIRECT-FLAGS PROCESSING
    unsigned flag_bytes )       // - # bytes of flags required
{
    if( flag_bytes > 0 ) {
        dtor_cond_sym = CgVarTemp( flag_bytes );
    } else {
        dtor_cond_sym = NULL;
    }
}


static void condInfoCallBack(   // SET A CALL-BACK
    void (*rtn)( void* ),       // - call-back routine
    boolean on_left )           // - TRUE ==> call-back on left
{
    cg_name expr;               // - top expression
    cg_type type;               // - top type
    COND_STK* stk;              // - stack ptr

    stk = PstkTopElement( &stack_cond_blks );
    expr = CgExprPopType( &type );
    if( on_left ) {
        expr = CgCallBackLeft( expr, rtn, stk, type );
    } else {
        expr = CgCallBackRight( expr, rtn, stk, type );
    }
    CgExprPush( expr, type );
}


void CondInfoTrue(              // SET UP CALL-BACK FOR IC_COND_TRUE
    void )
{
    condInfoCallBack( &callBackTrue, TRUE );
}


void CondInfoFalse(             // SET UP CALL-BACK FOR IC_COND_FALSE
    void )
{
    condInfoCallBack( &callBackFalse, TRUE );
}


void CondInfoEnd(               // SET UP CALL-BACK FOR IC_COND_END
    void )
{
    condInfoCallBack( &callBackEnd, FALSE );
}


void CondInfoNewCtorBeg(        // CTOR OF NEW'ED OBJECT: START
    FN_CTL* fctl )              // - function information
{
    CondInfoPush( fctl );
    CondInfoSetFlag( fctl, TRUE );
    condInfoCallBack( &callBackNewCtorBeg, TRUE );
}


void CondInfoNewCtorEnd(        // CTOR OF NEW'ED OBJECT: END
    FN_CTL* fctl )              // - function information
{
    CondInfoSetFlag( fctl, FALSE );
    condInfoCallBack( &callBackNewCtorEnd, FALSE );
    CondInfoPop();
}


// MODULE INITIALIZATION


static void init(               // CGBKCOND INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    PstkOpen( &stack_cond_blks );
    carveInfo = CarveCreate( sizeof( COND_STK ), 32 );
    CGDBG_CallBackName( callBackTrue );
    CGDBG_CallBackName( callBackFalse );
    CGDBG_CallBackName( callBackEnd );
    CGDBG_CallBackName( callBackNewCtorBeg );
    CGDBG_CallBackName( callBackNewCtorEnd );
}


static void fini(               // CGBKCOND COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
    PstkClose( &stack_cond_blks );
    CarveDestroy( carveInfo );
}

INITDEFN( conditional_blocks, init, fini );
