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
#include "errdefns.h"
#include "memmgr.h"
#include "ptree.h"
#include "specname.h"
#include "class.h"
#include "rtfuncod.h"
#include "analtyid.h"
#include "rtti.h"

static TYPE type_infoGetType( PTREE expr )
{
    TYPE type_info;
    auto TOKEN_LOCN locn;

    type_info = TypeGetCache( TYPC_TYPE_INFO_CONST_REF );
    if( type_info != NULL ) {
        return( type_info );
    }
    expr = PTreeExtractLocn( expr, &locn );
    type_info = ClassPreDefined( CppSpecialName( SPECIAL_TYPE_INFO ), &locn );
    if( ! TypeDefined( type_info ) ) {
        PTreeSetErrLoc( expr );
        CErr2p( WARN_USER_WARNING_MSG, "<typeinfo> not included" );
    }
    type_info = MakeModifiedType( type_info, TF1_CONST );
    type_info = MakeReferenceTo( type_info );
    return( TypeSetCache( TYPC_TYPE_INFO_CONST_REF , type_info ) );
}

PTREE NodeTypeid( TYPE type )
{
    PTREE lvalue;
    SYMBOL sym;

    sym = TypeidICAccess( type );
    lvalue = NodeSymbolNoRef( NULL, sym, NULL );
    DbgAssert( ! SymIsReferenced( sym ) || GetCurrScope() == ModuleInitScope() );
    return( lvalue );
}

CLASSINFO *GetWithinOffsetOfVFPtr( TYPE class_type, PTREE *pexpr )
{
    CLASSINFO *info;            // - source (or base class of source) class info
    BASE_CLASS *vfptr_base;     // - base class with a vfptr in it
    SEARCH_RESULT *result;      // - result for conversion to vbase

    DbgAssert( PolymorphicType( class_type ) != NULL );
    info = class_type->u.c.info;
    if( ! info->has_vfptr ) {
        /* virtual fns are in a virtual base class */
        vfptr_base = ScopeFindVFPtrInVBase( class_type );
        result = ScopeResultFromBase( class_type, vfptr_base );
        class_type = vfptr_base->type;
        info = class_type->u.c.info;
        NodeConvertToBasePtr( pexpr, class_type, result, TRUE );
        // type of expr is always 'X' so we need lvalue to keep it a pointer
        (*pexpr)->flags |= PTF_LVALUE;
        ScopeFreeResult( result );
    }
    return( info );
}

PTREE AnalyseTypeidExpr( PTREE typeid_expr )
/******************************************/
{
    PTREE expr;
    PTREE result_expr;
    PTREE args;
    PTREE extra;
    PTREE kill;
    TYPE expr_type;
    TYPE type_info;
    TYPE class_type;
    CLASSINFO *info;

    if( ! CompFlags.rtti_enabled ) {
        PTreeErrorExpr( typeid_expr, ERR_RTTI_DISABLED );
        return( typeid_expr );
    }
    DbgAssert( typeid_expr != NULL && typeid_expr->op == PT_UNARY );
    DbgAssert( typeid_expr->cgop == CO_TYPEID_EXPR );
    expr = typeid_expr->u.subtree[0];
    type_info = type_infoGetType( typeid_expr );
    expr_type = expr->type;
    class_type = ClassTypeForType( expr_type );
    if( expr->flags & PTF_LVALUE ) {
        if( PolymorphicType( class_type ) != NULL ) {
            /* polymorphic lvalue */
            typeid_expr->u.subtree[0] = NULL;
            PTreeFree( typeid_expr );
            expr = NodeUnComma( expr, &extra );
            if( NodeIsUnaryOp( expr, CO_INDIRECT ) ) {
                kill = expr;
                expr = kill->u.subtree[0];
                PTreeFree( kill );
            } else if( NodeIsBinaryOp( expr, CO_DOT ) && ( expr->flags & PTF_WAS_INDEX ) != 0 ) {
                kill = expr;
                expr = kill->u.subtree[0];
                kill->u.subtree[0] = NULL;
                NodeFreeDupedExpr( kill );
            }
            info = GetWithinOffsetOfVFPtr( class_type, &expr );
            expr = NodeComma( extra, expr );
            args = NodeArguments( NodeTypeid( class_type ),
                                  NodeOffset( info->vf_offset ),
                                  expr,
                                  NULL );
            result_expr = RunTimeCall( args, type_info, RTF_GET_TYPEID );
            return( result_expr );
        }
        expr_type = TypeReferenced( expr_type );
    }
    /* return type_info for static type of expr */
    if( class_type != NULL && ! TypeDefined( class_type ) ) {
        PTreeErrorExpr( typeid_expr, ERR_TYPEID_CLASS_MUST_BE_DEFINED );
        ConversionTypesSet( class_type, NULL );
        ConversionDiagnoseInf();
        return( typeid_expr );
    }
    result_expr = NodeTypeid( expr_type );
    result_expr = NodeSetType( result_expr, type_info, PTF_LVALUE );
    NodeFreeDupedExpr( typeid_expr );
    return( result_expr );
}

PTREE AnalyseTypeidType( PTREE expr )
/***********************************/
{
    PTREE type_expr;
    PTREE result_expr;
    TYPE type_info;
    TYPE class_type;
    TYPE expr_type;

    DbgAssert( expr != NULL && expr->op == PT_UNARY );
    DbgAssert( expr->cgop == CO_TYPEID_TYPE );
    type_expr = expr->u.subtree[0];
    DbgAssert( type_expr->op == PT_TYPE );
    type_info = type_infoGetType( expr );
    expr_type = type_expr->type;
    class_type = ClassTypeForType( expr_type );
    if( class_type != NULL && ! TypeDefined( class_type ) ) {
        PTreeErrorExpr( expr, ERR_TYPEID_CLASS_MUST_BE_DEFINED );
        ConversionTypesSet( class_type, NULL );
        ConversionDiagnoseInf();
        return( expr );
    }
    result_expr = NodeTypeid( expr_type );
    result_expr = NodeSetType( result_expr, type_info, PTF_LVALUE );
    PTreeFreeSubtrees( expr );
    return( result_expr );
}
