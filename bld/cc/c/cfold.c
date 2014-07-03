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
* Description:  Expression tree folding (compile time math).
*
****************************************************************************/


#include "cvars.h"
#include "i64.h"

static bool IsConstantZero( TREEPTR tree );

uint_32 DoOp32( uint_32 left, opr_code opr, uint_32 right, bool sign )
{
    uint_32         value;

    switch( opr ) {
    case OPR_ADD:
        value = left + right;
        break;
    case OPR_SUB:
        value = left - right;
        break;
    case OPR_MUL:
        value = left * right;
        break;
    case OPR_DIV:
        if( right == 0 ) {                 /* 10-mar-90 */
            value = 0;
        } else {
            if( sign ) {
                value = (int_32)left / (int_32)right;
            } else {
                value = left / right;
            }
        }
        break;
    case OPR_MOD:
        if( right == 0 ) {                 /* 10-mar-90 */
            value = 0;
        } else {
            if( sign ) {
                value = (int_32)left % (int_32)right;
            } else {
                value = left % right;
            }
        }
        break;
    case OPR_RSHIFT:
        if( sign ) {
            value = (int_32)left >> (int_32)right;
        } else {
            value = left >> right;
        }
        break;
    case OPR_LSHIFT:
        value = left << right;
        break;
    case OPR_OR:
        value = left | right;
        break;
    case OPR_OR_OR:
        value = left || right;
        break;
    case OPR_AND:
        value = left & right;
        break;
    case OPR_AND_AND:
        value = left && right;
        break;
    case OPR_XOR:
        value = left ^ right;
        break;
    case OPR_NEG:
        value = - (int_32)right;
        break;
    case OPR_COM:
        value = ~ right;
        break;
    case OPR_NOT:
        value = ! right;
        break;
    default:
        value = 0;
    }
    return( value );
}


uint64 DoOp64( uint64 left, opr_code opr, uint64 right, bool sign )
{
    uint64          tmp;
    uint64          value;
    int             shift;

    switch( opr ) {
    case OPR_ADD:
        U64Add( &left, &right, &value );
        break;
    case OPR_SUB:
        U64Sub( &left, &right, &value );
        break;
    case OPR_MUL:
        U64Mul( &left, &right, &value );
        break;
    case OPR_DIV:
        U32ToU64( 0, &value );
        if( U64Cmp( &right, &value ) != 0 ) {
            if( sign ) {
                I64Div( &left, &right, &value, &tmp );
            } else {
                U64Div( &left, &right, &value, &tmp );
            }
        }
        break;
    case OPR_MOD:
        U32ToU64( 0, &value );
        if( U64Cmp( &right, &value ) != 0 ) {
            if( sign ) {
                I64Div( &left, &right, &tmp, &value );
            } else {
                U64Div( &left, &right, &tmp, &value );
            }
        }
        break;
    case OPR_LSHIFT:
        shift = right.u._32[L];
        shift = -shift;
        goto do_shift;
    case OPR_RSHIFT:
        shift = right.u._32[L];
    do_shift:
        if( shift > 0 ) {
            if( sign ) {
                I64ShiftR( &left, shift, &value );
            } else {
                U64ShiftR( &left, shift, &value );
            }
        } else {
            shift = -shift;
            U64ShiftL( &left, shift, &value );
        }
        break;
    case OPR_OR:
        value = left;
        value.u._32[L] |= right.u._32[L];
        value.u._32[H] |= right.u._32[H];
        break;
    case OPR_OR_OR:
        U32ToU64( 0, &value );
        if( U64Cmp( &left, &value ) || U64Cmp( &right, &value ) ) {
            U32ToU64( 1, &value );
        }
        break;
    case OPR_AND:
        value = left;
        value.u._32[L] &= right.u._32[L];
        value.u._32[H] &= right.u._32[H];
        break;
    case OPR_AND_AND:
        U32ToU64( 0, &value );
        if( U64Cmp( &left, &value ) && U64Cmp( &right, &value ) ) {
            U32ToU64( 1, &value );
        }
        break;
    case OPR_XOR:
        value = left;
        value.u._32[L] ^= right.u._32[L];
        value.u._32[H] ^= right.u._32[H];
        break;
    case OPR_NEG:
        U64Neg( &right, &value );
//      value = - right;
        break;
    case OPR_COM:
        value.u._32[L] = ~right.u._32[L];
        value.u._32[H] = ~right.u._32[H];
        break;
    case OPR_NOT:
        U32ToU64( 0, &value );
        if( U64Cmp( &right, &value ) == 0 ) {
            U32ToU64( 1, &value );
        }
        break;
    default:
        U32ToU64( 0, &value );
    }
    return( value );
}

