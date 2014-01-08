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
* Description:  Resolve operator overloading.
*
****************************************************************************/


#include "plusplus.h"
#include "cgfront.h"
#include "fnovload.h"
#include "memmgr.h"
#include "name.h"
#include "ring.h"
#include "fold.h"
#include "initdefs.h"
#include "pcheader.h"
#include "stats.h"
#include "class.h"
#include "template.h"


typedef enum                    // Used to control scalar operand types
{   GETOP_CONST     = 0x01      // - make type const
,   GETOP_NOVOID    = 0x02      // - don't allow void *
,   GETOP_VOLATILE  = 0x04      // - make type volatile
,   GETOP_DEFAULT   = 0x00      // - default behaviour
} GETOP_CONTROL;

#define OPM_NOAMB        ( OPM_QUEST  | OPM_FUN )


#define OPPRO( code ) __PASTE( OPPRO_NO_, code )
typedef enum
#include "ppopscnv.h"
OP_PROTO_NO;
#undef OPPRO

#define OPPRO( code ) __PASTE( OPM_, code ) = 1 << __PASTE( OPPRO_NO_, code )
typedef enum
#include "ppopscnv.h"
OP_MASK;
#undef OPPRO

#define OPBASIC( arg1, arg2, mask ) NULL
static SYMBOL ovfuns[] =
#include "ppopscnv.h"
;
#undef OPBASIC

#define MAX_FUN_PROTOS ( sizeof(ovfuns) / sizeof(ovfuns[0]) )


#define OPBASIC( arg1, arg2, mask ) mask
static OP_MASK opfun_mask[] =
#include "ppopscnv.h"
;
#undef OPBASIC

#define OPBASIC( arg1, arg2, mask ) { __PASTE( TYP_, arg1 ) , \
                                      __PASTE( TYP_, arg2 ) }
static type_id op_basic_arg[][2] =
#include "ppopscnv.h"
;
#undef OPBASIC

#define OPCNV( code, protos ) protos
static OP_MASK opr_masks[] =
#include "ppopscnv.h"
;
#undef OPCNV

#define MAX_OPR_TYPES ( sizeof( fun_protos ) / sizeof( fun_protos[0] ) )

// If we get an ambiguity between an user-defined operator and a built-in
// operator, for Microsoft compatibility we try again, but this time use
// four void * functions in cv-pairs
// i.e. (void *, void*)
//      (const void *, const void*)
//      (volatile void *, volatile void*)
//      (const volatile void *, const volatile void*)
// but this time check for cv-qualification matches, and rank accordinly
// this appears to be what Microsoft does -- LMW
#define OPBASIC_EXTRA( arg1, arg2, mask, ctl ) NULL
static SYMBOL ovfuns_extra[] =
#include "ppopscnv.h"
;
#undef OPBASIC_EXTRA

#define MAX_FUN_PROTOS_EXTRA ( sizeof(ovfuns_extra) / sizeof(ovfuns_extra[0]) )

#define OPBASIC_EXTRA( arg1, arg2, mask, ctl ) mask
static OP_MASK opfun_mask_extra[] =
#include "ppopscnv.h"
;
#undef OPBASIC_EXTRA

#define OPBASIC_EXTRA( arg1, arg2, mask, ctl ) { __PASTE( TYP_, arg1 ) , \
                                                 __PASTE( TYP_, arg2 ) }
static type_id op_basic_arg_extra[][2] =
#include "ppopscnv.h"
;
#undef OPBASIC_EXTRA

#define OPBASIC_EXTRA( arg1, arg2, mask, ctl ) ctl
static GETOP_CONTROL extra_ctl[] =
#include "ppopscnv.h"
;
#define OPM_REF_MASK (OPM_RA|OPM_RI|OPM_RP)

typedef struct {                    // OVOP -- operand types
    PTREE operand;                  // - operand
    TYPE node_type;                 // - NodeType for node
    TYPE user_type;                 // - NULL, class, enum type
    TYPE class_type;                // - NULL, class type
} OVOP;

typedef struct {                    // OLINF -- overload information
    PTREE expr;                     // - expression
    OVOP left;                      // - operand classification: left
    OVOP right;                     // - operand classification: right
    SYMBOL scalars;                 // - scalars list
    SEARCH_RESULT *result_mem;      // - search result (member)
    SEARCH_RESULT *result_nonmem;   // - search result (non-member)
    SEARCH_RESULT *result_nonmem_namespace;   // - search result (non-member)
    OP_MASK mask;                   // - mask for operator
    PTO_FLAG flags;                 // - flags for operand
    unsigned scalar_overloadable:1; // - TRUE ==> scalar overloadable
    unsigned repeat_overload    :1; // - TRUE ==> repeat overloading (->)
    unsigned have_class_type    :1; // - TRUE ==> have a class operand
    unsigned have_user_type     :1; // - TRUE ==> have a class,enum operand
} OLINF;


ExtraRptCtr( ctrBoolConv );
ExtraRptCtr( ctrBoolRes );
ExtraRptCtr( ctrOverloads );
ExtraRptCtr( ctrRepeats );
ExtraRptCtr( ctrOverloadOps );
ExtraRptCtr( ctrResolveOps );
ExtraRptCtr( ctrResolveFun );
ExtraRptCtr( ctrResolveNone );
ExtraRptCtr( ctrResolveMember );
ExtraRptCtr( ctrResolveScalar );
ExtraRptCtr( ctrResolveNaked );


