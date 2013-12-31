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
* Description:  Expression support for classes.
*
****************************************************************************/


/*
WATCOM C++ uses two constructors to accomplish the task of turning raw memory
into a valid object.  They are called the "exact" constructor and the
"component" constructor.  The "exact" constructor is the constructor used
by C++ programmers.  The "component" constructor is used internally by the
C++ compiler to build up sub-objects for the final "exact" constructor.
The current implementation (V1.0) uses an extra bit flag parm to change its
behaviour from "exact" to "component".

Here are the tasks assigned to each type of constructor:

Exact constructor
=================

(EC1)   initialize all virtual base pointers
(EC2)   call component constructor for all virtual bases (may use ctor-init)
(EC3)   execute my component constructor

Component constructor
=====================
(CC1)   call direct non-virtual component constructors (may use ctor-init)
(CC2)   construct members (may use ctor-init)
(CC3)   - calculate ctor-disp for all known virtual bases
        - initialize virtual function pointers
        - execute user-specified constructor code

A constructor will return 'this' as its return value.  The equivalent
sequence for destructors is: (notice everything is reversed!)

Exact destructor
================

(ED1)   execute my component destructor
(ED2)   call component destructor for all virtual bases (in reverse order)
(ED3)   initialize all virtual base pointers (IGNORE for destructor)

Component destructor
====================
(CD1)   - calculate ctor-disp for all known virtual bases
        - initialize virtual function pointers
        - execute user-specified destructor code
(CD2)   destruct members (in reverse order)
(CD3)   call direct non-virtual component destructors (in reverse order)

A destructor will return 'this' as its return value.

*/

#include "plusplus.h"
#include "cgfront.h"
#include "cgsegid.h"
#include "codegen.h"
#include "name.h"
#include "fnbody.h"
#include "fnovload.h"
#include "ring.h"
#include "vstk.h"
#include "rtfuns.h"
#include "class.h"
#include "defarg.h"
#include "ctexcept.h"
#include "context.h"
#include "cdopt.h"
#include "vfun.h"
#include "rtngen.h"
#include "objmodel.h"
#include "datadtor.h"
#include "rtti.h"
#include "calldiag.h"

typedef enum {
    CI_EXACT    = 0x01,         // - constructing an exact copy of a class
    CI_MEM_INIT = 0x02,         // - init expression is not analysed yet
    CI_INLINED  = 0x04,         // - init'ing inlined componentry
    CI_VIRTUAL  = 0x08,         // - init'ing virtual base
    CI_NULL     = 0x00
} ctor_init;

typedef struct ctor_prologue ctor_prologue;
struct ctor_prologue {
    PTREE       member_init;    // - list of data member initializations
    PTREE       base_init;      // - list of base class initializations
    SCOPE       scope;          // - 'C' in C( C & x )
    CDOPT_ITER* optiter;        // - next input ctor for CD optimization
    PTREE       comp_expr;      // - initialization for component
    TYPE        comp_type;      // - type for component
    TYPE        this_type;      // - type of 'this' cache
    CGFILE_INS  sect_locn;      // - location of IC_SECT_BEG
    target_offset_t orig_offset;// - original offset of base/member
    target_offset_t comp_offset;// - offset for component
    ctor_init   comp_options;   // - options
    unsigned    cdopt     :1;   // - CD optimization active
    unsigned    dtor_reqd :1;   // - TRUE ==> a DTOR was required (in section)
    unsigned    gen_copy  :1;   // - a compiler generated copy ctor
    unsigned    excepts   :1;   // - TRUE ==> exceptions enabled
    unsigned    obj_dtor  :1;   // - TRUE ==> object requires DTOR
    unsigned    have_cdopt:1;   // - TRUE ==> have CDOPT component
    unsigned    done_cdopt:1;   // - TRUE ==> processed all CDOPT components
};

typedef struct assign_prologue assign_prologue;
struct assign_prologue {
    SCOPE       start;
    type_flag   qualifier;
};


static type_flag cvFlags(       // GET CV FLAGS FOR AN OPERAND
    PTREE expr )                // - class (reference) operand
{
    type_flag flags;            // - flags for expression

    TypeModFlags( TypeReferenced( expr->type ), &flags );
    return flags & TF1_CV_MASK;
}

static void returnThis(         // RETURN "this" value
    SYMBOL rtn )                // - routine being compiled
{
    PTREE expr;

    expr = PTreeThis();
    if( ! SymIsDtor( rtn ) ) {
        expr = PTreeUnary( CO_INDIRECT, expr );
    }
    expr = AnalyseReturnExpr( rtn , expr );
    IcEmitExpr( expr );
    CgFrontReturnSymbol( SymFunctionReturn() );
}

static PTREE addOffset( PTREE node, target_offset_t offset, TYPE type )
{
    node = NodeBinary( CO_DOT, node, NodeOffset( offset ) );
    node->type = type;
    node->flags |= PTF_LVALUE;
    return( node );
}

static PTREE addOffsetToThis( target_offset_t offset, TYPE type )
{
    return( addOffset( NodeThis(), offset, type ) );
}

static PTREE nodeFirstParm(     // GET FIRST PARM AS A NODE
    void )
{
    return NodeFetchReference( MakeNodeSymbol( ScopeFuncParm( 0 ) ) );
}

static PTREE addOffsetToFirstParm( target_offset_t offset, TYPE type )
{
    PTREE expr;

    expr = nodeFirstParm();
    type = TypeMergeForMember( expr->type, type );
    return addOffset( expr, offset, type );
}

static PTREE addVBOffsetToExpr( // ADD VB-OFFSET TO EXPRESSION
    SCOPE scope,                // - scope for vbase
    BASE_CLASS *base,           // - base class to be accessed
    TYPE type,                  // - type of symbol
    PTREE expr )                // - expression
{
    CLASSINFO *info;
    TYPE class_type;

    class_type = ScopeClass( scope );
    info = class_type->u.c.info;
    type = TypeMergeForMember( expr->type, type );
    expr = NodeConvertVirtualPtr( expr, type, info->vb_offset, base->vb_index );
    expr->flags |= PTF_LVALUE;
    return expr;
}

static PTREE addVBOffsetToFirstParm(
    SCOPE scope,
    BASE_CLASS *base,
    TYPE type )
{
    return addVBOffsetToExpr( scope, base, type, nodeFirstParm() );
}

static PTREE addVBOffsetToThis(
    SCOPE scope,
    BASE_CLASS *base,
    TYPE type )
{
    return addVBOffsetToExpr( scope, base, type, NodeThis() );
}

static PTREE accessSourceBase( SCOPE scope, BASE_CLASS *base )
{
    TYPE type;
    PTREE src;

    type = base->type;
    if( _IsVirtualBase( base ) ) {
        src = addVBOffsetToFirstParm( scope, base, type );
    } else {
        src = addOffsetToFirstParm( base->delta, type );
    }
    return src;
}

static PTREE accessThisBase( SCOPE scope, BASE_CLASS *base )
{
    TYPE type;
    PTREE src;

    type = base->type;
    if( _IsVirtualBase( base ) ) {
        src = addVBOffsetToThis( scope, base, type );
    } else {
        src = addOffsetToThis( base->delta, type );
    }
    return src;
}

static PTREE getRefSymFromFirstParm( SYMBOL sym, target_offset_t cl_offset )
{
    TYPE sym_type;
    PTREE first_parm_ref;

    sym_type = sym->sym_type;
    first_parm_ref = addOffsetToFirstParm( sym->u.member_offset + cl_offset, sym_type );
    sym_type = TypedefModifierRemove( sym_type );
    if( sym_type->id == TYP_BITFIELD ) {
        first_parm_ref = NodeUnaryCopy( CO_BITFLD_CONVERT, first_parm_ref );
        first_parm_ref->type = sym_type->of;
    }
    return( first_parm_ref );
}


static PTREE setThisFromOffset( // SET "THIS" BACK BY AN OFFSET
    PTREE expr,                 // - DTOR, CTOR call
    TYPE type,                  // - type of component accessed by 'this'
    TYPE this_type,             // - type of 'this'
    target_offset_t offset )    // - the offset
{
    PTREE offset_node;

    if( expr == NULL ) {
        /* no expr to adjust */
        return( expr );
    }
    if( expr->op == PT_ERROR ) {
        return expr;
    }
    if( expr->type->id == TYP_VOID ) {
        /* no return value to adjust */
        return( expr );
    }
    if( TypedefModifierRemove( type )->id == TYP_BITFIELD ) {
        /* bit-fields do not have l-values that can be adjusted */
        return( expr );
    }
    offset_node = NodeOffset( offset );
    offset_node->cgop = CO_IGNORE;
    expr = NodeBinary( CO_RESET_THIS, expr, offset_node );
    expr->type = this_type;
    return( expr );
}

static void initClassFunction(  // START GENERATION OF CLASS FUNCTION
    SYMBOL fun,                 // - symbol for function
    FUNCTION_DATA *fn_data,     // - function data storage
    error_state_t *check,       // - error state at beginning of func
    boolean fun_is_copy )       // - function is a copy ctor or op=
{
    TYPE fn_type;               // - type for function
    arg_list *alist;            // - prototype for function
    SYMBOL arg;                 // - function argument
    SCOPE sym_scope;

    CtxFunction( fun );
    CErrCheckpoint( check );
    sym_scope = SymScope( fun );
    ScopeAdjustUsing( GetCurrScope(), sym_scope );
    SetCurrScope( sym_scope );
    ScopeBeginFunction( fun );
    fn_type = FunctionDeclarationType( fun->sym_type );
    if( fun_is_copy ) {
        if( ! TypeHasNumArgs( fn_type, 0 ) ) {
            /* copy constructor or op= */
            alist = fn_type->u.f.args;
            arg = AllocTypedSymbol( alist->type_list[0] );
            arg->id = SC_AUTO;
            arg->flag |= SF_REFERENCED;
            // this special parm name is used by CtorPrologue to identify the
            // special case of compiler-generated copy constructors
            // (this is the only case in a constructor where copy ctors are used
            //  for all of the base classes instead of normal constructors)
            arg = ScopeInsert( GetCurrScope(), arg, CppSpecialName( SPECIAL_COPY_ARG ) );
        }
    }
    FunctionBodyStartup( fun, fn_data, FUNC_NULL );
}

static void finiClassFunction(  // COMPLETE GENERATION OF CLASS FUNCTION
    SYMBOL fun,                 // - symbol for function
    FUNCTION_DATA *fn_data,     // - function data storage
    error_state_t *check )      // - error state at beginning of func
{
    FunctionBodyShutdown( fun, fn_data );
    ScopeEnd( SCOPE_FUNCTION );
    if( CErrOccurred( check ) ) {
        CErr2p( ERR_DURING_GEN, fun );
    }
}