int DoSignedOp( TREEPTR op1, TREEPTR tree, TREEPTR op2 )
/******************************************************/
{
    int_32          value;
    int_32          left;
    int_32          right;
    DATA_TYPE       const_type;

    left = 0;
    const_type = tree->u.expr_type->decl_type;
    if( op1 != NULL ) {
        left = op1->op.u2.long_value;
    }
    right = op2->op.u2.long_value;
    if( tree->op.opr == OPR_CMP ) {
        switch( tree->op.u1.cc ) {
        case CC_EQ:
            value = (left == right);
            break;
        case CC_NE:
            value = (left != right);
            break;
        case CC_GT:
            value = (left > right);
            break;
        case CC_GE:
            value = (left >= right);
            break;
        case CC_LT:
            value = (left < right);
            break;
        case CC_LE:
            value = (left <= right);
            break;
        default:
            assert( 0 );
            value = 0;
            break;
        }
        const_type = TYPE_INT;
        tree->op.u2.long_value = (target_int)value;
    } else {
        value = DoOp32( left, tree->op.opr, right, TRUE );
        if( const_type == TYPE_LONG ) {
            tree->op.u2.long_value = value;
        } else {
            tree->op.u2.long_value = (target_int)value;
        }
    }
    tree->op.opr = OPR_PUSHINT;
    tree->op.u1.const_type = const_type;
    tree->left = NULL;
    tree->right = NULL;
    FreeExprNode( op1 );
    FreeExprNode( op2 );
    return( 1 );
}


int DoUnSignedOp( TREEPTR op1, TREEPTR tree, TREEPTR op2 )
/********************************************************/
{
    uint_32         value;
    uint_32         left;
    uint_32         right;
    DATA_TYPE       const_type;

    left = 0;
    const_type = tree->u.expr_type->decl_type;
    if( op1 != NULL ) {
        left = op1->op.u2.ulong_value;
    }
    right = op2->op.u2.ulong_value;
    if( tree->op.opr == OPR_CMP ) {
        switch( tree->op.u1.cc ) {
        case CC_EQ:
            value = (left == right);
            break;
        case CC_NE:
            value = (left != right);
            break;
        case CC_GT:
            value = (left > right);
            break;
        case CC_GE:
            value = (left >= right);
            break;
        case CC_LT:
            value = (left < right);
            break;
        case CC_LE:
            value = (left <= right);
            break;
        default:
            assert( 0 );
            value = 0;
            break;
        }
        const_type = TYPE_INT;
        tree->op.u2.long_value = (target_int)value;
    } else {
        value = DoOp32( left, tree->op.opr, right, FALSE );
        if( const_type == TYPE_ULONG || const_type == TYPE_POINTER ) {
            tree->op.u2.ulong_value = value;
        } else {
            tree->op.u2.ulong_value = (target_uint)value;
        }
    }
    tree->op.opr = OPR_PUSHINT;
    tree->op.u1.const_type = const_type;
    tree->left = NULL;
    tree->right = NULL;
    FreeExprNode( op1 );
    FreeExprNode( op2 );
    return( 1 );
}


int64 LongValue64( TREEPTR leaf )
/*******************************/
{
    int64           value;
    int_32          val32;
    FLOATVAL        *flt;
    char            *endptr;
    long_double     ld;
    bool            sign;

    switch( leaf->op.u1.const_type ) {
    case TYPE_CHAR:
        sign = TRUE;
        val32 = (signed char)leaf->op.u2.ulong_value;
        break;
    case TYPE_UCHAR:
        sign = FALSE;
        val32 = (unsigned char)leaf->op.u2.ulong_value;
        break;
    case TYPE_SHORT:
        sign = TRUE;
        val32 = (target_short)leaf->op.u2.ulong_value;
        break;
    case TYPE_USHORT:
        sign = FALSE;
        val32 = (target_ushort)leaf->op.u2.ulong_value;
        break;
    case TYPE_INT:
        sign = TRUE;
        val32 = (target_int)leaf->op.u2.ulong_value;
        break;
    case TYPE_UINT:
        sign = FALSE;
        val32 = (target_uint)leaf->op.u2.ulong_value;
        break;
    case TYPE_LONG:
        sign = TRUE;
        val32 = (target_long)leaf->op.u2.ulong_value;
        break;
    case TYPE_POINTER:
    case TYPE_ULONG:
        sign = FALSE;
        val32 = (target_ulong)leaf->op.u2.ulong_value;
        break;
    case TYPE_LONG64:
    case TYPE_ULONG64:
        return( leaf->op.u2.ulong64_value );
        break;
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_LONG_DOUBLE:
        sign = TRUE;
        flt = leaf->op.u2.float_value;
        if( flt->len == 0 ) {
            ld = flt->ld;
        } else {
            __Strtold( flt->string, &ld, &endptr );
        }
        CMemFree( flt );
#ifdef _LONG_DOUBLE_
        __LDI8( &ld, &value );
        return( value );
#elif ( _INTEGRAL_MAX_BITS >= 64 )
        value.u._64[0] = ld.u.value;
        return( value );
#else
        val32 = ld.value;
#endif
        break;
    default:
        sign = FALSE;
        val32 = 0;
        break;
    }
    if( sign ) {
        I32ToI64( val32, &value );
    } else {
        U32ToU64( val32, &value );
    }
    return( value );
}


