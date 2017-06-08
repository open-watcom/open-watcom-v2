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
#include "ptrcnv.h"

enum                            // DEFINE CONVERSION TO BE DONE
{   CNV_DER_BASE                // - derived -> non-virtual base
,   CNV_DER_VIRT                // - derived -> virtual base
,   CNV_BASE_DER                // - non-virtual base -> derived
,   CNV_ERR_NO_DERIVATION       // - error: no derivation exists
,   CNV_ERR_VIRT_DER            // - error: virtual base -> derived
,   CNV_ERR_AMBIGUOUS           // - error: derivation is ambiguous
,   CNV_ERR_PRIVATE             // - error: derivation is thru a private base
,   CNV_ERR_PROTECTED           // - error: derivation is thru a protected base
};


static TYPE type_pointed_to(    // GET BASE TYPE POINTED TO
    TYPE type )                 // - the type
{
    type_flag not_used;         // - not used

    return TypePointedAt( type, &not_used );
}


static bool same_ptr_types(     // TEST FOR EQUIVALENT POINTER TYPES
    TYPE t1,                    // - type [1]
    TYPE t2 )                   // - type [2]
{
    return TypeCompareExclude( type_pointed_to( t1 )
                             , type_pointed_to( t2 )
                             , TC1_FUN_LINKAGE );
}


static bool ptr_to_void(        // TEST IF PTR TYPE IS POINTER TO VOID
    TYPE type )                 // - the type
{
    return type_pointed_to( type )->id == TYP_VOID;
}


static PTREE adjust_base_ptr(   // ADJUST BASE PTR AS true/TESTING CONVERSION
    PTREE orig,                 // - original node
    PTREE offset,               // - offset calculation
    TYPE type )                 // - type, after conversion
{
    if( orig->flags & PTF_PTR_NONZERO ) {
        orig = NodeMakeBinary( CO_DOT, orig, offset );
    } else {
        orig = NodeMakeBinary( CO_PTR_DELTA, orig, offset );
    }
    orig->type = type;
    return orig;
}

PTREE NodeConvertVirtualPtr(    // EXECUTE A VIRTUAL BASE CAST
    PTREE expr,                 // - expr to cast
    TYPE final_type,            // - final type after cast
    target_offset_t vb_offset,  // - offset of vbptr
    vindex vb_index )           // - index in vbtable
{
    PTREE offset;
    TYPE vbptr_type;
    TYPE adjust_type;
    PTREE dup;

    vbptr_type = MakeVBTableFieldType( true );
    offset = NodeMakeConstantOffset( vb_offset );
    expr = NodeMakeBinary( CO_DOT, expr, offset );
    expr->flags |= PTF_LVALUE | PTF_PTR_NONZERO;
    expr->type = vbptr_type;
    dup = NodeMakeExprDuplicate( &expr );
    expr->flags |= PTF_LVALUE | PTF_PTR_NONZERO;
    expr = NodeFetch( expr );
    expr->type = vbptr_type;
    expr->flags &= ~ PTF_LVALUE;
    expr->flags |= PTF_PTR_NONZERO;
    adjust_type = TypePointedAtModified( vbptr_type );
    offset = NodeMakeConstantOffset( vb_index * CgMemorySize( adjust_type ) );
    expr = NodeMakeBinary( CO_DOT, expr, offset );
    expr->type = adjust_type;
    expr->flags |= PTF_LVALUE | PTF_PTR_NONZERO;
    expr = NodeFetch( expr );
    expr->type = adjust_type;
    expr->flags |= PTF_PTR_NONZERO;
    expr = NodeMakeBinary( CO_DOT, dup, expr );
    expr->type = final_type;
    expr->flags |= PTF_LVALUE | PTF_PTR_NONZERO;
    return expr;
}


static void adjust_by_delta(    // COMPUTE DELTA ADJUSTMENT
    PTREE *a_expr,              // - addr( ptr to be converted )
    TYPE base,                  // - base type
    unsigned delta,             // - adjustment
    bool positive )             // - true ==> use positive value
{
    PTREE offset;               // - node containing delta

    if( delta == 0 ) {
        *a_expr = NodeMakeConversion( base, *a_expr );
    } else {
        offset = NodeMakeConstantOffset( delta );
        if( ! positive ) {
            offset->u.int_constant = - delta;
        }
        *a_expr = adjust_base_ptr( *a_expr, offset, base );
    }
}


