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
#include "cgfront.h"
#include "cgback.h"
#include "codegen.h"
#include "cgbackut.h"
#include "carve.h"
#include "pstk.h"
#include "initdefs.h"
#include "errdefns.h"
#include "dbg.h"


typedef struct                  // patch_entry -- patch SE state variable
{   SE* se;                     // - state entry
    patch_handle patch;         // - CG patching handle
    target_offset_t flag_no;    // - flag # for patching
} patch_entry;

typedef struct                  // temp_entry -- entry for a temporary
{   SE* se;                     // - state entry for temporary
    SE* start;                  // - state entry at start of ctor
    patch_handle patch;         // - CG patching handle
} temp_entry;


static carve_t carve_patch_se;      // carver: SE patch entries
static carve_t carve_ctor_flag;     // carver: ctor flag patch entries
static carve_t carve_temp_entry;    // carver: temp_entry's
static PSTK_CTL stack_new_ctors;    // stack: newed ctoring

#ifndef NDEBUG

    #include <stdio.h>
    #include "toggle.h"
    #include "pragdefn.h"

    static void _peDump( patch_entry* pe, const char* msg ) {
        if( PragDbgToggle.dump_stab ) {
            printf( "%s[%x] se[%x] handle[%d]\n"
                  , msg
                  , pe
                  , pe->se
                  , pe->patch );
        }
    }

#else

    #define _peDump( a, b )

#endif


static void init(               // module initialization
    INITFINI* defn )
{
    defn = defn;
    carve_patch_se = CarveCreate( sizeof( patch_entry ), 16 );
    carve_ctor_flag = CarveCreate( sizeof( CTOR_FLAG_SET ), 16 );
    carve_temp_entry = CarveCreate( sizeof( temp_entry ), 16 );
    PstkOpen( &stack_new_ctors );
}


static void fini(               // module completion
    INITFINI* defn )
{
    defn = defn;
    CarveDestroy( carve_patch_se );
    CarveDestroy( carve_ctor_flag );
    CarveDestroy( carve_temp_entry );
    PstkClose( &stack_new_ctors );
}


INITDEFN( cg_call_back, init, fini );


static boolean ctorTestReqd(    // TEST IF CTOR-TEST REQUIRED
    SE* top,                    // - top entry
    SE* object )                // - object being ctored
{
    return top->base.se_type != DTC_CTOR_TEST
        || object != top->base.prev;
}


static cg_name emitPatch(       // EMIT A PATCH EXPRESSION
    patch_handle* a_handle )    // - addr[ patch handle ]
{
    return FstabEmitStateVarPatch( a_handle, FnCtlTop() );
}


cg_name CgCallBackLeft(         // MAKE A LEFT CALL-BACK
    cg_name expr,               // - expression
    void (*fun)( void* ),       // - call-back function
    void* data,                 // - data for call back
    cg_type type )              // - type of expression
{
    return CgComma( CGCallback( fun, data ), expr, type );
}


cg_name CgCallBackRight(        // MAKE A RIGHT CALL-BACK
    cg_name expr,               // - expression
    void (*fun)( void* ),       // - call-back function
    void* data,                 // - data for call back
    cg_type type )              // - type of expression
{
    return CgSideEffect( expr, CGCallback( fun, data ), type );
}


static cg_name ctorFlagSet(     // SET/RESET CTOR FLAG
    FN_CTL* fctl,               // - function info
    unsigned opcode,            // - set/reset opcode
    patch_handle* a_ph )        // - addr[ patch_handle ]
{
    unsigned offset;            // - offset of flag byte
    unsigned mask;              // - mask for byte
    cg_name op_flg;             // - expression for code-gen

    mask = FnCtlCondFlagCtor( fctl );
    offset = mask / 8;
    mask &= 7;
    op_flg = CgSymbolPlusOffset( FstabRw(), offset + CgbkInfo.size_rw_base );
    *a_ph = BEPatch();
    op_flg = CGLVPreGets( opcode
                        , op_flg
                        , CGPatchNode( *a_ph, T_UINT_1 )
                        , T_UINT_1 );
    return op_flg;
}


