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
#include "cgback.h"
#include "fnbody.h"
#include "fnovload.h"
#include "fold.h"
#include "vfun.h"
#include "initdefs.h"

#ifndef NDEBUG
    #include "dbg.h"
    #include "pragdefn.h"
#endif

typedef enum                    // CLASSIFICATION OF MEMBER-PTR EXPRESSIONS
{   MP_ZERO                     // - zero
,   MP_CONST                    // - member pointer constant
,   MP_EXPR                     // - member-pointer expression
,   MP_INVALID                  // - invalid operand
,   MP_DIMENSION                // # of different types for comparison
,   MP_ADDR_OF = MP_DIMENSION   // - &class::item detected
,   MP_MEMB_FN                  // - member function, by self
} MP_TYPE;

#define MP_OPS( op1, op2 ) op1*MP_DIMENSION+op2


static PTREE membPtrTemporary(  // PLACE MEMBER PTR IN TEMPORARY
    PTREE expr )                // - member-ptr expression
{
    expr = NodeAssign( NodeTemporary( expr->type ), expr );
    expr->flags |= PTF_MEMORY_EXACT;
    return expr;
}


static PTREE membPtrStoreTemp(  // STORE MEMBER PTR EXPR IN TEMPORARY
    PTREE expr )
{
    PTREE a_expr;               // - for better addressing

    a_expr = membPtrTemporary( expr );
    MembPtrAssign( &a_expr );
    return NodeRvalue( a_expr );
}


static bool nodeIsMembPtrCon(   // TEST IF NODE CAN BE MEMBER-PTR CONST
    PTREE node )                // - node to be tested
{
    if( node->op == PT_SYMBOL && (node->flags & PTF_COLON_QUALED) ) {
        return true;
    } else {
        return CompFlags.extensions_enabled;    // KLUGE for MFC
    }
}


static TYPE membPtrObject(      // GET OBJECT TYPE FOR MEMBER PTR
    TYPE mp )                   // - a member-ptr type
{
    return MemberPtrType( mp )->of;
}


static CNV_RETN validateOkObjs( // VALIDATE TARGET ONLY ADDS QUALIFICATION
    PTREE expr,                 // - expression for errors
    bool check_qual,            // - true ==> check CV qualification
    TYPE o1,                    // - source object type
    TYPE o2 )                   // - target object type
{
    CNV_RETN retn;              // - return: true ==> objects are same
    type_flag f1;               // - flags: object[1]
    type_flag f2;               // - flags: object[2]
    type_flag f3;               // - flags: object[1]
    type_flag f4;               // - flags: object[2]

    retn = CNV_OK;
    o1 = TypeModFlagsEC( TypeReferenced( o1 ), &f1 );
    f1 &= ~TF1_MPTR_REMOVE;
    o2 = TypeModFlagsEC( TypeReferenced( o2 ), &f2 );
    f2 &= ~TF1_MPTR_REMOVE;
    if( TypesIdentical( o1, o2 ) ) {
        f4 = f2;
        f3 = f1 | ( f4 & TF1_CV_MASK );
        if( f3 == f4 ) {
            retn = CNV_OK;
        } else {
            if( check_qual ) {
                retn = CNV_ERR;
            } else {
                retn = CNV_IMPOSSIBLE;
            }
        }
    } else {
        retn = CNV_ERR;
    }
    if( retn == CNV_ERR ) {
        PTreeErrorExpr( expr, ERR_MEMB_PTR_OBJS_MISMATCH );
        retn = CNV_ERR;
    }
    return retn;
}


static CNV_RETN validateMpObjs( // VALIDATE BOTH MEMPTR'S ARE TO SAME TYPE
    PTREE expr,                 // - expression for error
    bool check_qual,            // - true ==> check CV qualification
    TYPE mp1,                   // - source type
    TYPE mp2 )                  // - target type
{
    return validateOkObjs( expr
                         , check_qual
                         , membPtrObject( mp1 )
                         , membPtrObject( mp2 ) );
}


static CNV_RETN validateAddrOfObj( // VALIDATE &class::item OBJECT
    PTREE expr,                 // - expression for constant
    TYPE mp )                   // - target member-ptr type
{
    TYPE mp_obj;                // - object type for member ptr
    TYPE ad_obj;                // - object type for & object
    CNV_RETN retn;              // - return: conversion

    mp_obj = membPtrObject( mp );
    ad_obj = PTreeOpLeft( expr )->type;
    if( FunctionDeclarationType( mp_obj ) &&
        FunctionDeclarationType( ad_obj ) ) {
        /* wait until overload to check types */
        retn = CNV_OK;
    } else {
        retn = validateOkObjs( expr, true, ad_obj, mp_obj );
    }
    return retn;
}


static TYPE memberPtrLayout(    // SET TYPE, OFFSETS FOR MEMBER-PTR OBJECT
    unsigned *offset_delta,     // - addr[delta offset]
    unsigned *offset_index )    // - addr[delta index]
{
    TYPE type_offset;           // - type of an offset
    unsigned delta;             // - offset of delta

    delta = CgCodePtrSize();
    *offset_delta = delta;
    type_offset = TypeTargetSizeT();
    *offset_index = delta + CgMemorySize( type_offset );
    return type_offset;
}


static TYPE memberPtrHostClass( // GET HOST CLASS FOR MEMBER PTR.
    TYPE mp_type )              // - member ptr. type
{
    return MemberPtrClass( MemberPtrType( mp_type ) );
}


static SCOPE scopeMembPtrType(  // GET SCOPE FOR A MEMBER POINTER TYPE
    TYPE type )                 // - a memb-ptr type
{
    SCOPE scope;                // - scope of class for member pointer

    type = memberPtrHostClass( type );
    if( type == NULL ) {
        scope = NULL;
    } else {
        scope = type->u.c.scope;
    }
    return scope;
}


static PTREE accessOp(          // ACCESS AN OPERAND
    PTREE base,                 // - lvalue operand
    unsigned offset,            // - offset of item
    TYPE type )                 // - type of item
{
    PTREE expr;                 // - resultant expression

    expr = NodeBinary( CO_DOT, base, NodeOffset( offset ) );
    expr->type = type;
    expr->flags |= PTF_LVALUE;
    return expr;
}


static PTREE addOffset(         // ADD AN OFFSET TO AN LVALUE
    PTREE field,                // - field
    unsigned offset,            // - offset
    TYPE type )                 // - type of field
{
    PTREE expr;                 // - resultant expression

    expr = NodeAddToLeft( field, NodeOffset( offset ), type );
    expr->flags &= ~ PTF_LVALUE;
    return expr;
}


