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


#include <stdlib.h>

#include "plusplus.h"
#include "cgfront.h"
#include "memmgr.h"
#include "fnovload.h"
#include "calldiag.h"
#include "errdefns.h"
#include "rtfuncod.h"
#include "fold.h"
#include "defarg.h"
#include "typesig.h"
#include "fnbody.h"
#include "ctexcept.h"


static PTREE sizeOfUInt( void )
{
    return NodeOffset( SizeTargetSizeT() );
}

static PTREE newCheckForNULL( PTREE value, PTREE t_expr )
{
    PTREE b_expr;
    PTREE f_expr;

    f_expr = value;
    value = NodeDupExpr( &f_expr );
    b_expr = NodeCompareToZero( value );
    return( NodeTestExpr( b_expr, t_expr, f_expr ) );
}

static TYPE figureOutNewType( PTREE *pnumber, TYPE *of_type )
{
    PTREE array_number;
    PTREE extra_number;
    TYPE type;
    TYPE test_type;
    TYPE base_type;
    TYPE new_expr_type;
    target_size_t number;
    target_size_t size;

    /* adjust for cases like "new T" where T is an array typedef */
    array_number = *pnumber;
    type = *of_type;
    if( array_number == NULL ) {
        test_type = ArrayType( type );
        if( test_type != NULL ) {
            array_number = NodeOffset( test_type->u.a.array_size );
            new_expr_type = PointerTypeForArray( type );
            type = test_type->of;
        } else {
            new_expr_type = MakePointerTo( type );
        }
    } else {
        new_expr_type = MakePointerTo( type );
    }
    test_type = ArrayType( type );
    if( test_type != NULL ) {
        /* the base type is an array */
        size = CgTypeSize( type );
        base_type = ArrayBaseType( type );
        number = size / CgTypeSize( base_type );
        extra_number = NodeOffset( number );
        if( array_number != NULL ) {
            array_number = NodeRvalue( array_number );
            array_number = NodeBinary( CO_TIMES, array_number, extra_number );
            array_number->type = extra_number->type;
            array_number = FoldBinary( array_number );
        } else {
            array_number = extra_number;
        }
        type = base_type;
    }
    *of_type = type;
    *pnumber = array_number;
    return( new_expr_type );
}

static SEARCH_RESULT *findNewDelOp( SCOPE search_scope, char *name )
{
    SEARCH_RESULT *result;

    result = NULL;
    if( ! ScopeType( search_scope, SCOPE_FILE ) ) {
        result = ScopeFindMember( search_scope, name );
    }
    // if not found in class scope; search file scope
    if( result == NULL ) {
        result = ScopeFindNaked( FileScope, name );
    }
    return( result );
}

static SYMBOL accessDelete(     // ACCESS DELETE OPERATOR SYMBOL
    unsigned delete_op,         // - CO_DELETE or CO_DELETE_ARRAY
    SCOPE scope,                // - scope to search
    unsigned *num_args,         // - fill in # of arguments
    SEARCH_RESULT **presult )   // - fill in SEARCH_RESULT (if req'd)
{
    SEARCH_RESULT *result;
    SYMBOL del_sym;
    arg_list *args;

    /* there is always an "operator delete" */
    result = findNewDelOp( scope, CppOperatorName( delete_op ) );
    /* remember that delete cannot be overloaded */
    del_sym = result->sym_name->name_syms;
    if( presult == NULL ) {
        ScopeFreeResult( result );
    } else {
        *presult = result;
    }
    args = SymFuncArgList( del_sym );
    *num_args = args->num_args;
    return del_sym;
}


static SYMBOL checkDeleteResult( // CHECK ACCESS FOR DELETE SEARCH_RESULT
    SYMBOL sym,                 // - delete operator
    SEARCH_RESULT* result,      // - search result
    TOKEN_LOCN *locn,           // - error location
    boolean compiling_dtor )    // - TRUE ==> compiling delete inside DTOR
{
    if( result != NULL ) {
        ScopeResultErrLocn( result, locn );
        if( ( ! compiling_dtor ) && ScopeCheckSymbol( result, sym ) ) {
            sym = NULL;
        }
        ScopeFreeResult( result );
    }
    return sym;
}