int DoUnSignedOp64( TREEPTR op1, TREEPTR tree, TREEPTR op2 )
/**********************************************************/
{
    uint64          value;
    uint64          left;
    uint64          right;
    int             tmp;
    DATA_TYPE       const_type;

    if( op1 != NULL ) {
        left = LongValue64( op1 );
    } else {
        U32ToU64( 0, &left );
    }
    right = LongValue64( op2 );
    const_type = tree->u.expr_type->decl_type;
    if( tree->op.opr == OPR_CMP ) {
        tmp = U64Cmp( &left, &right );
        switch( tree->op.u1.cc ) {
        case CC_EQ:
            tmp = tmp == 0;
            break;
        case CC_NE:
            tmp = tmp != 0;
            break;
        case CC_GT:
            tmp = tmp > 0;
            break;
        case CC_LE:
            tmp = tmp <= 0;
            break;
        case CC_GE:
            tmp = tmp >= 0;
            break;
        case CC_LT:
            tmp = tmp < 0;
            break;
        }
        const_type = TYPE_INT;
        tree->op.u2.long_value = (target_int)tmp;
    } else {
        value = DoOp64( left, tree->op.opr, right, FALSE );
        if( const_type  == TYPE_ULONG64 ) {
            tree->op.u2.long64_value = value;
        } else {
            tree->op.u2.ulong_value = value.u._32[L];
        }
    }
    tree->op.opr = OPR_PUSHINT;
    tree->op.u1.const_type = const_type;
    tree->left = NULL;
    tree->right = NULL;
    FreeExprNode( op1 );
    FreeExprNode( op2 );
    return( 1 );
}


int DoSignedOp64( TREEPTR op1, TREEPTR tree, TREEPTR op2 )
/********************************************************/
{
    int64           value;
    int64           left;
    int64           right;
    int             tmp;
    DATA_TYPE       const_type;

    if( op1 != NULL ) {
        left = LongValue64( op1 );
    } else {
        U32ToU64( 0, &left );
    }
    right = LongValue64( op2 );
    const_type = tree->u.expr_type->decl_type;
    if( tree->op.opr == OPR_CMP ) {
        tmp = I64Cmp( &left, &right );
        switch( tree->op.u1.cc ) {
        case CC_EQ:
            tmp = tmp == 0;
            break;
        case CC_NE:
            tmp = tmp != 0;
            break;
        case CC_GT:
            tmp = tmp > 0;
            break;
        case CC_LE:
            tmp = tmp <= 0;
            break;
        case CC_GE:
            tmp = tmp >= 0;
            break;
        case CC_LT:
            tmp = tmp < 0;
            break;
        }
        const_type = TYPE_INT;
        tree->op.u2.long_value = (target_int)tmp;
    } else {
        value = DoOp64( left, tree->op.opr, right, TRUE );
        if( const_type  == TYPE_LONG64 ) {
            tree->op.u2.long64_value = value;
        } else {
            tree->op.u2.long_value = (signed_32)value.u._32[L];
        }
    }
    tree->op.opr = OPR_PUSHINT;
    tree->op.u1.const_type = const_type;
    tree->left = NULL;
    tree->right = NULL;
    FreeExprNode( op1 );
    FreeExprNode( op2 );
    return( 1 );
}

