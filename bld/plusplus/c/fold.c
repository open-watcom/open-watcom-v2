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
#include <limits.h>

#include "ptree.h"
#include "cgfront.h"
#include "fold.h"
#include "codegen.h"
#include "floatsup.h"

//-------------- Temporary Stubs -------------------------------

#define TWOTO32_STRING "4294967296"

static float_handle TwoTo32( void )
{
    return( BFCnvSF( TWOTO32_STRING ) );
}

static
float_handle BFCnvU64F( unsigned_64 val )
{
    float_handle t0, t1, t2;

    t0 = TwoTo32();
    t1 = BFCnvUF( val.u._32[ I64HI32 ] );
    t2 = BFMul( t0, t1 );
    BFFree( t0 );
    BFFree( t1 );
    t0 = BFCnvUF( val.u._32[ I64LO32 ] );
    t1 = BFAdd( t0, t2 );
    BFFree( t0 );
    BFFree( t2 );
    return t1;
}

static
float_handle BFCnvI64F( signed_64 val )
{
    float_handle t0, t1, t2;

    t0 = TwoTo32();
    t1 = BFCnvIF( val.u._32[ I64HI32 ] );
    t2 = BFMul( t0, t1 );
    BFFree( t0 );
    BFFree( t1 );
    if( val.u.sign.v ) {
        t0 = BFCnvUF( ( val.u._32[ I64LO32 ] ^ ULONG_MAX ) + 1 );
        BFNegate( t0 );
    } else {
        t0 = BFCnvUF( val.u._32[ I64LO32 ] );
    }
    t1 = BFAdd( t0, t2 );
    BFFree( t0 );
    BFFree( t2 );
    return t1;
}

static
signed_64 BFCnvF64( float_handle flt )
{
    signed_64 result;
    float_handle absol, t0, t1, t2, t3;
    bool positive;

    int sign = BFSign( flt );
    if( 0 == sign ) {
        result.u._32[0] = 0;
        result.u._32[1] = 0;
        return result;
    }
    positive = TRUE;
    absol = flt;
    if( sign < 0 ) {
        positive = FALSE;
        absol = BFCopy( flt );
        BFNegate( absol );
    }
    t0 = TwoTo32();
    t1 = BFDiv( flt, t0 );
    t3 = BFTrunc( t1 );
    BFFree( t1 );
    result.u._32[ I64HI32 ] = BFCnvF32( t3 );
    BFFree( t3 );
    t1 = BFCnvUF( result.u._32[ I64HI32 ] );
    t2 = BFMul( t0, t1 );
    BFFree( t0 );
    BFFree( t1 );
    t0 = BFSub( flt, t2 );
    BFFree( t2 );
    t3 = BFTrunc( t0 );
    BFFree( t0 );
    result.u._32[ I64LO32 ] = BFCnvF32( t3 );
    BFFree( t3 );
    if( ! positive ) {
        signed_64 neg;
        BFFree( absol );
        neg = result;
        U64Neg( &neg, &result );
    }
    return result;
}


//--------------------------------------------------------------


target_long FoldSignedRShiftMax( target_long v )
/**********************************************/
{
    if( v < 0 ) {
        return -1;
    }
    return 0;
}


