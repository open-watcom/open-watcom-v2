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
#include "ring.h"
#include "calldiag.h"

typedef struct {                // DIAG_INFO -- diagnostic information
    TOKEN_LOCN location;        // - error location
    TYPE bad_src;               // - source type
    TYPE bad_tgt;               // - target type
    SYMBOL bad_fn;              // - function to be listed
    int bad_parm;               // - index (base 1) of arg. in error
} DIAG_INFO;


void CallDiagnoseRejects(       // DIAGNOSE FUNCTIONS IN REJECT LIST
    PTREE expr,                 // - expression for error
    MSG_NUM msg,                // - message for error
    FNOV_DIAG *fnov_diag )      // - overload diagnosis information
{
    PTreeErrorExpr( expr, msg );
    for( ; ; ) {
        SYMBOL reject = FnovNextRejectEntry( fnov_diag );
        if( reject == NULL ) break;
        InfSymbolRejected( reject );
    }
}

static SYMBOL pickCorrectFunction(// FIND FUNCTION SYMBOL WITH CORRECT # PARMS
    SYMBOL syms,                // - function symbols
    PTREE expr )                // - call parse tree
{
    unsigned num_args;
    SYMBOL sym;
    PTREE arg;

    num_args = 0;
    for( arg = expr->u.subtree[1]; arg != NULL; arg = arg->u.subtree[0] ) {
        ++num_args;
    }
    RingIterBeg( syms, sym ) {
        if( TypeHasNumArgs( sym->sym_type, num_args ) ) {
            return( sym );
        }
    } RingIterEnd( sym )
    return( SymDefaultBase( syms ) );
}


static PTREE diagnoseArg(       // GET ARGUMENT TO BE DIAGNOSED
    PTREE expr,                 // - CTOR or call expression
    int index )                 // - expression index
{
    PTREE arg;                  // - error argument

    for( arg = expr->u.subtree[1]
       ; index > 0
       ; arg = arg->u.subtree[0], -- index );
    return arg;
}


void CallDiagAmbiguous(         // DIAGNOSE AMBIGUOUS CALL
    PTREE expr,                 // - expression for error
    MSG_NUM msg,                // - message for error
    FNOV_DIAG *fnov_diag )      // - overload diagnosis information
{
    PTreeErrorExpr( expr, msg );
    for( ; ; ) {
        SYMBOL reject = FnovNextAmbiguousEntry( fnov_diag );
        if( reject == NULL ) break;
        InfSymbolAmbiguous( reject );
    }
}


static void buildDiagInfo(      // BUILD DIAG_INFO FOR ARGUMENT
    DIAG_INFO *diag,            // - diagnostic information
    PTREE arg,                  // - expression for argument
    int bad_parm,               // - index of erroneous parameter
    SYMBOL fun )                // - function for argument
{
    arg_list* alist;            // - function args
    unsigned num_args;          // - # args

    diag->bad_parm = bad_parm + 1;
    alist = SymFuncArgList( fun );
    num_args = alist->num_args;
    if( bad_parm >= num_args ) {
        TYPE last_arg = alist->type_list[ num_args - 1 ];
        if( last_arg->id == TYP_DOT_DOT_DOT ) {
            diag->bad_tgt = last_arg;
        } else {
            diag->bad_tgt = NULL;
        }
    } else {
        diag->bad_tgt = alist->type_list[ bad_parm ];
    }
    PTreeExtractLocn( arg, &diag->location );
    if( PointerToFuncEquivalent( arg->type ) ) {
        PTREE operand;  // - argument operand
        operand = arg;
        if( NodeIsUnaryOp( operand, CO_ADDR_OF ) ) {
            operand = PTreeOpLeft( operand );
        }
        if( operand->op == PT_SYMBOL ) {
            diag->bad_fn = operand->u.symcg.symbol;
            if( IsActualOverloadedFunc( diag->bad_fn
                                      , operand->u.symcg.result ) ) {
                diag->bad_src = NULL;
            } else {
                diag->bad_fn = NULL;
                diag->bad_src = NodeType( arg );
            }
        } else {
            diag->bad_fn = NULL;
            diag->bad_src = NodeType( arg );
        }
    } else {
        diag->bad_fn = NULL;
        diag->bad_src = NodeType( arg );
    }
}


static void displayParmMismatch(// DISPLAY PARAMETER MISMATCH
    DIAG_INFO* diag )           // - diagnostic information
{
    CErr( INF_FUNC_PARM_MISMATCH, diag->bad_parm, &diag->location );
}


static void displayDiagInfo(    // DISPLAY DIAG_INFO FOR ARGUMENT
    DIAG_INFO* diag,            // - diagnostic information
    MSG_NUM msg,                // - error message
    PTREE expr,                 // - expression
    SYMBOL orig )               // - original function
{
    ConversionTypesSet( diag->bad_src, diag->bad_tgt );
    PTreeErrorExpr( expr, msg  );
    InfSymbolDeclaration( orig );
    if( diag->bad_parm == 0 ) {
        CErr1( INF_THIS_MISMATCH );
        ConversionDiagnoseInf();
    } else if( diag->bad_fn == NULL ) {
        displayParmMismatch( diag );
        ConversionDiagnoseInf();
    } else {
        displayParmMismatch( diag );
        CErr2p( INF_BAD_FN_OVERLOAD, diag->bad_fn );
        ConversionDiagnoseInfTgt();
    }
}