static void setupOVOP(          // SETUP OVOP
    OLINF* olinf,               // - overload information
    PTREE node,                 // - node for operand
    OVOP* ovop )                // - to be filled in
{
    TYPE node_type;             // - NodeType for operand
    TYPE user_type;             // - NULL, class, enum type
    TYPE class_type;            // - NULL, class type

    ovop->operand = node;
    class_type = NULL;
    if( NULL == node ) {
        node_type = NULL;
        user_type = NULL;
    } else {
        node_type = NodeType( node );
        user_type = UserDefTypeForType( node_type );
        if( user_type != NULL ) {
            olinf->have_user_type = TRUE;
            if( user_type->id == TYP_CLASS ) {
                olinf->have_class_type = TRUE;
                class_type = BindTemplateClass( class_type, &node->locn,
                                                FALSE );
                class_type = user_type;
            }
        }
    }
    ovop->node_type = node_type;
    ovop->user_type = user_type;
    ovop->class_type = class_type;
}


static bool initOLINF(          // INITIALIZE OVERLOAD INFORMATION
    PTREE node,                 // - overload node
    OLINF* olinf )              // - overload information
{
    unsigned cnv;               // - conversion number for operator
    OP_MASK mask;               // - conversion mask
    bool scov;                  // - TRUE ==> needs scalar overload processing

    olinf->expr = node;
    olinf->flags = PTreeOpFlags( node );
    olinf->have_class_type = FALSE;
    olinf->have_user_type = FALSE;
    setupOVOP( olinf, node->u.subtree[0], &olinf->left );
    cnv = ( olinf->flags & PTO_CNV ) >> PTO_CNV_SHIFT;
    if( cnv == 0 ) {
        mask = 0;
        scov = FALSE;
    } else {
        mask = opr_masks[ cnv - 1 ];
        scov = ( (mask & OPM_NOAMB) == 0 );
    }
    olinf->mask = mask;
    olinf->scalar_overloadable = FALSE;
    olinf->repeat_overload = FALSE;
    if( scov ) {
        olinf->scalar_overloadable = TRUE;
    }
    return scov || ( olinf->flags & PTO_OVLOAD );
}


static PTREE build_fun_name(    // BUILD NAME FOR A FUNCTION
    SEARCH_RESULT *result )     // - search result
{
    SYMBOL_NAME sym_name;

    sym_name = result->sym_name;
    return( NodeSymbolNoRef( PTreeId( sym_name->name )
                           , sym_name->name_syms
                           , result ) );
}

enum                            // Used for ":" overloading
{   COLON_OP1       = 0x01      // - use( type1, type1 )
,   COLON_OP2       = 0x02      // - use( type2, type2 )
,   COLON_NOTSAME2  = 0x04      // - use( type2, type2 ) if type1 != type2
,   COLON_NOTREF1   = 0x08      // - use( type2, type2 ) if ref(1) != type2
,   COLON_NOTREF2   = 0x10      // - use( type2, type2 ) if type1 ! refd(2)
};

static uint_8 colonTable[3][3]= // Used to determine overloading for ":"
{   COLON_OP1 | COLON_NOTSAME2  // - ( ref : ref )
,   COLON_OP1 | COLON_NOTREF1   // - ( ref : class )
,   COLON_OP1                   // - ( ref : other )
,   COLON_OP1 | COLON_NOTREF2   // - ( class : ref )
,   COLON_OP1 | COLON_NOTSAME2  // - ( class : class )
,   COLON_OP1                   // - ( class : other )
,   COLON_OP2                   // - ( other : ref )
,   COLON_OP2                   // - ( other : ref )
,   0                           // - ( other : other )
};

enum                            // Used to check for enumeration operands
{   ENUM_IGNORE                 // - always include
,   ENUM_MATCH_1                // - include if oper op(1) matches enum(1)
,   ENUM_MATCH_2                // - include if oper op(2) matches enum(2)
,   ENUM_MATCH_1_FUN            // - include if func op(1) matches enum(1)
,   ENUM_MATCH_2_FUN            // - include if func op(2) matches enum(2)
,   ENUM_MATCH_BOTH             // - include if oper op(1,2) matches enum(1,2)
,   ENUM_MATCH_MAX              // - include if oper op(1,2) matches max.enum.s
};


static unsigned colonIndex(     // COMPUTE INDEX FOR COLON OVERLOADING
    TYPE type )                 // - type of an operand
{
    unsigned index;             // - 0 == reference, 1 == class, 2 == other

    if( TypeReference( type ) ) {
        index = 0;
    } else if( StructType( type ) ) {
        index = 1;
    } else {
        index = 2;
    }
    return index;
}


static void colonFun(           // SET UP SCALAR FOR COLON OVERLOADING
    OLINF* olinf,               // - overload information
    unsigned curr,              // - function index
    TYPE type )                 // - overloading type
{
    SYMBOL fun;                 // - function in scalar-overload list
    arg_list *alist;            // - arguments for function

    fun = ovfuns[ curr ];
    alist = SymFuncArgList( fun );
    alist->type_list[0] = type;
    alist->type_list[1] = type;
    RingAppend( &olinf->scalars, fun );
}


static void overloadColon(      // OVERLOAD COLON OPERATOR
    OLINF* olinf,               // - overload information
    unsigned curr )             // - current index
{
    TYPE type1;                 // - NodeType( operand[1] )
    TYPE type2;                 // - NodeType( operand[2] )
    uint_8 mask;                // - mask for overloading type

    type1 = olinf->left.node_type;
    type2 = olinf->right.node_type;
    mask = colonTable[ colonIndex( type1 ) ][ colonIndex( type2 ) ];
    if( mask & COLON_OP1 ) {
        colonFun( olinf, curr, type1 );
    }
    ++curr;
    if( mask & COLON_OP2 ) {
        colonFun( olinf, curr, type2 );
    } else if( mask & COLON_NOTSAME2 ) {
        if( ! TypesIdentical( type1, type2 ) ) {
            colonFun( olinf, curr, type2 );
        }
    } else if( mask & COLON_NOTREF1 ) {
        if( ! TypesIdentical( TypeReferenced( type1 ), type2 ) ) {
            colonFun( olinf, curr, type2 );
        }
    } else if( mask & COLON_NOTREF2 ) {
        if( ! TypesIdentical( type1, TypeReferenced( type2 ) ) ) {
            colonFun( olinf, curr, type2 );
        }
    }
}