void CastFloatValue( TREEPTR leaf, DATA_TYPE newtype )
{
    FLOATVAL    *flt;
    char        *endptr;
    long_double ld;
#ifdef _LONG_DOUBLE_
    double      doubleval;
    float       floatval;
    int64       value;
#endif

    if( leaf->op.opr == OPR_PUSHFLOAT ) {
        flt = leaf->op.u2.float_value;
        if( flt->len == 0 ) {           // if contains binary value
            ld = flt->ld;
        } else {
            __Strtold( flt->string, &ld, &endptr );
        }
    } else {    // integer
        switch( leaf->op.u1.const_type ) {
        case TYPE_LONG64:
#ifdef _LONG_DOUBLE_
            value = leaf->op.u2.long64_value;
            __I8LD( &value, &ld );
#elif ( _INTEGRAL_MAX_BITS >= 64 )
            ld.u.value = (double)leaf->op.u2.long64_value.u._64[0];
#else

    #error not implemented for compiler with integral max bits < 64
            ld.value = 0;
#endif
            break;
        case TYPE_ULONG64:
#ifdef _LONG_DOUBLE_
            value = leaf->op.u2.long64_value;
            __U8LD( &value, &ld );
#elif ( _INTEGRAL_MAX_BITS >= 64 )
            ld.u.value = (double)leaf->op.u2.ulong64_value.u._64[0];
#else

    #error not implemented for compiler with integral max bits < 64
            ld.value = 0;
#endif
            break;
        //signed types
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_INT:
        case TYPE_LONG:
#ifdef _LONG_DOUBLE_
            __I4LD( leaf->op.u2.long_value, &ld );
#else
            ld.u.value = (double)leaf->op.u2.long_value;
#endif
            break;
        default:
        //unsigned types
#ifdef _LONG_DOUBLE_
            __U4LD( leaf->op.u2.ulong_value, &ld );
#else
            ld.u.value = (double)leaf->op.u2.ulong_value;
#endif
            break;
        }
        flt = CMemAlloc( sizeof(FLOATVAL) );
        leaf->op.u2.float_value = flt;
        leaf->op.opr = OPR_PUSHFLOAT;
    }
    leaf->op.u1.const_type = newtype;
    switch( newtype ) {
    case TYPE_FLOAT:
#ifdef _LONG_DOUBLE_
        __iLDFS( &ld, &floatval );
        __iFSLD( &floatval, &ld );
#else
        ld.u.value = (float)ld.u.value;
#endif
        break;
    case TYPE_DOUBLE:
#ifdef _LONG_DOUBLE_
        __iLDFD( &ld, &doubleval );
        __iFDLD( &doubleval, &ld );
#endif
        break;
    }
    flt->len = 0;
    flt->type = newtype;
    flt->ld = ld;
    flt->string[0] = '\0';
}


void MakeBinaryFloat( TREEPTR opnd )
{
    FLOATVAL    *flt;
    char        *endptr;
    long_double ld;
#ifdef _LONG_DOUBLE_
    double      doubleval;
    float       floatval;
#endif

    if( opnd->op.opr == OPR_PUSHINT ) {
        CastFloatValue( opnd, TYPE_DOUBLE );
    }
    flt = opnd->op.u2.float_value;
    if( flt->len != 0 ) {
        __Strtold( flt->string, &ld, &endptr );
        if( flt->type == TYPE_FLOAT ) {
#ifdef _LONG_DOUBLE_
            __iLDFS( &ld, &floatval );
            __iFSLD( &floatval, &ld );
#else
            ld.u.value = (float)ld.u.value;
#endif
        } else if( flt->type == TYPE_DOUBLE ) {
#ifdef _LONG_DOUBLE_
        __iLDFD( &ld, &doubleval );
        __iFDLD( &doubleval, &ld );
#endif
        }
        flt->ld = ld;
        flt->len = 0;
        flt->string[0] = '\0';
    }
}


int FltCmp( ld_arg ld1, ld_arg ld2 )
{
#ifdef _LONG_DOUBLE_
    return( __FLDC( ld1, ld2 ) );
#else
    if( ld1->u.value == ld2->u.value )
        return( 0 );
    if( ld1->u.value <  ld2->u.value )
        return( -1 );
    return( 1 );
#endif
}


