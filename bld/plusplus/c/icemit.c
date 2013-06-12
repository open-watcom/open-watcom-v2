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

#include <stddef.h>

#include "memmgr.h"
#include "stringl.h"
#include "ptree.h"
#include "carve.h"
#include "cgfront.h"
#include "dbg.h"
#include "toggle.h"
#include "conpool.h"
#include "label.h"
#include "codegen.h"
#include "fnbody.h"
#include "pragdefn.h"
#include "rtfuns.h"
#include "specfuns.h"
#include "objmodel.h"

static TOKEN_LOCN posn_gened;   // source position -- generated
static SYMBOL init_ref_temp;    // temp used for auto initialization of refer.


static void generateScopeCall(  // GENERATE SCOPE CALL FOR DIRECT CALL
    PTREE call_expr )           // - call expression
{
    SYMBOL callee;              // - symbol called
    PTREE call_node;            // - original node

    call_node = call_expr;
    call_expr = call_expr->u.subtree[0];
    if( NodeIsUnaryOp( call_expr, CO_CALL_SETUP ) ) {
        call_expr = PTreeOpLeft( call_expr );
        DbgVerify( call_expr->op == PT_SYMBOL
                 , "generateScopeCall -- not symbol" );
        callee = call_expr->u.symcg.symbol;
        if( SPFN_LONGJMP == SpecialFunction( callee ) ) {
            FunctionCouldThrow( call_node );
        } else {
            FunctionCalled( call_node, callee );
            if( ! SymIsCtor( callee )
             && ! SymIsThunkCtorCopy( callee )
             && ! SymIsThunkCtorDflt( callee ) ) {
                TYPE type = SymFuncReturnType( callee );
                call_expr = PtdCtoredExprType( call_node, callee, type );
            }
        }
    } else {
        FunctionCouldThrow( call_node );
    }
}


static void generate_type(      // GENERATE TYPE FOR A NODE
    PTREE node )                // - current node
{
    if( node->flags & PTF_LVALUE ) {
        CgSetTypeExact( node->type );
    } else {
        if( node->flags & PTF_CLASS_RVREF ) {
            CgSetTypeExact( MakeReferenceTo( node->type ) );
        } else {
            CgSetTypeExact( node->type );
        }
    }
}


static TYPE generate_node_type( // GENERATE EXACT TYPE FOR NODE
    PTREE node )                // - current node
{
    TYPE type;

    if( node->flags & PTF_LVALUE ) {
        type = NodeType( node );
    } else {
        if( node->flags & PTF_CLASS_RVREF ) {
            type = MakeReferenceTo( node->type );
        } else {
            type = node->type;
        }
    }
    CgSetTypeExact( type );
    return( type );
}


static void generate_type_instr(// GENERATE TYPE, INSTRUCTION
    TYPE type,                  // - type generated
    unsigned value,             // - value for ic instruction
    CGINTEROP opcode )          // - opcode
{
    CgSetType( type );
    CgFrontCodeUint( opcode, value );
}


static void generate_expr_instr(// GENERATE EXPR-TYPE, INSTRUCTION
    PTREE expr,                 // - current node
    unsigned value,             // - value for ic instruction
    CGINTEROP opcode )          // - opcode
{
    generate_node_type( expr );
    CgFrontCodeUint( opcode, value );
}


static SYMBOL getPtreeSymbol(   // PICK UP SYMBOL FROM PTREE NODE
    PTREE node )                // - the node
{
    SYMBOL sym;                 // - the symbol

    if( NULL == node ) {
        sym = NULL;
    } else {
        sym = node->u.symcg.symbol;
        if( sym != NULL ) {
            sym = SymDeAlias( sym );
        }
    }
    return sym;
}


static SYMBOL getLeftPtreeSymbol( // PICK UP SYMBOL FROM LEFT PTREE NODE
    PTREE node )                // - node at top
{
    return getPtreeSymbol( PTreeOpLeft( node ) );
}