static SEARCH_RESULT *classResult( // GET SEARCH RESULT FOR A TYPE
    TYPE type,                  // - the type
    SYMBOL *sym_ok,             // - addr( good symbol )
    NAME name,                  // - name to search for
    SCOPE access )              // - derived access scope
{
    SCOPE scope;                // - scope to search in
    SEARCH_RESULT *result;      // - result of lookup
    SEARCH_RESULT *retn;        // - NULL ==> no symbol
    SYMBOL sym;                 // - current symbol
    arg_list *alist;            // - argument list for function

    scope = TypeScope( type );
    result = ScopeContainsMember( scope, name );
    retn = NULL;
    *sym_ok = NULL;
    if( result != NULL ) {
        RingIterBeg( result->sym_name->name_syms, sym ) {
            alist = SymFuncArgList( sym );
            if( ( alist != NULL )
              &&( TypeHasNumArgs( sym->sym_type, 1 ) )
              &&( type == ClassTypeForType( alist->type_list[0] ) ) ) {
                *sym_ok = sym;
                if( access != NULL ) {
                    ScopeFreeResult( result );
                    retn = ScopeFindBaseMember( scope, name );
                } else {
                    retn = result;
                }
                break;
            }
        } RingIterEnd( sym )
        if( retn == NULL ) {
            ScopeFreeResult( result );
        }
    }
    return( retn );
}

static void memberWalkWithData( // WALK MEMBERS IN CTOR ORDER
    SCOPE scope,                // - class scope
    void (*memb_rtn)(           // - symbol walk routine
        SYMBOL,                 // -- symbol being visited
        void * ),               // -- traversal data
    void *data )                // - traversal data
{
    SYMBOL curr;
    SYMBOL stop;

    curr = NULL;
    stop = ScopeOrderedStart( scope );
    for(;;) {
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) break;
        (*memb_rtn)( curr, data );
    }
}


static SEARCH_RESULT *classAssignResult( // GET SEARCH_RESULT FOR OPERATOR=()
    TYPE type,                  // - type for class
    SYMBOL *assign,             // - addr[ SYMBOL for operator=() ]
    SCOPE access )              // - derived access scope
;


static SYMBOL classArrayRtn(    // OBTAIN ARRAY ROUTINE FOR ARRAY TYPE
    TYPE cltype,                // - class type
    unsigned ambig_msg,         // - ambiguity message
    type_flag this_qualifier,   // - T cv-qual *this; (cv-qual part)
    SEARCH_RESULT *result )     // - search result for function called
{
    arg_list alist;             // - argument list for overloading
    SYMBOL rtn;                 // - NULL or good routine

    InitArgList( &alist );
    alist.num_args = 1;
    alist.qualifier = this_qualifier;
    alist.type_list[0] = cltype;
    rtn = result->sym_name->name_syms;
    switch( FuncOverloaded( &rtn, result, rtn, &alist, NULL ) ) {
      case FNOV_AMBIGUOUS :
        CErr2p( ambig_msg, rtn );
        rtn = NULL;
        break;
      case FNOV_NONAMBIGUOUS :
        if( ScopeCheckSymbol( result, rtn ) ) {
            rtn = NULL;
        }
        break;
      DbgDefault( "classAssignMember: impossible return" );
    }
    ScopeFreeResult( result );
    return rtn;
}


static PTREE classArrayRtCall(  // GENERATE R/T CALL FOR ARRAY
    PTREE expr,                 // - specific arguments
    PTREE src,                  // - source expression
    PTREE tgt,                  // - target expression
    TYPE artype,                // - array type
    unsigned rtn_code )         // - R/T code
{
    target_size_t nelem;        // - number of array elements

    nelem = ArrayTypeNumberItems( artype );
    expr = NodeArgument( expr, NodeOffset( nelem ) );
    if( src != NULL ) {
        expr = NodeArgument( expr, src );
    }
    expr = NodeArgument( expr, tgt );
    expr = RunTimeCall( expr
                      , PointerTypeForArray( artype )
                      , rtn_code );
    return expr;
}


// This is called to generate the inline code for simple copy CTOR and
// for simple operator=().
//
static void generateCopyObject( // GENERATE COPY OF OBJECT
    void )
{
    PTREE expr;                 // - generated expression
    PTREE tgt;                  // - target node

    expr = nodeFirstParm();
    expr = NodeSetMemoryExact( expr );
    expr = NodeFetch( expr );
    expr->flags &= ~ PTF_LVALUE;
    tgt = NodeThis();
    tgt->type = TypedefModifierRemove( tgt->type )->of;
    tgt->flags |= PTF_LVALUE;
    expr = NodeCopyClassObject( tgt, expr );
    EmitAnalysedStmt( expr );
}

static PTREE bitFieldLValueAdjust( PTREE t )
{
    TYPE type;

    type = TypedefModifierRemove( t->type );
    if( type->id == TYP_BITFIELD ) {
        t = NodeUnaryCopy( CO_BITFLD_CONVERT, t );
        t->type = type->of;
    }
    return( t );
}

static PTREE bitFieldNodeAssign( PTREE tgt, PTREE src, PTREE (*fetch)( PTREE ) )
{
    src = bitFieldLValueAdjust( src );
    src = fetch( src );
    tgt = bitFieldLValueAdjust( tgt );
    return( NodeAssign( tgt, src ) );
}


static void emitOpeqCall(       // EMIT AN ASSIGNMENT FOR DEFAULT OP=
    TITER comp_type,            // - type of element
    type_flag qualifier,        // - qualifier for this
    SCOPE scope,                // - scope for op= being compiled
    CDOPT_ITER *iter,           // - iterator for class
    TYPE cltype,                // - elemental type
    TYPE artype,                // - array type
    BASE_CLASS* vbase )         // - NULL or virtual base
{
    PTREE src;                  // - source expression
    PTREE tgt;                  // - target expression
    PTREE expr = NULL;          // - expression to be emitted
    SYMBOL op_equal;            // - op= for array base element
    SYMBOL assop;               // - assignment operator
    target_offset_t offset;     // - offset of element

    offset = CDoptIterOffsetExact( iter );
    if( vbase == NULL ) {
        tgt = addOffsetToThis( offset, cltype );
        src = addOffsetToFirstParm( offset, cltype );
    } else {
        target_offset_t soff;   // - symbol offset from vbase
        soff = offset - vbase->delta;
        tgt = accessThisBase( scope, vbase );
        tgt = addOffset( tgt, soff, cltype );
        src = accessSourceBase( scope, vbase );
        src = addOffset( src, soff, cltype );
    }
    ScopeGenAccessSet( cltype );
    switch( comp_type ) {
      case TITER_CLASS_EXACT :
      case TITER_CLASS_VBASE :
      case TITER_CLASS_DBASE :
      case TITER_NAKED_DTOR  :
        if( NULL == StructType( cltype ) ) {
            expr = bitFieldNodeAssign( tgt, src, NodeRvalue );
        } else {
            expr = NodeAssign( tgt, src );
            expr = ClassAssign( expr );
        }
        break;
      case TITER_ARRAY_EXACT :
      case TITER_ARRAY_VBASE :
        if( NULL == StructType( cltype ) ) {
            src = NodeConvertFlags( artype, src, PTF_MEMORY_EXACT | PTF_LVALUE );
            tgt = NodeConvertFlags( artype, tgt, PTF_MEMORY_EXACT | PTF_LVALUE );
            expr = bitFieldNodeAssign( tgt, src, NodeFetch );
        } else if( ClassNeedsAssign( cltype, TRUE ) ) {
            if( qualifier != 0 ) {
                cltype = MakeModifiedType( cltype, qualifier );
            }
            assop = classArrayRtn( cltype
                                 , ERR_CALL_WATCOM
                                 , qualifier
                                 , classAssignResult( cltype
                                                    , &op_equal
                                                    , NULL ) );
            if( NULL == assop ) {
                NodeFreeDupedExpr( src );
                NodeFreeDupedExpr( tgt );
                expr = NULL;
            } else {
                CDtorScheduleArgRemap( assop );
                assop = ClassFunMakeAddressable( assop );
                assop->flag |= SF_ADDR_TAKEN;
                expr = NodeArguments( MakeNodeSymbol( assop )
                                    , NodeOffset( CgMemorySize( cltype ) )
                                    , NULL );
                expr = classArrayRtCall( expr
                                       , src
                                       , tgt
                                       , artype
                                       , RTF_ASSIGN_ARR );
                expr = PtdScopeCall( expr, op_equal );
            }
        } else {
#if 1
            src->type = artype;
            tgt->type = artype;
#endif
            src = NodeFetch( src );
            expr = NodeAssign( tgt, src );
        }
        break;
    }
    ScopeGenAccessReset();
    EmitAnalysedStmt( expr );
}


static void emitOpeq(           // EMIT DEFAULT ASSIGNMENTS FOR COMPONENT
    CDOPT_ITER *iter,           // - iterator for class
    type_flag qualifier,        // - qualifier for this
    SCOPE scope )               // - scope for op= being compiled
{
    TITER comp_type;            // - type of component
    TYPE artype;                // - type of array
    unsigned depth;             // - depth of elements

    depth = 0;
    for( ; ; ) {
        comp_type = CDoptIterNextElement( iter );
        switch( comp_type ) {
          case TITER_NONE :
            if( depth == 0 ) break;
            -- depth;
            continue;
          case TITER_ARRAY_EXACT :
            artype = CDoptIterType( iter );
            emitOpeqCall( comp_type
                        , qualifier
                        , scope
                        , iter
                        , ArrayBaseType( artype )
                        , artype
                        , NULL );
            continue;
          case TITER_CLASS_EXACT :
          case TITER_CLASS_DBASE :
            emitOpeqCall( comp_type
                        , qualifier
                        , scope
                        , iter
                        , CDoptIterType( iter )
                        , NULL
                        , NULL );
            continue;
          case TITER_ARRAY_VBASE :
            artype = CDoptIterType( iter );
            emitOpeqCall( comp_type
                        , qualifier
                        , scope
                        , iter
                        , ArrayBaseType( artype )
                        , artype
                        , CDoptIterVBase( iter ) );
            continue;
          case TITER_CLASS_VBASE :
          case TITER_NAKED_DTOR :
            emitOpeqCall( comp_type
                        , qualifier
                        , scope
                        , iter
                        , CDoptIterType( iter )
                        , NULL
                        , CDoptIterVBase( iter ) );
            continue;
          case TITER_MEMB :
          case TITER_DBASE :
          case TITER_VBASE :
            ++depth;
            continue;
          DbgDefault( "emitOpeq -- bad TITER value" );
        }
        break;
    }
}


void GenerateDefaultAssign(     // EMIT A DEFAULT ASSIGN
    SYMBOL operator )           // - the assignment operator
{
    CD_DESCR *optinfo;          // - optimization information
    type_flag qualifier;        // - qualifier for "this"
    CDOPT_ITER* iter;           // - iterator for class elements
    SCOPE scope;                // - scope for class
    TYPE class_type;            // - class operator= is in
    CLASSINFO *info;            // - info on class
    auto FUNCTION_DATA fn_data; // - function data for operator=
    auto error_state_t check;   // - error data

    if( SymClassCorrupted( operator ) ) {
        return;
    }
    CtxFunction( operator );
    class_type = SymClass( operator );
    info = class_type->u.c.info;
    if( info->const_ref ) {
        CErr2p( ERR_CANNOT_GENERATE_DEFAULT, operator );
        return;
    }
    optinfo = CDoptDefOpeqBuild( class_type );
    if( CDoptErrorOccurred( optinfo ) ) {
        return;
    }
    initClassFunction( operator, &fn_data, &check, TRUE );
    CDoptChkAccFun( optinfo );
    if( ClassNeedsAssign( class_type, FALSE ) ) {
        qualifier = SymFuncArgList( operator )->qualifier;
        scope = SymScope( operator );
        iter = CDoptIterBeg( optinfo );
        for( ; ; ) {
            TITER comp_type = CDoptIterNextComp( iter );
            if( comp_type == TITER_NONE ) break;
            emitOpeq( iter, qualifier, scope );
        }
        CDoptIterEnd( iter );
    } else if( info->has_data ) {
        generateCopyObject();
    }
    returnThis( operator );
    finiClassFunction( operator, &fn_data, &check );
}