static void generateOffsetFunc( // GENERATE CODE FOR OFFSET FUNCTION
    SYMBOL func,                // - function to be generated
    PTREE node )                // - symbol node used as a member
{
    SCOPE stashed_scope;        // - saved: CurrScope
    FUNCTION_DATA func_fd;      // - function data
    SCOPE scope_class;          // - scope for class
    SYMBOL ret;                 // - return symbol
    TYPE type_ret;              // - type of return value
    PTREE expr;                 // - expression in body of function
    SYMBOL previous_func;       // - current function, if any
    SYMBOL refed;               // - referenced symbol
    SEARCH_RESULT *result;      // - info for accessing vfptr

    refed = node->u.symcg.symbol;
    previous_func = CgFrontCurrentFunction();
    func->flag |= SF_INITIALIZED;
    stashed_scope = GetCurrScope();
    scope_class = SymScope( func );
    SetCurrScope(scope_class);
    ScopeBeginFunction( func );
    FunctionBodyStartup( func, &func_fd, FUNC_NO_STACK_CHECK );
    type_ret = SymFuncReturnType( func );
    ret = SymFunctionReturn();
    if( SymIsThisDataMember( refed ) ) {
        expr = NodeAddToLeft( NodeThis(), NodeOffset( refed->u.member_offset ), refed->sym_type );
        expr = NodeFetchReference( expr );
    } else {
        if( SymIsVirtual( refed ) ) {
            VfnReference( refed );
            result = node->u.symcg.result;
            expr = AccessVirtualFnAddress( NodeThis(), result, refed );
        } else {
            expr = MakeNodeSymbol( refed );
        }
    }
    expr->type = ret->sym_type;
    expr = NodeAssign( MakeNodeSymbol( ret ), expr );
    expr->type = type_ret;
    expr->flags |= PTF_LVALUE;
    IcEmitExpr( NodeDone( expr ) );
    CgFrontReturnSymbol( ret );
    FunctionBodyShutdown( func, &func_fd );
    ScopeEnd( SCOPE_FUNCTION );
    SetCurrScope(stashed_scope);
    CgFrontResumeFunction( previous_func );
}


static TYPE dereferenceFnType(  // GET TYPE OF DE-REFERENCING FUNCTION
    TYPE type )                 // - type of object pointed at
{
    TYPE fn_type;               // - function type

    fn_type = MakeSimpleFunction( MakePointerTo( type ), NULL );
    fn_type = MakeCommonCodeData( fn_type );
    return fn_type;
}


TYPE MembPtrDerefFnPtr(   // GET TYPE OF DE-REFERENCING FUNC. POINTER
    void )
{
    return MakePointerTo( dereferenceFnType( GetBasicType( TYP_VOID ) ) );
}


// These names should be notated as being overlayable (this function can
// exist in many object files) but not combinable (cannot be combined
// with a function with a different name), because comparisons of
// member pointers compare the function addresses within member-pointer
// structures.
//
static SYMBOL membPtrOffsetFunc(// GET OFFSET FUNCTION FOR MEMBER
    PTREE node )                // - symbol node
{
    NAME name;                  // - name for offset function
    SCOPE scope;                // - source scope
    SEARCH_RESULT *result;      // - lookup result
    SYMBOL src;                 // - symbol referenced
    SYMBOL tgt;                 // - symbol for offset function

    src = node->u.symcg.symbol;
    scope = SymScope( src );
    name = CppMembPtrOffsetName( src );
    result = ScopeContainsMember( scope, name );
    if( result == NULL ) {
        tgt = SymCreate( dereferenceFnType( TypeReferenced( src->sym_type ) )
                       , SC_MEMBER
                       , SF_REFERENCED
                       , name
                       , scope );
        generateOffsetFunc( tgt, node );
    } else {
        tgt = result->sym_name->name_syms;
        ScopeFreeResult( result );
    }
    return tgt;
}


static PTREE computeNewDelta(   // COMPUTE NEW DELTA
    PTREE new_delta,            // - new delta
    PTREE *new_index,           // - reference[ new_index RVALUE ]
    MEMBER_PTR_CAST *inf )      // - operands information
{
    PTREE node;                 // - node under construction
    int delta;                  // - delta
    TYPE type;                  // - element type

    type = TypeTargetSizeT();
    if( inf->delta_reqd ) {
        delta = ( inf->safe ) ? inf->delta : -inf->delta;
        if( inf->test_reqd ) {
            node = addOffset( NodeDupExpr( &new_delta ), delta, type );
            node = FoldBinary( node );
            node = NodeBinary( CO_COLON, new_delta, node );
            node->type = type;
            node = NodeBinary( CO_QUESTION
                             , NodeCompareToZero( NodeDupExpr( new_index ) )
                             , node );
            node->type = type;
        } else {
            node = addOffset( new_delta, delta, type );
        }
        new_delta = FoldBinary( node );
    }
    return new_delta;
}


static PTREE computeNewIndex(   // COMPUTE NEW INDEX
    PTREE new_index,            // - new_index RVALUE
    MEMBER_PTR_CAST *inf )      // - operands information
{
    TYPE type;                  // - element type
    PTREE node;                 // - node under construction
    PTREE off_node;             // - offset node

    if( inf->mapping_reqd ) {
        type = TypeTargetSizeT();
        if( inf->mapping == NULL ) {
            off_node = NodeOffset( inf->vb_index * TARGET_UINT );
            off_node->type = type;
            if( inf->single_mapping ) {
                node = NodeBinary( (inf->vb_index == 0 ) ? CO_GT : CO_EQ
                                 , NodeDupExpr( &new_index )
                                 , NodeOffset( inf->single_test * TARGET_UINT ) );
                node = NodeSetBooleanType( node );
                node = FoldBinary( node );
                new_index = NodeBinary( CO_COLON, off_node, new_index );
                new_index->type = type;
                new_index = NodeBinary( CO_QUESTION
                                      , node
                                      , new_index );
                new_index->type = type;
                new_index = FoldBinary( new_index );
            } else {
                NodeFreeDupedExpr( new_index );
                new_index = off_node;
            }
        } else {
            node = MakeNodeSymbol( inf->mapping );
            node = NodeBinary( CO_DOT, node, new_index );
            node->type = type;
            node->flags |= PTF_LVALUE;
            new_index = NodeFetch( node );
            new_index->flags &= ~PTF_LVALUE;
        }
    }
    return new_index;
}


static PTREE computeDeltaIndex( // COMPUTE NEW DELTA, INDEX
    PTREE expr,                 // - expression (CO_MEMPTR)
    TYPE type,                  // - expression type
    MEMBER_PTR_CAST *inf )      // - cast information
{
    PTREE original;             // - original expression
    PTREE new_index;            // - computed new index
    PTREE new_delta;            // - computed new delta

    original = expr;
    expr = PTreeOpLeft( original );
    expr = expr->u.subtree[0];
    new_delta = NodeRvalue( expr->u.subtree[1] );
    new_index = NodeRvalue( expr->u.subtree[0]->u.subtree[1] );
    if( inf->safe ) {
        new_delta = computeNewDelta( new_delta, &new_index, inf );
        new_index = computeNewIndex( new_index, inf );
    } else {
        new_index = computeNewIndex( new_index, inf );
        new_delta = computeNewDelta( new_delta, &new_index, inf );
    }
    expr->u.subtree[1] = new_delta;
    expr->flags &= ~ PTF_LVALUE;
    expr = expr->u.subtree[0];
    expr->u.subtree[1] = new_index;
    expr->flags &= ~ PTF_LVALUE;
    original->type = type;
    original->flags &= ~PTF_LVALUE;
    return original;
}


static PTREE makeMembPtrExpr(   // MAKE A MEMBER-PTR EXPRESSION
    TYPE type,                  // - type of member pointer
    PTREE func,                 // - dereferencing function
    PTREE delta,                // - delta expression
    PTREE index )               // - index expression
{
    PTREE expr;                 // - resultant expression

    expr = NodeUnary( CO_MEMPTR_CONST
                    , NodeArguments( index, delta, func, NULL ) );
    expr->type = type;
    return expr;
}


