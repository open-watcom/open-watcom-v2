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
#include "fold.h"
#include "convctl.h"
#include "class.h"
#include "ptrcnv.h"
#include "defarg.h"
#include "objmodel.h"
#include "calldiag.h"

static CNV_DIAG diagExplicit =  // DIAGNOSIS FOR EXPLICIT CAST
{   ERR_CAST_ILLEGAL            // - impossible
,   ERR_CAST_AMBIGUOUS          // - ambiguous
,   ERR_CONVERT_FROM_VIRTUAL_BASE
,   ERR_CAST_PROTECTED          // - protected violation
,   ERR_CAST_PRIVATE            // - private violation
};

#define diagImplicit diagExplicit
#define diagConst    diagExplicit
#define diagReint    diagExplicit
#define diagStatic   diagExplicit
#define diagDynamic  diagExplicit

#define dfnCAST_RESULTs                                                 \
  dfnCAST_RESULT( CAST_ERR_NODE )   /* one or both nodes in error    */ \
, dfnCAST_RESULT( CAST_REINT_PTR_TO_ARITH ) /* do reinterpret ptr->ar*/ \
, dfnCAST_RESULT( CAST_REINT_MPTR_TO_MPTR ) /* do reinterpret mp->mp */ \
, dfnCAST_RESULT( CAST_EXPLICIT_MPTR_REINT ) /* do reinterpret mp->mp */ \
, dfnCAST_RESULT( CAST_STATIC_MEMBPTR ) /* static, mptr -> mptr      */ \
, dfnCAST_RESULT( CAST_IMPLICIT_MEMBPTR ) /* implicit, mptr -> mptr  */ \
, dfnCAST_RESULT( CAST_DO_DYNAMIC )   /* do dynamic cast             */ \
, dfnCAST_RESULT( CAST_DO_CGCONV )    /* do codegen cast             */ \
, dfnCAST_RESULT( CAST_TO_VOID )      /* do cast to void             */ \
, dfnCAST_RESULT( CAST_ARITH_TO_PTR)/* (ptr)arith                    */ \
, dfnCAST_RESULT( CAST_PTR_TO_PTR)  /* (ptr)ptr                      */ \
, dfnCAST_RESULT( CAST_TO_VOID_PTR )/* (void cv *)ptr                */ \
, dfnCAST_RESULT( CAST_TO_BASE    ) /* (base ptr)ptr                 */ \
, dfnCAST_RESULT( CAST_LV_TO_BASE_LV ) /* (lvalue base)(lvalue)      */ \
, dfnCAST_RESULT( CAST_RV_TO_BASE_LV ) /* (lvalue base)(rvalue)      */ \
, dfnCAST_RESULT( CAST_LV_TO_BASE_RV ) /* (rvalue base)(lvalue)      */ \
, dfnCAST_RESULT( CAST_RV_TO_BASE_RV ) /* (rvalue base)(rvalue)      */ \
, dfnCAST_RESULT( CAST_TO_DERIVED    ) /* (derived ptr)ptr           */ \
, dfnCAST_RESULT( CAST_LV_TO_DERIVED_LV ) /* (lvalue derived)(lvalue)*/ \
, dfnCAST_RESULT( CAST_RV_TO_DERIVED_LV ) /* (lvalue derived)(rvalue)*/ \
, dfnCAST_RESULT( CAST_LV_TO_DERIVED_RV ) /* (rvalue derived)(lvalue)*/ \
, dfnCAST_RESULT( CAST_RV_TO_DERIVED_RV ) /* (rvalue derived)(rvalue)*/ \
, dfnCAST_RESULT( CAST_TO_SAME_PTR) /* (T cv *)T cv-2 *              */ \
, dfnCAST_RESULT( CAST_CTOR_LV )    /* ctor making lvalue            */ \
, dfnCAST_RESULT( CAST_CTOR_RV )    /* ctor making rvalue            */ \
, dfnCAST_RESULT( CAST_UDCF_LV )    /* udcf making lvalue            */ \
, dfnCAST_RESULT( CAST_UDCF_RV )    /* udcf making rvalue            */ \
, dfnCAST_RESULT( CAST_CONVERT_TO_BOOL )  /* convert to bool         */ \
, dfnCAST_RESULT( CAST_REPLACE_INTEGRAL)  /* replace with integral # */ \
                                                                        \
  /* ERRORS */                                                          \
, dfnCAST_RESULT( DIAG_ALREADY )    /* already diagnosed(must be 1st)*/ \
, dfnCAST_RESULT( DIAG_MESSAGE )    /* message set                   */ \
, dfnCAST_RESULT( DIAG_CORRUPTED )  /* class corrupted               */ \
, dfnCAST_RESULT( DIAG_IMPOSSIBLE )                                     \
, dfnCAST_RESULT( DIAG_CAST_AWAY_CONST )                                \
, dfnCAST_RESULT( DIAG_CAST_ILLEGAL )                                   \
, dfnCAST_RESULT( DIAG_CAST_PRIVATE )                                   \
, dfnCAST_RESULT( DIAG_CAST_PROTECTED )                                 \
, dfnCAST_RESULT( DIAG_CAST_TO_AMBIGUITY )                              \
, dfnCAST_RESULT( DIAG_CAST_FROM_AMBIGUITY )                            \
, dfnCAST_RESULT( DIAG_CONVERT_FROM_UNDEFD_TYPE )                       \
, dfnCAST_RESULT( DIAG_CONVERT_TO_UNDEFD_TYPE )                         \
, dfnCAST_RESULT( DIAG_CTOR_IMPOSSIBLE )                                \
, dfnCAST_RESULT( DIAG_UNDEFD_CLASS_PTR )                               \
, dfnCAST_RESULT( DIAG_CONST_CAST_REF_TYPE )                            \
, dfnCAST_RESULT( DIAG_CONST_CAST_PTR_TYPE )                            \
, dfnCAST_RESULT( DIAG_CONST_CAST_MPTR_CLASS )                          \
, dfnCAST_RESULT( DIAG_CONST_CAST_MPTR_TYPE )                           \
, dfnCAST_RESULT( DIAG_CONST_CAST_TYPE )                                \
, dfnCAST_RESULT( DIAG_REINT_CAST_REF_TYPE )                            \
, dfnCAST_RESULT( DIAG_REINT_CAST_INT_TYPE )                            \
, dfnCAST_RESULT( DIAG_REINT_CAST_PTR_TYPE )                            \
, dfnCAST_RESULT( DIAG_REINT_CAST_MPTR_TYPE )                           \
, dfnCAST_RESULT( DIAG_REINT_CAST_TYPE )                                \
, dfnCAST_RESULT( DIAG_REINT_TO_INT_TYPE )                              \
, dfnCAST_RESULT( DIAG_STATIC_CAST_REF_TYPE )                           \
, dfnCAST_RESULT( DIAG_STATIC_CAST_PTR_TYPE )                           \
, dfnCAST_RESULT( DIAG_STATIC_CAST_MPTR_TYPE )                          \
, dfnCAST_RESULT( DIAG_STATIC_CAST_TYPE )                               \
, dfnCAST_RESULT( DIAG_STATIC_CAST_EXPR )                               \
, dfnCAST_RESULT( DIAG_STATIC_CAST_OTHER_TO_ENUM )                      \
, dfnCAST_RESULT( DIAG_DYNAMIC_CAST_TYPE )                              \
, dfnCAST_RESULT( DIAG_DYNAMIC_CAST_EXPR )                              \
, dfnCAST_RESULT( DIAG_DYNAMIC_CAST_NO_VFN_SRC )                        \
, dfnCAST_RESULT( DIAG_DYNAMIC_CAST_NO_VFN_TGT )                        \
, dfnCAST_RESULT( DIAG_EXPLICIT_CAST_TYPE )                             \
, dfnCAST_RESULT( DIAG_BASE_AMBIGUOUS )                                 \
, dfnCAST_RESULT( DIAG_BASE_PRIVATE )                                   \
, dfnCAST_RESULT( DIAG_BASE_PROTECTED )                                 \
, dfnCAST_RESULT( DIAG_CTOR_AMBIGUOUS )                                 \
, dfnCAST_RESULT( DIAG_DERIVED_AMBIGUOUS )                              \
, dfnCAST_RESULT( DIAG_SRC_UNDEFED )                                    \
, dfnCAST_RESULT( DIAG_TGT_ABSTRACT )                                   \
, dfnCAST_RESULT( DIAG_TGT_UNDEFED )                                    \
, dfnCAST_RESULT( DIAG_TO_DERIVED )                                     \
, dfnCAST_RESULT( DIAG_UDCF_IMPOSSIBLE )                                \
, dfnCAST_RESULT( DIAG_UDCF_AMBIGUOUS )                                 \
, dfnCAST_RESULT( DIAG_VIRTUAL_DERIVED )                                \
, dfnCAST_RESULT( DIAG_UDC_AMBIGUOUS )                                  \
, dfnCAST_RESULT( DIAG_MPTR_NOT_DERIVED )                               \
, dfnCAST_RESULT( DIAG_BAD_ENUM_TGT )                                   \
, dfnCAST_RESULT( DIAG_TGT_VOID_VOLATILE_STAR )                         \
, dfnCAST_RESULT( DIAG_TGT_VOID_CONST_STAR )                            \
, dfnCAST_RESULT( DIAG_TGT_VOID_STAR )                                  \
, dfnCAST_RESULT( DIAG_REF_ADDS_BOTH )                                  \
, dfnCAST_RESULT( DIAG_REF_ADDS_CONST )                                 \
, dfnCAST_RESULT( DIAG_REF_ADDS_VOLATILE )                              \
, dfnCAST_RESULT( DIAG_NOT_CONST_REF )                                  \
, dfnCAST_RESULT( DIAG_FROM_BASE_PRIVATE )                              \
, dfnCAST_RESULT( DIAG_FROM_BASE_PROTECTED )                            \
/*                                          */                          \
/* following are only used as codes         */                          \
/*                                          */                          \
, dfnCAST_RESULT( CAST_TESTED_OK )  /* TEST SUCCEEDED               */  \
, dfnCAST_RESULT( CAST_CTOR )       /* CTOR FOUND                   */  \
, dfnCAST_RESULT( CAST_UDCF )       /* UDCF FOUND                   */  \
, dfnCAST_RESULT( CAST_UDC_IMPOSSIBLE )                                 \


#define dfnCAST_RESULT(a) a
typedef enum
{ dfnCAST_RESULTs
, DIAGNOSIS_START = DIAG_ALREADY    // start of diagnosis results
} CAST_RESULT;
#undef dfnCAST_RESULT


static bool okSoFar             // TEST IF STILL OK
    ( CONVCTL* ctl )            // - conversion control
{
    bool retn;                  // - return: TRUE ==> is ok so far
    PTREE expr;                 // - current expression

    expr = ctl->expr;
    if( PT_ERROR == expr->op ) {
        retn = FALSE;
    } else {
        if( PT_ERROR == expr->u.subtree[1]->op ) {
            PTreeErrorNode( expr );
            retn = FALSE;
        } else {
            retn = TRUE;
        }
    }
    return retn;
}


static PTREE stripOffCast       // STRIP CAST NODES
    ( CONVCTL* ctl              // - conversion control
    , bool ok )                 // - true ==> set type from cast node
{
    if( ! ctl->keep_cast
     && ( NodeIsBinaryOp( ctl->expr, CO_CONVERT )
       || NodeIsBinaryOp( ctl->expr, CO_CONST_CAST )
       || NodeIsBinaryOp( ctl->expr, CO_STATIC_CAST )
       || NodeIsBinaryOp( ctl->expr, CO_REINTERPRET_CAST )
       || NodeIsBinaryOp( ctl->expr, CO_DYNAMIC_CAST ) ) ) {
        PTREE old = ctl->expr;
        PTREE right = PTreeCopySrcLocation( old->u.subtree[1], old );
        if( ok ) {
            ctl->expr = NodeSetType( right
                                   , ctl->tgt.orig
                                   , old->flags & PTF_LVALUE );
        } else {
            ctl->expr = right;
        }
        PTreeFree( old->u.subtree[0] );
        old->u.subtree[0] = NULL;
        PTreeFree( old );
    }
    return ctl->expr;
}


static PTREE stripOffCastOk     // STRIP CAST NODES, WHEN OK
    ( CONVCTL* ctl )            // - conversion control
{
    return stripOffCast( ctl, TRUE );
}


static PTREE stripOffCastOrig   // STRIP CAST NODES, WHEN ORIGINAL TO BE LEFT
    ( CONVCTL* ctl )            // - conversion control
{
    return stripOffCast( ctl, FALSE );
}


static void markUserCast        // MARK AS USER CAST
    ( CONVCTL* ctl )            // - conversion control
{
    PTREE expr = ctl->expr;
    expr->cgop = CO_CONVERT;
    expr->flags |= ( ~PTF_NEVER_PROPPED ) & expr->u.subtree[1]->flags;
    if( ! ctl->clscls_implicit ) {
        expr->u.subtree[0]->cgop = CO_USER_CAST;
    }
}


static bool getClassRvalue      // GET RVALUE FOR CLASS EXPRESSION
    ( CONVCTL* ctl )            // - conversion control
{
    PTREE expr = ctl->expr->u.subtree[1];
    if( OMR_CLASS_VAL == ObjModelArgument( ctl->tgt.class_type ) ) {
        expr = NodeRvalue( expr );
    } else {
        expr = NodeRvForRefClass( expr );
    }
    ctl->expr->u.subtree[1] = expr;
    return okSoFar( ctl );
}


static bool getLvalue           // GET LVALUE FOR EXPRESSION
    ( CONVCTL* ctl              // - conversion control
    , bool force_to_temp )      // - TRUE ==> force it into a temp
{
    PTREE expr = ctl->expr->u.subtree[1];

    if( expr->flags & PTF_CLASS_RVREF ) {
        expr = NodeLvForRefClass( expr );
    } else {
        TYPE type = expr->type;
        OMR obj_model = ObjModelArgument( type );
        switch( obj_model ) {
          case OMR_CLASS_VAL :
            if( force_to_temp ) {
                expr = NodeCopyClassObject( NodeTemporary( type ), expr );
            } else {
                expr = NodeForceLvalue( expr );
            }
            break;
          case OMR_CLASS_REF :
            expr = NodeLvForRefClass( expr );
            if( ! force_to_temp ) break;
            // drops thru
          case OMR_SCALAR :
            break;
          DbgDefault( "funny OMR" );
        }
    }
    ctl->expr->u.subtree[1] = expr;
    return okSoFar( ctl );
}


static CAST_RESULT diagNonConstRefBinding // DIAGNOSE BINDING TO NON-CONST REF
    ( CONVCTL* ctl )            // - conversion control
{
    CAST_RESULT result;         // - cast result

    if( ctl->diag_bind_ncref ) {
        result = CAST_TESTED_OK;
    } else {
        ctl->diag_bind_ncref = TRUE;
        if( SymIsThunk( ScopeFunctionInProgress() ) ) {
            result = CAST_TESTED_OK;
        } else if( ConvCtlWarning( ctl, ANSI_TEMP_USED_TO_INIT_NONCONST_REF ) ) {
            result = DIAG_ALREADY;
        } else {
            result = CAST_TESTED_OK;
        }
    }
    return result;
}


