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
#include "fnbody.h"
#include "defarg.h"
#include "errdefns.h"
#include "rtngen.h"
#include "objmodel.h"
#include "cgio.h"


static SYMBOL existingThunk(    // GET EXISTING THUNK SYMBOL FOR NAME
    SCOPE scope,                // - scope to look in
    char *name )                // - thunk name
{
    SYMBOL sym;                 // - symbol for name
    SEARCH_RESULT *result;      // - search result

    if( ScopeId( scope ) == SCOPE_FILE ) {
        result = ScopeFindNaked( scope, name );
    } else {
        result = ScopeContainsMember( scope, name );
    }
    if( result != NULL ) {
        sym = result->sym_name->name_syms;
        ScopeFreeResult( result );
    } else {
        sym = NULL;
    }
    return( sym );
}


static unsigned classifyThunk(  // GET CLASSIFICATION OF THUNK
    SYMBOL sym )                // - original symbol
{
    unsigned classification;    // - classification of thunk

    DbgAssert( ! SymIsThunk( sym ) );
    if( SymIsDtor( sym ) ) {
        classification = SPECIAL_DTOR_THUNK;
    } else if( SymIsOpDel( sym ) ) {
        classification = SPECIAL_OP_DEL_THUNK;
    } else if( SymIsOpDelar( sym ) ) {
        classification = SPECIAL_OP_DELAR_THUNK;
    } else if( TypeHasNumArgs( sym->sym_type, 1 ) ) {
        DbgAssert( SymIsCtor( sym ) );
        classification = SPECIAL_COPY_THUNK;
    } else {
        DbgAssert( SymIsCtor( sym ) );
        classification = SPECIAL_CTOR_THUNK;
    }
    return classification;
}


static SYMBOL addrThunkSymbol(  // GET THUNK SYMBOL FROM ORIGINAL
    unsigned classification,    // - classification of thunk
    SYMBOL sym )                // - original symbol
{
    TYPE thunk_type;            // - type of new symbol
    SYMBOL new_sym;             // - the new symbol
    SCOPE scope;                // - scope for new symbol
    char *name;                 // - name of new symbol
    symbol_class thunk_class;   // - SC_.. for thunk

    switch( classification ) {
      case SPECIAL_OP_DEL_THUNK :
      case SPECIAL_OP_DELAR_THUNK :
        thunk_class = SC_STATIC;
        break;
      case SPECIAL_DTOR_THUNK :
      case SPECIAL_COPY_THUNK :
      case SPECIAL_CTOR_THUNK :
        thunk_class = SC_MEMBER;
        break;
      DbgDefault( "addrThunkSymbol -- bad classification" );
    }
    name = CppSpecialName( classification );
    scope = SymScope( sym );
    new_sym = existingThunk( scope, name );
    if( new_sym == NULL ) {
        thunk_type = MakeThunkFunction( sym->sym_type );
        new_sym = AllocTypedSymbol( thunk_type );
        new_sym->id = thunk_class;
        new_sym->flag |= SF_ADDR_THUNK;
        new_sym = ScopeInsert( scope, new_sym, name );
        SymDeriveThrowBits( new_sym, sym );
    }
    return new_sym;
}


static void insertArgument(     // INSERT AN ARGUMENT
    TYPE type )                 // - type of argument
{
    char *name;                 // - name of argument
    SYMBOL sym;                 // - new argument

    sym = SymMakeDummy( type, &name );
    ScopeInsert( CurrScope, sym, name );
}


static void declareThunkArgs(   // DECLARE ARG.S FOR THUNK
    SYMBOL fn_sym,              // - original symbol
    TYPE fn_type )              // - type of thunk
{
    unsigned num_args;          // - # arguments
    arg_list *args;             // - arguments
    TYPE *curr;                 // - addr[ current argument type ]

    if( SymCDtorExtraParm( fn_sym ) ) {
        insertArgument( MakeCDtorExtraArgType() );
    }
    args = fn_type->u.f.args;
    for( num_args = args->num_args, curr = args->type_list
       ; num_args != 0
       ; ++curr, --num_args ) {
        insertArgument( *curr );
    }
}