static PTREE makeMembPtrCon(    // MAKE A MEMBER-PTR CONSTANT EXPRESSION
    TYPE type,                  // - type of member pointer
    SYMBOL func,                // - dereferencing function
    unsigned delta,             // - delta offset
    unsigned index )            // - index offset
{
    PTREE deref;                // - expression for dereferencing function

    if( func == NULL ) {
        deref = NodeIntegralConstant( 0, MembPtrDerefFnPtr() );
    } else {
        deref = MakeNodeSymbol( func );
    }
    return makeMembPtrExpr( type
                          , deref
                          , NodeOffset( delta )
                          , NodeOffset( index ) );
}

PTREE MembPtrZero(              // MAKE A NULL MEMBER POINTER CONSTANT
    TYPE type )                 // - type for constant
{
    PTREE zero;

    zero = makeMembPtrCon( type, NULL, 0, 0 );
    return( zero );
}


static PTREE generateMembPtrCon(// GENERATE MEMBER-PTR CONSTANT
    TYPE type,                  // - type of member pointer
    PTREE item_addrof,          // - node for &[ base item for constant ]
    MEMBER_PTR_CAST *inf )      // - conversion information
{
    SYMBOL addrfun;             // - memb-ptr offset function
    PTREE expr;                 // - resultant expression
    PTREE item;                 // - node for base item for constant

    item = PTreeOpLeft( item_addrof );
    addrfun = membPtrOffsetFunc( item );
    NodeFreeSearchResult( item );
    expr = makeMembPtrCon( type, addrfun, 0, 0 );
    expr = computeDeltaIndex( expr, type, inf );
    expr = PTreeCopySrcLocation( expr, item_addrof );
    item_addrof = NodePruneLeft( item_addrof );
    return NodeReplaceTop( item_addrof, expr );
}


static PTREE nextArg(           // PRUNE NEXT ASSIGNMENT ARGUMENT
    PTREE *ref )                // - reference( argument before )
{
    PTREE arg;                  // - next argument
    PTREE retn;                 // - return value

    arg = (*ref)->u.subtree[0];
    *ref = arg;
    retn = arg->u.subtree[1];
    arg->u.subtree[1] = NULL;
    return retn;
}



static PTREE assignMembPtrArg(  // ASSIGN A MEMBER-PTR ARGUMENT
    PTREE *a_left,              // - addr[ left operand ]
    PTREE *a_right )            // - addr[ right operand ]
{
    return NodeAssign( nextArg( a_left ), NodeRvalue( nextArg( a_right ) ) );
}


static PTREE storeMembPtrCon(   // STORE MEMBER-PTR CONSTANT
    PTREE expr,                 // - expression to be stored
    PTREE tgt )                 // - target
{
    PTREE left;                 // - left operand
    PTREE right;                // - right operand
    PTREE result;               // - result

    left = PTreeOpLeft( expr );
    right = PTreeOpRight( expr );
    result = assignMembPtrArg( &left, &right );
    result = NodeComma( result, assignMembPtrArg( &left, &right ) );
    result = NodeComma( result, assignMembPtrArg( &left, &right ) );
    result = NodeReplace( expr , result );
    result = NodeComma( result, tgt );
    result->flags |= PTF_SIDE_EFF;
    return result;
}


static CNV_DIAG diagMembPtrOps= // diagnosis for analyse of memb-ptr operands
{   ERR_MEMB_PTR_NOT_DERIVED
,   ERR_MEMB_PTR_AMBIGUOUS
,   ERR_MEMB_PTR_DERIVED
,   ERR_MEMB_PTR_PRIVATE
,   ERR_MEMB_PTR_PROTECTED
};


// Note: inf->safe is set on input to indicate whether an unsafe (cast)
//       can be performed. On output, it is set to indicate whether an unsafe
//       cast will be performed.
//
static bool analyseMembPtr(     // ANALYSE MEMBER-PTR OPERANDS
    MEMBER_PTR_CAST *inf,       // - operands information
    PTREE expr )                // - expression for errors
{
    bool retn;                  // - true ==> analysis ok
    CNV_RETN cnv;               // - CNV_...: conversion result
    SCOPE scope;                // - used to switch scopes
    bool check_safety;          // - check if safe req'd
    msg_status_t status;        // - status for warning

    check_safety = false;
    switch( TypeCommonDerivation( ScopeClass( inf->derived )
                                , ScopeClass( inf->base ) ) ) {
      case CTD_NO :
        cnv = CNV_IMPOSSIBLE;
        break;
      case CTD_RIGHT :
        check_safety = true;
        cnv = CNV_OK;
        break;
      case CTD_RIGHT_VIRTUAL :
        check_safety = true;
        // drops thru
      case CTD_LEFT_VIRTUAL :
        status = PTreeWarnExpr( expr, ANSI_MPTR_ACROSS_VIRTUAL );
        if( (status & MS_WARNING) == 0 ) {
            cnv = CNV_ERR;
            break;
        }
        if( status & MS_PRINTED ) {
            ConversionDiagnoseInf();
        }
      case CTD_LEFT :
        cnv = CNV_OK;
        break;
      case CTD_RIGHT_AMBIGUOUS :
        check_safety = true;
        // drops thru
      case CTD_LEFT_AMBIGUOUS :
        cnv = CNV_AMBIGUOUS;
        break;
      case CTD_RIGHT_PRIVATE :
        check_safety = true;
        // drops thru
      case CTD_LEFT_PRIVATE :
        cnv = CNV_PRIVATE;
        break;
      case CTD_RIGHT_PROTECTED :
        check_safety = true;
        // drops thru
      case CTD_LEFT_PROTECTED :
        cnv = CNV_PROTECTED;
        break;
    }
    if( check_safety ) {
        if( inf->safe ) {
            cnv = CNV_VIRT_DER;
        } else {
            scope = inf->derived;
            inf->derived = inf->base;
            inf->base = scope;
        }
    } else {
        if( cnv != CNV_IMPOSSIBLE ) {
            inf->safe = true;
        }
    }
    if( CNV_OK == ConversionDiagnose( cnv, expr, &diagMembPtrOps ) ) {
        ScopeMemberPtrCastAction( inf );
#ifndef NDEBUG
        if( PragDbgToggle.dump_mptr ) {
            DumpMemberPtrInfo( inf );
        }
#endif
        retn = true;
    } else {
        retn = false;
    }
    return retn;
}


static CNV_DIAG diagMembFunCnv= // diagnosis for member-ptr conversion
{   ERR_MEMB_PTR_FUN_IMPOSSIBLE
,   ERR_MEMB_PTR_FUN_AMBIGUOUS
,   ERR_MEMB_PTR_DERIVED
,   ERR_MEMB_PTR_FUN_PRIVATE
,   ERR_MEMB_PTR_FUN_PROTECTED
};