static SYMBOL checkDeleteAccess( // CHECK ACCESS OF DELETE OPERATOR
    unsigned delete_op,         // - CO_DELETE or CO_DELETE_ARRAY
    SCOPE scope,                // - scope for operator
    unsigned *num_args,         // - number of arguments
    TOKEN_LOCN *locn,           // - error location
    boolean compiling_dtor )    // - TRUE ==> compiling delete inside DTOR
{
    SYMBOL sym;                 // - delete operator
    SEARCH_RESULT *result;      // - search result

    sym = accessDelete( delete_op, scope, num_args, &result );
    sym = checkDeleteResult( sym, result, locn, compiling_dtor );
    return sym;
}


static PTREE setupArrayStorage( // STORE COUNT IN ARRAY_STORAGE,POINT TO ARRAY
    PTREE expr,                 // - ARRAY_STORAGE pointer expression
    TYPE new_expr_type,         // - TYPE of expression
    PTREE array_number )        // - expression for count
{
    expr = NodeConvertFlags( GetBasicType( TYP_UINT ), expr, PTF_LVALUE );
    expr = NodeAssign( expr, array_number );
    expr = NodeConvert( new_expr_type, expr );
    expr = NodeBinary( CO_PLUS, expr, sizeOfUInt() );
    expr->type = new_expr_type;
    return expr;
}


static unsigned checkNewCtor(   // CHECK CTOR'ING OK FOR NEW
    TYPE class_type,            // - NULL or class type
    TYPE base_type,             // - base type of type being new'ed
    SYMBOL* a_ctor,             // - addr[ ctor symbol ]
    PTREE* a_initial,           // - addr[ initialization parse tree ]
    TOKEN_LOCN* err_locn )      // - error location
{
    unsigned ctor_overload;     // - analysis result
    PTREE initial;              // - initialization parse tree
    FNOV_DIAG fnov_diag;

    ctor_overload = AnalyseCtorDiag( base_type
                                   , a_ctor
                                   , a_initial
                                   , &fnov_diag );
    initial = *a_initial;
    switch( ctor_overload ) {
      case CNV_ERR :
      case CNV_OK :
        break;
      case CNV_AMBIGUOUS :
        CallDiagAmbiguous( initial, ERR_CTOR_AMBIGUOUS, &fnov_diag );
        break;
      case CNV_IMPOSSIBLE :
        if( initial == NULL ) {
            if( class_type != NULL ) {
                if( ! TypeNeedsCtor( class_type ) ) {
                    /* no initializer and struct type */
                    /* but struct doesn't need a ctor */
                    ctor_overload = CNV_OK;
                    break;
                }
            } else {
                /* no initializer and non-struct type */
                ctor_overload = CNV_OK;
                break;
            }
            SetErrLoc( err_locn );
            CErr1( ERR_NO_CTOR_FOR_NEW );
        } else {
            ConversionTypesSet( NodeType( initial ), base_type );
            initial = NodeBinary( CO_CTOR, NULL, initial );
            *a_initial = initial;
            initial = PTreeCopySrcLocation( initial, initial->u.subtree[1] );
            CtorDiagNoMatch( initial, ERR_NO_CTOR_FOR_NEW, &fnov_diag );
            ConversionDiagnoseInf();
        }
        break;
      default :
        if( initial == NULL ) {
            SetErrLoc( err_locn );
            CErr1( ERR_NO_CTOR_FOR_NEW );
        } else {
            ConversionTypesSet( NodeType( initial ), base_type );
            PTreeErrorExpr( initial, ERR_NO_CTOR_FOR_NEW );
            ConversionDiagnoseInf();
        }
    }
    FnovFreeDiag( &fnov_diag );
    return ctor_overload;
}

static SCOPE scopeLookup( TYPE type )
{
    SCOPE scope;

    type = StructType( type );
    if( type == NULL ) {
        scope = FileScope;
    } else {
        scope = type->u.c.scope;
    }
    return scope;
}

static SCOPE opNewSearchScope( TYPE new_type, CGOP cgop )
{
    SCOPE scope;

    scope = FileScope;
    if( cgop != CO_NEW_G ) {
        scope = scopeLookup( new_type );
    }
    return( scope );
}

