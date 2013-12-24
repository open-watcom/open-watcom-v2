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
#include "fnovload.h"
#include "cgsegid.h"
#include "defarg.h"
#include "class.h"
#include "objmodel.h"
#include "ptrcnv.h"
#include "stats.h"
#include "initdefs.h"


static unsigned get_ptr_cv(     // GET CONST/VOLATILE POINTED AT
    TYPE type )                 // - type
{
    TYPE ptype;                 // - pointer base type
    TYPE mtype;                 // - member pointer base type
    type_flag flags;            // - modifier flags

    ptype = PointerTypeEquivalent( type );
    if( ptype != NULL ) {
        TypeModFlagsEC( ptype->of, &flags );
        flags &= TF1_CV_MASK;
    } else {
        mtype = MemberPtrType( type );
        if( mtype != NULL ) {
            TypeModFlagsEC( mtype->of, &flags );
            flags &= TF1_CV_MASK;
        } else {
            flags = 0;
        }
    }
    return flags;
}


unsigned AnalysePtrCV(          // CHECK PTR CONVERSION FOR CONST/VOLATILE
    PTREE expr,                 // - expression for error
    TYPE proto,                 // - type of target
    TYPE argument,              // - type of source
    unsigned conversion )       // - type of conversion
{
    unsigned retn;              // - return: CNV_...
    unsigned cv_proto;          // - const/volatile for prototype
    unsigned cv_argument;       // - const/volatile for argument
    TYPE refed;                 // - a reference type

    refed = TypeReference( proto );
    if( refed == NULL ) {
        refed = TypeReference( argument );
        if( refed != NULL ) {
            argument = refed;
        }
    }
    cv_proto = get_ptr_cv( proto );
    cv_argument = get_ptr_cv( argument );
    if( TF1_CONST & cv_argument & ~ cv_proto ) {
        switch( conversion ) {
          case CNV_INIT :
          case CNV_INIT_COPY :
            if( NULL == TypeReference( proto ) ) {
                PTreeErrorExpr( expr, ERR_CONST_PTR_INIT );
            } else {
                PTreeErrorExpr( expr, ERR_CONST_REF_INIT );
            }
            retn = CNV_ERR;
            break;
          case CNV_FUNC_DARG :
          case CNV_FUNC_ARG :
            PTreeErrorExpr( expr, ERR_CONST_PTR_ARG );
            retn = CNV_ERR;
            break;
          case CNV_FUNC_THIS :
            PTreeErrorExpr( expr, ERR_CONST_PTR_THIS );
            retn = CNV_ERR;
            break;
          case CNV_FUNC_RET :
            PTreeErrorExpr( expr, ERR_CONST_PTR_RETURN );
            retn = CNV_ERR;
            break;
          default :
            retn = CNV_OK;
            break;
        }
    } else {
        retn = CNV_OK;
    }
    if( ( retn == CNV_OK )
      &&( TF1_VOLATILE & cv_argument & ~ cv_proto ) ) {
        switch( conversion ) {
          case CNV_INIT :
          case CNV_INIT_COPY :
            PTreeErrorExpr( expr, ERR_VOLATILE_PTR_INIT );
            retn = CNV_ERR;
            break;
          case CNV_FUNC_DARG :
          case CNV_FUNC_ARG :
            PTreeErrorExpr( expr, ERR_VOLATILE_PTR_ARG );
            retn = CNV_ERR;
            break;
          case CNV_FUNC_THIS :
            PTreeErrorExpr( expr, ERR_VOLATILE_PTR_THIS );
            retn = CNV_ERR;
            break;
          case CNV_FUNC_RET :
            PTreeErrorExpr( expr, ERR_VOLATILE_PTR_RETURN );
            retn = CNV_ERR;
            break;
          default :
            retn = CNV_OK;
            break;
        }
    }
    return( retn );
}


#ifdef XTRA_RPT