static int DoFloatOp( TREEPTR op1, TREEPTR tree, TREEPTR op2 )
/************************************************************/
{
    int         value;
    int         cond;
    long_double ld1;
    long_double ld2;
    long_double result;
    TYPEPTR     typ1;
    TYPEPTR     typ2;

    // load ld1 and ld2 from op1 and op2
    MakeBinaryFloat( op2 );
    ld2 = op2->op.u2.float_value->ld;
    typ2 = TypeOf( op2 );
    if( op1 != NULL ) {                 // if not unary op
        MakeBinaryFloat( op1 );
        ld1 = op1->op.u2.float_value->ld;
        typ1 = TypeOf( op1 );
    } else {
#ifdef _LONG_DOUBLE_
        __I4LD( 0, &ld1 );
#else
        ld1.u.value = 0;
#endif
        typ1 = TypeOf( op2 );           // default to same type
    }

    switch( tree->op.opr ) {
    case OPR_CMP:
        cond = FltCmp( &ld1, &ld2 );
        switch( tree->op.u1.cc ) {
        case CC_EQ:
            value = (cond == 0);
            break;
        case CC_NE:
            value = (cond != 0);
            break;
        case CC_GT:
            value = (cond > 0);
            break;
        case CC_GE:
            value = (cond >= 0);
            break;
        case CC_LT:
            value = (cond < 0);
            break;
        case CC_LE:
            value = (cond <= 0);
            break;
        default:
            assert( 0 );
            value = 0;
            break;
        }
        tree->op.opr = OPR_PUSHINT;
        tree->op.u2.ulong_value = value;
        tree->op.u1.const_type = TYPE_INT;
        tree->u.expr_type = GetType( TYPE_INT );
        tree->left = NULL;
        tree->right = NULL;
        FreeExprNode( op1 );
        FreeExprNode( op2 );
        return( 1 );
    case OPR_ADD:
#ifdef _LONG_DOUBLE_
        __FLDA( &ld1, &ld2, &result );
#else
        result.u.value = ld1.u.value + ld2.u.value;
#endif
        break;
    case OPR_SUB:
#ifdef _LONG_DOUBLE_
        __FLDS( &ld1, &ld2, &result );
#else
        result.u.value = ld1.u.value - ld2.u.value;
#endif
        break;
    case OPR_MUL:
#ifdef _LONG_DOUBLE_
        __FLDM( &ld1, &ld2, &result );
#else
        result.u.value = ld1.u.value * ld2.u.value;
#endif
        break;
    case OPR_DIV:
#ifdef _LONG_DOUBLE_
        if( ld2.exponent == 0 && ld2.high_word == 0 && ld2.low_word == 0 ) {
            result = ld2;
        } else {
            __FLDD( &ld1, &ld2, &result );
        }
#else
        if( ld2.u.value == 0.0 ) {
            result.u.value = 0.0;
        } else {
            result.u.value = ld1.u.value / ld2.u.value;
        }
#endif
        break;
    case OPR_NEG:
#ifdef _LONG_DOUBLE_
        result = ld2;
        result.exponent ^= 0x8000;
#else
        result.u.value = - ld2.u.value;
#endif
        break;
    default:
        return( 0 );
    }
    /* The expression type must obey the usual arithmetic conversions. */
    tree->op.opr = OPR_PUSHFLOAT;
    tree->op.u2.float_value = op2->op.u2.float_value;
    tree->op.u2.float_value->ld = result;
    tree->op.u1.const_type = TYPE_DOUBLE;
    tree->u.expr_type = GetType( BinExprType( typ1, typ2 ) );
    tree->left = NULL;
    tree->right = NULL;
    FreeExprNode( op1 );
    // we reused op2->op.u2.float_value, so change op2->op.opr so that
    // FreeExprNode doesn't try to free op2->op.u2.float_value
    op2->op.opr = OPR_NOP;
    FreeExprNode( op2 );
    return( 1 );
}

static int_32 LongValue( TREEPTR leaf )
{
    int_32      value;
    FLOATVAL    *flt;
    char        *endptr;
    long_double ld;

    switch( leaf->op.u1.const_type ) {
    case TYPE_CHAR:
        value = (signed char)leaf->op.u2.ulong_value;
        break;
    case TYPE_UCHAR:
        value = (unsigned char)leaf->op.u2.ulong_value;
        break;
    case TYPE_SHORT:
        value = (target_short)leaf->op.u2.ulong_value;
        break;
    case TYPE_USHORT:
        value = (target_ushort)leaf->op.u2.ulong_value;
        break;
    case TYPE_INT:
        value = (target_int)leaf->op.u2.ulong_value;
        break;
    case TYPE_UINT:
        value = (target_uint)leaf->op.u2.ulong_value;
        break;
    case TYPE_LONG:
        value = (target_long)leaf->op.u2.ulong_value;
        break;
    case TYPE_ULONG:
    case TYPE_POINTER:
        value = (target_ulong)leaf->op.u2.ulong_value;
        break;
    case TYPE_LONG64:
        value = (target_long)leaf->op.u2.ulong64_value.u._32[L];
        break;
    case TYPE_ULONG64:
        value = (target_ulong)leaf->op.u2.ulong64_value.u._32[L];
        break;
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
        flt = leaf->op.u2.float_value;
        if( flt->len == 0 ) {
            ld = flt->ld;
        } else {
            __Strtold( flt->string, &ld, &endptr );
        }
        CMemFree( flt );
#ifdef _LONG_DOUBLE_
        value = __LDI4( &ld );
#else
        value = ld.u.value;
#endif
        break;
    default:
        value = 0;
        break;
    }
    return( value );
}