void NodeConvertToBasePtr(      // CONVERT TO A BASE PTR, USING SEARCH_RESULT
    PTREE *a_expr,              // - addr( ptr to be converted )
    TYPE base,                  // - base type
    SEARCH_RESULT *result,      // - search result
    bool positive )             // - true ==> use positive value
{
    target_offset_t vb_offset;  // - offset of vbptr
    vindex vb_index;            // - index in vbtable
    target_offset_t delta;      // - delta for class
    PTREE node;                 // - new node
    PTREE dup;                  // - node containing duplicate of original
    PTREE orig;                 // - original value
    TYPE ref_type;              // - reference type for original value

    node = *a_expr;
    /* references can never be NULL by definition */
    ref_type = TypeReference( NodeType( node ) );
    if( ref_type != NULL ) {
        node->flags |= PTF_PTR_NONZERO;
    }
    if( node->flags & PTF_MEMORY_EXACT ) {
        adjust_by_delta( a_expr, base, result->exact_delta, positive );
    } else if( result->non_virtual ) {
        adjust_by_delta( a_expr, base, result->delta, positive );
    } else {
        PTF_FLAG orig_prop;     // - flags propogated from original
        SYMBOL ibp;             // - inline bound reference parameter
        target_offset_t offset; // - offset to ibp
#if 0
        orig_prop = ( node->flags & PTF_FETCH ) | PTF_LVALUE;
#else
        orig_prop = node->flags & PTF_FETCH;
        if( NULL != ref_type ) {
            orig_prop |= PTF_LVALUE;
        }
#endif
        if( NodeGetIbpSymbol( node, &ibp, &offset ) ) {
            PTREE expr;         // - expression under construction
            unsigned vb_exact;  // - exact offset for conversion
            vb_exact = result->exact_delta;
            if( ! positive ) {
                vb_exact = - vb_exact;
            }
            vb_exact += offset;
            if( NULL == ibp ) {
                expr = NULL;
            } else {
                expr = NodeMakeCallee( ibp );
                expr->cgop = CO_IGNORE;
            }
            expr = NodeMakeUnary( CO_VBASE_FETCH, expr );
            expr = PtdVbaseFetch( expr
                                , result->vb_offset
                                , result->vb_index
                                , result->delta + offset
                                , vb_exact );
            expr->type = base;
            expr->flags = orig_prop;
            node = NodeReplace( node, expr );
        } else {
            dup = NULL;
            if( ! NodeIsNonNullPtr( node ) ) {
                dup = NodeMakeExprDuplicate( &node );
            }
            vb_offset = result->vb_offset;
            vb_index = result->vb_index;
            node = NodeConvertVirtualPtr( node, base, vb_offset, vb_index );
            delta = result->delta;
            if( delta != 0 ) {
                node = NodeMakeBinary( CO_DOT, node, NodeMakeConstantOffset( delta ) );
                node->type = base;
                node->flags |= orig_prop;
            }
            if( dup != NULL ) {
                orig = NodeMakeExprDuplicate( &dup );
                node = NodeMakeTernaryExpr( NodeMakeZeroCompare( orig ), node, dup );
            }
        }
        *a_expr = node;
    }
    if( ref_type != NULL ) {
        (*a_expr)->flags |= PTF_LVALUE;
    }
}



static TYPE convert_base_ptr(   // CONVERT TO A BASE-CLASS POINTER
    PTREE *expr,                // - addr( ptr to be converted )
    TYPE base,                  // - base type
    SCOPE derived_scope,        // - derived scope
    SCOPE base_scope,           // - base scope
    bool positive )             // - true ==> use positive value
{
    SEARCH_RESULT *result;

    result = ScopeBaseResult( derived_scope, base_scope );
    NodeConvertToBasePtr( expr, base, result, positive );
    ScopeFreeResult( result );
    return base;
}