static CALL_DIAG diagAssign =   // diagnosis for assignment
{   ERR_ASSIGN_OPR_AMBIGUOUS
,   ERR_ASSIGN_OPR_NO_MATCH
,   ERR_ASSIGN_OPR_NO_MATCH
};


static SEARCH_RESULT *classAssignResult( // GET SEARCH_RESULT FOR OPERATOR=()
    TYPE type,                  // - type for class
    SYMBOL *assign,             // - addr[ SYMBOL for operator=() ]
    SCOPE access )              // - derived access scope
{
    SYMBOL sym;
    SEARCH_RESULT *result;
    NAME name;

    name = CppOperatorName( CO_EQUAL );
    type = ClassTypeForType( type );
    result = classResult( type, assign, name, access );
    if( result == NULL ) {
        sym = ClassAddDefaultAssign( type->u.c.scope );
        if( sym != NULL ) {
            result = classResult( type, assign, name, access );
        }
    }
    return( result );
}


SYMBOL ClassDefaultOpEq(        // GET DEFAULT OP= FOR A CLASS
    TYPE cltype,                // - class type
    TYPE derived )              // - derived access type
{
    SYMBOL opeq;                // - op= to be accessed
    SEARCH_RESULT *result;      // - search result for op=

    result = classAssignResult( cltype
                              , &opeq
                              , StructType( derived )->u.c.scope );
    if( result != NULL ) {
        ScopeFreeResult( result );
    }
    return opeq;
}


static boolean canBinaryCopy(   // TEST IF COPY CAN BE BINARY
    PTREE tgt,                  // - target
    PTREE src )                 // - source
{
    TYPE tgt_type;              // - type for class being assigned to
    TYPE src_type;              // - type for class being assigned from

    tgt_type = ClassTypeForType( tgt->type );
    src_type = ClassTypeForType( src->type );
    if( tgt_type == NULL || tgt_type != src_type ) {
        return( FALSE );
    }
    if( ! TypeDefined( tgt_type ) ) {
        return( FALSE );
    }
    if( tgt_type->u.c.info->const_ref ) {
        // op= cannot be generated for classes with const or ref members
        return( FALSE );
    }
    return( OMR_CLASS_VAL == ObjModelArgument( src_type ) );
}


static TYPE checkCopyModel(     // CHECK ASSIGNMENT FOR MEMORY-MODEL VIOLATION
    PTREE expr,                 // - expression to be checked
    unsigned error_code )       // - error code
{
    TYPE type;                  // - type for checking

    type = NodeType( expr );
    type = TypeReferenced( type );
    if( TypeTruncByMemModel( type ) ) {
        PTreeWarnExpr( expr, error_code );
    }
    return type;
}


static PTREE doBinaryCopy(      // DO A BINARY COPY
    PTREE tgt,                  // - target
    PTREE src,                  // - source
    unsigned code_tgt,          // - error for truncation on "this"
    unsigned code_src )         // - error for truncation on source
{
    TYPE tgt_type;              // - type for class being assigned to
//    TYPE src_type;              // - type for class being assigned from

    tgt_type = checkCopyModel( tgt, code_tgt );
    src = NodeRvalue( src );
//    src_type = checkCopyModel( src, code_src );
    checkCopyModel( src, code_src );
    if( OMR_CLASS_REF == ObjModelArgument( tgt_type ) ) {
        src->flags &= ~PTF_CLASS_RVREF;
        src->flags |= PTF_LVALUE | PTF_MEMORY_EXACT;
        src = NodeConvertFlags( tgt_type, src, PTF_MEMORY_EXACT | PTF_LVALUE );
        src = NodeFetch( src );
        src->flags |= PTF_MEMORY_EXACT;
    } else {
        src = NodeConvertFlags( tgt_type, src, PTF_MEMORY_EXACT );
    }
    tgt = NodeConvertFlags( tgt_type, tgt, PTF_MEMORY_EXACT | PTF_LVALUE );
    return NodeCopyClassObject( tgt, src );
}


static PTREE doClassAssign(     // ASSIGN TO CLASS OBJECT
    PTREE expr,                 // - expression ( "=" at top )
    SCOPE access )              // - derived access scope
{
    SEARCH_RESULT *result;      // - search result
    TYPE tgt_type;              // - type for class being assigned to
    PTREE src;                  // - source operand
    PTREE tgt;                  // - target operand
    PTREE fun;                  // - function operand
    PTREE call_expr;            // - call expression
    SYMBOL assign;              // - function for assignment

    tgt = expr->u.subtree[0];
    src = expr->u.subtree[1];
    tgt_type = ClassTypeForType( tgt->type );
    if( TypeDefined( tgt_type ) ) {
        if( ! ClassNeedsAssign( tgt_type, FALSE )
         && canBinaryCopy( tgt, src )
         && 0 == cvFlags( expr->u.subtree[0] )
         && 0 == ( cvFlags( expr->u.subtree[1] ) & TF1_VOLATILE ) ) {
            PTreeFree( expr );
            call_expr = doBinaryCopy( tgt
                                    , src
                                    , ERR_THIS_OBJ_MEM_MODEL
                                    , WARN_POINTER_TRUNCATION );
        } else {
            result = classAssignResult( tgt_type, &assign, access );
            if( result == NULL ) {
                PTreeErrorNode( expr );
                call_expr = expr;
            } else {
                fun = NodeMakeCallee( assign );
                fun->u.symcg.result = result;
                fun->cgop = CO_NAME_DOT;
                fun = PTreeCopySrcLocation( fun, expr );
                tgt = NodeDottedFunction( tgt, fun );
                tgt = PTreeCopySrcLocation( tgt, expr );
                call_expr = NodeBinary( CO_CALL_NOOVLD, tgt, NodeArg( src ) );
                call_expr = PTreeCopySrcLocation( call_expr, expr );
                call_expr = AnalyseCall( call_expr, &diagAssign );
                PTreeFree( expr );
            }
        }
    } else {
        PTreeErrorExpr( expr, ERR_ASSIGN_TO_UNDEF_CLASS );
        call_expr = expr;
    }
    return call_expr;
}

PTREE ClassAssign(              // ASSIGN TO CLASS OBJECT
    PTREE expr )                // - expression ( "=" at top )
{
    return doClassAssign( expr, NULL );
}

void GenerateDefaultCopy(       // EMIT A DEFAULT COPY CTOR
    SYMBOL copy_ctor )          // - the copy ctor
{
    auto FUNCTION_DATA fn_data;
    error_state_t check;

    if( SymClassCorrupted( copy_ctor ) ) {
        return;
    }
    initClassFunction( copy_ctor, &fn_data, &check, TRUE );
    finiClassFunction( copy_ctor, &fn_data, &check );
}


static SEARCH_RESULT *accessDefaultCopy( // ACCESS DEFAULT-COPY CTOR
    TYPE type,                  // - class type
    SYMBOL *ctor )              // - constructor
{
//    SYMBOL sym;
    SEARCH_RESULT *result;
    NAME name;

    name = CppConstructorName();
    result = classResult( type, ctor, name, NULL );
    if( result == NULL ) {
//        sym = ClassAddDefaultCopy( type->u.c.scope );
        ClassAddDefaultCopy( type->u.c.scope );
        result = classResult( type, ctor, name, NULL );
    }
    return result;
}


SYMBOL CopyCtorFind(            // FIND (OR CREATE) A COPY CTOR FOR A TYPE
    TYPE type,                  // - class type
    TOKEN_LOCN *locn )          // - error location
{
    SYMBOL ctor;                // - the copy CTOR
    SEARCH_RESULT *result;      // - result of look up

    type = ClassTypeForType( type );
    result = accessDefaultCopy( type, &ctor );
    ScopeResultErrLocn( result, locn );
    if( ScopeCheckSymbol( result, ctor ) ) {
        ctor = NULL;
    }
    ScopeFreeResult( result );
    return ctor;
}


static CALL_DIAG diagCopy =     // diagnosis for copy
{   ERR_COPYCTOR_AMBIGUOUS
,   ERR_COPYCTOR_IMPOSSIBLE
,   ERR_COPYCTOR_IMPOSSIBLE
};


static SEARCH_RESULT* classCopyResult( // GET SEARCH_RESULT FOR COPY CTOR
    TYPE type,                  // - class type
    SYMBOL *ctor )              // - addr[copy ctor ]
{
    SEARCH_RESULT *result;      // - search result
    NAME name;                  // - name of copy ctor

    name = CppConstructorName();
    result = classResult( type, ctor, name, NULL );
    if( result == NULL ) {
        result = accessDefaultCopy( type, ctor );
    }
    return result;
}


static PTREE genDefaultCopyDiag(// GENERATE COPY TO CLASS OBJECT, WITH DIAGNOSIS
    PTREE tgt,                  // - target expression
    PTREE src,                  // - source expression
    CALL_DIAG* diagnosis,       // - call diagnosis
    SYMBOL* a_ctor_used )       // - ctor used
{
    SEARCH_RESULT *result;      // - search result
    TYPE tgt_type;              // - type for class being assigned to
    PTREE fun;                  // - function operand
    PTREE expr;                 // - expression to be analysed, emitted
    SYMBOL ctor;                // - CTOR for copy
    PTREE this_arg;             // - "this" argument, after call analysis

    if( canBinaryCopy( tgt, src )
     && 0 == ( cvFlags( src ) & TF1_VOLATILE ) ) {
        expr = doBinaryCopy( tgt
                           , src
                           , ERR_CTOR_OBJ_MEM_MODEL
                           , WARN_POINTER_TRUNCATION );
        *a_ctor_used = NULL;
    } else {
        tgt_type = ClassTypeForType( tgt->type );
        result = classCopyResult( tgt_type, &ctor );
        fun = NodeMakeCallee( ctor );
        fun->u.symcg.result = result;
        fun->cgop = CO_NAME_DOT;
        fun = PTreeCopySrcLocation( fun, src );
        tgt = NodeBinary( CO_ARROW, tgt, fun );
        tgt = PTreeCopySrcLocation( tgt, src );
        expr = NodeBinary( CO_CALL_NOOVLD, tgt, NodeArg( src ) );
        expr = PTreeCopySrcLocation( expr, src );
        expr = AnalyseCall( expr, diagnosis );
        if( expr->op == PT_ERROR ) {
            *a_ctor_used = NULL;
        } else {
            this_arg = NodeGetCallExpr( expr )->u.subtree[1];
            this_arg->u.subtree[0]
                = NodeArgumentExactCtor( this_arg->u.subtree[0]
                                       , tgt_type
                                       , TRUE );
            *a_ctor_used = ctor;
        }
    }
    return expr;
}