void CastConstValue( TREEPTR leaf, DATA_TYPE newtyp )
{
    int_32      val32;
    int64       val64;
    DATA_TYPE   oldtyp;

    oldtyp = leaf->op.u1.const_type;

    if( (newtyp == TYPE_DOUBLE || newtyp == TYPE_FLOAT || newtyp == TYPE_LONG_DOUBLE)
     && (oldtyp == TYPE_DOUBLE || oldtyp == TYPE_FLOAT || oldtyp == TYPE_LONG_DOUBLE) ) {
        CastFloatValue( leaf, newtyp );  // float to float
        return;
    } else if( newtyp == TYPE_LONG64 || newtyp == TYPE_ULONG64 ) {
        val64 = LongValue64( leaf );
        leaf->op.u2.ulong64_value = val64;
    } else if( newtyp == TYPE_BOOL ) {
        leaf->op.u2.ulong_value = IsConstantZero( leaf ) ? 0 : 1;
        newtyp = TYPE_UCHAR;
    } else {
        val32 = LongValue( leaf );
        switch( newtyp ) {
        case TYPE_CHAR:
            leaf->op.u2.ulong_value = (signed char)val32;
            break;
        case TYPE_UCHAR:
            leaf->op.u2.ulong_value = (unsigned char)val32;
            break;
        case TYPE_SHORT:
            leaf->op.u2.ulong_value = (target_short)val32;
            break;
        case TYPE_USHORT:
            leaf->op.u2.ulong_value = (target_ushort)val32;
            break;
        case TYPE_INT:
            leaf->op.u2.ulong_value = (target_int)val32;
            break;
        case TYPE_UINT:
            leaf->op.u2.ulong_value = (target_uint)val32;
            break;
        case TYPE_LONG:
            leaf->op.u2.ulong_value = (target_long)val32;
            break;
        case TYPE_ULONG:
            leaf->op.u2.ulong_value = (target_ulong)val32;
            break;
        case TYPE_POINTER:
            leaf->op.u2.ulong_value = (target_ulong)val32;
            break;
        case TYPE_FLOAT:
        case TYPE_DOUBLE:
        case TYPE_LONG_DOUBLE:
            CastFloatValue( leaf, newtyp );
            return;
        default:
            return;
        }
    }
    leaf->op.opr = OPR_PUSHINT;
    leaf->op.u1.const_type = newtyp;
}

void CastConstNode( TREEPTR leaf, TYPEPTR newtyp )
{
    CastConstValue( leaf, newtyp->decl_type );
    if( newtyp->decl_type == TYPE_POINTER ) {
        leaf->op.flags = OpFlags( newtyp->u.p.decl_flags );
        // This really ought to be in CastConstValue, but that
        // function can't figure out the exact pointer size
        if ( TypeSize( newtyp ) == TARGET_SHORT ) {
            leaf->op.u2.ulong_value = (target_ushort)leaf->op.u2.ulong_value;
        }
    }
}

static bool IsConstantZero( TREEPTR tree )
{
    if( tree->op.opr == OPR_PUSHINT ) {
        uint64      val64;

        val64 = LongValue64( tree );
        return( !U64Test( &val64 ) );
    } else {
        FLOATVAL    *flt;
        char        *endptr;
        long_double ld;
        long_double ldzero;

        assert( tree->op.opr == OPR_PUSHFLOAT );
        flt = tree->op.u2.float_value;
        if( flt->len == 0 ) {           // if contains binary value
            ld = flt->ld;
        } else {
            __Strtold( flt->string, &ld, &endptr );
        }
#ifdef _LONG_DOUBLE_
        __I4LD( 0, &ldzero );
#else
        ldzero.u.value = 0;
#endif
        return( !FltCmp( &ld, &ldzero ) );
    }
}


static void FoldQuestionTree( TREEPTR tree )
{
    TREEPTR     node;
    TREEPTR     true_part;
    TREEPTR     false_part;
    op_flags    ops;

    node = tree->right;                 // point to OPR_COLON node
    true_part  = node->left;
    false_part = node->right;
    FreeExprNode( node );
    if( IsConstantZero( tree->left ) ) {
        node = false_part;              // want false_part
        FreeExprTree( true_part );
    } else {
        node = true_part;               // want true_part
        FreeExprTree( false_part );
    }
    ops = tree->op.flags;
    FreeExprNode( tree->left );
    *tree = *node;
    node->op.opr = OPR_NOP;
    FreeExprNode( node );
    if( ops & OPFLAG_RVALUE ) {
        if( tree->op.opr == OPR_PUSHADDR ) {
          tree->op.opr = OPR_PUSHSYM;
          ops &= ~OPFLAG_RVALUE;
        }
        tree->op.flags = ops;
    }
}


