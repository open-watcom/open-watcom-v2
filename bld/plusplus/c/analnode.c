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

#include <stdarg.h>

#include "cgfront.h"
#include "fold.h"
#include "fnovload.h"
#include "fnbody.h"
#include "objmodel.h"
#include "typesig.h"
#include "conpool.h"
#include "utility.h"


static PTREE set_meaningful_side_effects(   // SET MEANINGFUL, SIDE_EFFECTS
    PTREE node )            // - node
{
    if( node->op == PT_UNARY ) {
        node->flags |= PTreeEffFlags( node->u.subtree[0] );
    } else if( node->op == PT_BINARY ) {
        node->flags |= PTreeEffFlags( node->u.subtree[1] )
                     | PTreeEffFlags( node->u.subtree[0] );
    }
    return node;
}


#ifndef NDEBUG
bool NodeIsUnaryOp(             // TEST IF UNARY OPERATION OF GIVEN TYPE
    PTREE node,                 // - node
    CGOP operation )            // - operation
{
    DbgVerify( (node->cgop == operation) ? (node->op==PT_UNARY) : 1
             , "NodeIsUnaryOp -- node/operator mismatch" );
    return ___NodeIsOp( node, operation );
}


bool NodeIsBinaryOp(            // TEST IF BINARY OPERATION OF GIVEN TYPE
    PTREE node,                 // - node
    CGOP operation )            // - operation
{
    DbgVerify( (node->cgop == operation) ? (node->op==PT_BINARY) : 1
             , "NodeIsBinaryOp -- node/operator mismatch" );
    return ___NodeIsOp( node, operation );
}
#endif


PTREE NodeBinary(               // MAKE A BINARY NODE
    CGOP op,                    // - operator
    PTREE left,                 // - left operand
    PTREE right )               // - right operand
{
    return set_meaningful_side_effects( PTreeBinary( op, left, right ) );
}


PTREE NodeUnary(                // MAKE A UNARY NODE
    CGOP op,                    // - operator
    PTREE expr )                // - operand
{
    return set_meaningful_side_effects( PTreeUnary( op, expr ) );
}


PTREE NodeUnaryCopy(            // MAKE A UNARY NODE, COPY ATTRIBUTES
    CGOP op,                    // - operator
    PTREE expr )                // - operand
{
    PTREE node;                 // - new node

    node = NodeUnary( op, expr );
    node->type = expr->type;
    node->flags = expr->flags;
    node->flags &= ~PTF_NEVER_PROPPED;
    node = PTreeCopySrcLocation( node, expr );
    return node;
}


static PTREE nodeCommaPropogate(// PROPOGATE DATA FOR COMMA OPERATOR
    PTREE node )                // - CO_COMMA node
{
    PTREE right;                // - right operand

    right = node->u.subtree[1];
    node = PTreeCopySrcLocation( node, right );
    node->type = right->type;
    node->flags = right->flags;
    return set_meaningful_side_effects( node );
}


void NodeUnduplicate(           // UNDUPLICATE A NODE
    PTREE node )                // - node to be undupicated
{
    PTREE partner;              // - partner node
    PTREE subtree;              // - duplicated node
    PTD* deced;                 // - decoration on node to be undupicated

    DbgAssert( node->op == PT_DUP_EXPR );
    subtree = node->u.dup.subtree[0];
    if( subtree != NULL ) {
        node->u.dup.subtree[0] = NULL;
        partner = node->u.dup.node;
        deced = partner->decor;
        PTreeAssign( partner, subtree );
        subtree->op = PT_ERROR;
        PtdInsert( partner, deced );
        PTreeFree( subtree );
    }
}


static PTREE fixupTraverse(     // CLEANUP before freeing
    PTREE node )                // - current node
{
    switch( node->op ) {
      case PT_DUP_EXPR:
        NodeUnduplicate( node );
        break;
      case PT_SYMBOL:
        NodeFreeSearchResult( node );
        break;
    }
    return node;
}


// Algorithm follows CO_COMMA nodes to the right, linking to the parent
// using u.subtree[1].  The original node is marked by setting the
// u.subtree[1] field to NULL.
//
// The rightmost node is then deleted, with at most two subtrees left
// unlinked. These nodes are linked into the tree as follows:
//  - two subtrees: last CO_COMMA node points to a new CO_COMMA node
//    connecting the subtrees
//  - one subtree: last CO_COMMA node points at node
//  - no subtrees: last CO_COMMA is replaced by the subtree
//
// The CO_COMMA nodes are reversed linked to restore a proper parse tree. As
// each node is unlinked, the node has a type and flags propogated from the
// daughter subtrees.
//
//      BEFORE:                      AFTER:     - ',' nodes are optional    |
//                                              - exp-a, exp-b may be NULL, |
//           ,                             ,      in which case the tree    |
//          / \                           / \     collapses                 |
//         /   \                         /   \                              |
//      exp-1   ,                      exp-1  ,                             |
//             / \                           / \                            |
//            /   \                         /   \                           |
//         exp-2   .                     exp-2   .                          |
//                  .                             .                         |
//                   ,                             ,                        |
//                  / \                           / \                       |
//                 /   \                         /   \                      |
//              exp-n  node                   exp-n   ,                     |
//                     / \                           / \                    |
//                    /   \                         /   \                   |
//                 exp-a exp-b                   exp-a  exp-b               |
//                                                                          |
PTREE NodePruneTop(             // PRUNE TOP OPERAND NODE
    PTREE curr )                // - expression
{
    PTREE last;                 // - last node
    PTREE next;                 // - next node
    PTREE dlt;                  // - a node to be deleted

    if( curr == NULL ) return curr;
    last = NULL;
    while( NodeIsBinaryOp( curr, CO_COMMA ) ) {
        next = curr->u.subtree[1];
        curr->u.subtree[1] = last;
        last = curr;
        curr = next;
    }
    if( curr != NULL ) {
        dlt = NULL;
        switch( curr->op ) {
          case PT_DUP_EXPR :
            dlt = fixupTraverse( curr );
            break;
          case PT_ERROR :
          case PT_STRING_CONSTANT :
          case PT_INT_CONSTANT :
          case PT_FLOATING_CONSTANT :
          case PT_TYPE :
          case PT_ID :
          case PT_SYMBOL :
            dlt = curr;
            curr = NULL;
            break;
          case PT_UNARY :
            dlt = curr;
            curr = curr->u.subtree[0];
            break;
          case PT_BINARY :
            if( curr->u.subtree[0] == NULL ) {
                dlt = curr;
                curr = curr->u.subtree[1];
            } else if( curr->u.subtree[1] == NULL ) {
                dlt = curr;
                curr = curr->u.subtree[0];
            } else {
                curr->cgop = CO_COMMA;
                curr = nodeCommaPropogate( curr );
                dlt = NULL;
            }
            break;
        }
        PTreeFree( dlt );
    }
    if( last != NULL ) {
        if( curr == NULL ) {
            next = last->u.subtree[1];
            curr = last->u.subtree[0];
            PTreeFree( last );
            last = next;
        }
        while( last != NULL ) {
            next = last->u.subtree[1];
            last->u.subtree[1] = curr;
            curr = last;
            curr = nodeCommaPropogate( curr );
            last = next;
        }
    }
    return curr;
}


static PTREE pruneSubtree(      // PRUNE A SUBTREE (KEEP COMMA'D EXPR'S)
    PTREE expr,                 // - expression
    PTREE *subtree )            // - ref[ subtree of expression ]
{
    PTREE dlt;                  // - subtree to be pruned (with commas)
    PTREE *ref;                 // - ref[ actual tree to be pruned ]

    ref = PTreeRef( subtree );
    dlt = *subtree;
    NodeFreeDupedExpr( *ref );
    *subtree = NULL;
    if( subtree != ref ) {
        *ref = expr;
        expr = dlt;
    }
    return expr;
}


PTREE NodePruneLeft(            // PRUNE LEFT OPERAND
    PTREE expr )                // - expression
{
    return pruneSubtree( expr, &expr->u.subtree[0] );
}


PTREE NodePruneRight(           // PRUNE RIGHT OPERAND
    PTREE expr )                // - expression
{
    return pruneSubtree( expr, &expr->u.subtree[1] );
}


PTREE NodePruneLeftTop(         // PRUNE LEFT, THEN TOP
    PTREE expr )                // - expression to be pruned
{
    return NodePruneTop( NodePruneLeft( expr ) );
}


// This routine should be called to free a tree when the tree can
// contain PT_DUP_EXPR nodes whose partner is not in the tree to be
// freed. This is because PTreeFreeSubtrees will free the duplicated
// expression before encountering the PT_DUP_EXPR node.
// (notice the use of a prefix traversal to handle the case of cleaning
//  up dup nodes that duplicate subtrees with dup nodes in them)
//
void NodeFreeDupedExpr(         // FREE AN EXPRESSION WITH DUPLICATES
    PTREE expr )                // - expression to be freed
{
    PTreeFreeSubtrees( PTreeTraversePrefix( expr, &fixupTraverse ) );
}


PTREE NodeReplace(              // REPLACE EXPRESSION WITH ANOTHER
    PTREE old,                  // - expression to be replaced
    PTREE replace )             // - replacement expression
{
    PTREE *ref;                 // - reference[ rightmost CO_COMMA operand ]

    if( old != NULL ) {
        old = PTreeCopySrcLocation( old, replace );
        ref = PTreeRef( &old );
        NodeFreeDupedExpr( *ref );
        *ref = replace;
        replace = old;
    }
    return replace;
}


PTREE NodeReplaceTop(           // REPLACE TOP EXPRESSION WITH ANOTHER
    PTREE old,                  // - expression to be replaced
    PTREE replace )             // - replacement expression
{
    if( old != NULL ) {
        old = PTreeCopySrcLocation( old, replace );
        old = NodePruneTop( old );
        replace = NodeComma( old, replace );
    }
    return replace;
}