static SCOPE thunkPrologue(     // PROLOGUE FOR A SCOPE
    SYMBOL thunk_sym,           // - thunk symbol
    FUNCTION_DATA *data )       // - function data
{
    SYMBOL orig;                // - original symbol
    SCOPE arg_scope;            // - scope for arguments

    orig = thunk_sym->u.thunk_calls;
    thunk_sym->flag |= SF_INITIALIZED;
    CurrScope = SymScope( thunk_sym );
    ScopeBeginFunction( thunk_sym );
    arg_scope = CurrScope;
    declareThunkArgs( orig, FunctionDeclarationType( orig->sym_type ) );
    FunctionBodyStartup( thunk_sym, data, FUNC_NULL );
    return arg_scope;
}


static void thunkEpilogue(      // THUNK EPILOGUE
    SYMBOL thunk_sym,           // - thunk symbol
    FUNCTION_DATA *data )       // - generation data
{
    FunctionBodyShutdown( thunk_sym, data );
    ScopeEnd( SCOPE_FUNCTION );
}


static PTREE thunkArgList(      // BUILD THUNK ARGUMENT LIST
    SCOPE scope )               // - scope for arguments
{
    char *ret_name;             // - name of return value symbol
    PTREE expr;                 // - argument expression
    PTREE list;                 // - arg. list under construction
    SYMBOL stopper;             // - stopping value
    SYMBOL sym;                 // - current symbol
    unsigned count;             // - # in list
    OMR arg_model;              // - argument calling convention model
    TYPE arg_type;              // - unmodified argument type from symbol

    ret_name = CppSpecialName( SPECIAL_RETURN_VALUE );
    stopper = ScopeOrderedStart( scope );
    sym = NULL;
    list = NULL;
    for(;;) {
        sym = ScopeOrderedNext( stopper, sym );
        if( sym == NULL ) break;
        if( sym->name->name != ret_name ) {
            arg_type = TypedefModifierRemoveOnly( sym->sym_type );
            expr = MakeNodeSymbol( sym );
            expr = NodeFetchReference( expr );
            arg_model = ObjModelArgument( TypeReferenced( arg_type ) );
            if( TypeReference( arg_type ) == NULL && arg_model != OMR_CLASS_REF ) {
                expr = NodeRvalue( expr );
            }
            list = NodeArgument( list, expr );
        }
    }
    list = NodeReverseArgs( &count, list );
    return( list );
}


SYMBOL ClassFunMakeAddressable( // MAKE SURE THE FUNCTION CAN BE ADDRESSED
    SYMBOL orig_sym )           // - original symbol
{
    SYMBOL thunk_sym;           // - thunk symbol
    unsigned classification;    // - classification of thunk

    if( orig_sym == NULL ) {
        return orig_sym;
    }
    if( ( orig_sym->id != SC_DEFAULT )
      &&( ! TypeHasPragma( orig_sym->sym_type ) ) ) {
        type_flag flags;
        TypeModFlags( orig_sym->sym_type, &flags );
        if( ! ( flags & TF1_DLLIMPORT ) ) {
            return orig_sym;
        }
    }
    classification = classifyThunk( orig_sym );
    thunk_sym = addrThunkSymbol( classification, orig_sym );
    if( NULL == thunk_sym->u.thunk_calls ) {
        if( ! SymIsVirtual( orig_sym ) ) {
            thunk_sym = SymDeriveThrowBits( thunk_sym, orig_sym );
        }
        thunk_sym->u.thunk_calls = orig_sym;
        RtnGenAddSymbol( RGSYMBOL_GenThunk, thunk_sym );
    }
    return thunk_sym;
}


