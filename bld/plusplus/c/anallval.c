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
#include "memmgr.h"
#include "cgfront.h"
#include "fold.h"
#include "objmodel.h"
#include "stats.h"
#include "brinfo.h"


#define isUDF( node ) ( node->cgop == CO_NAME_CONVERT )

#ifndef NDEBUG
extern void DumpPTree( PTREE );
#endif

static TYPE lvalueErrMsg(       // GENERATE NOT-LVALUE ERROR MESSAGE
    PTREE expr_chk,             // - expression, to be checked
    PTREE expr_err,             // - expression, for error
    unsigned msg_normal,        // - msg, when normal
    unsigned msg_cast )         // - msg, when cast
{
    if( NodeIsBinaryOp( expr_chk, CO_CONVERT ) ) {
        expr_chk = PTreeOpLeft( expr_chk );
        if( expr_chk->cgop == CO_USER_CAST ) {
            PTreeErrorExpr( expr_err, msg_cast );
        } else {
            PTreeErrorExpr( expr_err, msg_normal );
        }
    } else {
        PTreeErrorExpr( expr_err, msg_normal );
    }
    return NULL;
}


TYPE LvalueErr(                 // NOT-LVALUE ERROR (NODE)
    PTREE expr_chk,             // - expression, to be checked
    PTREE expr )                // - expression
{
    return lvalueErrMsg( expr_chk
                       , expr
                       , ERR_MUST_BE_LVALUE
                       , ERR_MUST_BE_LVALUE_CAST );
}


TYPE LvalueErrLeft(             // NOT-LVALUE ERROR (LEFT NODE)
    PTREE expr_chk,             // - expression, to be checked
    PTREE expr )                // - expression
{
    return lvalueErrMsg( expr_chk
                       , expr
                       , ERR_LEFT_MUST_BE_LVALUE
                       , ERR_LEFT_MUST_BE_LVALUE_CAST );
}