static void scalarOperators(    // FIND SYMBOLS FOR SCALAR OPERATORS
    OLINF* olinf )              // - overload information
{
    unsigned curr;              // - current item
    PTREE right;                // - NULL or right node if used in overloading

    olinf->scalars = NULL;
    olinf->result_mem = NULL;
    olinf->result_nonmem = NULL;
    olinf->result_nonmem_namespace = NULL;
    right = NULL;
    if( olinf->flags & PTO_BINARY ) {
        if( (olinf->mask & OPM_LT) == 0 ) {
            right = olinf->expr->u.subtree[1];
        }
    }
    setupOVOP( olinf, right, &olinf->right );
    if( olinf->scalar_overloadable ) {
        if( olinf->mask & OPM_RR ) {
            if( olinf->have_class_type ) {
                for( curr = 0; curr < MAX_FUN_PROTOS; ++ curr ) {
                    OP_MASK fun_mask;
                    fun_mask = opfun_mask[ curr ];
                    if( fun_mask & OPM_RR ) {
                        overloadColon( olinf, curr );
                    } else if( olinf->mask & fun_mask ) {
                        RingAppend( &olinf->scalars, ovfuns[ curr ] );
                    }
                }
            }
        } else {
            if( olinf->have_user_type ) {
                bool complex_assign = ( olinf->mask & OPM_ASSIGN ) && ( olinf->left.class_type != NULL );
                if( !complex_assign || ( complex_assign && (olinf->mask & OPM_REF_MASK) ) ) {
                    for( curr = 0; curr < MAX_FUN_PROTOS; ++ curr ) {
                        OP_MASK fun_mask;
                        fun_mask = opfun_mask[ curr ];
                        if( olinf->mask & fun_mask ) {
                            if( ( olinf->mask & fun_mask ) != OPM_ASSIGN ) {
                            // not just assign matching
                                RingAppend( &olinf->scalars, ovfuns[ curr ] );
                            }
                        }
                    }
                }
            }
        }
    }
}


static CNV_DIAG diag_transform =// diagnosis for transformation
{   ERR_CALL_WATCOM
,   ERR_CALL_WATCOM
,   ERR_CALL_WATCOM
,   ERR_CALL_WATCOM
,   ERR_CALL_WATCOM
};


static CNV_RETN transform_operand(// TRANSFORM AN OPERAND (TO SCALAR)
    PTREE *a_operand,           // - addr( operand )
    TYPE type,                  // - target type
    OP_MASK mask )              // - operator mask
{
    TYPE nd_type;               // - node type
    CNV_RETN cnv_status;        // - conversion status

    nd_type = (*a_operand)->type;
    if( ( NULL != PointerTypeEquivalent( type ) || MemberPtrType( type ) )
      && ( 0 == ( mask & OPM_RR ) ) ) {
        type = UdcFindType( nd_type, type );
    }
    if( ( type != NULL ) && ( NULL == EnumType( nd_type ) ) ) {
        *a_operand = CastImplicit( *a_operand, type, CNV_INIT_COPY, &diag_transform );
        cnv_status = ( PT_ERROR == (*a_operand)->op ) ? CNV_ERR : CNV_OK;
    } else {
        cnv_status = CNV_OK;
    }
    return cnv_status;
}


static PTREE transform_conversions( // TRANSFORM OPERAND(S) BY CONVERSIONS
    OLINF* olinf,               // - overload information
    SYMBOL dummy )              // - dummy symbol for conversion
{
    PTREE node;                 // - node for operation
    OP_MASK mask;               // - operator mask
    arg_list *alist;            // - prototype for function
    unsigned error_right;       // - error occurred on right operand
    unsigned error_left;        // - error occurred on left operand

    node = olinf->expr;
    mask = olinf->mask;
    alist = SymFuncArgList( dummy );
    if( mask & OPM_ASSIGN ) {
        error_right = transform_operand( &node->u.subtree[1]
                                       , alist->type_list[1]
                                       , mask );
        error_left = transform_operand( &node->u.subtree[0]
                                      , alist->type_list[0]
                                      , mask );
    } else {
        error_left = transform_operand( &node->u.subtree[0]
                                      , alist->type_list[0]
                                      , mask );
        if( alist->num_args > 1 && (mask & OPM_POST) == 0 ) {
            error_right = transform_operand( &node->u.subtree[1]
                                           , alist->type_list[1]
                                           , mask );
        } else {
            error_right = CNV_OK;
        }
    }
    if( ( error_right != CNV_OK ) || ( error_left != CNV_OK ) ) {
        PTreeErrorNode( node );
    }
    return node;
}

// did this symbol come from this search result ?
static bool symInResult( SYMBOL sym, SEARCH_RESULT *result )
{
    SYMBOL      curr;
    SYM_REGION  *ptr;

    RingIterBeg( result->sym_name->name_syms, curr ) {
        if( curr == sym ) {
            return TRUE;
        } else if( SymIsFunctionTemplateModel( curr ) ) {
            FN_TEMPLATE_INST *fn_inst;

            RingIterBeg( curr->u.defn->instantiations, fn_inst ) {
                if( fn_inst->bound_sym == sym ) {
                    return TRUE;
                }
            } RingIterEnd( fn_inst )
        }
    } RingIterEnd( curr )
    RingIterBeg( result->region, ptr ) {

        RingIterBegFrom( ptr->from, curr ) {
            if( curr == sym ) {
                return TRUE;
            }

        } RingIterEndTo( curr, ptr->to )

    } RingIterEnd( ptr )
    return FALSE;
}