static CNV_RETN analyseAddrOfNode( // ANALYSE NODE FOR (& item)
    PTREE item,                 // - source item
    TYPE type_mp,               // - unmodified member-ptr type
    MEMBER_PTR_CAST *castinfo,  // - information for casting
    CNV_REQD reqd_cnv )         // - type of conversion
{
    CNV_RETN retn;              // - return: CNV_...
    SYMBOL base_item;           // - item required
    TYPE type_pted;             // - type pointed at

    base_item = item->u.symcg.symbol;
    type_mp = MemberPtrType( type_mp );
    type_pted = MakePointerTo( type_mp->of );
    if( SymIsThisDataMember( base_item ) ) {
        if( ScopeCheckSymbol( item->u.symcg.result, base_item ) ) {
            retn = CNV_ERR;
        } else {
            retn = CNV_OK;
        }
    } else if( SymIsFunction( base_item ) ) {
        if( ADDR_FN_ONE_USED == NodeAddrOfFun( item, &item ) ) {
            item->u.symcg.symbol = ActualNonOverloadedFunc( base_item, item->u.symcg.result );
            retn = CNV_OK;
        } else {
            switch( reqd_cnv ) {
//            case CNV_CAST :
              case CNV_EXPR :
                retn = CNV_ERR;
                break;
              default :
                retn = ConvertOvFunNode( type_pted, item );
                break;
            }
        }
        if( ( retn == CNV_OK ) || ( retn == CNV_OK_TRUNC ) ) {
            base_item = item->u.symcg.symbol;
            if( ScopeCheckSymbol( item->u.symcg.result, base_item )
             || CNV_ERR == validateOkObjs( item
                                         , true
                                         , base_item->sym_type
                                         , membPtrObject( type_mp ) )
              ) {
                retn = CNV_ERR;
            } else if( SymIsThisFuncMember( base_item ) ) {
                item->type = MakeMemberPointerTo( SymClass( base_item ), base_item->sym_type );
            } else {
                PTreeErrorExpr( item, ERR_MEMB_PTR_ADDR_OF );
                retn = CNV_ERR;
            }
        }
    } else {
        PTreeErrorExpr( item, ERR_MEMB_PTR_ADDR_OF );
        retn = CNV_ERR;
    }
    if( ( retn == CNV_OK ) || ( retn == CNV_OK_TRUNC ) ) {
        castinfo->base = item->u.symcg.result->scope;
        castinfo->derived = scopeMembPtrType( type_mp );
        if( ! analyseMembPtr( castinfo, item ) ) {
            retn = CNV_ERR;
        }
    }
    return retn;
}


static PTREE analyseAddrOf(     // ANALYSE (& item)
    PTREE src,                  // - source item
    TYPE type_mp,               // - unmodified member-ptr type
    MEMBER_PTR_CAST *castinfo ) // - information for casting
{
    PTREE item;                 // - item being pointed at
    CNV_REQD reqd_cnv;          // - type of conversion

    item = PTreeOpLeft( src );
    if( nodeIsMembPtrCon( item ) ) {
        if( castinfo->safe ) {
            reqd_cnv = CNV_ASSIGN;
        } else {
            reqd_cnv = CNV_CAST;
        }
        if( CNV_OK != ConversionDiagnose( analyseAddrOfNode( item
                                                           , type_mp
                                                           , castinfo
                                                           , reqd_cnv )
                                        , src
                                        , &diagMembFunCnv ) ) {
            PTreeErrorNode( src );
        }
    } else {
        PTreeErrorExpr( src, ERR_MEMB_PTR_ADDR_OF );
    }
    return src;
}


static PTREE memPtrAddrOfCon(   // MAKE MEMPTR EXPRN FROM &class::item
    PTREE expr,                 // - expression to be converted
    TYPE type_mp,               // - type of member ptr result
    bool safe,                  // - true ==> only safe assignment allowed
    bool init )                 // - true ==> an initialization
{
    MEMBER_PTR_CAST castinfo;   // - information for casting
    PTREE mp_node;              // - member-ptr node

    castinfo.safe = safe;
    castinfo.init_conv = init;
    mp_node = analyseAddrOf( expr, type_mp, &castinfo );
    if( mp_node->op == PT_ERROR ) {
        PTreeErrorNode( expr );
    } else {
        expr = generateMembPtrCon( type_mp, mp_node, &castinfo );
    }
    return expr;
}


static PTREE convertMembPtrExpr(// CONVERT A MEMBER POINTER EXPRESSION
    PTREE expr,                 // - expression (CO_MEMPTR)
    TYPE type,                  // - expression type
    bool safe,                  // - true ==> safe cast only
    bool init )                 // - true ==> initialization expression
{
    MEMBER_PTR_CAST inf;        // - cast information

    inf.init_conv = init;
    inf.safe = safe;
    inf.derived = scopeMembPtrType( type );
    inf.base = scopeMembPtrType( expr->type );
    if( analyseMembPtr( &inf, expr ) ) {
        expr = computeDeltaIndex( expr, type, &inf );
        expr->type = type;
        expr->flags = ( expr->flags & ~PTF_LVALUE ) | PTF_LV_CHECKED;
    }
    return expr;
}


static bool membPtrAddrOfNode(  // TEST IF (& class::member)
    PTREE node )
{
    TYPE mbrptr;
    PTREE fn;
    addr_func_t retn;

    if( NodeIsUnaryOp( node, CO_ADDR_OF ) ) {
        mbrptr = MemberPtrType( node->type );
        if( NULL != mbrptr ) {
            return true;
        } else {
            retn = NodeAddrOfFun( node, &fn );
            if( (retn != ADDR_FN_NONE )  &&
                ( (fn->flags & PTF_COLON_QUALED) ||     // S::foo not just foo
                  ( CompFlags.extensions_enabled ) ) ) { // just foo (MFC ext)
                return (retn == ADDR_FN_MANY ||
                        retn == ADDR_FN_ONE  ||
                        CompFlags.extensions_enabled ); // S::foo OK (ext)
            }
        }
    }
    return false;
}


bool MembPtrZeroConst(          // DETERMINE IF ZERO MEMBER-PTR CONSTANT
    PTREE expr )                // - CO_MEMPTR_CONST expression
{
    expr = expr->u.subtree[0];
    expr = expr->u.subtree[1];

    return NodeIsZeroConstant( expr );
}


bool MembPtrExtension           // TEST IF EXTENDED FUNCTION MEMBER PTR
    ( PTREE expr )              // - expression
{
    return CompFlags.extensions_enabled
        && expr->op == PT_SYMBOL
        && expr->cgop != CO_NAME_THIS
        && SymIsThisFuncMember( expr->u.symcg.symbol );
}


static
bool membPtrExtensionFmt        // TEST IF EXTENDED FUNCTION MEMBER PTR FORMAT
    ( PTREE expr )              // - expression
{
    return CompFlags.extensions_enabled
        && expr->op == PT_SYMBOL
        && expr->cgop != CO_NAME_THIS
        && SymIsFuncMember( expr->u.symcg.symbol );
}


PTREE MembPtrExtend             // FAKE AN ADDRESS-OF NODE FOR BARE FUNCTION
    ( PTREE expr )              // - expression
{
    SYMBOL sym;                 // - symbol for function

    DbgVerify( CompFlags.extensions_enabled, "bad call of MembPtrExtend" );
    sym = expr->u.symcg.symbol;
    expr->flags |= PTF_COLON_QUALED;
    expr->flags &= ~PTF_LVALUE;
    expr = NodeUnaryCopy( CO_ADDR_OF, expr );
    expr->type = MakeMemberPointerTo( SymClass(sym ), sym->sym_type );
    return expr;
}