static PTREE nodeMakeConvert(   // MAKE A CONVERSION NODE
    TYPE type,                  // - type for conversion
    PTREE expr )                // - expression to be converted
{
#if 0
#define PTF_CONVERT ( PTF_SIDE_EFF     \
                    | PTF_MEANINGFUL   \
                    | PTF_MEMORY_EXACT \
                    | PTF_PTR_NONZERO  )

    PTREE orig;                 // - original node
    PTREE cast;                 // - cast node
    PTF_FLAG flags;             // - flags for cast node

    orig = expr;
    cast = PTreeType( type );
    cast = PTreeCopySrcLocation( cast, orig );
    expr = NodeBinary( CO_CONVERT, cast, orig );
    expr = PTreeCopySrcLocation( expr, orig );
    expr = NodeSetType( expr, type, orig->flags & PTF_CONVERT );
    if( flags & PTF_MEMORY_EXACT ) {
        TYPE t_conv = TypedefModifierRemoveOnly( NodeType( expr ) );
        TYPE t_orig = TypedefModifierRemoveOnly( NodeType( orig ) );
        if( t_conv != t_orig ) {
            t_conv = TypeReferenced( t_conv );
            t_orig = TypeReferenced( t_orig );
            if( t_conv != t_orig ) {
                expr->flags &= ~ PTF_MEMORY_EXACT;
            }
        }
    }
    expr = FoldBinary( expr );
    return expr;

#undef PTF_CONVERT
#else
    PTREE orig = expr;
    expr = AddCastNode( expr, type );
    if( expr->flags & PTF_MEMORY_EXACT ) {
        TYPE t_conv = TypedefModifierRemoveOnly( NodeType( expr ) );
        TYPE t_orig = TypedefModifierRemoveOnly( NodeType( orig ) );
        if( t_conv != t_orig ) {
            t_conv = TypeReferenced( t_conv );
            t_orig = TypeReferenced( t_orig );
            if( t_conv != t_orig ) {
                expr->flags &= ~ PTF_MEMORY_EXACT;
            }
        }
    }
    expr = FoldBinary( expr );
    return expr;
#endif
}


PTREE NodeConvert(              // MAKE A CONVERSION NODE IF REQ'D
    TYPE type,                  // - type for conversion
    PTREE expr )                // - expression to be converted
{
    if( ( ! TypesIdentical( NodeType( expr ), type )
      ||( ( expr->flags & PTF_LVALUE )
        &&( NULL == TypeReference( type ) ) ) ) ) {
        expr = nodeMakeConvert( type, expr );
    }
    return expr;
}


PTREE NodeConvertFlags(         // MAKE A CONVERSION NODE WITH FLAGS, IF REQ'D
    TYPE type,                  // - type for conversion
    PTREE expr,                 // - expression to be converted
    PTF_FLAG flags )            // - flags to be added
{
    if( flags & PTF_LVALUE ) {
        if( NULL == TypeReference( type ) ) {
            type = MakeReferenceTo( type );
        }
    }
    expr = NodeConvert( type, expr );
    expr = NodeSetType( expr, type, flags );
    return expr;
}


PTREE NodeSetBooleanType(       // SET NODE TO TYPE OF A REL-OP EXPR
    PTREE expr )                // - expression
{
    expr->flags |= PTF_BOOLEAN;
    expr->type = GetBasicType( TYP_BOOL );
    return expr;
}


PTREE NodeCompareToZero(        // MAKE A COMPARE-TO-ZERO NODE, IF REQ'D
    PTREE expr )
{
    PTREE zero;                 // - constant node (contains zero)
    PTREE operand;              // - operand to be compared
    TYPE type;                  // - expression type, unmodified

    if( expr->flags & PTF_BOOLEAN ) {
        return expr;
    }
    if( PTreeOpFlags( expr ) & PTO_BOOLEAN ) {
        return expr;
    }
    type = TypeReferenced( expr->type );
    type = TypedefModifierRemoveOnly( type );
    if( type->id == TYP_VOID ) {
        PTreeErrorExpr( expr, ERR_EXPR_IS_VOID );
    } else {
        if( ( NULL == StructType( type ) )
          &&( NULL == MemberPtrType( type ) ) ) {
            expr = NodeRvalue( expr );
            if( ! ArithType( type ) ) {
                // may be &fn or array decay to a pointer
                type = expr->type;
            }
        } else {
            type = GetBasicType( TYP_SINT );
        }
        operand = expr;
        zero = NodeIntegralConstant( 0, type );
        expr = NodeBinary( CO_NE, operand, zero );
        expr = PTreeCopySrcLocation( expr, operand );
        expr = NodeSetBooleanType( expr );
        expr = ConvertBoolean( expr );
    }
    return expr;
}

static CNV_DIAG diagConvertToBool = // DIAGNOSIS FOR CONVERT-TO-BOOL NODE
{   ERR_IMPLICIT_CAST_ILLEGAL   // - impossible
,   ERR_CALL_WATCOM             // - ambiguous
,   ERR_CALL_WATCOM
,   ERR_CALL_WATCOM             // - protected violation
,   ERR_CALL_WATCOM             // - private violation
};

PTREE NodeConvertToBool(        // MAKE A CONVERT-TO-BOOL NODE, IF REQ'D
    PTREE expr )
{
    PTREE zero;                 // - constant node (contains zero)
    PTREE operand;              // - operand to be compared
    TYPE type;                  // - expression type, unmodified

    if( expr->flags & PTF_BOOLEAN ) {
        return expr;
    }
    if( PTreeOpFlags( expr ) & PTO_BOOLEAN ) {
        return expr;
    }
    type = TypeReferenced( expr->type );
    type = TypedefModifierRemoveOnly( type );
    if( type->id == TYP_VOID ) {
        PTreeErrorExpr( expr, ERR_EXPR_IS_VOID );
    } else if( type->id == TYP_CLASS ) {
        TYPE bool_type = GetBasicType( TYP_BOOL );
        expr = CastImplicit( expr, bool_type, CNV_EXPR, &diagConvertToBool );
    } else {
        operand = expr;
        zero = NodeIntegralConstant( 0, GetBasicType( TYP_SINT ) );
        expr = NodeBinary( CO_NE, expr, zero );
        expr = PTreeCopySrcLocation( expr, operand );
        expr = AnalyseOperator( expr );
    }
    return expr;
}


PTREE NodeRemoveCasts(          // REMOVE CASTING FROM NODE
    PTREE node )                // - starting node
{
    while( NodeIsBinaryOp( node, CO_CONVERT ) ) {

        node = node->u.subtree[1];

        while( ( node->op == PT_BINARY )
            && ( node->cgop == CO_COMMA )
            && !( node->u.subtree[0]->flags & PTF_SIDE_EFF ) ) {

            node = node->u.subtree[1];
        }
    }

    return node;
}


PTREE NodeRemoveCastsCommas(    // REMOVE COMMAS, DTORING, CASTING FROM NODE
    PTREE node )                // - starting node
{
    PTREE not_used;             // - dtoring, not used

    return *NodeReturnSrc( &node, &not_used );
}


PTREE NodeSymbolNoRef(          // FILL IN NODE FOR A SYMBOL, NO REF. SETTING
    PTREE expr,                 // - node
    SYMBOL sym,                 // - symbol
    SEARCH_RESULT *result )     // - search result
{
    if( NULL == expr ) {
        expr = PTreeAlloc();
    }
    expr->u.symcg.result = result;
    expr->u.symcg.symbol = sym;
    expr->op = PT_SYMBOL;
    expr->flags |= PTF_LVALUE | PTF_LV_CHECKED;
    expr->type = sym->sym_type;
    expr->cgop = CO_NAME_NORMAL;
    if( result != NULL ) {
        ScopeResultErrLocn( result, &expr->locn );
        if( result->sym != NULL ) {
            expr->cgop = CO_NAME_CONVERT;
        }
    }
    return expr;
}


PTREE NodeSymbol(               // CONVERT NODE TO BE SYMBOL
    PTREE expr,                 // - node
    SYMBOL sym,                 // - symbol
    SEARCH_RESULT *result )     // - search result
{
    sym = SymMarkRefed( sym );
    return NodeSymbolNoRef( expr, sym, result );
}


PTREE NodeSymbolCallee(         // MAKE SYMBOL FOR A CALLEE
    PTREE expr,                 // - node
    SYMBOL sym,                 // - symbol
    SEARCH_RESULT *result )     // - search result
{
    if( ! SymIsVirtual( sym ) ) {
        sym = SymMarkRefed( sym );
    }
    return NodeSymbolNoRef( expr, sym, result );
}


PTREE MakeNodeSymbol(           // MAKE PT_SYMBOL NODE FROM SYMBOL
    SYMBOL sym )                // - symbol
{
    sym = SymMarkRefed( sym );
    if( NULL != StructType( sym->sym_type ) ) {
        sym->flag |= PTF_MEMORY_EXACT;
    }
    return NodeSymbolNoRef( NULL, sym, NULL );
}


PTREE NodeMakeCallee(           // MAKE A CALLEE NODE
    SYMBOL func )               // - function
{
    if( ! SymIsVirtual( func ) ) {
        func = SymMarkRefed( func );
    }
    return NodeSymbolNoRef( NULL, func, NULL );
}


PTREE NodeGetConstantNode(      // RETURN CONSTANT-INT NODE
    PTREE node )                // - node
{
    for( ; ; ) {
        if( NodeIsBinaryOp( node, CO_CONVERT ) ) {
            if( NULL == IntegralType( NodeType( node ) ) ) {
                node = NULL;
                break;
            }
            node = node->u.subtree[1];
        } else if( NodeIsBinaryOp( node, CO_COMMA ) ) {
            node = node->u.subtree[1];
        } else {
            break;
        }
    }
    if( node != NULL )
    switch( node->op ) {
      case PT_INT_CONSTANT :
        if( NULL == IntegralType( NodeType( node ) ) ) {
            node = NULL;
        }
        break;
      case PT_SYMBOL :
        if( SymIsConstantInt( node->u.symcg.symbol ) ) {
            node = PTreeOpRight( node );
        } else {
            node = NULL;
        }
        break;
      default :
        node = NULL;
        break;
    }
    return node;
}