static PTREE buildNewCall(      // BUILD CALL TO NEW OPERATOR
    PTREE node,                 // - arguments
    SCOPE opnew_scope,          // - scope to search for op new
    TYPE new_expr_type,         // - type of new'ed expression
    unsigned count_placement,   // - # arguments
    TOKEN_LOCN* err_locn,       // - error location
    unsigned operator_code )    // - operator code
{
    arg_list *alist;            // - arguments structure
    PTREE *ptlist;              // - parse tree for arguments
    SEARCH_RESULT *result_new;  // - search result for operator new
    SYMBOL sym;                 // - symbol for operator new
    FNOV_RESULT ovret;          // - overload resolution result
    char *name;                 // - op new name
    TEMP_PT_LIST default_list;  // - default PTREE list
    TEMP_ARG_LIST default_args; // - default arg_list
    FNOV_DIAG fnov_diag;        // - diagnosis information

    name = CppOperatorName( operator_code );
    result_new = findNewDelOp( opnew_scope, name );
    ScopeResultErrLocn( result_new, err_locn );
    /* will always find an "operator new" or "operator new[] */
    alist = ArgListTempAlloc( &default_args, count_placement );
    ptlist = PtListAlloc( default_list, count_placement );
    NodeBuildArgList( alist, ptlist, node, count_placement );
    /* find the appropriate "operator new" or operator new[] */
    sym = result_new->sym_name->name_syms;
    ovret = FuncOverloadedDiag( &sym
                           , result_new
                           , sym
                           , alist
                           , ptlist
                           , &fnov_diag ) ;
    if( ovret == FNOV_NONAMBIGUOUS ) {
        if( ScopeCheckSymbol( result_new, sym ) ) {
            PTreeErrorNode( node );
        } else {
            node = NodeMakeCall( sym, new_expr_type, node );
            node = NodeConvertCallArgList( node
                                         , alist->num_args
                                         , sym->sym_type
                                         , &node->u.subtree[1] );
            if( node->op != PT_ERROR ) {
                AddDefaultArgs( sym, node );
            }
        }
    } else {
        sym = result_new->sym_name->name_syms;
        node = NodeMakeCall( sym, new_expr_type, node );
        switch( ovret ) {
          case FNOV_AMBIGUOUS:
            CallDiagAmbiguous( node, ERR_NEW_OVERLOAD_AMBIGUOUS, &fnov_diag );
            break;
          case FNOV_NO_MATCH:
            CallDiagNoMatch( node
                           , ERR_FUNCTION_NO_MATCH
                           , ERR_NEW_OVERLOAD_FAILURE
                           , NULL
                           , sym
                           , &fnov_diag
                           );
            break;
          DbgDefault( "buildNewCall: unexpected return from FuncOverloaded" );
        }
    }
    FnovFreeDiag( &fnov_diag );
    ScopeFreeResult( result_new );
    ArgListTempFree( alist, count_placement );
    PtListFree( ptlist, count_placement );
    return node;
}