static PTREE transform_to_call( // TRANSFORM NODE TO FUNCTION CALL
    PTREE node,                 // - type node
    PTREE left,                 // - left operand
    PTREE right )               // - right operand
{
    node->u.subtree[0] = left;
    node->u.subtree[1] = right;
    node->cgop = CO_CALL_NOOVLD;
    node->op = PT_BINARY;
    node->flags = 0;
    return( node );
}


static PTREE transform_naked(   // TRANSFORM TO CALL TO NON-MEMBER FUNCTION
    OLINF* olinf,
    SYMBOL fun )                // - overload information
{
    CGOP cgop;                  // - expression operator
    PTREE param;                // - parameters node
    PTREE retn;                 // - resultant call expression
    SEARCH_RESULT *result;

    cgop = olinf->expr->cgop;
    param = NodeArg( olinf->left.operand );
    if( olinf->flags & PTO_BINARY ) {
        if( cgop == CO_CALL ) {
            param->u.subtree[0] = olinf->right.operand;
        } else {
            if( olinf->right.operand != NULL ) {
                param = NodeArgument( param, olinf->right.operand );
            } else {
                PTreeErrorNode( param );
            }
        }
    } else if( cgop == CO_POST_PLUS_PLUS
            || cgop == CO_POST_MINUS_MINUS ) {
        param = NodeArgument( param, NodeZero() );
    }
    result = NULL;
    if( olinf->result_nonmem != NULL ) {
        if( olinf->result_nonmem_namespace == NULL ) {
            result = olinf->result_nonmem;
        } else {
            // both non-null
            if( symInResult( fun, olinf->result_nonmem ) ) {
                result = olinf->result_nonmem;
                ScopeFreeResult( olinf->result_nonmem_namespace );
            } else {
                result = olinf->result_nonmem_namespace;
                ScopeFreeResult( olinf->result_nonmem );
            }
        }
    } else {
        result = olinf->result_nonmem_namespace;
    }
    DbgAssert( symInResult( fun, result ) );
    retn = transform_to_call( olinf->expr
                            , build_fun_name( result )
                            , param );
    retn = PTreeCopySrcLocation( retn, olinf->expr );
    return retn;
}


static PTREE transform_member(  // TRANSFORM TO CALL TO MEMBER FUNCTION
    OLINF* olinf )              // - overload information
{
    PTREE op;                   // - original operator
    PTREE param;                // - parameter list
    PTREE caller;               // - caller node
    SYMBOL op_sym;              // - symbol for overloaded op
    CGOP cgop;                  // - expression operator

    op = olinf->expr;
    cgop = op->cgop;
    if( cgop == CO_ARROW ) {
        op_sym = olinf->result_mem->sym_name->name_syms;
        if( ! ArrowMemberOK( op_sym ) ) {
            PTreeErrorExprSym( op, ERR_OPERATOR_ARROW_RETURN_BAD, op_sym );
            return op;
        }
        caller = NodeDottedFunction( olinf->left.operand
                                   , build_fun_name( olinf->result_mem ) );
        caller = PTreeCopySrcLocation( caller, op );
        caller = NodeBinary( CO_CALL, caller, NULL );
        caller = PTreeCopySrcLocation( caller, op );
        caller = AnalyseCall( caller, NULL );
        op->u.subtree[0] = caller;
        if( caller->op == PT_ERROR ) {
            PTreeErrorNode( op );
        } else {
            olinf->repeat_overload = TRUE;
        }
    } else {
        if( olinf->flags & PTO_BINARY ) {
            param = olinf->right.operand;
            if( cgop != CO_CALL ) {
                param = NodeArg( param );
            }
        } else {
            if( ( cgop == CO_POST_PLUS_PLUS   )
              ||( cgop == CO_POST_MINUS_MINUS ) ) {
                param = NodeArg( NodeZero() );
            } else {
                param = NULL;
            }
        }
        caller = NodeDottedFunction( olinf->left.operand
                                   , build_fun_name( olinf->result_mem ) );
        caller = PTreeCopySrcLocation( caller, op );
        op = transform_to_call( op, caller, param );
        op = PTreeCopySrcLocation( op, olinf->expr );
    }
    return op;
}