bool NodeIsConstantInt(         // TEST IF A CONSTANT INT NODE
    PTREE node )                // - node
{
    bool retn;                  // - TRUE ==> is zero constant

    if( node == NULL ) {
        retn = FALSE;
    } else {
        node = NodeRemoveCasts( PTreeOp( &node ) );
        switch( node->op ) {
          case PT_INT_CONSTANT :
            retn = ( NULL != IntegralType( node->type ) );
            break;
          case PT_SYMBOL :
            retn = SymIsConstantInt( node->u.symcg.symbol );
            break;
          default :
            retn = FALSE;
            break;
        }
    }
    return retn;
}


bool NodeIsConstant(            // TEST IF NODE IS A CONSTANT
    PTREE node )                // - node
{
    bool retn;                  // - TRUE ==> is constant

    if( TypeIsConst( NodeType( node ) ) ) {
        retn = TRUE;
    } else {
        switch( node->op ) {
          case PT_INT_CONSTANT :       // these are typed w/o TF1_CONST
          case PT_STRING_CONSTANT :
          case PT_FLOATING_CONSTANT :
            retn = TRUE;
            break;
          default :
            retn = FALSE;
            break;
        }
    }
    return retn;
}


int NodeConstantValue(  // GET CONSTANT VALUE FOR A NODE
    PTREE node )        // - a constant node
{
    SYMBOL sym;         // - symbol for node
    int retn = 0;       // - return value

    node = NodeRemoveCasts( node );
    switch( node->op ) {
    case PT_INT_CONSTANT :
        retn = node->u.int_constant;
        break;
    case PT_SYMBOL :
        sym = node->u.symcg.symbol;
        retn = sym->u.sval;
        break;
    DbgDefault( "non-constant node passed to NodeConstantValue" );
    }
    return( retn );
}


static bool nodeGetConstant     // TEST IF CONSTANT AND GET VALUE
    ( PTREE node                // - potential constant node
    , INT_CONSTANT* pval )      // - addr[ value ]
{
    bool retn;                  // - return: TRUE ==> is integral constant
    SYMBOL sym;                 // - symbol for node

    if( node == NULL ) {
        retn = FALSE;
    } else {
        node = NodeRemoveCasts( PTreeOp( &node ) );
        switch( node->op ) {
          case PT_INT_CONSTANT :
            pval->type = TypedefModifierRemoveOnly( node->type );
            pval->u.value = node->u.int64_constant;
            retn = TRUE;
            break;
          case PT_SYMBOL :
            sym = node->u.symcg.symbol;
            retn = SymIsConstantInt( sym );
            if( retn ) {
                SymConstantValue( sym, pval );
            }
            break;
          default :
            retn = FALSE;
            break;
        }
    }
    return retn;
}


bool NodeIsIntConstant          // TEST IF INTEGRAL CONSTANT AND GET VALUE
    ( PTREE node                // - potential constant node
    , INT_CONSTANT* pval )      // - addr[ value ]
{
    bool retn;                  // - return: TRUE ==> is integral constant

    if( NULL == IntegralType( node->type ) ) {
        retn = FALSE;
    } else {
        retn = nodeGetConstant( node, pval );
    }
    return retn;
}


bool NodeIsZeroConstant(        // TEST IF A ZERO CONSTANT
    PTREE node )                // - node
{
    bool retn;                  // - TRUE ==> is zero constant
    INT_CONSTANT icon;          // - integral constant

    if( nodeGetConstant( node, &icon ) ) {
        retn = Zero64( &icon.u.value );
    } else {
        retn = FALSE;
    }
    return retn;
}