static bool isCondDecor(        // TEST IF CONDITIONALLY DECORATED
    PTREE node )                // - the expression
{
    bool retn;                  // - TRUE ==> conditionally decorated

    if( NodeIsBinaryOp( node, CO_COMMA ) ) {
        node = node->u.subtree[0];
        if( node->op == PT_IC ) {
            if( node->u.ic.opcode == IC_COND_TRUE
             || node->u.ic.opcode == IC_COND_FALSE ) {
                retn = TRUE;
            } else {
                retn = FALSE;
            }
        } else {
            retn = FALSE;
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


static PTREE overCondDecor(     // BY-PASS CONDITIONAL DECORATION
    PTREE expr )                // - expression
{
    if( isCondDecor( expr ) ) {
        expr = expr->u.subtree[1];
    }
    return expr;
}


bool Zero64                     // TEST IF 64-BITTER IS ZERO
    ( signed_64 const *test )   // - value to be tested
{
    return( test->u._32[0] == 0 && test->u._32[1] == 0 );
}


static bool zeroConstant(       // TEST IF NODE IS ZERO CONSTANT
    PTREE expr )                // - the expression
{
    PTREE orig;

    switch( expr->op ) {
    case PT_INT_CONSTANT:
        if( NULL == Integral64Type( expr->type ) ) {
            return( expr->u.int_constant == 0 );
        } else {
            return( Zero64( &expr->u.int64_constant ) );
        }
    case PT_FLOATING_CONSTANT:
    {   target_ulong ul_val = BFCnvF32( expr->u.floating_constant );
        return 0 == ul_val;
    }
    case PT_BINARY:
        orig = expr;
        expr = NodeRemoveCasts( expr );
        if( expr == orig ) break;
        return( zeroConstant( expr ) );
    }
    return( FALSE );
}


static bool nonZeroExpr(        // TEST IF NODE IS NON-ZERO EXPRESSION
    PTREE expr )                // - the expression
{
    PTREE orig;

    switch( expr->op ) {
    case PT_INT_CONSTANT:
    case PT_FLOATING_CONSTANT:
        return ! zeroConstant( expr );
    case PT_SYMBOL:
        /* a symbol r-value has a fetch by now so this PTREE means &name */
        return( TRUE );
    case PT_BINARY:
        orig = expr;
        expr = NodeRemoveCasts( expr );
        if( expr == orig ) break;
        return( nonZeroExpr( expr ) );
    }
    if( expr->flags & PTF_PTR_NONZERO ) {
        return( TRUE );
    }
    return( FALSE );
}


static bool notFoldable(        // TEST IF NON-FOLDABLE EXPRESSION
    PTREE expr )                // - the expression
{
    switch( expr->op ) {
      case PT_INT_CONSTANT:
      case PT_FLOATING_CONSTANT:
        return( FALSE );
      default :
        if( zeroConstant( expr ) ) {
            return( FALSE );
        } else {
            return( TRUE );
        }
    }
}

PTREE CastIntConstant( PTREE expr, TYPE type, bool *happened )
{
    PTREE new_expr;
    target_ulong ul_val;
    float_handle dbl_val;
    type_id id;
    bool signed_type;


    signed_type = SignedIntType( expr->type );
    id = TypedefModifierRemove( type )->id;
    ul_val = expr->u.uint_constant;
    if( NULL == Integral64Type( expr->type ) ) {
        switch( id ) {
        case TYP_SCHAR:
            ul_val = (target_schar) ul_val;
            /* fall through */
        case TYP_SSHORT:
            ul_val = (target_short) ul_val;
            /* fall through */
        case TYP_SINT:
            ul_val = (target_int) ul_val;
            /* fall through */
        case TYP_SLONG:
            ul_val = (target_long) ul_val;
            new_expr = PTreeIntConstant( ul_val, id );
            break;
        case TYP_UCHAR:
            ul_val = (target_uchar) ul_val;
            /* fall through */
        case TYP_USHORT:
            ul_val = (target_ushort) ul_val;
            /* fall through */
        case TYP_UINT:
            ul_val = (target_uint) ul_val;
            /* fall through */
        case TYP_ULONG:
            ul_val = (target_ulong) ul_val;
            new_expr = PTreeIntConstant( ul_val, id );
            break;
        case TYP_ULONG64:
        case TYP_SLONG64:
            if( PT_FLOATING_CONSTANT == expr->op ) {
                new_expr = PTreeInt64Constant
                                ( BFCnvF64( expr->u.floating_constant )
                                , id );
            } else {
                new_expr = PTreeInt64Constant( expr->u.int64_constant, id );
            }
            break;
        case TYP_POINTER:
        case TYP_MEMBER_POINTER:
            ul_val = (target_ulong) ul_val;
            new_expr = PTreeIntConstant( ul_val, TYP_ULONG );
            new_expr->type = type;
            break;
        case TYP_FLOAT:
#if 0
// these are now identical, with canonical floating point
            if( signed_type ) {
                flt_val = BFCnvIF( expr->u.int_constant );
            } else {
                flt_val = BFCnvUF( expr->u.uint_constant );
            }
            new_expr = PTreeFloatingConstant( flt_val, id );
            break;
#endif
        case TYP_LONG_DOUBLE:
        case TYP_DOUBLE:
            if( signed_type ) {
                dbl_val = BFCnvIF( expr->u.int_constant );
            } else {
                dbl_val = BFCnvUF( expr->u.uint_constant );
            }
            new_expr = PTreeFloatingConstant( dbl_val, id );
            break;
        case TYP_WCHAR:
            ul_val = (target_wchar) ul_val;
            new_expr = PTreeIntConstant( ul_val, id );
            break;
        default:
            return( expr );
        }
    } else {
        ul_val = expr->u.int_constant;
        switch( id ) {
        case TYP_SCHAR:
            ul_val = (target_schar) ul_val;
            /* fall through */
        case TYP_SSHORT:
            ul_val = (target_short) ul_val;
            /* fall through */
        case TYP_SINT:
            ul_val = (target_int) ul_val;
            /* fall through */
        case TYP_SLONG:
            ul_val = (target_long) ul_val;
            new_expr = PTreeIntConstant( ul_val, id );
            break;
        case TYP_SLONG64:
        case TYP_ULONG64:
            new_expr = PTreeInt64Constant( expr->u.int64_constant, id );
            break;
        case TYP_UCHAR:
            ul_val = (target_uchar) ul_val;
            /* fall through */
        case TYP_USHORT:
            ul_val = (target_ushort) ul_val;
            /* fall through */
        case TYP_UINT:
            ul_val = (target_uint) ul_val;
            /* fall through */
        case TYP_ULONG:
            ul_val = (target_ulong) ul_val;
            new_expr = PTreeIntConstant( ul_val, id );
            break;
        case TYP_POINTER:
        case TYP_MEMBER_POINTER:
            ul_val = (target_ulong) ul_val;
            new_expr = PTreeIntConstant( ul_val, TYP_ULONG );
            new_expr->type = type;
            break;
        case TYP_FLOAT:
#if 0
// these are now identical, with canonical floating point
            if( signed_type ) {
                flt_val = BFCnvI64F( expr->u.int64_constant );
            } else {
                flt_val = BFCnvU64F( expr->u.uint64_constant );
            }
            new_expr = PTreeFloatingConstant( flt_val, id );
            break;
#endif
        case TYP_LONG_DOUBLE:
        case TYP_DOUBLE:
            if( signed_type ) {
                dbl_val = BFCnvI64F( expr->u.int64_constant );
            } else {
                dbl_val = BFCnvU64F( expr->u.int64_constant );
            }
            new_expr = PTreeFloatingConstant( dbl_val, id );
            break;
        case TYP_WCHAR:
            ul_val = (target_wchar) ul_val;
            new_expr = PTreeIntConstant( ul_val, id );
            break;
        default:
            return( expr );
        }
    }
    *happened = TRUE;
    new_expr->flags = expr->flags;
    new_expr = PTreeCopySrcLocation( new_expr, expr );
    PTreeFree( expr );
    return( new_expr );
}

static PTREE castFloatingConstant( PTREE expr, TYPE type, bool *happened )
{
    target_long value;
    PTREE new_expr;
    type_id id;

    id = TypedefModifierRemove( type )->id;
    switch( id ) {
    case TYP_POINTER:
    case TYP_MEMBER_POINTER:
        id = TYP_ULONG;
        // drops thru
    case TYP_SCHAR:
    case TYP_SSHORT:
    case TYP_SINT:
    case TYP_SLONG:
    case TYP_UCHAR:
    case TYP_USHORT:
    case TYP_UINT:
    case TYP_ULONG:
    case TYP_WCHAR:
        value = BFGetLong( &(expr->u.floating_constant) );
        new_expr = PTreeIntConstant( (target_ulong) value, id );
        new_expr = CastIntConstant( new_expr, type, happened );
        break;
    case TYP_FLOAT: {
        float_handle flt_val;

        flt_val = BFCopy( expr->u.floating_constant );
        new_expr = PTreeFloatingConstant( flt_val, id );
    }
        break;
    case TYP_LONG_DOUBLE:
    case TYP_DOUBLE:
    {   float_handle flt_val;
        flt_val = BFCopy( expr->u.floating_constant );
        new_expr = PTreeFloatingConstant( flt_val, id );
    }   break;
    case TYP_SLONG64:
    case TYP_ULONG64:
    {   signed_64 val = BFCnvF64( expr->u.floating_constant );
        new_expr = PTreeInt64Constant( val, id );
    }   break;
    default:
        return( expr );
    }
    *happened = TRUE;
    new_expr = PTreeCopySrcLocation( new_expr, expr );
    PTreeFree( expr );
    return( new_expr );
}

static PTREE castConstant( PTREE expr, TYPE type, bool *happened )
{
    TYPE type_final;

    *happened = FALSE;
    if( notFoldable( expr ) ) {
        return( expr );
    }
    type_final = TypedefModifierRemoveOnly( type );
    type = TypedefModifierRemove( type_final );
    switch( expr->op ) {
    case PT_INT_CONSTANT:
        expr = CastIntConstant( expr, type, happened );
        expr->type = type_final;
        break;
    case PT_FLOATING_CONSTANT:
        expr = castFloatingConstant( expr, type, happened );
        expr->type = type_final;
        break;
    }
    return( expr );
}

static bool soFarSoGood( PTREE expr, unsigned op, CGOP cgop )
{
    if( expr != NULL && expr->op == op && expr->cgop == cgop ) {
        return( TRUE );
    }
    return( FALSE );
}

static bool referenceSymbol( PTREE expr )
{
    SYMBOL ref_sym;

    ref_sym = expr->u.symcg.symbol;
    if( ref_sym != NULL && TypeReference( ref_sym->sym_type ) != NULL ) {
        return( TRUE );
    }
    return( FALSE );
}

static bool thisSymbol( PTREE expr )
{
    SYMBOL this_sym;

    this_sym = expr->u.symcg.symbol;
    if( this_sym == NULL ) {
        return( TRUE );
    }
    return( FALSE );
}

static bool anachronismFound( PTREE expr )
{
    /* two anachronisms supported:

        (1) this != 0, this == 0
        (2) &r != 0, &r == 0    -- r is 'T & r;' (parm or auto)
    */
    if( ! CompFlags.extensions_enabled ) {
        /* ANSI C++ code cannot require these constructs */
        return( FALSE );
    }
    if( soFarSoGood( expr, PT_UNARY, CO_ADDR_OF ) ) {
        expr = expr->u.subtree[0];
        if( soFarSoGood( expr, PT_UNARY, CO_RARG_FETCH ) ) {
            /*
                int foo( X &r )
                {
                    return &r != NULL;
                }
            */
            expr = expr->u.subtree[0];
            if( soFarSoGood( expr, PT_SYMBOL, CO_NAME_PARM_REF ) ) {
                return( referenceSymbol( expr ) );
            }
        } else if( soFarSoGood( expr, PT_UNARY, CO_FETCH ) ) {
            /*
                int foo( X *p )
                {
                    X &r = *p;
                    return &r != NULL;
                }
            */
            expr = expr->u.subtree[0];
            if( soFarSoGood( expr, PT_SYMBOL, CO_NAME_NORMAL ) ) {
                return( referenceSymbol( expr ) );
            }
        }
    } else if( soFarSoGood( expr, PT_UNARY, CO_RARG_FETCH ) ) {
        /*
            int S::foo()
            {
                return this ? 0 : field;
            }
        */
        expr = expr->u.subtree[0];
        if( soFarSoGood( expr, PT_SYMBOL, CO_NAME_PARM_REF ) ) {
            return( thisSymbol( expr ) );
        }
    }
    return( FALSE );
}

static PTREE makeTrueFalse( PTREE expr, PTREE op, int value )
{
    PTREE node;

    if(( op->flags & PTF_SIDE_EFF ) != 0 ) {
        return( expr );
    }
    if( anachronismFound( op ) ) {
        return( expr );
    }
    node = NodeOffset( value );
    node = NodeSetBooleanType( node );
    node = PTreeCopySrcLocation( node, expr );
    NodeFreeDupedExpr( expr );
    return( node );
}

static PTREE makeBooleanConst( PTREE expr, int value )
{
    DbgVerify( ( value & 1 ) == value, "invalid boolean constant fold" );
    expr->u.int_constant = value;
    expr->op = PT_INT_CONSTANT;
    expr = NodeSetBooleanType( expr );
    return( expr );
}

PTREE FoldUnary( PTREE expr )
/***************************/
{
    PTREE op1;
    TYPE result_type;
    bool dont_care;

    op1 = expr->u.subtree[0];
    if( notFoldable( op1 ) ) {
        switch( expr->cgop ) {
        case CO_EXCLAMATION:
            if( nonZeroExpr( op1 ) ) {
                expr = makeTrueFalse( expr, op1, 0 );
            }
            break;
        }
        return( expr );
    }
    result_type = expr->type;
    if( op1->op == PT_FLOATING_CONSTANT ) {
        switch( expr->cgop ) {
        case CO_EXCLAMATION:
            op1->u.int64_constant.u._32[ I64HI32 ] = 0;
            op1->u.int64_constant.u._32[ I64LO32 ]
                = ( BFSign( op1->u.floating_constant ) == 0 );
            op1->op = PT_INT_CONSTANT;
            op1 = NodeSetBooleanType( op1 );
            break;
        case CO_UMINUS:
            BFNegate( op1->u.floating_constant );
            break;
        case CO_UPLUS:
            break;
        default:
            return( expr );
        }
        op1 = PTreeCopySrcLocation( op1, expr );
        PTreeFree( expr );
        return( castConstant( op1, result_type, &dont_care ) );
    }
    switch( expr->cgop ) {
    case CO_TILDE:
        op1->u.int64_constant.u._32[0] = ~ op1->u.int64_constant.u._32[0];
        op1->u.int64_constant.u._32[1] = ~ op1->u.int64_constant.u._32[1];
        break;
    case CO_EXCLAMATION:
        op1 = makeBooleanConst( op1, ! nonZeroExpr( op1 ) );
        break;
    case CO_UMINUS:
        U64Neg( &op1->u.int64_constant, &op1->u.int64_constant );
        break;
    case CO_UPLUS:
        break;
    default:
        return( expr );
    }
    op1 = PTreeCopySrcLocation( op1, expr );
    PTreeFree( expr );
    return( castConstant( op1, result_type, &dont_care ) );
}

static PTREE foldFloating( CGOP op, PTREE left, float_handle v2 )
{
    float_handle v1;
    float_handle tmp;

    v1 = left->u.floating_constant;
    switch( op ) {
    case CO_PLUS:
        tmp = BFAdd( v1, v2 );
        BFFree( v1 );
        v1 = tmp;
        break;
    case CO_MINUS:
        tmp = BFSub( v1, v2 );
        BFFree( v1 );
        v1 = tmp;
        break;
    case CO_TIMES:
        tmp = BFMul( v1, v2 );
        BFFree( v1 );
        v1 = tmp;
        break;
    case CO_DIVIDE:
        if( BFSign( v2 ) == 0 ) {
            CErr1( ERR_DIVISION_BY_ZERO );
        }
        tmp = BFDiv( v1, v2 );
        BFFree( v1 );
        v1 = tmp;
        break;
    case CO_EQ:
        left = makeBooleanConst( left, BFCmp( v1, v2 ) == 0 );
        BFFree( v1 );
        return( left );
    case CO_NE:
        left = makeBooleanConst( left, BFCmp( v1, v2 ) != 0 );
        BFFree( v1 );
        return( left );
    case CO_GT:
        left = makeBooleanConst( left, BFCmp( v1, v2 ) > 0 );
        BFFree( v1 );
        return( left );
    case CO_LE:
        left = makeBooleanConst( left, BFCmp( v1, v2 ) <= 0 );
        BFFree( v1 );
        return( left );
    case CO_LT:
        left = makeBooleanConst( left, BFCmp( v1, v2 ) < 0 );
        BFFree( v1 );
        return( left );
    case CO_GE:
        left = makeBooleanConst( left, BFCmp( v1, v2 ) >= 0 );
        BFFree( v1 );
        return( left );
    case CO_COMMA:
        BFFree( v1 );
        v1 = BFCopy( v2 );
        break;
    default:
        return( NULL );
    }

    left->u.floating_constant = v1;
    left->op = PT_FLOATING_CONSTANT;
    return( left );
}

static PTREE foldUInt( CGOP op, PTREE left, target_ulong v2 )
{
    double test;
    target_ulong v1;

    v1 = left->u.uint_constant;
    switch( op ) {
    case CO_PLUS:
        v1 += v2;
        break;
    case CO_MINUS:
        v1 -= v2;
        break;
    case CO_TIMES:
        test = (double) v1 * (double) v2;
        v1 *= v2;
        if( test != v1 ) {
            CErr1( ANSI_ARITHMETIC_OVERFLOW );
        }
        break;
    case CO_DIVIDE:
        if( v2 == 0 ) {
            CErr1( ERR_DIVISION_BY_ZERO );
            v1 = 1;
        } else {
            v1 /= v2;
        }
        break;
    case CO_PERCENT:
        if( v2 == 0 ) {
            CErr1( ERR_DIVISION_BY_ZERO );
            v1 = 1;
        } else {
            v1 %= v2;
        }
        break;
    case CO_AND:
        v1 &= v2;
        break;
    case CO_OR:
        v1 |= v2;
        break;
    case CO_XOR:
        v1 ^= v2;
        break;
    case CO_RSHIFT:
        if( ((target_ulong) v2) >= TARGET_LONG * CHAR_BIT ) {
            v1 = 0;
        } else {
            v1 >>= v2;
        }
        break;
    case CO_LSHIFT:
        if( ((target_ulong) v2) >= TARGET_LONG * CHAR_BIT ) {
            v1 = 0;
        } else {
            v1 <<= v2;
        }
        break;
    case CO_EQ:
        left = makeBooleanConst( left, v1 == v2 );
        return( left );
    case CO_NE:
        left = makeBooleanConst( left, v1 != v2 );
        return( left );
    case CO_GT:
        left = makeBooleanConst( left, v1 > v2 );
        return( left );
    case CO_LE:
        left = makeBooleanConst( left, v1 <= v2 );
        return( left );
    case CO_LT:
        left = makeBooleanConst( left, v1 < v2 );
        return( left );
    case CO_GE:
        left = makeBooleanConst( left, v1 >= v2 );
        return( left );
    case CO_AND_AND:
        left = makeBooleanConst( left, v1 && v2 );
        return( left );
    case CO_OR_OR:
        left = makeBooleanConst( left, v1 || v2 );
        return( left );
    case CO_COMMA:
        v1 = v2;
        break;
#if _CPU == 8086
    case CO_SEG_OP:
        v1 = (v2 << (TARGET_NEAR_POINTER * 8)) | v1;
        break;
#endif
    default:
        return( NULL );
    }
    left->u.uint_constant = v1;
    left->op = PT_INT_CONSTANT;
    return( left );
}

static PTREE foldInt( CGOP op, PTREE left, target_long v2 )
{
    double test;
    target_long v1;

    v1 = left->u.int_constant;
    switch( op ) {
    case CO_PLUS:
        v1 += v2;
        break;
    case CO_MINUS:
        v1 -= v2;
        break;
    case CO_TIMES:
        test = (double) v1 * (double) v2;
        v1 *= v2;
        if( test != v1 ) {
            CErr1( ANSI_ARITHMETIC_OVERFLOW );
        }
        break;
    case CO_DIVIDE:
        if( v2 == 0 ) {
            CErr1( ERR_DIVISION_BY_ZERO );
            v1 = 1;
        } else {
            v1 /= v2;
        }
        break;
    case CO_PERCENT:
        if( v2 == 0 ) {
            CErr1( ERR_DIVISION_BY_ZERO );
            v1 = 1;
        } else {
            v1 %= v2;
        }
        break;
    case CO_AND:
        v1 &= v2;
        break;
    case CO_OR:
        v1 |= v2;
        break;
    case CO_XOR:
        v1 ^= v2;
        break;
    case CO_RSHIFT:
        if( ((target_ulong) v2) >= TARGET_LONG * CHAR_BIT ) {
            v1 = FoldSignedRShiftMax( v1 );
        } else {
            v1 >>= v2;
        }
        break;
    case CO_LSHIFT:
        if( ((target_ulong) v2) >= TARGET_LONG * CHAR_BIT ) {
            v1 = 0;
        } else {
            v1 <<= v2;
        }
        break;
    case CO_EQ:
        left = makeBooleanConst( left, v1 == v2 );
        return( left );
    case CO_NE:
        left = makeBooleanConst( left, v1 != v2 );
        return( left );
    case CO_GT:
        left = makeBooleanConst( left, v1 > v2 );
        return( left );
    case CO_LE:
        left = makeBooleanConst( left, v1 <= v2 );
        return( left );
    case CO_LT:
        left = makeBooleanConst( left, v1 < v2 );
        return( left );
    case CO_GE:
        left = makeBooleanConst( left, v1 >= v2 );
        return( left );
    case CO_AND_AND:
        left = makeBooleanConst( left, v1 && v2 );
        return( left );
    case CO_OR_OR:
        left = makeBooleanConst( left, v1 || v2 );
        return( left );
    case CO_COMMA:
        v1 = v2;
        break;
    default:
        return( NULL );
    }
    left->u.int_constant = v1;
    left->op = PT_INT_CONSTANT;
    return( left );
}

static void idiv64              // DO 64-BIT SIGNED DIVISION
    ( signed_64 const * v1      // - top
    , signed_64 const * v2      // - divisor
    , signed_64* result         // - result
    , signed_64* rem )          // - remainder
{
    if( v2->u._32[0] == 0
     && v2->u._32[1] == 0 ) {
        CErr1( ERR_DIVISION_BY_ZERO );
        result->u._32[ I64HI32 ] = 0;
        result->u._32[ I64LO32 ] = 1;
        rem->u._32[ I64HI32 ] = 0;
        rem->u._32[ I64LO32 ] = 0;
    } else {
        I64Div( v1, v2, result, rem );
    }
}

static PTREE foldInt64( CGOP op, PTREE left, signed_64 v2 )
{
    signed_64 test;
    signed_64 v1;
    float_handle t0, t1, t2;

    v1 = left->u.int64_constant;
    switch( op ) {
    case CO_PLUS:
        U64Add( &v1, &v2, &left->u.int64_constant );
        break;
    case CO_MINUS:
        U64Sub( &v1, &v2, &left->u.int64_constant );
        break;
    case CO_TIMES:
        t0 = BFCnvI64F( v1 );
        t1 = BFCnvI64F( v2 );
        t2 = BFMul( t0, t1 );
        test = BFCnvF64( t2 );
        BFFree( t0 );
        BFFree( t1 );
        BFFree( t2 );
        U64Mul( &v1, &v2, &left->u.int64_constant );
        if( 0 != I64Cmp( &test, &left->u.int64_constant ) ) {
            CErr1( ANSI_ARITHMETIC_OVERFLOW );
        }
        break;
    case CO_DIVIDE:
    {   signed_64 rem;
        idiv64( &v1, &v2, &left->u.int64_constant, &rem );
    }   break;
    case CO_PERCENT:
    {   signed_64 div;
        idiv64( &v1, &v2, &div, &left->u.int64_constant );
    }   break;
    case CO_AND:
        left->u.int64_constant.u._32[0] = v1.u._32[0] & v2.u._32[0];
        left->u.int64_constant.u._32[1] = v1.u._32[1] & v2.u._32[1];
        break;
    case CO_OR:
        left->u.int64_constant.u._32[0] = v1.u._32[0] | v2.u._32[0];
        left->u.int64_constant.u._32[1] = v1.u._32[1] | v2.u._32[1];
        break;
    case CO_XOR:
        left->u.int64_constant.u._32[0] = v1.u._32[0] ^ v2.u._32[0];
        left->u.int64_constant.u._32[1] = v1.u._32[1] ^ v2.u._32[1];
        break;
    case CO_RSHIFT:
        I64ShiftR( &v1, v2.u._32[ I64LO32 ], &left->u.int64_constant );
        break;
    case CO_LSHIFT:
        U64ShiftL( &v1, v2.u._32[ I64LO32 ], &left->u.int64_constant );
        break;
    case CO_EQ:
        left = makeBooleanConst( left, 0 == I64Cmp( &v1, &v2 ) );
        return( left );
    case CO_NE:
        left = makeBooleanConst( left, 0 != I64Cmp( &v1, &v2 ) );
        return( left );
    case CO_GT:
        left = makeBooleanConst( left, 0 < I64Cmp( &v1, &v2 )) ;
        return( left );
    case CO_LE:
        left = makeBooleanConst( left, 0 >= I64Cmp( &v1, &v2 ) );
        return( left );
    case CO_LT:
        left = makeBooleanConst( left, 0 > I64Cmp( &v1, &v2 )) ;
        return( left );
    case CO_GE:
        left = makeBooleanConst( left, 0 <= I64Cmp( &v1, &v2 ) );
        return( left );
    case CO_AND_AND:
        left = makeBooleanConst( left, !Zero64( &v1 ) && !Zero64( &v2 ) );
        return( left );
    case CO_OR_OR:
        left = makeBooleanConst( left, !Zero64( &v1) || !Zero64( &v2 ) );
        return( left );
    case CO_COMMA:
        left->u.int64_constant = v2;
        break;
    default:
        return( NULL );
    }
    left->op = PT_INT_CONSTANT;
    return( left );
}

static void udiv64              // DO 64-BIT UNSIGNED DIVISION
    ( unsigned_64 const * v1    // - top
    , unsigned_64 const * v2    // - divisor
    , unsigned_64* result       // - result
    , unsigned_64* rem )        // - remainder
{
    if( v2->u._32[0] == 0
     && v2->u._32[1] == 0 ) {
        CErr1( ERR_DIVISION_BY_ZERO );
        result->u._32[ I64HI32 ] = 0;
        result->u._32[ I64LO32 ] = 1;
        rem->u._32[ I64HI32 ] = 0;
        rem->u._32[ I64LO32 ] = 0;
    } else {
        U64Div( v1, v2, result, rem );
    }
}

static PTREE foldUInt64( CGOP op, PTREE left, signed_64 v2 )
{
    signed_64 test;
    signed_64 v1;
    float_handle t0, t1, t2;

    v1 = left->u.int64_constant;
    switch( op ) {
    case CO_PLUS:
        U64Add( &v1, &v2, &left->u.int64_constant );
        break;
    case CO_MINUS:
        U64Sub( &v1, &v2, &left->u.int64_constant );
        break;
    case CO_TIMES:
        t0 = BFCnvU64F( v1 );
        t1 = BFCnvU64F( v2 );
        t2 = BFMul( t0, t1 );
        test = BFCnvF64( t2 );
        BFFree( t0 );
        BFFree( t1 );
        BFFree( t2 );
        U64Mul( &v1, &v2, &left->u.int64_constant );
        if( 0 != U64Cmp( &test, &left->u.int64_constant ) ) {
            CErr1( ANSI_ARITHMETIC_OVERFLOW );
        }
        break;
    case CO_DIVIDE:
    {   signed_64 rem;
        udiv64( &v1, &v2, &left->u.int64_constant, &rem );
    }   break;
    case CO_PERCENT:
    {   signed_64 div;
        udiv64( &v1, &v2, &div, &left->u.int64_constant );
    }   break;
    case CO_AND:
        left->u.int64_constant.u._32[0] = v1.u._32[0] & v2.u._32[0];
        left->u.int64_constant.u._32[1] = v1.u._32[1] & v2.u._32[1];
        break;
    case CO_OR:
        left->u.int64_constant.u._32[0] = v1.u._32[0] | v2.u._32[0];
        left->u.int64_constant.u._32[1] = v1.u._32[1] | v2.u._32[1];
        break;
    case CO_XOR:
        left->u.int64_constant.u._32[0] = v1.u._32[0] ^ v2.u._32[0];
        left->u.int64_constant.u._32[1] = v1.u._32[1] ^ v2.u._32[1];
        break;
    case CO_RSHIFT:
        U64ShiftR( &v1, v2.u._32[ I64LO32 ], &left->u.int64_constant );
        break;
    case CO_LSHIFT:
        U64ShiftL( &v1, v2.u._32[ I64LO32 ], &left->u.int64_constant );
        break;
    case CO_EQ:
        left = makeBooleanConst( left, 0 == U64Cmp( &v1, &v2 ) );
        return( left );
    case CO_NE:
        left = makeBooleanConst( left, 0 != U64Cmp( &v1, &v2 ) );
        return( left );
    case CO_GT:
        left = makeBooleanConst( left, 0 < U64Cmp( &v1, &v2 )) ;
        return( left );
    case CO_LE:
        left = makeBooleanConst( left, 0 >= U64Cmp( &v1, &v2 ) );
        return( left );
    case CO_LT:
        left = makeBooleanConst( left, 0 > U64Cmp( &v1, &v2 )) ;
        return( left );
    case CO_GE:
        left = makeBooleanConst( left, 0 <= U64Cmp( &v1, &v2 ) );
        return( left );
    case CO_AND_AND:
        left = makeBooleanConst( left, !Zero64( &v1 ) && !Zero64( &v2 ) );
        return( left );
    case CO_OR_OR:
        left = makeBooleanConst( left, !Zero64( &v1) || !Zero64( &v2 ) );
        return( left );
    case CO_COMMA:
        left->u.int64_constant = v2;
        break;
    default:
        return( NULL );
    }
    left->op = PT_INT_CONSTANT;
    return( left );
}


static PTREE pruneExpr(         // PRUNE ONE SIDE FROM EXPRESSION
    PTREE orig,                 // - original expression
    PTREE* ref,                 // - subtree[0] or [1] in orig
    PTREE undec )               // - undecorated result
{
    if( *ref == undec ) {
        *ref = NULL;
    } else {
        (*ref)->u.subtree[1] = NULL;
    }
    undec = PTreeCopySrcLocation( undec, orig );
    NodeFreeDupedExpr( orig );
    return undec;
}

#define isIntFloatOp( op ) \
    (((1L << (op)) & ((1L << PT_INT_CONSTANT) | (1L << PT_FLOATING_CONSTANT))) != 0 )


PTREE FoldBinary( PTREE expr )
/****************************/
{
    PTREE orig1;
    PTREE orig2;
    PTREE op1;
    PTREE op2;
    PTREE op_t;
    PTREE op_f;
    PTREE op_test;
    TYPE type;
    unsigned typ1;
    unsigned typ2;
    bool cast_happened;
    bool has_decoration_left;
    bool has_decoration_right;
    bool has_decoration;

    type = expr->type;
    orig1 = expr->u.subtree[0];
    orig2 = expr->u.subtree[1];
    type = expr->type;
    op1 = overCondDecor( orig1 );
    has_decoration_left = op1 != orig1;
    op2 = overCondDecor( orig2 );
    has_decoration_right = op2 != orig2;
    has_decoration = has_decoration_left | has_decoration_left;
    if( notFoldable( op1 ) ) {
        if( notFoldable( op2 ) ) {
            return( expr );
        }
        switch( expr->cgop ) {
        case CO_EQ:
            DbgVerify( ! has_decoration, "FoldBinary -- bad ==" );
            if( zeroConstant( op2 ) ) {
                if( zeroConstant( op1 ) ) {
                    expr = makeTrueFalse( expr, op1, 1 );
                } else if( nonZeroExpr( op1 ) ) {
                    expr = makeTrueFalse( expr, op1, 0 );
                }
            }
            break;
        case CO_NE:
            DbgVerify( ! has_decoration, "FoldBinary -- bad !=" );
            if( zeroConstant( op2 ) ) {
                if( zeroConstant( op1 ) ) {
                    expr = makeTrueFalse( expr, op1, 0 );
                } else if( nonZeroExpr( op1 ) ) {
                    expr = makeTrueFalse( expr, op1, 1 );
                }
            }
            break;
        case CO_PLUS_EQUAL :
        case CO_MINUS_EQUAL :
        case CO_AND_EQUAL :
        case CO_OR_EQUAL :
        case CO_XOR_EQUAL :
        case CO_EQUAL:
            /* have to be careful with pointer scaling of numbers */
            DbgVerify( ! has_decoration, "FoldBinary -- bad equals" );
            if( ArithType( type ) != NULL ) {
                expr->u.subtree[1] = castConstant( op2, type, &cast_happened );
            }
            break;
        case CO_CONVERT:
            DbgVerify( ! has_decoration, "FoldBinary -- bad convert" );
            op_test = castConstant( op2, type, &cast_happened );
            if( cast_happened ) {
                /* op2 was freed */
                op_test = PTreeCopySrcLocation( op_test, expr );
                NodeFreeDupedExpr( op1 );
                PTreeFree( expr );
                return( op_test );
            }
            break;
        case CO_COMMA :
            //
            // X, c -- can be optimized when X is PT_IC( IC_COND_TRUE )
            // and comma node has PTF_COND_END set
            //
            if( (expr->flags & PTF_COND_END)
             && op1->op == PT_IC
             && op1->u.ic.opcode == IC_COND_TRUE ) {
                expr = pruneExpr( expr, &expr->u.subtree[1], op2 );
            }
            break;
        }
        return( expr );
    }
    if( notFoldable( op2 ) ) {
        switch( expr->cgop ) {
        case CO_EQ:
            DbgVerify( ! has_decoration, "FoldBinary -- bad ==" );
            if( zeroConstant( op1 ) ) {
                if( zeroConstant( op2 ) ) {
                    expr = makeTrueFalse( expr, op2, 1 );
                } else if( nonZeroExpr( op2 ) ) {
                    expr = makeTrueFalse( expr, op2, 0 );
                }
            }
            break;
        case CO_NE:
            DbgVerify( ! has_decoration, "FoldBinary -- bad !=" );
            if( zeroConstant( op1 ) ) {
                if( zeroConstant( op2 ) ) {
                    expr = makeTrueFalse( expr, op2, 0 );
                } else if( nonZeroExpr( op2 ) ) {
                    expr = makeTrueFalse( expr, op2, 1 );
                }
            }
            break;
        case CO_AND_AND:
//          DbgVerify( has_decoration, "FoldBinary -- bad &&" );
            if( ! zeroConstant( op1 ) ) {
                /* 1 && X => X (X is already boolean) */
                expr = pruneExpr( expr, &expr->u.subtree[1], op2 );
            } else {
                /* 0 && X => 0 */
                return pruneExpr( expr, &expr->u.subtree[0], op1 );
            }
            break;
        case CO_OR_OR:
//          DbgVerify( has_decoration, "FoldBinary -- bad ||" );
            if( zeroConstant( op1 ) ) {
                /* 0 || X => X (X is already boolean) */
                return pruneExpr( expr, &expr->u.subtree[1], op2 );
            } else {
                /* 1 || X => 1 */
                return pruneExpr( expr, &expr->u.subtree[0], op1 );
            }
            break;
        case CO_COMMA:
            /* c , X => X */
//          DbgVerify( ! has_decoration, "FoldBinary -- bad comma" );
            expr->u.subtree[1] = NULL;
            op2 = PTreeCopySrcLocation( op2, expr );
            NodeFreeDupedExpr( expr );
            return( op2 );
        case CO_QUESTION:
            DbgVerify( ! has_decoration, "FoldBinary -- bad ?" );
            op_t = op2->u.subtree[0];
            op_f = op2->u.subtree[1];
            has_decoration = isCondDecor( op_t );
            DbgVerify( has_decoration == isCondDecor( op_f )
                     , "FoldBinary -- bad ?:" );
            if( has_decoration ) {
                op_t = op_t->u.subtree[1];
                op_f = op_f->u.subtree[1];
            }
            if( ! zeroConstant( op1 ) ) {
                /* 1 ? T : F => T */
                if( has_decoration ) {
                    op2->u.subtree[0]->u.subtree[1] = NULL;
                } else {
                    op2->u.subtree[0] = NULL;
                }
                op2 = op_t;
            } else {
                /* 0 ? T : F => F */
                if( has_decoration ) {
                    op2->u.subtree[1]->u.subtree[1] = NULL;
                } else {
                    op2->u.subtree[1] = NULL;
                }
                op2 = op_f;
            }
            op2 = PTreeCopySrcLocation( op2, expr );
            NodeFreeDupedExpr( expr );
            return( op2 );
        }
    } else {
        typ1 = op1->op;
        typ2 = op2->op;
        if( ! isIntFloatOp( typ1 ) || ! isIntFloatOp( typ2 ) ) {
            // (void)0 can make it here
            return expr;
        }
        if( typ1 != typ2 ) {
            if( PT_FLOATING_CONSTANT == typ1 ) {
                if( NULL == Integral64Type( op2->type ) ) {
                    if( SignedIntType( op2->type ) ) {
                        op2->u.floating_constant
                            = BFCnvIF( op2->u.int_constant );
                    } else {
                        op2->u.floating_constant
                            = BFCnvUF( op2->u.uint_constant );
                    }
                } else {
                    if( SignedIntType( op2->type ) ) {
                        op2->u.floating_constant
                            = BFCnvI64F( op2->u.int64_constant );
                    } else {
                        op2->u.floating_constant
                            = BFCnvU64F( op2->u.int64_constant );
                    }
                }
                typ2 = PT_FLOATING_CONSTANT;
                op2->op = typ2;
            } else {
                if( NULL == Integral64Type( op1->type ) ) {
                    if( SignedIntType( op1->type ) ) {
                        op1->u.floating_constant
                            = BFCnvIF( op1->u.int_constant );
                    } else {
                        op1->u.floating_constant
                            = BFCnvUF( op1->u.uint_constant );
                    }
                } else {
                    if( SignedIntType( op1->type ) ) {
                        op1->u.floating_constant
                            = BFCnvI64F( op1->u.int64_constant );
                    } else {
                        op1->u.floating_constant
                            = BFCnvU64F( op1->u.int64_constant );
                    }
                }
                typ1 = PT_FLOATING_CONSTANT;
                op1->op = typ2;
            }
        }
        if( PT_FLOATING_CONSTANT == typ1 ) {
            op1 = foldFloating( expr->cgop, op1, op2->u.floating_constant );
        } else if( SignedIntType( op1->type ) ) {
            if( NULL == Integral64Type( op1->type )
             && NULL == Integral64Type( op2->type ) ) {
                op1 = foldInt( expr->cgop, op1, op2->u.int_constant );
            } else {
                op1 = foldInt64( expr->cgop, op1, op2->u.int64_constant );
            }
        } else {
            if( NULL == Integral64Type( op1->type )
             && NULL == Integral64Type( op2->type ) ) {
                op1 = foldUInt( expr->cgop, op1, op2->u.uint_constant );
            } else {
                op1 = foldUInt64( expr->cgop, op1, op2->u.int64_constant );
            }
        }
        if( op1 != NULL ) {
            /* binary op was folded! */
            if( has_decoration ) {
                orig1->u.subtree[1] = NULL;
                orig2->u.subtree[1] = NULL;
            } else {
                expr->u.subtree[0] = NULL;
            }
            op1 = castConstant( op1, type, &cast_happened );
            op1 = PTreeCopySrcLocation( op1, expr );
            NodeFreeDupedExpr( expr );
            return op1;
        }
    }
    return( expr );
}



PTREE Fold( PTREE expr )        // Fold expression
/**********************/
{
    switch( expr->op ) {
      case PT_UNARY :
        if( expr->u.subtree[0] != NULL ) {
            expr = FoldUnary( expr );
        }
        break;
      case PT_BINARY :
        if( expr->u.subtree[0] != NULL
         && expr->u.subtree[1] != NULL ) {
            expr = FoldBinary( expr );
        }
        break;
    }
    return expr;
}