static MP_TYPE classifyMpExpr(  // CLASSIFY A MEMBER-POINTER EXPRESSION
    PTREE *a_expr )             // - addr[ expression ]
{
    PTREE expr;                 // - expression
    PTREE replace;              // - replacement expression (CO_MEMPTR)
    MP_TYPE retn;               // - type of member-ptr expression
    TYPE type_mp;               // - type of member pointer
    unsigned offset_delta;      // - delta offset
    unsigned offset_index;      // - index offset
    TYPE type;                  // - field type
    PTREE func;                 // - accessed function
    PTREE delta;                // - accessed delta
    PTREE index;                // - accessed index

    expr = PTreeOp( a_expr );
    type_mp = MemberPtrType( (*a_expr)->type );
    if( NodeIsUnaryOp( expr, CO_MEMPTR_CONST ) ) {
        if( MembPtrZeroConst( expr ) ) {
            retn = MP_ZERO;
        } else {
            expr = expr->u.subtree[0];
            if( expr->u.subtree[1]->op == PT_SYMBOL ) {
                expr = expr->u.subtree[0];
                if( NULL == NodeGetConstantNode( expr->u.subtree[1] ) ) {
                    retn = MP_EXPR;
                } else {
                    expr = expr->u.subtree[0];
                    if( NULL == NodeGetConstantNode( expr->u.subtree[1] ) ) {
                        retn = MP_EXPR;
                    } else {
                        retn = MP_CONST;
                    }
                }
            } else {
                retn = MP_EXPR;
            }
        }
    } else if( NodeIsZeroConstant( expr ) ) {
        replace = MembPtrZero( TypeGetCache( TYPC_VOID_MEMBER_PTR ) );
        *a_expr = NodeReplaceTop( *a_expr, replace );
        retn = MP_ZERO;
    } else if( membPtrAddrOfNode( expr ) ) {
        retn = MP_ADDR_OF;
    } else if( NULL != type_mp ) {
        expr = *a_expr;
        if( 0 == ( expr->flags & PTF_LVALUE ) ) {
            if( ( PTreeOpFlags( expr ) & PTO_RVALUE )
              &&( 0 == ( PTF_LVALUE & expr->u.subtree[0]->flags ) ) ) {
                PTREE dlt;      // - node to be deleted
                dlt = expr;
                expr = expr->u.subtree[0];
                PTreeFree( dlt );
            } else {
                expr = membPtrTemporary( expr );
            }
        }
        type = MakePointerTo( dereferenceFnType( type_mp->of ) );
        func = accessOp( NodeDupExpr( &expr ), 0, type );
        type = memberPtrLayout( &offset_delta, &offset_index );
        delta = accessOp( NodeDupExpr( &expr ), offset_delta, type );
        index = accessOp( expr, offset_index, type );
        expr = makeMembPtrExpr( expr->type, func, delta, index );
        expr->flags |= PTF_LVALUE;
        *a_expr = expr;
        retn = MP_EXPR;
    } else if( membPtrExtensionFmt( expr ) ) {
        retn = MP_MEMB_FN;
    } else {
        retn = MP_INVALID;
    }
    return retn;
}


CNV_RETN MembPtrAssign(         // ASSIGNMENT/INITIALIZATION OF MEMBER POINTER
    PTREE *a_expr )             // - addr[ assignment expression ]
{
    PTREE expr;                 // - assignment expression
    PTREE tgt;                  // - target operand
    SYMBOL sym;                 // - symbol being initialized/assigned
    CNV_RETN retn;              // - return: CNV_...

    expr = *a_expr;
    retn = MembPtrConvert( &expr->u.subtree[1]
                         , expr->u.subtree[0]->type
                         , CNV_INIT_COPY );
    if( retn == CNV_OK ) {
        tgt = NodeDupExpr( &expr->u.subtree[0] );
        classifyMpExpr( &expr->u.subtree[0] );
        switch( classifyMpExpr( &expr->u.subtree[1] ) ) {
          case MP_CONST :
          case MP_ZERO :
            if( NodeIsBinaryOp( expr, CO_INIT ) ) {
                if( expr->flags & PTF_KEEP_MPTR_SIMPLE ) {
                    // we won't be executing this so keep it simple
                    NodeFreeDupedExpr( tgt );
                    NodeFreeDupedExpr( expr->u.subtree[0] );
                    expr->u.subtree[0] = NULL;
                    break;
                }
                if( tgt->op == PT_SYMBOL ) {
                    sym = tgt->u.symcg.symbol;
                    if( ( sym->id == SC_AUTO )
                      ||( sym->id == SC_REGISTER ) ) {
                        expr = storeMembPtrCon( expr, tgt );
                    } else {
                        expr->u.subtree[0] = NodeReplace( expr->u.subtree[0]
                                                        , tgt );
                        expr->type = tgt->type;
                        expr->flags |= PTF_LVALUE;
                    }
                } else {
                    expr = storeMembPtrCon( expr, tgt );
                }
            } else {
                expr = storeMembPtrCon( expr, tgt );
            }
            break;
          default :
            expr = storeMembPtrCon( expr, tgt );
            break;
        }
        *a_expr = expr;
        if( expr->op == PT_ERROR ) {
            retn = CNV_ERR;
        } else {
            retn = CNV_OK;
        }
    }
    return retn;
}


static CNV_DIAG diagDerefPtr =  // diagnosis for ptr conversion
{   ERR_MP_PTR_IMPOSSIBLE
,   ERR_MP_PTR_AMBIGUOUS
,   0
,   ERR_MP_PTR_PRIVATE
,   ERR_MP_PTR_PROTECTED
};


static CNV_DIAG diagDerefObject = // diagnosis for ptr conversion
{   ERR_MP_OBJECT_IMPOSSIBLE
,   ERR_MP_OBJECT_AMBIGUOUS
,   0
,   ERR_MP_OBJECT_PRIVATE
,   ERR_MP_OBJECT_PROTECTED
};


static PTREE doDereference(     // GENERATE DE-REFERENCING CODE
    PTREE lhs,                  // - node: ptr. to class
    PTREE rhs )                 // - node: member pointer object (lvalue)
{
    TYPE type_mp;               // - type of member pointer
    TYPE type_mc;               // - type of member pointer's class
    TYPE type_cp;               // - type of class pointer for member-ptr class
    TYPE type_fn;               // - type of de-referencing function
    TYPE type_offset;           // - type of the offset
    SCOPE scope;                // - scope of class for member-ptr
    PTREE expr;                 // - resultant expression
    PTREE expr_root;            // - resultant expression root
    PTREE func;                 // - function node
    PTREE delta;                // - delta node
    PTREE index;                // - index node
    PTREE temp;                 // - temporary
    PTREE *r_rhs;               // - reference[ member-ptr constant ]

    type_mp = MemberPtrType( rhs->type );
    type_mc = MemberPtrClass( type_mp );
    type_cp = MakePointerTo( type_mc );
    scope = TypeScope( type_mc );
    type_fn = dereferenceFnType( type_mp->of );
    r_rhs = PTreeRef( &rhs );
    expr_root = *r_rhs;
    *r_rhs = NULL;
    expr = expr_root;
    expr = expr->u.subtree[0];
    func = NodeRvalue( expr->u.subtree[1] );
    expr->u.subtree[1] = NULL;
    expr = expr->u.subtree[0];
    delta = NodeRvalue( expr->u.subtree[1] );
    expr->u.subtree[1] = NULL;
    type_offset = delta->type;
    if( ScopeHasVirtualBases( scope ) ) {
        expr = expr->u.subtree[0];
        index = NodeRvalue( expr->u.subtree[1] );
        expr->u.subtree[1] = NULL;
        temp = accessOp( NodeDupExpr( &lhs )
                       , ScopeVBPtrOffset( scope )
                       , MakePointerTo( type_offset ) );
        expr = NodeAddToLeft( NodeFetch( NodeDupExpr( &temp ) )
                        , NodeDupExpr( &index )
                        , type_offset );
        expr->flags |= PTF_LVALUE;
        expr = NodeAddToLeft( temp, NodeFetch( expr ), type_cp );
        expr->flags &= ~ PTF_LVALUE;
        expr = NodeBinary( CO_COLON, expr, lhs );
        expr->type = type_cp;
        expr = NodeBinary( CO_QUESTION, NodeCompareToZero( index ), expr );
        expr->type = type_cp;
    } else {
        expr = lhs;
    }
    NodeFreeDupedExpr( expr_root );
    expr = NodeAddToLeft( delta, expr, type_cp );
    expr = NodeArg( expr );
    func = NodeUnaryCopy( CO_CALL_SETUP_IND, func );
    func->type = type_fn;
    expr = NodeBinary( CO_CALL_EXEC_IND, func, expr );
    expr->type = type_mp->of;
    expr->flags |= PTF_LVALUE;
    if( FunctionDeclarationType( type_mp->of ) ) {
        expr->flags |= PTF_CALLED_ONLY;
    }
    *r_rhs = NodeReplace( *r_rhs, expr );
    return rhs;
}