static PTREE resolve_symbols(   // RESOLVE MULTIPLE OVERLOAD DEFINITIONS
    OLINF* olinf )              // - overload information
{
    PTREE ptlist[2];            // - PT list for all but call
    PTREE zero_node;            // - , 0 ) parm for @++ and @-- overloading
    FNOV_RESULT ovret;          // - overload return
    unsigned count;             // - # of arguments
    SYMBOL fun;                 // - overloaded function to use
    struct                      // - arg.list
    {   arg_list    base;       // - - hdr, entry[1]
        TYPE        arg2;       // - - entry[2]
    } alist;
    FNOV_DIAG fnov_diag;        // - diagnosis information

    ExtraRptIncrementCtr( ctrResolveOps );
    if( olinf->mask & OPM_FUN ) {
        PTREE oper;
        ExtraRptIncrementCtr( ctrResolveFun );
        fun = olinf->result_mem->sym_name->name_syms;
        oper = PTreeIdSym( fun );
        oper = NodeSymbolNoRef( oper, fun, olinf->result_mem );
        oper->cgop = CO_NAME_DOT;
        olinf->expr->u.subtree[0] = NodeDottedFunction( olinf->left.operand
                                                      , oper );

        ScopeFreeResult( olinf->result_nonmem );
        ScopeFreeResult( olinf->result_nonmem_namespace );
        return olinf->expr;
    }
    InitArgList( &alist.base );
    zero_node = NULL;
    ptlist[0] = olinf->left.operand;
    alist.base.type_list[ 0 ] = olinf->left.node_type;
    if( olinf->flags & PTO_BINARY ) {
        if( olinf->mask & OPM_ASSIGN ) {
            ptlist[1] = olinf->right.operand;
            alist.base.type_list[ 1 ] = olinf->right.node_type;
            count = 2;
        } else if( olinf->mask & OPM_LT ) {
            count = 1;
        } else {
            if( ( olinf->mask & OPM_SUB ) && ( olinf->result_mem != NULL ) ) {
                olinf->scalars = NULL;
            }
            ptlist[1] = olinf->right.operand;
            alist.base.type_list[ 1 ] = olinf->right.node_type;
            count = 2;
        }
    } else if( olinf->mask & OPM_POST ) {
        zero_node = NodeZero();
        setupOVOP( olinf, zero_node, &olinf->right );
        ptlist[1] = olinf->right.operand;
        alist.base.type_list[ 1 ] = olinf->right.node_type;
        count = 2;
    } else {
        count = 1;
    }
    alist.base.num_args = count;
    // this qualifier is handled explicitly by the first entry in the arg_list
    ovret = OpOverloadedDiag( &fun
                        , olinf->result_mem
                        , olinf->result_nonmem
                        , olinf->result_nonmem_namespace
                        , olinf->scalars
                        , (arg_list*)&alist
                        , ptlist
                        , &fnov_diag );
    if( ovret == FNOV_AMBIGUOUS && CompFlags.extensions_enabled ) {
        FNOV_LIST* amb_list;    // - ambiguity list
        SYMBOL next;            // - next symbol
        bool have_user_defined = FALSE;
        bool have_void = FALSE;
        if( ( (olinf->mask & OPM_NV) == 0 ) && (olinf->mask & OPM_PP) ) {
            // we're in the (void *, void *) area
            for( amb_list = NULL; ; ) {
                next = FnovGetAmbiguousEntry( &fnov_diag, &amb_list );
                if( next == NULL ) break;
                if( next->name != NULL ) {
                    have_user_defined = TRUE;
                } else if( op_basic_arg[ next->u.scalar_order ][ 0 ] ==
                           TYP_POINTER
                        && op_basic_arg[ next->u.scalar_order ][ 1 ] ==
                           TYP_POINTER ) {
                    // the (void *, void *) was one of the ambiguous ones
                    have_void = TRUE;
                }
                if( have_user_defined && have_void ) {
                    break;
                }
            }
        }
        if( have_user_defined && have_void ) { // try again
            int i;
            FnovFreeDiag( &fnov_diag );
            for( i = 0; i < MAX_FUN_PROTOS_EXTRA; i++ ) {
                OP_MASK fun_mask;
                fun_mask = opfun_mask_extra[ i ];
                if( olinf->mask & fun_mask ) {
                    RingAppend( &olinf->scalars, ovfuns_extra[ i ] );
                }
            }
            ovret = OpOverloadedLimitDiag( &fun
                                , olinf->result_mem
                                , olinf->result_nonmem
                                , olinf->result_nonmem_namespace
                                , olinf->scalars
                                , (arg_list*)&alist
                                , ptlist
                                , FNC_STDOP_CV_VOID
                                , &fnov_diag );
        }
    }
    if( ovret == FNOV_AMBIGUOUS && CompFlags.overload_13332 ) {
        FNOV_LIST* amb_list;    // - ambiguity list
        SYMBOL next;            // - next symbol
        bool have_user_defined = FALSE;
        for( amb_list = NULL; ; ) {
            next = FnovGetAmbiguousEntry( &fnov_diag, &amb_list );
            if( next == NULL ) break;
            if( next->name != NULL ) {
                have_user_defined = TRUE;
                break;
            }
        }
        if( have_user_defined ) {
            FnovFreeDiag( &fnov_diag );
            ovret = OpOverloadedLimitDiag( &fun
                                , olinf->result_mem
                                , olinf->result_nonmem
                                , olinf->result_nonmem_namespace
                                , olinf->scalars
                                , (arg_list*)&alist
                                , ptlist
                                , FNC_USE_WP13332
                                , &fnov_diag );
        }
    }
    switch( ovret ) {
      case FNOV_AMBIGUOUS:
      { FNOV_LIST* amb_list;    // - ambiguity list
        SYMBOL next;            // - next symbol
        fun = NULL;
        for( amb_list = NULL; ; ) {
            next = FnovGetAmbiguousEntry( &fnov_diag, &amb_list );
            if( next == NULL ) break;
            if( next->name == NULL
             && next->id != SC_MEMBER
             && next->u.scalar_order < ( MAX_FUN_PROTOS - 2 ) ) {
                if( fun == NULL
                 || fun->u.scalar_order < next->u.scalar_order ) {
                    fun = next;
                }
            } else {
                fun = NULL;
                break;
            }
        }
        if( fun == NULL ) {
            PTreeErrorExpr( olinf->expr, ERR_OPERATOR_AMBIGUOUS_OVERLOAD );
            for( ; ; ) {
                SYMBOL reject = FnovNextAmbiguousEntry( &fnov_diag );
                if( reject == NULL ) break;
                if( reject->name == NULL ) {
                    CErr2p( INF_CONV_AMBIG_SCALAR
                          , SymFuncArgList( reject )->type_list[0] );
                } else {
                    InfSymbolAmbiguous( reject );
                }
            }
            ScopeFreeResult( olinf->result_mem );
            ScopeFreeResult( olinf->result_nonmem );
            ScopeFreeResult( olinf->result_nonmem_namespace );
            break;
        }
        // drops thru
      }
      case FNOV_NONAMBIGUOUS :
        if( fun->id == SC_MEMBER ) {
            ExtraRptIncrementCtr( ctrResolveMember );
            if( olinf->expr->cgop == CO_EQUAL
             && olinf->left.class_type ==
                 ClassTypeForType( SymFuncArgList( fun )->type_list[0] ) ) {
                ScopeFreeResult( olinf->result_mem );
            } else {
                olinf->expr = transform_member( olinf );
            }
            ScopeFreeResult( olinf->result_nonmem );
            ScopeFreeResult( olinf->result_nonmem_namespace );
        } else if( fun->name == NULL ) {
            ExtraRptIncrementCtr( ctrResolveScalar );
            olinf->expr = transform_conversions( olinf, fun );
            ScopeFreeResult( olinf->result_mem );
            ScopeFreeResult( olinf->result_nonmem );
            ScopeFreeResult( olinf->result_nonmem_namespace );
        } else {
            ExtraRptIncrementCtr( ctrResolveNaked );
            olinf->expr = transform_naked( olinf, fun );
            ScopeFreeResult( olinf->result_mem );
        }
        break;
      case FNOV_NO_MATCH:
        ExtraRptIncrementCtr( ctrResolveNone );
        ScopeFreeResult( olinf->result_mem );
        ScopeFreeResult( olinf->result_nonmem );
        ScopeFreeResult( olinf->result_nonmem_namespace );
        break;
    }
    if( zero_node != NULL ) {
        PTreeFree( zero_node );
    }
    FnovFreeDiag( &fnov_diag );
    return olinf->expr;
}