static boolean accessCopyCtor(  // CHECK ACCESS TO DEFAULT COPY CTOR
    TYPE type,                  // - type for class
    SYMBOL *ctor )              // - addr[ copy ctor ]
{
    boolean retn;               // - TRUE ==> access is ok
    SEARCH_RESULT *result;      // - search result

    type = ClassTypeForType( type );
    result = accessDefaultCopy( type, ctor );
    if( result == NULL ) {
        ctor = NULL;
        retn = TRUE;
    } else {
        retn = ! ScopeCheckSymbol( result, *ctor );
        ScopeFreeResult( result );
    }
    return( retn );
}


static PTREE defaultCopyDiag(   // COPY TO CLASS OBJECT, WITH DIAGNOSIS
    PTREE left,                 // - target expression
    PTREE src,                  // - source expression
    CALL_DIAG* diagnosis,       // - call diagnosis
    boolean temp_ok,            // - TRUE ==> temp ok
    SYMBOL* a_ctor_used )       // - ctor used
{
    TYPE type;                  // - type being initialized (unmodified)
    TYPE type_modified;         // - type being initialized (modified)
    TYPE type_left;             // - type being initialized (original)
    TYPE type_right;            // - type of source (original)
    PTREE dtor;                 // - CO_DTOR operand
    PTREE right;                // - source expression, after dtor'ing
    PTREE expr;                 // - resultant expression
    SYMBOL ctor_udc;            // - udcf/ctor from udc lookup
    CALL_OPT opt;               // - type of optimization
    FNOV_COARSE_RANK rank;      // - UDC RANKING
    boolean is_ctor;            // - TRUE ==> ctor udc, FALSE ==> udcf udc
    FNOV_LIST* fnov_list;       // - matches list
    FNOV_DIAG fnov_diag;        // - for diagnostics
    TOKEN_LOCN err_locn;        // - location for errors
    SEARCH_RESULT* result;      // - search result for symbol
    TYPE udcf_type = NULL;      // - target type for UDCF
    PTREE src_list[1];          // - source list

    type_left = NodeType( left );
    type_right = NodeType( src );
    type_modified = TypeReference( type_left );
    DbgVerify( NULL != type_modified, "defaultCopyDiag -- not lvalue" );
    type = TypedefModifierRemove( type_modified );
    DbgVerify( NULL != StructType( type ), "defaultCopyDiag -- not class" );
    ClassAddDefaultCopy( type->u.c.scope );
    src_list[0] = src;
    rank = UdcLocate( FNOV_UDC_COPY
                    , TypeReferenced( type_right )
                    , type_modified
                    , src_list
                    , &is_ctor
                    , &fnov_list
                    , &fnov_diag );
    opt = CALL_OPT_ERR;
    switch( rank ) {
      case OV_RANK_UD_CONV :
        ctor_udc = fnov_list->sym;
        FnovListFree( &fnov_list );
        FnovFreeDiag( &fnov_diag );
        opt = CALL_OPT_NONE;
        if( is_ctor ) {
            src = NodeLvExtract( src );
            result = classResult( type, &ctor_udc, ctor_udc->name->name, NULL );
        } else {
            TYPE src_class = ClassTypeForType( type_right );
            udcf_type = SymFuncReturnType( ctor_udc );
            result = ScopeFindScopedMemberConversion
                            ( src_class->u.c.scope
                            , NULL
                            , udcf_type
                            , TF1_NULL );
        }
        DbgVerify( result != NULL, "No CTOR/UDCF after UDC lookup" );
        PTreeExtractLocn( src, &err_locn );
        ScopeResultErrLocn( result, &err_locn );
        if( ScopeCheckSymbol( result, ctor_udc ) ) {
            opt = CALL_OPT_ERR;
        }
        ScopeFreeResult( result );
        if( ! is_ctor ) {
            src = UdcCall( src, udcf_type, diagnosis );
            if( PT_ERROR == src->op ) {
                opt = CALL_OPT_ERR;
            } else {
                SCOPE curr = GetCurrScope();
                SetCurrScope(type->u.c.scope);
                accessCopyCtor( type, &ctor_udc );
                SetCurrScope(curr);
            }
        }
        break;
      case OV_RANK_UD_CONV_AMBIG :
        CallDiagAmbiguous( src, ERR_UDC_AMBIGUOUS, &fnov_diag );
        FnovFreeDiag( &fnov_diag );
        ConversionTypesSet( type_left, type_right );
        ConversionDiagnoseInf();
        break;
      case OV_RANK_NO_MATCH :
#if 0
        CallDiagnoseRejects( src, diagnosis->msg_no_match_one, &fnov_diag );
#else
        UdcDiagNoMatch( src
                      , type_modified
                      , diagnosis->msg_no_match_one
                      , diagnosis->msg_no_match_many
                      , &fnov_diag );
#endif
        FnovFreeDiag( &fnov_diag );
        break;
      DbgDefault( "FindConvFunc -- bad return from overloading" );
    }
    if( opt == CALL_OPT_NONE ) {
        opt = AnalyseCallOpts( type, src, &dtor, &right );
        if( type == ClassTypeForType( right->type ) ) {
            TYPE src_type;
            if( ctor_udc == NULL ) {
                src_type = MakeConstReferenceTo( StructType( type ) );
            } else {
                src_type = SymFuncArgList( ctor_udc )->type_list[0];
            }
            if( !temp_ok && NodeNonConstRefToTemp( src_type, right ) ) {
                opt = CALL_OPT_ERR;
            }
        }
    }
    if( opt == CALL_OPT_NONE ) {
        expr = genDefaultCopyDiag( left, src, diagnosis, &ctor_udc );
    } else {
        expr = CopyOptimize( right, src, left, dtor, opt );
        ctor_udc = NULL;
    }
    if( expr->op == PT_ERROR ) {
        ctor_udc = NULL;
    } else {
        expr->flags |= PTF_MEMORY_EXACT;
    }
    *a_ctor_used = ctor_udc;
    return expr;
}


PTREE ClassDefaultCopy(         // COPY TO CLASS OBJECT
    PTREE tgt,                  // - target expression
    PTREE src )                 // - source expression
{
    SYMBOL not_used;            // - not used

    return defaultCopyDiag( tgt, src, &diagCopy, FALSE, &not_used );
}


PTREE ClassDefaultCopyDiag(     // COPY TO CLASS OBJECT, WITH DIAGNOSIS
    PTREE tgt,                  // - target expression
    PTREE src,                  // - source expression
    CNV_DIAG *cnvdiag )         // - conversion diagnosis
{
    CALL_DIAG diagnosis;        // - diagnosis for call
    SYMBOL not_used;            // - not used

    return defaultCopyDiag( tgt
                          , src
                          , CallDiagFromCnvDiag( &diagnosis, cnvdiag )
                          , FALSE
                          , &not_used );
}


PTREE ClassDefaultCopyTemp(     // COPY TEMP TO CLASS OBJECT, WITH DIAGNOSIS
    PTREE tgt,                  // - target expression
    PTREE src,                  // - source expression
    CNV_DIAG *cnvdiag )         // - conversion diagnosis
{
    CALL_DIAG diagnosis;        // - diagnosis for call
    SYMBOL not_used;            // - not used

    return defaultCopyDiag( tgt
                          , src
                          , CallDiagFromCnvDiag( &diagnosis, cnvdiag )
                          , TRUE
                          , &not_used );
}


static CNV_DIAG diagTempCopy =     // diagnosis for copy
{   ERR_COPYCTOR_IMPOSSIBLE
,   ERR_COPYCTOR_AMBIGUOUS
,   ERR_CONVERT_FROM_VIRTUAL_BASE
,   ERR_INIT_PRIVATE
,   ERR_INIT_PROTECTED
};


PTREE ClassCopyTemp(            // COPY A TEMPORARY
    TYPE cl_type,               // - class type
    PTREE expr,                 // - source expression
    PTREE temp_node )           // - target temporary
{
#if 0
    SYMBOL ctor;                // - CTOR used
#endif
    expr = NodeLvExtract( expr );
    if( NodeReferencesTemporary( expr )
     && cl_type == ClassTypeForType( expr->type ) ) {
        NodeFreeDupedExpr( temp_node );
        expr = NodeLvExtract( expr );
        expr->type = cl_type;
        expr->flags |= PTF_LVALUE;
    } else {
        if( NULL == temp_node ) {
            temp_node = NodeTemporary( cl_type );
            temp_node = PTreeCopySrcLocation( temp_node, expr );
        }
#if 0
        expr = defaultCopyDiag( temp_node, expr, &diagCopy, &ctor );
        expr = NodeDtorExpr( expr, temp_node->u.symcg.symbol );
        if( ctor != NULL ) {
            expr = PtdCtoredExprType( expr, ctor, cl_type );
        }
#else
        expr = CopyInit( expr, temp_node, cl_type, &diagTempCopy );
        expr = NodeDtorExpr( expr, temp_node->u.symcg.symbol );
#endif
    }
    return expr;
}


#if 0
boolean ClassAccessDefaultCopy( // CHECK ACCESS TO DEFAULT COPY CTOR
    TYPE type )                 // - type for class
{
    SYMBOL ctor;                // - copy ctor (not used)
    boolean retn;               // - TRUE ==> access if OK

    type = ClassTypeForType( type );
    if( OMR_CLASS_VAL == ObjModelArgument( type ) ) {
        retn = TRUE;
    } else {
        retn = accessCopyCtor( type, &ctor );
    }
    return retn;
}
#endif


static CNV_DIAG diagDefaultCtor = // diagnosis for default CTOR failure
    {   ERR_DEF_CTOR_IMPOSSIBLE // - impossible
    ,   ERR_CALL_WATCOM         // - ambiguous
    ,   ERR_CALL_WATCOM         // - virtual->derived
    ,   ERR_CALL_WATCOM         // - private
    ,   ERR_CALL_WATCOM         // - protected
    };


static SYMBOL findDefCtorToGen( // FIND DEFAULT CTOR TO GENERATE
    TYPE cl_type )              // - type for class
{
    SYMBOL ctor;                // - symbol for CTOR
    CNV_RETN retn;              // - conversion return: CNV_...

    cl_type = StructType( cl_type );
    retn = ClassDefaultCtorFind( cl_type, &ctor, NULL );
    ConversionInfDisable();
    if( CNV_OK == ConversionDiagnose( retn, NULL, &diagDefaultCtor ) ) {
        if( ! cl_type->u.c.info->needs_ctor ) {
            ctor = NULL;
        }
    } else {
        InfMsgPtr( INF_DEF_CTOR, cl_type );
        ctor = NULL;
    }
    return ctor;
}


static PTREE generateArrayClassCall( // CALL CTOR/DTOR FOR ARRAY OF CLASS OBJ.s
    TYPE artype,                // - array type
    TYPE_SIG *sig,              // - type signature
    PTREE sym,                  // - symbol being operated upon
    unsigned rtn_code )         // - code for runtime call
{
    PTREE expr;                 // - generated expression

    expr = classArrayRtCall( NodeTypeSigArg( sig )
                           , NULL
                           , sym
                           , artype
                           , rtn_code );
    return expr;
}