static CAST_RESULT diagExtConversion // DIAGNOSE EXTENDED CONVERSION
    ( CONVCTL* ctl )            // - conversion control
{
    CAST_RESULT result;         // - cast result

    if( ctl->diag_ext_conv ) {
        result = CAST_TESTED_OK;
    } else {
        ctl->diag_ext_conv = TRUE;
        if( ConvCtlWarning( ctl, ANSI_EXTENDED_CONVERSION_UDC ) ) {
            result = DIAG_ALREADY;
        } else {
            result = CAST_TESTED_OK;
        }
    }
    return result;
}


static CAST_RESULT checkConstRef// CHECK FOR TEMP -> NON-CONST REFERENCE
    ( CONVCTL* ctl              // - conversion control
    , CAST_RESULT def_result )  // - default result
{
    CAST_RESULT result;         // - cast result

    if( ! ctl->clscls_cv ) {
        result = CAST_TESTED_OK;
    } else if( TypeIsConst( ctl->tgt.unmod->of ) ) {
        result = CAST_TESTED_OK;
    } else {
        PTREE expr;
        expr = ctl->expr->u.subtree[1];
        if( ! ExprIsLvalue( expr )
         || NodeReferencesTemporary( expr ) ) {
            result = diagNonConstRefBinding( ctl );
        } else {
            // always an error if not a temp.
            result = def_result;
        }
    }
    return result;
}


static CAST_RESULT reqdConstRef // CHECK FOR TEMP -> NON-CONST REFERENCE
    ( CONVCTL* ctl )            // - conversion control
{
    CAST_RESULT result;         // - cast result

    if( ctl->diag_bind_ncref
     || TypeIsConst( ctl->tgt.unmod->of ) ) {
        result = CAST_TESTED_OK;
    } else {
        result = diagNonConstRefBinding( ctl );
    }
    return result;
}


static CAST_RESULT findConvRtn   // LOCATE UDC FOR CONVERSION
    ( CONVCTL* ctl               // - conversion control
    , FNOV_UDC_CONTROL control ) // - control type of udc initialization
{
    CAST_RESULT result;         // - cast result
    FNOV_COARSE_RANK rank;      // - UDC RANKING
    bool is_ctor;               // - TRUE ==> ctor udc, FALSE ==> udcf udc
    FNOV_LIST* fnov_list;       // - matches list

    result = 0;
    if( ctl->tgt.class_operand ) {
        ClassAddDefaultCopy( ctl->tgt.class_type->u.c.scope );
    }
    if( ctl->tgt.reference ) {
        rank = UdcLocateRef( control
                           , ctl->src.orig
                           , ctl->tgt.orig
                           , &ctl->expr->u.subtree[1]
                           , &is_ctor
                           , &fnov_list
                           , &ctl->fnov_diag );
        if( rank == OV_RANK_NO_MATCH
         && (ctl->tgt.ptedflags & TF1_CONST) == 0
         && ! ( ( ctl->clscls_explicit || ctl->clscls_static )
             && ctl->src.reference )
         && NULL != ctl->tgt.pted
         && TYP_CLASS == ctl->tgt.pted->id
          ) {
            // later: use the following list for diagnosis or free it later
            // We may wish to save this list for possible later restore in
            //  this case.
            FnovFreeDiag( &ctl->fnov_diag );
            FnovListFree( &fnov_list );
            // Attempt to find conversion to class, so there would be a
            // binding to a non-const reference.
            // It is better to diagnose the error as a binding to a
            // non-const-ref than to report that no conversion was available.
            rank = UdcLocate( FNOV_UDC_DIRECT
                            , ctl->src.orig
                            , ctl->tgt.pted
                            , &ctl->expr->u.subtree[1]
                            , &is_ctor
                            , &fnov_list
                            , &ctl->fnov_diag );
            if( rank == OV_RANK_NO_MATCH ) {
                FnovListFree( &fnov_list );
                FnovFreeDiag( &ctl->fnov_diag );
                rank = UdcLocate( FNOV_UDC_COPY
                                , ctl->src.orig
                                , ctl->tgt.pted
                                , &ctl->expr->u.subtree[1]
                                , &is_ctor
                                , &fnov_list
                                , &ctl->fnov_diag );
            }
            if( rank == OV_RANK_UD_CONV ) {
                result = diagNonConstRefBinding( ctl );
                if( result != CAST_TESTED_OK ) {
                    rank = OV_RANK_NULL; // fake return, better diagnostic
                }
            } else {
                result = CAST_UDC_IMPOSSIBLE;
                rank = OV_RANK_NULL; // fake return, original rank
            }
        }
    } else {
        rank = UdcLocate( control
                        , ctl->src.orig
                        , ctl->tgt.orig
                        , &ctl->expr->u.subtree[1]
                        , &is_ctor
                        , &fnov_list
                        , &ctl->fnov_diag );
        if( rank == OV_RANK_NO_MATCH
         && NULL != ctl->tgt.pted
         && TYP_CLASS == ctl->tgt.pted->id
         && CompFlags.extensions_enabled ) {
            FNOV_UDC_CONTROL ext_ctl;
            // later: use the following list for diagnosis or free it later
            // We may wish to save this list for possible later restore in
            //  this case.
            FnovFreeDiag( &ctl->fnov_diag );
            FnovListFree( &fnov_list );
            if( control == FNOV_UDC_DIRECT ) {
                ext_ctl = FNOV_UDC_COPY;
            } else {
                ext_ctl = FNOV_UDC_DIRECT;
            }
            rank = UdcLocate( ext_ctl
                            , ctl->src.orig
                            , ctl->tgt.orig
                            , &ctl->expr->u.subtree[1]
                            , &is_ctor
                            , &fnov_list
                            , &ctl->fnov_diag );
            if( rank == OV_RANK_UD_CONV
             && is_ctor ) {
                // don't extend to explicit ctor.s
                TYPE ctor_type
                    = FunctionDeclarationType( fnov_list->sym->sym_type );
                if( ctor_type->flag & TF1_EXPLICIT ) {
                    rank = OV_RANK_NULL;
                }
            }
            if( rank == OV_RANK_UD_CONV ) {
                result = diagExtConversion( ctl );
                if( result != CAST_TESTED_OK ) {
                    rank = OV_RANK_NULL; // fake return, better diagnostic
                }
            } else {
                result = CAST_UDC_IMPOSSIBLE;
                rank = OV_RANK_NULL; // fake return, original rank
            }
        }
    }
    switch( rank ) {
      case OV_RANK_UD_CONV :
        ctl->conv_fun = fnov_list->sym;
        ctl->conv_type = SymFuncReturnType( ctl->conv_fun );
        if( is_ctor ) {
            if( NULL == ctl->destination
             && TypeAbstract( ctl->tgt.class_type ) ) {
                result = DIAG_TGT_ABSTRACT;
            } else {
                result = CAST_CTOR;
            }
        } else {
            result = CAST_UDCF;
        }
        // drops thru
      case OV_RANK_NULL :       // fake return
        FnovListFree( &fnov_list );
        FnovFreeDiag( &ctl->fnov_diag );
        break;
      case OV_RANK_UD_CONV_AMBIG :
        result = ctl->clscls_copy_init
                    ? DIAG_UDC_AMBIGUOUS
                    : DIAG_CTOR_AMBIGUOUS;
        break;
      case OV_RANK_NO_MATCH :
        // later: use the following list for diagnosis or free it later
        FnovListFree( &fnov_list );
        FnovFreeDiag( &ctl->fnov_diag );
        result = CAST_UDC_IMPOSSIBLE;
        break;
      DbgDefault( "FindConvRtn -- bad return from overloading" );
    }
    return result;
}


static CAST_RESULT findConvFunc // LOCATE UDC FOR CONVERSION
    ( CONVCTL* ctl )            // - conversion control
{
    FNOV_UDC_CONTROL control;

    control = ctl->clscls_copy_init ? FNOV_UDC_COPY : FNOV_UDC_DIRECT;
    return findConvRtn( ctl, control );
}


static CAST_RESULT findConvCtor // LOCATE CTOR FOR CONVERSION
    ( CONVCTL* ctl )            // - conversion control
{
    CAST_RESULT retn = findConvRtn( ctl, FNOV_UDC_DIRECT );
    if( retn == CAST_UDC_IMPOSSIBLE ) {
        retn = DIAG_CAST_ILLEGAL;
    } else if( retn == CAST_CTOR ) {
        SEARCH_RESULT* result = ScopeFindBaseMember( ctl->tgt.class_type->u.c.scope, ctl->conv_fun->name->name );
        if( NULL == result ) {
            retn = DIAG_ALREADY;
        } else {
            ScopeFreeResult( result );
            retn = CAST_CTOR;
        }
    }
    return retn;
}


static CNV_DIAG* getDiagnosis   // GET DIAGNOSIS TO BE USED
    ( CONVCTL* ctl )            // - conversion control
{
    CNV_DIAG* diag;             // - diagnosis to be used

    diag = ctl->diag_cast;
    if( NULL == diag ) {
        diag = ctl->diag_good;
    }
    return diag;
}


static bool warnTruncTypes      // WARN IF TRUNCATION
    ( CONVCTL* ctl              // - conversion control
    , TYPE src                  // - source type
    , TYPE tgt )                // - target type
{
    bool retn;                  // - return: TRUE ==> no truncation error
    unsigned msg_no;            // - message #

    if( src == tgt ) {
        retn = TRUE;
    } else {
        CNV_RETN trunc;
        if( ctl->clscls_implicit ) {
            trunc = NodeCheckPtrTrunc( src, tgt );
            msg_no = WARN_POINTER_TRUNCATION;
        } else {
            trunc = NodeCheckPtrCastTrunc( src, tgt );
            msg_no = WARN_POINTER_TRUNCATION_CAST;
        }
        if( trunc == CNV_OK ) {
            retn = TRUE;
        } else {
            retn = ! ConvCtlWarning( ctl, msg_no );
        }
    }
    return retn;
}


static CAST_RESULT warnRefTruncation // WARN IF TRUNCATION
    ( CONVCTL* ctl              // - conversion control
    , TYPE src_type )           // - source type
{
    return warnTruncTypes( ctl, ctl->tgt.unmod, src_type )
         ? CAST_TESTED_OK
         : DIAG_ALREADY;
}


static bool zeroSrc             // TEST IF SOURCE OPERAND IS CONST ZERO
    ( CONVCTL* ctl )            // - conversion control
{
    PTREE expr = PTreeOpRight( ctl->expr );
    return NodeIsZeroIntConstant( expr );
}


static void setConversionNode   // SET CONVERSION TYPE INTO PTREE NODE
    ( CONVCTL* ctl )            // - conversion control
{
    NodeSetType( ctl->expr, ctl->tgt.orig, PTF_LV_CHECKED );
}


static PTREE doReintMPtrToMPtr  // DO A RE-INTERPRET MEMB-PTR CONVERSION
    ( CONVCTL* ctl )            // - conversion control
{
#ifndef NDEBUG
    unsigned retn;
    retn = MembPtrReint( &ctl->expr->u.subtree[1], ctl->tgt.orig );
    DbgVerify( CNV_OK == retn, "ReintCast -- should work" );
    DbgVerify( ctl->expr->u.subtree[1]->cgop != CO_MEMPTR_CONST, "ReintCast -- mp const" );
#else
    MembPtrReint( &ctl->expr->u.subtree[1], ctl->tgt.orig );
#endif
    stripOffCastOk( ctl );
    return ctl->expr;
}


// Not for member ptrs
//
static PTREE doCgConversion     // DO A CONVERSION ACCOMPLISHED BY CODEGEN
    ( CONVCTL* ctl )            // - conversion control
{
    TYPE result = NodeType( ctl->expr->u.subtree[1] );

    if( result != ctl->tgt.unmod && ! TypesIdentical( TypedefModifierRemoveOnly( result ), ctl->tgt.unmod ) ) {
        ctl->keep_cast = TRUE;
    }
    ctl->expr->cgop = CO_CONVERT;
    markUserCast( ctl );
    setConversionNode( ctl );
    return ctl->expr;
}


static CAST_RESULT staticCvClClOk // TEST IF CONST CASTED AWAY ON STATIC CONV
    ( CONVCTL* ctl )            // - conversion control
{
    CAST_RESULT retn;           // - return: cast result

    if( ctl->clscls_cv
     && ! ctl->diag_bind_ncref ) {
        type_flag src = ctl->src.ptedflags & TF1_CV_MASK;
        type_flag tgt = ctl->tgt.ptedflags & TF1_CV_MASK;
        type_flag both = src & tgt;
        if( src == both ) {
            retn = CAST_TESTED_OK;
        } else {
            if( ctl->clscls_explicit ) {
                ctl->mismatch = src & ~ both;
                ctl->cv_mismatch = TRUE;
                retn = DIAG_CAST_AWAY_CONST;
            } else {
                retn = diagNonConstRefBinding( ctl );
            }
        }
    } else {
        retn = CAST_TESTED_OK;
    }
    return retn;
}


static TYPE getTargetAsReference// GET TARGET TYPE AS A REFERENCE
    ( CONVCTL* ctl )            // - conversion control
{
    return ctl->tgt.reference
         ? ctl->tgt.orig
         : MakeReferenceTo( ctl->tgt.orig );
}


static bool castToBase          // DO CAST TO LV --> LV BASE
    ( CONVCTL* ctl )            // - conversion control
{
    NodeConvertDerivedToBase( &ctl->expr->u.subtree[1]
                            , getTargetAsReference( ctl )
                            , ctl->src.class_type->u.c.scope
                            , ctl->tgt.class_type->u.c.scope );
    return okSoFar( ctl );
}


static PTREE castLvToLvBase     // CAST LV-DERIVED --> LV-BASE
    ( CONVCTL* ctl )            // - conversion control
{
    if( castToBase( ctl ) ) {
        doCgConversion( ctl );
    }
    return ctl->expr;
}


static PTREE castLvToRvBase     // CAST LV-DERIVED --> RV-BASE
    ( CONVCTL* ctl )            // - conversion control
{
    if( castToBase( ctl )
     && getClassRvalue( ctl ) ) {
        doCgConversion( ctl );
    }
    return ctl->expr;
}


static PTREE castRvToLvBase     // CAST RV-DERIVED --> LV-BASE
    ( CONVCTL* ctl )            // - conversion control
{
    if( getLvalue( ctl, TRUE )
     && castToBase( ctl ) ) {
        doCgConversion( ctl );
    }
    return ctl->expr;
}


static PTREE castRvToRvBase     // CAST RV-DERIVED --> LV-BASE
    ( CONVCTL* ctl )            // - conversion control
{
    if( getLvalue( ctl, TRUE )
     && castToBase( ctl )
     && getClassRvalue( ctl ) ) {
        doCgConversion( ctl );
    }
    return ctl->expr;
}