ExtraRptTable( cnvKind, 20, 1 ); // counts converted cases
ExtraRptTable( cnvType, 13, 1 ); // counts "conversion"

static void init(               // START OF RANK REPORTING
    INITFINI* defn )            // - definition
{
    defn = defn;
    ExtraRptRegisterTab( "Conversion Kinds"
                       , NULL
                       , &cnvKind[0][0]
                       , sizeof( cnvKind ) / sizeof( cnvKind[0] )
                       , 1);
    ExtraRptRegisterTab( "Conversion Types"
                       , NULL
                       , &cnvType[0][0]
                       , sizeof( cnvType ) / sizeof( cnvType[0] )
                       , 1);
}


INITDEFN( cnv_reports, init, InitFiniStub );

#endif


unsigned ConvertOvFunNode(      // CONVERT FUN (FUN IS OVERLOADED), NO FREE
    TYPE tgt,                   // - target type
    PTREE func )                // - overloaded function
{
    unsigned retn;              // - conversion return
    SEARCH_RESULT *result;      // - previous search result
    SYMBOL sym;                 // - symbol for function
    TYPE points;                // - unmodified pointer to function
    FNOV_RESULT ov_retn;        // - return from overloading

    retn = CNV_IMPOSSIBLE;
    points = PointerTypeEquivalent( tgt );
    if( points != NULL ) {
        points = FunctionDeclarationType( points->of );
        if( points != NULL ) {
            sym = func->u.symcg.symbol;
            if( SymIsUDC( sym ) ) {
                ov_retn = UdcOverloaded( &sym
                                       , func->u.symcg.result
                                       , sym
                                       , points->of
                                       , points->u.f.args->qualifier );
            } else {
                ov_retn = FuncOverloadedLimitDiag( &sym
                                                 , func->u.symcg.result
                                                 , sym
                                                 , points->u.f.args
                                                 , NULL
                                                 , FNC_EXCLUDE_DEFARG
                                                 , NULL
                                                 , NULL );
            }
            switch( ov_retn ) {
              case FNOV_AMBIGUOUS :
              case FNOV_NO_MATCH :
                break;
              case FNOV_NONAMBIGUOUS :
                result = func->u.symcg.result;
                if( ScopeCheckSymbol( result, sym ) ) {
                    retn = CNV_ERR;
                } else {
                    DbgAssert( sym->id != SC_DEFAULT );
                    if( sym->id != SC_DEFAULT ) {
                        func->u.symcg.symbol = sym;
                        func->type = sym->sym_type;
                        retn = CNV_OK;
                    }
                }
                break;
              DbgDefault( "ConvertOvFunNode: unexpected return" );
            }
        }
    }
    return( retn );
}


static unsigned diagnoseCommon( // DIAGNOSE A COMMON CONVERSION
    CTD ctd,                    // - derivation type
    CNV_DIAG *diagnosis,        // - diagnosis
    PTREE expr )                // - common expression
{
    unsigned retn;              // - conversion return: CNV_...
    TYPE left;                  // - type of operand on left
    TYPE right;                 // - type of operand on right

    retn = CNV_IMPOSSIBLE;
    switch( ctd ) {
      case CTD_RIGHT :
      case CTD_RIGHT_VIRTUAL :
      case CTD_NO :
      case CTD_LEFT :
      case CTD_LEFT_VIRTUAL :
        break;
      case CTD_LEFT_AMBIGUOUS :
      case CTD_RIGHT_AMBIGUOUS :
        retn = CNV_AMBIGUOUS;
        break;
      case CTD_LEFT_PRIVATE :
      case CTD_RIGHT_PRIVATE :
        retn = CNV_PRIVATE;
        break;
      case CTD_LEFT_PROTECTED :
      case CTD_RIGHT_PROTECTED :
        retn = CNV_PROTECTED;
        break;
    }
    left = NodeType( expr->u.subtree[0] );
    right = NodeType( expr->u.subtree[1] );
    ConversionInfDisable();
    ConversionDiagnose( retn, expr, diagnosis );
    ConversionDiagLR( left, right );
    return CNV_ERR;
}