static void generateRargOffset( // GENERATE BINDING FOR REFERENCE ARG., OFFSET
    PTREE arg,                  // - next call argument
    int arg_no,                 // - argument #
    unsigned offset )           // - offset
{
    CgFrontCodePtr( IC_RARG_SYM, getPtreeSymbol( arg ) );
    if( 0 != offset ) {
        CgFrontCodeInt( IC_RARG_OFFSET, offset );
    }
    CgFrontCodeInt( IC_RARG_FUNC, arg_no );
}


static void generateRarg(       // GENERATE BINDING FOR REFERENCE ARG.
    PTREE arg,                  // - next call argument
    int arg_no )                // - argument #
{
    generateRargOffset( arg, arg_no, 0 );
}


static void generateRargVbOffset( // GENERATE BINDING FOR VB-REF ARG., OFFSET
    PTREE arg,                  // - next call argument
    int arg_no,                 // - argument #
    unsigned offset )           // - offset
{
// Note: This optimization is enabled only when the symbol has exact type.
//       This is because the offset is calculated assuming that the item is
//       exact.
//       In the future, we could soup this up (like IC_VB_FETCH) when we
//       know, in the called routine, the exact type of the symbol which
//       is bound to this symbol.
//
//       The symbol does not have known exact type when it is an argument.
    SYMBOL sym = getPtreeSymbol( arg );
    if( NULL != sym
     && ! SymIsArgument( sym ) ) {
        CgFrontCodePtr( IC_RARG_SYM, sym );
//      CgFrontCodeInt( IC_RARG_VBOFFSET, offset );
        CgFrontCodeInt( IC_RARG_OFFSET, offset );
        CgFrontCodeInt( IC_RARG_FUNC, arg_no );
    }
}


static unsigned vbaseDelta(     // GET OFFSET FOR CO_VBASE_FETCH
    PTREE expr )                // - CO_VBASE_FETCH expression
{
    return PtdGetVbExact( expr );
}


static PTREE getChildNode(      // GET CHILD, REMOVE CASTING
    PTREE *a_src )              // - addr[ source ]
{
    PTREE retn;                 // - NULL or child node

    if( NULL == *a_src ) {
        retn = NULL;
    } else {
        retn = NodeRemoveCasts( PTreeOp( a_src ) );
    }
    return retn;
}


static void generateCallRefICs( // GENERATE IC'S FOR REFERENCE PARAMETERS
    PTREE expr )                // - call expression
{
    PTREE arg;                  // - next call argument
    PTREE right;                // - RHS of arg
    int arg_no;                 // - argument #
    unsigned off;               // - offset argument

    for( expr = expr->u.subtree[1], arg_no = 0
       ; expr != NULL
       ; expr = expr->u.subtree[0], ++arg_no ) {
        if( expr->flags & PTF_ARG_THIS
         || NULL != TypeReference( NodeType( expr ) ) ) {
            arg = getChildNode( &expr->u.subtree[1] );
            if( arg->op == PT_SYMBOL ) {
                generateRarg( arg, arg_no );
            } else if( NodeIsUnaryOp( arg, CO_RARG_FETCH ) ) {
                arg = getChildNode( &arg->u.subtree[0] );
                generateRarg( arg, arg_no );
            } else {
                if( NodeIsUnaryOp( arg, CO_INDIRECT ) ) {
                    arg = getChildNode( &arg->u.subtree[0] );
                }
                if( NodeIsBinaryOp( arg, CO_DOT ) ) {
                    INT_CONSTANT icon;
                    right = arg->u.subtree[1];
                    if( NodeIsIntConstant( right, &icon ) ) {
                        off = icon.uval;
                        arg = getChildNode( &arg->u.subtree[0] );
                        if( arg->op == PT_SYMBOL ) {
                            generateRargOffset( arg, arg_no, off );
                        } else if( NodeIsUnaryOp( arg, CO_RARG_FETCH ) ) {
                            arg = getChildNode( &arg->u.subtree[0] );
                            generateRargOffset( arg, arg_no, off );
                        } else if( NodeIsUnaryOp( arg, CO_VBASE_FETCH ) ) {
                            off += vbaseDelta( arg );
                            arg = getChildNode( &arg->u.subtree[0] );
                            generateRargVbOffset( arg, arg_no, off );
                        }
                    }
                } else if( NodeIsUnaryOp( arg, CO_VBASE_FETCH ) ) {
                    off = vbaseDelta( arg );
                    arg = getChildNode( &arg->u.subtree[0] );
                    generateRargVbOffset( arg, arg_no, off );
                }
            }
        }
    }
}