static PTREE generateArrayDtorCall( // CALL R/T ROUTINE TO DTOR ARRAY
    TYPE array_type,            // - array type
    PTREE node_this )           // - item to be DTOR'ed
{
    TYPE base_type;             // - array base type
    TYPE_SIG *base_sig;         // - type signature for base type
    boolean error_occurred;     // - TRUE ==> error occurred
    PTREE expr;                 // - generated expression

    base_type = ArrayBaseType( array_type );
    base_sig = TypeSigFind( TSA_DTOR, base_type, NULL, &error_occurred );
    if( error_occurred ) {
        expr = NULL;
    } else {
        node_this = NodeRvalue( node_this );
        TypeSigReferenced( base_sig );
        expr = generateArrayClassCall( array_type
                                     , base_sig
                                     , node_this
                                     , RTF_DTOR_ARR );
    }
    return expr;
}

void GenerateDefaultCtor(       // EMIT A DEFAULT CTOR
    SYMBOL ctor )               // - the ctor
{
    auto error_state_t check;
    auto FUNCTION_DATA fn_data;

    if( SymClassCorrupted( ctor ) ) {
        return;
    }
    initClassFunction( ctor, &fn_data, &check, FALSE );
    finiClassFunction( ctor, &fn_data, &check );
}


CNV_RETN ClassDefaultCtorFind(  // FIND DEFAULT CTOR FOR A CLASS
    TYPE cl_type,               // - type for class
    SYMBOL *pctor,              // - ptr( default CTOR )
    TOKEN_LOCN *src_locn )      // - source location for errors
{
    arg_list alist;             // - argument list

    cl_type = ClassTypeForType( cl_type );
    InitArgList( &alist );
    return CtorFind( NULL, cl_type, &alist, NULL, src_locn, pctor );
}


CNV_RETN ClassDefaultCtorDefine(// DEFINE DEFAULT CTOR FOR A CLASS
    TYPE cl_type,               // - type for class
    SYMBOL *pctor )             // - ptr( default CTOR )
{
    CNV_RETN result;            // - return status
    CLASSINFO *info;            // - class information
    SYMBOL ctor;                // - local ctor

    cl_type = ClassTypeForType( cl_type );
    info = cl_type->u.c.info;
    if( info->has_ctor ) {
        *pctor = NULL;
        result = CNV_IMPOSSIBLE;
    } else {
        ctor = ClassAddDefaultCtor( cl_type->u.c.scope );
        *pctor = ctor;
        result = CNV_OK;
    }
    return( result );
}

void GenerateDefaultDtor(       // EMIT A DEFAULT DTOR
    SYMBOL dtor )               // - the dtor
{
    auto FUNCTION_DATA fn_data;
    auto error_state_t check;

    if( SymClassCorrupted( dtor ) ) {
        return;
    }
    initClassFunction( dtor, &fn_data, &check, FALSE );
    finiClassFunction( dtor, &fn_data, &check );
}


static SYMBOL createArrayDtorSymbol( // CREATE ARRAY DTOR SYMBOL
    TYPE cl_type,               // - class type
    NAME name )                 // - name of DTOR
{
    SYMBOL sym;
    TYPE fn_type;

    fn_type = TypePtrVoidFunOfCDtorArg();
    fn_type = MakeCommonCodeData( fn_type );
    sym = AllocTypedSymbol( fn_type );
    sym->id = SC_MEMBER;
    sym->flag |= SF_INITIALIZED;
    return( ScopeInsert( TypeScope( cl_type ), sym, name ) );
}


static SYMBOL injectDtorSymbol( // INJECT DTOR INTO SCOPE
    TYPE cl_type )              // - class type
{
    SYMBOL sym;

    sym = ClassAddDefaultDtor( cl_type->u.c.scope );
    return( sym );
}


SEARCH_RESULT *DtorFindResult(  // FIND DTOR FOR A POSSIBLE VIRTUAL CALL
    TYPE cl_type )              // - class type
{
    SEARCH_RESULT *result;      // - result of search
    SCOPE scope;
    NAME dtor_name;

    cl_type = ClassTypeForType( cl_type );
    scope = cl_type->u.c.scope;
    dtor_name = CppDestructorName();
    result = ScopeContainsMember( scope, dtor_name );
    if( result != NULL ) {
        return( result );
    }
    injectDtorSymbol( cl_type );
    return( ScopeContainsMember( scope, dtor_name ) );
}


boolean ClassAccessDtor(        // CHECK ACCESS TO DTOR
    TYPE type )                 // - type for class
{
    boolean retn;               // - TRUE ==> access is ok
    SEARCH_RESULT *result;      // - search result
    SYMBOL dtor;

    type = ArrayBaseType( type );
    result = DtorFindResult( type );
    if( result == NULL ) {
        retn = TRUE;
    } else {
        dtor = result->sym_name->name_syms;
        retn = ! ScopeCheckSymbol( result, dtor );
        ScopeFreeResult( result );
    }
    return( retn );
}

static SYMBOL findOrDefineDtor( // FIND OR DEFINE DTOR FOR DIRECT CALL
    TYPE cl_type,               // - class type for dtor
    boolean check,              // - check access
    SCOPE access,               // - derived access scope
    TOKEN_LOCN *locn,           // - error location
    boolean del )               // - TRUE ==> return NULL on access violation
{
    NAME dtor_name;
    SYMBOL dtor;
    SCOPE class_scope;
    SEARCH_RESULT *result;

    cl_type = ClassTypeForType( cl_type );
    class_scope = TypeScope( cl_type );
    dtor_name = CppDestructorName();
    result = ScopeContainsMember( class_scope, dtor_name );
    if( result == NULL ) {
        dtor = injectDtorSymbol( cl_type );
        if( check ) {
            if( access == NULL ) {
                result = ScopeContainsMember( class_scope, dtor_name );
            } else {
                result = ScopeFindBaseMember( class_scope, dtor_name );
            }
        }
    } else {
        if( check ) {
            if( access != NULL ) {
                ScopeFreeResult( result );
                result = ScopeFindBaseMember( class_scope, dtor_name );
            }
        } else {
            dtor = result->sym_name->name_syms;
            ScopeFreeResult( result );
        }
    }
    if( check ) {
        ScopeResultErrLocn( result, locn );
        dtor = result->sym_name->name_syms;
        if( ScopeCheckSymbol( result, dtor ) && del ) {
            dtor = NULL;
        }
        ScopeFreeResult( result );
    }
    return( dtor );
}

static SYMBOL findOrDefineArrayDtor( // FIND (OR ALLOCATE) ARRAY DTOR FOR SYMBOL
    TYPE ar_type,               // - array type for dtor
    boolean check,              // - check access
    SCOPE access,               // - derived access scope
    TOKEN_LOCN *locn,           // - error location
    boolean del )               // - TRUE ==> return NULL on access violation
{
    NAME name;                  // - name of array dtor
    SCOPE scope;                // - scope for name
    SYMBOL dtor;                // - symbol created
    SEARCH_RESULT *result;      // - search result
    TYPE cl_type;               // - class type for array

    cl_type = ArrayBaseType( ar_type );
    dtor = findOrDefineDtor( cl_type, check, access, locn, del );
    if( dtor != NULL ) {
        name = CppArrayDtorName( ar_type );
        scope = TypeScope( cl_type );
        result = ScopeContainsMember( scope, name );
        if( result == NULL ) {
            dtor = createArrayDtorSymbol( cl_type, name );
            SymDeriveThrowBits( dtor, RoDtorFindType( cl_type ) );
            RtnGenAddType( RGTYPE_ArrayDtor, ar_type );
        } else {
            dtor = result->sym_name->name_syms;
            ScopeFreeResult( result );
        }
    }
    return( dtor );
}

void RtnGenCallBackArrayDtor(   // GENERATE ARRAY DTOR
    TYPE ar_type )              // - array type
{
    SYMBOL dtor;                // - DTOR symbol
    SYMBOL p1;                  // - parameter[1]: DTOR_NULL
    SYMBOL retn;                // - return symbol
    SCOPE scope;                // - scope for parameters
    NAME name;                  // - dummy name for parameter
    PTREE stmt;
    auto FUNCTION_DATA fn_data;
    auto error_state_t check;

    ar_type = ArrayType( ar_type );
    if( ClassCorrupted( ArrayBaseType( ar_type ) ) ) {
        return;
    }
    dtor = findOrDefineArrayDtor( ar_type, FALSE, NULL, NULL, FALSE );
    initClassFunction( dtor, &fn_data, &check, FALSE );
    scope = ScopeFunctionScopeInProgress();
    p1 = SymMakeDummy( MakeCDtorExtraArgType(), &name );
    p1 = ScopeInsert( scope, p1, name );
    stmt = generateArrayDtorCall( ar_type, NodeThis() );
    retn = SymFunctionReturn();
    stmt = NodeAssignRef( MakeNodeSymbol( retn ), stmt );
    EmitAnalysedStmt( stmt );
    CgFrontCodePtr( IC_PROC_RETURN, retn );
    finiClassFunction( dtor, &fn_data, &check );
}

SYMBOL DtorFind(                // FIND DTOR, CHECK ACCESS
    TYPE type )                 // - a class or array class type
{
    SYMBOL dtor;                // - DTOR found or created

    if( ArrayType( type ) != NULL ) {
        dtor = findOrDefineArrayDtor( type, TRUE, NULL, NULL, FALSE );
    } else {
        dtor = findOrDefineDtor( type, TRUE, NULL, NULL, FALSE );
    }
    return dtor;
}

SYMBOL DtorFindCg(              // FIND DTOR, DURING CODE-GEN
    TYPE type )                 // - a class or array class type
{
    SYMBOL dtor;                // - DTOR found or created

    if( ArrayType( type ) != NULL ) {
        dtor = findOrDefineArrayDtor( type, FALSE, NULL, NULL, FALSE );
    } else {
        dtor = findOrDefineDtor( type, FALSE, NULL, NULL, FALSE );
    }
    return dtor;
}

SYMBOL DtorFindLocn(            // FIND DTOR, CHECK ACCESS, WITH ERR LOC'N
    TYPE type,                  // - a class or array-class type
    TOKEN_LOCN *err_locn )      // - location for errors
{
    SYMBOL dtor;                // - DTOR found or created

    if( ArrayType( type ) != NULL ) {
        dtor = findOrDefineArrayDtor( type, TRUE, NULL, err_locn, TRUE );
    } else {
        dtor = findOrDefineDtor( type, TRUE, NULL, err_locn, TRUE );
    }
//  dtor = ClassFunMakeAddressable( dtor );
    return dtor;
}

SYMBOL RoDtorFindTypeLocn       // FIND DTOR FOR USE WITH R/O BLOCKS
    ( TYPE type                 // - type for DTOR
    , TOKEN_LOCN *err_locn )    // - location for errors
{
    SYMBOL dtor;                // - symbol for DTOR

    if( ArrayType( type ) == NULL ) {
        dtor = findOrDefineDtor( type, FALSE, NULL, err_locn, FALSE );
    } else {
        dtor = findOrDefineArrayDtor( type, FALSE, NULL, err_locn, FALSE );
    }
    dtor = ClassFunMakeAddressable( dtor );
    dtor->flag |= SF_ADDR_TAKEN;
    return SymMarkRefed( dtor );
}

SYMBOL RoDtorFindType(          // FIND DTOR FOR USE WITH R/O BLOCKS
    TYPE type )                 // - type for DTOR
{
    return RoDtorFindTypeLocn( type, NULL );
}

SYMBOL RoDtorFind(              // FIND DTOR FOR USE WITH R/O BLOCKS
    SYMBOL sym )                // - symbol to be DTOR'd
{
    return RoDtorFindTypeLocn( sym->sym_type, NULL );
}