PTREE AnalyseNew(               // ANALYSE A "NEW" OPERATOR (WITH OVERLOADING)
    PTREE expr,                 // - "new" expression
    TYPE type )                 // - type of expression
{
    unsigned count_placement;   // - # placement arg.s
    unsigned ctor_overload;     // - result of ctor overload
    unsigned num_args;          // - number of args for delete operator
    PTREE new_parms1;           // - CO_NEW_PARMS1 node
    PTREE new_parms2;           // - CO_NEW_PARMS2 node
    PTREE elem_size;            // - constant for size of an element
    PTREE node;                 // - new node
    PTREE dup;                  // - duplicated node
    PTREE placement;            // - placement arguments
    PTREE initial;              // - initialization arguments
    PTREE array_number;         // - number of array arguments
    TYPE base_type;             // - base type of element
    TYPE class_type;            // - != NULL if base_type is a class
    TYPE new_expr_type;         // - final type of the new-expr
    TYPE offset_type;           // - type for offset computations
    SYMBOL sym_ctor;            // - constructor function
    SYMBOL op_dlt;              // - delete operator used to free new'ed store
    SYMBOL addr_op_del;         // - addressible operator delete
    SEARCH_RESULT *result_dlt;  // - search result for operator delete
    CGOP cgop;                  // - type of new expression
    TOKEN_LOCN err_locn;        // - error location
    struct {
      unsigned needs_dtor : 1;  // - class says DTOR is req'd (DTOR may be empty)
      unsigned really_dtorable:1;//- class DTOR really req'd
      unsigned needs_count:1;   // - array new needs count in allocation
      unsigned free_array_no :1;// - "array_number" needs to be freed
    } flag;


    /*
                        CO_NEW
                       /      \
        PT_TYPE(type)           CO_NEW_PARMS1
                               /             \
                placement ->  ,               CO_NEW_PARMS2
                             / \             /             \
                            ,   arg n      nelem            , <- new-init
                           / \          (may be NULL)      / \
                          ,   ...                         ,   arg n
                           \                             / \
                            arg 1                       ,   ...
                                                         \
                                                          arg 1
    */
    if( ! TypeDefined( type ) ) {
        PTreeErrorExpr( expr, ERR_CANT_NEW_UNDEFD );
        return( expr );
    }
    PTreeExtractLocn( expr, &err_locn );
    /* extract the components of the new-expression and free the CO_NEW* parts */
    new_parms1 = expr->u.subtree[1];
    placement = NodeReverseArgs( &count_placement, new_parms1->u.subtree[0] );
    new_parms1->u.subtree[0] = NULL;
    new_parms2 = new_parms1->u.subtree[1];
    initial = new_parms2->u.subtree[1];
    new_parms2->u.subtree[1] = NULL;
    array_number = new_parms2->u.subtree[0];
    new_parms2->u.subtree[0] = NULL;
    cgop = expr->cgop;
    NodeFreeDupedExpr( expr );
    /* 'type', 'placement', 'array_number', and 'initial' are set now */
    new_expr_type = figureOutNewType( &array_number, &type );
    elem_size = NodeOffset( CgTypeSize( type ) );
    offset_type = elem_size->type;
    base_type = ArrayBaseType( type );
    class_type = StructType( type );
    //
    // verify CTORING is ok
    //
    ctor_overload = checkNewCtor( class_type
                                , base_type
                                , &sym_ctor
                                , &initial
                                , &err_locn );
    if( ctor_overload != CNV_OK ) {
        NodeFreeDupedExpr( placement );
        NodeFreeDupedExpr( elem_size );
        NodeFreeDupedExpr( array_number );
        NodeFreeDupedExpr( initial );
        return( PTreeErrorNode( NULL ) );
    }
    //
    // optimize away CTORing if possible
    //
    if( sym_ctor != NULL ) {
        if( initial == NULL && ! TypeReallyDefCtorable( class_type ) ) {
            sym_ctor = NULL;
        }
    }
    flag.needs_count = FALSE;
    flag.needs_dtor = FALSE;
    flag.really_dtorable = FALSE;
    flag.free_array_no = FALSE;
    if( TypeRequiresDtoring( class_type ) ) {
        flag.needs_dtor = TRUE;
        // the structure of the allocation can't be influenced by 'really_dtorable'
        // since the alloc and delete may not agree on 'really_dtorable'
        // the flag is currently not used in the new but could be used to
        // influence the choice of run-time routines for array news in the
        // face of exceptions
        //if( TypeReallyDtorable( class_type ) ) {
            //flag.really_dtorable = TRUE;
        //}
    }
    op_dlt = accessDelete( array_number == NULL ? CO_DELETE : CO_DELETE_ARRAY
                         , scopeLookup( class_type )
                         , &num_args
                         , &result_dlt );
    addr_op_del = ClassFunMakeAddressable( op_dlt );
    //
    // compute size
    //
    if( array_number != NULL ) {
        if( flag.needs_dtor || num_args == 2 ) {
            flag.needs_count = TRUE;
        }
        array_number = NodeRvalue( array_number );
        if( IntegralType( array_number->type ) == NULL ) {
            PTreeErrorExpr( array_number, ERR_NEW_ARRAY_EXPRESSION );
            ScopeFreeResult( result_dlt );
            NodeFreeDupedExpr( placement );
            NodeFreeDupedExpr( elem_size );
            NodeFreeDupedExpr( array_number );
            NodeFreeDupedExpr( initial );
            return( PTreeErrorNode( NULL ) );
        }
        if( initial != NULL ) {
            PTreeErrorExpr( initial, ERR_CANT_INIT_NEW_ARRAY );
            ScopeFreeResult( result_dlt );
            NodeFreeDupedExpr( placement );
            NodeFreeDupedExpr( elem_size );
            NodeFreeDupedExpr( array_number );
            NodeFreeDupedExpr( initial );
            return( PTreeErrorNode( NULL ) );
        }
        /* calculate "nelem * sizeof( elem )" */
        dup = array_number;
        if( sym_ctor != NULL || flag.needs_count ) {
            array_number = NodeDupExpr( &dup );
            flag.free_array_no = TRUE;
        }
        node = NodeBinary( CO_TIMES, dup, elem_size );
        node->type = offset_type;
        node = FoldBinary( node );
        if( flag.needs_count ) {
            node = NodeBinary( CO_PLUS, node, sizeOfUInt() );
            node->type = offset_type;
            node = FoldBinary( node );
        }
    } else {
        node = elem_size;
    }
    //
    // build call to appropriate operator new
    //
    node->locn = err_locn;
    node = NodeArgument( placement, node );
    ++count_placement;
    node = buildNewCall( node
                       , opNewSearchScope( class_type, cgop )
                       , new_expr_type
                       , count_placement
                       , &err_locn
                       , array_number == NULL ? CO_NEW : CO_NEW_ARRAY );
    if( node->op == PT_ERROR ) {
        ScopeFreeResult( result_dlt );
        NodeFreeDupedExpr( node );
        if( flag.free_array_no ) {
            NodeFreeDupedExpr( array_number );
        }
        NodeFreeDupedExpr( initial );
        return( PTreeErrorNode( NULL ) );
    }
    //
    // CTOR the new'ed memory and set up ARRAY_STORAGE, if required
    //
    if( array_number != NULL ) {
        if( sym_ctor != NULL ) {
            unsigned rt_code;       // - run-time function code
            PTREE args;             // - run-time call arguments
            TYPE_SIG *sig;          // - type signature for class
            boolean errors;         // - TRUE ==> errors
            TYPE_SIG_ACCESS acc;    // - rtn.s to be accessed
            if( flag.needs_dtor ) {
                acc = TSA_DTOR | TSA_DEFAULT_CTOR;
            } else {
                acc = TSA_DEFAULT_CTOR;
            }
            sig = TypeSigFind( acc, class_type, &err_locn, &errors );
            if( errors ) {
                ScopeFreeResult( result_dlt );
                NodeFreeDupedExpr( node );
                if( flag.free_array_no ) {
                    NodeFreeDupedExpr( array_number );
                }
                NodeFreeDupedExpr( initial );
                return( PTreeErrorNode( NULL ) );
            }
            TypeSigReferenced( sig );
            op_dlt = checkDeleteResult( op_dlt
                                      , result_dlt
                                      , &err_locn
                                      , FALSE );
            if( op_dlt == NULL ) {
                NodeFreeDupedExpr( node );
                if( flag.free_array_no ) {
                    NodeFreeDupedExpr( array_number );
                }
                NodeFreeDupedExpr( initial );
                return( PTreeErrorNode( NULL ) );
            }
            if( num_args == 1 ) {
                if( ScopeId( SymScope( op_dlt ) ) == SCOPE_FILE
                 && addr_op_del == op_dlt ) {
                    if( flag.needs_count ) {
                        rt_code = RTF_CTAS_GS;
                    } else {
                        rt_code = RTF_CTAS_GM;
                    }
                    args = NULL;
                } else {
                    if( flag.needs_count ) {
                        rt_code = RTF_CTAS_1S;
                    } else {
                        rt_code = RTF_CTAS_1M;
                    }
                    args = NodeArg( MakeNodeSymbol( addr_op_del ) );
                    args = PtdScopeCall( args, op_dlt );
                }
            } else {
                DbgVerify( flag.needs_count
                         , "AnalyseNew -- flags mismatch" );
                rt_code = RTF_CTAS_2S;
                args = NodeArg( MakeNodeSymbol( addr_op_del ) );
                args = PtdScopeCall( args, op_dlt );
            }
            args = NodeArgument( args, NodeTypeSig( sig ) );
            args = NodeArgument( args, array_number );
            args = NodeArgument( args, node );
            node = RunTimeCall( args, new_expr_type, rt_code );
            if( placement != NULL ) {
                dup = node;
                node = NodeDupExpr( &dup );
                node = newCheckForNULL( dup, node );
            }
        } else {
            ScopeFreeResult( result_dlt );
            /* no constructor req'd */
            if( flag.needs_count ) {
                dup = node;
                node = NodeDupExpr( &dup );
                expr = setupArrayStorage( dup, new_expr_type, array_number );
                node = newCheckForNULL( node, expr );
            }
        }
    } else {
        ScopeFreeResult( result_dlt );
        if( initial != NULL || sym_ctor != NULL ) {
            dup = node;
            node = NodeDupExpr( &dup );
#if 0
            dup->type = type;
            dup->flags |= PTF_LVALUE;
#else
            dup = NodeConvertFlags( type, dup, PTF_LVALUE );
#endif
            if( sym_ctor != NULL && placement == NULL ) {
                dup = PtdNewAlloc( dup );
            }
            expr = EffectCtor( initial
                             , sym_ctor
                             , base_type
                             , dup
                             , &err_locn
                             , EFFECT_EXACT | EFFECT_VALUE_THIS );
            if( expr->op == PT_ERROR ) {
                NodeFreeDupedExpr( node );
                node = expr;
            } else {
                if( sym_ctor != NULL && placement == NULL ) {
                    FunctionHasRegistration();
                    expr = PtdNewCtor( expr, class_type );
                }
                expr = NodeConvert( new_expr_type, expr );
                node = newCheckForNULL( node, expr );
            }
        }
    }
    PTreeSetLocn( node, &err_locn );
    return( node );
}