static boolean convertCommonClass(// CONVERT TO COMMON TYPE, FROM CLASS
    PTREE *a_expr,              // - binary expression
    CNV_DIAG *diagnosis )       // - used to diagnose errors
{
    boolean cretn;              // - TRUE ==> conversion handled
    PTREE expr;                 // - expression
    PTREE *a_cnv = NULL;        // - converted subtree
    PTREE cnv;                  // - converted subtree
    CTD ctd;                    // - common-type derivation
    TYPE tgt_type = NULL;       // - target type

    expr = *a_expr;
    ctd = TypeCommonDerivation( expr->u.subtree[0]->type, expr->u.subtree[1]->type );
    switch( ctd ) {
      case CTD_NO :
        if( CastCommonClass( a_expr, diagnosis ) ) {
            return TRUE;
        }
        // drops thru
      case CTD_LEFT_AMBIGUOUS :
      case CTD_LEFT_PRIVATE :
      case CTD_LEFT_PROTECTED :
      case CTD_RIGHT_AMBIGUOUS :
      case CTD_RIGHT_PRIVATE :
      case CTD_RIGHT_PROTECTED :
        diagnoseCommon( ctd, diagnosis, expr );
        break;
      case CTD_LEFT :
      case CTD_LEFT_VIRTUAL :
        a_cnv = &expr->u.subtree[0];
        tgt_type = expr->u.subtree[1]->type;
        break;
      case CTD_RIGHT :
      case CTD_RIGHT_VIRTUAL :
        a_cnv = &expr->u.subtree[1];
        tgt_type = expr->u.subtree[0]->type;
        break;
    }
    if( PT_ERROR == expr->op ) {
        cretn = FALSE;
    } else {
        cnv = CastImplicit( *a_cnv, tgt_type, CNV_EXPR, diagnosis );
        *a_cnv = cnv;
        if( PT_ERROR == cnv->op ) {
            PTreeErrorNode( expr );
            cretn = FALSE;
        } else {
            expr = NodeSetType( expr, cnv->type, PTF_LV_CHECKED );
            cretn = TRUE;
        }
    }
    *a_expr = expr;
    return cretn;
}


static boolean nodeMemberPtr(   // TEST IF NODE IS MEMB-PTR
    PTREE node )                // - NODE
{
    return NULL != MemberPtrType( TypeReferenced( node->type ) );
}


boolean ConvertCommonType(      // CONVERT TO COMMON TYPE (:, ==, !=)
    PTREE *a_expr,              // - addr [ expression ]
    CNV_DIAG *diag_class,       // - diagnosis: class
    CNV_DIAG *diag_mem_ptr )    // - diagnosis: member ptr.
{
    boolean retn;               // - FALSE ==> diagnose bad operands
    PTREE expr;                 // - expression

    expr = *a_expr;
    expr->u.subtree[0]->type =
        BindTemplateClass( expr->u.subtree[0]->type, &expr->locn, TRUE );
    expr->u.subtree[1]->type =
        BindTemplateClass( expr->u.subtree[1]->type, &expr->locn, TRUE );
    if( NULL != StructType( expr->u.subtree[0]->type )
     || NULL != StructType( expr->u.subtree[1]->type ) ) {
        retn = convertCommonClass( a_expr, diag_class );
    } else
    if( nodeMemberPtr( expr->u.subtree[0] )
     || nodeMemberPtr( expr->u.subtree[1] ) ) {
        expr->u.subtree[0] = NodeRvalueLeft( expr );
        expr->u.subtree[1] = NodeRvalueRight( expr );
        expr = MembPtrCommonType( expr );
        *a_expr = expr;
        if( PT_ERROR == expr->op ) {
            ConversionDiagnose( CNV_ERR, expr, diag_mem_ptr );
        }
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return retn;
}
