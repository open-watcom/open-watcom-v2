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


#include "i64.h"
#include "cvars.h"

extern  int     __Strtold(char *,long_double *,char **);

uint_32 DoOp32( uint_32 left, opr_code opr, uint_32 right, bool sign )
{
    uint_32              value;

    switch( opr ) {
    case OPR_ADD:
        value =  left + right;
        break;
    case OPR_SUB:
        value =  left - right;
        break;
    case OPR_MUL:
        value =  left * right;
        break;
    case OPR_DIV:
        if( right == 0 ) {                 /* 10-mar-90 */
            value = 0;
        } else {
            if( sign ){
                value =  (int_32)left / (int_32)right;
            }else{
                value =  left / right;
            }
        }
        break;
    case OPR_MOD:
        if( right == 0 ) {                 /* 10-mar-90 */
            value = 0;
        } else {
            if( sign ){
                value = (int_32)left % (int_32)right;
            }else{
                value = left % right;
            }
        }
        break;
    case OPR_RSHIFT:
        if( sign ){
            value = (int_32)left >> (int_32)right;
        }else{
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
        value = - right;
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
    uint64              tmp;
    uint64              value;
    int                 shift;

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
        if( U64Cmp( &right, &value ) != 0 ){
            if( sign ){
                I64Div( &left, &right, &value, &tmp );
            }else{
                U64Div( &left, &right, &value, &tmp );
            }
        }
        break;
    case OPR_MOD:
        U32ToU64( 0, &value );
        if( U64Cmp( &right, &value ) != 0 ){
            if( sign ){
                I64Div( &left, &right, &tmp, &value );
            }else{
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
        if( shift > 0 ){
            if( sign ){
                I64ShiftR( &left, shift, &value );
            }else{
                U64ShiftR( &left, shift, &value );
            }
        }else{
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
        if( U64Cmp( &left, &value ) || U64Cmp( &right, &value ) ){
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
        if( U64Cmp( &left, &value ) && U64Cmp( &right, &value ) ){
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
        if( U64Cmp( &right, &value ) == 0 ){
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
    int_32        value;
    int_32        left;
    int_32        right;
    DATA_TYPE   const_type;

    const_type = tree->expr_type->decl_type;
    if( op1 != NULL ){
        left = op1->op.long_value;
    }
    right = op2->op.long_value;
    if( tree->op.opr == OPR_CMP ) {
        switch( tree->op.cc ) {
        case CC_EQ:
            value = ( left == right );
            break;
        case CC_NE:
            value = ( left != right );
            break;
        case CC_GT:
            value = ( left > right );
            break;
        case CC_GE:
            value = ( left >= right );
            break;
        case CC_LT:
            value = ( left < right );
            break;
        case CC_LE:
            value = ( left <= right );
            break;
        }
        const_type = TYPE_INT;
        tree->op.long_value = (target_int)value;
    }else{
        value = DoOp32( left, tree->op.opr, right, TRUE );
        if( const_type == TYPE_LONG ) {
            tree->op.long_value = value;
        } else {
            tree->op.long_value = (target_int)value;
        }
    }
    tree->op.opr = OPR_PUSHINT;
    tree->op.const_type = const_type;
    tree->left = NULL;
    tree->right = NULL;
    FreeExprNode( op1 );
    FreeExprNode( op2 );
    return( 1 );
}


int DoUnSignedOp( TREEPTR op1, TREEPTR tree, TREEPTR op2 )
/********************************************************/
{
    uint_32       value;
    uint_32       left;
    uint_32       right;
    DATA_TYPE           const_type;

    const_type = tree->expr_type->decl_type;
    if( op1 != NULL ){
        left = op1->op.ulong_value;
    }
    right = op2->op.ulong_value;
    if( tree->op.opr == OPR_CMP ) {
        switch( tree->op.cc ) {
        case CC_EQ:
            value = ( left == right );
            break;
        case CC_NE:
            value = ( left != right );
            break;
        case CC_GT:
            value = ( left > right );
            break;
        case CC_GE:
            value = ( left >= right );
            break;
        case CC_LT:
            value = ( left < right );
            break;
        case CC_LE:
            value = ( left <= right );
            break;
        }
        const_type = TYPE_INT;
        tree->op.long_value = (target_int)value;
    }else{
        value = DoOp32( left, tree->op.opr, right, FALSE );
        if( const_type == TYPE_ULONG ) {
            tree->op.long_value = value;
        } else {
            tree->op.long_value = (target_uint)value;
        }
    }
    tree->op.opr = OPR_PUSHINT;
    tree->op.const_type = const_type;
    tree->left = NULL;
    tree->right = NULL;
    FreeExprNode( op1 );
    FreeExprNode( op2 );
    return( 1 );
}

static int64  LongValue64( TREEPTR leaf )
{
    int64        value;
    int_32         val32;
    FLOATVAL    *flt;
    char        *endptr;
    long_double ld;
    bool        sign;

    switch( leaf->op.const_type ) {
    case TYPE_CHAR:
        sign = TRUE;
        val32 = (signed char)leaf->op.ulong_value;
        break;
    case TYPE_UCHAR:
        sign = FALSE;
        val32 = (unsigned char)leaf->op.ulong_value;
        break;
    case TYPE_SHORT:
        sign = TRUE;
        val32 = (target_short)leaf->op.ulong_value;
        break;
    case TYPE_USHORT:
        sign = FALSE;
        val32 = (target_ushort)leaf->op.ulong_value;
        break;
    case TYPE_INT:
        sign = TRUE;
        val32 = (target_int)leaf->op.ulong_value;
        break;
    case TYPE_UINT:
        sign = FALSE;
        val32 = (target_uint)leaf->op.ulong_value;
        break;
    case TYPE_LONG:
        sign = TRUE;
        val32 = (target_long)leaf->op.ulong_value;
        break;
    case TYPE_POINTER:
    case TYPE_ULONG:
        sign = FALSE;
        val32 = (target_ulong)leaf->op.ulong_value;
        break;
    case TYPE_LONG64:
    case TYPE_ULONG64:
        return( leaf->op.ulong64_value );
        break;
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
        sign = TRUE;
        flt = leaf->op.float_value;
        if( flt->len == 0 ) {
            ld = flt->ld;
        } else {
            __Strtold( flt->string, &ld, &endptr );
        }
        CMemFree( flt );
#ifdef _LONG_DOUBLE_
        val32 = __LDI4( (long_double near *)&ld );
#else
        val32 = ld.value;
#endif
        break;
    default:
        val32 = 0;
        break;
    }
    if( sign ){
        I32ToI64( val32, &value );
    }else{
        U32ToU64( val32, &value );
    }
    return( value );
}


int DoUnSignedOp64( TREEPTR op1, TREEPTR tree, TREEPTR op2 )
/********************************************************/
{
    uint64          value;
    uint64          left;
    uint64          right;
    int             tmp;
    DATA_TYPE       const_type;

    if( op1 != NULL ){
        left =  LongValue64( op1 );
    }
    right = LongValue64( op2 );
    const_type = tree->expr_type->decl_type;
    if( tree->op.opr == OPR_CMP ) {
        tmp = U64Cmp( &left, &right );
        switch( tree->op.cc ) {
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
        tree->op.long_value = (target_int)tmp;
    }else{
        value = DoOp64( left, tree->op.opr, right, FALSE );
        if( const_type  == TYPE_ULONG64 ){
            tree->op.long64_value = value;
        }else{
            tree->op.ulong_value = value.u._32[L];
        }
    }
    tree->op.opr = OPR_PUSHINT;
    tree->op.const_type = const_type;
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

    if( op1 != NULL ){
        left =  LongValue64( op1 );
    }
    right = LongValue64( op2 );
    const_type = tree->expr_type->decl_type;
    if( tree->op.opr == OPR_CMP ) {
        tmp = I64Cmp( &left, &right );
        switch( tree->op.cc ) {
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
        tree->op.long_value = (target_int)tmp;
    }else{
        value = DoOp64( left, tree->op.opr, right, TRUE );
        if( const_type  == TYPE_LONG64 ){
            tree->op.long64_value = value;
        }else{
            tree->op.long_value = value.u._32[L];
        }
    }
    tree->op.opr = OPR_PUSHINT;
    tree->op.const_type = const_type;
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
#endif

    if( leaf->op.opr == OPR_PUSHFLOAT ) {
        flt = leaf->op.float_value;
        if( flt->len == 0 ) {           // if contains binary value
            ld = flt->ld;
        } else {
            __Strtold( flt->string, &ld, &endptr );
        }
    } else {    // integer
        switch( leaf->op.const_type ) {
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_INT:
        case TYPE_LONG:
#ifdef _LONG_DOUBLE_
            __I4LD( leaf->op.long_value, (long_double near *)&ld );
#else
            ld.value = (double)leaf->op.long_value;
#endif
            break;
        default:
#ifdef _LONG_DOUBLE_
            __U4LD( leaf->op.long_value, (long_double near *)&ld );
#else
            ld.value = (double)leaf->op.ulong_value;
#endif
            break;
        }
        flt = CMemAlloc( sizeof(FLOATVAL) );
        leaf->op.float_value = flt;
        leaf->op.opr = OPR_PUSHFLOAT;
    }
    leaf->op.const_type = newtype;
    switch( newtype ) {
    case TYPE_FLOAT:
#ifdef _LONG_DOUBLE_
        __LDFS( (long_double near *)&ld, (float near *)&floatval );
        __FSLD( (float near *)&floatval, (long_double near *)&ld );
#else
        ld.value = (float)ld.value;
#endif
        break;
    case TYPE_DOUBLE:
#ifdef _LONG_DOUBLE_
        __LDFD( (long_double near *)&ld, (double near *)&doubleval );
        __FDLD( (double near *)&doubleval, (long_double near *)&ld );
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

    if( opnd->op.opr == OPR_PUSHINT ) {
        CastFloatValue( opnd, TYPE_DOUBLE );
    }
    flt = opnd->op.float_value;
    if( flt->len != 0 ) {
        __Strtold( flt->string, &ld, &endptr );
        flt->ld = ld;
        flt->len = 0;
        flt->string[0] = '\0';
    }
}

int FltCmp( long_double near *ld1, long_double near *ld2 )
{
#ifdef _LONG_DOUBLE_
    return( __FLDC( ld1, ld2 ) );
#else
    if( ld1->value == ld2->value ) return( 0 );
    if( ld1->value <  ld2->value ) return( -1 );
    return( 1 );
#endif
}

int DoFloatOp( TREEPTR op1, TREEPTR tree, TREEPTR op2 )
/*****************************************************/
{
    int         value;
    int         cond;
    long_double ld1;
    long_double ld2;
    long_double result;

    // load ld1 and ld2 from op1 and op2
    if( op1 != NULL ) {                 // if not unary op
        MakeBinaryFloat( op1 );
        ld1 = op1->op.float_value->ld;
    }
    MakeBinaryFloat( op2 );
    ld2 = op2->op.float_value->ld;
    switch( tree->op.opr ) {
    case OPR_CMP:
        cond = FltCmp( (long_double near *)&ld1, (long_double near *)&ld2 );
        switch( tree->op.cc ) {
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
        }
        tree->op.opr = OPR_PUSHINT;
        tree->op.ulong_value = value;
        tree->op.const_type = TYPE_INT;
        tree->expr_type = GetType( TYPE_INT );
        tree->left = NULL;
        tree->right = NULL;
        FreeExprNode( op1 );
        FreeExprNode( op2 );
        return( 1 );
    case OPR_ADD:
#ifdef _LONG_DOUBLE_
        __FLDA( (long_double near *)&ld1,
                (long_double near *)&ld2,
                (long_double near *)&result );
#else
        result.value = ld1.value + ld2.value;
#endif
        break;
    case OPR_SUB:
#ifdef _LONG_DOUBLE_
        __FLDS( (long_double near *)&ld1,
                (long_double near *)&ld2,
                (long_double near *)&result );
#else
        result.value = ld1.value - ld2.value;
#endif
        break;
    case OPR_MUL:
#ifdef _LONG_DOUBLE_
        __FLDM( (long_double near *)&ld1,
                (long_double near *)&ld2,
                (long_double near *)&result );
#else
        result.value = ld1.value * ld2.value;
#endif
        break;
    case OPR_DIV:
#ifdef _LONG_DOUBLE_
        if( ld2.exponent == 0 && ld2.high_word == 0 && ld2.low_word == 0 ) {
            result = ld2;
        } else {
            __FLDD( (long_double near *)&ld1,
                    (long_double near *)&ld2,
                    (long_double near *)&result );
        }
#else
        if( ld2.value == 0.0 ) {
            result.value = 0.0;
        } else {
            result.value = ld1.value / ld2.value;
        }
#endif
        break;
    case OPR_NEG:
#ifdef _LONG_DOUBLE_
        result = ld2;
        result.exponent ^= 0x8000;
#else
        result.value = - ld2.value;
#endif
        break;
    default:
        return( 0 );
    }
    // should the result be forced into a double or float?
    tree->op.opr = OPR_PUSHFLOAT;
    tree->op.float_value = op2->op.float_value;
    tree->op.float_value->ld = result;
    tree->op.const_type = TYPE_DOUBLE;
    tree->expr_type = GetType( TYPE_DOUBLE );
    tree->left = NULL;
    tree->right = NULL;
    FreeExprNode( op1 );
    // we reused op2->op.float_value, so change op2->op.opr so that
    // FreeExprNode doesn't try to free op2->op.float_value
    op2->op.opr = OPR_NOP;
    FreeExprNode( op2 );
    return( 1 );
}

static int_32 LongValue( TREEPTR leaf )
{
    int_32        value;
    FLOATVAL    *flt;
    char        *endptr;
    long_double ld;

    switch( leaf->op.const_type ) {
    case TYPE_CHAR:
        value = (signed char)leaf->op.ulong_value;
        break;
    case TYPE_UCHAR:
        value = (unsigned char)leaf->op.ulong_value;
        break;
    case TYPE_SHORT:
        value = (target_short)leaf->op.ulong_value;
        break;
    case TYPE_USHORT:
        value = (target_ushort)leaf->op.ulong_value;
        break;
    case TYPE_INT:
        value = (target_int)leaf->op.ulong_value;
        break;
    case TYPE_UINT:
        value = (target_uint)leaf->op.ulong_value;
        break;
    case TYPE_LONG:
        value = (target_long)leaf->op.ulong_value;
        break;
    case TYPE_ULONG:
    case TYPE_POINTER:
        value = (target_ulong)leaf->op.ulong_value;
        break;
    case TYPE_LONG64:
        value = (target_long)leaf->op.ulong64_value.u._32[L];
        break;
    case TYPE_ULONG64:
        value = (target_ulong)leaf->op.ulong64_value.u._32[L];
        break;
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
        flt = leaf->op.float_value;
        if( flt->len == 0 ) {
            ld = flt->ld;
        } else {
            __Strtold( flt->string, &ld, &endptr );
        }
        CMemFree( flt );
#ifdef _LONG_DOUBLE_
        value = __LDI4( (long_double near *)&ld );
#else
        value = ld.value;
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

    oldtyp = leaf->op.const_type;

    if( (newtyp == TYPE_DOUBLE || newtyp == TYPE_FLOAT)
     && (oldtyp == TYPE_DOUBLE || oldtyp == TYPE_FLOAT) ){
        CastFloatValue( leaf, newtyp );  // float to float
        return;
    }else if( newtyp == TYPE_LONG64 || newtyp == TYPE_ULONG64 ){
        val64 = LongValue64( leaf );
        leaf->op.ulong64_value = val64;
    }else{
        val32 = LongValue( leaf );
        switch( newtyp ) {
        case TYPE_CHAR:
            leaf->op.ulong_value = (signed char)val32;
            break;
        case TYPE_UCHAR:
            leaf->op.ulong_value = (unsigned char)val32;
            break;
        case TYPE_SHORT:
            leaf->op.ulong_value = (target_short)val32;
            break;
        case TYPE_USHORT:
            leaf->op.ulong_value = (target_ushort)val32;
            break;
        case TYPE_INT:
            leaf->op.ulong_value = (target_int)val32;
            break;
        case TYPE_UINT:
            leaf->op.ulong_value = (target_uint)val32;
            break;
        case TYPE_LONG:
            leaf->op.ulong_value = (target_long)val32;
            break;
        case TYPE_ULONG:
            leaf->op.ulong_value = (target_ulong)val32;
            break;
        case TYPE_FLOAT:
        case TYPE_DOUBLE:
            CastFloatValue( leaf, newtyp );
            return;
        default:
            return;
        }
    }
    leaf->op.opr = OPR_PUSHINT;
    leaf->op.const_type = newtyp;
}

void FoldQuestionTree( TREEPTR tree )
{
    TREEPTR     node;
    TREEPTR     true_part;
    TREEPTR     false_part;
    op_flags    ops;

    node = tree->right;                 // point to OPR_COLON node
    true_part  = node->left;
    false_part = node->right;
    FreeExprNode( node );
    if( tree->left->op.long_value == 0 ) {
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
    if( ops & OPFLAG_RVALUE ){
        if( tree->op.opr == OPR_PUSHADDR ){
          tree->op.opr = OPR_PUSHSYM;
          ops &= ~OPFLAG_RVALUE;
        }
    }
    tree->op.flags = ops;
}


static bool ConstantLeaf( TREEPTR opnd )
{
    if( opnd->op.opr == OPR_PUSHINT  || opnd->op.opr == OPR_PUSHFLOAT ) {
        return( TRUE );
    }else{
        return( FALSE );
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
        return( ConstantLeaf( tree->left ) && ConstantLeaf( tree->right ) );
    case OPR_NEG:
    case OPR_COM:
    case OPR_NOT:
        return( ConstantLeaf( tree->right ) );
    case OPR_CONVERT:
        opnd = tree->right;
        if( opnd->op.opr == OPR_PUSHINT || opnd->op.opr == OPR_PUSHFLOAT ) {
            typ = tree->expr_type;
            CastConstValue( opnd, typ->decl_type );
            *tree = *opnd;
            tree->expr_type = typ;
            opnd->op.opr = OPR_NOP;
            FreeExprNode( opnd );
        }
        break;
    case OPR_COMMA:
        opnd = tree->left;
        if( opnd->op.opr == OPR_PUSHINT || opnd->op.opr == OPR_PUSHFLOAT ) {
            FreeExprNode( opnd );
            opnd = tree->right;
            *tree = *opnd;
            opnd->op.opr = OPR_NOP;
            FreeExprNode( opnd );
        }
        break;
    case OPR_QUESTION:
        if( tree->left->op.opr == OPR_PUSHINT ) {
            FoldQuestionTree( tree );
        }
        break;
    case OPR_ADDROF:                    // look for offsetof() pattern
        offset = 0;
        opnd = tree->right;
        while( opnd->op.opr == OPR_DOT ) {
            offset += opnd->right->op.long_value;
            opnd = opnd->left;
        }
        if( opnd->op.opr == OPR_ARROW ) {
            offset += opnd->right->op.long_value;
            opnd = opnd->left;
            if( opnd->op.opr == OPR_PUSHINT ) {
                offset += opnd->op.long_value;
                opnd = tree->right;
                tree->op.opr = OPR_PUSHINT;
                tree->op.long_value = offset;
                tree->op.const_type = TYPE_UINT;
                tree->expr_type = GetType( TYPE_UINT );
                tree->right = NULL;
                FreeExprTree( opnd );
            }
        }
        break;
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
}arithmetic_type;

static  arithmetic_type ArithmeticType( DATA_TYPE decl_type )
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

void DoConstFold( TREEPTR tree )
{
    DATA_TYPE   decl_type;

    if( FoldableTree( tree ) ){
        arithmetic_type    con;
        if( tree->op.opr == OPR_CMP ){
            decl_type = tree->op.compare_type->decl_type;
        }else{
            decl_type = tree->expr_type->decl_type;
        }
        con = ArithmeticType( decl_type );
        switch( con ){
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
    uint64      tmp;
    bool         ret;

    ConstExprAndType( &val );
    //Must be int
    if( val.type == TYPE_LONG64 || val.type == TYPE_ULONG64 ){
        U32ToU64( 0, &tmp );
        if( U64Cmp( &tmp, &val.val64 ) != 0 ){
            ret = TRUE;
        }else{
            ret = FALSE;
        }
    }else{
        if( val.val32 != 0 ){
            ret = TRUE;
        }else{
            ret = FALSE;
        }
    }
    return( ret );
}