static TYPE generate_call_type( // GENERATE TYPE FOR CALL SETUP
    PTREE expr )                // - setup expression
{
    TYPE type;                  // - could be pointer to function, function
    TYPE pted;                  // - NULL or function type
    TYPE retn;                  // - return type
    TYPE unmd;                  // - unmodified function type

    type = expr->type;
    pted = TypePointedAtModified( type );
    if( pted != NULL ) {
        type = pted;
    }
    unmd = FunctionDeclarationType( type );
    retn = unmd->of;
    if( unmd->flag & TF1_PLUSPLUS ) {
        CgSetType( retn );
    } else {
        CgSetTypeExact( retn );
    }
    return type;
}


static SYMBOL getAliasDtorSym(  // GET ALIASED DTOR SYMBOL
    SYMBOL sym )                // - original symbol
{
    sym = SymDeAlias( sym );
    sym->flag |= SF_ADDR_TAKEN;
    return SymMarkRefed( sym );
}


static boolean emitAutoMarking( // EMIT MARKING FOR AN AUTO VAR
    SYMBOL sym )                // - the auto var
{
    boolean br = CgFrontRetnOptVar( sym );
    CgFrontCodePtr( IC_DTOR_AUTO, sym );
    FunctionHasCtorTest();
    return br;
}


static void emitDtorMarking(    // EMIT DTOR MARKING OF FOR SYMBOL
    PTREE expr )                // - expression init'ing symbol
{
    SCOPE sym_scope;            // - symbol's scope
    PTREE sym_node;             // - node for symbol
    SYMBOL sym;                 // - symbol for dtoring
    SYMBOL dtor;                // - dtor for symbol
    SYMBOL alias;               // - alias'ed symbol

    sym_node = expr->u.subtree[0];
    sym = sym_node->u.symcg.symbol;
    dtor = RoDtorFind( sym );
    SegmentMarkRoBlock();
    alias = getAliasDtorSym( sym );
    if( SymIsModuleDtorable( sym ) ) {
        SegmentMarkUsed( SEG_DATA );
        LabelDeclInited( alias );
        sym_scope = SymScope( alias );
        CgFrontCodePtr( IC_DTOR_STATIC, alias );
    } else {
        if( SymIsCatchAlias( sym ) && !SymIsFnTemplateMatchable( sym ) ) {
            if( emitAutoMarking( sym ) ) {
                CgFrontRetnOptEnd();
            }
        } else if( SymIsTemporary( sym )
                && ! ( expr->flags & PTF_BLOCK_INIT ) ) {
            LabelTempDtored( expr->u.subtree[1], alias );
            CgFrontCodePtr( IC_DTOR_TEMP, alias );    // - temporary
            PtdDtorUseSym( expr, dtor );
            FunctionHasCtorTest();
        } else {
            if( expr->flags & PTF_BLOCK_INIT ) {
                DbgVerify( init_ref_temp == NULL
                         , "emitDtorMarking -- already auto init-ref" );
                init_ref_temp = alias;
                CgFrontCodePtr( IC_DTOR_REF, dtor );
            } else {
                boolean br = emitAutoMarking( alias );
                CgFrontCodePtr( IC_DTOR_REF, dtor );
                if( br ) {
                    CgFrontRetnOptEnd();
                }
            }
        }
    }
}