static SCOPE nsExtract( OLINF *inf )
{
    TYPE user_type;
    SCOPE scope;

    user_type = inf->left.user_type;
    if( user_type == NULL ) {
        user_type = inf->right.user_type;
    }
    if( user_type->id == TYP_CLASS ) {
        scope = user_type->u.c.scope;
    } else {
        DbgAssert( user_type->id == TYP_ENUM );
        scope = user_type->u.t.scope;
    }
    scope = ScopeEnclosingId( scope, SCOPE_FILE );
    if( scope == GetFileScope() ) {
        scope = NULL;
    }
    return( scope );
}


PTREE OverloadOperator(         // HANDLE OPERATOR OVERLOADING, IF REQ'D
    PTREE op )                  // - node containing operator
{
    OLINF oli;                  // - overload info
    SCOPE enclosing;

    ExtraRptIncrementCtr( ctrOverloads );
    for( ; ; ) {
        if( ! initOLINF( op, &oli ) ) break;
        scalarOperators( &oli );
        if( ! oli.have_user_type ) break;
        if( oli.flags & PTO_OVLOAD ) {
            NAME ov_fun_name;           // - name of overloaded function
            TYPE type;                  // - class type
            SCOPE scope;                // - class scope
            CGOP cgop;                  // - expression operator

            ExtraRptIncrementCtr( ctrOverloadOps );
            cgop = oli.expr->cgop;
            type = oli.left.class_type;
            if( type == NULL ) {
                if( ( 0 == ( oli.mask & ( OPM_QUEST | OPM_FUN | OPM_SUB ) ) )
                 && cgop != CO_EQUAL ) {
                    SCOPE scope;
                    ov_fun_name = CppOperatorName( cgop );
                    oli.result_nonmem = ScopeFindNaked( GetFileScope(), ov_fun_name );
                    scope = nsExtract( &oli );
                    if( scope != NULL ) {
                        oli.result_nonmem_namespace = ScopeContainsNaked( scope, ov_fun_name );
                    }
                }
            } else {
                scope = TypeScope( type );
                ov_fun_name = CppOperatorName( cgop );
                if( cgop == CO_EQUAL ) {
                    ClassDefaultOpEq( type, type );
                    oli.result_mem = ScopeContainsMember( scope, ov_fun_name );
                } else {
                    oli.result_mem = ScopeFindMember( scope, ov_fun_name );
                    oli.result_nonmem = ScopeFindNaked( GetFileScope(), ov_fun_name );
                    enclosing = nsExtract( &oli );
                    if( enclosing != NULL ) {
                        oli.result_nonmem_namespace = ScopeContainsNaked( enclosing, ov_fun_name );
                    }
                }
            }
            if( oli.result_mem == NULL
             && oli.result_nonmem == NULL
             && oli.result_nonmem_namespace == NULL ) {
                if( oli.scalars == NULL ) break;
                if( ! oli.have_class_type ) break;
            }
            op = resolve_symbols( &oli );
            if( ! oli.repeat_overload ) break;
            if( op->op == PT_ERROR ) break;
            ExtraRptIncrementCtr( ctrRepeats );
        } else {
            if( NULL == oli.scalars ) break;
            op = resolve_symbols( &oli );
            break;
        }
    }
    return op;
}


static bool isBadFun(           // DIAGNOSE IF MEMBER FUNC. OR OVERLOADED
    PTREE expr,                 // - expression being analysed
    PTREE operand )             // - operand to be diagnosed
{
    bool retn;                  // - return: TRUE ==> diagnosed
    PTREE fnode;                // - function node
    PTREE node;                 // - node to be examined

    node = PTreeOp( &operand );
    switch( NodeAddrOfFun( node, &fnode ) ) {
      default :
        retn = FALSE;
        break;
      case ADDR_FN_ONE :
        if( SymIsThisFuncMember( fnode->u.symcg.symbol ) ) {
            PTreeErrorExprSymInf( node
                                , ERR_ADDR_NONSTAT_MEMBER_FUNC
                                , fnode->u.symcg.symbol );
            PTreeErrorNode( expr );
            retn = TRUE;
        } else {
            retn = FALSE;
        }
        break;
      case ADDR_FN_MANY :
        PTreeErrorExprSymInf( node
                            , ERR_ADDR_OF_OVERLOADED_FUN
                            , fnode->u.symcg.symbol );
        PTreeErrorNode( expr );
        retn = TRUE;
        break;
    }
    return retn;
}