static CNV_DIAG diagProInit =   // diagnosis for CTOR determination failure
{   ERR_CTOR_IMPOSSIBLE         // - impossible
,   ERR_CTOR_AMBIGUOUS          // - ambiguous
,   ERR_CALL_WATCOM             // - virtual->derived
,   ERR_CALL_WATCOM             // - private CTOR
,   ERR_CALL_WATCOM             // - protected CTOR
};


static void ctorScopeCall(      // EMIT IC_SCOPE_CALL FOR CTORING
    TYPE cltype )               // - class type
{
    cltype = StructType( cltype );
    if( NULL != cltype ) {
        SYMBOL dtor = RoDtorFindType( cltype );
        CgFrontScopeCall( NULL, dtor, DTORING_COMPONENT );
    }
}


static PTREE ctorPrologueInit(  // GENERATE INITIALIZATION FOR CTOR PROLOGUE
    PTREE init_item,            // - address of item to be initialized
    PTREE init_expr,            // - expression for initialization
    TYPE init_type,             // - type of item being initialized
    ctor_init options,          // - extra optional requests
    ctor_prologue *data )       // - traversal data
{
    SYMBOL ctor;                // - CTOR to be used
    CNV_RETN cnv_retn;          // - conversion diagnosis
    PTREE right;                // - right expression
    PTREE stmt;                 // - executable statement
    TYPE init_type_mod;         // - modified init_type
    SCOPE access;               // - scope that is accessing the constructor

    access = data->scope;
    init_type_mod = init_type;
    init_type = TypedefModifierRemoveOnly( init_type );
    if( init_expr == NULL ) {
        if( StructType( init_type ) == NULL || ! TypeNeedsCtor( init_type ) ) {
            PTreeFreeSubtrees( init_item );
            ctorScopeCall( init_type );
            return NULL;
        }
    } else {
        if( options & CI_MEM_INIT ) {
            /* init_expr is an unanalysed CO_CTOR parse tree */
            right = init_expr->u.subtree[1];
            init_expr->u.subtree[1] = NULL;
            NodeFreeDupedExpr( init_expr );
            init_expr = AnalyseRawExpr( right );
            if( init_expr != NULL && init_expr->op == PT_ERROR ) {
                NodeFreeDupedExpr( init_expr );
                PTreeFreeSubtrees( init_item );
                return NULL;
            }
        }
    }
    if( options & CI_INLINED ) {
        access = TypeScope( init_type );
    } else if( options & CI_EXACT ) {
        access = NULL;
    }
    cnv_retn = AnalyseTypeCtor( access
                              , init_type
#if 0
                              , CNV_INIT_COPY
#else
                              , CNV_INIT
#endif
                              , &ctor
                              , &init_expr );
    if( cnv_retn != CNV_OK ) {
        if( access != NULL && access->id == SCOPE_CLASS ) {
            ConversionTypesSet( ScopeClass( access ), init_type_mod );
        }
        if( ConversionDiagnose( cnv_retn, init_expr, &diagProInit )
            != CNV_OK ) {
            NodeFreeDupedExpr( init_expr );
            PTreeFreeSubtrees( init_item );
            return NULL;
        }
    }
    {
        TOKEN_LOCN locn;        // - error location
        unsigned effect_flags;  // - flags for EffectCtor
        if( init_item != NULL ) {
            PTreeExtractLocn( init_item, &locn );
        } else {
            PTreeExtractLocn( init_expr, &locn );
        }
        effect_flags = EFFECT_CTOR_DECOR;
        if( options & CI_EXACT ) {
            effect_flags |= EFFECT_EXACT;
        }
        if( options & CI_VIRTUAL ) {
            effect_flags |= EFFECT_VIRTUAL;
        }
        stmt = EffectCtor( init_expr
                         , ctor
                         , init_type
                         , init_item
                         , &locn
                         , effect_flags );
    }
    return stmt;
}


static PTREE ctorPrologueArray( // GENERATE INITIALIZATION FOR CTOR OF ARRAY
    PTREE init_item,            // - address of item to be initialized
    PTREE init_expr,            // - NULL or expression for source
    TYPE init_type,             // - type of item being initialized
    ctor_prologue *data )       // - traversal data
{
    TYPE cltype;                // - type of array element
    unsigned rtn_code;          // - code for run-time routine
    PTREE stmt;                 // - executable statement
    boolean errors;             // - indicates error
    TYPE_SIG_ACCESS tsa;        // - access for type sig
    TYPE_SIG *sig;              // - type signature for class

    init_type = TypedefModifierRemoveOnly( init_type );
    cltype = ArrayBaseType( init_type );
    if( data->comp_options & CI_INLINED ) {
        ScopeGenAccessSet( cltype );
    }
    if( init_expr != NULL ) {
        PTREE arg;              // - CO_LIST node
        DbgVerify( NodeIsBinaryOp( init_expr, CO_LIST )
                 , "ctorPrologueArray -- not CO_LIST" );
        arg = init_expr;
        init_expr = init_expr->u.subtree[1];
        PTreeFree( arg );
    }
    if( data->gen_copy ) {
        if( NULL == StructType( cltype )
         || NULL == CopyCtorFind( cltype, NULL ) ) {
            return bitFieldNodeAssign( init_item, init_expr, NodeFetch );
        }
        if( TypeHasVirtualBases( cltype ) ) {
            rtn_code = RTF_COPY_VARR;
        } else {
            rtn_code = RTF_COPY_ARR;
        }
        tsa = TSA_COPY_CTOR;
    } else if( TypeNeedsCtor( cltype ) ) {
        if( NULL == findDefCtorToGen( cltype ) ) {
            NodeFreeDupedExpr( init_item );
            return NULL;
        }
        if( TypeHasVirtualBases( cltype ) ) {
            rtn_code = RTF_CTOR_VARR;
        } else {
            rtn_code = RTF_CTOR_ARR;
        }
        tsa = TSA_DEFAULT_CTOR;
    } else {
        ctorScopeCall( cltype );
        NodeFreeDupedExpr( init_item );
        return NULL;
    }
    sig = TypeSigFind( tsa, cltype, NULL, &errors );
    DbgVerify( ! errors, "ctorPrologueArray -- unexpected errors" );
    TypeSigReferenced( sig );
    stmt = classArrayRtCall( NodeTypeSigArg( sig )
                           , init_expr
                           , init_item
                           , init_type
                           , rtn_code );
    {
        SYMBOL ctor;
        if( tsa == TSA_COPY_CTOR ) {
            ctor = sig->copy_ctor;
        } else {
            ctor = sig->default_ctor;
        }
        stmt = PtdScopeCall( stmt, ctor );
        stmt = PtdCtoredComponent( stmt, init_type );
    }
    if( data->comp_options & CI_INLINED ) {
        ScopeGenAccessReset();
    }
    return stmt;
}


static PTREE extractBaseInit( ctor_prologue *data, TYPE base_type )
{
    PTREE *last;
    PTREE curr;
    PTREE next;
    PTREE item;
    PTREE init;

    last = &(data->base_init);
    for( curr = *last; curr != NULL; curr = next ) {
        next = curr->u.subtree[0];
        init = curr->u.subtree[1];
        item = init->u.subtree[0];
        if( StructType( item->type ) == base_type ) {
            /* remove 'curr' from list */
            *last = next;
            PTreeFree( curr );
            return( init );
        }
        last = &(curr->u.subtree[0]);
    }
    return( NULL );
}


static PTREE ctorOfDtorable(    // MARK CTOR OF DTORABLE, IF REQUIRED
    PTREE expr,                 // - current expression
    TYPE type,                  // - elemental type
    target_size_t offset,       // - offset of component
    DTC_KIND kind )             // - kind of component
{
    if( ( expr == NULL || expr->op != PT_ERROR ) && TypeExactDtorable( type ) ) {
        expr = DataDtorCompClass( expr, offset, kind );
    }
    return( expr );
}

static TYPE getCtorThisType( ctor_prologue *data )
{
    TYPE this_type;

    this_type = data->this_type;
    if( this_type == NULL ) {
        this_type = TypeThis();
        data->this_type = this_type;
    }
    return( this_type );
}


static void ctorGenComponent(   // GENERATE CTOR OF COMPONENT
    ctor_prologue* data,        // - traversal data
    TYPE type,                  // - component type
    target_offset_t offset,     // - component offset
    PTREE expr,                 // - NULL or initialization expression
    DTC_KIND kind )             // - kind of component
{
    PTREE target;               // - ctor target
    TYPE this_type;             // - type of 'this'

    if( expr != NULL || TypeRequiresCtoring( type ) ) {
        target = addOffsetToThis( offset, type );
        if( NULL == ArrayType( type ) ) {
            expr = ctorPrologueInit( target
                                   , expr
                                   , type
                                   , data->comp_options
                                   , data );
        } else {
            expr = ctorPrologueArray( target, expr, type, data );
        }
        this_type = getCtorThisType( data );
        expr = setThisFromOffset( expr, type, this_type, offset );
    } else if( TypeReallyDtorable( type ) ) {
        ctorScopeCall( type );
    }
    expr = ctorOfDtorable( expr, type, offset, kind );
    EmitAnalysedStmt( expr );
}

static boolean isForThisItem(   // IS CDOPT ITER FOR THIS ITEM?
    ctor_prologue *data  )      // - traversal data
{
    CDOPT_ITER* optiter;        // - next input ctor for CD optimization

    optiter = data->optiter;
    if( data->comp_offset != CDoptIterOffsetExact( optiter ) ) {
        return( FALSE );
    }
    if( data->comp_type != CDoptIterType( optiter ) ) {
        return( FALSE );
    }
    return( TRUE );
}


static DTC_KIND getComponentKind( // GET DTC_... FOR COMPONENT
    ctor_init options )         // - options
{
    DTC_KIND kind = DTC_COMP_DBASE;
    if( options & CI_EXACT ) {
        kind = DTC_COMP_MEMB;
    } else if( options & CI_VIRTUAL ) {
        kind = DTC_COMP_VBASE;
    }
    return( kind );
}