static void handleSpecialLValue(// HANDLE VOLATILE/UNALIGNED/IN_MEM/PRECISION LVALUES
    PTREE expr,                 // - expression
    unsigned offset )           // - 0 - top, 1 - under top
{
    TYPE type;                  // - unmodified type
    type_flag flags;            // - modifier flags
    unsigned volatile_op;       // - set to non-CO_NOP
    unsigned unaligned_op;      // - set to non-CO_NOP
    unsigned const_op;          // - set to non-CO_NOP

    volatile_op = CO_NOP;
    type = TypeModFlags( TypeReferenced( NodeType( expr ) ), &flags );
    if( flags & TF1_STAY_MEMORY ) {
        volatile_op = CO_VOLATILE_TOP;
    } else if( CompFlags.op_switch_used ) {
        switch( type->id ) {
        case TYP_FLOAT :
        case TYP_DOUBLE :
        case TYP_LONG_DOUBLE :
            volatile_op = CO_VOLATILE_TOP;
            break;
        }
    }
    if( volatile_op != CO_NOP ) {
        CgFrontCodeUint( IC_OPR_UNARY, volatile_op + offset );
    }
    const_op = CO_NOP;
    if( flags & TF1_CONST ) {
        const_op = CO_CONST_TOP;
    }
    if( const_op != CO_NOP ) {
        CgFrontCodeUint( IC_OPR_UNARY, const_op + offset );
    }
    unaligned_op = CO_NOP;
    if( flags & TF1_UNALIGNED ) {
        unaligned_op = CO_UNALIGNED_TOP;
    }
    if( unaligned_op != CO_NOP ) {
        CgFrontCodeUint( IC_OPR_UNARY, unaligned_op + offset );
    }
}


static void exprCondEnd(        // END OF CONDITIONAL DTORING, IF REQ'D
    PTREE expr )                // - current flow expression
{
    if( expr->flags & PTF_COND_END ) {
        LabelCondEnd();
    }
}

#ifndef NDEBUG
static boolean klugedTyping(    // DEBUG -- see if ptr/ref consistency
    TYPE t1,                    // - type[1]
    TYPE t2 )                   // - type[2]
{
    boolean retn;               // - return: TRUE ==> ok

    t1 = TypePointedAtModified( t1 );
    if( t1 == NULL ) {
        retn = FALSE;
    } else {
        t2 = TypeReference( t2 );
        if( t2 == NULL ) {
            retn = FALSE;
        } else {
            retn = TypesIdentical( t1, t2 );
        }
    }
    return retn;
}


static boolean goodArgType(     // DEBUG -- verify arg/expr consistency
    PTREE expr )                // - CO_LIST expression
{
    boolean retn;               // - return: TRUE ==> ok
    TYPE atype;                 // - argument type
    TYPE etype;                 // - expression type

    atype = NodeType( expr );
    etype = NodeType( expr->u.subtree[1] );
    retn = TypesIdentical( etype, atype );
    if( ! retn ) {
        if( klugedTyping( etype, atype )
         || klugedTyping( atype, etype ) ) {
            retn = TRUE;
        } else {
            retn = FALSE;
        }
    }
    return retn;
}
#endif

static TYPE setFloatUsed( TYPE type ) {
    type = TypedefModifierRemoveOnly( type );
    if(( type->id == TYP_POINTER ) && ( type->flag & TF1_REFERENCE ) == 0 ) {
        type = TypedefModifierRemove( type->of );
    }
    return( FloatingType( type ) );
}