TYPE NodeConvertDerivedToBase(  // CONVERT DERIVED PTR TO NONVIRTUAL BASE PTR
    PTREE *expr,                // - addr( ptr to be converted )
    TYPE tgt,                   // - derived type
    SCOPE derived_scope,        // - derived scope
    SCOPE base_scope )          // - base scope
{
    return convert_base_ptr( expr, tgt, derived_scope, base_scope, true );
}


static TYPE NodeConvertDerivedToVirt(  // CONVERT DERIVED PTR TO VIRTUAL BASE PTR
    PTREE *expr,                // - addr( ptr to be converted )
    TYPE tgt,                   // - derived type
    SCOPE derived_scope,        // - derived scope
    SCOPE base_scope )          // - base scope
{
    return convert_base_ptr( expr, tgt, derived_scope, base_scope, true );
}


TYPE NodeConvertBaseToDerived(  // CONVERT BASE PTR TO DERIVED PTR
    PTREE *expr,                // - addr( ptr to be converted )
    TYPE tgt,                   // - derived type
    SCOPE derived_scope,        // - derived scope
    SCOPE base_scope )          // - base scope
{
    return convert_base_ptr( expr, tgt, derived_scope, base_scope, false );
}


void NodeWarnPtrTrunc(          // WARN FOR POINTER/REFERENCE TRUNCATION
    PTREE node )                // - node for warning
{
    PTreeWarnExpr( node, WARN_POINTER_TRUNCATION );
}


void NodeWarnPtrTruncCast(      // WARN FOR CAST POINTER/REFERENCE TRUNCATION
    PTREE node )                // - node for warning
{
    PTreeWarnExpr( node, WARN_POINTER_TRUNCATION_CAST );
}


CNV_RETN NodeCheckPtrTrunc(     // CHECK FOR POINTER TRUNCATION WARNING
    TYPE tgt,                   // - target type
    TYPE src )                  // - source type
{
    return CgTypeTruncation( tgt, src );
}


CNV_RETN NodeCheckPtrCastTrunc( // CHECK FOR CAST POINTER TRUNCATION WARNING
    TYPE tgt,                   // - target type
    TYPE src )                  // - source type
{
    CNV_RETN retn;              // - return: CNV_OK, CNV_OK_TRUNC_CAST

    retn = CgTypeTruncation( tgt, src );
    if( retn == CNV_OK_TRUNC ) {
        retn = CNV_OK_TRUNC_CAST;
    }
    return retn;
}


static SCOPE scope_for_ptr(     // GET SCOPE FOR CLASS POINTED AT
    TYPE type )                 // - pointer type
{
    return TypeScope( type_pointed_to( type ) );
}


static CNV_RETN check_result(   // CHECK RESULT OF A CONVERSION
    PTREE *expr,                // - addr( resultant tree )
    TYPE src,                   // - source type
    TYPE tgt )                  // - target type
{
    CNV_RETN retn;              // - return: CNV_ERR or CNV_OK
    PTREE node;                 // - resultant tree

    node = *expr;
    if( node->op == PT_ERROR ) {
        retn = CNV_ERR;
    } else {
        node->type = tgt;
        NodeSetReference( tgt, node );
        retn = NodeCheckPtrTrunc( tgt, src );
    }
    return retn;
}


static CNV_RETN check_result_cv(// CHECK RESULT FOR CONST/VOLATILE RETURN
    PTREE *expr,                // - addr( resultant tree )
    TYPE src,                   // - source type
    TYPE tgt,                   // - target type
    CNVPTR_REQD reqd_cnvptr )   // - type of conversion
{
    CNV_RETN retn;              // - return: CNV_ERR or CNV_OK

    if( reqd_cnvptr & CNVPTR_NO_TRUNC ) {
        retn = CNV_OK;
    } else {
        retn = check_result( expr, src, tgt );
    }
    if( reqd_cnvptr & CNVPTR_CONST_VOLATILE ) {
        switch( retn ) {
          case CNV_OK :
            retn = CNV_OK_CV;
            break;
          case CNV_OK_TRUNC :
            if( reqd_cnvptr & CNVPTR_CAST ) {
                retn = CNV_OK_TRUNC_CAST_CV;
            } else {
                retn = CNV_OK_TRUNC_CV;
            }
            break;
        }
    }
    return retn;
}