// This routine could put out debugging code to ensure that a NULL
// member pointer is not being dereferenced.
//
PTREE MembPtrDereference(       // DO '.*' AND '->*' operations
    PTREE expr )                // - expression to be processed
{
    PTREE left;                 // - "this" operand
    PTREE right;                // - member pointer operand
    TYPE this_type;             // - type for LHS conversion
    TYPE unmod;                 // - unmodified type
    TYPE ftype;                 // - NULL or function type
    type_flag flags;            // - flags for object type
    CNV_DIAG *diag;             // - diagnosis to be used

    right = expr->u.subtree[1];
    switch( classifyMpExpr( &right ) ) {
        PTREE *ref;         // - reference to right
        SYMBOL sym;         // - symbol being dereferenced
      case MP_MEMB_FN :
      case MP_INVALID :
        PTreeErrorExpr( expr, ERR_RIGHT_MEMB_PTR_OPERAND );
        break;
      case MP_ZERO :
        PTreeErrorExpr( expr, ERR_MEMB_PTR_DEREF_ZERO );
        break;
      case MP_ADDR_OF :
        ref = PTreeRefRight( expr );
        ref = &((*ref)->u.subtree[0]);
        right = *ref;
        *ref = NULL;
        expr->u.subtree[1] = NodeComma( NodePruneTop( expr->u.subtree[1] )
                                      , right );
        sym = right->u.symcg.symbol;
        if( expr->cgop == CO_DOT_STAR ) {
            expr->cgop = CO_DOT;
        } else {
            expr->cgop = CO_ARROW;
        }
        expr->type = right->type;
        if( SymIsThisDataMember( sym ) ) {
            if( ! AnalyseThisDataItem( &expr ) ) {
                PTreeErrorNode( expr );
            }
        } else if( ! SymIsThisFuncMember( sym ) ) {
            PTreeErrorExpr( expr, ERR_RIGHT_MEMB_PTR_OPERAND );
        }
        break;
      default :
        unmod = TypedefModifierRemoveOnly( right->type );
        this_type = MemberPtrClass( unmod );
        ftype = FunctionDeclarationType( unmod->of );
        if( NULL == ftype ) {
            TypeModFlags( unmod->of, &flags );
        } else {
            flags = TypeArgList( ftype )->qualifier;
        }
        flags &= TF1_CV_MASK;
        left = expr->u.subtree[0];
        unmod = TypedefModifierRemoveOnly( NodeType( left ) );
        {   type_flag lflags;
            TypeModFlags( unmod->of, &lflags );
            flags |= lflags & TF1_THIS_MASK;
        }
        if( 0  != flags ) {
            this_type = MakeModifiedType( this_type, flags );
        }
        if( expr->cgop == CO_DOT_STAR ) {
            diag = &diagDerefObject;
            this_type = MakeReferenceTo( this_type );
        } else {
            diag = &diagDerefPtr;
            this_type = MakePointerTo( this_type );
        }
        left->flags |= PTF_PTR_NONZERO;
        left = CastImplicit( left, this_type, CNV_EXPR, diag );
        expr->u.subtree[0] = left;
        if( PT_ERROR == left->op ) {
            PTreeErrorNode( expr );
        } else {
            expr->u.subtree[0] = NULL;
            expr->u.subtree[1] = NULL;
            expr = NodeReplace( expr, doDereference( left, right ) );
        }
        break;
    }
    return expr;
}


CNV_RETN MembPtrReint(          // REINTERPRET A MEMBER POINTER
    PTREE *a_expr,              // - addr[ conversion expression, not class ]
    TYPE tgt_type )             // - target type (member-pointer)
{
    CNV_RETN retn;              // - return: CNV_...
    PTREE expr;                 // - conversion expression
    unsigned classification;    // - operand classification

    classification = classifyMpExpr( a_expr );
    expr = *a_expr;
    switch( classification ) {
      case MP_INVALID :
        PTreeErrorExpr( expr, ERR_MEMB_PTR_OPERAND );
        retn = CNV_ERR;
        break;
      case MP_MEMB_FN :
      { SYMBOL sym;             // - symbol for node
        sym = expr->u.symcg.symbol;
        expr->flags |= PTF_COLON_QUALED;
        expr = NodeUnaryCopy( CO_ADDR_OF, expr );
        expr->type = MakeMemberPointerTo( SymClass(sym ), sym->sym_type );
      } // drops thru
      case MP_ADDR_OF :
        expr = memPtrAddrOfCon( expr, expr->type, false, false );
        DbgVerify( expr->op != PT_ERROR, "should work" );
        // drops thru
      case MP_EXPR :
      case MP_CONST :
      case MP_ZERO :
        expr->type = tgt_type;
        expr = membPtrStoreTemp( expr );
        retn = CNV_OK;
        break;
    }
    *a_expr = expr;
    return retn;
}


CNV_RETN MembPtrConvert(        // CONVERT A MEMBER POINTER
    PTREE *a_expr,              // - addr[ conversion expression, not class ]
    TYPE tgt_type,              // - target type (member-pointer)
    CNV_REQD reqd_cnv )         // - type of conversion
{
    CNV_RETN retn;              // - return: CNV_...
    PTREE expr;                 // - conversion expression
    bool safe;                  // - true ==> only safe conversion allowed
    bool init;                  // - true ==> an initialization or assignment
    unsigned classification;    // - operand classification

    switch( reqd_cnv ) {
      case CNV_INIT :
      case CNV_FUNC_ARG :
      case CNV_FUNC_RET :
      case CNV_ASSIGN :
        safe = true;
        init = true;
        break;
      case CNV_CAST :
        safe = false;
        init = false;
        break;
      default :
        safe = true;
        init = false;
        break;
    }
    classification = classifyMpExpr( a_expr );
    expr = *a_expr;
    ConversionTypesSet( expr->type, tgt_type );
    switch( classification ) {
      case MP_INVALID :
        PTreeErrorExpr( expr, ERR_MEMB_PTR_OPERAND );
        retn = CNV_ERR;
        break;
      case MP_MEMB_FN :
      { SYMBOL sym;             // - symbol for node
        sym = expr->u.symcg.symbol;
        expr->flags |= PTF_COLON_QUALED;
        expr = NodeUnaryCopy( CO_ADDR_OF, expr );
        expr->type = MakeMemberPointerTo( SymClass(sym ), sym->sym_type );
        *a_expr = expr;
      } // drops thru
      case MP_ADDR_OF :
        if( (! safe )
         || CNV_ERR != validateAddrOfObj( expr, tgt_type ) ) {
            expr = memPtrAddrOfCon( expr, tgt_type, safe, init );
            *a_expr = expr;
            if( expr->op == PT_ERROR ) {
                PTreeErrorNode( expr );
                retn = CNV_ERR;
            } else {
                retn = CNV_OK;
            }
        } else {
            retn = CNV_ERR;
        }
        break;
      case MP_ZERO :
        expr->type = tgt_type;
        expr->flags &= PTF_LVALUE;
        *a_expr = expr;
        retn = CNV_OK;
        break;
      case MP_EXPR :
        init = false;
      case MP_CONST :
        if( (! safe )
//        || reqd_cnv == CNV_CAST
          || CNV_OK == validateMpObjs( expr, true, expr->type, tgt_type ) ) {
            expr = convertMembPtrExpr( expr, tgt_type, safe, init );
            *a_expr = expr;
            if( expr->op == PT_ERROR ) {
                retn = CNV_ERR;
            } else {
                retn = CNV_OK;
            }
        } else {
            retn = CNV_ERR;
        }
        break;
    }
    return retn;
}