static PTREE emitNode(          // EMIT A PTREE NODE
    PTREE expr )                // - current node
{
    SEARCH_RESULT *result;      // - old search result in node
    SYMBOL sym;                 // - symbol for node
    TYPE type;                  // - type
    TYPE parm_type;             // - type of pushed parm
    type_flag flags;            // - type flags

    if( expr->decor != NULL ) {
        PtdGenBefore( expr->decor );
    }
    switch( expr->op ) {
      case PT_INT_CONSTANT:
        if( expr->cgop != CO_IGNORE ) {
            if( NULL == Integral64Type( expr->type ) ) {
                generate_expr_instr( expr
                                   , expr->u.uint_constant
                                   , IC_LEAF_CONST_INT );
            } else {
                generate_type( expr );
                CgFrontCodePtr( IC_LEAF_CONST_INT64
                              , ConPoolInt64Add( expr->u.int64_constant ) );
            }
        }
        break;
      case PT_STRING_CONSTANT :
        if( expr->cgop != CO_IGNORE ) {
            generate_type( expr );
            CgFrontCodePtr( IC_LEAF_CONST_STR, expr->u.string );
        }
        break;
      case PT_FLOATING_CONSTANT :
        generate_type( expr );
        CgFrontCodePtr( IC_LEAF_CONST_FLT, ConPoolFloatAdd( expr ) );
        break;
  #ifndef NDEBUG
      case PT_DUP_EXPR :
        CFatal( "ICEMIT -- PT_DUP_EXPR not promoted away" );
        break;
  #endif
      case PT_SYMBOL:
        result = expr->u.symcg.result;
        if( result != NULL ) {
            ScopeFreeResult( result );
            expr->u.symcg.result = NULL;
        }
        if( expr->cgop == CO_NAME_THIS ) {
            generate_type( expr );
            CgFrontCode( IC_LEAF_THIS );
        } else if( expr->cgop == CO_NAME_DTOR_SYM ) {
            // name will be picked up by CO_DTOR_SYM
        } else if( expr->cgop == CO_NAME_PARM_REF ) {
            // name will be picked up by CO_RARG_FETCH
        } else if( expr->cgop == CO_NAME_CDTOR_EXTRA ) {
            generate_type( expr );
            CgFrontCode( IC_LEAF_CDTOR_EXTRA );
        } else if( expr->cgop != CO_IGNORE ) {
            sym = getPtreeSymbol( expr );
            CgFrontSymbol( sym );
        }
        break;
      case PT_UNARY:
        switch( expr->cgop ) {
          case CO_EXPR_DONE:
            if( init_ref_temp != NULL ) {
                CgFrontCode( IC_INIT_REF_END );
            }
            CgFrontCode( IC_EXPR_DONE );
            if( expr->flags & PTF_RETN_OPT ) {
                CgFrontCode( IC_RETNOPT_END );
            }
            break;
          case CO_TRASH_EXPR:
            CgFrontCode( IC_EXPR_TRASH );
            break;
          case CO_CALL_SETUP_IND :
          { TYPE pted;                // - type, if pointer type
            type = generate_call_type( expr );
            pted = TypePointedAtModified( type );
            if( pted != NULL ) {
                type = pted;
            }
            type = MakeNonInlineFunction( type );
            CgFrontCodePtr( IC_CALL_SETUP_IND, type );
          } break;
          case CO_CALL_SETUP :
          { SYMBOL func;        // - current function
            unsigned spec_fun;
            generate_call_type( expr );
            func = getLeftPtreeSymbol( expr );
            CgFrontCodePtr( IC_CALL_SETUP, func );
            spec_fun = SpecialFunction( func );
            if( SPFN_SETJMP == spec_fun
#if _CPU == _AXP
             || SPFN_SETJMP1 == spec_fun
             || SPFN_SETJMP2 == spec_fun
             || SPFN_SETJMP3 == spec_fun
#endif
            ) {
                LabelExprSetjmp();
            }
          } break;
          case CO_BITFLD_CONVERT :
            generate_type( expr );
            for( type = PTreeOpLeft( expr )->type
               ; type->id != TYP_BITFIELD
               ; type = type->of );
            CgFrontCodePtr( IC_BIT_MASK, type );
            break;
          case CO_ADDR_OF :
          case CO_CONVERT_INT :
            break;
          case CO_RARG_FETCH_OFF :
          case CO_FETCH :
            generate_type( expr->u.subtree[0] );
            handleSpecialLValue( expr, 0 );
            CgFrontCodeUint( IC_OPR_UNARY, expr->cgop );
            break;
          case CO_RARG_FETCH :
            generate_type( expr );
            CgFrontCodePtr( IC_RARG_FETCH, getLeftPtreeSymbol( expr ) );
            break;
          case CO_INDIRECT :
            TypeModFlags( expr->type, &flags );
            if( flags & TF1_FAR16 ) {
                CgFrontCodeUint( IC_OPR_UNARY, CO_FAR16_TO_POINTER );
            }
            break;
          case CO_FAR16_TO_POINTER:
            CgFrontCodeUint( IC_OPR_UNARY, CO_FAR16_TO_POINTER );
            break;
          case CO_POINTER_TO_FAR16:
            CgFrontCodeUint( IC_OPR_UNARY, CO_POINTER_TO_FAR16 );
            break;
          case CO_TYPE_SIG :
            CgFrontCodePtr( IC_THROW_RO_BLK, expr->type );
            break;
          case CO_SEGOP_SEG :
          { unsigned segid;     // - segment #
            if( expr->u.subtree[0] == NULL ) {
                segid = SEG_CODE;
            } else {
                segid = getLeftPtreeSymbol( expr )->segid;
            }
            CgFrontCodeUint( IC_SEGOP_SEG, segid );
          } break;
          case CO_EXCLAMATION:
            generate_expr_instr( expr, expr->cgop, IC_OPR_UNARY );
            CgFrontResultBoolean();
            break;
          case CO_VBASE_FETCH :
            generate_node_type( expr );
            CgFrontCodePtr( IC_VB_FETCH, getLeftPtreeSymbol( expr ) );
            break;
#if _CPU == _AXP
          case CO_ALLOCA :
            generate_node_type( expr );
            CgFrontCode( IC_ALLOCA );
            break;
#endif
          case CO_VFUN_PTR :
            CgFrontCode( IC_VFUN_PTR );
            break;
          case CO_CALL_SETUP_VFUN :
          { PTREE callee;               // - callee node
            SYMBOL vfun;                // - virtual function
            generate_type( expr );
            callee = expr->u.subtree[0];
            DbgVerify( callee->op == PT_SYMBOL, "CO_CALL_SETUP_VFUN -- sym?" );
            vfun = callee->u.symcg.symbol;
            CgFrontCodePtr( IC_SETUP_VFUN, vfun );
          } break;
          default:
            generate_expr_instr( expr, expr->cgop, IC_OPR_UNARY );
            break;
        }
        break;
      case PT_BINARY:
        switch( expr->cgop ) {
          case CO_CALL_EXEC :
            generateCallRefICs( expr );
            generate_node_type( expr );
            CgFrontCode( IC_CALL_EXEC );
            generateScopeCall( expr );
            break;
          case CO_CALL_EXEC_IND :
            generate_node_type( expr );
            if( NodeIsBinaryOp( expr->u.subtree[0], CO_VIRT_FUNC ) ) {
                CgFrontCode( IC_CALL_EXEC_VFUN );
            } else {
                CgFrontCode( IC_CALL_EXEC_IND );
            }
            FunctionCouldThrow( expr );
            break;
          case CO_QUESTION:
            generate_type_instr( NodeType( expr )
                               , expr->cgop
                               , IC_OPR_TERNARY );
            exprCondEnd( expr );
            break;
          case CO_COLON_COLON :
            break;
          case CO_DOT :
            generate_type_instr( NodeType( expr )
                               , expr->cgop
                               , IC_OPR_BINARY );
            break;
          case CO_ARROW :
            generate_type_instr( expr->u.subtree[0]->type
                               , expr->cgop
                               , IC_OPR_BINARY );
            break;
          case CO_CONST_CAST :
          case CO_STATIC_CAST :
          case CO_DYNAMIC_CAST :
          case CO_REINTERPRET_CAST :
            // we could just ignore these, but then we would have to take
            // them into consideration whenever we scan a parse tree
            DbgNever();
            // drops thru
          case CO_CONVERT :
          { TYPE type = NodeType( expr );
            if( NULL == StructType( type )
             && NULL == MemberPtrType( type ) ) {
                generate_expr_instr( expr, CO_CONVERT, IC_OPR_UNARY );
            }
          } break;
          case CO_RETURN:
            generate_type( expr );
            break;
          case CO_LIST :
            DbgVerify( goodArgType( expr )
                     , "emitNode -- parameter-type mismatch" );
            parm_type = generate_node_type( expr );
            if( expr->flags & PTF_ARG_THIS_VFUN ) {
                CgFrontCode( IC_VF_THIS_ADJ );
            }
            if( setFloatUsed( parm_type ) ) {
                CgFrontCode( IC_CALL_PARM_FLT );
            } else {
                CgFrontCode( IC_CALL_PARM );
            }
            break;
          case CO_COPY_OBJECT :
            CgSetType( NodeType( expr ) );
            CgFrontCodePtr( IC_COPY_OBJECT, expr->type );
            break;
          case CO_DTOR :
            emitDtorMarking( expr );
            break;
          case CO_EQ :
          case CO_NE :
          case CO_GT :
          case CO_LT :
          case CO_GE :
          case CO_LE :
            generate_node_type( expr->u.subtree[0] );
            CgFrontCodeUint( IC_OPR_BINARY, expr->cgop );
            CgFrontResultBoolean();
            break;
          case CO_EQUAL_REF :
          case CO_INIT_REF :
          { TYPE lv_type;
            lv_type = NodeType( expr );
            CgFrontCodePtr( IC_LVALUE_TYPE, lv_type );
            generate_type_instr( lv_type, CO_EQUAL, IC_OPR_BINARY );
          } break;
          case CO_EQUAL :
          case CO_PLUS_EQUAL :
          case CO_PERCENT_EQUAL :
          case CO_MINUS_EQUAL :
          case CO_DIVIDE_EQUAL :
          case CO_TIMES_EQUAL :
          case CO_AND_EQUAL :
          case CO_OR_EQUAL :
          case CO_XOR_EQUAL :
          case CO_LSHIFT_EQUAL :
          case CO_RSHIFT_EQUAL :
          case CO_BPRE_BOOL_PLUS_PLUS :
          case CO_BPRE_PLUS_PLUS :
          case CO_BPRE_MINUS_MINUS :
          case CO_INIT :
          { TYPE lv_type;
            TYPE rf_type;
            lv_type = NodeType( expr );
            rf_type = TypeReferenced( lv_type );
            CgFrontCodePtr( IC_LVALUE_TYPE, lv_type );
            handleSpecialLValue( expr, 1 );
            generate_type_instr( rf_type, expr->cgop, IC_OPR_BINARY );
          } break;
          case CO_BPOST_PLUS_PLUS :
          case CO_BPOST_BOOL_PLUS_PLUS :
          case CO_BPOST_MINUS_MINUS :
            handleSpecialLValue( expr, 1 );
            generate_expr_instr( expr, expr->cgop, IC_OPR_BINARY );
            break;
          case CO_VIRT_FUNC :   // - virtual function call
          { PTREE left;
            left = expr->u.subtree[0];
            DbgVerify( NodeIsUnaryOp( left, CO_CALL_SETUP_IND )
                     , "IcEmit -- not CO_CALL_SETUP_IND" );
            if( ! NodeIsUnaryOp( PTreeOpLeft( left )
                               , CO_CALL_SETUP_VFUN ) ) {
                CgFrontCode( IC_VF_CODED );
            }
            CgFrontCodePtr( IC_VIRT_FUNC
                          , getPtreeSymbol( expr->u.subtree[1] ) );
          } break;
          case CO_COLON:
            break;
          case CO_RESET_THIS :
          { PTREE right;
            right = PTreeOpRight( expr );
            CgFrontCodeUint( IC_RESET_THIS, right->u.uint_constant );
          } break;
          case CO_AND_AND :
          case CO_OR_OR :
            generate_expr_instr( expr, expr->cgop, IC_OPR_BINARY );
            exprCondEnd( expr );
            CgFrontResultBoolean();
            break;
          case CO_INDEX :
            CgFrontCodePtr( IC_LVALUE_TYPE, NodeType( expr ) );
            generate_expr_instr( expr, expr->cgop, IC_OPR_BINARY );
            break;
          default:
            generate_expr_instr( expr, expr->cgop, IC_OPR_BINARY );
            break;
        }
        break;
      case PT_IC :
        switch( expr->u.ic.opcode ) {
          case IC_COND_TRUE :
            LabelCondTrue();
            break;
          case IC_COND_FALSE :
            LabelCondFalse();
            break;
          default :
            generate_type( expr );
            CgFrontCodePtr( expr->u.ic.opcode, expr->u.ic.value.pvalue );
            break;
        }
        break;
    }
    if( expr->flags & PTF_DTORABLE_INIT ) {
        CgFrontCode( IC_DTORABLE_INIT );
    }
    if( expr->decor != NULL ) {
        PtdGenAfter( expr->decor );
    }
    return expr;
}