static CNV_RETN emitConvMsg(    // EMIT A CONVERSION MESSAGE
    PTREE expr,                 // - expression for error
    TYPE src,                   // - source type
    TYPE tgt,                   // - target type
    MSG_NUM msg )               // - message code
{
    ConversionTypesSet( src, tgt );
    PTreeErrorExpr( expr, msg );
    return CNV_ERR;
}


CNV_RETN NodeCheckCnvPtrVoid(   // CHECK CONVERSION TO 'VOID*'
    PTREE *a_expr,              // - addr( resultant tree )
    TYPE src,                   // - source type
    TYPE tgt )                  // - target type
{
    CNV_RETN retn;              // - return: CNV_ERR or CNV_OK
    type_flag src_flags;        // - source modifier flags
    type_flag tgt_flags;        // - target modifier flags
    TYPE type;                  // - type pointed at by src
    PTREE expr;                 // - expression

    type = TypePointedAt( src, &src_flags );
    if( type == NULL ) {
        retn = CNV_OK;
    } else {
        expr = *a_expr;
        if( type->id == TYP_FUNCTION ) {
            retn = NodeCheckPtrTrunc( tgt, src );
            if( retn == CNV_OK_TRUNC ) {
                retn = emitConvMsg( expr, src, tgt, ERR_FUNC_ADDR_TOO_BIG );
            }
        } else if( CNV_OK_TRUNC == NodeCheckPtrTrunc( tgt, src ) ) {
            retn = emitConvMsg( expr, src, tgt, ERR_OBJECT_ADDR_TOO_BIG );
        } else {
#if 1
            TypePointedAt( tgt, &tgt_flags );
            src_flags &= TF1_CONST | TF1_VOLATILE;
            tgt_flags &= TF1_CONST | TF1_VOLATILE;
            tgt_flags &= src_flags;
            if( src_flags == tgt_flags ) {
                retn = CNV_OK;
            } else {
                MSG_NUM msg;    // - message code
                switch( tgt_flags ) {
                  case 0:
                    msg = ERR_CNV_VOID_STAR;
                    break;
                  case TF1_CONST :
                    msg = ERR_CNV_VOID_STAR_CONST;
                    break;
                  case TF1_VOLATILE :
                    msg = ERR_CNV_VOID_STAR_VOLATILE;
                    break;
                  case TF1_CONST | TF1_VOLATILE :
                    msg = ERR_CNV_VOID_STAR_CONST_VOLATILE;
                    break;
                  default:
                    msg = ERR_FRONT_END;
                    break;
                }
                retn = emitConvMsg( expr, src, tgt, msg );
            }
#else
            retn = AnalysePtrCV( expr, tgt, src, CNV_EXPR );
#endif
        }
    }
    return retn;
}