static PTREE deleteCheckForNULL( PTREE value, PTREE t_expr )
{
    PTREE b_expr;
    PTREE f_expr;

    b_expr = NodeCompareToZero( value );
    f_expr = NodeOffset( 0 );
    return( NodeTestExpr( b_expr, t_expr, f_expr ) );
}


static PTREE setDeleteType( PTREE expr, TOKEN_LOCN *locn )
{
    expr = NodeConvertFlags( GetBasicType( TYP_VOID )
                           , expr
                           , PTF_MEANINGFUL | PTF_SIDE_EFF );
    PTreeSetLocn( expr, locn );
    return( expr );
}


PTREE AnalyseDelete(            // ANALYSE DELETE OPERATOR
    PTREE expr,                 // - delete expression
    boolean in_dtor )           // - TRUE ==> call from inside DTOR
{
    CGOP oper;                  // - operator (original)
    CGOP del_op;                // - delete operator class
    TYPE type;                  // - type of expression
    TYPE ptr_type;              // - pointer part of type
    TYPE pted;                  // - type pointed at
    TYPE cltype;                // - class type pointed at
    TYPE offset_type;           // - type of size to pass to op del()
    PTREE data;                 // - data to be deleted
    PTREE dup;                  // - duplicate of pointer value
    PTREE arg;                  // - temporary arg
    PTREE args;                 // - args for delete
    SCOPE class_scope;          // - scope of class type
    SCOPE opdel_scope;          // - scope to search op del for
    SYMBOL sym;                 // - symbol for delete routine
    CLASSINFO *info;            // - info part of delete type
    TOKEN_LOCN err_locn;        // - error location
    unsigned dtor_code;         // - code for DTOR routine
    unsigned num_args;          // - # of args for "op delete"
    struct {
      unsigned test_null:1;     // - TRUE ==> test for 0 address
      unsigned array_delete:1;  // - TRUE ==> delete[]
      unsigned inside_dtor:1;   // - TRUE ==> delete gening in DTOR
      unsigned num_in_alloc:1;  // - TRUE ==> number elements is in allocation
      unsigned really_dtorable:1;//- TRUE ==> non-trivial destructor req'd
      unsigned adjust_for_num:1;// - TRUE ==> subtract sizeof(int) from ptr
    } flag;

    flag.inside_dtor = in_dtor;
    oper = expr->cgop;
    flag.array_delete = ( oper == CO_DELETE_ARRAY || oper == CO_DELETE_G_ARRAY );
    data = expr->u.subtree[0];
    PTreeExtractLocn( expr, &err_locn );
    PTreeFree( expr );
    data = NodeRvalue( data );
    type = data->type;
    ptr_type = PointerType( type );
    if( ptr_type == NULL ) {
        PTreeErrorExpr( data, ERR_DLT_NOT_PTR_TO_DATA );
        return( data );
    }
    pted = ptr_type->of;
    if( TypeIsConst( pted ) ) {
        // 5.3.5 para 4 note
        PTreeErrorExpr( data, WARN_DLT_PTR_TO_CONST );
    }
    if( TypeTruncByMemModel( pted ) ) {
        PTreeErrorExpr( data, ERR_DLT_OBJ_MEM_MODEL  );
        return( data );
    }
    del_op = flag.array_delete ? CO_DELETE_ARRAY : CO_DELETE;
    flag.num_in_alloc = FALSE;
    pted = ArrayBaseType( pted );
    cltype = StructType( pted );
    if( cltype == NULL ) {
        /* simple type */
        if( FunctionDeclarationType( pted ) != NULL ) {
            PTreeErrorExpr( data, ERR_DLT_PTR_TO_FUNCTION );
            return( data );
        }
        opdel_scope = FileScope;
    } else {
        if( ! TypeDefined( cltype ) ) {
            // 5.3.5 para 5 can delete undef'd class ptr
            PTreeErrorExpr( data, WARN_CANT_DEL_UNDEFD );
        }
        info = cltype->u.c.info;
        class_scope = TypeScope( cltype );
        if( info->needs_dtor && ! flag.inside_dtor ) {
            /* class type needs a destructor */
            if( !info->needs_vdtor && ScopeHasVirtualFunctions( class_scope ) ) {
                /* possible problem: class has vfns but a non-virtual dtor */
                PTreeWarnExpr( data, WARN_VIRTUAL_DTOR_DELETE );
            }
        }
        opdel_scope = class_scope;
        switch( oper ) {
        case CO_DELETE_G_ARRAY:
            // find out what the op new side assumed
            accessDelete( del_op, opdel_scope, &num_args, NULL );
            if( num_args == 2 ) {
                flag.num_in_alloc = TRUE;
            }
            /* fall through */
        case CO_DELETE_G:
            opdel_scope = FileScope;
            break;
        }
    }
    sym = checkDeleteAccess( del_op, opdel_scope, &num_args, &err_locn, flag.inside_dtor );
    if( sym == NULL ) {
        PTreeErrorNode( data );
        return( data );
    }
    flag.test_null = FALSE;
    if( cltype == NULL ) {
        // delete a simple type; call op del() or op del []()
        args = NodeArgument( NULL, data );
        expr = NodeMakeCall( sym, GetBasicType( TYP_VOID ), args );
    } else {
        if( ! flag.inside_dtor && info->needs_vdtor ) {
            // call virtual dtor
            if( flag.array_delete ) {
                dtor_code = DTOR_DELETE_VECTOR;
            } else {
                dtor_code = DTOR_DELETE_THIS;
            }
            flag.test_null = TRUE;
            dup = data;
            data = NodeDupExpr( &dup );
            expr = AnalyseDtorCall( cltype, data, dtor_code );
        } else {
            target_size_t elem_size;    // - size of an element

            // cases:
            //  1 - simple delete
            //  2 - simple delete (needs dtor)
            //  3 - array delete
            //  4 - array delete (needs dtor)
            //  5 - simple delete (inside dtor)
            //  6 - array delete (inside dtor)

            elem_size = CgMemorySize( pted );
            if( ! ( flag.inside_dtor && oper == CO_DELETE ) && info->needs_dtor ) {
                // class needs a dtor
                // handle cases: 2, 4, 6
                flag.really_dtorable = TypeReallyDtorable( cltype );
                if( !flag.inside_dtor && !flag.really_dtorable ) {
                    // no dtor to call but check access anyway
                    // call to delete is done later in this code
                    // optimizes cases: 2, 4
                    ClassAccessDtor( cltype );
                    if( flag.array_delete ) {
                        flag.test_null = TRUE;
                        dup = data;
                        data = NodeDupExpr( &dup );
                        expr = NodeBinary( CO_MINUS, data, sizeOfUInt() );
                        expr->type = expr->u.subtree[0]->type;
                    } else {
                        expr = data;
                    }
                } else if( flag.array_delete ) {
                    // handle cases: 4, 6
                    boolean errors; // - errors accessing DTOR
                    TYPE_SIG* sig;  // - class type signature
                    sig = TypeSigFind( TSA_DTOR, cltype, &err_locn, &errors );
                    if( !errors ) {
                        if( num_args == 2 ) {
                            data = PtdDltDtorSize( data, elem_size );
                        }
                        data = PtdDltDtorArr( data, sym );
                        TypeSigReferenced( sig );
                        arg = NodeTypeSig( sig );
                        args = NodeArguments( arg, data, NULL );
                        expr = RunTimeCall( args, type, RTF_DTOR_AR_STORE );
                        expr = PtdDltDtorEnd( expr );
                    } else {
                        expr = data;
                    }
                } else {
                    // handle case: 2
                    flag.test_null = TRUE;
                    dup = data;
                    data = NodeDupExpr( &dup );
                    if( num_args == 2 ) {
                        data = PtdDltDtorSize( data, elem_size );
                    }
                    data = PtdDltDtorElm( data, sym );
                    dtor_code = DTOR_NULL;
                    expr = AnalyseDtorCall( cltype, data, dtor_code );
                    expr = PtdDltDtorEnd( expr );
                }
            } else {
                // no dtor call required
                // handle cases: 1, 3, 5
                if( flag.array_delete ) {
                    flag.adjust_for_num = FALSE;
                    if( num_args == 2 ) {
                        flag.adjust_for_num = TRUE;
                    } else if( flag.inside_dtor && oper == CO_DELETE_ARRAY ) {
                        flag.adjust_for_num = TRUE;
                    } else if( flag.num_in_alloc ) {
                        flag.adjust_for_num = TRUE;
                    }
                    if( flag.adjust_for_num ) {
                        flag.test_null = TRUE;
                        dup = data;
                        data = NodeDupExpr( &dup );
                        expr = NodeBinary( CO_MINUS, data, sizeOfUInt() );
                        expr->type = expr->u.subtree[0]->type;
                    } else {
                        expr = data;
                    }
                } else {
                    expr = data;
                }
            }
            if( num_args == 2 ) {
                args = NodeOffset( elem_size );
                if( flag.array_delete ) {
                    PTREE dup2;

                    // 'expr' must already be adjusted down by sizeof(unsigned)
                    dup2 = expr;
                    expr = NodeDupExpr( &dup2 );
                    expr = NodeConvert( MakeReferenceTo( GetBasicType( TYP_UINT ) )
                                      , expr );
                    args = NodeBinary( CO_TIMES, NodeRvalue( expr ), args );
                    offset_type = args->u.subtree[1]->type;
                    args->type = offset_type;
                    args = NodeBinary( CO_PLUS, args, sizeOfUInt() );
                    args->type = offset_type;
                    expr = dup2;
                    if( ! flag.test_null ) {
                        flag.test_null = TRUE;
                        dup = expr;
                        expr = NodeDupExpr( &dup );
                    }
                }
            } else {
                args = NULL;
            }
            if( args != NULL ) {
                args = NodeArg( args );
            }
            args = NodeArgument( args, expr );
            expr = NodeMakeCall( sym, GetBasicType( TYP_VOID ), args );
        }
    }
    if( flag.test_null ) {
        // 'dup' must be setup with a duplicate of the ptr expr
        expr = deleteCheckForNULL( dup, expr );
    }
    return setDeleteType( expr, &err_locn );
}