void RtnGenCallBackGenThunk(    // GENERATE THUNK CODE
    SYMBOL thunk_sym )          // - the thunk symbol
{
    SYMBOL orig_sym;            // - original symbol
    PTREE args;                 // - arguments for call
    PTREE stmt;                 // - expression under construction
    TYPE return_type;           // - return type of original, thunk
    SYMBOL return_sym;          // - return symbol for thunk routine
    SYMBOL curr_func;           // - function being compiled before thunk
    SCOPE fn_scope;             // - scope for thunk-function arguments
    SCOPE save_scope;           // - CurrScope before generation of thunk
    FUNCTION_DATA func_data;    // - controls thunk generation
    PTREE extra_arg;            // - Ctor/Dtor extra argument
    PTREE user_args;            // - user arguments for function
    PTREE this_arg;             // - "this" argument
    unsigned classification;    // - classification of thunk
    CGFILE* cgfile;             // - CGFILE for thunk
    symbol_flag orig_ref;       // - keep original SF_REFERENCED setting

    orig_sym = thunk_sym->u.thunk_calls;
    if( thunk_sym->flag & SF_REFERENCED ) {
        orig_sym->flag |= SF_REFERENCED;
    }
    if( SymIsInitialized( thunk_sym ) ) {
        return;
    }
    classification = classifyThunk( orig_sym );
    save_scope = CurrScope;
    curr_func = CgFrontCurrentFunction();
    fn_scope = thunkPrologue( thunk_sym, &func_data );
    return_type = SymFuncReturnType( thunk_sym );
    args = thunkArgList( fn_scope );
    orig_ref = orig_sym->flag & SF_REFERENCED;
    stmt = NodeMakeCall( orig_sym, return_type, args );
    if( ! orig_ref ) {
        orig_sym->flag &= ~SF_REFERENCED;
    }
    if( ! AddDefaultArgs( orig_sym, stmt ) ) {
        NodeFreeDupedExpr( stmt );
    } else {
        user_args = stmt->u.subtree[1];
        if( SymCDtorExtraParm( orig_sym ) ) {
            extra_arg = user_args;
            user_args = user_args->u.subtree[0];
        } else {
            extra_arg = NULL;
        }
        if( SymIsThisMember( orig_sym ) ) {
            this_arg = NodeArg( NodeThis() );
        } else {
            this_arg = NULL;
        }
        stmt = CallArgsArrange( orig_sym->sym_type
                              , stmt
                              , user_args
                              , this_arg
                              , extra_arg
                              , NULL );
        stmt = FunctionCalled( stmt, orig_sym );
        return_sym = SymFunctionReturn();
        if( NULL != return_sym ) {
            stmt = PTreeBinary( CO_RETURN, PTreeType( return_type ), stmt );
            stmt = AnalyseNode( stmt );
        }
        EmitAnalysedStmt( stmt );
    }
    CgFrontCodePtr( IC_PROC_RETURN, return_sym );
    thunkEpilogue( thunk_sym, &func_data );
    CurrScope = save_scope;
    CgFrontResumeFunction( curr_func );
    cgfile = CgioLocateFile( thunk_sym );
    cgfile->thunk = TRUE;
    if( ( SymIsInitialized( orig_sym ) )
      ||( SymIsDefArg( orig_sym ) )
      ||( classification == SPECIAL_OP_DEL_THUNK )
      ||( classification == SPECIAL_OP_DELAR_THUNK ) ) {
        CgioThunkMarkGen( cgfile );
    }
}

static PTREE applyReturnThunk(  // GENERATE A RETURN THUNK
    THUNK_ACTION *thunk,        // - thunk description
    PTREE expr )                // - current expression
{
    target_offset_t delta;      // - output delta
    target_offset_t vb_offset;  // - output offset
    target_offset_t vb_index;   // - output index
    TYPE ret_type;              // - type of call expression
    TYPE ptr_type;              // - pointer equivalent of "ret_type"
    PTREE dup1;
    PTREE dup2;

    delta = thunk->out.delta;
    if( !thunk->output_virtual && delta == 0 ) {
        /* no return thunk */
        return( expr );
    }
    ret_type = NodeType( expr );
    dup1 = NULL;
    ptr_type = PointerTypeEquivalent( ret_type );
    if( ptr_type != NULL ) {
        /* only need NULL checks for pointer casts */
        if( ptr_type->id == TYP_POINTER ) {
            if(( ptr_type->flag & TF1_REFERENCE ) == 0 ) {
                dup1 = NodeDupExpr( &expr );
            }
        }
    }
    DbgAssert( StructType( ret_type ) == NULL );
    if( thunk->output_virtual ) {
        vb_offset = thunk->out.vb_offset;
        vb_index = thunk->out.vb_index;
        expr = NodeConvertVirtualPtr( expr, ret_type, vb_offset, vb_index );
    }
    if( delta != 0 ) {
        expr = NodeBinary( CO_PLUS, expr, NodeOffset( delta ) );
        expr->type = ret_type;
    }
    if( dup1 != NULL ) {
        dup2 = NodeDupExpr( &dup1 );
        dup1 = NodeCompareToZero( dup1 );
        expr = NodeTestExpr( dup1, expr, dup2 );
    }
    return( expr );
}