bool NodeIsZeroIntConstant(     // TEST IF A ZERO INTEGER CONSTANT
    PTREE node )                // - node
{
    bool retn;                  // - TRUE ==> is zero constant
    INT_CONSTANT icon;          // - integral constant

    if( nodeGetConstant( node, &icon ) ) {
        if( ( icon.type->id < TYP_BOOL ) || ( icon.type->id > TYP_ULONG64 ) ) {
            retn = FALSE;
        } else {
            retn = Zero64( &icon.u.value );
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


PTREE NodeFromConstSym(         // BUILD CONSTANT NODE FROM CONSTANT SYMBOL
    SYMBOL con )                // - constant symbol
{
    INT_CONSTANT icon;          // - integral constant
    PTREE retn;                 // - new entry

    con = SymConstantValue( con, &icon );
    retn = PTreeInt64Constant( icon.u.value, TYP_SLONG64 );
    retn->type = SymUnmodifiedType( con );
    return retn;
}


void NodeFreeSearchResult(      // FREE SEARCH_RESULT FROM A NODE
    PTREE node )                // - the node
{
    if( node->op == PT_SYMBOL ) {
        ScopeFreeResult( node->u.symcg.result );
        node->u.symcg.result = NULL;
    }
}


static bool nodeIsAssignment(   // TEST IF NODE IS "=" or initialization
    PTREE node )                // - node
{
    return NodeIsBinaryOp( node, CO_EQUAL )
        || NodeIsBinaryOp( node, CO_INIT )
        || NodeIsBinaryOp( node, CO_EQUAL_REF )
        || NodeIsBinaryOp( node, CO_INIT_REF );
}


static PTREE nodeSimplifyComma( // REMOVE CO_TRASH TO GET RVALUE
    PTREE node )                // - the node
{
    PTREE trash;                // - trash node
    PTREE op;                   // - assignment operator
    PTREE dup;                  // - duplicating node
    PTREE right;                // - node to right of comma

    if( NodeIsBinaryOp( node, CO_COMMA ) ) {
        right = node->u.subtree[1];
        if( right->op == PT_SYMBOL ) {
            op = PTreeOpLeft( node );
            if( nodeIsAssignment( op ) ) {
                dup = PTreeOpLeft( op );
                if( ( dup->op == PT_SYMBOL )
                  &&( dup->u.symcg.symbol == right->u.symcg.symbol ) ) {
                    trash = node;
                    node = node->u.subtree[0];
                    trash->u.subtree[0] = NULL;
                    NodeFreeDupedExpr( trash );
                }
            }
        } else if( ( right->op == PT_DUP_EXPR )
                 &&( right->u.dup.subtree[0] == NULL ) ) {
            op = PTreeOpLeft( node );
            if( nodeIsAssignment( op ) ) {
                dup = PTreeOpLeft( op );
                if( ( dup->op == PT_DUP_EXPR )
                  &&( dup->u.dup.node == right->u.dup.node ) ) {
                    DbgUseless();
                    op->u.subtree[0] = dup->u.dup.subtree[0];
                    PTreeFree( dup );
                    trash = node;
                    node = node->u.subtree[0];
                    trash->u.subtree[0] = NULL;
                    NodeFreeDupedExpr( trash );
                }
            }
        }
    }
    return node;
}


PTREE NodeStripPoints(          // STRIP * APPLIED TO & OPERATION
    PTREE expr )                // - * expression
{
    PTREE left;                 // - object of * operation
    PTREE orig;                 // - original expression
    PTREE orig_left;            // - left operand of original expression

    left = PTreeOpLeft( expr );
    if( NodeIsUnaryOp( left, CO_ADDR_OF ) ) {
        orig = expr;
        orig_left = PTreeOpLeft( orig );
        if( orig_left == left ) {
            expr = left->u.subtree[0];
        } else {
            expr = orig_left;
            expr->u.subtree[1] = left->u.subtree[0];
        }
        PTreeFree( orig );
        PTreeFree( left );
    }
    return expr;
}


static PTREE nodeDoFetch(       // FETCH A VALUE
    PTREE curr,                 // - node to be transformed
    CGOP opcode )               // - fetch opcode
{
    PTREE* ref;                 // - ref[ rightmost ',' operand ]
    PTREE fet;                  // - fetch node
    PTREE old;                  // - node being fetched
    PTF_FLAG mask;              // - propogation mask
    TYPE result_type;           // - resultant type

    if( NodeIsBinaryOp( curr, CO_COMMA ) ) {
        ref = PTreeRef( &curr->u.subtree[1] );
        if( ! NodeIsUnaryOp( *ref, CO_BITFLD_CONVERT ) ) {
            ref = &curr;
        }
    } else {
        ref = &curr;
    }
    old = *ref;
    fet = NodeUnary( opcode, old );
    *ref = fet;
    result_type = TypeReferenced( old->type );
    fet->type = result_type;
    mask = PTF_FETCH;
    if( NULL == PointerTypeEquivalent( result_type ) ) {
        mask &= ~PTF_PTR_NONZERO;
    }
    fet->flags |= old->flags & mask;
    fet = set_meaningful_side_effects( fet );
    PTreeCopySrcLocation( fet, old );
    curr->flags &= PTF_FETCH;
    return curr;
}


static PTREE nodeDoFetchRef(    // DO A REFERENCE-PARAMETER FETCH
    PTREE node )                // - the reference parameter
{
    node->cgop = CO_NAME_PARM_REF;
    return nodeDoFetch( node, CO_RARG_FETCH );
}


PTREE NodeFetch(                // FETCH A VALUE
    PTREE curr )                // - node to be transformed
{
    return NodeStripPoints( nodeDoFetch( curr, CO_FETCH ) );
}


static PTREE nodeRvalueArray(   // CONVERT ARRAY TO POINTER
    PTREE curr )                // - the node
{
    PTREE orig;                 // - original node
    TYPE type = NodeType( curr );
    TYPE unmod = TypeReferenced( type );

    orig = curr;
    curr = nodeMakeConvert( PointerTypeForArray( unmod ), curr );
    if( ( orig->op == PT_SYMBOL )
      &&( ScopeType( SymScope( orig->u.symcg.symbol )
                   , SCOPE_FUNCTION ) ) ) {
        curr = NodeFetch( curr );
    }
    return curr;
}


static PTREE nodeRvalueFetch(   // FETCH RVALUE IF REQUIRED
    PTREE curr )                // - the node
{
    TYPE type = NodeType( curr );
    TYPE unmod = TypeReferenced( type );
    if( NULL != ArrayType( type ) ) {
        curr = nodeRvalueArray( curr );
    } else {
        type = TypedefModifierRemoveOnly( unmod );
        switch( type->id ) {
          case TYP_ARRAY :
            curr = nodeRvalueArray( curr );
            break;
          case TYP_CLASS :
            if( OMR_CLASS_VAL == ObjModelArgument( type ) ) {
                curr->type = unmod;
                curr->flags |= PTF_LVALUE;
                curr = NodeFetch( curr );
                curr->flags &= ~ PTF_LVALUE;
            } else {
                curr = NodeConvertFlags( unmod, curr, PTF_CLASS_RVREF );
            }
            break;
          case TYP_FUNCTION :
            if( curr->op == PT_SYMBOL ) {
                SymMarkRefed( curr->u.symcg.symbol );
            }
            curr->type = MakePointerTo( unmod );
            curr->flags &= ~PTF_LVALUE;
            break;
          default :
            curr->type = unmod;
            curr->flags |= PTF_LVALUE;
            curr = NodeFetch( curr );
            curr->flags &= ~ PTF_LVALUE;
            break;
        }
    }
    return curr;
}


PTREE NodeRvalue(               // GET RVALUE, IF LVALUE
    PTREE curr )                // - node to be transformed
{
    TYPE node_type;             // - type of node
    TYPE sym_type;              // - type of symbol node
    type_flag mod_flags;        // - modifier flags of result
    PTREE orig;                 // - original node

    curr = nodeSimplifyComma( curr );
    node_type = NodeType( curr );
    if( NULL != TypeReference( node_type ) ) {
        if( NodeIsBinaryOp( curr, CO_QUESTION ) ) {
            PTREE colon;        // - right operand
            PTREE new_right;    // - new right of colon
            PTREE new_left;     // - new left of colon
            colon = PTreeOpRight( curr );
            new_left = NodeRvalueLeft( colon );
            new_right = NodeRvalueRight( colon );
            colon->flags &= ~ PTF_LVALUE;
            mod_flags = ( new_right->flags | new_left->flags )
                      & ( PTF_FETCH & ~ PTF_MEANINGFUL );
            colon->flags |= mod_flags;
            curr->flags |= mod_flags;
            curr->flags &= ~ PTF_LVALUE;
            curr->type = new_right->type;
        } else {
            sym_type = curr->type;
            if( curr->op == PT_SYMBOL ) {
                if( curr->cgop == CO_NAME_THIS ) {
                    if( CompFlags.extensions_enabled ) {
                        curr = nodeDoFetchRef( curr );
                    } else {
                        curr = NodeFetch( curr );
                    }
                    curr->flags &= ~ PTF_LVALUE;
                } else if( curr->cgop == CO_NAME_CDTOR_EXTRA ) {
                    orig = curr;
                    curr = NodeIc( IC_CDARG_FETCH );
                    curr->type = TypeReferenced( node_type );
                    curr = PTreeCopySrcLocation( curr, orig );
                    PTreeFree( orig );
                } else if( SymIsConstantInt( curr->u.symcg.symbol ) ) {
                    SYMBOL con;     // constant symbol
                    con = curr->u.symcg.symbol;
                    orig = curr;
                    curr = NodeFromConstSym( con );
                    curr = PTreeCopySrcLocation( curr, orig );
                    NodeFreeSearchResult( orig );
                    PTreeFree( orig );
                } else {
                    curr = nodeRvalueFetch( curr );
                }
            } else {
                curr = nodeRvalueFetch( curr );
            }
            TypeModFlags( sym_type, &mod_flags );
            if( mod_flags & TF1_VOLATILE ) {
                curr->flags |= PTF_SIDE_EFF;
            }
            sym_type = TypedefModifierRemoveOnly( curr->type );
            if( sym_type != curr->type ) {
                switch( sym_type->id ) {
                  default :
                    break;
                  case TYP_BOOL :
                  case TYP_CHAR :
                  case TYP_SCHAR :
                  case TYP_UCHAR :
                  case TYP_WCHAR :
                  case TYP_SSHORT :
                  case TYP_USHORT :
                  case TYP_SINT :
                  case TYP_UINT :
                  case TYP_SLONG :
                  case TYP_ULONG :
                  case TYP_SLONG64 :
                  case TYP_ULONG64 :
                  case TYP_FLOAT :
                  case TYP_DOUBLE :
                  case TYP_LONG_DOUBLE :
                  case TYP_ENUM :
                  case TYP_MEMBER_POINTER :
                    curr->type = sym_type;
                    break;
                }
            }
        }
    } else if( NULL != ArrayType( node_type ) ) {
        curr = nodeRvalueArray( curr );
    }
    return curr;
}


// assumes that propogation from start to mod is thru right comma operand
//
static PTREE nodeRefedRvalue(   // PROPOGATE RVALUE RESULT
    PTREE* r_start )            // - ref[ starting node ]
{
    PTREE start;                // - starting node
    PTREE* r_mod;               // - ref[ modified node ]
    PTREE mod;                  // - modified node
//    TYPE src_type;              // - starting (source) type

//    src_type = (*r_start)->type;
    r_mod = PTreeRef( r_start );
    mod = NodeRvalue( *r_mod );
    *r_mod = mod;
    for( start = *r_start; start != mod; start = start->u.subtree[1] ) {
        start->flags = mod->flags;
        start->type = mod->type;
    }
    return mod;
}


PTREE NodeRvalueLeft(           // SET RVALUE ON LEFT
    PTREE node )                // - current node
{
    return nodeRefedRvalue( &node->u.subtree[0] );
}


PTREE NodeRvalueRight(          // SET RVALUE ON RIGHT
    PTREE node )                // - current node
{
    return nodeRefedRvalue( &node->u.subtree[1] );
}


PTREE NodeRvalueExact(          // SET RVALUE (EXACT)
    PTREE node )                // - current node
{
    TYPE exact_type;            // - exact type

    exact_type = node->type;
    node = NodeRvalue( node );
    switch( TypedefModifierRemove( exact_type )->id ) {
      case TYP_BOOL :
      case TYP_CHAR :
      case TYP_SCHAR :
      case TYP_UCHAR :
      case TYP_WCHAR :
      case TYP_SSHORT :
      case TYP_USHORT :
      case TYP_SINT :
      case TYP_UINT :
      case TYP_SLONG :
      case TYP_ULONG :
      case TYP_SLONG64 :
      case TYP_ULONG64 :
      case TYP_FLOAT :
      case TYP_DOUBLE :
      case TYP_LONG_DOUBLE :
      case TYP_ENUM :
        node->type = exact_type;
        break;
    }
    return node;
}


PTREE NodeRvalueExactLeft(      // SET RVALUE (EXACT) ON LEFT
    PTREE node )                // - current node
{
    return NodeRvalueExact( nodeRefedRvalue( &node->u.subtree[0] ) );
}


PTREE NodeRvalueExactRight(     // SET RVALUE (EXACT) ON RIGHT
    PTREE node )                // - current node
{
    return NodeRvalueExact( nodeRefedRvalue( &node->u.subtree[1] ) );
}


PTREE NodeCDtorArg(             // BUILD CONSTANT NODE FOR CDTOR EXTRA ARG
    target_offset_t code )      // - the code
{
    return NodeIntegralConstant( code, MakeCDtorExtraArgType() );
}


PTREE NodeIntegralConstant      // BUILD AN INTEGRAL NODE FOR A VALUE
    ( int val                   // - value
    , TYPE type )               // - node type (integral,enum,ptr)
{
    PTREE retn;                 // - result
    type_id id;                 // - id for constant

    type = TypedefModifierRemoveOnly( type );
    switch( type->id ) {
    case TYP_POINTER:
    case TYP_FUNCTION:
    case TYP_ARRAY:
        id = TYP_SINT;
        break;
    default:
        id = TypedefModifierRemove( type )->id;
    }
    retn = PTreeIntConstant( val, id );
    retn->type = type;
    return retn;
}


PTREE NodeOffset(               // BUILD CONSTANT NODE FOR AN OFFSET
    target_offset_t offset )    // - the offset
{
    TYPE otype;

    otype = GetBasicType( TYP_UINT );
    return NodeIntegralConstant( offset, otype );
}


PTREE NodeArgument(             // MAKE AN ARGUMENT NODE
    PTREE left,                 // - left subtree
    PTREE right )               // - right subtree
{
    PTREE arg;                  // - the argument

    arg = NodeBinary( CO_LIST, left, right );
    arg->type = right->type;
    arg->flags = right->flags;
    arg = PTreeCopySrcLocation( arg, right );
    return arg;
}


PTREE NodeArg(                  // MAKE A SINGLE ARGUMENT NODE
    PTREE argval )              // - value for argument
{
    return NodeArgument( NULL, argval );
}


PTREE NodeArguments(            // MAKE A LIST OF ARGUMENTS
    PTREE first,                // - first arg
    ... )                       // - NULL terminated, in reverse order
{
    PTREE expr;                 // - expression for arg.s
    PTREE argument;             // - next argument
    va_list args;

    va_start( args, first );
    expr = NULL;
    argument = first;
    while( argument != NULL ) {
        expr = NodeArgument( expr, argument );
        argument = va_arg( args, PTREE );
    }
    va_end( args );
    return expr;
}


static bool nodeIsFetchSym(     // TEST IF NODE IS FETCH OF A SYMBOL
    PTREE expr )                // - expression
{
    bool retn;                  // - TRUE ==> is a fetch

    if( NodeIsUnaryOp( expr, CO_RARG_FETCH ) ) {
        retn = TRUE;
    } else if( NodeIsUnaryOp( expr, CO_FETCH )
            && expr->u.subtree[0]->op == PT_SYMBOL ) {
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return retn;
}


static PTREE makeDupNode(       // DUPLICATE THE EXPRESSION
    PTREE *expr )               // - addr( expression )
{
    PTREE node;                 // - new node (partner)
    PTREE node_expr;            // - new node (expression)
    PTREE old;                  // - old node

    old = *expr;
    node_expr = PTreeDupExpr( old );
    *expr = node_expr;
    node = PTreeDupExpr( old );
    node->u.dup.node = node_expr;
    node_expr->u.dup.node = node;
    return node;
}


PTREE NodeDupExpr(              // DUPLICATE EXPRESSION
    PTREE *expr )               // - addr( expression )
{
    PTREE node;                 // - new node (partner)
    PTREE old;                  // - old node

    if( (*expr)->flags & PTF_SIDE_EFF ) {
        node = makeDupNode( expr );
    } else {
        old = PTreeOp( expr );
        switch( old->op ) {
          case PT_INT_CONSTANT :
          case PT_STRING_CONSTANT :
          case PT_FLOATING_CONSTANT :
          case PT_SYMBOL :
            node = PTreeAssign( NULL, old );
            break;
          case PT_UNARY :
            if( nodeIsFetchSym( old ) ) {
                node = PTreeAssign( NULL, old );
                node->u.subtree[0] = PTreeAssign( NULL, old->u.subtree[0] );
                break;
            }
            // drops thru
          default :
            node = makeDupNode( expr );
            break;
        }
    }
    return node;
}


bool NodeBitField(              // TEST IF NODE IS A BIT FIELD
    PTREE node )                // - the node
{
    bool retn;                  // - TRUE ==> is a bit field

    for( ; ; node = PTreeOpLeft( node ) ) {
        if( node == NULL ) {
            retn = FALSE;
            break;
        } else if( 0 == ( node->flags & PTF_LVALUE ) ) {
            retn = FALSE;
            break;
        } else if( node->op == PT_UNARY ) {
            if( node->cgop == CO_BITFLD_CONVERT ) {
                retn = TRUE;
                break;
            }
        } else if( node->op == PT_BINARY ) {
            if( ( node->cgop == CO_DOT )
              ||( node->cgop == CO_DOT_STAR )
              ||( node->cgop == CO_ARROW )
              ||( node->cgop == CO_ARROW_STAR )
              ||( node->cgop == CO_COLON_COLON ) ) {
                retn = FALSE;
                break;
            }
        } else {
            retn = FALSE;
            break;
        }
    }
    return retn;
}


PTREE NodeComma(                // MAKE A COMMA PTREE NODE
    PTREE left,                 // - left operand
    PTREE right )               // - right operand
{
    PTREE node;                 // - new node

    if( left == NULL ) {
        node = right;
    } else if( right == NULL ) {
        node = left;
    } else {
        node = NodeBinary( CO_COMMA, left, right );
        node = nodeCommaPropogate( node );
    }
    return node;
}


PTREE NodeCommaIfSideEffect(    // MAKE A COMMA PTREE NODE (IF LHS HAS side-effects)
    PTREE left,                 // - left operand
    PTREE right )               // - right operand
{
    PTREE node;                 // - new node

    if( left == NULL ) {
        node = right;
    } else if( right == NULL ) {
        node = left;
    } else {
        if(( left->flags & PTF_SIDE_EFF ) != 0 ) {
            node = NodeBinary( CO_COMMA, left, right );
            node = nodeCommaPropogate( node );
        } else {
            NodeFreeDupedExpr( left );
            node = right;
        }
    }
    return node;
}


TYPE NodeSetReference(          // MAKE AN LVALUE IF REFERENCE TYPE
    TYPE type,                  // - type
    PTREE node )                // - node
{
    if( type != NULL ) {
        if( NULL != TypeReference( type ) ) {
            node->flags |= PTF_LVALUE;
        }
    }
    return type;
}


PTREE NodeThis(                 // MAKE A "THIS" NODE
    void )
{
    TYPE type;                  // - type of "this" node
    PTREE node;                 // - "this" node

    type = TypeThis();
    if( type == NULL ) {
        node = NULL;
    } else {
        TYPE cl_type;
        node = PTreeThis();
        node->op = PT_SYMBOL;
        node->type = type;
        node->flags |= PTF_LVALUE | PTF_PTR_NONZERO | PTF_LV_CHECKED;
        node->u.symcg.symbol = NULL;
        node->u.symcg.result = NULL;
        node = NodeRvalue( node );
        node->flags |= PTF_PTR_NONZERO | PTF_LV_CHECKED;
        cl_type = StructType( TypePointedAtModified( type ) );
        if( ! TypeHasVirtualBases( cl_type )
         && SymIsCtorOrDtor( ScopeFunctionInProgress() ) ) {
            node->flags |= PTF_MEMORY_EXACT;
        }
    }
    return node;
}

PTREE NodeThisCopyLocation(     // MAKE A RVALUE "THIS" NODE WITH LOCATION
    PTREE use_locn )            // - node to grab locn from
{
    PTREE this_node;

    this_node = NodeThis();
    if( this_node != NULL && use_locn != NULL ) {
        this_node = PTreeCopySrcLocation( this_node, use_locn );
    }
    return this_node;
}


PTREE NodeCDtorExtra(           // MAKE A CTOR/DTOR EXTRA PARM NODE
    void )
{
    PTREE node;                 // - "this" node

    node = PTreeCDtorExtra();
    node->op = PT_SYMBOL;
    node->type = MakeCDtorExtraArgType();
    node->flags |= PTF_LVALUE | PTF_LV_CHECKED;
    node->u.symcg.symbol = NULL;
    node->u.symcg.result = NULL;
    node = NodeRvalue( node );
    return node;
}


static PTREE assignNode(        // CREATE ASSIGNMENT NODE
    PTREE tgt,                  // - target
    PTREE src,                  // - source
    CGOP opcode )               // - opcode
{
    PTREE expr;                 // - result

    expr = NodeBinary( opcode, tgt, src );
    expr->type = tgt->type;
    expr->flags |= PTF_LVALUE | PTF_LV_CHECKED;
    expr = PTreeCopySrcLocation( expr, src );
    return expr;
}


PTREE NodeAssign(               // CREATE ASSIGNMENT NODE FOR VALUE
    PTREE tgt,                  // - target
    PTREE src )                 // - source
{
    return assignNode( tgt, src, CO_EQUAL );
}


PTREE NodeAssignRef(            // CREATE ASSIGNMENT NODE FOR REFERENCE
    PTREE tgt,                  // - target
    PTREE src )                 // - source
{
    return assignNode( tgt, src, CO_EQUAL_REF );
}


PTREE NodeTemporary(            // CREATE TEMPORARY AND NODE FOR IT
    TYPE type )                 // - type of temporary
{
    return MakeNodeSymbol( TemporaryAlloc( type ) );
}


PTREE NodeAssignTemporaryNode(  // ASSIGN NODE TO A TEMPORARY NODE
    TYPE type,                  // - type of temporary
    PTREE expr,                 // - the expression to be assigned to temp
    PTREE temp_node )           // - node for temporary symbol
{
    TYPE cl_type;               // - class type

    temp_node = PTreeCopySrcLocation( temp_node, expr );
    expr = PTreeExtractLocn( expr, &temp_node->locn );
    cl_type = ClassTypeForType( type );
    if( cl_type != NULL ) {
        expr = ClassCopyTemp( TypeReferenced( type ), expr, temp_node );
    } else {
        if( NULL == TypeReference( type ) ) {
            expr = NodeAssign( temp_node, expr );
            if( expr->op != PT_ERROR
             && NULL != MemberPtrType( type ) ) {
                PTREE a_expr = expr;
                MembPtrAssign( &a_expr );
                expr = a_expr;
            }
        } else {
            expr = NodeAssignRef( temp_node, expr );
        }
    }
    if( expr->op != PT_ERROR ) {
        expr->flags |= PTF_MEMORY_EXACT;
        expr->locn = temp_node->locn;
    }
    return expr;
}


PTREE NodeAssignTemporary(      // ASSIGN NODE TO A TEMPORARY
    TYPE type,                  // - type of temporary
    PTREE expr )                // - the expression to be assigned to temp
{
    return NodeAssignTemporaryNode( type, expr, NodeTemporary( type ) );
}


PTREE NodeDone(                 // MAKE A NODE-DONE
    PTREE expr )                // - expression
{
    if( expr->op != PT_ERROR ) {
        expr = nodeSimplifyComma( expr );
        expr = NodeUnaryCopy( CO_EXPR_DONE, expr );
    }
    return expr;
}


PTREE NodeFetchReference(       // FETCH A REFERENCE, IF REQ'D
    PTREE expr )                // - expression
{
    TYPE type;                  // - node type
    TYPE type_refd;             // - type of reference

    type = expr->type;
    type_refd = TypeReference( type );
    if( expr->op == PT_SYMBOL
     && ( type_refd != NULL || OMR_CLASS_REF == ObjModelArgument( type ) )
     && SymIsArgument( expr->u.symcg.symbol )
     && ( TF1_PLUSPLUS
        & FunctionDeclarationType( ScopeFunctionInProgress()->sym_type )
          ->flag ) ) {
        expr = nodeDoFetchRef( expr );
        if( type_refd == NULL ) {
            expr->type = type;
        } else {
            expr->type = type_refd;
        }
        expr->flags |= PTF_LVALUE;
    } else if( type_refd != NULL ) {
        expr = NodeFetch( expr );
        expr->type = type_refd;
        expr->flags |= PTF_LVALUE;
    }
    return expr;
}


PTREE NodeCopyClassObject(      // COPY OBJECT W/O CTOR
    PTREE tgt,                  // - target object (LVALUE)
    PTREE src )                 // - source object (RVALUE)
{
    PTREE expr;                 // - created expression

    DbgVerify( tgt->flags & PTF_LVALUE, "NodeCopyClassObject to non-lvalue" );
    tgt->flags |= PTF_MEMORY_EXACT;
    expr = NodeBinary( CO_COPY_OBJECT, tgt, src );
    expr->type = tgt->type;
    expr->flags |= PTF_LVALUE
                 | PTF_SIDE_EFF
                 | PTF_MEANINGFUL
                 | PTF_MEMORY_EXACT;
    return expr;
}


PTREE CallArgumentExactCtor(    // GET EXACT CTOR ARG., IF REQUIRED
    TYPE type,                  // - type for class
    bool exact )                // - TRUE ==> exact CTORing of classes
{
    PTREE arg;                  // - NULL or CDTOR argument
    unsigned ctor_code;         // - code for cdtor'ing

    if( TypeRequiresCtorParm( type ) ) {
        if( exact ) {
            ctor_code = CTOR_NULL;
        } else {
            ctor_code = CTOR_COMPONENT;
        }
        arg = NodeArg( NodeCDtorArg( ctor_code ) );
    } else {
        arg = NULL;
    }
    return arg;
}


PTREE NodeArgumentExactCtor(    // ADD EXACT CTOR ARG., IF REQUIRED
    PTREE args,                 // - other arguments
    TYPE type,                  // - type for class
    bool exact )                // - TRUE ==> exact CTORing of classes
{
    PTREE arg = CallArgumentExactCtor( type, exact );

    if( arg != NULL ) {
        arg->u.subtree[0] = args;
        args = arg;
    }
    return args;
}


static addr_func_t checkFunction(   // CHECK IF FUNCTION
    PTREE fun )                     // - potential function node
{
    addr_func_t retn;               // - return: ADDR_FN_...
    SYMBOL sym;                     // - symbol for function

    if( fun->op == PT_SYMBOL ) {
        sym = fun->u.symcg.symbol;
        if( sym == NULL ) {
            retn = ADDR_FN_NONE;
        } else if( SymIsFunction( sym ) ) {
            if( IsActualOverloadedFunc( sym, fun->u.symcg.result ) ) {
                retn = ADDR_FN_MANY;
            } else {
                retn = ADDR_FN_ONE;
            }
        } else {
            retn = ADDR_FN_NONE;
        }
    } else {
        retn = ADDR_FN_NONE;
    }
    return retn;
}


addr_func_t NodeAddrOfFun(      // GET PTREE FOR &FUN (FUN IS OVERLOADED)
    PTREE oper,                 // - expression
    PTREE *addr_func )          // - addr[ function ]
{
    addr_func_t retn;           // - return: ADDR_FN_...

    retn = ADDR_FN_NONE;
    *addr_func = NULL;
    if( oper != NULL ) {
        switch( oper->op ) {
          case PT_UNARY :
            if( oper->cgop == CO_ADDR_OF ) {
                oper = PTreeOpLeft( oper );
                retn = checkFunction( oper );
                switch( retn ) {
                  case ADDR_FN_ONE :
                  case ADDR_FN_MANY :
                  case ADDR_FN_ONE_USED :
                  case ADDR_FN_MANY_USED :
                    *addr_func = oper;
                    break;
                }
            }
            break;
          case PT_BINARY :
            if( oper->cgop == CO_ARROW
             || oper->cgop == CO_DOT ) {
                oper = PTreeOpRight( oper );
                // drops thru
            } else {
                break;
            }
          case PT_SYMBOL :
            retn = checkFunction( oper );
            switch( retn ) {
              case ADDR_FN_ONE :
                retn = ADDR_FN_ONE_USED;
                // drops thru
              case ADDR_FN_ONE_USED :
                *addr_func = oper;
                break;
              case ADDR_FN_MANY :
                retn = ADDR_FN_MANY_USED;
                // drops thru
              case ADDR_FN_MANY_USED :
                *addr_func = oper;
                break;
            }
            break;
        }
    }
    return retn;
}


bool NodePtrNonZero(            // TEST IF A PTR NODE IS ALWAYS NON-ZERO
    PTREE node )                // - node to be tested
{
    bool non_zero;              // - TRUE ==> is non-zero

    if( node->flags & PTF_PTR_NONZERO ) {
        non_zero = TRUE;
    } else if( node->op == PT_SYMBOL ) {
        if( node->flags & PTF_LVALUE ) {
            non_zero = TRUE;
        } else {
            non_zero = FALSE;
        }
    } else {
        non_zero = FALSE;
    }
    return non_zero;
}


PTREE NodeTestExpr(             // GENERATE A TERNARY TEST EXPRESSION
    PTREE b_expr,               // - bool expression
    PTREE t_expr,               // - TRUE expression
    PTREE f_expr )              // - FALSE expression
{
    PTREE expr;
    TYPE type;

    type = t_expr->type;
    expr = NodeBinary( CO_COLON, t_expr, f_expr );
    expr->type = type;
    expr = NodeBinary( CO_QUESTION, b_expr, expr );
    expr->type = type;
    return expr;
}


TYPE NodeType(                  // GET TYPE FOR A NODE
    PTREE node )                // - the node
{
    TYPE type;                  // - type for the node

    type = node->type;
    if( ( node->flags & PTF_LVALUE )
      &&( NULL != type )
      &&( NULL == TypeReference( type ) ) ) {
        type = MakeReferenceTo( type );
    }
    return type;
}


PTREE NodeDtorExpr(             // MARK FOR DTOR'ING AFTER EXPRESSION
    PTREE expr,                 // - expression computing symbol
    SYMBOL sym )                // - SYMBOL being computed
{
    PTREE orig;                 // - original expression
    PTREE dtored;               // - node for symbol to be DTOR'd
    TOKEN_LOCN err_locn;        // - location, when errors
    SYMBOL dtor;                // - dtor symbol

    if( ( expr->op != PT_ERROR ) && SymRequiresDtoring( sym ) ) {
        sym->flag |= SF_ADDR_TAKEN;
        orig = expr;
        if( ! SymIsModuleDtorable( sym ) ) {
            sym->flag |= SF_CG_ADDR_TAKEN;
            SymScope( sym )->u.s.dtor_reqd = TRUE;
        }
        PTreeExtractLocn( expr, &err_locn );
        dtor = DtorFindLocn( sym->sym_type, &err_locn );
        if( NULL == dtor ) {
            PTreeErrorNode( expr );
        } else if( TypeTruncByMemModel( expr->type ) ) {
            PTreeErrorExpr( expr, ERR_DTOR_OBJ_MEM_MODEL );
        } else if( TypeExactDtorable( sym->sym_type ) ) {
//          CDtorScheduleArgRemap( dtor );
            SymMarkRefed( dtor );
            dtored = MakeNodeSymbol( sym );
            dtored->cgop = CO_NAME_DTOR_SYM;
            expr = NodeBinary( CO_DTOR, dtored, expr );
            expr->locn = err_locn;
            expr->type = orig->type;
            expr->flags = orig->flags;
        }
    }
    return expr;
}


PTREE NodeSetMemoryExact(       // SET PTF_MEMORY_EXACT, IF REQ'D
    PTREE expr )                // - expression
{
    if( StructType( expr->type ) != NULL ) {
        expr->flags |= PTF_MEMORY_EXACT;
    }
    return expr;
}


PTREE NodeBasedStr(             // BUILD EXPRESSION FOR TF1_BASED_STRING TYPE
    TYPE expr_type )            // - TF1_BASED_STRING type
{
    PTREE node;                 // - new node
    CGVALUE segid;              // - segment id

    segid.uvalue = SegmentFindBased( expr_type );
    node = PTreeIc( IC_SEGOP_SEG, segid );
    node->type = TypeSegmentShort();
    return node;
}


static CNV_DIAG diag_deref =    // diagnosis for de-referencing
{   ERR_CALL_WATCOM
,   ERR_CALL_WATCOM
,   ERR_CALL_WATCOM
,   ERR_CALL_WATCOM
,   ERR_CALL_WATCOM
};


bool NodeDerefPtr(              // DEREFERENCE A POINTER
    PTREE *a_ptr )              // - addr[ ptr operand ]
{
    bool retn;                  // - TRUE ==> all ok
    PTREE ptr;                  // - ptr operand

    ptr = *a_ptr;
    ptr = NodeRvalue( ptr );
    if( TypeIsBasedPtr( ptr->type ) ) {
        ptr = CastImplicit( ptr
                          , TypeConvertFromPcPtr( ptr->type )
                          , CNV_EXPR
                          , &diag_deref );
        retn = ( PT_ERROR != ptr->op );
    } else {
        retn = TRUE;
    }
    *a_ptr = ptr;
    return retn;
}


bool NodeCallsCtor(             // DETERMINE IF NODE CALLS CTOR
    PTREE node )                // - a call node
{
    bool retn;                  // - TRUE ==> calling a ctor
    if( NodeIsBinaryOp( node, CO_CALL_EXEC )
     || NodeIsBinaryOp( node, CO_CALL_NOOVLD ) ) {
        retn = SymIsCtor( node->u.subtree[0]->u.subtree[0]->u.symcg.symbol );
    } else {
        retn = FALSE;
    }
    return retn;
}


PTREE NodeActualNonOverloaded(  // POSITION OVER DEFAULT-ARG SYMBOLS
    PTREE node )                // - PT_SYMBOL for function
{
    SYMBOL func;                // - function symbol

    func = ActualNonOverloadedFunc( node->u.symcg.symbol, node->u.symcg.result );
    node->u.symcg.symbol = func;
    node->type = func->sym_type;
    return node;
}


PTREE* NodeReturnSrc(           // GET ADDR OF SOURCE OPERAND RETURNED
    PTREE* src,                 // - addr[ operand ]
    PTREE* dtor )               // - addr[ addr[ CO_DTOR operand ] ]
{
    PTREE node;                 // - current node

    *dtor = NULL;
    for( node = *src; ; ) {
        if( NodeIsBinaryOp( node, CO_CONVERT )
         || NodeIsBinaryOp( node, CO_COMMA ) ) {
            src = &node->u.subtree[1];
            node = *src;
        } else if( NodeIsBinaryOp( node, CO_DTOR ) ) {
            *dtor = node;
            src = &node->u.subtree[1];
            node = *src;
        } else {
            break;
        }
    }
    return src;
}


PTREE* getTempSrc(              // GET ADDR OF SOURCE OPERAND FOR TEMPORARY
    PTREE* src,                 // - addr[ operand ]
    PTREE* dtor )               // - addr[ addr[ CO_DTOR operand ] ]
{
    PTREE node;                 // - current node

    *dtor = NULL;
    for( node = *src; ; ) {
        if( NodeIsBinaryOp( node, CO_CONVERT ) ) {
            if( node->u.subtree[0]->cgop == CO_USER_CAST ) break;
            src = &node->u.subtree[1];
            node = *src;
        } else if( NodeIsBinaryOp( node, CO_COMMA ) ) {
            src = &node->u.subtree[1];
            node = *src;
        } else if( NodeIsBinaryOp( node, CO_DTOR ) ) {
            *dtor = node;
            src = &node->u.subtree[1];
            node = *src;
        } else {
            break;
        }
    }
    return src;
}


static PTREE* nodePossibleTemp( // LOCATE POSSIBLE TEMPORARY LOCATION
    PTREE* src,                 // - addr[ operand ]
    PTREE* dtor )               // - addr[ addr[ CO_DTOR operand ] ]
{
    src = getTempSrc( src, dtor );
    if( NULL != StructType( NodeType( *src ) )
     && NodeIsUnaryOp( *src, CO_FETCH ) ) {
        src = getTempSrc( &(*src)->u.subtree[0], dtor );
    }
    return src;
}


static bool isNonConstRef(      // DETERMINE IF NON-CONSTANT REFERENCE
    TYPE arg_type )             // - type to be checked
{
    TYPE refed;                 // - type referenced

    refed = TypeReference( arg_type );
    return refed != NULL && ! TypeIsConst( refed );
}


static bool nodeMakesTemporary( // CHECK IF NODE PRODUCES A TEMPORARY
    PTREE node )                // - possible temporary
{
    bool retn;                  // - TRUE ==> is invalid
    SYMBOL fun;                 // - function called

    fun = NULL;
    if( NodeIsBinaryOp( node, CO_CALL_EXEC ) ) {
        fun = NodeFuncForCall( node )->u.symcg.symbol;
        if( SymIsCtor( fun ) ) {
            retn = TRUE;
        } else {
            retn = FALSE;
        }
    } else if( NodeIsBinaryOp( node, CO_COPY_OBJECT ) ) {
        node = PTreeOpLeft( node );
#if 0
        DbgVerify( node->op == PT_SYMBOL
                 , "nodeMakesTemporary -- not symbol" );
        retn = SymIsTemporary( node->u.symcg.symbol );
#else
        if( node->op == PT_SYMBOL ) {
            retn = SymIsTemporary( node->u.symcg.symbol );
        } else {
            retn = FALSE;
        }
#endif
    } else {
        retn = FALSE;
    }
    if( !retn && 0 == ( node->flags & PTF_LVALUE ) && NULL == TypeReference( node->type ) ) {
        if( fun != NULL ) {
            if( NULL != TypeReference( SymFuncReturnType( fun ) ) ) {
                retn = FALSE;
            } else if( SymIsAssign( fun ) ) {
                retn = FALSE;
            } else {
                retn = TRUE;
            }
        } else if( NodeIsBinaryOp( node, CO_CALL_EXEC_IND ) ) {
            TYPE ret_type;
            node = NodeFuncForCall( node );
            ret_type = TypeFunctionCalled( node->type );
            DbgVerify( ret_type != NULL
                     , "nodeMakesTemporary -- not function type" );
            ret_type = ret_type->of;
            if( NULL == TypeReference( ret_type ) ) {
                retn = TRUE;
            } else {
                retn = FALSE;
            }
        } else {
            retn = FALSE;
        }
    }
    return retn;
}


bool NodeNonConstRefToTemp(     // CHECK IF TEMP. PASSED AS NON-CONST REF
    TYPE arg_type,              // - possible non-const reference
    PTREE node )                // - possible temporary
{
    PTREE dtor;                 // - CO_DTOR ( not used )
    PTREE* a_src;               // - addr[ source operand ]
    SYMBOL func;                // - function being compiled

    func = ScopeFunctionInProgress();
    if( ! SymIsThunk( func )
     && isNonConstRef( arg_type ) ) {
        a_src = nodePossibleTemp( &node, &dtor );
        if( nodeMakesTemporary( *a_src ) ) {
            PTreeErrorExpr( node, ANSI_TEMP_USED_TO_INIT_NONCONST_REF );
            if( node->op == PT_ERROR ) {
                return TRUE;
            }
        }
    }
    return FALSE;
}


bool NodeReferencesTemporary(   // CHECK IF NODE PRODUCES OR IS TEMPORARY
    PTREE node )                // - possible temporary
{
    PTREE dtor;                 // - addr CO_DTOR ( not used )
    PTREE* src;                 // - source operand
    bool retn;                  // - TRUE ==> non-const ref && temp. ref.ed

    src = nodePossibleTemp( &node, &dtor );
    if( node->op == PT_SYMBOL
     && SymIsTemporary( node->u.symcg.symbol ) ) {
        retn = TRUE;
    } else {
        retn = nodeMakesTemporary( *src );
    }
    return retn;
}


PTREE NodeSegname(              // BUILD EXPRESSION FOR __segname
    char* segname )             // - name of segment
{
    PTREE node;                 // - new node
    CGVALUE sym;                // - symbol for segment

    sym.pvalue = SegmentLabelSym( SegmentFindNamed( segname ) );
    node = PTreeIc( IC_SEGNAME, sym );
    node->type = TypeSegmentShort();
    return node;
}


static void assignBitDup(      // ASSIGN DUPLICATED BIT FIELD EXPRESSION
    PTREE *lhs,                 // - addr[ lhs expression ]
    PTREE dup )                 // - duplicated assignment
{
    lhs = PTreeRef( lhs );
    dup->u.subtree[0] = *lhs;
    *lhs = dup;
}


PTREE NodeBitQuestAssign(       // ASSIGN (expr?bit-fld:bit-fld) = expr
    PTREE expr )                // - the expression
{
    PTREE result;               // - result expression
    PTREE dup;                  // - dup of RHS + expr
    PTREE colon;                // - actual : expression

    result = expr->u.subtree[0];
    expr->u.subtree[0] = NULL;
    expr->flags &= ~PTF_LVALUE;
    dup = PTreeAssign( NULL, expr );
    dup->u.subtree[1] = NodeDupExpr( &expr->u.subtree[1] );
    colon = PTreeOpRight( PTreeOp( &result ) );
    assignBitDup( &colon->u.subtree[0], expr );
    assignBitDup( &colon->u.subtree[1], dup );
    return result;
}


static bool nonVolatileSymbol(  // TEST IF EXPRESSION IS NON-VOLATILE SYM
    PTREE expr )                // - expression to be tested
{
    bool retn;                  // - TRUE ==> is non-volatile symbol
    type_flag flags;            // - modifier flags

    if( expr->op == PT_SYMBOL ) {
        TypeModFlagsEC( expr->u.symcg.symbol->sym_type, &flags );
        if( flags & TF1_VOLATILE ) {
            retn = FALSE;
        } else {
            retn = TRUE;
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


static PTREE nodeIcCgValue(     // ADD A PTREE NODE
    CGINTEROP opcode,           // - opcode
    CGVALUE value )             // - operand
{
    PTREE node = PTreeIc( opcode, value );
    node->type = GetBasicType( TYP_UINT );
    return node;
}


PTREE NodeIcUnsigned(           // ADD A PTREE-IC NODE, UNSIGNED OPERAND
    CGINTEROP opcode,           // - opcode
    unsigned operand )          // - operand
{
    CGVALUE val;                // - operand

    val.uvalue = operand;
    return nodeIcCgValue( opcode, val );
}


PTREE NodeIc(                   // ADD A PTREE-IC NODE
    CGINTEROP opcode )          // - opcode
{
    return NodeIcUnsigned( opcode, 0 );
}


// This routine cannot use duplicates because the side effects must
// take place in the proper order.
//
PTREE NodeAddSideEffect(        // ADD A SIDE-EFFECT EXPRESSION
    PTREE side_effect,          // - side-effect expression
    PTREE expr )                // - original expression or NULL
{
    PTREE* ref;                 // - reference[ tree, after comma removal ]
    PTREE top;                  // - inspection expression
    PTREE orig;                 // - original node
    SYMBOL temp;                // - temporary
    PTREE dup;                  // - same as expr
    PTREE fetched;              // - fetched operand
    TYPE temp_type;             // - type of temporary
    TEMP_TYPE old;              // - old class of temporary

    if( NULL != expr ) {
        dup = expr;
        ref = PTreeRef( &dup );
        top = *ref;
        if( NodeIsUnaryOp( top, CO_FETCH ) ) {
            fetched = PTreeOpLeft( top );
            if( nonVolatileSymbol( fetched ) ) {
                top = fetched;
            }
        }
        orig = expr;
        if( top->op == PT_SYMBOL
         || NodeIsConstantInt( top ) ) {
            *ref = NodeComma( side_effect, *ref );
            expr = dup;
        } else {
            old = TemporaryClass( TEMP_TYPE_EXPR );
            temp_type = NodeType( expr );
            temp = TemporaryAlloc( temp_type );
            if( NULL == TypeReference( temp_type ) ) {
                if( expr->flags & PTF_CLASS_RVREF ) {
                    temp = TemporaryAlloc( MakeReferenceTo( temp_type ) );
                    expr = NodeAssignRef( MakeNodeSymbol( temp ), expr );
                } else {
                    temp = TemporaryAlloc( temp_type );
                    expr = NodeAssign( MakeNodeSymbol( temp ), expr );
                }
            } else {
                temp = TemporaryAlloc( temp_type );
                expr = NodeAssignRef( MakeNodeSymbol( temp ), expr );
            }
            expr = NodeComma( expr, side_effect );
            side_effect = NodeFetch( MakeNodeSymbol( temp ) );
            if( NULL != TypeReference( temp_type ) ) {
                side_effect->flags |= PTF_LVALUE;
                side_effect->type = TypeReferenced( temp_type );
            }
            expr = NodeComma( expr, side_effect );
            TemporaryClass( old );
        }
        side_effect = expr;
        side_effect->flags = orig->flags;
        side_effect->type = orig->type;
        side_effect = PTreeCopySrcLocation( side_effect, orig );
    }
    return side_effect;
}


PTREE NodeFuncForCall(          // GET FUNCTION NODE FOR CALL
    PTREE call_node )           // - a call node
{
    call_node = call_node->u.subtree[0];
    if( NodeIsBinaryOp( call_node, CO_VIRT_FUNC ) ) {
        call_node = call_node->u.subtree[0];
    }
    return call_node->u.subtree[0];
}


bool NodeGetIbpSymbol(          // GET BOUND-REFERENCE SYMBOL, IF POSSIBLE
    PTREE node,                 // - node
    SYMBOL* a_ibp,              // - bound parameter to use
    target_offset_t* a_offset ) // - addr[ offset to basing symbol ]
{
    bool retn;                  // - TRUE ==> have got one
    PTREE act;                  // - actual node used for access
    SYMBOL bound;               // - bound symbol
    target_offset_t offset;     // - offset to it

    act = PTreeOp( &node );
    if( NULL == node ) {
        bound = NULL;
        offset = 0;
        retn = FALSE;
    } else if( NodeIsUnaryOp( act, CO_RARG_FETCH ) ) {
        act = act->u.subtree[0];
        DbgVerify( act->op == PT_SYMBOL, "NodeGetIbpSymbol -- not symbol" );
        bound = act->u.symcg.symbol;
        offset = 0;
        retn = TRUE;
    } else if( NodeIsBinaryOp( act, CO_DOT ) ) {
        INT_CONSTANT icon;
        PTREE left = act->u.subtree[0];
        PTREE right = act->u.subtree[1];
        if( NodeIsIntConstant( right, &icon )
         && NodeGetIbpSymbol( left, a_ibp, a_offset ) ) {
            bound = *a_ibp;
            offset = *a_offset + icon.u.uval;
            retn = TRUE;
        } else {
            bound = NULL;
            offset = 0;
            retn = FALSE;
        }
    } else if( NodeIsUnaryOp( act, CO_VBASE_FETCH ) ) {
        if( NodeGetIbpSymbol( act->u.subtree[0], a_ibp, a_offset ) ) {
            bound = *a_ibp;
            offset = PtdGetVbOffset( act ) + *a_offset;
            retn = TRUE;
        } else {
            bound = NULL;
            offset = 0;
            retn = FALSE;
        }
    } else {
        bound = NULL;
        offset = 0;
        retn = FALSE;
    }
    *a_offset = offset;
    *a_ibp = bound;
    return retn;
}


PTREE NodeTypeSig               // MAKE NODE FOR TYPE-SIG ADDRESS
    ( TYPE_SIG* sig )           // - type signature
{
    SYMBOL sym;                 // - symbol
    target_offset_t offset;     // - offset to be used
    PTREE node;                 // - node being created

    TypeSigSymOffset( sig, &sym, &offset );
    node = MakeNodeSymbol( sym );
    if( 0 != offset ) {
        PTREE snode = node;
        node = NodeBinary( CO_DOT, snode, NodeOffset( offset ) );
        node->type = snode->type;
        node->flags = snode->flags;
    }
    return node;
}


PTREE NodeTypeSigArg            // MAKE ARGUMENT NODE FOR TYPE-SIG ADDRESS
    ( TYPE_SIG* sig )           // - type signature
{
    return NodeArg( NodeTypeSig( sig ) );
}


PTREE NodeSetType               // SET NODE TYPE, FLAGS
    ( PTREE expr                // - the node
    , TYPE type                 // - new type for node
    , PTF_FLAG flags )          // - new flags
{
    if( type != NULL ) {
        if( NULL != TypeReference( type ) ) {
            flags |= PTF_LVALUE;
            type = TypeReferenced( type );
        }
    }
    expr->type = type;
    expr->flags |= flags;
    return expr;
}


PTREE NodeLvExtract             // EXTRACT LVALUE, IF POSSIBLE
    ( PTREE expr )              // - expression
{
    TYPE expr_type;             // - expression type
    PTREE curr;                 // - current node
    PTREE last;                 // - last node
    PTREE next;                 // - next node downwards
    unsigned flags;             // - flags to be set

    expr_type = NodeType( expr );
    if( NULL == TypeReference( expr_type ) ) {
        TYPE cltype = StructType( expr_type );
        if( NULL != cltype
         && OMR_CLASS_REF == ObjModelArgument( cltype ) ) {
            expr = NodeConvert( MakeReferenceTo( expr_type ), expr );
        } else {
            for( last = NULL, curr = expr; ; ) {
                if( NodeIsBinaryOp( curr, CO_CONVERT )
                 || NodeIsBinaryOp( curr, CO_COMMA )
                 || NodeIsBinaryOp( curr, CO_DTOR ) ) {
                    next = curr->u.subtree[1];
                    curr->u.subtree[1] = last;
                    last = curr;
                    curr = next;
                } else {
                    break;
                }
            }
            if( ! ExprIsLvalue( curr )
             && NodeIsUnaryOp( curr, CO_FETCH ) ) {
                next = curr;
                curr = PTreeCopySrcLocation( curr->u.subtree[0], curr );
                PTreeFree( next );
                flags = PTF_LVALUE;
            } else {
                flags = 0;
            }
            for( ; last != NULL; ) {
                next = curr;
                curr = last;
                last = curr->u.subtree[1];
                curr->u.subtree[1] = next;
                if( 0 != flags ) {
                    curr = nodeCommaPropogate( curr );
                    curr->flags |= flags;
                }
            }
            expr = curr;
        }
    }
    return expr;
}


PTREE NodeForceLvalue           // FORCE EXPRESSION TO BE LVALUE
    ( PTREE expr )              // - expression
{
    expr = NodeLvExtract( expr );
    if( ! ExprIsLvalue( expr ) ) {
        TYPE type = TypeReferenced( expr->type );
        TEMP_TYPE old = TemporaryClass( TEMP_TYPE_EXPR );
        SYMBOL temp = TemporaryAllocNoStorage( type );
        expr = NodeAssign( MakeNodeSymbol( temp ), expr );
        TemporaryClass( old );
    }
    return expr;
}


PTREE NodeRvForRefClass         // MAKE RVALUE FOR REF CLASS
    ( PTREE expr )              // - LVALUE ref-class expression
{
    TYPE type = TypeForLvalue( expr );
    DbgVerify( OMR_CLASS_REF == ObjModelArgument( type )
             , "NodeRvForRefClass -- not reference class" );
    expr = NodeConvertFlags( type, expr, PTF_CLASS_RVREF );
    return expr;
}


PTREE NodeLvForRefClass         // MAKE LVALUE FOR REF CLASS
    ( PTREE expr )              // - RVALUE ref-class expression
{
    PTF_FLAG flags = expr->flags | PTF_LVALUE | PTF_LV_CHECKED;
    TYPE type = expr->type;
    DbgVerify( ! ExprIsLvalue( expr ), "NodeRvForRefClass -- not lvalue" );
    DbgVerify( OMR_CLASS_REF == ObjModelArgument( ClassTypeForType( type ) )
             , "NodeRvForRefClass -- not reference class" );
    expr = NodeConvertFlags( type, expr, flags );
    return expr;
}

// use as follows:
//      'expr'
//      expr = NodeUnComma( expr, &extra );
//      create/modify 'expr'
//      expr = NodeComma( extra, expr );
//
PTREE NodeUnComma(              // EXTRACT OUT UNCOMMA'D EXPR (rest is stashed)
    PTREE expr,                 // - (possibly comma'd) expr
    PTREE *pextra )             // - extra subtrees stored here
{
    PTREE last;
    PTREE curr;
    PTREE next;
    PTREE uncomma;
    PTREE bottom;

    if( ! NodeIsBinaryOp( expr, CO_COMMA ) ) {
        *pextra = NULL;
        return expr;
    }
    // Algorithm follows CO_COMMA nodes to the right, linking to the parent
    // using u.subtree[1].  The original node is marked by setting the
    // u.subtree[1] field to NULL.
    curr = expr;
    last = NULL;
    while( NodeIsBinaryOp( curr, CO_COMMA ) ) {
        next = curr->u.subtree[1];
        curr->u.subtree[1] = last;
        last = curr;
        curr = next;
    }
    DbgAssert( last != NULL && NodeIsBinaryOp( last, CO_COMMA ) );
    uncomma = curr;
    bottom = last->u.subtree[0];
    curr = last->u.subtree[1];
    last->u.subtree[0] = NULL;
    last->u.subtree[1] = NULL;
    PTreeFree( last );
    while( curr != NULL ) {
        next = curr->u.subtree[1];
        curr->u.subtree[1] = bottom;
        bottom = nodeCommaPropogate( curr );
        curr = next;
    }
    *pextra = bottom;
    return uncomma;
}


PTREE NodeDottedFunction        // BUILD A DOT NODE FOR A FUNCTION
    ( PTREE left                // - left operand
    , PTREE right )             // - right operand
{
    PTREE node;                 // - node

    node = NodeBinary( CO_DOT, NodeForceLvalue( left ), right );
    node->flags = right->flags | PTF_LVALUE | PTF_LV_CHECKED;
    node->type = right->type;
    return node;
}


PTREE NodeZero                  // BUILD A ZERO NODE
    ( void )
{
    return PTreeIntConstant( 0, TYP_SINT );
}


PTREE NodeIntDummy              // BUILD A DUMMY INTEGRAL NODE
    ( void )
{
    return PTreeIntConstant( 12345, TYP_SINT );
}


PTREE NodeAddToLeft(            // FABRICATE AN ADDITION TO LEFT
    PTREE left,                 // - left operand
    PTREE right,                // - right operand
    TYPE type )                 // - type of result
{
    PTREE expr;                 // - resultant expression

    expr = NodeBinary( CO_PLUS, left, right );
    expr->type = type;
    return( expr );
}