static void ctorPrologueComponents( // GENERATE CTOR OF COMPONENTS
    ctor_prologue* data )       // - traversal data
{
    ctor_init remove_mask;      // - flasg to turn off after ctor genned
    boolean have_cdopt;         // - TRUE ==> have cdopt component

    if( ! data->have_cdopt && ! data->done_cdopt ) {
        if( TITER_NONE == CDoptIterNextComp( data->optiter ) ) {
            data->done_cdopt = TRUE;
        } else {
            data->have_cdopt = TRUE;
        }
    }
    if( data->done_cdopt ) {
        have_cdopt = FALSE;
    } else if( isForThisItem( data ) ) {
        if( data->gen_copy ) {
            have_cdopt = FALSE;
        } else {
            have_cdopt = TRUE;
        }
        data->have_cdopt = FALSE;
    } else {
        have_cdopt = FALSE;
    }
    if( data->comp_expr != NULL ) {
        ctorGenComponent( data
                        , data->comp_type
                        , data->comp_offset
                        , data->comp_expr
                        , getComponentKind( data->comp_options ) );
    } else if( have_cdopt ) {
        unsigned depth = 0;
        CLASSINFO* ci = TypeClassInfo( ArrayBaseType( data->comp_type ) );
        if( ci != NULL && ci->has_ctor && !ci->has_def_ctor && !ci->ctor_defined ) {
            CErr2p( ERR_NO_DEFAULT_INIT_CTOR, data->comp_type );
        } else {
            for(;;) {
                boolean is_exact;
                TITER elem_titer = CDoptIterNextElement( data->optiter );
                target_offset_t offset = CDoptIterOffsetExact( data->optiter );
                TYPE type = CDoptIterType( data->optiter );
                switch( elem_titer ) {
                  case TITER_NONE :
                    if( depth == 0 ) break;
                    --depth;
                    if( depth <= 1 ) {
                        data->comp_options &= ~ CI_INLINED;
                    }
                    if( TypeExactDtorable( type ) ) {
                        PtdObjPop( NULL );
                        EmitAnalysedStmt(
                            ctorOfDtorable( NULL
                                          , type
                                          , offset
                                          , CDoptObjectKind( data->optiter ) )
                                        );
                    }
                    continue;
                  case TITER_ARRAY_EXACT :
                  case TITER_ARRAY_VBASE :
                    // note: comes thru here only for default ctor
                    if( CDoptInlinedMember( data->optiter ) ) {
                        data->comp_options |= CI_INLINED;
                    }
                    // fall thru
                  case TITER_NAKED_DTOR :
                  case TITER_CLASS_EXACT :
                  case TITER_CLASS_VBASE :
                  case TITER_CLASS_DBASE :
                    if( CDoptIterCannotDefine( data->optiter ) ) {
                        CErr2p( ERR_NO_DEFAULT_INIT_CTOR, type );
                        continue;
                    }
                    remove_mask = CI_NULL;
                    is_exact = CDoptIterExact( data->optiter );
                    if( is_exact && ( data->comp_options & CI_EXACT ) == 0 ) {
                        remove_mask |= CI_EXACT;
                        data->comp_options |= CI_EXACT;
                    }
                    ctorGenComponent( data
                                    , type
                                    , offset
                                    , NULL
                                    , CDoptObjectKind( data->optiter ) );
                    if( depth == 0 ) {
                        remove_mask |= CI_INLINED;
                    }
                    data->comp_options &= ~ remove_mask;
                    continue;
                  case TITER_MEMB :
                  case TITER_DBASE :
                  case TITER_VBASE :
                    if( TypeExactDtorable( type ) ) {
                        PtdObjPush( NULL, type, NULL, offset );
                    }
                    ++depth;
                    if( depth > 1 ) {
                        data->comp_options |= CI_INLINED;
                    }
                    continue;
                  DbgDefault( "ctorGenComponents: bad TITER value" );
                }
                break;
            }
        }
    }
}


static PTREE extractMemberInit( // EXTRACT INITIALIZATION TREE
    ctor_prologue *data,        // - traversal data
    NAME name )                 // - name of item
{
    PTREE *last;
    PTREE curr;
    PTREE next;
    PTREE item;
    PTREE init;

    last = &(data->member_init);
    for( curr = *last; curr != NULL; curr = next ) {
        next = curr->u.subtree[0];
        init = curr->u.subtree[1];
        item = init->u.subtree[0];
        if( item->u.id.name == name ) {
            /* remove 'curr' from list */
            *last = next;
            PTreeFree( curr );
            return( init );
        }
        last = &(curr->u.subtree[0]);
    }
    return NULL;
}


static void ctorPrologueMember( // GENERATE PROLOGUE FOR MEMBER
    SYMBOL sym,                 // - member
    void *_data )               // - traversal data
{
    ctor_prologue *data = _data;
    if( data->gen_copy && SymIsAnonymousMember( sym ) ) {
        /* anonymous member host will be copied */
        return;
    }
    if( SymIsThisDataMember( sym ) ) {
        data->comp_type = sym->sym_type;
        data->comp_offset = sym->u.member_offset;
        data->comp_expr = extractMemberInit( data, sym->name->name );
        if( data->comp_expr != NULL ) {
            if( NULL != ArrayType( sym->sym_type ) ) {
                PTreeErrorExprSym( data->comp_expr
                                 , ERR_MEM_INIT_MEMBER
                                 , sym );
                PTreeFreeSubtrees( data->comp_expr );
            } else {
                data->comp_options = CI_EXACT | CI_MEM_INIT;
                ctorPrologueComponents( data );
            }
        } else {
            if( data->gen_copy ) {
                PTREE expr; // - expression for source
                expr = getRefSymFromFirstParm( sym, 0 );
                if( NULL != TypeReference( sym->sym_type ) ) {
                    expr = NodeUnaryCopy( CO_FETCH, expr );
                }
                data->comp_expr = NodeArg( expr );
            }
            data->comp_options = CI_NULL;
            ctorPrologueComponents( data );
        }
    }
}

static void checkForGenCopy( ctor_prologue *data )
{
    SEARCH_RESULT *result;

    result = ScopeFindNaked( GetCurrScope(), CppSpecialName( SPECIAL_COPY_ARG ) );
    if( result != NULL ) {
        data->gen_copy = TRUE;
        ScopeFreeResult( result );
    }
}

static void initCtorPrologue( ctor_prologue *data, PTREE mem_init, SCOPE scope )
{
    PTREE curr;
    PTREE next;
    PTREE item;
    PTREE *insertion;

    data->member_init = NULL;
    data->base_init = NULL;
    data->scope = scope;
    data->this_type = NULL;
    data->gen_copy = FALSE;
    data->excepts = CompFlags.excs_enabled;
    data->cdopt = FALSE;
    data->have_cdopt = FALSE;
    data->done_cdopt = FALSE;
    checkForGenCopy( data );
    for( curr = mem_init; curr != NULL; curr = next ) {
        next = curr->u.subtree[0];
        item = curr->u.subtree[1]->u.subtree[0];
        if( item->op == PT_ID ) {
            insertion = &(data->member_init);
        } else {
            insertion = &(data->base_init);
        }
        curr->u.subtree[0] = *insertion;
        *insertion = curr;
    }
}

static void emitOffset( target_offset_t offset )
{
    PTREE expr;

    expr = NodeOffset( offset );
    DgStoreScalar( expr, 0, expr->type );
    PTreeFreeSubtrees( expr );
}

static void emitVPtrInit( CLASS_TABLE *locn, SYMBOL sym, boolean vbptr )
{
    CgFrontCodeUint( IC_PARM_BIN, vbptr );
    CgFrontCodePtr( IC_PARM_SYM, sym );
    if( vbptr || locn->exact_delta == locn->delta ) {
        /* we know the exact delta to the vptr */
        CgFrontCodeUint( IC_EXACT_VPTR_INIT, locn->exact_delta );
        return;
    }
    CgFrontCodeUint( IC_PARM_BIN, locn->vb_offset );
    CgFrontCodeUint( IC_PARM_BIN, locn->vb_index );
    CgFrontCodeUint( IC_PARM_BIN, locn->exact_delta );
    CgFrontCodeUint( IC_VBASE_VPTR_INIT, locn->delta );
}


static void genVBTable( SCOPE scope, SYMBOL vbtable, CLASS_VBTABLE *table )
{
    TYPE *curr;
    BASE_CLASS *base;

    if( !SymIsInitialized( vbtable ) ) {
        DgSymbolLabel( vbtable );
        vbtable->flag |= SF_INITIALIZED;
        emitOffset( table->h.exact_delta );
        for( curr = table->data; *curr != NULL; ++curr ) {
            base = ScopeFindVBase( scope, *curr );
            emitOffset( base->delta - table->h.exact_delta );
        }
        CgSegId( vbtable );
    }
}


static void genVBPtrInits( SCOPE scope )
{
    SYMBOL sym;
    CLASS_VBTABLE *tables;
    CLASS_VBTABLE *table;

    tables = ScopeCollectVBTable( scope, SCV_CTOR );
    RingIterBeg( tables, table ) {
        sym = MakeVBTableSym( scope, table->h.count, table->h.exact_delta );
        emitVPtrInit( &(table->h), sym, TRUE );
        genVBTable( scope, sym, table );
    } RingIterEnd( table )
    RingFree( &tables );
}

static void genCtorDispInit( SCOPE scope )
{
    TYPE host_class;
    CLASSINFO *info;
    boolean no_code_reqd;
    BASE_CLASS *base;

    host_class = ScopeClass( scope );
    info = host_class->u.c.info;
    if( info->last_vbase == 0 ) {
        return;
    }
    no_code_reqd = TRUE;
    RingIterBeg( info->bases, base ) {
        if( _IsVirtualBase( base ) ) {
            if( TypeCtorDispRequired( host_class, base->type ) ) {
                no_code_reqd = FALSE;
                break;
            }
        }
    } RingIterEnd( base )
    if( no_code_reqd ) {
        return;
    }
    CgFrontCodePtr( IC_GEN_CTOR_DISP, host_class );
}

static void emitThunks( CLASS_VFTABLE *table, SYMBOL ctor )
{
    THUNK_ACTION *thunk;
    SCOPE scope;

    scope = SymScope( ctor );
    for( thunk = table->data; ; ++thunk ) {
        if( thunk->non_empty ) {
            EmitVfunThunk( scope, thunk );
        }
        if( thunk->last_entry ) {
            break;
        }
    }
    CgFrontSwitchFile( ctor );
}

static void emitRttiRef( SYMBOL sym, target_offset_t offset )
{
    TYPE type;
    PTREE expr;

    CgFrontDataPtr( IC_RTTI_REF, sym );
    if( sym != NULL ) {
        sym->flag |= SF_ADDR_TAKEN;
        expr = MakeNodeSymbol( sym );
    } else {
        expr = NodeZero();
    }
    type = TypePtrToVoid();
    DgStoreScalar( expr, offset, type );
    PTreeFreeSubtrees( expr );
}

static void emitVFNPointer( SYMBOL sym )
{
    TYPE type;
    PTREE expr;

    if( sym != NULL ) {
        sym->flag |= SF_ADDR_TAKEN;
        if( SymIsDtor( sym ) ) {
            sym = SymMarkRefed( sym );
        }
        expr = NodeMakeCallee( sym );
    } else {
        expr = NodeZero();
    }
    type = TypeVoidFunOfVoid();
    type = MakePointerTo( type );
    DgStoreScalar( expr, 0, type );
    PTreeFreeSubtrees( expr );
}

static void genVFPtrCode( SCOPE scope, SYMBOL ctor, CLASS_VFTABLE *table )
{
    SYMBOL sym;
    SYMBOL vfn_sym;
    SYMBOL rtti_sym;
    THUNK_ACTION *thunk;
    target_offset_t offset;

    sym = MakeVFTableSym( scope, table->h.count, table->h.exact_delta );
    emitVPtrInit( &(table->h), sym, FALSE );
    CgFrontCodePtr( IC_VFT_REF, sym );
    if( !SymIsInitialized( sym ) ) {
        emitThunks( table, ctor );
        CgFrontDataPtr( IC_VFT_BEG, sym );
        DgSymbolLabel( sym );
        sym->flag |= SF_INITIALIZED;
        rtti_sym = RttiBuild( scope, &(table->h), &offset );
        emitRttiRef( rtti_sym, offset );
        for( thunk = table->data; ; ++thunk ) {
            vfn_sym = thunk->sym;
            if( thunk->non_empty ) {
                vfn_sym = thunk->thunk;
            } else if( thunk->override != NULL ) {
                vfn_sym = thunk->override;
            }
            emitVFNPointer( vfn_sym );
            if( thunk->last_entry ) {
                break;
            }
        }
        CgSegId( sym );
        CgFrontData( IC_INIT_DONE );
    }
}