void EmitVfunThunk(             // EMIT THUNK FOR VIRTUAL FUNCTION
    SCOPE host_scope,           // - scope for thunk insertion
    THUNK_ACTION *thunk )       // - thunk description
{
    SCOPE scope;
    SYMBOL sym;
    SYMBOL thunk_sym;
    SYMBOL override_sym;
    SYMBOL return_sym;
    char *name;
    PTREE stmt;
    PTREE args;
    PTREE this_arg;
    PTREE return_node;
    TYPE thunk_type;
    TYPE fn_type;
    TYPE override_class;
    TYPE return_type;
    SCOPE save_scope;
    SCOPE fn_scope;
    auto FUNCTION_DATA func_data;

    override_sym = thunk->override;
    if( override_sym == NULL ) {
        /* pure virtual function with no override */
        thunk->thunk = thunk->sym;      // let CGINFO do it
        return;
    }
    sym = thunk->sym;
    if( SymIsEllipsisFunc( sym ) ) {
        CErr( ERR_NO_VIRTUAL_ELLIPSE_FUNCTION_THUNKS, sym );
        InfMsgPtr( INF_THUNK_TARGET, override_sym );
    }
    save_scope = CurrScope;
    thunk_type = MakeThunkPragmaFunction( sym->sym_type );
    thunk_sym = AllocTypedSymbol( thunk_type );
    name = CppThunkName( host_scope, thunk );
    scope = SymScope( thunk->sym );
    CurrScope = scope;
    thunk_sym = ScopeInsert( scope, thunk_sym, name );
    thunk->thunk = thunk_sym;
    ScopeBeginFunction( thunk_sym );
    fn_scope = CurrScope;
    fn_type = FunctionDeclarationType( thunk_type );
    declareThunkArgs( sym, fn_type );
    FunctionBodyStartup( thunk_sym, &func_data, FUNC_NO_STACK_CHECK );
    if( thunk->delta != 0 ) {
        CgFrontCodeUint( IC_VTHUNK_MDELTA, thunk->delta );
    }
    if( thunk->ctor_disp ) {
        CgFrontCode( IC_VTHUNK_CDISP );
        if( thunk->input_virtual ) {
            CgFrontCodeUint( IC_VTHUNK_PDELTA, thunk->in.vb_offset );
            CgFrontCodeUint( IC_VTHUNK_VBASE, thunk->in.vb_index );
        }
        if( thunk->in.delta ) {
            CgFrontCodeUint( IC_VTHUNK_PDELTA, thunk->in.delta );
        }
    }
    override_class = ScopeClass( SymScope( override_sym ) );
    args = thunkArgList( fn_scope );
    /* make "this" arg */
    this_arg = NodeRvalue( NodeThis() );
    this_arg = NodeConvert( MakePointerTo( override_class ), this_arg );
    this_arg = NodeArg( this_arg );
    return_type = SymFuncReturnType( override_sym );
    return_node = NULL;
    if( OMR_CLASS_REF == ObjModelArgument( return_type ) ) {
        return_node = NodeTemporary( return_type );
    }
    stmt = NodeMakeCall( override_sym, return_type, args );
    stmt = CallArgsArrange( override_sym->sym_type, stmt, args, this_arg, NULL, return_node );
    if( return_node != NULL ) {
        stmt = NodeDtorExpr( stmt, return_node->u.symcg.symbol );
        stmt = PtdCtoredExprType( stmt, override_sym, return_type );
    }
    return_sym = SymFunctionReturn();
    if( return_sym == NULL ) {
        EmitAnalysedStmt( stmt );
    } else {
        stmt = applyReturnThunk( thunk, stmt );
        stmt = PTreeBinary( CO_RETURN, PTreeType( return_type ), stmt );
        stmt = AnalyseNode( stmt );
        EmitAnalysedStmt( stmt );
        CgFrontCodePtr( IC_PROC_RETURN, return_sym );
    }
    FunctionBodyShutdown( thunk_sym, &func_data );
    ScopeEnd( SCOPE_FUNCTION );
    CurrScope = save_scope;
}


// Later: generate proper thunk which will re-map arguments for
//        the specialized calling conventions
//
void RtnGenCallBackRunTimeTableThunk(  // GEN. THUNK TO RE-MAP CTOR/DTOR arg.s
    SYMBOL sym )        // - the symbol
{
    CErr2p( ERR_BAD_CALL_CONVENTION, sym );
}


void CDtorScheduleArgRemap(     // SCHEDULE CTOR/DTOR ARG.S REMAP, IF REQ'D
    SYMBOL cdtor )              // - CTOR OR DTOR
{
    TYPE ftype;                 // - declaration type of function

    ftype = FunctionDeclarationType( cdtor->sym_type );
    if( TypeHasEllipsisArg( ftype ) || TypeHasReverseArgs( ftype ) ) {
        RtnGenAddSymbol( RGSYMBOL_RunTimeTableThunk, cdtor );
    }
}