static bool FoldableTree( TREEPTR tree )
{
    TREEPTR     opnd;
    TYPEPTR     typ;
    unsigned    offset;

    switch( tree->op.opr ) {
    case OPR_ADD:
    case OPR_SUB:
    case OPR_MUL:
    case OPR_DIV:
    case OPR_CMP:
    case OPR_MOD:
    case OPR_OR:
    case OPR_AND:
    case OPR_XOR:
    case OPR_RSHIFT:
    case OPR_LSHIFT:
    case OPR_OR_OR:
    case OPR_AND_AND:
        return( IsConstLeaf( tree->left ) && IsConstLeaf( tree->right ) );
    case OPR_NEG:
    case OPR_COM:
    case OPR_NOT:
        return( IsConstLeaf( tree->right ) );
    case OPR_CONVERT:
        opnd = tree->right;
        if( IsConstLeaf( opnd ) ) {
            typ = tree->u.expr_type;
            CastConstNode( opnd, typ );
            *tree = *opnd;
            tree->u.expr_type = typ;
            opnd->op.opr = OPR_NOP;
            FreeExprNode( opnd );
        }
        break;
    case OPR_RETURN:
        opnd = tree->right;
        if( IsConstLeaf( opnd ) ) {
            CastConstNode( opnd, tree->u.expr_type );
        }
        break;
    case OPR_COMMA:
        opnd = tree->left;
        if( IsConstLeaf( opnd ) ) {
            FreeExprNode( opnd );
            opnd = tree->right;
            *tree = *opnd;
            opnd->op.opr = OPR_NOP;
            FreeExprNode( opnd );
        }
        break;
    case OPR_QUESTION:
        opnd = tree->left;
        if( opnd->op.opr == OPR_ADDROF
          && (opnd->right->op.opr == OPR_PUSHADDR || opnd->right->op.opr == OPR_PUSHSYM) ) {
            SYM_ENTRY   sym;

            SymGet( &sym, opnd->right->op.u2.sym_handle );
            if( sym.attribs.stg_class != SC_AUTO && sym.attribs.stg_class != SC_REGISTER ) {
                /* &(static object) is known to be non-zero */
                /* replace it by a 1 */
                FreeExprNode( opnd->right );
                FreeExprNode( opnd );
                tree->left = UIntLeaf( 1 );
                opnd = tree->left;
            }
        }
        if( IsConstLeaf( opnd ) ) {
            FoldQuestionTree( tree );
        }
        break;
    case OPR_ADDROF:                    // look for offsetof() pattern
        offset = 0;
        opnd = tree->right;
        while( opnd->op.opr == OPR_DOT ) {
            offset += opnd->right->op.u2.long_value;
            opnd = opnd->left;
        }
        if( opnd->op.opr == OPR_ARROW ) {
            offset += opnd->right->op.u2.long_value;
            opnd = opnd->left;
            if( opnd->op.opr == OPR_PUSHINT ) {
                offset += opnd->op.u2.long_value;
                opnd = tree->right;
                tree->op.opr = OPR_PUSHINT;
                tree->op.u2.long_value = offset;
                tree->op.u1.const_type = TYPE_UINT;
                tree->u.expr_type = GetType( TYPE_UINT );
                tree->right = NULL;
                FreeExprTree( opnd );
            }
        }
        break;
#if _CPU == 8086
    /* The :> operator is currently only folded for 16-bit targets. The
     * folding itself can handle 16:32 pointers, but other places in cfe
     * and cg don't understand larger than 32-bit pointer constants.
     */
    case OPR_FARPTR:
        if( IsConstLeaf( tree->left ) && IsConstLeaf( tree->right ) ) {
            uint64      seg_val;
            uint64      off_val;
            uint64      value;

            seg_val = LongValue64( tree->left );
            off_val = LongValue64( tree->right );
            U64ShiftL( &seg_val, TARGET_NEAR_POINTER * 8, &value );
            U64Or( &value, &off_val, &value );
            tree->op.u2.ulong64_value = value;
            tree->op.opr = OPR_PUSHINT;
            tree->op.u1.const_type = TYPE_POINTER;
            tree->op.flags |= OPFLAG_FARPTR;
            tree->left = NULL;
            tree->right = NULL;
            FreeExprNode( tree->left );
            FreeExprNode( tree->right );
        }
        break;
#endif
    default:
        break;
    }
    return( FALSE );
}

typedef enum {
    SIGNED_INT,
    UNSIGNED_INT,
    SIGNED_INT64,
    UNSIGNED_INT64,
    FLOATING,
    NOT_A_NUMBER
} arithmetic_type;

static arithmetic_type ArithmeticType( DATA_TYPE decl_type )
{
    switch( decl_type ) {
    case TYPE_CHAR:
    case TYPE_SHORT:
    case TYPE_INT:
    case TYPE_LONG:
        return( SIGNED_INT );
    case TYPE_LONG64:
        return( SIGNED_INT64 );
    case TYPE_UCHAR:
    case TYPE_USHORT:
    case TYPE_UINT:
    case TYPE_ULONG:
    case TYPE_POINTER:
        return( UNSIGNED_INT );
    case TYPE_ULONG64:
        return( UNSIGNED_INT64 );
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
        return( FLOATING );
    }
    return( NOT_A_NUMBER );
}


/* Check values of constant operands of certain operators. Needs to be done
 * after all sub-trees are folded (the checked operands may be result of
 * constant folding) but before potentially constant folding the examined
 * operator.
 */