void CallDiagNoMatch(           // DIAGNOSE NO MATCHES FOR CALL
    PTREE expr,                 // - call expression
    MSG_NUM msg_one,            // - message: one function
    MSG_NUM msg_many,           // - message: many functions
    PTREE this_node,            // - this node (or NULL)
    SYMBOL orig,                // - original symbol for overloading
    FNOV_DIAG *fnov_diag )      // - overload diagnosis information
{
    DIAG_INFO diag;             // - diagnostic information
    PTREE arg;                  // - argument that didn't match
    int bad_parm;               // - index of bad argument

    switch( fnov_diag->num_candidates ) {
      case 0 :
        if( SymIsFunctionTemplateModel( orig ) ) {
            PTreeErrorExprSym( expr, ERR_TEMPLATE_FN_MISMATCH, orig );
        } else {
            PTreeErrorExprSym( expr, ERR_PARM_COUNT_MISMATCH, orig );
        }
        break;
      case 1 :
        if( ! SymIsFunctionTemplateModel( orig ) ) {
            bad_parm = FnovRejectParm( fnov_diag );
            if( bad_parm == -1 ) {
                diag.bad_parm = 0;
                diag.bad_src = NodeType( this_node );
                diag.bad_tgt = TypeThisForCall( this_node, orig );
            } else {
                arg = diagnoseArg( expr, bad_parm );
                orig = pickCorrectFunction( orig, expr );
                buildDiagInfo( &diag, arg, bad_parm, orig );
            }
            displayDiagInfo( &diag, msg_one, expr, orig );
            break;
        }
        // fall through
      default :
        CallDiagnoseRejects( expr, msg_many, fnov_diag );
        break;
    }
}


void CtorDiagNoMatch(           // DIAGNOSE NO MATCHES FOR CTOR
    PTREE expr,                 // - ctor expression
    MSG_NUM msg_none,           // - message: no CTOR's
    FNOV_DIAG *fnov_diag )      // - overload diagnosis information
{
    int bad_parm;
    SYMBOL orig;
    PTREE arg;
    DIAG_INFO diag;

    switch( fnov_diag->num_candidates ) {
      case 0 :
        PTreeErrorExpr( expr, msg_none );
        break;
      case 1 :
        bad_parm = FnovRejectParm( fnov_diag );    // must be before FnovNextRejectEntry
        orig = FnovNextRejectEntry( fnov_diag );
        if( orig == NULL ) {
            /* no fns matched at all but there happened to be one */
            PTreeErrorExpr( expr, msg_none );
        } else {
            arg = diagnoseArg( expr, bad_parm );
            buildDiagInfo( &diag, arg, bad_parm, orig );
            displayDiagInfo( &diag, msg_none, expr, orig );
        }
        break;
      default :
        CallDiagnoseRejects( expr, msg_none, fnov_diag );
        break;
    }
}


void UdcDiagNoMatch(            // DIAGNOSE NO MATCHES FOR UDC LOOKUP
    PTREE src,                  // - ctor expression
    TYPE tgt_type,              // - target type
    MSG_NUM msg_none,           // - message: no UDC's
    MSG_NUM msg_many,           // - message: many functions
    FNOV_DIAG *fnov_diag )      // - overload diagnosis information
{
    SYMBOL orig;
    DIAG_INFO diag;

    diag.bad_src = NodeType( src );
    diag.bad_tgt = tgt_type;
    switch( fnov_diag->num_candidates ) {
      case 0 :
        PTreeErrorExpr( src, msg_none );
        break;
      case 1 :
        orig = FnovNextRejectEntry( fnov_diag );
        if( orig == NULL ) {
            /* no fns matched at all but there happened to be one */
            PTreeErrorExpr( src, msg_none );
        } else {
            diag.bad_fn = NULL;
            diag.bad_parm = 1;
            PTreeExtractLocn( src, &diag.location );
            displayDiagInfo( &diag, msg_none, src, orig );
        }
        break;
      default :
        CallDiagnoseRejects( src, msg_many, fnov_diag );
        ConversionTypesSet( diag.bad_src, diag.bad_tgt );
        ConversionDiagnoseInf();
        break;
    }
}


CALL_DIAG* CallDiagFromCnvDiag  // MAKE CALL_DIAG FROM CNV_DIAG
    ( CALL_DIAG* call_diag      // - call diagnosis
    , CNV_DIAG* cnv_diag )      // - conversion diagnosis
{
    call_diag->msg_ambiguous = cnv_diag->msg_ambiguous;
    call_diag->msg_no_match_one = cnv_diag->msg_impossible;
    call_diag->msg_no_match_many = cnv_diag->msg_impossible;
    return call_diag;
}