CNV_RETN NodeConvertPtr(        // CONVERT A POINTER
    CNVPTR_REQD reqd_cnvptr,    // - type of conversion
    PTREE *expr,                // - expression to be converted
    TYPE src,                   // - source type (converted from)
    TYPE tgt )                  // - target type (converted to)
{
    SCOPE src_scope;            // - source scope
    SCOPE tgt_scope;            // - target scope
    CNV_RETN retn = CNV_ERR;    // - return: CNV_... indicates what happened

    if( same_ptr_types( src, tgt ) ) {
        (*expr)->type = src;
        return CNV_OK;
    }
    src_scope = scope_for_ptr( src );
    tgt_scope = scope_for_ptr( tgt );
    switch( TypeCommonDerivation( TypePointedAtModified( src )
                                , TypePointedAtModified( tgt ) ) ) {
      case CTD_NO :
        if( ptr_to_void( tgt ) ) {
            *expr = NodeMakeConversion( tgt, *expr );
            retn = check_result_cv( expr, src, tgt, reqd_cnvptr );
        } else {
            retn = CNV_IMPOSSIBLE;
        }
        break;
      case CTD_RIGHT_PROTECTED :
        if( (reqd_cnvptr & CNVPTR_CAST) == 0 ) {
            if( reqd_cnvptr & CNVPTR_VIRT_TO_DERIVED ) {
                retn = CNV_PROTECTED;
            } else {
                retn = CNV_IMPOSSIBLE;
            }
            break;
        }
        // drops thru
      case CTD_RIGHT_PRIVATE :
        if( (reqd_cnvptr & CNVPTR_CAST) == 0 ) {
            if( reqd_cnvptr & CNVPTR_VIRT_TO_DERIVED ) {
                retn = CNV_PRIVATE;
            } else {
                retn = CNV_IMPOSSIBLE;
            }
            break;
        }
        // drops thru
      case CTD_RIGHT :
        if( reqd_cnvptr & CNVPTR_VIRT_TO_DERIVED ) {
            NodeConvertBaseToDerived( expr, tgt, tgt_scope, src_scope );
            retn = check_result_cv( expr, src, tgt, reqd_cnvptr );
        } else {
            retn = CNV_IMPOSSIBLE;
        }
        break;
      case CTD_RIGHT_VIRTUAL :
        if( reqd_cnvptr & CNVPTR_VIRT_TO_DERIVED ) {
            retn = CNV_VIRT_DER;
        } else {
            retn = CNV_IMPOSSIBLE;
        }
        break;
      case CTD_RIGHT_AMBIGUOUS :
        if( reqd_cnvptr & CNVPTR_VIRT_TO_DERIVED ) {
            retn = CNV_AMBIGUOUS;
        } else {
            retn = CNV_IMPOSSIBLE;
        }
        break;
      case CTD_LEFT_PRIVATE :
        if( (reqd_cnvptr & CNVPTR_CAST) == 0 ) {
            retn = CNV_PRIVATE;
            break;
        }
        // drops thru
      case CTD_LEFT_PROTECTED :
        if( (reqd_cnvptr & CNVPTR_CAST) == 0 ) {
            retn = CNV_PROTECTED;
            break;
        }
        // drops thru
      case CTD_LEFT :
        NodeConvertDerivedToBase( expr, tgt, src_scope, tgt_scope );
        retn = check_result_cv( expr, src, tgt, reqd_cnvptr );
        break;
      case CTD_LEFT_VIRTUAL :
        NodeConvertDerivedToVirt( expr, tgt, src_scope, tgt_scope );
        retn = check_result_cv( expr, src, tgt, reqd_cnvptr );
        break;
      case CTD_LEFT_AMBIGUOUS :
        retn = CNV_AMBIGUOUS;
        break;
    }
    return retn;
}


#if 0
static CNV_RETN propogateNonZero( // PROPOGATE PTF_PTR_NONZERO TO COMMON
    CNV_RETN retn,              // - return: CNV_...
    PTREE expr )                // - expression
{
    if( NodeIsBinaryOp( expr, CO_COLON ) ) {
        if( ( PTreeOpLeft( expr )->flags & PTF_PTR_NONZERO )
          && ( PTreeOpRight( expr )->flags & PTF_PTR_NONZERO ) ) {
            expr->flags |= PTF_PTR_NONZERO;
        }
    }
    return retn;
}


static CNV_RETN check_common(   // CHECK RESULT OF COMMON CONVERSION
    CNV_RETN retn,              // - return: CNV_...
    PTREE expr,                 // - expression
    TYPE type )                 // - resultant type
{
    if( ( retn == CNV_OK ) || ( retn == CNV_OK_TRUNC ) ) {
        expr->type = type;
        retn = propogateNonZero( retn, expr );
    }
    return retn;
}