static void CheckOpndValues( TREEPTR tree )
{
    TYPEPTR             r_type;
    TREEPTR             opnd;
    arithmetic_type     con;

    if( tree->checked )
        return;

    switch( tree->op.opr ) {
    case OPR_LSHIFT:
    case OPR_RSHIFT:
    case OPR_LSHIFT_EQUAL:
    case OPR_RSHIFT_EQUAL:
        if( IsConstLeaf( tree->right ) ) {
            bool    shift_too_big = FALSE;
            bool    shift_negative = FALSE;
            int     max_shift;

            opnd = tree->right;
            r_type = opnd->u.expr_type;
            con = ArithmeticType( r_type->decl_type );

            // shift arguments undergo integral promotion; 'char c = 1 << 10;'
            // is not undefined, though it will overflow
            max_shift = max( SizeOfArg( tree->left->u.expr_type ),
                        SizeOfArg( GetType( TYPE_INT ) ) ) * 8;
            switch( con ) {
            case SIGNED_INT: {
                int_32      right;

                right = opnd->op.u2.long_value;
                if( right < 0 )
                    shift_negative = TRUE;
                else if( right >= max_shift )
                    shift_too_big = TRUE;
                break;
                }
            case SIGNED_INT64: {
                int64       right;
                int64       big_shift;

                right = LongValue64( opnd );
                I32ToI64( max_shift, &big_shift );
                if( I64Test( &right ) < 0 )
                    shift_negative = TRUE;
                else if( I64Cmp( &right, &big_shift ) >= 0 )
                    shift_too_big = TRUE;
                break;
                }
            case UNSIGNED_INT:
                if( (uint_32)opnd->op.u2.long_value >= (uint_32)max_shift )
                    shift_too_big = TRUE;
                break;
            case UNSIGNED_INT64: {
                uint64      right;
                uint64      big_shift;

                right = LongValue64( opnd );
                U32ToU64( max_shift, &big_shift );
                if( U64Cmp( &right, &big_shift ) >= 0 )
                    shift_too_big = TRUE;
                break;
                }
            default:
                // Not supposed to happen!
                break;
            }
            if( shift_negative ) {
                CWarn1( WARN_SHIFT_AMOUNT_NEGATIVE, ERR_SHIFT_AMOUNT_NEGATIVE );
            } else if( shift_too_big ) {
                CWarn1( WARN_SHIFT_AMOUNT_TOO_BIG, ERR_SHIFT_AMOUNT_TOO_BIG );
            }
        }
        break;
    case OPR_DIV:
    case OPR_MOD:
        if( IsConstLeaf( tree->right ) ) {
            bool    zero_divisor = FALSE;

            opnd = tree->right;
            r_type = opnd->u.expr_type;
            con = ArithmeticType( r_type->decl_type );

            switch( con ) {
            case SIGNED_INT:
            case UNSIGNED_INT:
                if( opnd->op.u2.long_value == 0 )
                    zero_divisor = TRUE;
                break;
            case SIGNED_INT64:
            case UNSIGNED_INT64: {
                uint64      right;

                right = LongValue64( opnd );
                if( U64Test( &right ) == 0 )
                    zero_divisor = TRUE;
                break;
                }
            case FLOATING:
                // Should we warn here? Floating-point division by zero is
                // not necessarily undefined...
                break;
            default:
                // Not supposed to happen?
                break;
            }
            if( zero_divisor ) {
                CWarn1( WARN_DIV_BY_ZERO, ERR_DIV_BY_ZERO );
            }
        }
        break;
    default:
        break;
    }
    tree->checked = TRUE;
}


void DoConstFold( TREEPTR tree )
{
    DATA_TYPE   decl_type;

    CheckOpndValues( tree );
    if( FoldableTree( tree ) ) {
        arithmetic_type     con;

        if( tree->op.opr == OPR_CMP ) {
            decl_type = tree->op.u2.compare_type->decl_type;
        } else {
            decl_type = tree->u.expr_type->decl_type;
        }
        con = ArithmeticType( decl_type );
        switch( con ) {
        case SIGNED_INT:
            DoSignedOp( tree->left, tree, tree->right );
            break;
        case SIGNED_INT64:
            DoSignedOp64( tree->left, tree, tree->right );
            break;
        case UNSIGNED_INT:
            DoUnSignedOp( tree->left, tree, tree->right );
            break;
        case UNSIGNED_INT64:
            DoUnSignedOp64( tree->left, tree, tree->right );
            break;
        case FLOATING:
            DoFloatOp( tree->left, tree, tree->right );
            break;
        }
    }
}


void FoldExprTree( TREEPTR tree )
{
    WalkExprTree( tree, NoOp, NoOp, NoOp, DoConstFold );
}


bool BoolConstExpr( void )
{
    const_val   val;

    ConstExprAndType( &val );
    return( U64Test( &val.value ) != 0 );
}
