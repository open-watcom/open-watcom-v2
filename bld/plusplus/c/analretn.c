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
#include "cast.h"
#include "defarg.h"
#include "class.h"

static CNV_DIAG diagDefarg =        // diagnosis for def. argument conversion
    {   ERR_DEFARG_IMPOSSIBLE
    ,   ERR_DEFARG_AMBIGUOUS
    ,   ERR_CALL_WATCOM
    ,   ERR_DEFARG_PRIVATE
    ,   ERR_DEFARG_PROTECTED
    };

static CNV_DIAG diagReturn =        // diagnosis for return conversion
    {   ERR_RETURN_IMPOSSIBLE
    ,   ERR_RETURN_AMBIGUOUS
    ,   ERR_CALL_WATCOM
    ,   ERR_RETURN_PRIVATE
    ,   ERR_RETURN_PROTECTED
    };


static PTREE getReturnSym       // MAKE NODE FOR RETURN SYMBOL
    ( void )
{
    SYMBOL sym;                 // - return symbol
    PTREE node;                 // - node for above

    sym = SymFunctionReturn();
    sym->flag |= SF_REFERENCED;
    node = MakeNodeSymbol( sym );
    return node;
}


static PTREE removeReturnNode   // REMOVE RETURN NODE
    ( PTREE expr )              // - original expression
{
    PTREE old;                  // - old expression

    old = expr;
    expr = old->u.subtree[1];
    old->u.subtree[1] = NULL;
    PTreeFreeSubtrees( old );
    return expr;
}


static void checkAutoReturn(    // CHECK IF AUTOMATIC BEING RETURNED
    PTREE node,                 // - node to be checked
    TYPE ret_type )             // - return type
{
    TYPE func_ret;              // - type of function return
    SYMBOL func;                // - function called
    SYMBOL comped;              // - function being compiled
    PTREE expr;                 // - node for error
    TYPE refed;                 // - NULL ==> not reference

    comped = ScopeFunctionInProgress();
    if( SymIsGenedFunc( comped ) ) {
        return;
    }
    expr = node;
    refed = TypeReference( ret_type );
    for( ; ; ) {
        node = NodeRemoveCastsCommas( node );
        if( ( node->op == PT_SYMBOL )
          &&( SymIsAutomatic( node->u.symcg.symbol ) ) ) {
            if( NULL == refed ) {
                PTreeWarnExpr( expr, WARN_RET_ADDR_OF_AUTO );
            } else {
                PTreeErrorExpr( expr, ERR_RET_AUTO_REF );
            }
            break;
        } else if( NodeIsBinaryOp( node, CO_DOT )
                || NodeIsBinaryOp( node, CO_DOT_STAR ) ) {
            node = PTreeOpLeft( node );
        } else if( NodeIsUnaryOp( node, CO_ADDR_OF ) ) {
            node = PTreeOpLeft( node );
        } else if( NULL != refed ) {
            if( NodeIsBinaryOp( node, CO_DTOR ) ) {
                node = PTreeOpRight( node );
            } else if( NodeIsBinaryOp( node, CO_CALL_EXEC ) ) {
                node = PTreeOpLeft( PTreeOpLeft( node ) );
                if( node->op == PT_SYMBOL ) {
                    func = node->u.symcg.symbol;
                    if( SymIsCtor( func ) ) {
                        PTreeErrorExpr( expr, ERR_RET_AUTO_REF );
                    } else {
                        func_ret = SymFuncReturnType( func );
                        if( NULL != StructType( func_ret ) ) {
                            PTreeErrorExpr( expr, ERR_RET_AUTO_REF );
                        }
                    }
                }
                break;
            } else if( NodeIsBinaryOp( node, CO_CALL_EXEC_IND ) ) {
                func_ret = TypeFunctionCalled( NodeFuncForCall( node )->type );
                func_ret = func_ret->of;
                if( NULL != StructType( func_ret ) ) {
                    PTreeErrorExpr( expr, ERR_RET_AUTO_REF );
                }
                break;
            } else {
                break;
            }
        } else {
            break;
        }
    }
}


PTREE AnalyseReturnClassVal     // RETURN CLASS VALUE
    ( PTREE expr )              // - expression for return
{
    TYPE retn_type;             // - return type
    TYPE retn_class;            // - class for return
    PTREE tgt;                  // - target expression
    CNV_DIAG* diag;             // - diagnosis

    retn_type = expr->u.subtree[0]->type;
    retn_class = StructType( retn_type );
    DbgVerify( retn_class != NULL, "AnalyseReturnClassVal -- not class" );
    if( ClassCorrupted( retn_class ) ) {
        PTreeErrorNode( expr );
    } else if( TypeAbstract( retn_class ) ) {
        PTreeErrorExprType( expr, ERR_CONVERT_TO_ABSTRACT_TYPE, retn_class );
        ScopeNotePureFunctions( retn_class );
    } else {
        diag = DefargBeingCompiled() ? &diagDefarg : &diagReturn;
        tgt = NodeFetchReference( getReturnSym() );
        expr = removeReturnNode( expr );
        expr = CopyClassRetnVal( expr, tgt, retn_type, diag );
        if( expr->op != PT_ERROR ) {
            if( NodeIsBinaryOp( expr, CO_DTOR ) ) {
                PTREE node = expr->u.subtree[0];
                if( SymFunctionReturn() == node->u.symcg.symbol ) {
                    PTreeFree( node );
                    node = expr;
                    expr = expr->u.subtree[1];
                    PTreeFree( node );
                }
            }
        }
#if 0
            // this is just so we can do some checking
            expr = CastImplicit( expr
                               , retn_type
                               , CNV_EXPR
                               , DefargBeingCompiled()
                                    ? &diagDefarg : &diagReturn );
        }
#endif
    }
    return expr;
}


PTREE AnalyseReturnSimpleVal    // RETURN A SIMPLE VALUE
    ( PTREE expr )              // - expression for return
{
    TYPE retn_type;             // - return type
    unsigned cnv_type;          // - type of conversion
    CNV_DIAG *diag;             // - diagnosis to be used

    retn_type = expr->u.subtree[0]->type;
    if( DefargBeingCompiled() ) {
        cnv_type = CNV_FUNC_DARG;
        diag = &diagDefarg;
    } else {
        cnv_type = CNV_FUNC_RET;
        diag = &diagReturn;
    }
    expr = CastImplicit( removeReturnNode( expr )
                       , retn_type
                       , cnv_type
                       , diag );
    if( expr->op != PT_ERROR ) {
        if( NULL != PointerTypeEquivalent( retn_type ) ) {
            checkAutoReturn( expr, retn_type );
        }
        if( expr->op != PT_ERROR ) {
            PTREE tgt = getReturnSym();
            if( NULL == TypeReference( retn_type ) ) {
                expr = NodeRvalue( expr );
                expr = NodeAssign( tgt, expr );
                if( NULL != MemberPtrType( retn_type ) ) {
                    PTREE new_expr = expr;
                    MembPtrAssign( &new_expr );
                    expr = new_expr;
                }
            } else {
                expr = NodeAssignRef( tgt, expr );
            }
        }
    }
    return expr;
}