static boolean requiresThis(    // TEST IF SYMBOL REQUIRES A THIS
    SYMBOL sym )                // - symbol
{
    boolean retn;               // - TRUE ==> requires "this"
    SCOPE func_class_scope;     // - scope for function

    if( SymIsClassMember( sym ) ) {
        if( SymIsStaticMember( sym ) || SymIsEnumeration( sym ) ) {
            retn = FALSE;
        } else if( NULL == TypeThisExists() ) {
            retn = TRUE;
        } else {
            func_class_scope = ScopeFunctionScopeInProgress()->enclosing;
            if( ( func_class_scope->id == SCOPE_CLASS )
              &&( ScopeDerived( func_class_scope, SymScope( sym ) ) ) ) {
                retn = FALSE;
            } else {
                retn = TRUE;
            }
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


boolean AnalyseSymbolAccess(    // ANALYSE ACCESS TO SYMBOL
    PTREE expr,                 // - expression for errors
    PTREE symbol,               // - symbol being accessed
    PTREE this_expr,            // - expression for "this"
    SYMBOL_DIAG *diag )         // - diagnosis to be used
{
    boolean retn;               // - return: TRUE ==> access ok
    SEARCH_RESULT *result;      // - search result
    TOKEN_LOCN err_locn;        // - location for errors

    PTreeExtractLocn( expr, &err_locn );
    result = symbol->u.symcg.result;
#ifdef OPT_BR
    if( CompFlags.optbr_f
     || CompFlags.optbr_m
     || CompFlags.optbr_t
     || CompFlags.optbr_v ) {
        BrinfReferenceSymbol( &err_locn, symbol->u.symcg.symbol );
    }
#endif
    ScopeResultErrLocn( result, &err_locn );
    SymSetNvReferenced( symbol->u.symcg.symbol );
    if( ScopeCheckSymbol( result, symbol->u.symcg.symbol ) ) {
        PTreeErrorNode( expr );
        retn = FALSE;
    } else if( this_expr == NULL ) {
        if( result->simple ) {
            retn = TRUE;
        } else if( result->no_this ) {
            retn = TRUE;
        } else if( requiresThis( symbol->u.symcg.symbol ) ) {
            PTreeErrorExpr( expr, diag->msg_no_this );
            retn = FALSE;
        } else {
            retn = TRUE;
        }
    } else {
        retn = SymIsClassMember( symbol->u.symcg.symbol );
    }
    return retn;
}


static SYMBOL_DIAG diagMemb =   // diagnosis for member
{   ERR_INVALID_NONSTATIC_ACCESS// - no "this"
,   ERR_EXTRA_THIS_FOR_DATA     // - extra "this" for data element
,   ERR_ENCLOSING_THIS_DATA     // - accessing enclosing class member
};


boolean AnalyseThisDataItem(    // ANALYSE "THIS" DATA ITEM IN PARSE TREE
    PTREE *a_expr )             // - addr[ expression ]
{
    boolean retn;               // - return: TRUE ==> ok
    PTREE expr;                 // - expression
    SEARCH_RESULT *result;      // - search result for node
    PTREE *r_right;             // - ref[ node for symbol to be adjusted ]
    PTREE right;                // - node for symbol to be adjusted
    unsigned offset;            // - offset for field
    TYPE type;                  // - field type
    type_flag flags;            // - cv flags, when bit field

    expr = *a_expr;
    r_right = PTreeRefRight( expr );
    right = *r_right;
    if( AnalyseSymbolAccess( expr
                           , right
                           , expr->u.subtree[0]
                           , &diagMemb ) ) {
        type = NodeType( expr->u.subtree[0] );
        type = TypedefModifierRemove( type )->of;
        type = TypeMergeForMember( type, expr->type );
        result = right->u.symcg.result;
        right->u.symcg.result = NULL;
        if( expr->u.subtree[0]->flags & PTF_MEMORY_EXACT ) {
            offset = result->exact_delta + result->offset;
            right = NodeReplace( right, NodeOffset( offset ) );
            *r_right = right;
        } else if( result->non_virtual ) {
            offset = result->delta + result->offset;
            right = NodeReplace( right, NodeOffset( offset ) );
            *r_right = right;
        } else {
            expr = NodePruneRight( expr );
            expr = NodePruneTop( expr );
            expr->flags |= PTF_PTR_NONZERO;
            NodeConvertToBasePtr( &expr, type, result, TRUE );
            expr->flags |= PTF_LVALUE | PTF_LV_CHECKED;
            expr = NodeBinary( CO_DOT, expr, NodeOffset( result->offset ) );
        }
        expr->type = type;
        expr->flags |= PTF_LVALUE;
        ScopeFreeResult( result );
        type = TypeModFlags( type, &flags );
        if( type->id == TYP_BITFIELD ) {
            expr = NodeUnaryCopy( CO_BITFLD_CONVERT, expr );
            expr->type = MakeModifiedType( type->of, flags & TF1_CV_MASK );
            expr->flags |= PTF_LVALUE;
        }
        expr = NodeSetMemoryExact( expr );
        expr = NodeFetchReference( expr );
        expr->flags |= PTF_LV_CHECKED;
        *a_expr = expr;
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return retn;
}


static PTREE reduceToRight(     // REDUCE EXPRESSION TO RIGHT EXPR
    PTREE *a_node )             // - addr( expression )
{
    PTREE node;                 // - expression

    node = NodePruneLeftTop( *a_node );
    *a_node = node;
    return node;
}


static PTREE thisPointsNode(    // MAKE this->node
    PTREE node )                // - original node
{
    TYPE type;                  // - node type
    PTREE left;                 // - this node to left

    left = NodeThisCopyLocation( node );
    if( left == NULL ) {
        PTreeErrorExpr( node, ERR_INVALID_NONSTATIC_ACCESS );
    } else {
        type = node->type;
        node = NodeBinary( CO_ARROW, left, node );
        node->type = type;
        node->flags |= PTF_LVALUE;
        node = PTreeCopySrcLocation( node, left );
    }
    return node;
}


static boolean checkConversionLookup( // CHECK RESULT OF CONVERSION LOOKUP
    SEARCH_RESULT *result,      // - result of lookup
    PTREE conv,                 // - node for conversion routine
    PTREE expr )                // - node being analysed
{
    boolean retn;               // - TRUE ==> ok

    if( ( result == NULL ) || ( result->sym == NULL ) ) {
        PTreeErrorExpr( conv, ERR_CONVERSION_NOT_DEFINED );
        PTreeErrorNode( expr );
        retn = FALSE;
    } else {
        ExtraRptSymUsage( result->sym );
        NodeSymbolCallee( conv, result->sym, result );
        retn = TRUE;
    }
    return retn;
}


static boolean checkIdLookup(   // CHECK RESULT OF ID LOOKUP
    SEARCH_RESULT *result,      // - result of lookup
    SCOPE scope,                // - scope for lookup
    PTREE id,                   // - node for id
    PTREE expr )                // - node being analysed
{
    boolean retn;               // - TRUE ==> ok
    SYMBOL sym;                 // - a symbol lookup up
    unsigned msg;               // - undeclared sym error message
    NAME name;                  // - id name

    if( result == NULL ) {
        ScopeInsertErrorSym( scope, id );
        name = id->u.id.name;
        msg = ERR_UNDECLARED_SYM;
        if( ScopeType( scope, SCOPE_CLASS ) ) {
            if( NodeIsBinaryOp( expr, CO_DOT ) ) {
                msg = ERR_UNDECLARED_MEMBER;
            } else if( NodeIsBinaryOp( expr, CO_ARROW ) ) {
                msg = ERR_UNDECLARED_MEMBER;
            }
        }
        if( msg == ERR_UNDECLARED_SYM ) {
            PTreeErrorExprName( id, msg, name );
        } else {
            PTreeErrorExprNameType( id, msg, name, ScopeClass( scope ) );
        }
        PTreeErrorNode( expr );
        retn = FALSE;
    } else {
        sym = result->sym_name->name_syms;
        if( sym == NULL ) {
            PTreeErrorExpr( id, ERR_ILLEGAL_TYPE_USE );
            ScopeFreeResult( result );
            PTreeErrorNode( expr );
            retn = FALSE;
        } else if( SymIsAnError( sym ) ) {
            ScopeFreeResult( result );
            PTreeErrorNode( expr );
            retn = FALSE;
        } else if( NULL != FunctionDeclarationType( sym->sym_type ) ) {
            ExtraRptSymUsage( sym );
            NodeSymbolNoRef( id, sym, result );
            id->flags |= PTF_LV_CHECKED;
            retn = TRUE;
        } else {
            ExtraRptSymUsage( sym );
            NodeSymbol( id, sym, result );
            id->flags |= PTF_LV_CHECKED;
            retn = TRUE;
        }
    }
    return retn;
}


static boolean analyseFunction( // ANALYSE FUNCTION NODE
    PTREE expr,                 // - original expression
    PTREE func )                // - function node
{
    boolean retn;               // - TRUE ==> function ok

    if( ScopeImmediateCheck( func->u.symcg.result ) ) {
        PTreeErrorNode( expr );
        retn = FALSE;
    } else {
        expr->flags |= PTF_LVALUE;
        retn = TRUE;
    }
    return retn;
}


static void checkVolatileVar(   // SET SIDE EFFECT IF VOLATILE VARIABLE
    PTREE expr )                // - expression for variable
{
    type_flag flags;            // - flags for expression

    TypeModFlags( expr->type, &flags );
    if( flags & TF1_VOLATILE ) {
        expr->flags |= PTF_SIDE_EFF;
    }
}


static boolean analyseBareSymbol( // ANALYSE AN BARE SYMBOL
    PTREE *a_expr )             // - addr( symbol entry )
{
    PTREE expr;                 // - symbol entry
    PTREE alias;                // - alias expr
    boolean retn;               // - return: TRUE ==> all ok
    SYMBOL sym;                 // - the symbol

    expr = *a_expr;
    sym = expr->u.symcg.symbol;
    if( SymIsAnError( sym ) ) {
        PTreeErrorNode( expr );
        retn = FALSE;
    } else if( sym->id == SC_ADDRESS_ALIAS ) {
        alias = NodeSymbol( NULL, sym->u.alias, expr->u.symcg.result );
        alias = PTreeCopySrcLocation( alias, expr );
        if( PointerType( sym->sym_type ) != NULL ) {
            alias = NodeConvert( sym->sym_type, alias );
        }
        PTreeFree( expr );
        *a_expr = alias;
        retn = TRUE;
    } else if( NULL != FunctionDeclarationType( sym->sym_type ) ) {
        if( MainProcedure( sym ) ) {
            PTreeErrorExpr( expr, ERR_REFERENCED_MAIN );
            retn = FALSE;
        } else {
            retn = analyseFunction( expr, expr );
        }
    } else {
        sym = SymDeAlias( sym );
        expr->u.symcg.symbol = sym;
        retn = AnalyseSymbolAccess( expr, expr, NULL, &diagMemb );
        NodeFreeSearchResult( expr );
        if( retn ) {
            if( SymIsEnumeration( sym ) ) {
                sym = expr->u.symcg.symbol;
                PTreeFree( expr );
                expr = NodeFromConstSym( sym );
                expr->type = sym->sym_type;
            } else {
                expr = NodeSetMemoryExact( expr );
                expr = NodeFetchReference( expr );
                checkVolatileVar( expr );
            }
            *a_expr = expr;
            retn = TRUE;
        }
    }
    return retn;
}

static boolean massageStaticEnumAccess( // x.static, x.enum adjustments
    PTREE *a_expr )             // - addr( member expr )
{
#ifdef OLD_STATIC_MEMBER_ACCESS
    boolean retn;

    reduceToRight( a_expr );
    retn = analyseBareSymbol( a_expr );
#else
    PTREE expr;
    PTREE lhs;
    PTREE rhs;
    boolean retn;

    expr = *a_expr;
    DbgAssert( NodeIsBinaryOp( expr, CO_ARROW ) || NodeIsBinaryOp( expr, CO_DOT ) );
    lhs = expr->u.subtree[0];
    expr->u.subtree[0] = NULL;
    rhs = expr->u.subtree[1];
    expr->u.subtree[1] = NULL;
    PTreeFree( expr );
    retn = analyseBareSymbol( &rhs );
    *a_expr = NodeCommaIfSideEffect( lhs, rhs );
#endif
    return retn;
}


static boolean analyseMemberExpr( // ANALYSE A MEMBER EXPRESION
    PTREE *a_expr )             // - addr( member expression )
{
    PTREE expr;                 // - symbol entry
    boolean retn;               // - return value
    SYMBOL sym;                 // - the symbol

    expr = *a_expr;
    if( expr->op == PT_ERROR ) {
        retn = FALSE;
    } else if( NodeIsBinaryOp( expr->u.subtree[1], CO_TEMPLATE ) ) {
        DbgAssert( expr->u.subtree[1]->u.subtree[0]->op == PT_SYMBOL );
        sym = expr->u.subtree[1]->u.subtree[0]->u.symcg.symbol;
        expr->type = sym->sym_type;

        DbgAssert( NULL != FunctionDeclarationType( sym->sym_type ) );
        retn = analyseFunction( expr->u.subtree[1],
                                expr->u.subtree[1]->u.subtree[0] );
    } else {
        DbgAssert( expr->u.subtree[1]->op == PT_SYMBOL );
        sym = expr->u.subtree[1]->u.symcg.symbol;
        expr->type = sym->sym_type;

        if( NULL != FunctionDeclarationType( sym->sym_type ) ) {
            retn = analyseFunction( expr, expr->u.subtree[1] );
        } else if( SymIsStaticDataMember( sym ) || SymIsEnumeration( sym ) ) {
            retn = massageStaticEnumAccess( a_expr );
        } else {
            retn = AnalyseThisDataItem( a_expr );
            if( retn ) {
                checkVolatileVar( *a_expr );
                retn = TRUE;
            }
        }
    }
    return retn;
}


static boolean analyseSymbol(   // ANALYSE AN UNDECORATED SYMBOL
    PTREE *a_expr )             // - addr( symbol entry )
{
    PTREE expr;                 // - symbol entry
    boolean retn;               // - return value
    SYMBOL sym;                 // - the symbol

    expr = *a_expr;
    sym = expr->u.symcg.symbol;
    if( SymIsThisDataMember( sym ) ) {
        if( expr->u.symcg.result->use_this ) {
            expr = thisPointsNode( expr );
            *a_expr = expr;
            if( expr->op == PT_ERROR ) {
                retn = FALSE;
            } else {
                retn = analyseMemberExpr( a_expr );
            }
        } else {
            PTreeErrorExpr( expr, ERR_INVALID_NONSTATIC_ACCESS );
            retn = FALSE;
        }
    } else {
        retn = analyseBareSymbol( a_expr );
    }
    return retn;
}


static boolean analyseMember(   // ANALYSE A MEMBER NODE
    PTREE *a_expr,              // - addr( expression for member )
    SCOPE start,                // - scope to start lookup
    SCOPE disamb )              // - scope to disambiguate
{
    PTREE member;               // - node for member
    PTREE expr;                 // - expression
    SEARCH_RESULT *result;      // - result of search
    boolean retn;               // - TRUE ==> ok

    expr = *a_expr;
    member = PTreeOpRight( expr );
    if( isUDF( member ) ) {
        result = ScopeFindScopedMemberConversion( start
                                                , disamb
                                                , member->type
                                                , TF1_NULL );
        retn = checkConversionLookup( result, member, expr );
    } else {
        if( member->op == PT_ID ) {
            result = ScopeFindScopedMember( start, disamb, member->u.id.name );
            retn = checkIdLookup( result, start, member, expr );
        } else if( NodeIsBinaryOp( member, CO_TEMPLATE )
                && ( member->u.subtree[0]->op == PT_ID ) ) {
            result = ScopeFindScopedMember( start, disamb,
                                            member->u.subtree[0]->u.id.name );
            retn = checkIdLookup( result, start, member->u.subtree[0], expr );
        } else {
            DbgAssert( 0 );
        }
    }
    if( retn ) {
        retn = analyseMemberExpr( a_expr );
    }
    return retn;
}


static boolean simpleTypeDtor(  // TEST IF DTOR OF A SIMPLE TYPE
    TYPE type,                  // - type to be DTOR'd
    PTREE expr )                // - "->" or "." expression
{
    boolean retn;               // - return: TRUE ==> is DTOR of simple type
    PTREE right;                // - right  operand

    type = type;                // may have to check some day
    right = PTreeOpRight( expr );
    if( ( right->op == PT_ID )
      &&( right->cgop == CO_NAME_DTOR ) ) {
        if( TypedefRemove( type )->id == TYP_VOID ) {
            CErr1( ERR_DTOR_TYPE_VOID );
        } else if( ! TypesIdentical( type,
                                     expr->u.subtree[1]->type ) ) {
            CErr1( ERR_INVALID_SCALAR_DESTRUCTOR );
        }
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return retn;
}


static boolean analyseDtor(     // ANALYSE A DTOR CALL
    PTREE *a_expr,              // - addr[ DTOR expression, before lookup ]
    SCOPE scope,                // - scope of object
    SCOPE disamb )              // - NULL or disambiguating scope
{
    PTREE expr;                 // - DTOR expression, before lookup
    PTREE dtor;                 // - possible DTOR symbol
    boolean retn;               // - return: TRUE ==> everything ok
    SEARCH_RESULT *result;      // - result of search

    expr = *a_expr;
    dtor = expr->u.subtree[1];
    if( scope != TypeScope( dtor->type ) ) {
        PTreeErrorExpr( dtor, ERR_DTOR_NOT_SAME );
        PTreeErrorNode( expr );
        retn = FALSE;
    } else {
        dtor = expr->u.subtree[1];
        if( disamb == NULL ) {
            result = DtorFindResult( dtor->type );
            retn = checkIdLookup( result, scope, dtor, expr );
            if( retn ) {
                retn = analyseMemberExpr( a_expr );
            }
        } else {
            if( disamb != scope ) {
                PTreeErrorExpr( dtor, ERR_DTOR_BAD_QUAL );
                PTreeErrorNode( dtor );
                retn = FALSE;
            } else {
                DtorFind( dtor->type );
                result = ScopeContainsMember( scope
                                            , CppDestructorName() );
                retn = checkIdLookup( result, scope, dtor, expr );
                if( retn ) {
                    retn = analyseMemberExpr( a_expr );
                }
            }
        }
    }
    return retn;
}


static boolean analyseMembRight(// ANALYSE MEMBER ON RIGHT
    PTREE *a_expr,              // - addr( expression for member )
    TYPE type )                 // - type for class on left
{
    boolean retn;               // - return: TRUE ==> is ok
    PTREE expr;                 // - operation
    PTREE right;                // - right operand
    SCOPE scope;                // - scope for class
    SCOPE disamb;               // - disambiguating scope

    if( type == NULL ) {
        retn = FALSE;
    } else {
        scope = TypeScope( type );
        expr = *a_expr;
        if( scope == NULL ) {
            expr->type = TypeVoidFunOfVoid();
            retn = TRUE;                // assumes only DTOR will get thru
        } else {
            right = expr->u.subtree[1];
            if( right->op == PT_ID ) {
                disamb = NULL;
                if( right->cgop == CO_NAME_DTOR ) {
                    retn = analyseDtor( a_expr, scope, disamb );
                } else {
                    retn = analyseMember( a_expr, scope, disamb );
                }
            } else if( NodeIsBinaryOp( right, CO_COLON_COLON ) ) {
                if( right->u.subtree[0] == NULL ) { // - already lexical error
                    PTreeErrorNode( expr );
                    retn = FALSE;
                } else {
                    disamb = TypeScope( right->u.subtree[0]->type );
                    right = reduceToRight( &expr->u.subtree[1] );
                    right->flags |= PTF_COLON_QUALED;
                    if( right->cgop == CO_NAME_DTOR ) {
                        retn = analyseDtor( a_expr, scope, disamb );
                    } else {
                        retn = analyseMember( a_expr, scope, disamb );
                    }
                }
            } else if( right->op == PT_SYMBOL ) {
                // this will be the form from datainit.c
                retn = analyseMemberExpr( a_expr );
            } else if( NodeIsBinaryOp( right, CO_TEMPLATE )
                    && ( right->u.subtree[0]->op == PT_ID ) ) {
                disamb = NULL;
                retn = analyseMember( a_expr, scope, disamb );
            } else if( NodeIsBinaryOp( right, CO_TEMPLATE )
                    && ( right->u.subtree[0]->op == PT_SYMBOL ) ) {
                // TODO
#ifndef NDEBUG
                printf("%s:%d\n", __FILE__, __LINE__);
                DumpPTree( right );
#endif
                retn = analyseMemberExpr( a_expr );
#ifndef NDEBUG
            } else {
                CFatal( "corrupted member tree" );
#endif
            }
        }
    }
    return retn;
}

static TYPE diagMember( PTREE left, PTREE expr, unsigned msg )
{
    TYPE left_type;

    left_type = NodeType( left );
    PTreeErrorExpr( left, msg );
    InfMsgPtr( INF_OPERAND_TYPE, left_type );
    PTreeErrorNode( expr );
    return NULL;
}


static TYPE analyseClPtrLeft(   // ANALYSE A CLASS POINTER ON LEFT
    PTREE *a_expr )             // - addr( expression )
{
    PTREE expr;                 // - operation
    PTREE left;                 // - left node
    TYPE type;                  // - node type

    expr = *a_expr;
    left = NodeRvalueLeft( expr );
    type = TypedefModifierRemove( left->type );
    if( ( type->id != TYP_POINTER ) || ( TF1_REFERENCE & type->flag ) ) {
        type = diagMember( left, expr, ERR_MUST_BE_PTR_TO_STRUCT_OR_UNION );
    } else {
        type = type->of;
        if( NULL == StructType( type ) ) {
            if( ! simpleTypeDtor( type, expr ) ) {
                type = diagMember( left, expr, ERR_MUST_BE_PTR_TO_STRUCT_OR_UNION );
            }
        } else {
            if( ! TypeDefined( type ) ) {
                PTreeErrorExpr( left, ERR_UNDEFED_CLASS_PTR );
                InfClassDecl( type );
                PTreeErrorNode( expr );
                type = NULL;
            } else if( ! NodeDerefPtr( &expr->u.subtree[0] ) ) {
                PTreeErrorNode( expr );
                type = NULL;
            }
        }
    }
    return type;
}


static TYPE analyseClassLeft(   // ANALYSE A CLASS ON LEFT
    PTREE *a_expr )             // - addr( expression )
{
    PTREE expr;                 // - operation
    PTREE left;                 // - left node
    TYPE type;                  // - type for class

    expr = *a_expr;
    left = PTreeOpLeft( expr );
    if( left->flags & PTF_LVALUE ) {
        type = left->type;
        if( ( NULL == ClassTypeForType( type ) )
          &&( ! simpleTypeDtor( type, expr ) ) ) {
            type = diagMember( left, expr, ERR_MUST_BE_STRUCT_OR_UNION );
        } else if( ! TypeDefined( type ) ) {
            PTreeErrorExpr( left, ERR_UNDEFINED_CLASS_OBJECT );
            InfClassDecl( type );
            PTreeErrorNode( expr );
            type = NULL;
        }
    } else {
        type = LvalueErrLeft( left, expr );
    }
    return type;
}


static boolean analyseClQual(   // ANALYSE :: operator
    PTREE *a_expr )             // - addr( expression to be analysed )
{
    boolean retn;               // - return: TRUE ==> all ok
    PTREE expr;                 // - expression to be analysed
    SCOPE start;                // - starting scope
    SCOPE disam;                // - disambiguating scope
    PTREE right;                // - expression on right
    SEARCH_RESULT *result;      // - result of lookup

    expr = *a_expr;
    right = expr->u.subtree[1];
    if( expr->u.subtree[0] == NULL ) {
        start = GetFileScope();
        disam = GetFileScope();
    } else {
        start = GetCurrScope();
        DbgAssert( NodeIsBinaryOp( expr, CO_COLON_COLON ) );
        DbgAssert( expr->u.subtree[0] != NULL );
        DbgAssert( expr->u.subtree[0]->op == PT_TYPE );
        disam = expr->u.subtree[0]->u.type.scope;
    }
    if( isUDF( right ) ) {
        result = ScopeFindScopedNakedConversion( start
                                               , disam
                                               , right->type
                                               , TF1_NULL );
        retn = checkConversionLookup( result, right, expr );
    } else {
        result = ScopeFindScopedNaked( start
                                     , disam
                                     , right->u.id.name );
        retn = checkIdLookup( result, start, right, expr );
    }
    if( retn ) {
        right = reduceToRight( a_expr );
        right->flags |= PTF_COLON_QUALED;
        retn = TRUE;
    }
    return retn;
}


boolean AnalyseLvalue(          // ANALYSE AN LVALUE
    PTREE *a_expr )             // - addr( expression to be analysed )
{
    PTREE expr;                 // - expression to be analysed
    PTREE right;                // - expression on right
    boolean retn;               // - return: TRUE ==> all ok
    SEARCH_RESULT *result;      // - result of lookup

    DbgStmt( retn = UNKNOWN );
    expr = *a_expr;
    switch( expr->op ) {
      case PT_ID :
        if( expr->cgop == CO_NAME_THIS ) {
            right = NodeThisCopyLocation( expr );
            if( NULL == right ) {
                PTreeErrorExpr( expr, ERR_NO_THIS_PTR_DEFINED );
                retn = FALSE;
            } else {
                right->flags |= PTF_LV_CHECKED;
                *a_expr = right;
                PTreeFree( expr );
                retn = TRUE;
            }
        } else if( expr->cgop == CO_NAME_CDTOR_EXTRA ) {
            *a_expr = NodeCDtorExtra();
            PTreeFree( expr );
            retn = TRUE;
        } else if( expr->cgop == CO_NAME_DTOR ) {
            PTreeErrorExpr( expr, ERR_DTOR_NO_OBJECT );
            retn = FALSE;
        } else {
            if( isUDF( expr ) ) {
                result = ScopeFindNakedConversion( GetCurrScope()
                                                 , expr->type
                                                 , TF1_NULL );
                retn = checkConversionLookup( result, expr, expr );
            } else {
                result = ScopeFindNaked( GetCurrScope(), expr->u.id.name );
                retn = checkIdLookup( result, GetCurrScope(), expr, expr );
            }
            if( ! retn ) break;
            retn = analyseSymbol( a_expr );
        }
        break;
      case PT_BINARY :
        if( CO_COLON_COLON == expr->cgop ){
            retn = analyseClQual( a_expr );
            if( ! retn ) break;
            retn = analyseSymbol( a_expr );
            break;
        }
        // drops thru
      default :
        expr->flags |= PTF_LV_CHECKED;
        retn = TRUE;
        break;
    }
    DbgAssert( DbgIsBoolean( retn ) );
    if( retn ) {
        (*a_expr)->type =
            BindTemplateClass( (*a_expr)->type, &(*a_expr)->locn, FALSE );
    }
    return retn;
}


boolean AnalyseLvalueAddrOf(    // ANALYSE LVALUE FOR "&"
    PTREE *a_expr )             // - addr[ expression to be analysed ]
{
    boolean retn;               // - return: TRUE ==> all ok
    SYMBOL sym;                 // - symbol

    if( NodeIsBinaryOp( *a_expr, CO_COLON_COLON ) ) {
        retn = analyseClQual( a_expr );
        if( retn ) {
            sym = (*a_expr)->u.symcg.symbol;
            if( SymIsFunction( sym ) ) {
                if( SymIsCtor( sym ) ) {
                    PTreeErrorExpr( *a_expr, ERR_ADDR_OF_CTOR );
                    retn = FALSE;
                } else if( SymIsDtor( sym ) ) {
                    PTreeErrorExpr( *a_expr, ERR_ADDR_OF_DTOR );
                    retn = FALSE;
                } else {
                    (*a_expr)->flags |= PTF_LVALUE;
                    retn = TRUE;
                }
            } else if( SymIsThisDataMember( sym ) ) {
                (*a_expr)->flags |= PTF_LVALUE;
                retn = TRUE;
            } else {
                retn = analyseSymbol( a_expr );
            }
        }
    } else {
        retn = AnalyseLvalue( a_expr );
    }
    return retn;
}


PTREE AnalyseLvArrow(           // ANALYSE LVALUE "->"
    PTREE expr )                // - addr[ expression to be analysed ]
{
    TYPE type;                  // - class type of left operand

    if( ( expr->u.subtree[0]->flags & PTF_LV_CHECKED )
     || AnalyseLvalue( &expr->u.subtree[0] ) ) {
        if( expr->type == NULL ) {
            PTREE orig = expr;
            //orig = OverloadOperator( orig );
            if( orig->op != PT_ERROR ) {
                type = analyseClPtrLeft( &orig );
                if( analyseMembRight( &orig, type ) ) {
                    orig->flags |= PTF_LV_CHECKED;
                } else {
                    PTreeErrorNode( orig );
                }
            }
            expr = orig;
        }
    } else {
        PTreeErrorNode( expr );
    }
    return expr;
}


PTREE AnalyseLvDot(             // ANALYSE LVALUE "."
    PTREE expr )                // - expression to be analysed
{
    TYPE type;                  // - class type of left operand
    PTREE orig;                 // - used for optimization

    if( ( expr->u.subtree[0]->flags & PTF_LV_CHECKED )
     || AnalyseLvalue( &expr->u.subtree[0] ) ) {
        if( expr->type == NULL ) {
            PTREE left = expr->u.subtree[0];
            if( ! ( left->flags & PTF_LVALUE ) ) {
                TYPE cl_type = StructType( left->type );
                if( cl_type != NULL ) {
#if 0
                    if( OMR_CLASS_REF == ObjModelArgument( cl_type ) ) {
                        left = NodeConvert( MakeReferenceTo( left->type )
                                          , left );
                        left->flags |= PTF_LVALUE;
                    } else {
                        left = NodeAssignTemporary( left->type, left );
                    }
                    expr->u.subtree[0] = left;
#else
                    expr->u.subtree[0] = NodeForceLvalue( left );
#endif
                }
            }
            orig = expr;
            type = analyseClassLeft( &orig );
            if( analyseMembRight( &orig, type ) ) {
                orig->flags |= PTF_LV_CHECKED;
            } else {
                PTreeErrorNode( orig );
            }
            expr = orig;
        }
    } else {
        PTreeErrorNode( expr );
    }
    return expr;
}

static boolean diagnoseOffsetof(// DIAGNOSE ERROR IN OFFSETOF
    PTREE id,                   // - node for id
    SYMBOL sym )                // - member symbol
{
    if( sym == NULL ) {
        PTreeErrorExpr( id, ERR_ILLEGAL_TYPE_USE );
    } else if( SymIsAnError( sym ) ) {
        PTreeErrorNode( id );
    } else if( SymIsFunction( sym ) ) {
        PTreeErrorExpr( id, ERR_OFFSETOF_FUNCTION );
    } else if( SymIsStaticMember( sym ) ) {
        PTreeErrorExpr( id, ERR_OFFSETOF_STATIC );
    } else if( SymIsEnumeration( sym ) ) {
        PTreeErrorExpr( id, ERR_OFFSETOF_ENUM );
    } else if( SymUnmodifiedType( sym )->id == TYP_BITFIELD ) {
        PTreeErrorExpr( id, ERR_OFFSETOF_BITFIELD );
    } else {
        return FALSE;
    }
    return TRUE;
}

static PTREE reverseTree( PTREE *ptree )
{
    PTREE tree;
    PTREE left;
    PTREE parent;

    tree = *ptree;
    parent = NULL;
    while( tree != NULL ) {
        DbgAssert( NodeIsBinaryOp( tree, CO_DOT )
                || NodeIsBinaryOp( tree, CO_INDEX ) );
        left = tree->u.subtree[0];
        tree->u.subtree[0] = parent;
        parent = tree;
        tree = left;
    }
    *ptree = parent;
    return parent;
}

static TYPE applyIndex( PTREE tree, TYPE type, target_offset_t *poffset )
{
    target_offset_t offset;     // - index offset
    PTREE curri;                // - subscript operations

    if( ! TypeDefined( type ) ) {
        PTreeErrorExpr( tree, ERR_PTR_SCALES_LEFT );
        return NULL;
    }
    for( curri = reverseTree( &(tree->u.subtree[1]) ); curri != NULL; curri = curri->u.subtree[0] ) {
        DbgAssert( NodeIsBinaryOp( curri, CO_INDEX ) );
        type = ArrayType( type );
        if( type == NULL ) {
            PTreeErrorExpr( tree, ERR_EXPR_MUST_BE_ARRAY );
            return NULL;
        }
        offset = curri->u.subtree[1]->u.int_constant * CgTypeSize( type->of );
        *poffset += offset;
        type = type->of;
    }
    return type;
}

PTREE AnalyseOffsetOf(          // ANALYSE OFFSETOF
    PTREE expr )                // - expression to be analysed
{
    target_offset_t offset;     // - current offset
    target_offset_t exact;      // - exact member offset
    SYMBOL sym;                 // - member sym
    NAME name;                  // - member name
    PTREE right;                // - RHS expr
    PTREE field;                // - field expr
    PTREE curr;                 // - current field expr
    PTREE member;               // - current member id
    TYPE type;                  // - class type of left operand
    SCOPE scope;                // - search scope
    SEARCH_RESULT *result;      // - search result

    type = BindTemplateClass( expr->u.subtree[0]->type, &expr->locn, FALSE );
    field = PtdGetOffsetofExpr( expr );
    DbgAssert( field != NULL );
    offset = 0;
    for( curr = reverseTree( &(field) ); curr != NULL; curr = curr->u.subtree[0] ) {
        DbgAssert( NodeIsBinaryOp( curr, CO_DOT ) );
        type = StructType( type );
        if( type == NULL ) {
            PTreeErrorExpr( curr, ERR_OFFSETOF_CLASS );
            PTreeErrorNode( expr );
            break;
        }
        if( ! TypeDefined( type ) ) {
            PTreeErrorExpr( curr, ERR_OFFSETOF_UNDEFINED_TYPE );
            PTreeErrorNode( expr );
            break;
        }
        scope = TypeScope( type );
        right = curr->u.subtree[1];
        member = right->u.subtree[0];
        DbgAssert( member->op == PT_ID );
        name = member->u.id.name;
        result = ScopeFindScopedMember( scope, NULL, name );
        if( result == NULL ) {
            ScopeInsertErrorSym( scope, member );
            PTreeErrorExprNameType( member, ERR_UNDECLARED_MEMBER, name, type );
            PTreeErrorNode( expr );
            break;
        }
        exact = result->exact_delta + result->offset;
        sym = result->sym_name->name_syms;
        ScopeFreeResult( result );
        if( diagnoseOffsetof( member, sym ) ) {
            PTreeErrorNode( expr );
            break;
        }
        ExtraRptSymUsage( sym );
        type = sym->sym_type;
        offset += exact;
        type = applyIndex( right, type, &offset );
        if( type == NULL ) {
            PTreeErrorNode( expr );
            break;
        }
    }
    PTreeFreeSubtrees( field );
    if( curr == NULL ) {
        PTreeFreeSubtrees( expr );
        expr = NodeOffset( offset );
    }
    return expr;
}