static bool validateComparisonTypes( // VERIFY CONVERSION IS POSSIBLE
    PTREE expr )                // expression
{
    PTREE left;                 // - left operand
    PTREE right;                // - right operand
    bool rc;                    // - true ==> can convert
    CNV_RETN retn;              // - return from validation

    left = PTreeOpLeft( expr );
    right = PTreeOpRight( expr );
    if( ( left->op == PT_ERROR ) || ( right->op == PT_ERROR ) ) {
        PTreeErrorNode( expr );
        rc = false;
    } else {
        TYPE host_left;         // - host class to left
        TYPE host_right;        // - host class to right
        host_right = memberPtrHostClass( right->type );
        host_left = memberPtrHostClass( left->type );
        switch( TypeCommonDerivation( host_left, host_right ) ) {
          case CTD_LEFT :
            if( host_left == host_right ) {
                retn = validateMpObjs( expr
                                         , false
                                         , right->type
                                         , left->type );
                if( retn == CNV_OK ) break;
                if( retn == CNV_ERR ) break;
                // will drop thru to test left --> right
            } else {
          case CTD_LEFT_VIRTUAL :
          case CTD_LEFT_PRIVATE :
          case CTD_LEFT_PROTECTED :
                retn = validateMpObjs( expr
                                         , true
                                         , right->type
                                         , left->type );
                break;
            }
            // drops thru
          case CTD_RIGHT :
          case CTD_RIGHT_VIRTUAL :
          case CTD_RIGHT_PRIVATE :
          case CTD_RIGHT_PROTECTED :
            retn = validateMpObjs( expr, true, left->type, right->type );
            break;
          default :
            PTreeErrorExpr( expr, ERR_MEMB_PTR_CMP_NOT_DERIVED );
            retn = CNV_ERR;
            break;
        }
        if( CNV_OK == retn ) {
            rc = true;
        } else {
            rc = false;
        }
    }
    return rc;
}


static PTREE compareConst(      // BUILD COMPARISON CONSTANT
    PTREE *r_con )              // - reference( constant )
{
    PTREE con;                  // - constant
    MEMBER_PTR_CAST castinfo;   // - information for casting

    castinfo.safe = true;
    castinfo.init_conv = true;
    con = *r_con;
    con = analyseAddrOf( con, con->type, &castinfo );
    if( con->op != PT_ERROR ) {
        con = generateMembPtrCon( con->type, con, &castinfo );
    }
    *r_con = con;
    return con;
}


static PTREE replaceCompare(    // REPLACE COMPARISON WITH A CONSTANT
    PTREE expr,                 // - expression to be replaced
    bool val )                  // - true ==> operands are the same
{
    PTREE con;                  // - constant node

    if( expr->cgop == CO_NE ) {
        val = !val;
    }
    con = PTreeBoolConstant( val );
    return NodeReplace( expr, con );
}


static MP_TYPE classifyCompare( // CLASSIFY COMPARISON
    PTREE *ref )                // - reference[ comparison operand ]
{
    MP_TYPE retn;               // - operand type

    retn = classifyMpExpr( ref );
    switch( retn ) {
      case MP_ADDR_OF :
        *ref = compareConst( ref );
        retn = MP_CONST;
        break;
      case MP_MEMB_FN :
        retn = MP_INVALID;
        break;
      default :
        break;
    }
    return retn;
}


static PTREE compareOperand(    // GET COMPARISON OPERAND
    PTREE *ref )                // - reference[ CO_MEMPTR_CONST operand ]
{
    PTREE operand;              // - function operand
    PTREE *a_operand;           // - operand ptr

    ref = PTreeRef( ref );
    a_operand = &( (*ref)->u.subtree[0]->u.subtree[1] );
    operand = *a_operand;
    *a_operand = NULL;
    return operand;
}


static PTREE doCompare(         // DO A COMPARISON
    PTREE expr )                // - expression
{
    PTREE left;                 // - left operand
    PTREE right;                // - right operand

    left = compareOperand( &expr->u.subtree[0] );
    right = compareOperand( &expr->u.subtree[1] );
    if( ( left->op == PT_SYMBOL )
      &&( right->op == PT_SYMBOL ) ) {
        expr = replaceCompare( expr
                             , right->u.symcg.symbol == left->u.symcg.symbol
                             );
    } else {
        expr->u.subtree[0] = NodeReplace( expr->u.subtree[0]
                                        , NodeRvalue( left ) );
        expr->u.subtree[1] = NodeReplace( expr->u.subtree[1]
                                        , NodeRvalue( right ) );
    }
    return expr;
}


