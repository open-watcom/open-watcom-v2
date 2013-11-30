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
#include "objmodel.h"


CALL_OPT AnalyseCallOpts        // ANALYSE CALL OPTIMIZATIONS
    ( TYPE type                 // - class type of target operand
    , PTREE src                 // - source operand
    , PTREE* a_dtor             // - addr[ CO_DTOR ]
    , PTREE* a_right )          // - addr[ base source operand ]
{
    CALL_OPT retn;              // - type of return
    PTREE right;                // - bare source operand

    right = *NodeReturnSrc( &src, a_dtor );
    *a_right = right;
    if( type == ClassTypeForType( right->type ) ) {
        if( NodeCallsCtor( right ) ) {
            retn = CALL_OPT_CTOR;
        } else if( NodeIsBinaryOp( right, CO_CALL_EXEC )
                || NodeIsBinaryOp( right, CO_CALL_EXEC_IND ) ) {
            TYPE ftype;         // - function type
            ftype = TypeFunctionCalled( NodeFuncForCall( right )->type );
            DbgVerify( ftype != NULL
                     , "AnalyseCLassCallOpts -- impossible parse tree" );
            if( OMR_CLASS_REF == ObjModelFunctionReturn( ftype ) ) {
                retn = CALL_OPT_FUN_CALL;
            } else {
                retn = CALL_OPT_NONE;
            }
        } else if( NodeIsBinaryOp( right, CO_COPY_OBJECT ) ) {
            PTREE tgt = NodeRemoveCastsCommas( right->u.subtree[0] );
            if( tgt->op == PT_SYMBOL
             && SymIsTemporary( tgt->u.symcg.symbol ) ) {
               retn = CALL_OPT_BIN_COPY;
            } else {
                retn = CALL_OPT_NONE;
            }
        } else {
            retn = CALL_OPT_NONE;
        }
    } else {
        retn = CALL_OPT_NONE;
    }
    return retn;
}


static PTREE doCopySubstitution( // EFFECT COPY SUBSTITUTION
    PTREE* a_repl,              // - addr[ temporary to be replaced ]
    PTREE orig,                 // - addr[ original node ]
    PTREE tgt,                  // - target
    PTREE dtor )                // - NULL or CO_DTOR node on right
{
    PTREE repl;                 // - temporary to be replaced

    repl = NodeRemoveCastsCommas( *a_repl );
    DbgVerify( repl->op == PT_SYMBOL, "doCopySubstitution -- not symbol" );
    DbgVerify( SymIsTemporary( repl->u.symcg.symbol )
             , "doCopySubstitution -- not temporary" );
    if( dtor != NULL ) {
        PTREE old = orig;
        DbgVerify( dtor->u.subtree[0]->u.symcg.symbol
                        == repl->u.symcg.symbol
                 , "doCopySubstitution -- not same temporary" );
        orig = dtor->u.subtree[1];
        dtor->u.subtree[1] = NULL;
        NodeFreeDupedExpr( old );
    }
    repl->u.symcg.symbol->flag = 0;
    *a_repl = NodeReplace( *a_repl, tgt );
    orig = NodeConvertFlags( ClassTypeForType( tgt->type )
                           , orig
                           , PTF_LVALUE
                           | PTF_MEMORY_EXACT
                           | PTF_SIDE_EFF
                           | PTF_MEANINGFUL );
    return orig;
}


static PTREE doCopyElimination( // EFFECT COPY ELIMINATION
    PTREE parm,                 // - parameter node to be replaced
    PTREE orig,                 // - addr[ original node ]
    PTREE tgt,                  // - target
    PTREE dtor )                // - NULL or CO_DTOR node on right
{
    PTREE retn = doCopySubstitution( &parm->u.subtree[1], orig, tgt, dtor );
    if( retn->op != PT_ERROR ) {
        PTREE repl = PTreeOpRight( parm );
        parm->flags = ( parm->flags & PTF_ARGS ) | repl->flags;
        parm->type = repl->type;
    }
    return retn;
}


PTREE CopyOptimize              // COPY OPTIMIZATION
    ( PTREE right               // - base source operand (function call)
    , PTREE src                 // - original source operand
    , PTREE left                // - target operand
    , PTREE dtor                // - NULL or CO_DTOR expression
    , CALL_OPT opt )            // - type of optimization
{
    PTREE expr = NULL;          // - resultant expression
    PTREE parm;                 // - parameter node to be replaced

    switch( opt ) {
      case CALL_OPT_NONE :
        DbgVerify( 0, "CopyOptimize -- CALL_OPT_NONE" );
        break;
      case CALL_OPT_CTOR :
        for( parm = right->u.subtree[1]
           ; 0 == ( parm->flags & PTF_ARG_THIS )
           ; parm = parm->u.subtree[0] ) ;
        expr = doCopyElimination( parm, src, left, dtor );
        break;
      case CALL_OPT_FUN_CALL :
        for( parm = right->u.subtree[1]
           ; 0 == ( parm->flags & PTF_ARG_RETURN )
           ; parm = parm->u.subtree[0] ) ;
        expr = doCopyElimination( parm, src, left, dtor );
        break;
      case CALL_OPT_BIN_COPY :
        expr = doCopySubstitution( &right->u.subtree[0], src, left, dtor );
        break;
      case CALL_OPT_ERR :
        PTreeErrorNode( left );
        NodeFreeDupedExpr( src );
        expr = left;
        break;
      DbgDefault( "CopyOptimize -- impossible optimization" );
    }
    return expr;
}