CNV_RETN PtrConvertCommon(      // CONVERT TO COMMON PTR
    PTREE expr )                // - expression
{
    PTREE *r_left;              // - reference( left node )
    PTREE *r_right;             // - reference( right node )
    TYPE ltype;                 // - type on left
    TYPE rtype;                 // - type on right
    TYPE lptype;                // - new ptr type on left
    TYPE rptype;                // - new ptr type on right
    TYPE lbtype;                // - base type on left
    TYPE rbtype;                // - base type on right
    TYPE type;                  // - resultant type
    TYPE common;                // - common base type
    CNV_RETN retn;              // - conversion return

    r_left = &expr->u.subtree[0];
    ltype = NodeType( *r_left );
    r_right = &expr->u.subtree[1];
    rtype = NodeType( *r_right );
    if( same_ptr_types( ltype, rtype ) ) {
        type = ltype;
        if( CgTypeSize( ltype ) > CgTypeSize( rtype ) ) {
            expr->type = ltype;
        } else {
            expr->type = rtype;
        }
        retn = propogateNonZero( CNV_OK, expr );
    } else if( ptr_to_void( ltype ) ) {
        retn = NodeCheckCnvPtrVoid( &expr, rtype, ltype );
        retn = check_common( retn, expr, ltype );
    } else if( ptr_to_void( rtype ) ) {
        retn = NodeCheckCnvPtrVoid( &expr, ltype, rtype );
        retn = check_common( retn, expr, rtype );
    } else{
        lbtype = TypePointedAtModified( ltype );
        rbtype = TypePointedAtModified( rtype );
        switch( TypeCommonDerivation( lbtype, rbtype ) ) {
          case CTD_LEFT :
          case CTD_LEFT_VIRTUAL :
            retn = NodeConvertPtr( CNVPTR_DERIVED_ONLY, r_left, ltype, rtype );
            retn = check_common( retn, expr, rtype );
            break;
          case CTD_RIGHT :
          case CTD_RIGHT_VIRTUAL :
            retn = NodeConvertPtr( CNVPTR_DERIVED_ONLY, r_right, rtype, ltype );
            retn = check_common( retn, expr, ltype );
            break;
          case CTD_NO :
            common = TypeCommonBase( rbtype, lbtype );
            if( common != NULL ) {
                lptype = TypePointedAtReplace( ltype, common );
                rptype = TypePointedAtReplace( rtype, common );
                /* TypeCommonBase verified everything so these convs will work*/
                NodeConvertPtr( CNVPTR_DERIVED_ONLY, r_left, ltype, lptype );
                NodeConvertPtr( CNVPTR_DERIVED_ONLY, r_right, rtype, rptype);
                expr->type = expr->u.subtree[0]->type;
                retn = propogateNonZero( CNV_OK, expr );
            } else {
                retn = UserDefCnvToType( r_left, ltype, rtype );
                if( retn == CNV_IMPOSSIBLE ) {
                    retn = UserDefCnvToType( r_right, rtype, ltype );
                    retn = check_common( retn, expr, ltype );
                } else {
                    retn = check_common( retn, expr, rtype );
                }
            }
            break;
          case CTD_RIGHT_AMBIGUOUS :
            ConversionTypesSet( rtype, ltype );
          case CTD_LEFT_AMBIGUOUS :
            retn = CNV_AMBIGUOUS;
            break;
          case CTD_RIGHT_PRIVATE :
            ConversionTypesSet( rtype, ltype );
          case CTD_LEFT_PRIVATE :
            retn = CNV_PRIVATE;
            break;
          case CTD_RIGHT_PROTECTED :
            ConversionTypesSet( rtype, ltype );
          case CTD_LEFT_PROTECTED :
            retn = CNV_PROTECTED;
            break;
        }
    }
    return retn;
}