PTREE MembPtrCommonType(        // IMPLICIT CONVERSION TO COMMON TYPE
    PTREE expr )                // - binary expression
{
    PTREE *r_left;              // - reference[ left operand ]
    PTREE *r_right;             // - reference[ right operand ]
    TYPE orig_left;             // - original left type
    TYPE orig_right;            // - original right type
    PTREE right;                // - right actual
    PTREE left;                 // - left actual

    r_left = PTreeRefLeft( expr );
    r_right = PTreeRefRight( expr );
    right = *r_right;
    left = *r_left;
    orig_left = NodeType( left );
    orig_right = NodeType( right );
    ConversionTypesSet( orig_right, orig_left );
    switch( MP_OPS( classifyCompare( r_left  )
                  , classifyCompare( r_right ) ) ) {
      case MP_OPS( MP_ZERO, MP_ZERO ) :
      case MP_OPS( MP_ZERO, MP_CONST ) :
      case MP_OPS( MP_CONST, MP_ZERO ) :
      case MP_OPS( MP_EXPR, MP_ZERO ) :
      case MP_OPS( MP_ZERO, MP_EXPR ) :
      case MP_OPS( MP_CONST, MP_EXPR ) :
      case MP_OPS( MP_CONST, MP_CONST ) :
      case MP_OPS( MP_EXPR, MP_CONST ) :
      case MP_OPS( MP_EXPR, MP_EXPR ) :
      { TYPE host_left;         // - host class to left
        TYPE host_right;        // - host class to right
        right = *r_right;
        left = *r_left;
        if( left->type == TypeGetCache( TYPC_VOID_MEMBER_PTR ) ) {
            left->type = orig_right;
            orig_left = orig_right;
        } else if( right->type == TypeGetCache( TYPC_VOID_MEMBER_PTR ) ) {
            right->type = orig_left;
            orig_right = orig_left;
        }
        if( ! validateComparisonTypes( expr ) ) break;
        host_right = memberPtrHostClass( orig_right );
        host_left = memberPtrHostClass( orig_left );
        switch( TypeCommonDerivation( host_left, host_right ) ) {
          case CTD_LEFT :
            if( host_left != host_right
             || CNV_OK == validateMpObjs( expr
                                        , false
                                        , orig_right
                                        , orig_left ) ) {
          case CTD_LEFT_VIRTUAL :
          case CTD_LEFT_PRIVATE :
          case CTD_LEFT_PROTECTED :
                expr->type = orig_left;
                if( CNV_ERR == MembPtrConvert( &expr->u.subtree[1]
                                             , orig_left
                                             , CNV_EXPR ) ) {
                    PTreeErrorNode( expr );
                }
                break;
            } else {
          case CTD_RIGHT :
          case CTD_RIGHT_VIRTUAL :
          case CTD_RIGHT_PRIVATE :
          case CTD_RIGHT_PROTECTED :
                ConversionTypesSet( orig_left, orig_right );
                expr->type = orig_right;
                if( CNV_ERR == MembPtrConvert( &expr->u.subtree[0]
                                             , orig_right
                                             , CNV_EXPR ) ) {
                    PTreeErrorNode( expr );
                }
                break;
            }
          default :
            PTreeErrorExpr( expr, ERR_MEMB_PTR_CMP_NOT_DERIVED );
            break;
        }
      } break;
      case MP_OPS( MP_ZERO, MP_INVALID ) :
      case MP_OPS( MP_CONST, MP_INVALID ) :
      case MP_OPS( MP_EXPR, MP_INVALID ) :
        PTreeErrorExpr( expr, ERR_RIGHT_MEMB_PTR_OPERAND );
        break;
      case MP_OPS( MP_INVALID, MP_ZERO ) :
      case MP_OPS( MP_INVALID, MP_CONST ) :
      case MP_OPS( MP_INVALID, MP_EXPR ) :
      case MP_OPS( MP_INVALID, MP_INVALID ) :
        PTreeErrorExpr( expr, ERR_LEFT_MEMB_PTR_OPERAND );
        break;
      DbgDefault( "MembPtrCommonType -- bad" );
    }
    return expr;
}


PTREE MembPtrCompare(           // COMPARISON OF MEMBER POINTERS
    PTREE expr )                // - comparison expression
{
    PTREE *r_left;              // - reference[ left operand ]
    PTREE *r_right;             // - reference[ right operand ]

    r_left = PTreeRefLeft( expr );
    r_right = PTreeRefRight( expr );
    switch( MP_OPS( classifyCompare( r_left  )
                  , classifyCompare( r_right ) ) ) {
      case MP_OPS( MP_ZERO, MP_ZERO ) :
        expr = replaceCompare( expr, true );
        break;
      case MP_OPS( MP_ZERO, MP_CONST ) :
      case MP_OPS( MP_CONST, MP_ZERO ) :
        expr = replaceCompare( expr, false );
        break;
      case MP_OPS( MP_EXPR, MP_ZERO ) :
      case MP_OPS( MP_ZERO, MP_EXPR ) :
        expr = doCompare( expr );
        break;
      case MP_OPS( MP_CONST, MP_EXPR ) :
      case MP_OPS( MP_CONST, MP_CONST ) :
      case MP_OPS( MP_EXPR, MP_CONST ) :
      case MP_OPS( MP_EXPR, MP_EXPR ) :
        if( validateComparisonTypes( expr ) ) {
            expr = doCompare( expr );
        }
        break;
      case MP_OPS( MP_ZERO, MP_INVALID ) :
      case MP_OPS( MP_CONST, MP_INVALID ) :
      case MP_OPS( MP_EXPR, MP_INVALID ) :
        PTreeErrorExpr( expr, ERR_RIGHT_MEMB_PTR_OPERAND );
        break;
      case MP_OPS( MP_INVALID, MP_ZERO ) :
      case MP_OPS( MP_INVALID, MP_CONST ) :
      case MP_OPS( MP_INVALID, MP_EXPR ) :
      case MP_OPS( MP_INVALID, MP_INVALID ) :
        PTreeErrorExpr( expr, ERR_LEFT_MEMB_PTR_OPERAND );
        break;
      DbgDefault( "MembPtrCompare -- bad" );
    }
    return expr;
}


static SYMBOL funcArgSym(       // DETERMINE COMPONENT'S SYMBOL, IF THERE
    PTREE component,            // - component to be checked
    unsigned offset )           // - offset from field
{
    PTREE con;                  // - constant node for offset
    SYMBOL sym;                 // - symbol for member pointer

    component = component->u.subtree[1];
    if( PTreeOpFlags( component ) & PTO_RVALUE ) {
        component = component->u.subtree[0];
    }
    if( NodeIsBinaryOp( component, CO_DOT )
     || NodeIsBinaryOp( component, CO_ARROW ) ) {
        con = NodeGetConstantNode( component->u.subtree[1] );
        if( con == NULL ) {
            sym = NULL;
        } else if( offset != con->u.uint_constant ) {
            sym = NULL;
        } else {
            component = component->u.subtree[0];
            if( component->op == PT_SYMBOL ) {
                sym = component->u.symcg.symbol;
            } else {
                sym = NULL;
            }
        }
    } else {
        sym = NULL;
    }
    return sym;
}


PTREE MembPtrFuncArg(           // EXPRESSION FOR FUNCTION ARGUMENT
    PTREE arg )                 // - converted expression
{
    SYMBOL sym;                 // - symbol for function arg
    PTREE component;            // - member-ptr component
    unsigned offset_delta;      // - delta offset
    unsigned offset_index;      // - index offset

    component = arg->u.subtree[0];
    sym = funcArgSym( component, 0 );
    if( sym != NULL ) {
        memberPtrLayout( &offset_delta, &offset_index );
        component = component->u.subtree[0];
        if( sym == funcArgSym( component, offset_delta ) ) {
            component = component->u.subtree[0];
            if( sym != funcArgSym( component, offset_index ) ) {
                sym = NULL;
            }
        } else {
            sym = NULL;
        }
    }
    if( sym == NULL ) {
        arg = membPtrStoreTemp( arg );
    } else {
//      arg = NodeReplace( arg, MakeNodeSymbol( sym ) );
        arg = NodeReplace( arg, NodeMakeCallee( sym ) );
    }
    return NodeRvalue( arg );
}


PTREE ConvertMembPtrConst(      // CONVERT TO TEMP. A MEMBER-PTR CONST
    PTREE *a_expr )             // - addr [expression]
{
    PTREE expr;                 // - expression

    expr = *a_expr;
    if( (PTreeOpFlags( expr ) & PTO_RVALUE) == 0 ) {
        MembPtrConvert( a_expr, expr->type, CNV_EXPR );
        expr = *a_expr;
        if( NodeIsUnaryOp( expr, CO_MEMPTR_CONST ) ) {
            expr = membPtrStoreTemp( expr );
            *a_expr = expr;
        }
    }
    return expr;
}