void SrcPosnEmit(               // EMIT SOURCE POSITION
    TOKEN_LOCN *posn )          // - position to emit
{
    if( posn->src_file != NULL ) {
        CgFrontDbgLine( posn );
        posn_gened = *posn;
    }
}


TOKEN_LOCN SrcPosnEmitCurrent(  // EMIT LINE NO. FOR CURRENT POSITION
    void )
{
    TOKEN_LOCN posn;            // - current position

    SrcFileGetTokenLocn( &posn );
    SrcPosnEmit( &posn );
    return posn;
}


static void checkDeadCode(      // TEST FOR DEAD CODE
    PTREE expr )                // - expression
{
    SYMBOL func;                // - function called

    if( NodeIsUnaryOp( expr, CO_EXPR_DONE ) ) {
        expr = expr->u.subtree[0];
    }
    expr = PTreeOp( &expr );
    if( NodeIsBinaryOp( expr, CO_CALL_EXEC ) ) {
        expr = PTreeOp( &expr->u.subtree[0]->u.subtree[0] );
        if( expr->op == PT_SYMBOL ) {
            func = getPtreeSymbol( expr );
            if( IsPragmaAborts( func ) ) {
                FunctionBodyDeadCode();
                CgFrontCode( IC_AFTER_ABORTS );
            }
            if( RunTimeIsThrow( func ) ) {
                FunctionBodyDeadCode();
                CgFrontCode( IC_AFTER_THROW );
            }
        }
    }
}