bool PtrCnvInfo(                // FILL IN PTR-CONVERSION INFORMATION
    TYPE ptr_src,               // - source type
    TYPE ptr_tgt,               // - target pointer type
    PTRCNV* info )              // - pointer-conversion information
{
    bool retb;                  // - return: true ==> can convert trivially
    bool first_level;           // - true ==> at first level
    bool const_always;          // - true ==> const on all preceding levels
    TYPE orig_src;              // - original src type

    info->converts = false;
    info->to_base = false;
    info->to_derived = false;
    info->to_void = false;
    info->ptr_integral_ext = false;
    info->cv_err_0 = false;
    info->reint_cast_ok = false;
    orig_src = ptr_src;
    ptr_src = TypePointedAtModified( ptr_src );
    ptr_tgt = TypePointedAtModified( ptr_tgt );
    if( ptr_src == NULL ) {
        info->pted_src = NULL;
        info->flags_src = 0;
        ptr_tgt = TypeGetActualFlags( ptr_tgt, &info->flags_tgt );
        info->pted_tgt = ptr_tgt;
        if( ptr_tgt->id == TYP_VOID ) {
            info->to_void = true;
        }
        if( NULL != orig_src && IntegralType( orig_src ) ) {
            info->reint_cast_ok = true;
        }
        retb = false;
    } else {
        first_level = true;
        const_always = true;
        info->reint_cast_ok = true;
        for( ; ; ) {
            type_flag flags_src;    // source flags
            type_flag flags_tgt;    // target flags
            type_flag cv_src;       // source CV flags
            type_flag cv_tgt;       // target CV flags
            ptr_src = TypeGetActualFlags( ptr_src, &flags_src );
            ptr_tgt = TypeGetActualFlags( ptr_tgt, &flags_tgt );
            cv_src = flags_src & TF1_CV_MASK;
            cv_tgt = flags_tgt & TF1_CV_MASK;
            if( cv_src != ( cv_tgt & cv_src ) ) {   // test cv-containment
                if( first_level ) {
                    info->cv_err_0 = true;          // - diagnose elsewhere
                } else {
                    retb = false;
                    break;
                }
            }
            if( first_level ) {
                TYPE cl_src;        // class for source
                TYPE cl_tgt;        // class for target
                retb = true;
                info->pted_src = ptr_src;
                info->pted_tgt = ptr_tgt;
                info->flags_src = flags_src;
                info->flags_tgt = flags_tgt;
                cl_src = StructType( ptr_src );
                if( ptr_tgt->id == TYP_VOID ) {
                    info->to_void = true;
//                  retb = (ptr_src == TYP_VOID);
//                  break;
                } else if( NULL != cl_src ) {
                    cl_tgt = StructType( ptr_tgt );
                    if( NULL != cl_tgt
                     && cl_tgt != cl_src ) {
                        if( TypeDerived( ptr_src, ptr_tgt ) ) {
                            info->to_base = true;
                            retb = false;
//                          break;
                        } else if( TypeDerived( ptr_tgt, ptr_src ) ) {
                            info->to_derived = true;
                            retb = false;
//                          break;
                        }
                    }
                } else if( ( ptr_src->id != ptr_tgt->id )
                        && IntegralType( ptr_src )
                        && IntegralType( ptr_tgt )
                        && ( CgMemorySize( ptr_src ) == CgMemorySize( ptr_tgt ) ) ) {
                    info->ptr_integral_ext = true;
                }
                if( !retb ) {
                    if( info->cv_err_0 ) {
                        info->reint_cast_ok = false;
                    }
                    break;
                }
                first_level = false;
            }
            if( cv_tgt != cv_src ) {                // test const'ed to here
                if( ! const_always ) {
                    info->reint_cast_ok = false;
                    retb = false;
                    break;
                }
            }
            if( (cv_tgt & TF1_CONST) == 0 ) {
                const_always = false;
            }
            if( ptr_src == ptr_tgt ) {
                retb = true;
                break;
            }
            if( TYP_FUNCTION == ptr_src->id
             || TYP_FUNCTION == ptr_tgt->id ) {
                retb = TypeCompareExclude( ptr_src
                                         , ptr_tgt
                                         , TC1_FUN_LINKAGE |
                                           TC1_NOT_ENUM_CHAR );
                break;
            }
            ptr_src = TypePointedAtModified( ptr_src );
            ptr_tgt = TypePointedAtModified( ptr_tgt );
            if( NULL == ptr_src ) {
                if( NULL != ptr_tgt
                 && NULL != FunctionDeclarationType( ptr_tgt ) ) {
                    info->reint_cast_ok = false;
                }
                retb = false;
                break;
            }
            if( NULL == ptr_tgt ) {
                retb = false;
                break;
            }
        }
    }
    return( retb );
}
#endif