static void callBackCtorFlag(   // CALL-BACK FOR CTOR-FLAG AFTER CTORING
    void* data )                // - patch entry
{
    CTOR_FLAG_SET* cfs = data;  // - patch entry

    if( ctorTestReqd( FstabActualPosn(), cfs->se ) ) {
        FN_CTL* fctl = FnCtlTop();
        unsigned mask = 1 << ( FnCtlCondFlagCtor( fctl ) & 7 );
        BEPatchInteger( cfs->ph_clr, 255 - mask );
    } else {
        BEPatchInteger( cfs->ph_clr, -1 );
    }
    BEFiniPatch( cfs->ph_clr );
    CarveFree( carve_ctor_flag, cfs );
}

static cg_name genCtorFlagClr(  // CLEAR CTOR FLAGGING
    cg_name expr,               // - current expression
    cg_type type,               // - expression type
    SE* se )                    // - state entry for ctor'ed object
{
    FN_CTL* fctl = FnCtlTop();  // - function information
    CTOR_FLAG_SET* cfs;         // - call-back data

    if( DtmTabular( fctl ) ) {
        cfs = CarveAlloc( carve_ctor_flag );
        cfs->se = se;
        expr = CgComma( ctorFlagSet( fctl, O_AND, &cfs->ph_clr )
                      , expr
                      , type );
        expr = CgCallBackRight( expr, &callBackCtorFlag, cfs, type );
    }
    return expr;
}


static void setSeCtorTest(      // CALL BACK: SET SE AND CTOR-TEST
    void* data )                // - state entry
{
    SE* se = (SE*)data;

    se = BlkPosnUpdate( se );
    se = BlkPosnTempEndSet( se );
    FstabAdd( se );
    FstabCtorTest( FnCtlTop() );
}


cg_name CgCallBackCtorStart(    // SET A CALL BACK FOR A CTOR-TEST : START
    cg_name expr,               // - expression
    cg_type type,               // - type of expression
    SE* se )                    // - state entry to be inserted on call back
{
    expr = genCtorFlagClr( expr, type, se );
    expr = CgCallBackLeft( expr, &setSeCtorTest, se, type );
    return expr;
}


cg_name CgCallBackInitRefBeg(   // START CALL-BACK FOR INIT-REF
    SE* se )                    // - state entry for init-ref variable
{
    FstabCtorTest( FnCtlTop() );
    return genCtorFlagClr( NULL, T_POINTER, se );
}


static void checkCtorTest(      // ELIMINATE CTOR-TEST IF POSSIBLE
    void* data )                // - state entry
{
    SE* se = data;
    SE* test = FstabActualPosn();
#ifndef NDEBUG
    if( PragDbgToggle.dump_stab ) {
        printf( "CallBack: checkCtorTest: se[%x] test[%x]\n"
              , se
              , test );
    }
#endif
    if( ctorTestReqd( test, se ) ) {
        // intervening state entries
        FstabSetSvSe( se );
    } else {
        // no intervening state entries
        FstabRemove();
    }
}


static void patchSE(            // PATCH STATE ENTRY'S VALUE
    patch_entry* pe )           // - patch entry
{
    STATE_VAR state_var = SeStateOptimal( pe->se );
    FstabMarkedPosnSet( pe->se );
    BEPatchInteger( pe->patch, state_var );
    BEFiniPatch( pe->patch );
    CarveFree( carve_patch_se, pe );
}


#if 0
static void patchCtorTest(      // PATCH AFTER ELIMINATING CTOR-TEST
    void* data )                // - patching entry
{
    patch_entry* pe = data;     // - patching entry
    _peDump( pe, "CallBack: patchCtorTest" );
    checkCtorTest( pe->se );
    patchSE( pe );
}
#endif


static cg_name emitPatchCallBack( // EMIT CODE FOR CALL-BACK FOR STATE PATCH
    cg_name expr,               // - current expression
    cg_name emit,               // - code for state patch
    cg_type type,               // - type of current expression
    void (*rtn)(void*),         // - patch routine
    patch_handle patch,         // - handle for patching
    SE* se )                    // - state entry for patch
{
    patch_entry* pe;            // - entry when patching

    expr = CgSideEffect( expr, emit, type );
    pe = CarveAlloc( carve_patch_se );
    pe->se = se;
    pe->patch = patch;
    return CgCallBackRight( expr, rtn, pe, type );
}