static boolean genVFPtrInits( SCOPE scope, SYMBOL ctor )
{
    CLASS_VFTABLE *tables;
    CLASS_VFTABLE *table;
    boolean something_went_wrong;

    tables = ScopeCollectVFTable( scope, SCV_CTOR );
    something_went_wrong = FALSE;
    /* initialize vfptrs of non-virtual classes */
    RingIterBeg( tables, table ) {
        if( table->corrupted ) {
            something_went_wrong = TRUE;
        }
        if( table->h.delta == table->h.exact_delta ) {
            genVFPtrCode( scope, ctor, table );
        }
    } RingIterEnd( table )
    /* initialize vfptrs of virtual classes */
    RingIterBeg( tables, table ) {
        if( table->corrupted ) {
            something_went_wrong = TRUE;
        }
        if( table->h.delta != table->h.exact_delta ) {
            genVFPtrCode( scope, ctor, table );
        }
    } RingIterEnd( table )
    RingFree( &tables );
    RttiDone( scope );
    return( something_went_wrong );
}


static void testExtraParm(          // EMIT TEST OF EXTRA PARAMETER
    unsigned mask,                  // - mask
    boolean branch_on )             // - TRUE ==> branch if on
{
    CgFrontCodeUint( branch_on ? IC_CDARG_TEST_ON : IC_CDARG_TEST_OFF, mask );
}


static void ctorPrologueBaseGen(    // GENERATE FOR CTOR BASE CLASS
    BASE_CLASS *base,               // - base
    void *_data )           // - traversal data
{
    ctor_prologue * data = _data;
    data->comp_type = base->type;
    data->comp_offset = base->delta;
    data->comp_expr = extractBaseInit( data, base->type );
    if( data->comp_expr == NULL ) {
        if( data->gen_copy ) {
            data->comp_expr = NodeArg( accessSourceBase( data->scope
                                                       , base ) );
        }
        data->comp_options = CI_NULL;
    } else {
        data->comp_options = CI_NULL | CI_MEM_INIT;
    }
    if( _IsVirtualBase( base ) ) {
        data->comp_options |= CI_VIRTUAL;
    }
    ctorPrologueComponents( data );
}

static void initVFPtr( SCOPE scope, SYMBOL ctor )
{
    if( ScopeHasVirtualFunctions( scope ) ) {
        genCtorDispInit( scope );
        genVFPtrInits( scope, ctor );
    }
}


void CtorPrologue(              // GENERATE PROLOGUE FOR CTOR
    SYMBOL ctor,                // - constructor SYMBOL
    PTREE mem_init,             // - optional mem-initializer
    void (*push_scope)          // - pushes SCOPE into control structure
        ( void ) )
{
    SCOPE scope;                // - scope for CTOR
    ctor_prologue data;         // - prologue data
    CD_DESCR* optinfo;          // - class traversal information
    TYPE class_type;            // - type if CTOR's class

    if( SymClassCorrupted( ctor ) ) {
        return;
    }
    scope = ScopeNearestNonTemplate( SymScope( ctor ) );
    initCtorPrologue( &data, mem_init, scope );
    class_type = ScopeClass( scope );
    if( TypeReallyDtorable( class_type ) ) {
        CDoptDtorBuild( class_type );
    }
    optinfo = CDoptDefCtorBuild( class_type );
    if( CDoptErrorOccurred( optinfo ) ) {
        return;
    }
    if( TypeHasNumArgs( ctor->sym_type, 0 ) ) {
        CDoptNoAccFun( optinfo );
    }
    data.optiter = CDoptIterBeg( optinfo );
    data.cdopt = TRUE;
    (*push_scope)();
    if( ScopeHasVirtualBases( scope ) ) {
        CgFrontCode( IC_CTOR_COMP_BEG );
        testExtraParm( CTOR_COMPONENT, TRUE );
        genVBPtrInits( scope );
        ScopeWalkVirtualBases( scope, ctorPrologueBaseGen, &data );
        CgFrontCode( IC_CDARG_LABEL );
        CgFrontCode( IC_CTOR_COMP_END );
    }
    ScopeWalkDirectBases( scope, ctorPrologueBaseGen, &data );
    if( ! CompFlags.extensions_enabled ) {
        // ISO C++ requires vfptr to be set here
        initVFPtr( scope, ctor );
    }
    memberWalkWithData( scope, ctorPrologueMember, &data );
    CgFrontCode( IC_CTOR_CODE );
    CDoptIterEnd( data.optiter );
    if( CompFlags.extensions_enabled ) {
        initVFPtr( scope, ctor );
    }
    NodeFreeDupedExpr( data.member_init );
    NodeFreeDupedExpr( data.base_init );
}


void CtorEpilogue(              // GENERATE EPILOGUE FOR CTOR
    SYMBOL ctor )               // - constructor SYMBOL
{
    if( SymClassCorrupted( ctor ) ) {
        return;
    }
    returnThis( ctor );
}

// Deletion of array of the class is done when destructor is virtual and
// the class is not abstract.
//
static void genDeleteVector(
    SYMBOL dtor,
    CGLABEL *end )
{
    PTREE stmt;
    TYPE class_type;

    *end = CgFrontLabelCs();
    if( SymIsVirtual( dtor ) ) {
        class_type = SymClass( dtor );
        if( ! TypeAbstract( class_type ) ) {
            CgFrontCode( IC_DTOR_DAR_BEG );
            testExtraParm( DTOR_DELETE_VECTOR, FALSE );
            stmt = NodeUnary( CO_DELETE_ARRAY, NodeThis() );
            stmt->flags |= PTF_MEMORY_EXACT;
            stmt = AnalyseDelete( stmt, TRUE );
            EmitAnalysedStmt( stmt );
            CgFrontGotoNear( IC_LABEL_CS, O_GOTO, *end );
            CgFrontCode( IC_CDARG_LABEL );
            CgFrontCode( IC_DTOR_DAR_END );
        }
    }
}


static void dtorSubObject(      // PROCESS DTOR FOR SUB-OBJECT
    SYMBOL sub_dtor )           // - dtor for sub-object
{
    SymMarkRefed( sub_dtor );
    CgFrontScopeCall( NULL, sub_dtor, DTORING_SCOPE );
    if( ! ( sub_dtor->flag & SF_NO_LONGJUMP ) ) {
        FunctionMightThrow();
    }
}


void DtorPrologue(              // GENERATE PROLOGUE FOR DTOR
    SYMBOL dtor,                // - DTOR
    CGLABEL *real_end,          // - returns label for end of dtor
    CGFILE_INS *rtbeg )         // - position for run-time call
{
    SCOPE scope;                // - scope for class
    TYPE host_class;            // - type for class
    CLASSINFO *info;            // - class information
    CD_DESCR* optinfo;          // - traversal data
    CDOPT_ITER* iter;           // - iterator for traversal data
    boolean regster;            // - TRUE ==> function needs registration
    TYPE class_type;            // - type for class

    rtbeg = rtbeg;              // - can remove argument
    if( SymClassCorrupted( dtor ) ) {
        return;
    }
    genDeleteVector( dtor, real_end );
    scope = SymScope( dtor );
    if( ScopeHasVirtualFunctions( scope ) ) {
        host_class = ScopeClass( scope );
        info = host_class->u.c.info;
        info->dtor_user_code_checked = TRUE;
        if( info->dtor_user_code ) {
            genCtorDispInit( scope );
            genVFPtrInits( scope, dtor );
        }
    }
    class_type = ScopeClass( scope );
    optinfo = CDoptDtorBuild( class_type );
    if( CDoptErrorOccurred( optinfo ) ) {
        return;
    }
    CDoptChkAccFun( optinfo );
    iter = CDoptIterBeg( optinfo );
    regster = FALSE;
    for(;;) {
        unsigned depth;
        TITER iter_status = CDoptIterNextComp( iter );
        if( iter_status == TITER_NONE ) break;
        depth = 0;
        for(;;) {
//            boolean is_exact;
            TITER sub_status = CDoptIterNextElement( iter );
            switch( sub_status ) {
              case TITER_NONE :
                if( depth == 0 ) break;
                -- depth;
                continue;
              case TITER_MEMB :
              case TITER_DBASE :
              case TITER_VBASE :
                ++ depth;
                // drops thru
              case TITER_CLASS_EXACT :
              case TITER_CLASS_DBASE :
              case TITER_CLASS_VBASE :
//                is_exact = CDoptIterExact( iter );
                dtorSubObject( CDoptIterFunction( iter ) );
                regster = TRUE;
                continue;
              case TITER_ARRAY_EXACT :
              case TITER_ARRAY_VBASE :
              { TYPE artype = CDoptIterType( iter );
                dtorSubObject( RoDtorFindType( artype ) );
                regster = TRUE;
              } continue;
              DbgDefault( "DtorEpilogue -- bad TITER" );
            }
            break;
        }
    }
    CDoptIterEnd( iter );
    if( regster ) {
        FunctionHasRegistration();
        ScopeKeep( GetCurrScope() );
    }
    // don't hide this in if( regster ) because there may be other cases
    // where registration is req'd (the IC code handler will deal with this)
    CgFrontCode( IC_DTOR_REG );
}

// Deletion is embedded within the destructor when the class is not abstract
// and the destructor is virtual
//
static void genDeleteThis( SYMBOL dtor )
{
    PTREE stmt;

    if( ! TypeAbstract( SymClass( dtor ) ) && SymIsVirtual( dtor ) ) {
        CgFrontCode( IC_DTOR_DLT_BEG );
        testExtraParm( DTOR_DELETE_THIS, FALSE );
        stmt = NodeUnary( CO_DELETE, NodeThis() );
        stmt->flags |= PTF_MEMORY_EXACT;
        stmt = AnalyseDelete( stmt, TRUE );
        EmitAnalysedStmt( stmt );
        CgFrontCode( IC_CDARG_LABEL );
        CgFrontCode( IC_DTOR_DLT_END );
    }
}


void DtorEpilogue(              // GENERATE EPILOGUE FOR DTOR
    SYMBOL dtor,                // - DTOR
    CGLABEL end_dtor,           // - label for end of dtor
    CGFILE_INS *rtbeg )         // - position for run-time call
{
    SCOPE scope;
    TYPE class_type;

    rtbeg = rtbeg;
    if( SymClassCorrupted( dtor ) ) {
        return;
    }
    scope = SymScope( dtor );
    class_type = ScopeClass( scope );
    CgFrontCode( IC_FUNCTION_EPILOGUE );
    if( TypeHasDtorableObjects( class_type ) ) {
        CgFrontCodePtr( IC_DTOR_SUBOBJS, class_type );
    }
    genDeleteThis( dtor );
    CgFrontCode( IC_DTOR_DEREG );
    CgFrontCode( IC_DTOR_DAR_BEG );
    CgFrontLabdefCs( end_dtor );
    CgFrontCode( IC_DTOR_DAR_END );
    CgFrontLabfreeCs( 1 );
    returnThis( dtor );
}