void IcEmitDtorAutoSym(         // EMIT EXPRESSION TO MARK AUTO AS DTOR'ED
    SYMBOL sym )                // - symbol to be dtor'ed
{
    PTREE node;                 // - expression to be emitted

    node = NodeDtorExpr( NodeIntDummy(), sym );
    node = PtdDtorScopeType( node, sym->sym_type );
    node = NodeUnaryCopy( CO_TRASH_EXPR, node );
    IcEmitExpr( node );
}


void IcEmitDtorStaticSym(       // EMIT EXPRESSION TO MARK STATIC AS DTOR'ED
    SYMBOL sym )                // - symbol to be dtor'ed
{
    PTREE node;                 // - expression to be emitted

    node = NodeDtorExpr( NodeIntDummy(), sym );
    node = NodeUnaryCopy( CO_TRASH_EXPR, node );
    IcEmitExpr( node );
}


PTREE IcEmitExpr(               // EMIT EXPRESSION
    PTREE expr )
{
    TOKEN_LOCN posn;            // - position for source file

    if( expr == NULL ) {
        return expr;
    }
    init_ref_temp = NULL;
    LabelExprBegin();
    expr = NodePromoteDups( expr, &posn );
    SrcPosnEmit( &posn );
#ifndef NDEBUG
    if( PragDbgToggle.ic_trace_on ) {
        CgFrontCode( IC_TRACE_BEG );
        PragDbgToggle.ic_trace_on = 0;
    }
    if( PragDbgToggle.ic_trace_off ) {
        CgFrontCode( IC_TRACE_END );
        PragDbgToggle.ic_trace_off = 0;
    }
    if( PragDbgToggle.dump_ptree ) {
        DumpCommentary( "Parse tree to be emited" );
        DumpPTree( expr );
    }
    if( PragDbgToggle.print_ptree ) {
        DumpCommentary( "Parse tree to be emited" );
        DbgPrintPTREE( expr );
    }
#endif
    if( NodeIsUnaryOp( expr, CO_EXPR_DONE ) ) {
        if( NodeIsBinaryOp( expr->u.subtree[0], CO_INIT_REF ) ) {
            InitRefFixup( expr->u.subtree[0] );
        } else if( expr->flags & PTF_RETN_OPT ) {
            CgFrontCode( IC_RETNOPT_BEG );
        }
    }
    expr = PTreeTraversePostfix( expr, &emitNode );
    checkDeadCode( expr );
    LabelExprEnd();
    PTreeFreeSubtrees( expr );
    return expr;
}


void EmitAnalysedStmt(          // EMIT ANALYSED STMT (IF IT EXISTS)
    PTREE stmt )                // - the statement
{
    if( stmt != NULL ) {
        IcEmitExpr( NodeDone( stmt ) );
    }
}