cg_name CgCallBackCtorDone(     // SET A CALL BACK FOR A CTOR-TEST : DONE
    cg_name expr,               // - expression
    cg_type type,               // - type of expression
    SE* se )                    // - state entry for ctored object
{
    return CgCallBackRight( expr, &checkCtorTest, se, type );
}


cg_name CgCallBackAutoCtor(     // SET CALL BACKS FOR A DCL'ED AUTO
    cg_name expr,               // - expression
    cg_type type,               // - type of expression
    SE* se )                    // - state entry for ctored object
{
    expr = CgCallBackCtorStart( expr, type, se );
    expr = CgCallBackCtorDone( expr, type, se );
    return expr;
}


static void setTempStart(       // CALL BACK: SET START OF TEMP CTORING
    void* data )                // - state entry
{
    temp_entry* te = (temp_entry*)data;

    te->start = FstabActualPosn();
}


static void setTempDone(        // CALL BACK: SET END OF TEMP CTORING
    void* data )                // - state entry
{
    temp_entry* te = (temp_entry*)data;

//  FstabSetSvSe( te->start );
    FstabAdd( te->se );
    FstabMarkedPosnSet( te->se );
    if( te->patch != NULL ) {
        STATE_VAR state_var = SeStateOptimal( te->se );
        BEPatchInteger( te->patch, state_var );
        BEFiniPatch( te->patch );
    }
    BlkPosnTempBegSet( te->se );
    CarveFree( carve_temp_entry, te );
}


cg_name CgCallBackTempCtor(     // SET CALL BACKS FOR TEMP CTORED
    cg_name expr,               // - expression
    cg_type type,               // - type of expression
    SE* se )                    // - state entry to be inserted on call back
{
    temp_entry* te;             // - entry for ctored temporary
    cg_name emit;               // - emitted expression

    te = CarveAlloc( carve_temp_entry );
    te->se = se;
    te->patch = NULL;
    expr = CgCallBackLeft( expr, &setTempStart, te, type );
    emit = emitPatch( &te->patch );
    if( emit != NULL ) {
        expr = CgSideEffect( expr, emit, type );
    }
    expr = CgCallBackRight( expr, &setTempDone, te, type );
    return expr;
}


static void patchForDtorDelBeg( // CALL-BACK: patch state for DTOR-DEL (start)
    void* data )                // - patch entry
{
    patch_entry* pe = data;     // - patching entry
#if 0
    SE* marked;                 // - current marked position

    marked = FstabMarkedPosn();
    if( marked != FstabCurrPosn() ) {
        FstabSetSvSe( marked );
    }
#endif
    _peDump( pe, "CallBack: patchForDtorDelBeg" );
    FstabAdd( pe->se );
    patchSE( pe );
}


SE* DtorForDelBeg(              // DTORING AREA TO BE DELETED: start
    FN_CTL* fctl,               // - function information
    target_size_t elem_size,    // - size of one element in area
    unsigned dlt1,              // - entry type when one arg
    unsigned dlt2,              // - entry type when two args
    SYMBOL op_del )             // - operator delete to be used
{
    SE* se_dlt;                 // - entry allocated
    SYMBOL var;                 // - var containing address of delete area
    cg_name top_expr;           // - top expression
    cg_type top_type;           // - type of top expression
    cg_name emit;               // - expression for state update
    patch_handle patch;         // - patch handle for area

    if( DtmTabular( fctl ) ) {
        if( 2 == SymFuncArgList( op_del )->num_args ) {
            se_dlt = SeAlloc( dlt2 );
            se_dlt->del_2_array.size = elem_size;
        } else {
            se_dlt = SeAlloc( dlt1 );
        }
        se_dlt->del_1_array.op_del = op_del;
        var = CgVarRw( T_POINTER, SC_AUTO );
        if( se_dlt->base.gen ) {
            AutoRelRegister( var, &se_dlt->del_1_array.offset );
        }
        top_expr = CgExprPopType( &top_type );
        top_expr = CGLVAssign( CgSymbol( var ), top_expr, top_type );
        top_expr = CgFetchType( top_expr, top_type );
        emit = emitPatch( &patch );
        top_expr = emitPatchCallBack( top_expr
                                    , emit
                                    , top_type
                                    , &patchForDtorDelBeg
                                    , patch
                                    , se_dlt );
        CgExprPush( top_expr, top_type );
        DbgSetState( "patchForDtorDelBeg", se_dlt );
    } else {
        se_dlt = NULL;
    }
    return se_dlt;
}