static bool castCtor            // APPLY CTOR
    ( CONVCTL* ctl )            // - conversion control
{
    PTREE inp_node;             // - input node
    PTREE node;                 // - node under construction
    bool retn;                  // - return: TRUE ==> conversion worked

    if( ctl->src.reference
     || getLvalue( ctl, FALSE ) ) {
        inp_node = NodeArg( ctl->expr->u.subtree[1] );
        ctl->expr->u.subtree[1] = inp_node;
        NodeConvertCallArgList( ctl->expr
                              , 1
                              , ctl->conv_fun->sym_type
                              , &ctl->expr->u.subtree[1] );
        if( okSoFar( ctl ) ) {
            PTREE temp;
            CALL_OPT opt;
            PTREE dtor_node;
            PTREE src_node;
            temp = ctl->destination;
            if( NULL == temp ) {
                if( ctl->tgt.reference ) {
                    CAST_RESULT result = reqdConstRef( ctl );
                    if( result != CAST_TESTED_OK ) return FALSE;
                }
                temp = NodeTemporary( ctl->tgt.class_type );
                ctl->destination = temp;
                ctl->dtor_destination = TRUE;
            }
            ctl->used_destination = TRUE;
            opt = AnalyseCallOpts( ctl->tgt.class_type
                                 , inp_node->u.subtree[1]
                                 , &dtor_node
                                 , &src_node );
            if( opt == CALL_OPT_NONE ) {
                if( ctl->tgt.class_type
                        == ClassTypeForType( inp_node->type )
                 && OMR_CLASS_VAL
                        == ObjModelArgument( ctl->tgt.class_type ) ) {
                    ctl->conv_fun = NULL;
                    inp_node = NodeRvalue( inp_node->u.subtree[1] );
                    PTreeFree( ctl->expr->u.subtree[1] );
                    ctl->expr->u.subtree[1] = NULL;
                    node = NodeCopyClassObject( temp, inp_node );
                } else {
                    temp = NodeArg( temp );
                    node = NodeMakeCall( ctl->conv_fun
                                       , SymFuncReturnType( ctl->conv_fun )
                                       , ctl->expr->u.subtree[1] );
                    node->flags |= PTF_LVALUE;
                    if( AddDefaultArgs( ctl->conv_fun, node ) ) {
                        node = CallArgsArrange( ctl->conv_fun->sym_type
                                              , node
                                              , node->u.subtree[1]
                                              , temp
                                              , CallArgumentExactCtor
                                                    ( ctl->tgt.class_type
                                                    , TRUE )
                                              , NULL );
                    }
                }
                if( PT_ERROR != node->op ) {
                    if( ctl->req == CNV_FUNC_RET ) {
                        node = PtdScopeCall( node, ctl->conv_fun );
                    } else {
                        node = PtdCtoredExprType( node
                                                , ctl->conv_fun
                                                , ctl->tgt.class_type );
                    }
                    if( ctl->dtor_destination ) {
                        node->flags |= PTF_TEMP_CONV;
                    }
                }
            } else {
                inp_node = inp_node->u.subtree[1];
                PTreeFree( ctl->expr->u.subtree[1] );
                ctl->expr->u.subtree[1] = NULL;
                node = CopyOptimize( src_node
                                   , inp_node
                                   , temp
                                   , dtor_node
                                   , opt );
            }
            ctl->expr->u.subtree[1] = node;
            retn = okSoFar( ctl );
        } else {
            retn = FALSE;
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


static PTREE castCtorLv         // CTOR --> LVALUE
    ( CONVCTL* ctl )            // - conversion control
{
    if( castCtor( ctl ) ) {
        doCgConversion( ctl );
    }
    return ctl->expr;
}


static PTREE castCtorRv         // CTOR --> RVALUE
    ( CONVCTL* ctl )            // - conversion control
{
    if( castCtor( ctl )
     && getClassRvalue( ctl ) ) {
        doCgConversion( ctl );
    }
    return ctl->expr;
}


static PTREE castUdcf           // APPLY USER-DEFINED CONVERSION FUNCTION
    ( CONVCTL* ctl )            // - conversion control
{
    if( ctl->src.reference
     || getLvalue( ctl, FALSE ) ) {
        CALL_DIAG call_diag;
        ctl->expr->u.subtree[1] = UdcCall( ctl->expr->u.subtree[1]
                                         , ctl->conv_type
                                         , CallDiagFromCnvDiag
                                                ( &call_diag
                                                , ctl->diag_cast )
                                         );
        if( okSoFar( ctl ) ) {
            // If we have already warned if temp to non-const ref to temp,
            // we convert to a const ref (which will be casted away by
            // doCgConversion
            TYPE new_targ;
            if( ctl->diag_bind_ncref
             && ctl->tgt.reference ) {
                new_targ = ctl->tgt.unmod->of;
                new_targ = MakeConstTypeOf( new_targ );
                new_targ = MakeReferenceTo( new_targ );
                if( NULL != ctl->destination ) {
                    ctl->destination = NodeConvert( new_targ
                                                  , ctl->destination ) ;
                }
            } else {
                new_targ = ctl->tgt.orig;
            }
            if( NULL == ctl->destination
             || ! ctl->tgt.class_operand ) {
                // no destination or not class
                if( ctl->tgt.orig != ctl->conv_type ) {
                    ctl->expr->u.subtree[1] = CastImplicit
                          ( ctl->expr->u.subtree[1]
                          , new_targ
                          , CNV_INIT
                          , ctl->diag_cast );
                }
            } else {
                // destination req'd
                // must be copy initialization
                //
                // the scope is temporarily reset to avoid protection
                // using ClassDefaultCopy will optimize away the actual
                //  call to a copy ctor
                //
                SCOPE curr = GetCurrScope();
                DbgVerify( ctl->clscls_copy_init || ctl->diag_ext_conv
                         , "not copy init" );
                ctl->used_destination = TRUE;
                ctl->expr->u.subtree[1] =
                    ClassDefaultCopyTemp( ctl->destination
                                        , ctl->expr->u.subtree[1]
                                        , ctl->diag_cast );
                if( ! ctl->tgt.reference
                 && okSoFar( ctl ) ) {
                    ctl->expr->u.subtree[1] =
                        NodeRvalue( ctl->expr->u.subtree[1] );
                }
                SetCurrScope(curr);

            }
            if( okSoFar( ctl ) ) {
                doCgConversion( ctl );
            }
        }
    }
    return ctl->expr;
}


static PTREE castUdcfLv         // UDCF --> LVALUE
    ( CONVCTL* ctl )            // - conversion control
{
    return castUdcf( ctl );
}


static PTREE castUdcfRv         // UDCF --> RVALUE
    ( CONVCTL* ctl )            // - conversion control
{
    return castUdcf( ctl );
}


static bool castToDerived       // CAST LV-BASE --> LV_DERIVED
    ( CONVCTL* ctl )            // - conversion control
{
    NodeConvertBaseToDerived( &ctl->expr->u.subtree[1]
                            , getTargetAsReference( ctl )
                            , ctl->tgt.class_type->u.c.scope
                            , ctl->src.class_type->u.c.scope );
    return okSoFar( ctl );
}


static PTREE castLvToLvDerived  // CAST LV-BASE --> LV_DERIVED
    ( CONVCTL* ctl )            // - conversion control
{
    if( castToDerived( ctl ) ) {
        doCgConversion( ctl );
    }
    return ctl->expr;
}


static PTREE castLvToRvDerived  // CAST LV-BASE --> RV_DERIVED
    ( CONVCTL* ctl )            // - conversion control
{
    if( castToDerived( ctl )
     && getClassRvalue( ctl ) ) {
        doCgConversion( ctl );
    }
    return ctl->expr;
}


static PTREE castRvToLvDerived  // CAST RV-BASE --> LV_DERIVED
    ( CONVCTL* ctl )            // - conversion control
{
    if( getLvalue( ctl, TRUE )
     && castToDerived( ctl ) ) {
        doCgConversion( ctl );
    }
    return ctl->expr;
}


static PTREE castRvToRvDerived  // CAST RV-BASE --> RV_DERIVED
    ( CONVCTL* ctl )            // - conversion control
{
    if( getLvalue( ctl, TRUE )
     && castToDerived( ctl )
     && getClassRvalue( ctl ) ) {
        doCgConversion( ctl );
    }
    return ctl->expr;
}


static CAST_RESULT resultToNonBase // DO A CONVERSION TO NON-BASE CLASS
    ( CONVCTL* ctl )            // - conversion control
{
    CAST_RESULT result;         // - cast result

    result = findConvFunc( ctl );
    if( ctl->tgt.reference ) {
        // ANSI allows only conversions to reference
        if( result == CAST_CTOR ) {
            if( TypeIsConst( ctl->tgt.unmod->of ) ) {
                result = CAST_CTOR_LV;
            } else if( ctl->clscls_explicit
                    || ctl->clscls_static ) {
                result = CAST_UDC_IMPOSSIBLE;
            } else {
                result = diagNonConstRefBinding( ctl );
                if( result == CAST_TESTED_OK ) {
                    result = CAST_CTOR_LV;
                }
            }
        } else if( result == CAST_UDCF ) {
            result = CAST_UDCF_LV;
        } else if( result == DIAG_CTOR_AMBIGUOUS
                || result == DIAG_UDCF_AMBIGUOUS ) {
            if( ctl->clscls_explicit
             && ctl->src.reference ) {
                result = CAST_UDC_IMPOSSIBLE;
            }
        }
        if( result == CAST_UDC_IMPOSSIBLE
         && ctl->src.reference ) {
            if( ctl->clscls_explicit ) {
                switch( ctl->ctd ) {
                  case CTD_RIGHT_PRIVATE :
                  case CTD_RIGHT_PROTECTED :
                  case CTD_RIGHT :
                    result = CAST_LV_TO_DERIVED_LV;
                    break;
                  default :
                    result = CAST_DO_CGCONV;
                    break;
                }
            } else if( ctl->clscls_static ) {
                switch( ctl->ctd ) {
                  case CTD_RIGHT_PRIVATE :
                    result = DIAG_FROM_BASE_PRIVATE;
                    break;
                  case CTD_RIGHT_PROTECTED :
                    result = DIAG_FROM_BASE_PROTECTED;
                    break;
                  case CTD_RIGHT :
                    result = CAST_LV_TO_DERIVED_LV;
                    break;
                }
            } else {
                switch( ctl->ctd ) {
                  case CTD_RIGHT_PRIVATE :
                  case CTD_RIGHT_PROTECTED :
                  case CTD_RIGHT :
                    result = DIAG_TO_DERIVED;
                    break;
                }
            }
        }
    } else {
        if( result == CAST_CTOR ) {
            result = CAST_CTOR_RV;
        } else if( result == CAST_UDCF ) {
            result = CAST_UDCF_RV;
        }
    }
    return result;
}


static CAST_RESULT resultToClass// DO A CONVERSION TO CLASS, CLASS-REF
    ( CONVCTL* ctl )            // - conversion control
{
    CAST_RESULT result;         // - cast result

    result = CAST_TESTED_OK;
    if( ! ctl->tgt.reference ) {
        if( ClassCorrupted( ctl->tgt.class_type ) ) {
            result = DIAG_CORRUPTED;
        } else if( ! TypeDefined( ctl->tgt.class_type ) ) {
            result = DIAG_TGT_UNDEFED;
        } else if( TypeAbstract( ctl->tgt.class_type ) ) {
            result = DIAG_TGT_ABSTRACT;
        }
    }
    if( result == CAST_TESTED_OK
     && ctl->src.class_operand
     && ! ctl->src.reference ) {
        if( ClassCorrupted( ctl->src.class_type ) ) {
            result = DIAG_CORRUPTED;
        } else if( ! TypeDefined( ctl->src.class_type ) ) {
            result = DIAG_SRC_UNDEFED;
        }
    }
    if( result == CAST_TESTED_OK ) switch( ctl->ctd ) {
      case CTD_LEFT :
      case CTD_LEFT_VIRTUAL :
        if( NULL != ctl->destination ) {
            // req'd to initialize destination
            result = findConvCtor( ctl );
            if( result != CAST_CTOR ) break;
            if( ctl->tgt.reference ) {
                result = CAST_CTOR_LV;
            } else {
                result = CAST_CTOR_RV;
            }
        } else if( ctl->tgt.reference ) {
            // no destination, lvalue req'd
            result = staticCvClClOk( ctl );
            if( result != CAST_TESTED_OK ) break;
            if( ctl->src.reference ) {
                result = checkConstRef( ctl, CAST_TESTED_OK );
                if( result != CAST_TESTED_OK ) break;
                result = warnRefTruncation( ctl, ctl->src.orig );
                if( result != CAST_TESTED_OK ) break;
                result = CAST_LV_TO_BASE_LV;
            } else {
                result = checkConstRef( ctl, CAST_TESTED_OK );
                if( result != CAST_TESTED_OK ) break;
                result = warnRefTruncation
                            ( ctl, MakeReferenceTo( ctl->src.orig ) );
                if( result != CAST_TESTED_OK ) break;
                result = CAST_RV_TO_BASE_LV;
            }
        } else {
            // no destination, rvalue req'd
            result = findConvCtor( ctl );
            if( result != CAST_CTOR ) break;
            if( ctl->src.reference ) {
                if( OMR_CLASS_VAL == ObjModelArgument( ctl->tgt.class_type ) ) {
                    result = CAST_LV_TO_BASE_RV;
                } else {
                    result = CAST_CTOR_RV;
                }
            } else {
                result = CAST_RV_TO_BASE_RV;
            }
        }
        break;
      case CTD_RIGHT_VIRTUAL :
        result = resultToNonBase( ctl );
        if( result != CAST_UDC_IMPOSSIBLE ) break;
        if( ctl->clscls_explicit
         || ctl->clscls_static ) {
            result = DIAG_VIRTUAL_DERIVED;
        } else {
            result = DIAG_TO_DERIVED;
        }
        break;
      case CTD_RIGHT :
      case CTD_RIGHT_PRIVATE :
      case CTD_RIGHT_PROTECTED :
        result = resultToNonBase( ctl );
        if( result != CAST_UDC_IMPOSSIBLE ) break;
        result = DIAG_TO_DERIVED;
        break;
      case CTD_NO :
        result = resultToNonBase( ctl );
        if( result != CAST_UDC_IMPOSSIBLE ) break;
        result = DIAG_CAST_ILLEGAL;
        break;
      case CTD_LEFT_AMBIGUOUS :
        result = DIAG_BASE_AMBIGUOUS;
        break;
      case CTD_LEFT_PRIVATE :
        result = DIAG_BASE_PRIVATE;
        break;
      case CTD_LEFT_PROTECTED :
        result = DIAG_BASE_PROTECTED;
        break;
      case CTD_RIGHT_AMBIGUOUS :
        if( ctl->clscls_explicit ) {
            result = DIAG_VIRTUAL_DERIVED;
        } else {
            result = DIAG_TO_DERIVED;
        }
        break;
      DbgDefault( "resultToClass -- impossible CTD" );
    }
    return result;
}


static CAST_RESULT resultClassToScalar // DO A CONVERSION CLASS -> SCALAR
    ( CONVCTL* ctl )            // - conversion control
{
    CAST_RESULT result;         // - cast result

    if( ctl->tgt.kind == RKD_VOID ) {
        result = CAST_TO_VOID;
    } else if( ClassCorrupted( ctl->src.class_type ) ) {
        result = DIAG_CORRUPTED;
    } else if( ( ! ctl->src.reference )
            && ( ! TypeDefined( ctl->src.class_type ) ) ) {
        result = DIAG_SRC_UNDEFED;
    } else {
        result = CAST_TESTED_OK;
    }
    if( result == CAST_TESTED_OK ) {
        result = findConvFunc( ctl );
        switch( result ) {
          case CAST_UDCF :
            if( ctl->tgt.reference ) {
                if( NULL == TypeReference( ctl->conv_type ) ) {
                    CAST_RESULT test;
                    test = reqdConstRef( ctl );
                    if( test != CAST_TESTED_OK ) {
                        result = test;
                    } else {
                        result = CAST_UDCF_LV;
                    }
                } else {
                    result = CAST_UDCF_LV;
                }
            } else {
                result = CAST_UDCF_RV;
            }
            break;
          case CAST_UDC_IMPOSSIBLE :
            if( ctl->clscls_explicit
             && ctl->src.reference
             && ctl->tgt.reference ) {
                TYPE src_refed = TypeReferenced( ctl->src.unmod );
                TYPE tgt_refed = TypeReferenced( ctl->tgt.unmod );
                if( NULL == FunctionDeclarationType( src_refed ) ) {
                    if( NULL == FunctionDeclarationType( tgt_refed ) ) {
                        result = CAST_TESTED_OK;
                    } else {
                        result = DIAG_CAST_ILLEGAL;
                    }
                } else {
                    if( NULL == FunctionDeclarationType( tgt_refed ) ) {
                        result = DIAG_CAST_ILLEGAL;
                    } else {
                        result = CAST_TESTED_OK;
                    }
                }
                if( result == CAST_TESTED_OK ) {
                    result = warnRefTruncation( ctl, ctl->src.unmod );
                    if( result == CAST_TESTED_OK ) {
                        result = CAST_DO_CGCONV;
                    }
                }
            } else {
                result = DIAG_CAST_ILLEGAL;
            }
            break;
        }
    }
    return result;
}



static PTREE diagnoseCastError  // DIAGNOSE CASTING ERROR
    ( CONVCTL* ctl )            // - conversion control
{
    if( ConvCtlWarning( ctl, ctl->msg_no ) ) {
        ConvCtlDiagnoseTypes( ctl );
    }
    return ctl->expr;
}


static PTREE diagnoseCast       // DIAGNOSE CASTING ERROR
    ( CONVCTL* ctl              // - conversion control
    , unsigned msg )            // - message #
{
    if( ctl->cv_mismatch ) {
        type_flag cv_add = ctl->mismatch & TF1_CV_MASK;
        switch( cv_add ) {
          default :
            DbgVerify( ctl->mismatch & TF1_UNALIGNED, "bad ctl->mismatch" );
            msg = ERR_CAST_AWAY_UNALIGNED;
            break;
          case TF1_CONST :
            msg = ERR_CAST_AWAY_CONST;
            break;
          case TF1_VOLATILE :
            msg = ERR_CAST_AWAY_VOLATILE;
            break;
          case TF1_CONST | TF1_VOLATILE :
            msg = ERR_CAST_AWAY_CONSTVOL;
            break;
        }
    } else if( ctl->ptr_integral_ext && ctl->clscls_implicit ) {
        msg = ERR_PTR_INTEGER_EXTENSION;
    } else if( ctl->size_ptr_to_int ) {
        msg = ERR_REINT_INTEGRAL_PTR;
    }
    ctl->msg_no = msg;
    return diagnoseCastError( ctl );
}


static bool ptrToIntTruncs      // TEST IF TRUNCATION ON PTR --> INT
    ( CONVCTL* ctl )            // - conversion control
{
    bool retn;
    if( CNV_OK != NodeCheckPtrCastTrunc( ctl->tgt.unmod, ctl->src.orig )
     && ( CompFlags.extensions_enabled
       || CgMemorySize( GetBasicType( TYP_SINT ) ) >
          CgMemorySize( ctl->src.unmod ) ) ) {
        ctl->size_ptr_to_int = TRUE;
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return retn;
}


static PTREE doReintPtrToArith  // DO REINTERPRET: PTR -> ARITH
    ( CONVCTL* ctl )            // - conversion control
{
    PTREE expr;                 // - resultant expression

    if( NULL == IntegralType( ctl->tgt.unmod ) ) {
        expr = diagnoseCast( ctl, ERR_CAST_ILLEGAL );
    } else if( NULL != SegmentShortType( ctl->tgt.unmod ) ) {
        if( ! NodeDerefPtr( &ctl->expr->u.subtree[1] ) ) {
            expr = diagnoseCast( ctl, ERR_CAST_ILLEGAL );
        } else {
            #if _CPU == 8086
                TYPE argument;
                TOKEN_LOCN locn;
                PTreeExtractLocn( ctl->expr, &locn );
                stripOffCastOrig( ctl );
                argument = MakeModifiedType( GetBasicType( TYP_VOID ), TF1_FAR );
                argument = MakePointerTo( argument );
                expr = NodeConvert( argument, ctl->expr );
                argument = GetBasicType( TYP_ULONG );
                expr = NodeConvert( argument, expr );
                expr = NodeBinary( CO_RSHIFT, expr, NodeOffset( 16 ) );
                expr->type = argument;
                expr = NodeConvert( TypeSegmentShort(), expr );
                expr->locn = locn;
            #else
                expr = doCgConversion( ctl );
            #endif
        }
    } else if( ! CompFlags.extensions_enabled
            && ptrToIntTruncs( ctl ) ) {
        expr = diagnoseCast( ctl, ERR_REINT_INTEGRAL_PTR );
    } else {
        expr = doCgConversion( ctl );
    }
    return expr;
}


static PTREE convertToBool          // CONVERT TO BOOL
    ( CONVCTL* ctl )                // - cast info
{
    stripOffCastOrig( ctl );
    return NodeConvertToBool( ctl->expr );
}


static CAST_RESULT arithToArith     // EXPLICIT, IMPLICIT ARITH->ARITH
    ( CONVCTL* ctl )                // - cast info
{
    CAST_RESULT result;             // - cast result

    if( NULL != SegmentShortType( ctl->tgt.unmod ) && NULL == IntegralType( ctl->src.unmod ) ) {
        result = DIAG_CAST_ILLEGAL;
    } else if( ctl->tgt.unmod->id == TYP_BOOL ) {
        result = CAST_CONVERT_TO_BOOL;
    } else {
        if( ctl->clscls_explicit ) {
            result = CAST_DO_CGCONV;
        } else {
            AnalyseIntTrunc( ctl->expr->u.subtree[1], ctl->tgt.unmod );
            if( okSoFar( ctl ) ) {
                result = CAST_DO_CGCONV;
            } else {
                result = DIAG_ALREADY;
            }
        }
    }
    return result;
}


static CAST_RESULT implicitArithToPtr // IMPLICIT ARITH,ENUM->PTR
    ( CONVCTL* ctl )                // - cast info
{
    CAST_RESULT result;             // - cast result

    if( ctl->src.unmod->id == TYP_SINT || NULL != IntegralType( ctl->src.unmod ) ) {
        if( zeroSrc( ctl ) ) {
            result = CAST_REPLACE_INTEGRAL;
        } else if( TypeIsBasedPtr( ctl->tgt.unmod ) ) {
            if( NodeIsConstantInt( ctl->expr->u.subtree[1] ) ) {
                result = CAST_REPLACE_INTEGRAL;
            } else {
                result = CAST_ARITH_TO_PTR;
            }
        } else {
            result = DIAG_CAST_ILLEGAL;
        }
    } else {
        result = DIAG_CAST_ILLEGAL;
    }
    return result;
}


static CAST_RESULT analysePtrToPtr  // ANALYSE PTR --> PTR
    ( CONVCTL* ctl )                // - cast info
{
    CAST_RESULT result;             // - cast result

    ConvCtlAnalysePoints( ctl );
    if( ctl->implicit_cast_ok || ( ctl->explicit_cast_ok && ctl->clscls_explicit ) ) {
        if( ctl->to_base && ctl->to_ambiguous ) {
            result = DIAG_CAST_TO_AMBIGUITY;
        } else {
            CNV_RETN retn = PcPtrValidate( ctl->tgt.unmod
                                         , ctl->src.unmod
                                         , ctl->tgt.pc_ptr
                                         , ctl->src.pc_ptr
                                         , ctl->expr
                                         , ctl->req );
            result = DIAG_CAST_ILLEGAL;
            switch( retn ) {
              DbgDefault( "analysePtrToPtr -- funny pc validation" );
              case CNV_IMPOSSIBLE :
                result = DIAG_CAST_ILLEGAL;
                break;
              case CNV_ERR :
                result = DIAG_ALREADY;
                break;
              case CNV_OK :
                result = CAST_PTR_TO_PTR;
                break;
            }
        }
    } else if( ctl->to_derived ) {
        result = DIAG_TO_DERIVED;
    } else if( ctl->tgt.reference
            && ! ctl->to_base
            && ! TypeCompareExclude( ctl->tgt.pted
                                   , ctl->src.pted
                                   , TC1_NOT_ENUM_CHAR
                                   )
             ) {
        if( ctl->tgt.ptedflags & TF1_CONST ) {
            PTREE node = ctl->expr->u.subtree[1];
            TYPE tgt_type;
            if( ctl->src.reference ) {
                node = NodeRvalue( node );
            }
            tgt_type = ctl->tgt.unmod->of;
            result = CAST_DO_CGCONV;
            if( tgt_type != node->type ) {
                node = CastImplicit( node
                                   , tgt_type
                                   , ctl->req
                                   , ctl->diag_cast );
                if( PT_ERROR == node->op ) {
                    result = CAST_ERR_NODE;
                }
            }
            if( result != CAST_ERR_NODE ) {
                node = NodeAssignTemporary( tgt_type, node );
                ctl->expr->u.subtree[1] = node;
                if( PT_ERROR == node->op ) {
                    result = CAST_ERR_NODE;
                }
            }
        } else {
            ctl->cv_mismatch = FALSE;
            result = DIAG_NOT_CONST_REF;
        }
    } else if( ctl->from_void && ( ctl->clscls_explicit || ctl->clscls_static ) ) {
        result = CAST_DO_CGCONV;
    } else {
        result = DIAG_CAST_ILLEGAL;
    }
    return result;
}


static CAST_RESULT analyseImplicitPtrToPtr // IMPLICIT ANALYSIS: PTR --> PTR
    ( CONVCTL* ctl )                // - cast info
{
    CAST_RESULT result;             // - cast result

    result = analysePtrToPtr( ctl );
    switch( result ) {
      case CAST_PTR_TO_PTR :
        if( ctl->to_base ) {
            if( ctl->to_protected ) {
                result = DIAG_CAST_PROTECTED;
            } else if( ctl->to_private ) {
                result = DIAG_CAST_PRIVATE;
            }
        }
        break;
      case DIAG_CAST_ILLEGAL :
        if( ctl->cv_mismatch ) {
            type_flag tgt_cv = ctl->tgt.ptedflags & TF1_CV_MASK;
            if( ctl->to_void ) {
                if( tgt_cv & TF1_CONST ) {
                    if( tgt_cv & TF1_VOLATILE ) {
                        // use existing result
                    } else {
                        result = DIAG_TGT_VOID_CONST_STAR;
                    }
                } else {
                    if( tgt_cv & TF1_VOLATILE ) {
                        result = DIAG_TGT_VOID_VOLATILE_STAR;
                    } else {
                        result = DIAG_TGT_VOID_STAR;
                    }
                }
            } else if( ctl->tgt.reference ) {
                type_flag src_cv = ctl->src.ptedflags & TF1_CV_MASK;
                type_flag added = src_cv & ~ tgt_cv;
                if( added & TF1_CONST ) {
                    if( added & TF1_VOLATILE ) {
                        result = DIAG_REF_ADDS_BOTH;
                    } else {
                        result = DIAG_REF_ADDS_CONST;
                    }
                } else {
                    result = DIAG_REF_ADDS_VOLATILE;
                }
            }
            ctl->cv_mismatch = FALSE;
        }
        break;
    }
    return result;
}


static CAST_RESULT analyseExplicitPtrToPtr // EXPLICIT ANALYSIS: PTR --> PTR
    ( CONVCTL* ctl )                // - cast info
{
    CAST_RESULT result;             // - cast result

    result = analysePtrToPtr( ctl );
    if( result == CAST_PTR_TO_PTR ) {
        if( ctl->to_derived
         && ctl->ctd == CTD_RIGHT_VIRTUAL ) {
            result = DIAG_VIRTUAL_DERIVED;
        }
    }
    return result;
}


static PTREE checkMptrConversion    // CHECK RESULT OF MEMBER-PTR CONVERSION
    ( CONVCTL* ctl                  // - cast info
    , CNV_RETN retn )               // - return from conversion
{
    if( CNV_OK == ConversionDiagnose( retn, ctl->expr, ctl->diag_cast ) ) {
        stripOffCastOk( ctl );
    }
    return ctl->expr;
}

static TYPE isHostDerivationKnown( TYPE type )
{
    DbgAssert( type != NULL );
    if( type->id == TYP_MEMBER_POINTER ) {
        TYPE host = type->u.mp.host;

        if( ! TypePartiallyDefined( host ) ) {
            return( host );
        }
    }
    return( NULL );
}

static CNV_RETN doMPtrCast( CONVCTL *ctl )
{
    return( MembPtrConvert( &ctl->expr->u.subtree[1], ctl->tgt.orig, CNV_CAST ) );
}

msg_status_t errorStatus( CONVCTL *ctl, MSG_NUM msg )
{
    msg_status_t status;        // - message status

    status = PTreeErrorExpr( ctl->expr, msg );
    if( MS_PRINTED & status ) {
        ConvCtlDiagnoseTypes( ctl );
    }
    return( status );
}


static PTREE doCastResult           // DO CAST RESULT
    ( CONVCTL* ctl                  // - cast info
    , CAST_RESULT result )          // - cast result
{
    PTREE expr;                     // - resultant expression
    CNV_RETN retn;                  // - conversion indication

    switch( result ) {
      case CAST_ERR_NODE :
        expr = PTreeErrorNode( ctl->expr );
        break;
      case CAST_REINT_PTR_TO_ARITH :
        expr = doReintPtrToArith( ctl );
        break;
      case CAST_EXPLICIT_MPTR_REINT :
        if( ctl->diff_mptr_class ) {
            if( ctl->bad_mptr_class ) {
                TYPE host_src = isHostDerivationKnown( ctl->src.unmod );
                TYPE host_tgt = isHostDerivationKnown( ctl->tgt.unmod );
                if( host_src != NULL || host_tgt != NULL ) {
                    // host class of member ptr is undef'd
                    if( errorStatus( ctl, WARN_REINT_FOR_EXPLICIT_MPTR_UNDEFD ) & MS_PRINTED ) {
                        if( host_src != NULL ) {
                            CErr2p( INF_CLASS_NOT_DEFINED, host_src );
                        }
                        if( host_tgt != NULL ) {
                            CErr2p( INF_CLASS_NOT_DEFINED, host_tgt );
                        }
                    }
                } else {
                    // two defined but unrelated classes
                    errorStatus( ctl, WARN_REINT_FOR_EXPLICIT_MPTR_UNRELATED );
                }
                expr = doReintMPtrToMPtr( ctl );
            } else {
                // classes related by derivation
                errorStatus( ctl, WARN_REINT_FOR_EXPLICIT_MPTR_DERIVED );
                retn = doMPtrCast( ctl );
                expr = checkMptrConversion( ctl, retn );
            }
        } else {
            // same classes
            expr = doReintMPtrToMPtr( ctl );
        }
        break;
      case CAST_REINT_MPTR_TO_MPTR :
        expr = doReintMPtrToMPtr( ctl );
        break;
      case CAST_STATIC_MEMBPTR :
        retn = doMPtrCast( ctl );
        expr = checkMptrConversion( ctl, retn );
        break;
      case CAST_IMPLICIT_MEMBPTR :
        retn = MembPtrConvert( &ctl->expr->u.subtree[1]
                             , ctl->tgt.orig
                             , ctl->req );
        expr = checkMptrConversion( ctl, retn );
        break;
      case CAST_ARITH_TO_PTR :
      case CAST_TO_SAME_PTR :
      case CAST_TO_BASE :
      case CAST_TO_DERIVED :
      case CAST_TO_VOID_PTR :
      case CAST_PTR_TO_PTR :
        ConversionTypesSet( ctl->src.orig, ctl->tgt.orig );
        retn = ConversionDiagnose( CastPtrToPtr( ctl )
                                 , ctl->expr
                                 , &diagExplicit );
        setConversionNode( ctl );
        expr = ctl->expr;
        break;
      case CAST_DO_DYNAMIC :
        if( CompFlags.rtti_enabled ) {
            expr = DynamicCast( ctl );
        } else {
            expr = diagnoseCast( ctl, ERR_RTTI_DISABLED );
        }
        break;
      case CAST_DO_CGCONV :
        expr = doCgConversion( ctl );
        expr = CheckCharPromotion( expr );
        break;
      case CAST_TO_VOID :
        ctl->expr->type = ctl->tgt.orig;
        ctl->expr->u.subtree[1] = NodeConvertFlags( ctl->tgt.orig
                                    , ctl->expr->u.subtree[1]
                                    , PTF_MEANINGFUL | PTF_SIDE_EFF );
        markUserCast( ctl );
        expr = ctl->expr;
        break;
      case CAST_LV_TO_BASE_LV :
        expr = castLvToLvBase( ctl );
        break;
      case CAST_LV_TO_BASE_RV :
        expr = castLvToRvBase( ctl );
        break;
      case CAST_RV_TO_BASE_LV :
        expr = castRvToLvBase( ctl );
        break;
      case CAST_RV_TO_BASE_RV :
        expr = castRvToRvBase( ctl );
        break;
      case CAST_LV_TO_DERIVED_LV :
        expr = castLvToLvDerived( ctl );
        break;
      case CAST_LV_TO_DERIVED_RV :
        expr = castLvToRvDerived( ctl );
        break;
      case CAST_RV_TO_DERIVED_LV :
        expr = castRvToLvDerived( ctl );
        break;
      case CAST_RV_TO_DERIVED_RV :
        expr = castRvToRvDerived( ctl );
        break;
      case CAST_CTOR_LV :
        expr = castCtorLv( ctl );
        break;
      case CAST_CTOR_RV :
        expr = castCtorRv( ctl );
        break;
      case CAST_UDCF_LV :
        expr = castUdcfLv( ctl );
        break;
      case CAST_UDCF_RV :
        expr = castUdcfRv( ctl );
        break;
      case CAST_CONVERT_TO_BOOL :
        expr = convertToBool( ctl );
        break;
      case CAST_REPLACE_INTEGRAL :
      { expr = PTreeOp( &ctl->expr->u.subtree[1] );
        expr = NodeIntegralConstant( NodeConstantValue( expr ), ctl->tgt.orig );
        expr = PTreeCopySrcLocation( expr, ctl->expr->u.subtree[1] );
        expr = NodeReplace( ctl->expr, expr );
//      expr = CheckCharPromotion( expr );
        ctl->expr = expr;
      } break;
      case DIAG_MESSAGE :
        expr = diagnoseCastError( ctl );
        break;
      case DIAG_CORRUPTED :
        expr = PTreeErrorNode( ctl->expr );
        break;
      case DIAG_ALREADY :
        ConversionTypesSet( ctl->src.orig, ctl->tgt.orig );
        ConversionDiagnoseInf();
        expr = ctl->expr;
        break;
      case DIAG_IMPOSSIBLE :
        expr = diagnoseCast( ctl, ERR_CALL_WATCOM );
        break;
      case DIAG_CAST_AWAY_CONST :
        expr = diagnoseCast( ctl, ERR_CAST_AWAY_CONST );
        break;
      case DIAG_CTOR_IMPOSSIBLE :
      case DIAG_UDCF_IMPOSSIBLE :
      case DIAG_CAST_ILLEGAL :
        expr = diagnoseCast( ctl, getDiagnosis( ctl )->msg_impossible );
        break;
      case DIAG_CAST_PRIVATE :
        expr = diagnoseCast( ctl, getDiagnosis( ctl )->msg_private );
        break;
      case DIAG_CAST_PROTECTED :
        expr = diagnoseCast( ctl, getDiagnosis( ctl )->msg_protected );
        break;
      case DIAG_UDCF_AMBIGUOUS :
      case DIAG_CAST_TO_AMBIGUITY :
        expr = diagnoseCast( ctl, getDiagnosis( ctl )->msg_ambiguous );
        break;
      case DIAG_VIRTUAL_DERIVED :
        expr = diagnoseCast( ctl, getDiagnosis( ctl )->msg_virt_der );
        break;
      case DIAG_CAST_FROM_AMBIGUITY :
        expr = diagnoseCast( ctl, ERR_CAST_FROM_AMBIGUITY );
        break;
      case DIAG_CONVERT_FROM_UNDEFD_TYPE :
        expr = diagnoseCast( ctl, ERR_CONVERT_FROM_UNDEFD_TYPE );
        break;
      case DIAG_CONVERT_TO_UNDEFD_TYPE :
        expr = diagnoseCast( ctl, ERR_CONVERT_TO_UNDEFD_TYPE );
        break;
      case DIAG_UNDEFD_CLASS_PTR :
        expr = diagnoseCast( ctl, ERR_UNDEFED_CLASS_PTR);
        break;
      case DIAG_CONST_CAST_REF_TYPE :
        expr = diagnoseCast( ctl, ERR_CONST_CAST_REF_TYPE );
        break;
      case DIAG_CONST_CAST_PTR_TYPE :
        expr = diagnoseCast( ctl, ERR_CONST_CAST_PTR_TYPE );
        break;
      case DIAG_CONST_CAST_MPTR_TYPE :
        expr = diagnoseCast( ctl, ERR_CONST_CAST_MPTR_TYPE );
        break;
      case DIAG_CONST_CAST_MPTR_CLASS :
        expr = diagnoseCast( ctl, ERR_CONST_CAST_MPTR_CLASS );
        break;
      case DIAG_CONST_CAST_TYPE :
        expr = diagnoseCast( ctl, ERR_CONST_CAST_TYPE );
        break;
      case DIAG_REINT_CAST_REF_TYPE :
        expr = diagnoseCast( ctl, ERR_REINT_CAST_REF_TYPE );
        break;
      case DIAG_REINT_CAST_INT_TYPE :
        expr = diagnoseCast( ctl, ERR_REINT_CAST_INT_TYPE );
        break;
      case DIAG_REINT_CAST_PTR_TYPE :
        expr = diagnoseCast( ctl, ERR_REINT_CAST_PTR_TYPE );
        break;
      case DIAG_REINT_CAST_MPTR_TYPE :
        expr = diagnoseCast( ctl, ERR_REINT_CAST_MPTR_TYPE );
        break;
      case DIAG_REINT_CAST_TYPE :
        expr = diagnoseCast( ctl, ERR_REINT_CAST_TYPE );
        break;
      case DIAG_REINT_TO_INT_TYPE :
        expr = diagnoseCast( ctl, ERR_REINT_TO_INT_TYPE );
        break;
      case DIAG_STATIC_CAST_REF_TYPE :
        expr = diagnoseCast( ctl, ERR_STATIC_CAST_REF_TYPE );
        break;
      case DIAG_STATIC_CAST_PTR_TYPE :
        expr = diagnoseCast( ctl, ERR_STATIC_CAST_PTR_TYPE );
        break;
      case DIAG_STATIC_CAST_MPTR_TYPE :
        expr = diagnoseCast( ctl, ERR_STATIC_CAST_MPTR_TYPE );
        break;
      case DIAG_STATIC_CAST_TYPE :
        expr = diagnoseCast( ctl, ERR_STATIC_CAST_TYPE );
        break;
      case DIAG_STATIC_CAST_EXPR :
        expr = diagnoseCast( ctl, ERR_STATIC_CAST_EXPR );
        break;
      case DIAG_STATIC_CAST_OTHER_TO_ENUM :
        expr = diagnoseCast( ctl, ERR_STATIC_CAST_OTHER_TO_ENUM );
        break;
      case DIAG_DYNAMIC_CAST_TYPE :
        expr = diagnoseCast( ctl, ERR_DYNAMIC_CAST_TYPE );
        break;
      case DIAG_DYNAMIC_CAST_EXPR :
        expr = diagnoseCast( ctl, ERR_DYNAMIC_CAST_EXPR );
        break;
      case DIAG_DYNAMIC_CAST_NO_VFN_SRC :
        PTreeErrorExprType( ctl->expr
                          , ERR_DYNAMIC_CAST_NO_VFN
                          , ctl->src.pted );
        expr = ctl->expr;
        break;
      case DIAG_DYNAMIC_CAST_NO_VFN_TGT :
        PTreeErrorExprType( ctl->expr
                          , ERR_DYNAMIC_CAST_NO_VFN
                          , ctl->tgt.pted );
        expr = ctl->expr;
        break;
      case DIAG_EXPLICIT_CAST_TYPE :
        expr = diagnoseCast( ctl, ERR_EXPLICIT_CAST_TYPE );
        break;
      case DIAG_BASE_AMBIGUOUS :
        expr = diagnoseCast( ctl, ERR_CAST_TO_AMBIGUITY );
        break;
      case DIAG_BASE_PRIVATE :
        expr = diagnoseCast( ctl, ERR_CAST_PRIVATE );
        break;
      case DIAG_BASE_PROTECTED :
        expr = diagnoseCast( ctl, ERR_CAST_PROTECTED );
        break;
      case DIAG_DERIVED_AMBIGUOUS :
        expr = diagnoseCast( ctl, ERR_CAST_FROM_AMBIGUITY );
        break;
      case DIAG_CTOR_AMBIGUOUS :
        CallDiagAmbiguous( ctl->expr, ERR_CTOR_AMBIGUOUS, &ctl->fnov_diag );
        FnovFreeDiag( &ctl->fnov_diag );
        expr = doCastResult( ctl, DIAG_ALREADY );
        break;
      case DIAG_UDC_AMBIGUOUS :
        CallDiagAmbiguous( ctl->expr, ERR_UDC_AMBIGUOUS, &ctl->fnov_diag );
        FnovFreeDiag( &ctl->fnov_diag );
        expr = doCastResult( ctl, DIAG_ALREADY );
        break;
      case DIAG_TO_DERIVED :
        expr = diagnoseCast( ctl, ERR_CONV_BASE_TO_DERIVED );
        break;
      case DIAG_SRC_UNDEFED :
        expr = diagnoseCast( ctl, ERR_CONVERT_FROM_UNDEFD_TYPE );
        break;
      case DIAG_TGT_UNDEFED :
        expr = diagnoseCast( ctl, ERR_CONVERT_TO_UNDEFD_TYPE );
        break;
      case DIAG_TGT_ABSTRACT :
        PTreeErrorExprType( ctl->expr
                          , ERR_CONVERT_TO_ABSTRACT_TYPE
                          , ctl->tgt.class_type );
        ScopeNotePureFunctions( ctl->tgt.class_type );
        expr = doCastResult( ctl, DIAG_ALREADY );
        break;
      case DIAG_MPTR_NOT_DERIVED :
        expr = diagnoseCast( ctl, ERR_MEMB_PTR_NOT_DERIVED );
        break;
      case DIAG_TGT_VOID_STAR :
        expr = diagnoseCast( ctl, ERR_CNV_VOID_STAR );
        break;
      case DIAG_TGT_VOID_CONST_STAR :
        expr = diagnoseCast( ctl, ERR_CNV_VOID_STAR_CONST );
        break;
      case DIAG_TGT_VOID_VOLATILE_STAR :
        expr = diagnoseCast( ctl, ERR_CNV_VOID_STAR_VOLATILE );
        break;
      case DIAG_REF_ADDS_BOTH :
        expr = diagnoseCast( ctl, ERR_REF_CNV_ADDS_BOTH );
        break;
      case DIAG_REF_ADDS_CONST :
        expr = diagnoseCast( ctl, ERR_REF_CNV_ADDS_CONST );
        break;
      case DIAG_REF_ADDS_VOLATILE :
        expr = diagnoseCast( ctl, ERR_REF_CNV_ADDS_VOLATILE );
        break;
      case DIAG_NOT_CONST_REF :
        expr = diagnoseCast( ctl, ERR_NOT_CONST_REF );
        break;
      case DIAG_FROM_BASE_PRIVATE :
        expr = diagnoseCast( ctl, ERR_STATIC_CAST_ACROSS_PRIVATE );
        break;
      case DIAG_FROM_BASE_PROTECTED :
        expr = diagnoseCast( ctl, ERR_STATIC_CAST_ACROSS_PROTECTED );
        break;
      DbgDefault( "doCastResult -- impossible action" );
    }
    if( NodeIsBinaryOp( expr, CO_CONST_CAST )
     || NodeIsBinaryOp( expr, CO_STATIC_CAST )
     || NodeIsBinaryOp( expr, CO_DYNAMIC_CAST )
     || NodeIsBinaryOp( expr, CO_REINTERPRET_CAST ) ) {
         expr->cgop = CO_CONVERT;
         expr = NodeSetType( expr, ctl->tgt.orig, PTF_LV_CHECKED );
    }
    return expr;
}


static PTREE forceToDestination // FORCE TO DESTINATION ON CAST, FUNC_ARG
    ( CONVCTL* ctl )            // - conversion control
{
    PTREE expr;                 // - resultant expression

    if( PT_ERROR == ctl->expr->op ) {
        if( ! ctl->used_destination ) {
            NodeFreeDupedExpr( ctl->destination );
        }
        expr = ctl->expr;
    } else {
        bool fold_it = FALSE;
        bool has_convert = FALSE;
        if( ctl->clscls_implicit
         && ! ctl->keep_cast ) {
            expr = stripOffCastOk( ctl );
        } else {
            expr = ctl->expr;
            if( NodeIsBinaryOp( expr, CO_CONVERT ) ) {
                expr->flags |= expr->u.subtree[1]->flags & ~( PTF_NEVER_PROPPED );
                fold_it = TRUE;
                has_convert = TRUE;
                expr = expr->u.subtree[1];
            }
        }
        if( NULL != ctl->destination ) {
            if( ! ctl->used_destination ) {
                if( fold_it ) {
                    expr = Fold( expr );
                }
                if( ctl->tgt.class_operand ) {
                    expr = ClassDefaultCopyDiag( ctl->destination
                                               , expr
                                               , ctl->diag_cast );
                } else {
                    if( ctl->tgt.reference ) {
                        expr = NodeAssignRef( ctl->destination, expr );
                    } else {
                        expr = NodeAssign( ctl->destination, expr );
                    }
                    if( MemberPtrType( ctl->tgt.unmod ) ) {
                        PTREE dup = expr;
                        MembPtrAssign( &dup );
                        expr = dup;
                    }
                }
            }
            if( ctl->dtor_destination
             && ctl->tgt.class_operand
             && PT_ERROR != expr->op ) {
                expr = NodeDtorExpr( expr, ctl->destination->u.symcg.symbol );
            }
            if( PT_ERROR != expr->op
             && ! ctl->tgt.reference ) {
                expr = NodeRvalue( expr );
            }
            fold_it = FALSE;
        }
        if( has_convert ) {
            ctl->expr->u.subtree[1] = expr;
            if( okSoFar( ctl )
             && RKD_MEMBPTR == ctl->tgt.kind ) {
                stripOffCastOk( ctl );
                fold_it = FALSE;
            }
            expr = ctl->expr;
        }
        if( PT_ERROR != expr->op ) {
            if( fold_it ) {
                expr = Fold( expr );
            }
            if( NodeIsUnaryOp( expr, CO_MEMPTR_CONST )
             && CNV_FUNC_ARG == _CNV_TYPE( ctl->req ) ) {
                expr = MembPtrFuncArg( expr );
            }
        }
    }
    return expr;
}


static void allocClassDestination//ALLOCATE DESTINATION WHEN CLASS
    ( CONVCTL* ctl )            // - conversion control
{
    if( NULL == ctl->destination ) {
        if( ctl->tgt.class_operand
         && ! ctl->tgt.reference ) {
            ctl->destination = NodeTemporary( ctl->tgt.orig );
            ctl->dtor_destination = TRUE;
        }
    }
}


static uint_8 constTable[RKD_MAX][RKD_MAX] = // ranking-combinations table
//      source operand
//      --------------
//         a           c           m       g
//         r   e       l   f   v           e
//     e   i   n   p   a   u   o   p   .   n
//     r   t   u   t   s   n   i   t   .   e
//     r   h   m   r   s   c   d   r   .   r
//                                               target operand
//                                               --------------
    {  1,  1,  1,  1,  1,  1,  1,  1,  0,  0  // error
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // arithmetic
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // enumeration
    ,  1,  3,  3,  5,  3,  3,  3,  3,  0,  0  // pointer
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // class
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // function
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // void
    ,  1,  4,  4,  4,  4,  4,  4,  6,  0,  0  // member pointer
    ,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0  // ellipsis
    ,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0  // generic
    };

//  0 - impossible
//  1 - one operand error ==> reduce to error node
//  2 - type invalid in const_cast<type>
//  3 - source expr not pointer
//  4 - source expr not member pointer
//  5 - const_cast<ptr>( ptr )
//  6 - const_cast<memb-ptr>( memb-ptr )


PTREE CastConst                 // CONST_CASTE< TYPE >( EXPR )
    ( PTREE expr )              // - cast expression
{
    CONVCTL ctl;                // - conversion control
    CAST_RESULT result;         // - cast result

    ConvCtlInitCast( &ctl, expr, &diagConst );
    if( ctl.tgt.reference ) {
        if( ctl.src.reference ) {
            ConvCtlAnalysePoints( &ctl );
            if( ctl.const_cast_ok ) {
                result = CAST_DO_CGCONV;
            } else {
                result = DIAG_CONST_CAST_REF_TYPE;
            }
        } else {
            result = DIAG_CONST_CAST_REF_TYPE;
        }
    } else switch( constTable[ ctl.tgt.kind ][ ctl.src.kind ] ) {
      case  0 : // impossible
        DbgVerify( 0, "ReintCast -- bad selection" );
        result = DIAG_IMPOSSIBLE;
        break;
      case  1 : // an operand is error
        result = CAST_ERR_NODE;
        break;
      case  2 : // illegal type in const_cast<type>
        result = DIAG_CONST_CAST_TYPE;
        break;
      case  3 : // source expr not pointer
        result = DIAG_CONST_CAST_PTR_TYPE;
        break;
      case  4 : // source expr not memb-pointer
        result = DIAG_CONST_CAST_MPTR_TYPE;
        break;
      case  5 : // const_cast<ptr>( ptr )
        ConvCtlAnalysePoints( &ctl );
        if( ctl.const_cast_ok ) {
            result = CAST_DO_CGCONV;
        } else {
            result = DIAG_CONST_CAST_PTR_TYPE;
        }
        break;
      case  6 : // const_cast<memb-ptr>( memb-ptr )
        ConvCtlAnalysePoints( &ctl );
        if( ctl.const_cast_ok ) {
            result = CAST_REINT_MPTR_TO_MPTR;
        } else if( ctl.diff_mptr_class ) {
            result = DIAG_CONST_CAST_MPTR_CLASS;
        } else {
            result = DIAG_CONST_CAST_MPTR_TYPE;
        }
        break;
      default:
        result = CAST_ERR_NODE;
        break;
    }
    return doCastResult( &ctl, result );
}


static uint_8 reintTable[RKD_MAX][RKD_MAX] = // ranking-combinations table
//      source operand
//      --------------
//         a           c           m       g
//         r   e       l   f   v           e
//     e   i   n   p   a   u   o   p   .   n
//     r   t   u   t   s   n   i   t   .   e
//     r   h   m   r   s   c   d   r   .   r
//                                               target operand
//                                               --------------
    {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1  // error
    ,  1,  4,  4,  3,  4,  4,  4,  4,  0,  0  // arithmetic
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // enumeration
    ,  1,  5,  7,  6,  7,  7,  7,  7,  0,  0  // pointer
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // class
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // function
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // void
    ,  1,  9,  9,  9,  9,  9,  9,  8,  0,  0  // member pointer
    ,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0  // ellipsis
    ,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0  // generic
    };

//  0 - impossible
//  1 - one operand error ==> reduce to error node
//  2 - type invalid in reinterpret_cast<type>
//  3 - reinterpret<arith>(ptr)
//  4 - source expression cannot be converted to integral type
//  5 - reinterpret<ptr>(arith)
//  6 - reinterpret<ptr>(ptr)
//  7 - source expression cannot be converted to pointer type
//  8 - reinterpret<memb-ptr>(memb-ptr)
//  9 - source expression cannot be converted to memb-ptr type


PTREE CastReint                 // REINTERPRET_CASTE< TYPE >( EXPR )
    ( PTREE expr )              // - cast expression
{
    CONVCTL ctl;                // - conversion control
    CAST_RESULT result;         // - cast result

    ConvCtlInitCast( &ctl, expr, &diagReint );
    if( ctl.tgt.reference ) {
        if( ctl.src.reference ) {
            ConvCtlAnalysePoints( &ctl );
            if( ctl.reint_cast_ok ) {
                result = CAST_DO_CGCONV;
            } else {
                result = DIAG_REINT_CAST_REF_TYPE;
            }
        } else {
            result = DIAG_REINT_CAST_REF_TYPE;
        }
    } else switch( reintTable[ ctl.tgt.kind ][ ctl.src.kind ] ) {
      case  0 : // impossible
        DbgVerify( 0, "ReintCast -- bad selection" );
        result = DIAG_IMPOSSIBLE;
        break;
      case  1 : // an operand is error
        result = CAST_ERR_NODE;
        break;
      case  2 : // illegal type in reinterpret_cast<type>
        result = DIAG_REINT_CAST_TYPE;
        break;
      case  3 : // reinterpret<arith>(ptr)
        result = CAST_REINT_PTR_TO_ARITH;
        break;
      case  4 : // source expression cannot be converted to integral type
        result = DIAG_REINT_TO_INT_TYPE;
        break;
      case  5 : // reinterpret<ptr>(arith)
        result = CAST_DO_CGCONV;
        break;
      case  6 : // reinterpret<ptr>(ptr)
        ConvCtlAnalysePoints( &ctl );
        if( ctl.reint_cast_ok ) {
            result = CAST_DO_CGCONV;
        } else {
        // drops thru
      case  7 : // source expression cannot be converted to ptr type
            result = DIAG_REINT_CAST_PTR_TYPE;
        }
        break;
      case  8 : // reinterpret<memb-ptr>( memb-ptr )
        ConvCtlAnalysePoints( &ctl );
        if( ctl.reint_cast_ok ) {
            result = CAST_REINT_MPTR_TO_MPTR;
        } else {
            result = DIAG_REINT_CAST_MPTR_TYPE;
        }
        break;
      case  9 : // source expression cannot be converted to memb_ptr type
        result = DIAG_REINT_CAST_MPTR_TYPE;
        break;
      default:
        result = CAST_ERR_NODE;
        break;
    }
    return doCastResult( &ctl, result );
}

static uint_8 staticTable[RKD_MAX][RKD_MAX] = // ranking-combinations table
//      source operand
//      --------------
//         a           c           m       g
//         r   e       l   f   v           e
//     e   i   n   p   a   u   o   p   .   n
//     r   t   u   t   s   n   i   t   .   e
//     r   h   m   r   s   c   d   r   .   r
//                                               target operand
//                                               --------------
    {  1,  1,  1,  1,  1,  1,  1,  1,  0,  0  // error
    ,  1,  4,  4,  3,  0,  3,  3,  3,  0,  0  // arithmetic
    ,  1, 14,  4,  3,  0,  3,  3,  3,  0,  0  // enumeration
    ,  1,  7,  7, 11,  0,  5,  3,  3,  0,  0  // pointer
    ,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0  // class
    ,  1,  2,  2,  2,  0,  2,  2,  2,  0,  0  // function
    ,  1,  6,  6,  6,  0,  6,  6,  6,  0,  0  // void
    ,  1, 10, 10,  3,  0,  3,  3, 13,  0,  0  // member pointer
    ,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0  // ellipsis
    ,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0  // generic
    };

//  0 - impossible
//  1 - one operand error ==> reduce to error node
//  2 - bad static<type>
//  3 - bad expression for static<type>
//  4 - standard conversion
//  5 - func -> ptr
//  6 - cast to cv void
//  7 - zero to ptr
// 10 - zero to memb-ptr
// 11 - ptr -> ptr
// 13 - mptr -> mptr
// 14 - integral -> enum


PTREE CastStatic                // STATIC_CASTE< TYPE >( EXPR )
    ( PTREE expr )              // - cast expression
{
    CONVCTL ctl;                // - conversion control
    CAST_RESULT result;         // - cast result
    uint_8 jump;                // - jump value from table

    ConvCtlInitCast( &ctl, expr, &diagStatic );
    ctl.clscls_copy_init = FALSE;
    ctl.clscls_static = TRUE;
    ctl.clscls_cv = TRUE;
    result = CAST_ERR_NODE;
    switch( ctl.rough ) {
      case CRUFF_CL_TO_CL :
      case CRUFF_SC_TO_CL :
        result = resultToClass( &ctl );
        break;
      case CRUFF_CL_TO_SC :
        result = resultClassToScalar( &ctl );
        break;
      case CRUFF_NO_CL :
        jump = staticTable[ ctl.tgt.kind ][ ctl.src.kind ];
        if( 1 < jump
         && ctl.tgt.reference != ctl.src.reference ) {
            result = DIAG_STATIC_CAST_REF_TYPE;
            break;
        }
        switch( jump ) {
          case  0 : // impossible
            DbgVerify( 0, "ReintCast -- bad selection" );
            result = DIAG_IMPOSSIBLE;
            break;
          case  1 : // an operand is error
            result = CAST_ERR_NODE;
            break;
          case  2 : // illegal type in static_cast<type>
            result = DIAG_STATIC_CAST_TYPE;
            break;
          case  3 : // bad expression for static<type>
            result = DIAG_STATIC_CAST_EXPR;
            break;
          case  4 : // standard conversion
            result = CAST_DO_CGCONV;
            break;
          case  6 : // cast to cv void
            result = CAST_DO_CGCONV;
            break;
          case  7 : // zero to ptr
            if( zeroSrc( &ctl ) ) {
                result = CAST_DO_CGCONV;
            } else {
                result = DIAG_STATIC_CAST_PTR_TYPE;
            }
            break;
          case 10 : // zero to memb-ptr
            if( zeroSrc( &ctl ) ) {
                result = CAST_IMPLICIT_MEMBPTR;
            } else {
                result = DIAG_STATIC_CAST_MPTR_TYPE;
            }
            break;
          case  5 : // func -> ptr
            ConvCtlTypeInit( &ctl
                           , &ctl.src
                           , MakePointerTo( ctl.src.orig ) );
            // drops thru
          case 11 : // ptr -> ptr
            ConvCtlAnalysePoints( &ctl );
            if( ctl.static_cast_ok ) {
                if( ctl.src.pted == ctl.tgt.pted ) {
                    result = CAST_TO_SAME_PTR;
                } else if( ctl.to_base ) {
                    result = CAST_TO_BASE;
                } else if( ctl.to_derived ) {
                    result = CAST_TO_DERIVED;
                } else if( ctl.to_void ) {
                    result = CAST_TO_VOID_PTR;
                } else if( ctl.from_void ) {
                    result = CAST_DO_CGCONV;
                } else {
                    if( ctl.ptr_integral_ext ) {
                        result = DIAG_STATIC_CAST_PTR_TYPE;
                    } else {
                        result = CAST_TO_SAME_PTR;
                    }
                }
            } else {
                result = DIAG_STATIC_CAST_PTR_TYPE;
            }
            break;
          case 13 : // mptr -> mptr
            ConvCtlAnalysePoints( &ctl );
            if( ctl.implicit_cast_ok ) {
                result = CAST_IMPLICIT_MEMBPTR;
            } else if( ctl.static_cast_ok ) {
                result = CAST_STATIC_MEMBPTR;
            } else {
                result = DIAG_STATIC_CAST_MPTR_TYPE;
            }
            break;
          case 14 : // arith -> enum
            if( NULL == IntegralType( ctl.src.unmod ) ) {
                result = DIAG_STATIC_CAST_OTHER_TO_ENUM;
            } else {
                result = CAST_DO_CGCONV;
            }
            break;
        }
        break;
    }
    return doCastResult( &ctl, result );
}


static uint_8 dynamicTable[RKD_MAX][RKD_MAX] = // ranking-combinations table
//      source operand
//      --------------
//         a           c           m       g
//         r   e       l   f   v           e
//     e   i   n   p   a   u   o   p   .   n
//     r   t   u   t   s   n   i   t   .   e
//     r   h   m   r   s   c   d   r   .   r
//                                               target operand
//                                               --------------
    {  1,  1,  1,  1,  1,  1,  1,  1,  0,  0  // error
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // arithmetic
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // enumeration
    ,  1,  3,  3,  4,  3,  3,  3,  3,  0,  0  // pointer
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // class
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // function
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // void
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // member pointer
    ,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0  // ellipsis
    ,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0  // generic
    };

//  0 - impossible
//  1 - one operand error ==> reduce to error node
//  2 - bad dynamic-cast type
//  3 - bad expression for dynamic_cast<type>
//  4 - ptr->ptr (or ref->ref )


PTREE CastDynamic               // DYNAMIC_CASTE< TYPE >( EXPR )
    ( PTREE expr )              // - cast expression
{
    CONVCTL ctl;                // - conversion control
    CAST_RESULT result;         // - cast result

    ConvCtlInitCast( &ctl, expr, &diagDynamic );
    result = CAST_ERR_NODE;
    switch( dynamicTable[ ctl.tgt.kind ][ ctl.src.kind ] ) {
      case  0 : // impossible
        DbgVerify( 0, "DynamicCast -- bad selection" );
        result = DIAG_IMPOSSIBLE;
        break;
      case  1 : // an operand is error
        result = CAST_ERR_NODE;
        break;
      case  2 : // illegal type in static_cast<type>
        result = DIAG_DYNAMIC_CAST_TYPE;
        break;
      case  3 : // bad expression for static<type>
        result = DIAG_DYNAMIC_CAST_EXPR;
        break;
      case  4 : // should be two ptr.s or two ref.s
        if( ctl.tgt.reference != ctl.src.reference ) {
            result = DIAG_DYNAMIC_CAST_EXPR;
            break;
        }
        ConvCtlAnalysePoints( &ctl );
        if( ctl.dynamic_cast_ok ) {
            if( ! TypeDefined( ctl.tgt.pted ) ) {
                result = DIAG_CONVERT_TO_UNDEFD_TYPE;
            } else if( ! TypeDefined( ctl.src.pted ) ) {
                if( ctl.src.reference ) {
                    result = DIAG_CONVERT_FROM_UNDEFD_TYPE;
                } else {
                    result = DIAG_UNDEFD_CLASS_PTR;
                }
            } else {
                if( ctl.tgt.pted->id == TYP_CLASS ) {
                    derived_status status =
                            ScopeDerived( ctl.src.pted->u.c.scope
                                        , ctl.tgt.pted->u.c.scope );
                    switch( status ) {
                      case DERIVED_YES :
                      case DERIVED_YES_BUT_VIRTUAL :
                        result = CAST_TO_BASE;
                        break;
                      default :
                        if( ! ctl.src.pted->u.c.info->has_vfn ) {
                            result = DIAG_DYNAMIC_CAST_NO_VFN_SRC;
                        } else {
                            switch( status ) {
                              case DERIVED_YES_BUT_AMBIGUOUS :
                                diagnoseCast( &ctl
                                            , WARN_DYNAMIC_CAST_AMBIGUOUS );
                                break;
                              case DERIVED_YES_BUT_PROTECTED :
                                diagnoseCast( &ctl
                                            , WARN_DYNAMIC_CAST_PROTECTED );
                                break;
                              case DERIVED_YES_BUT_PRIVATE :
                                diagnoseCast( &ctl
                                            , WARN_DYNAMIC_CAST_PRIVATE );
                                break;
                            }
                            result = CAST_DO_DYNAMIC;
                        }
                        break;
                    }
                } else {
                    if( ! ctl.src.pted->u.c.info->has_vfn ) {
                        result = DIAG_DYNAMIC_CAST_NO_VFN_SRC;
                    } else {
                        result = CAST_DO_DYNAMIC;
                    }
                }
            }
        } else {
            result = DIAG_DYNAMIC_CAST_EXPR;
        }
        break;
    }
    return doCastResult( &ctl, result );
}


static uint_8 explicitTable[RKD_MAX][RKD_MAX] = // ranking-combinations table
//      source operand
//      --------------
//         a           c           m       g
//         r   e       l   f   v           e
//     e   i   n   p   a   u   o   p   .   n
//     r   t   u   t   s   n   i   t   .   e
//     r   h   m   r   s   c   d   r   .   r
//                                               target operand
//                                               --------------
    {  1,  1,  1,  1,  1,  1,  1,  1,  0,  0  // error
    ,  1, 11, 12,  4,  0,  0,  3,  3,  0,  0  // arithmetic
    ,  1, 12, 12,  4,  0,  0,  3,  3,  0,  0  // enumeration
    ,  1,  6,  7,  5,  0,  0,  3,  3,  0,  0  // pointer
    ,  1,  0,  0,  0,  0,  0,  3,  0,  0,  0  // class
    ,  1,  2,  2,  2,  0,  2,  2,  2,  0,  0  // function
    ,  1, 13, 13, 13, 13,  0, 13, 13,  0,  0  // void
    ,  1, 10, 10,  3,  0,  0,  3, 14,  0,  0  // member pointer
    ,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0  // ellipsis
    ,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0  // generic
    };

//  0 - impossible
//  1 - one operand error ==> reduce to error node
//  2 - bad explicit-cast type
//  3 - bad expression for explicit cast
//  4 - ptr to arith
//  5 - ptr to ptr
//  6 - arith -> ptr
//  7 - enum -> ptr
// 10 - zero to memb-ptr
// 11 - arith -> arith
// 12 - enum -> arith, arith -> enum, enum->enum
// 13 - ??? -> void
// 14 - membptr -> membptr
// 99 - do old conversion for now


PTREE CastExplicit              // EXPLICIT CASTE: ( TYPE )( EXPR )
    ( PTREE expr )              // - cast expression
{
    CONVCTL ctl;                // - conversion control
    CAST_RESULT result;         // - cast result
    uint_8 jump;                // - jump code

    ConvCtlInitCast( &ctl, expr, &diagExplicit );
    CErr2p( WARN_C_STYLE_CAST, ctl.tgt.orig );
    ctl.clscls_explicit = TRUE;
    ctl.clscls_derived = TRUE;
    ctl.clscls_refundef = TRUE;
    ctl.clscls_copy_init = FALSE;
    ctl.diag_cast = &diagExplicit;
    allocClassDestination( &ctl );
    result = CAST_ERR_NODE;
    switch( ctl.rough ) {
      case CRUFF_CL_TO_CL :
      case CRUFF_SC_TO_CL :
        result = resultToClass( &ctl );
        break;
      case CRUFF_CL_TO_SC :
        result = resultClassToScalar( &ctl );
        break;
      case CRUFF_NO_CL :
        jump = explicitTable[ ctl.tgt.kind ][ ctl.src.kind ];
        if( jump > 1
         && ctl.tgt.reference != ctl.src.reference ) {
            result = DIAG_CAST_ILLEGAL;
            break;
        }
        switch( jump ) {
          default :
          case  0 : // impossible
            DbgVerify( 0, "ExplictCast -- bad selection" );
            result = DIAG_IMPOSSIBLE;
            break;
          case  1 : // an operand is error
            result = CAST_ERR_NODE;
            break;
          case  2 : // illegal type in explicit-cast type
            result = DIAG_EXPLICIT_CAST_TYPE;
            break;
          case  3 : // bad expression for explicit-cast type
            result = DIAG_CAST_ILLEGAL;
            break;
          case  4 : // (arith)ptr
            if( ctl.tgt.unmod->id == TYP_BOOL ) {
                result = CAST_CONVERT_TO_BOOL;
            } else {
                result = CAST_REINT_PTR_TO_ARITH;
            }
            break;
          case  5 : // (ptr)ptr
            result = analyseExplicitPtrToPtr( &ctl );
            break;
          case  6 : // (ptr)arith
            if( NULL == IntegralType( ctl.src.unmod ) ) {
                result = DIAG_CAST_ILLEGAL;
                break;
            }
            // drops thru
          case  7 : // (ptr)enum
            {   PTREE op = PTreeOp( &ctl.expr->u.subtree[1] );
                if( NodeIsConstantInt( op ) ) {
                    result = CAST_REPLACE_INTEGRAL;
                } else {
                    result = CAST_DO_CGCONV;
                }
            }   break;
          case 10 : // zero to memb-ptr
            if( zeroSrc( &ctl ) ) {
                result = CAST_IMPLICIT_MEMBPTR;
            } else {
                result = DIAG_EXPLICIT_CAST_TYPE;
            }
            break;
          case 11 : // arith -> arith
            result = arithToArith( &ctl );
            break;
          case 12 : // enum -> arith
            result = CAST_DO_CGCONV;
            break;
          case 13 : // ??? -> void
            result = CAST_TO_VOID;
            break;
          case 14 : // mptr -> mptr
            ConvCtlAnalysePoints( &ctl );
            if( ctl.implicit_cast_ok ) {
                result = CAST_IMPLICIT_MEMBPTR;
            } else if( ctl.static_cast_ok ) {
                result = CAST_STATIC_MEMBPTR;
            } else if( ctl.explicit_cast_ok ) {
                result = CAST_EXPLICIT_MPTR_REINT;
            } else {
                result = DIAG_CAST_ILLEGAL;
            }
            break;
        }
        break;
    }
    ctl.expr = doCastResult( &ctl, result );
    return forceToDestination( &ctl );
}


static uint_8 implicitTable[RKD_MAX][RKD_MAX] = // ranking-combinations table
//      source operand
//      --------------
//         a           c           m       g
//         r   e       l   f   v           e
//     e   i   n   p   a   u   o   p   .   n
//     r   t   u   t   s   n   i   t   .   e
//     r   h   m   r   s   c   d   r   .   r
//                                               target operand
//                                               --------------
    {  1,  1,  1,  1,  1,  1,  1,  1,  0,  0  // error
    ,  1, 11, 13,  4,  0,  3,  2,  4,  0,  0  // arithmetic
    ,  1,  7, 12,  3,  0,  3,  2,  3,  0,  0  // enumeration
    ,  1,  6,  6,  5,  0,  0,  2,  3,  0,  0  // pointer
    ,  1,  0,  0,  0,  0,  0,  2,  0,  0,  0  // class
    ,  1,  3,  3,  3,  0,  3,  2,  3,  0,  0  // function
    ,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0  // void
    ,  1, 10, 10,  3,  0,  3,  2, 14,  0,  0  // member pointer
    ,  1,  0,  0,  0,  0,  0,  2,  0,  0,  0  // ellipsis
    ,  1,  0,  0,  0,  0,  0,  2,  0,  0,  0  // generic
    };

//  0 - impossible
//  1 - one operand error ==> reduce to error node
//  2 - bad explicit-cast type
//  3 - bad expression for explicit cast
//  4 - memb-ptr,ptr to arith (only bool is ok)
//  5 - ptr to ptr
//  6 - arith, enum -> ptr
//  7 - arith -> enum
// 10 - zero to memb-ptr
// 11 - arith -> arith
// 12 - enum -> enum
// 13 - enum -> arith
// 14 - membptr -> membptr
// 99 - do old conversion for now


bool CharIsPromoted             // CHECK IF char PROMOTED TO int
    ( TYPE src_type             // - source type
    , TYPE tgt_type )           // - target type
{
    return TYP_CHAR == TypedefModifierRemoveOnly( src_type )->id
        && TYP_SINT == TypedefModifierRemoveOnly( tgt_type )->id;
}


PTREE CheckCharPromotion        // CHECK FOR CHARACTER-TO-INT promotion
    ( PTREE expr )              // - expression
{
    if( CompFlags.plain_char_promotion
     && NULL != expr
     && NULL != expr->u.subtree[0]
     && NULL != expr->u.subtree[1] ) {
        if( CharIsPromoted( expr->u.subtree[1]->type
                          , expr->u.subtree[0]->type ) ) {
            PTreeWarnExpr( expr, WARN_CHAR_PROMOTION );
        }
    }
    return expr;
}


PTREE AddCastNode               // ADD A CAST NODE
    ( PTREE expr                // - expression
    , TYPE type )               // - type
{
#define PTF_CONVERT ( PTF_SIDE_EFF     \
                    | PTF_MEANINGFUL   \
                    | PTF_MEMORY_EXACT \
                    | PTF_PTR_NONZERO  )
    PTF_FLAG flags;             // - flags for cast node
    flags = expr->flags & PTF_CONVERT;
    type = BindTemplateClass( type, &expr->locn, TRUE );
    expr = NodeBinary( CO_CONVERT, PTreeType( type ), expr );
    expr = NodeSetType( expr, type, flags );
    expr = PTreeCopySrcLocation( expr, expr->u.subtree[1] );
    return CheckCharPromotion( expr );
#undef PTF_CONVERT
}


#ifndef NDEBUG
int infinite_ctr;
#define INFINITY 10
#endif


static void setupCastImplicit   // SETUP FOR AN IMPLICIT CAST
    ( PTREE expr                // - expression
    , TYPE type                 // - target type
    , CNV_REQD reqd             // - required kind of conversion
    , CNV_DIAG* diagnosis       // - diagnosis
    , PTREE destination         // - NULL or supplied destination
    , CONVCTL* ctl )            // - conversion control
{
    DbgVerify( ++infinite_ctr <= INFINITY, "Infinite Implicit Conversion LOOP" );
    expr = AddCastNode( expr, type );
    ConvCtlInit( ctl, expr, reqd, diagnosis );
    ctl->destination = destination;
    switch( reqd ) {
      case CNV_FUNC_CD_THIS :
        ctl->clscls_implicit = TRUE;
        break;
      case CNV_FUNC_THIS :
      case CNV_INIT :
        ctl->clscls_cv = TRUE;
        ctl->clscls_implicit = TRUE;
        break;
      case CNV_INIT_COPY :
      case CNV_FUNC_ARG :
      case CNV_FUNC_RET :
      case CNV_FUNC_DARG :
      case CNV_EXPR :
      case CNV_ASSIGN :
        ctl->clscls_copy_init = TRUE;
        ctl->clscls_cv = TRUE;
        ctl->clscls_implicit = TRUE;
        break;
      DbgDefault( "Surprising CNV_... code" );
    }
    if( NULL == ctl->destination ) {
        if( CNV_EXPR == reqd
         || CNV_FUNC_ARG == reqd ) {
            if( ctl->tgt.reference
             || ! ctl->tgt.class_type ) {
                allocClassDestination( ctl );
            }
        } else if( CNV_ASSIGN == reqd
                || CNV_FUNC_DARG == reqd ) {
            allocClassDestination( ctl );
        }
    }
    if( NULL != diagnosis ) {
        ctl->diag_cast = diagnosis;
    }
}



static PTREE doCastImplicit     // DO AN IMPLICIT CAST
    ( PTREE expr                // - expression
    , TYPE type                 // - target type
    , CNV_REQD reqd             // - required kind of conversion
    , CNV_DIAG* diagnosis       // - diagnosis
    , PTREE destination )       // - NULL or supplied destination
{
    CONVCTL ctl;                // - conversion control
    CAST_RESULT result;         // - cast result
    uint_8 jump;                // - jump code

    setupCastImplicit( expr, type, reqd, diagnosis, destination, &ctl );
    result = CAST_ERR_NODE;
    switch( ctl.rough ) {
      case CRUFF_CL_TO_CL :
      case CRUFF_SC_TO_CL :
        result = resultToClass( &ctl );
        break;
      case CRUFF_CL_TO_SC :
        result = resultClassToScalar( &ctl );
        break;
      case CRUFF_NO_CL :
        jump = implicitTable[ ctl.tgt.kind ][ ctl.src.kind ];
        if( jump > 1
         && ctl.tgt.reference != ctl.src.reference ) {
            result = DIAG_CAST_ILLEGAL;
            break;
        }
        switch( jump ) {
          default :
          case  0 : // impossible
            DbgVerify( 0, "ExplictCast -- bad selection" );
            result = DIAG_IMPOSSIBLE;
            break;
          case  1 : // an operand is error
            result = CAST_ERR_NODE;
            break;
          case  2 : // illegal type in explicit-cast type
          case  3 : // bad expression for explicit-cast type
            result = DIAG_CAST_ILLEGAL;
            break;
          case  4 : // (arith)ptr
            if( ctl.tgt.unmod->id == TYP_BOOL ) {
                result = CAST_CONVERT_TO_BOOL;
            } else {
                result = DIAG_CAST_ILLEGAL;
            }
            break;
          case  5 : // (ptr)ptr
            result = analyseImplicitPtrToPtr( &ctl );
            break;
          case  6 : // arith,enum->ptr
            result = implicitArithToPtr( &ctl );
            break;
          case  7 : // arith -> enum
            if( ConvCtlWarning( &ctl, ERR_BAD_ENUM_ASSIGNMENT ) ) {
                result = DIAG_ALREADY;
            } else {
                result = CAST_DO_CGCONV;
            }
            break;
          case 10 : // zero to memb-ptr
            if( zeroSrc( &ctl ) ) {
                result = CAST_IMPLICIT_MEMBPTR;
            } else {
                result = DIAG_CAST_ILLEGAL;
            }
            break;
          case 11 : // arith -> arith
            result = arithToArith( &ctl );
            break;
          case 12 : // enum -> enum
            if( EnumType( ctl.tgt.unmod ) != EnumType( ctl.src.unmod ) ) {
                PTreeErrorExpr( expr, ERR_BAD_ENUM_ASSIGNMENT );
                if( ! okSoFar( &ctl ) ) {
                    result = DIAG_ALREADY;
                    break;
                }
            }
            // drops thru
          case 13 : // enum -> arith
            if( ctl.tgt.unmod->id == TYP_BOOL ) {
                result = CAST_CONVERT_TO_BOOL;
            } else {
                result = CAST_DO_CGCONV;
            }
            break;
          case 14 : // mptr -> mptr
            ConvCtlAnalysePoints( &ctl );
            if( ctl.implicit_cast_ok ) {
                result = CAST_IMPLICIT_MEMBPTR;
            } else if( ctl.bad_mptr_class ) {
                result = DIAG_MPTR_NOT_DERIVED;
            } else {
                result = DIAG_CAST_ILLEGAL;
            }
            break;
        }
    }
    ctl.expr = doCastResult( &ctl, result );
    ctl.expr = forceToDestination( &ctl );
    DbgVerify( --infinite_ctr >= 0, "Bad Implicit Conversion Unwind" );
    return ctl.expr;
}


PTREE CastImplicit              // IMPLICIT CAST
    ( PTREE expr                // - expression
    , TYPE type                 // - target type
    , CNV_REQD reqd             // - required kind of conversion
    , CNV_DIAG* diagnosis )     // - diagnosis
{
    return doCastImplicit( expr, type, reqd, diagnosis, NULL );
}


PTREE CastImplicitRight         // IMPLICIT CAST OF RIGHT OPERAND
    ( PTREE expr                // - expression
    , TYPE type                 // - target type
    , CNV_REQD reqd             // - required kind of conversion
    , CNV_DIAG* diagnosis )     // - diagnosis
{
    PTREE right = PTreeCopySrcLocation( expr->u.subtree[1], expr );
    right = doCastImplicit( right, type, reqd, diagnosis, NULL );
    expr->u.subtree[1] = right;
    if( PT_ERROR == right->op ) {
        PTreeErrorNode( expr );
    } else if( NodeIsBinaryOp( expr, CO_INIT )
            && NULL != TypeReference( type ) ) {
        expr->cgop = CO_INIT_REF;
    }
    return expr;
}


PTREE CopyDirect                // DIRECT COPY
    ( PTREE src                 // - source expression
    , PTREE tgt                 // - target expression
    , TYPE type                 // - type copied
    , CNV_DIAG* diag )          // - diagnosis
{
    return doCastImplicit( src
                         , type
                         , CNV_INIT
                         , diag
                         , tgt );
}


PTREE CopyInit                  // INITIALIZATION COPY
    ( PTREE src                 // - source expression
    , PTREE tgt                 // - target expression
    , TYPE type                 // - type copied
    , CNV_DIAG* diag )          // - diagnosis
{
    return doCastImplicit( src
//                       , MakeReferenceTo( type )
                         , type
                         , CNV_INIT_COPY
                         , diag
                         , tgt );
}


PTREE CopyClassRetnVal          // COPY TO RETURN SYMBOL A CLASS VALUE
    ( PTREE src                 // - source expression
    , PTREE tgt                 // - target expression
    , TYPE type                 // - type copied
    , CNV_DIAG* diag )          // - diagnosis
{
    return doCastImplicit( src
                         , type
                         , CNV_FUNC_RET
                         , diag
                         , tgt );
}


static TYPE commonDiag          // PRINT DIAGNOSIS MESSAGE
    ( PTREE expr                // - original expression
    , unsigned msg              // - error message
    , TYPE src                  // - source type
    , TYPE tgt )                // - target type
{
    if( PTreeErrorExpr( expr, msg ) & MS_PRINTED ) {
        ConversionTypesSet( src, tgt );
        ConversionDiagnoseInf();
    }
    return NULL;
}


static CNV_DIAG diagImpossible  // DIAGNOSIS FOR IMPOSSIBLE CONVERT FAILURE
=   { ERR_CALL_WATCOM
    , ERR_CALL_WATCOM
    , ERR_CALL_WATCOM
    , ERR_CALL_WATCOM
    , ERR_CALL_WATCOM
    };


void ConvCtlInitCommon          // INITIALIZE CONVCTL FOR COMMON-TYPE'ING
    ( CONVCTL* ctl              // - control info.
    , PTREE expr                // - expression
    , CNV_DIAG* diag )          // - diagnosis
{
    ctl->req = CNV_EXPR;
    ctl->expr = expr;
    ctl->diag_good = &diagImpossible;
    if( diag == NULL ) {
        diag = &diagImpossible;
    }
    ctl->diag_cast = diag;
    ctl->conv_fun = NULL;
    ctl->conv_type = NULL;
    ctl->destination = NULL;
    #define CONVCTL_FLAG( flag ) ctl->flag = FALSE;
    CONVCTL_FLAGS
    #undef CONVCTL_FLAG
    ctl->ctd = 0;
    ctl->rough = CRUFF_NO_CL;
    ConvCtlTypeInit( ctl, &ctl->tgt, expr->u.subtree[0]->type );
    ConvCtlTypeInit( ctl, &ctl->src, expr->u.subtree[1]->type );
    if( ctl->has_err_operand ) {
        ctl->src.kind = RKD_ERROR;
        ctl->tgt.kind = RKD_ERROR;
    }
}


static PTREE commonCastEx       // EXPLICIT CAST FOR COMMON
    ( PTREE expr                // - expression to be casted
    , TYPE final_type )         // - type for cast
{
    PTREE retn;                 // - converted expression

    if( final_type == expr->type ) {
        retn = expr;
    } else {
        expr = AddCastNode( expr, final_type );
        expr = CastExplicit( expr );
        DbgVerify( expr->op != PT_ERROR, "Unexpected failure" );
        if( NodeIsBinaryOp( expr, CO_CONVERT )
         && final_type == NodeType( expr->u.subtree[1] ) ) {
            PTreeFree( expr->u.subtree[0] );
            expr->u.subtree[0] = NULL;
            retn = expr->u.subtree[1];
            PTreeFree( expr );
        } else {
            retn = expr;
        }
    }
    return retn;
}


// This routine is called to convert the left and right pointers to one
// of the other (it is used in comparison and colon operators).  The
// operator-overloading mechanism has already converted from class operands
// if required. The only allowed conversions are to base-pointers and to
// void*.  Comparisons ignore the CV qualification of the pointer.
//
PTREE CastImplicitCommonPtrExpr // CONVERT TO COMMON PTR EXPRESSION
    ( PTREE expr                // - the expression
    , CNV_DIAG *diagnosis       // - diagnosis
    , bool check_cv )           // - TRUE ==> check CV QUALIFICATION
{
    TYPE final_type;            // - final type
    CONVCTL ctl;                // - controller

    final_type = NULL;
    ConvCtlInitCommon( &ctl, expr, diagnosis );
    ConvCtlAnalysePoints( &ctl );
    switch( ctl.ctd ) {
      case CTD_LEFT_AMBIGUOUS :
        final_type = commonDiag( expr
                               , diagnosis->msg_ambiguous
                               , ctl.tgt.unmod
                               , ctl.src.unmod );
        break;
      case CTD_LEFT_PRIVATE :
        final_type = commonDiag( expr
                               , diagnosis->msg_private
                               , ctl.tgt.unmod
                               , ctl.src.unmod );
        break;
      case CTD_LEFT_PROTECTED :
        final_type = commonDiag( expr
                               , diagnosis->msg_protected
                               , ctl.tgt.unmod
                               , ctl.src.unmod );
        break;
      case CTD_LEFT :
      case CTD_LEFT_VIRTUAL :
        if( ctl.implicit_cast_ok ) {
            final_type = ctl.tgt.unmod;
        } else if( ctl.cv_mismatch ) {
            final_type = ctl.src.unmod;
        } else {
            final_type = ctl.tgt.unmod;
        }
        break;
      case CTD_RIGHT_AMBIGUOUS :
        final_type = commonDiag( expr
                               , diagnosis->msg_ambiguous
                               , ctl.src.unmod
                               , ctl.tgt.unmod );
        break;
      case CTD_RIGHT_PRIVATE :
        final_type = commonDiag( expr
                               , diagnosis->msg_private
                               , ctl.src.unmod
                               , ctl.tgt.unmod );
        break;
      case CTD_RIGHT_PROTECTED :
        final_type = commonDiag( expr
                               , diagnosis->msg_protected
                               , ctl.src.unmod
                               , ctl.tgt.unmod );
        break;
      case CTD_RIGHT :
      case CTD_RIGHT_VIRTUAL :
        final_type = ctl.src.unmod;
        break;
      case CTD_NO :
        if( ctl.implicit_cast_ok ) {
            final_type = ctl.tgt.unmod;
        } else if( TYP_VOID == ctl.src.pted->id ) {
            final_type = ctl.src.unmod;
        } else if( ctl.src.pted->id == ctl.tgt.pted->id
                && ctl.src.pted->id != TYP_CLASS ) {
            type_flag src_flag = ctl.src.ptedflags & TF1_CV_MASK;
            type_flag tgt_flag = ctl.tgt.ptedflags & TF1_CV_MASK;
            if( tgt_flag == ( src_flag & tgt_flag ) ) {
                final_type = ctl.src.unmod;
            } else {
                final_type = ctl.tgt.unmod;
            }
        } else {
            PTreeErrorExpr( expr, diagnosis->msg_impossible );
            InfMsgPtr( INF_LEFT_OPERAND_TYPE, ctl.tgt.unmod );
            InfMsgPtr( INF_RIGHT_OPERAND_TYPE, ctl.src.unmod );
            final_type = NULL;
        }
        break;
    }
    if( NULL != final_type ) {
        if( check_cv ) {
            if( final_type != ctl.tgt.unmod ) {
                expr->u.subtree[0] = CastImplicit( expr->u.subtree[0]
                                                 , final_type
                                                 , CNV_EXPR
                                                 , diagnosis );
            }
            if( final_type != ctl.src.unmod ) {
                expr->u.subtree[1] = CastImplicit( expr->u.subtree[1]
                                                 , final_type
                                                 , CNV_EXPR
                                                 , diagnosis );
            }
        } else {
            expr->u.subtree[0] = commonCastEx( expr->u.subtree[0]
                                             , final_type );
            expr->u.subtree[1] = commonCastEx( expr->u.subtree[1]
                                             , final_type );
        }
        if( PT_ERROR == expr->u.subtree[0]->op
         || PT_ERROR == expr->u.subtree[1]->op ) {
            PTreeErrorNode( expr );
        }
    }
    if( PT_ERROR != expr->op ) {
        expr = NodeSetType( expr, final_type, PTF_LV_CHECKED );
    }
    return expr;
}


static CAST_RESULT castCommon   // ATTEMPT A COMMON-CAST
    ( PTREE expr                // - expression
    , TYPE type                 // - target type
    , CNV_DIAG* diagnosis       // - diagnosis
    , CONVCTL* ctl )            // - conversion control
{
    CAST_RESULT result;         // - cast result

    setupCastImplicit( expr, type, CNV_EXPR, diagnosis, NULL, ctl );
    switch( ctl->rough ) {
      case CRUFF_CL_TO_CL :
      case CRUFF_SC_TO_CL :
        result = resultToClass( ctl );
        break;
      case CRUFF_CL_TO_SC :
        result = resultClassToScalar( ctl );
        break;
      default :
        DbgVerify( ctl->src.kind == RKD_ERROR
                || ctl->tgt.kind == RKD_ERROR
                 , "castCommon -- funny types" );
        result = CAST_ERR_NODE;
    }
    return result;
}


static PTREE castCommonExpr     // CAST TO COMMON EXPRESSION
    ( CONVCTL* ctl              // - control
    , CAST_RESULT result )      // - type of conversion to perform
{
    ctl->expr = doCastResult( ctl, result );
    ctl->expr = forceToDestination( ctl );
    return ctl->expr;
}


#define castCommonOk( result )  \
    ( result ) != DIAG_CAST_ILLEGAL


// The following is called only when extensions are enabled.
//
// One or both of the operands are class values.
//
bool CastCommonClass            // CAST (IMPLICITLY) TO A COMMON CLASS
    ( PTREE* a_expr             // - addr[ expression ]
    , CNV_DIAG* diagnosis )     // - diagnosis
{
    bool retn;                  // - return: TRUE ==> converted or diagnosed
    PTREE expr;                 // - expression
    CAST_RESULT result_left;    // - result of cast to left
    CAST_RESULT result_right;   // - result of cast to right
    CONVCTL ctl_left;           // - control for cast to left
    CONVCTL ctl_right;          // - control for cast to right

    expr = *a_expr;
    result_left = castCommon( expr->u.subtree[1]
                            , NodeType( expr->u.subtree[0] )
                            , diagnosis
                            , &ctl_left );
    result_right = castCommon( expr->u.subtree[0]
                             , NodeType( expr->u.subtree[1] )
                             , diagnosis
                             , &ctl_right );
    if( result_right == CAST_ERR_NODE || result_left == CAST_ERR_NODE ) {
        stripOffCastOrig( &ctl_left );
        stripOffCastOrig( &ctl_right );
        PTreeErrorNode( expr );
        retn = TRUE;
    } else if( castCommonOk( result_left ) ) {
        if( castCommonOk( result_right ) ) {
            if( result_right >= DIAGNOSIS_START || result_left  >= DIAGNOSIS_START ) {
                expr->u.subtree[0] = castCommonExpr( &ctl_right, result_right );
                expr->u.subtree[1] = castCommonExpr( &ctl_left, result_left );
                PTreeErrorNode( expr );
            } else {
                stripOffCastOrig( &ctl_left );
                stripOffCastOrig( &ctl_right );
                PTreeErrorExpr( expr, ERR_UDC_AMBIGUOUS );
                InfSymbolAmbiguous( ctl_left.conv_fun );
                InfSymbolAmbiguous( ctl_right.conv_fun );
            }
            retn = TRUE;
        } else {
            expr->type = ctl_left.tgt.orig;
            stripOffCastOrig( &ctl_right );
            expr->u.subtree[1] = castCommonExpr( &ctl_left, result_left );
            retn = TRUE;
        }
    } else {
        if( castCommonOk( result_right ) ) {
            expr->type = ctl_right.tgt.orig;
            stripOffCastOrig( &ctl_left );
            expr->u.subtree[0] = castCommonExpr( &ctl_right, result_right );
            retn = TRUE;
        } else {
            stripOffCastOrig( &ctl_left );
            stripOffCastOrig( &ctl_right );
            retn = FALSE;
        }
    }
    *a_expr = expr;
    DbgVerify( --infinite_ctr >= 0, "Bad Implicit Conversion Unwind" );
    return retn;

}
