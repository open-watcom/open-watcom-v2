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
#include "calldiag.h"
#include "memmgr.h"
#include "ring.h"
#include "defarg.h"
#include "class.h"
#include "objmodel.h"

static CNV_DIAG diagCtor =      // diagnosis for CTORing
    {   ERR_CTOR_IMPOSSIBLE
    ,   ERR_CTOR_AMBIGUOUS
    ,   ERR_CALL_WATCOM
    ,   ERR_CALL_WATCOM
    ,   ERR_CALL_WATCOM
    };


static bool ctorDefineDefault(  // DEFINE A DEFAULT CTOR
    TYPE cl_type,               // - class type
    arg_list *alist )           // - arguments list
{
    bool retn;                  // - TRUE ==> a default CTOR was defined
    SYMBOL ctor;                // - CTOR symbol

    if( ! TypeDefined( cl_type ) ) {
        retn = FALSE;
    } else if( alist->num_args == 0 ) {
        if( CNV_OK == ClassDefaultCtorDefine( cl_type, &ctor ) ) {
            retn = TRUE;
        } else {
            retn = FALSE;
        }
    } else if( alist->num_args == 1 ) {
        if( NULL == ClassAddDefaultCopy( cl_type->u.c.scope ) ) {
            retn = FALSE;
        } else {
            retn = TRUE;
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


static bool ctorDefineDefaultCnv(   // DEFINE A DEFAULT CTOR FOR CONVERSION
    TYPE cl_type,                   // - class type
    arg_list *alist )               // - arguments list
{
    bool retn;                      // - TRUE ==> a default CTOR was defined

    if( ! TypeDefined( cl_type ) ) {
        retn = FALSE;
    } else if( alist->num_args == 0 ) {
        retn = ctorDefineDefault( cl_type, alist );
    } else if( alist->num_args == 1 ) {
        cl_type = ClassTypeForType( cl_type );
        if( NULL == ClassAddDefaultCopy( cl_type->u.c.scope ) ) {
            retn = FALSE;
        } else {
            retn = TRUE;
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


static SEARCH_RESULT *ctorResult(   // GET SEARCH RESULT FOR A CTOR
    SCOPE access,               // - accessing derived class
    TOKEN_LOCN *locn,           // - location for any access errors
    TYPE cl_type )              // - a class type
{
    SCOPE class_scope;
    NAME ctor_name;
    SEARCH_RESULT *result;

    ctor_name = CppConstructorName();
    class_scope = TypeScope( cl_type );
    if( access != NULL ) {
        result = ScopeFindBaseMember( class_scope, ctor_name );
    } else {
        result = ScopeContainsMember( class_scope, ctor_name );
    }
    if( locn != NULL && result != NULL ) {
        ScopeResultErrLocn( result, locn );
    }
    return result;
}


FNOV_LIST *CtorFindList( TYPE src, TYPE tgt )
/*******************************************/
// Find all ctors from src to tgt type
// - does not rank or resolve list
{
    arg_list            alist;          // argument list for ctor
    SEARCH_RESULT       *search_result; // search result
    SYMBOL              sym;            // current symbol in list
    FNOV_LIST           *list;          // FNOV_LIST of ctors found

    InitArgList( &alist );
    alist.num_args = 1;
    alist.type_list[0] = src;

    search_result = ctorResult( NULL, NULL, tgt );
    if( search_result == NULL ) {
        if( ctorDefineDefaultCnv( tgt, &alist ) ) {
            search_result = ctorResult( NULL, NULL, tgt );
        }
    }
    list = NULL;
    if( search_result != NULL ) {
        RingIterBeg( search_result->sym_name->name_syms, sym ) {
            BuildCtorList( &list, sym, &alist );
        } RingIterEnd( sym )
        ScopeFreeResult( search_result );
    }
    return list;
}


static FNOV_RESULT ctorExplicitDiag(// FIND CONSTRUCTOR FOR ARGUMENT LIST
    SCOPE access,               // - accessing derived class
    TYPE cl_type,               // - type for class
    arg_list *alist,            // - arguments list
    PTREE *ptlist,              // - parse tree nodes for arguments
    TOKEN_LOCN *locn,           // - location for access errors
    SYMBOL *ctor,               // - ctor symbol
    FNOV_DIAG *fnov_diag )      // - diagnosis information
{
    SEARCH_RESULT *result;      // - search result
    FNOV_RESULT ovret;          // - overload resolution result

    cl_type = ClassTypeForType( cl_type );
    result = ctorResult( access, locn, cl_type );
    if( result == NULL ) {
        ovret = FNOV_NO_MATCH;
    } else {
        ovret = FuncOverloadedLimitDiag( ctor
                                       , result
                                       , result->sym_name->name_syms
                                       , alist
                                       , ptlist
                                       , FNC_RANKING_CTORS
                                       , NULL
                                       , fnov_diag );
        if( ovret == FNOV_NONAMBIGUOUS ) {
            if( ScopeCheckSymbol( result, *ctor ) ) {
                ovret = FNOV_ERR;
            }
        }
        ScopeFreeResult( result );
    }
    return ovret;
}


static CNV_RETN ctorFindDiag(   // FIND CONSTRUCTOR FOR ARGUMENT LIST
    SCOPE access,               // - accessing derived class
    TYPE cl_type,               // - type for class
    arg_list *alist,            // - arguments list
    PTREE *ptlist,              // - parse tree nodes for arguments
    TOKEN_LOCN *locn,           // - location for access errors
    SYMBOL *ctor,               // - addr( constructor symbol )
    FNOV_DIAG *fnov_diag )      // - diagnosis information
{
    CNV_RETN retn;              // - return: TRUE ==> no error
    FNOV_RESULT ovret;          // - overload resolution result

    *ctor = NULL;
    ovret = ctorExplicitDiag( access, cl_type, alist, ptlist, locn, ctor, fnov_diag );
    if( ovret == FNOV_NO_MATCH ) {
        if( ctorDefineDefault( cl_type, alist ) ) {
            FnovInitDiag( fnov_diag );
            ovret = ctorExplicitDiag( access, cl_type, alist, ptlist, locn, ctor, fnov_diag );
        }
    }
    retn = CNV_ERR;
    switch( ovret ) {
      case FNOV_ERR :
        retn = CNV_ERR;
        break;
      case FNOV_NONAMBIGUOUS :
        retn = CNV_OK;
        break;
      case FNOV_NO_MATCH :
        retn = CNV_IMPOSSIBLE;
        break;
      case FNOV_AMBIGUOUS :
        retn = CNV_AMBIGUOUS;
        break;
      DbgDefault( "unexpected return from ctorExplicitDiag" );
    }
    return retn;
}


CNV_RETN CtorFind(              // FIND CONSTRUCTOR FOR ARGUMENT LIST
    SCOPE access,               // - accessing derived class
    TYPE cl_type,               // - type for class
    arg_list *alist,            // - arguments list
    PTREE *ptlist,              // - parse tree nodes for arguments
    TOKEN_LOCN *locn,           // - location for access errors
    SYMBOL *ctor )              // - addr( constructor symbol )
{
    return ctorFindDiag( access, cl_type, alist, ptlist, locn, ctor, NULL );
}


static CNV_RETN single_arg(     // VERIFY ZERO OR SINGLE CTOR ARGUMENT
    PTREE *a_expr )             // - addr( expression )
{
    CNV_RETN retn;              // - return: CNV_...
    PTREE expr;                 // - expression

    expr = *a_expr;
    if( expr == NULL ) {
        retn = CNV_OK;
    } else if( expr->u.subtree[0] != NULL ) {
        PTreeErrorExpr( expr, ERR_ONE_CTOR_ARG_REQD );
        retn = CNV_ERR;
    } else {
        *a_expr = expr->u.subtree[1];
        PTreeFree( expr );
        retn = CNV_OK;
    }
    return retn;
}


static CNV_RETN analyseCtorClassDiag( // ANALYSE A CLASS CTOR
    TYPE type,                  // - class type for CTOR
    SCOPE access,               // - scope ctor is accessed from
    SYMBOL *ctor,               // - ctor to be filled in
    PTREE initial,              // - initialization arguments (modified)
    PTREE *expr,                // - expression, when CTOR'd
    FNOV_DIAG *fnov_diag )      // - diagnosis information
{
    PTREE node;                 // - converted arguments
    arg_list *alist;            // - arguments structure
    PTREE *ptlist;              // - parse tree for arguments
    unsigned count;             // - # arg.s
    CNV_RETN retn;              // - return: CNV_...
    TOKEN_LOCN err_locn;        // - location for access errors
    TEMP_PT_LIST default_list;  // - default PTREE list
    TEMP_ARG_LIST default_args; // - default arg_list

    node = NodeReverseArgs( &count, initial );
    alist = ArgListTempAlloc( &default_args, count );
    ptlist = PtListAlloc( default_list, count );
    NodeBuildArgList( alist, ptlist, node, count );
    PTreeExtractLocn( initial, &err_locn );
    retn = ctorFindDiag( access
                       , type
                       , alist
                       , ptlist
                       , &err_locn
                       , ctor
                       , fnov_diag );
    switch( retn ) {
      case CNV_OK :
        node = NodeConvertCallArgList( node
                                     , count
                                     , (*ctor)->sym_type
                                     , &node );
        if( ( node != NULL ) && ( node->op == PT_ERROR ) ) {
            retn = CNV_ERR;
        }
        break;
      case CNV_AMBIGUOUS :
        if( fnov_diag != NULL ) {
            CallDiagAmbiguous( node, ERR_CTOR_AMBIGUOUS, fnov_diag );
            retn = CNV_ERR;
        }
        break;
      case CNV_IMPOSSIBLE :
        if( count == 1
         && StructType( initial->type ) == type
         && OMR_CLASS_VAL == ObjModelArgument( type ) ) {
            retn = CNV_OK;
            break;
        }
        // drops thru
      default :
        if( initial != NULL ) {
            ConversionTypesSet( NodeType( initial ), type );
        }
        break;
    }
    *expr = node;
    ArgListTempFree( alist, count );
    PtListFree( ptlist, count );
    return retn;
}

// Determine if the "type" is properly CTORable:
//
// - for a scalar type, this is so if there is no initializer list
// - for a scalar type, this is so if there is one argument which can be
//   converted to the argument type
// - for a class, this is so if there is no initializer list
//      - the ctor SYMBOL might be found
// - for a class, this is so if there exists a CTOR to match the initializer
//   list
//      - the ctor SYMBOL is found
//
// - the initializer list is converted to a parse tree which can be used
//   by EffectCtor
//
// - an error can be detected by examining the returned initializer list to
//   see if it starts with a PT_ERROR node
//
static CNV_RETN analyseTypeCtorDiag( // ANALYSE CONSTRUCTOR FOR A TYPE
    SCOPE scope,                // - start scope for component ctors (NULLable)
    TYPE type,                  // - type for CTOR
    unsigned conversion,        // - type of conversion reqd
    SYMBOL *ctor,               // - ctor to be filled in
    PTREE *initial,             // - initialization arguments (modified)
    FNOV_DIAG *fnov_diag )      // - diagnosis information
{
    PTREE expr;                 // - original args
    TYPE base_type;             // - base type
    CNV_RETN retn;              // - return: CNV_...

    *ctor = NULL;
    expr = *initial;
    base_type = TypedefModifierRemoveOnly( type );
    switch( base_type->id ) {
      default :
        retn = CNV_IMPOSSIBLE;
        break;
      case TYP_ERROR:
        /* we've already diagnosed something somewhere */
        retn = CNV_ERR;
        break;
      case TYP_BITFIELD :
        return analyseTypeCtorDiag( scope
                                  , base_type->of
                                  , conversion
                                  , ctor
                                  , initial
                                  , fnov_diag );
        break;
      case TYP_FUNCTION :
      case TYP_VOID :
        if( NULL == *initial ) {
            retn = CNV_IMPOSSIBLE;
            break;
        }
        // drops thru
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
      case TYP_POINTER :
      case TYP_MEMBER_POINTER :
        retn = single_arg( initial );
        if( retn == CNV_ERR ) break;
        if( *initial == NULL ) break;
        *initial = CastImplicit( *initial, base_type, CNV_INIT, &diagCtor );
        if( PT_ERROR == (*initial)->op ) {
            retn = CNV_ERR;
        } else {
            retn = CNV_OK;
        }
        break;
      case TYP_CLASS :
        retn = analyseCtorClassDiag( base_type, scope, ctor, expr, initial, fnov_diag );
        break;
      case TYP_ARRAY :
        if( expr != NULL ) {
            PTreeErrorExpr( expr, ERR_CANT_INIT_NEW_ARRAY );
            retn = CNV_IMPOSSIBLE;
        } else {
            retn = AnalyseCtorDiag( ArrayBaseType( type ), ctor, initial, NULL );
        }
        break;
    }
    return retn;
}

CNV_RETN AnalyseTypeCtor(       // ANALYSE CONSTRUCTOR FOR A TYPE
    SCOPE scope,                // - start scope for component ctors (NULLable)
    TYPE type,                  // - type for CTOR
    unsigned conversion,        // - type of conversion reqd
    SYMBOL *ctor,               // - ctor to be filled in
    PTREE *initial )            // - initialization arguments (modified)
{
    return analyseTypeCtorDiag( scope
                              , type
                              , conversion
                              , ctor
                              , initial
                              , NULL );
}


CNV_RETN AnalyseCtorDiag(       // ANALYSE CONSTRUCTOR
    TYPE type,                  // - type for CTOR
    SYMBOL *ctor,               // - ctor to be filled in
    PTREE *initial,             // - initialization arguments (modified)
    FNOV_DIAG *fnov_diag ) // Don't know how to implement LMW
{
    TYPE class_type;
    PTREE expr;

    class_type = StructType( type );
    fnov_diag = FnovInitDiag( fnov_diag );
    if( class_type != NULL ) {
        *ctor = NULL;
        expr = *initial;
        if( ! TypeDefedNonAbstract( class_type
                                  , expr
                                  , ERR_CONSTRUCT_AN_ABSTRACT_TYPE
                                  , ERR_CONSTRUCT_AN_UNDEFD_TYPE ) ) {
            return( CNV_ERR );
        }
    }
    return analyseTypeCtorDiag( NULL
                              , type
                              , CNV_CAST
                              , ctor
                              , initial
                              , fnov_diag );
}


static PTREE bareArg(           // STRIP LIST NODE FROM ARGUMENT
    PTREE arg )                 // - CO_LIST node
{
    PTREE retn;                 // - target node

    DbgVerify( NodeIsBinaryOp( arg, CO_LIST ), "bareArg -- impossible" );
    retn = arg->u.subtree[1];
    PTreeFree( arg );
    return retn;
}


PTREE EffectCtor(               // EFFECT A CONSTRUCTION
    PTREE initial,              // - initialization list
    SYMBOL ctor,                // - constructor
    TYPE base_type,             // - type being constructed
    PTREE this_node,            // - NULL, or node for "this"
    TOKEN_LOCN *err_locn,       // - error location
    unsigned control )          // - control mask
{
    PTREE node = NULL;          // - resultant node
    CNV_RETN retn;              // - conversion return

    /*
        if the caller depends on a non-NULL 'this_node' being the final
        value of the expression returned by EffectCtor, see the code in
        AnalyseNew()
    */
    switch( base_type->id ) {
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
      case TYP_BITFIELD :
      case TYP_FUNCTION :
      case TYP_POINTER :
      case TYP_MEMBER_POINTER :
        if( initial == NULL ) {
            initial = NodeZero();
        }
        if( this_node == NULL ) {
            node = NodeConvert( base_type, initial );
        } else {
            if( base_type->id == TYP_MEMBER_POINTER ) {
                node = NodeBinary( CO_INIT, this_node, initial );
                node->type = this_node->type;
                retn = MembPtrAssign( &node );
                /* value of expression is 'this_node' */
                ConversionDiagnose( retn, node, &diagCtor );
            } else {
                PTREE dup;

                dup = NULL;
                if( control & EFFECT_VALUE_THIS ) {
                    dup = NodeDupExpr( &this_node );
                }
                if( base_type->id == TYP_BITFIELD ) {
                    this_node = NodeUnaryCopy( CO_BITFLD_CONVERT, this_node );
                    this_node->type = base_type->of;
                }
                node = NodeBinary( NULL == TypeReference( base_type )
                                   ? CO_INIT : CO_INIT_REF
                                 , this_node
                                 , initial );
                node->type = this_node->type;
                if( dup != NULL ) {
                    node = NodeComma( node, dup );
                }
            }
        }
        break;
      case TYP_CLASS :
        if( ctor == NULL ) {
            node = ClassDefaultCopy( this_node, bareArg( initial ) );
        } else {
            bool check_dtoring;     // - TRUE ==> need to check DTORing
            TYPE this_type;         // - type of this arg
            CALL_OPT opt;           // - type of optimization
            PTREE bare;             // - bare source operand
            PTREE co_dtor;          // - CO_DTOR operand
            if( this_node == NULL ) {
                this_node = NodeTemporary( base_type );
                this_node = PTreeSetLocn( this_node, err_locn );
                check_dtoring = TRUE;
            } else {
                check_dtoring = FALSE;
            }
            if( initial != NULL
             && initial->u.subtree[0] == NULL
             && base_type == ClassTypeForType( initial->type ) ) {
                opt = AnalyseCallOpts( base_type
                                     , initial->u.subtree[1]
                                     , &co_dtor
                                     , &bare );
                if( opt != CALL_OPT_NONE ) {
                    initial = bareArg( initial );
                }
            } else {
                opt = CALL_OPT_NONE;
            }
            if( opt == CALL_OPT_NONE ) {
                arg_list *args;     // - function arguments
                if( control & EFFECT_EXACT ) {
                    this_node->flags |= PTF_MEMORY_EXACT;
                }
                args = SymFuncArgList( ctor );
                this_type = base_type;
                if( args->qualifier != 0 ) {
                    this_type = MakeModifiedType( this_type
                                                , args->qualifier );
                }
                if( NULL == PointerTypeEquivalent( this_node->type ) ) {
                    this_type = MakeReferenceTo( this_type );
                } else {
                    this_type = MakePointerTo( this_type );
                }
                this_node = CastImplicit( this_node
                                        , this_type
                                        , CNV_FUNC_CD_THIS
                                        , &diagCtor );
                if( PT_ERROR == this_node->op ) {
                    NodeFreeDupedExpr( initial );
                    node = PTreeErrorNode( this_node );
                    break;
                }
                node = NodeMakeCall( ctor
                                   , SymFuncReturnType( ctor )
                                   , initial );
                node->flags |= PTF_LVALUE;
                node = PTreeSetLocn( node, err_locn );
                if( AddDefaultArgs( ctor, node ) ) {
                    node = CallArgsArrange( ctor->sym_type
                                          , node
                                          , node->u.subtree[1]
                                          , NodeArg( this_node )
                                          , CallArgumentExactCtor
                                              ( base_type
                                              , ( control & EFFECT_EXACT ) != 0 )
                                          , NULL );
                }
            } else {
                node = CopyOptimize( bare
                                   , initial
                                   , this_node
                                   , co_dtor
                                   , opt );
                control &= ~ EFFECT_CTOR_DECOR;
            }
            if( node->op == PT_ERROR ) break;
            if( check_dtoring ) {
                node = NodeDtorExpr( node, this_node->u.symcg.symbol );
                if( node->op == PT_ERROR ) break;
            }
            if( control & EFFECT_EXACT ) {
                node->flags |= PTF_MEMORY_EXACT;
            }
            if( control & EFFECT_CTOR_DECOR ) {
                if( control & EFFECT_DECOR_COMP ) {
                    node = PtdScopeCall( node, ctor );
                    node = PtdCtoredComponent( node, base_type );
                } else if( control & EFFECT_DECOR_TEMP ) {
                    node = PtdCtoredExprType( node, ctor, base_type );
                } else {
                    node = PtdCtoredScopeType( node, ctor, base_type );
                }
            }
        }
        break;
      case TYP_VOID :
        node = NodeUnary( CO_TRASH_EXPR, node );
        node->type = base_type;
        break;
      default :
        node = this_node;
        break;
    }
    return node;
}