PTREE ConvertBoolean(           // CONVERSION TO BOOLEAN EXPRESSION
    PTREE bexpr )               // - the bool expression
{
    PTREE left;                 // - left expression
    OLINF oli;                  // - overload info

    ExtraRptIncrementCtr( ctrBoolConv );
    initOLINF( bexpr, &oli );
    bexpr = oli.expr;
    left = bexpr->u.subtree[0];
    if( NULL != oli.left.user_type ) {
        ExtraRptIncrementCtr( ctrBoolRes );
        scalarOperators( &oli );
        if( NULL != oli.scalars ) {
            bexpr = resolve_symbols( &oli );
            if( ( bexpr->op != PT_ERROR )
              &&( NULL != ClassTypeForType( bexpr->u.subtree[0]->type ) ) ) {
                PTreeErrorExpr( PTreeOpLeft( bexpr ), ERR_NOT_BOOLEAN );
                PTreeErrorNode( bexpr );
                return bexpr;
            }
        }
        if( MemberPtrType( bexpr->u.subtree[0]->type ) ) {
            bexpr = MembPtrCompare( bexpr );
            if( bexpr->op == PT_ERROR ) return bexpr;
        }
        if( PT_BINARY == bexpr->op ) {
            bexpr = FoldBinary( bexpr );
        } else if( PT_UNARY == bexpr->op ) {
            bexpr = FoldUnary( bexpr );
        }
    } else if( NULL != MemberPtrType( left->type ) ) {
        bexpr = MembPtrCompare( bexpr );
    } else if( oli.flags & PTO_BINARY ) {
        if( ! isBadFun( bexpr, bexpr->u.subtree[0] )
         && ! isBadFun( bexpr, bexpr->u.subtree[1] ) ) {
            bexpr = FoldBinary( bexpr );
        }
    } else if( oli.flags & PTO_UNARY ) {
        if( ! isBadFun( bexpr, bexpr->u.subtree[0] ) ) {
            bexpr = FoldUnary( bexpr );
        }
    }
    return bexpr;
}


static TYPE getOperandType(     // GET OPERAND TYPE
    type_id id,                 // - type id
    GETOP_CONTROL ctl )         // - control flavour
{
    TYPE type;                  // - manufactured type

    switch( id ) {
      case TYP_POINTER :
        if( ctl & GETOP_CONST ) {
            if( ctl & GETOP_VOLATILE ) {
                // const volatile
                DbgAssert( (ctl & GETOP_NOVOID) == 0 );
                type = TypePtrToConstVolatileVoidStdop();
            } else {
                // const
                if( ctl & GETOP_NOVOID ) {
                    type = TypePtrToConstVoidStdopArith();
                } else {
                    type = TypePtrToConstVoidStdop();
                }
            }
        } else {
            if( ctl & GETOP_VOLATILE ) {
                // volatile
                DbgAssert( (ctl & GETOP_NOVOID) == 0 );
                type = TypePtrToVolatileVoidStdop();
            } else {
                // neither const nor volatile
                if( ctl & GETOP_NOVOID ) {
                    type = TypePtrToVoidStdopArith();
                } else {
                    type = TypePtrToVoidStdop();
                }
            }
        }
        break;
      case TYP_MEMBER_POINTER :
        type = TypeGetCache( TYPC_VOID_MEMBER_PTR );
        break;
      default :
        type = GetBasicType( id );
        break;
    }
    return( type );
}