static void patchForDtorDelEnd( // CALL-BACK: patch state for DTOR-DEL (end)
    void* data )                // - patch entry
{
    patch_entry* pe = data;     // - patching entry
    SE* prev;                   // - previous position

    _peDump( pe, "CallBack: patchForDtorDelEnd" );
    DbgVerify( pe->se != NULL, "patchForDtorDelEnd -- ordering" );
    prev = FstabPrevious( pe->se );
    pe->se = prev;
    FstabSetSvSe( prev );
    patchSE( pe );
}


void DtorForDelEnd(             // DTORING AREA TO BE DELETED: end
    FN_CTL* fctl,               // - function information
    SE* se_dlt )                // - entry
{
    cg_name emit;               // - expression for state update
    patch_handle patch;         // - patch handle for area
    cg_name top_expr;           // - top expression
    cg_type top_type;           // - type of top expression

    if( DtmTabular( fctl ) ) {
        DbgVerify( se_dlt != NULL, "DtorForDelEnd -- NULL entry" );
        emit = emitPatch( &patch );
        top_expr = CgExprPopType( &top_type );
        top_expr = emitPatchCallBack( top_expr
                                    , emit
                                    , top_type
                                    , &patchForDtorDelEnd
                                    , patch
                                    , se_dlt );
        CgExprPush( top_expr, top_type );
    }
}


static void callBackNewCtorBeg( // CALL-BACK: START CTOR OF NEW OBJECT
    void *data )                // - patch entry
{
    patch_entry* pe = data;     // - patch entry

    _peDump( pe, "CallBack: cgCallBackNewAlloc" );
    PstkPush( &stack_new_ctors, pe->se );
    FstabAdd( pe->se );
    patchSE( pe );
}


static void callBackNewCtorEnd( // CALL-BACK: END CTOR OF NEW OBJECT
    void *data )                // - patch entry
{
    patch_entry* pe = data;     // - patch entry
    SE* posn;                   // - current position

    posn = FstabActualPosn();
    if( posn == pe->se ) {
        posn = FstabPrevious( posn );
    }
    pe->se = posn;
    patchSE( pe );
    FstabSetSvSe( posn );
}


void CgCallBackNewCtored(       // NEW OBJECT WAS CTOR'ED
    SE* se_del,                 // - state entry for delete during CTOR throw
    FN_CTL* fctl )              // - function information
{
    cg_name expr;               // - emitted expression
    patch_entry* pe;            // - entry when patching
    cg_type type;               // - not used

    CondInfoNewCtorBeg( fctl );
    expr = CgExprPopType( &type );
    CgCommaBefore( expr, type );
    pe = CarveAlloc( carve_patch_se );
    pe->se = se_del;
    expr = CgExprPopType( &type );
    expr = CgCallBackLeft( expr
                         , &callBackNewCtorBeg
                         , pe
                         , type );
    expr = CgComma( emitPatch( &pe->patch ), expr, type );
    CgExprPush( expr, type );
    CondInfoNewCtorEnd( fctl );
    expr = CgExprPopType( &type );
    CgCommaOptional( expr, type );
    pe = CarveAlloc( carve_patch_se );
    pe->se = se_del;
    expr = CgExprPopType( &type );
    expr = CgSideEffect( expr, emitPatch( &pe->patch ), type );
    expr = CgCallBackRight( expr
                          , &callBackNewCtorEnd
                          , pe
                          , type );
    CgExprPush( expr, type );
}