static void overloadOperatorInit( // INITIALIZATION
    INITFINI* defn )            // - definition
{
    unsigned arg;               // - current basic argument
    type_id arg1;               // - type of argument(1)
    type_id arg2;               // - type of argument(2)
    TYPE fn_type;               // - full type for function
    TYPE retn_type;             // - return type
    TYPE arg1_type;             // - first argument type
    TYPE arg2_type;             // - second argument type
    SYMBOL fun;                 // - dummy symbol for function
    OP_MASK mask;               // - mask for function

    defn = defn;
    for( arg = 0; arg < MAX_FUN_PROTOS; ++arg ) {
        mask = opfun_mask[arg];
        if( ( mask == 0 ) || ( mask & OPM_RR ) ) {
            arg_list *alist;    // - this gets hammered on overloading
            arg1_type = MakeReferenceTo( GetBasicType( TYP_CHAR ) );
            alist = AllocArgListPerm( 2 );
            alist->type_list[0] = arg1_type;
            alist->type_list[1] = arg1_type;
            fn_type = MakeType( TYP_FUNCTION );
            fn_type->flag |= TF1_PLUSPLUS;
            fn_type->u.f.args = alist;
            fn_type->of = arg1_type;
            CheckUniqueType( fn_type );    // - don't use CheckDupType
        } else {
            GETOP_CONTROL ctl;
            arg1 = op_basic_arg[ arg ][ 0 ];
            retn_type = getOperandType( arg1, GETOP_CONST );
            if( mask & OPM_ASSIGN ) {
                ctl = GETOP_CONST;
            } else {
                ctl = GETOP_DEFAULT;
            }
            if( mask & OPM_NV ) {
                ctl |= GETOP_NOVOID;
            }
            arg1_type = getOperandType( arg1, ctl );
            if( mask & OPM_ASSIGN ) {
                arg1_type = MakeReferenceTo( arg1_type );
                retn_type = MakeReferenceTo( retn_type );
            }
            arg2 = op_basic_arg[ arg ][ 1 ];
            if( arg2 == TYP_MAX ) {
                fn_type = MakeSimpleFunction( retn_type
                                            , arg1_type
                                            , NULL );
            } else {
                ctl = GETOP_DEFAULT;
                if( mask & OPM_NV ) {
                    ctl |= GETOP_NOVOID;
                }
                arg2_type = getOperandType( arg2, ctl );
                fn_type = MakeSimpleFunction( retn_type
                                            , arg1_type
                                            , arg2_type
                                            , NULL );
            }
        }
        fun = AllocSymbol();
        fun->sym_type = fn_type;
        fun->u.scalar_order = arg;
        ovfuns[ arg ] = fun;
    }
    if( CompFlags.extensions_enabled ) {
        // initializes extra function needed to implement extension for
        // Microsoft compatibility
        for( arg = 0; arg < MAX_FUN_PROTOS_EXTRA; ++arg ) {
            mask = opfun_mask_extra[arg];

            arg1 = op_basic_arg_extra[ arg ][ 0 ];
            arg1_type = getOperandType( arg1, extra_ctl[arg] );

            fn_type = MakeSimpleFunction( arg1_type, arg1_type, arg1_type, NULL );
            fun = AllocSymbol();
            fun->sym_type = fn_type;
            fun->u.scalar_order = arg;
            ovfuns_extra[ arg ] = fun;
        }
    }
    ExtraRptRegisterCtr( &ctrBoolConv,      "# Boolean Coversions" );
    ExtraRptRegisterCtr( &ctrBoolRes,       "# Boolean overload resolutions" );
    ExtraRptRegisterCtr( &ctrOverloads,     "# operator overloads" );
    ExtraRptRegisterCtr( &ctrRepeats,       "# repeats of overloading" );
    ExtraRptRegisterCtr( &ctrOverloadOps,   "# operators with C++ overloading" );
    ExtraRptRegisterCtr( &ctrResolveOps,    "# operators requiring resolution" );
    ExtraRptRegisterCtr( &ctrResolveFun,    "# resolved function calls" );
    ExtraRptRegisterCtr( &ctrResolveNone,   "# unresolved overloads" );
    ExtraRptRegisterCtr( &ctrResolveMember, "# resolved by member fun" );
    ExtraRptRegisterCtr( &ctrResolveScalar, "# resolved by scalar conversion" );
    ExtraRptRegisterCtr( &ctrResolveNaked,  "# resolved by file-scope fun" );
}


static void overloadOperatorFini( // COMPLETION
    INITFINI* defn )            // - definition
{
    unsigned arg;               // - current basic argument

    defn = defn;
    for( arg = 0; arg < MAX_FUN_PROTOS; ++arg ) {
        if( ovfuns[ arg ] != NULL ) {
            FreeSymbol( ovfuns[ arg ] );
        }
    }
    if( CompFlags.extensions_enabled ) {
        for( arg = 0; arg < MAX_FUN_PROTOS_EXTRA; ++arg ) {
            if( ovfuns_extra[ arg ] != NULL ) {
                FreeSymbol( ovfuns_extra[ arg ] );
            }
        }
    }
}


INITDEFN( overload_operator, overloadOperatorInit, overloadOperatorFini )

pch_status PCHWriteOperatorOverloadData( void )
{
    unsigned arg;

    for( arg = 0; arg < MAX_FUN_PROTOS; ++arg ) {
        PCHWriteCVIndex( (cv_index)(pointer_int)SymbolGetIndex( ovfuns[ arg ] ) );
    }
    for( arg = 0; arg < MAX_FUN_PROTOS_EXTRA; ++arg ) {
        PCHWriteCVIndex( (cv_index)(pointer_int)SymbolGetIndex( ovfuns_extra[ arg ] ) );
    }
    return( PCHCB_OK );
}

pch_status PCHReadOperatorOverloadData( void )
{
    unsigned arg;

    for( arg = 0; arg < MAX_FUN_PROTOS; ++arg ) {
        ovfuns[arg] = SymbolMapIndex( (SYMBOL)(pointer_int)PCHReadCVIndex() );
    }
    for( arg = 0; arg < MAX_FUN_PROTOS_EXTRA; ++arg ) {
        ovfuns_extra[arg] = SymbolMapIndex( (SYMBOL)(pointer_int)PCHReadCVIndex() );
    }
    return( PCHCB_OK );
}

pch_status PCHInitOperatorOverloadData( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniOperatorOverloadData( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}


#ifndef NDEBUG

#include "dbg.h"
#include <stdio.h>

static void dumpOVOP            // DEBUG -- DUMP OVOP structure
    ( OVOP* ovop                // - OVOP
    , char const *operand )     // - title info
{
    printf( "Operand[%p] (%s)\n"
          , ovop->operand
          , operand );
    printf( "  node_type: " );
    DumpFullType( ovop->node_type );
    printf( "  user_type: " );
    DumpFullType( ovop->user_type );
    printf( "  class_type: " );
    DumpFullType( ovop->class_type );
}

void DumpOpovLoad               // DEBUG -- DUMP ROUTINE
    ( OLINF* oli )              // - overload info
{
    SYMBOL sym;

    DbgPrintPTREE( oli->expr );
    dumpOVOP( &oli->left, "left" );
    dumpOVOP( &oli->right, "right" );
    printf( "\nScalars:" );
    RingIterBeg( oli->scalars, sym ) {
        PrintFullType( sym->sym_type );
    } RingIterEnd( sym );
    printf( "\nSearch results: memb[%p] non-memb[%p]"
            "\nMask[%x] Flags[%x] scalar_overload(%d) repeat_overload(%d)"
            "\nhave_user_type(%d) have_class_type(%d)"
            "\n"
          , oli->result_mem
          , oli->result_nonmem
          , oli->mask
          , oli->flags
          , oli->scalar_overloadable
          , oli->repeat_overload
          , oli->have_class_type
          , oli->have_user_type
          );
}

#endif
