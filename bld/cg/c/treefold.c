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
* Description:  Processor independent constant folding optimizations.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cgdefs.h"
#include "tree.h"
#include "cfloat.h"
#include "zoiks.h"
#include "i64.h"
#include "data.h"
#include "types.h"
#include "treefold.h"
#include "treeprot.h"
#include "bldins.h"
#include "utils.h"
#include "namelist.h"
#include "feprotos.h"

typedef union i32 {
    signed_32   s;
    unsigned_32 u;
} i32;

extern  an              TreeGen(tn);
extern  unsigned_32     U32ModDiv(unsigned_32*,unsigned_32);
extern  void            BurnTree(tn);

#define HasBigConst( t )       ( ( (t)->attr & TYPE_FLOAT ) || (t)->length == 8 )

static cg_op RevOpcode[] = {
    O_EQ,    /* O_EQ*/
    O_NE,    /* O_NE*/
    O_LT,    /* O_GT*/
    O_GE,    /* O_LE*/
    O_GT,    /* O_LT*/
    O_LE     /* O_GE*/
};

/* CheckCmpRange is based on code stolen from CheckMeaninglessCompare(),
 * used by the C++ front end. It is quite handy to be able to perform the
 * folding inside the cg, especially since the C++ front end doesn't do it!
 */

#define SIGN_BIT        (0x8000)
#define NumSign( a )    ((a) & SIGN_BIT)
#define NumBits( a )    ((a) & 0x7fff)

typedef enum {
    CMP_VOID    = 0,    /* comparison can't be folded */
    CMP_FALSE   = 1,
    CMP_TRUE    = 2,
} cmp_result;

typedef enum {
    REL_EQ,    // x == c
    REL_LT,    // x < c
    REL_LE,    // x <= c
    REL_SIZE
} rel_op;

//  a <= x <=  b   i.e range of x is between a and b
enum  case_range {
    CASE_LOW,         // c < a
    CASE_LOW_EQ,      // c == a
    CASE_HIGH,        // c > b
    CASE_HIGH_EQ,     // c == b
    CASE_SIZE
};

static char const CmpResult[REL_SIZE][CASE_SIZE] = {
//    c < a      c == a     c > b      c == b
    { CMP_FALSE, CMP_VOID , CMP_FALSE, CMP_VOID },  // x == c
    { CMP_FALSE, CMP_FALSE, CMP_TRUE , CMP_VOID },  // x < c
    { CMP_FALSE, CMP_VOID , CMP_TRUE , CMP_TRUE },  // x <= c
};

#define MAXSIZE         64      /* Must not be greater! */

static int CmpType( type_def *tipe )
/**********************************/
/* Convert cg type to a value used by CheckCmpRange */
{
    int     ret;

    ret =  tipe->length * 8;
    if( tipe->attr & TYPE_SIGNED )
        ret |= SIGN_BIT;
    return( ret );
}

static cmp_result CheckCmpRange( cg_op op, int op_type, float_handle val )
/************************************************************************/
/* Check if comparison 'op' of operand of type 'op_type' against constant
 * 'val' can be folded, eg. '(unsigned char)x <= 255'. Integer only, can
 * be used for bitfields (op_type contains number of bits).
 */
{
    enum case_range     range;
    cmp_result          ret;
    signed_64           low;
    signed_64           high;
    signed_64           konst;
    rel_op              rel;
    bool                rev_ret = false;

    /* Map cg rel ops to equivalent cases */
    switch( op ) {
    case O_NE:
        rev_ret = true;
    case O_EQ:
        rel = REL_EQ;
        break;
    case O_GE:
        rev_ret = true;
    case O_LT:
        rel = REL_LT;
        break;
    case O_GT:
        rev_ret = true;
    case O_LE:
        rel = REL_LE;
        break;
    default:
        rel = 0;
        _Zoiks( ZOIKS_112 );
    }
    /* Determine type range */
    if( NumSign( op_type ) ) {
        U64Set( &low, 0, 0x80000000 );
        I64ShiftR( &low, MAXSIZE - NumBits( op_type ), &low );
        U64Not( &low, &high );
    } else {
        U64Clear( low );
        U64Not( &low, &high );
        U64ShiftR( &high, MAXSIZE - NumBits( op_type ), &high );
    }
    /* Determine how to compare */
    konst = CFCnvF64( val );
    if( I64Cmp( &konst, &low ) == 0 ) {
        range = CASE_LOW_EQ;
    } else if( I64Cmp( &konst, &high) == 0 ) {
        range = CASE_HIGH_EQ;
    } else if( NumBits( op_type ) < MAXSIZE ) { /* Can't be outside range */
        if( I64Cmp( &konst, &low ) < 0 ) {      /* Don't need unsigned compare */
            range = CASE_LOW;
        } else if( I64Cmp( &konst, &high ) > 0 ) {
            range = CASE_HIGH;
        } else {
            range = CASE_SIZE;
        }
    } else {
        range = CASE_SIZE;
    }
    /* Figure out comparison result, if possible */
    if( range != CASE_SIZE ) {
        ret = CmpResult[rel][range];
        if( rev_ret && (ret != CMP_VOID) ) {
            /* Flip result */
            if( ret == CMP_FALSE ) {
                ret = CMP_TRUE;
            } else {
                ret = CMP_FALSE;
            }
        }
    } else {
        ret = CMP_VOID;
    }
    return( ret );
}

static signed_32 CFConvertByType( float_handle cf, type_def *tipe )
/*****************************************************************/
{
    signed_32   data;

    data = CFCnvF32( cf );
    switch( tipe->length ) {
    case 1:
        if( tipe->attr & TYPE_SIGNED ) {
            data = (signed_8)data;
        } else {
            data = (unsigned_8)data;
        }
        break;
    case 2:
        if( tipe->attr & TYPE_SIGNED ) {
            data = (signed_16)data;
        } else {
            data = (unsigned_16)data;
        }
        break;
    }
    return( data );
}

static signed_64 CFGetInteger64Value( float_handle cf )
/*****************************************************/
{
    signed_64       value;
    int             neg;
    float_handle    trunc;

    trunc = CFTrunc( cf );
    neg = CFTest( trunc );
    if( neg < 0 ) CFNegate( trunc );
    value = CFCnvF64( trunc );
    if( neg < 0 ) U64Neg( &value, &value );
    CFFree( trunc );
    return( value );
}

static  float_handle IntToCF( signed_64 value, type_def *tipe )
/*********************************************************/
{
    signed_8    s8;
    unsigned_8  u8;
    signed_16   s16;
    unsigned_16 u16;
    signed_32   s32;
    unsigned_32 u32;

    if( tipe->attr & TYPE_SIGNED ) {
        switch( tipe->length ) {
        case 1:
            s8 = value.u._8[I64LO8];
            return( CFCnvI32F( s8 ) );
        case 2:
            s16 = value.u._16[I64LO16];
            return( CFCnvI32F( s16 ) );
        case 4:
        case 6:
            s32 = value.u._32[I64LO32];
            return( CFCnvI32F( s32 ) );
        case 8:
            return( CFCnvI64F( value.u._32[I64LO32], value.u._32[I64HI32] ) );
        default:
            _Zoiks( ZOIKS_112 );
            return( NULL );
        }
    } else {
        switch( tipe->length ) {
        case 1:
            u8 = value.u._8[I64LO8];
            return( CFCnvU32F( u8 ) );
        case 2:
            u16 = value.u._16[I64LO16];
            return( CFCnvU32F( u16 ) );
        case 4:
        case 6:
            u32 = value.u._32[I64LO32];
            return( CFCnvU32F( u32 ) );
        case 8:
            return( CFCnvU64F( value.u._32[I64LO32], value.u._32[I64HI32] ) );
        default:
            _Zoiks( ZOIKS_112 );
            return( NULL );
        }
    }
}

static  tn      IntToType( signed_32 value, type_def *tipe )
/**********************************************************/
{
    signed_64   temp;

    I32ToI64( value, &temp );
    return( TGConst( IntToCF( temp, tipe ), tipe ) );
}


static  tn      Int64ToType( signed_64 value, type_def *tipe )
/************************************************************/
{
    return( TGConst( IntToCF( value, tipe ), tipe ) );
}


static  tn      CFToType( float_handle cf, type_def *tipe )
/*********************************************************/
{
    tn          result;

    if( ( tipe->attr & TYPE_FLOAT ) == EMPTY ) {
        result = TGConst( IntToCF( CFGetInteger64Value( cf ), tipe ), tipe );
        CFFree( cf );
    } else {
        result = TGConst( cf, tipe );
    }
    return( result );
}



int     GetLog2( unsigned_32 value )
/******************************************/
{
    unsigned_32     count;
    int             log;

    if( _IsPowerOfTwo( value ) && value != 0 ) {
        log = 0;
        count = 1;
        for( ;; ) {
            if( count == value ) return( log );
            count += count;
            ++log;
        }
    }
    return( -1 );
}


tn      FoldTimes( tn left, tn rite, type_def *tipe )
/***********************************************************/
{
    tn              temp;
    tn              fold;
    int             test;
    int             log;
    float_handle    lv;
    float_handle    rv;
    unsigned_32     li;
    unsigned_32     ri;

    if( left->class == TN_CONS ) {
        temp = left;
        left = rite;
        rite = temp;
    }
    if( rite->class != TN_CONS ) return( NULL );
    if( left->class == TN_BINARY && left->u2.t.op == O_TIMES &&
        tipe == left->tipe && !HasBigConst( tipe ) ) {
        if( left->u.left->class == TN_CONS ) {
            left->u.left = FoldTimes( left->u.left, rite, tipe );
            return( left );
        }
        if( left->u2.t.rite->class == TN_CONS ) {
            left->u2.t.rite = FoldTimes( left->u2.t.rite, rite, tipe );
            return( left );
        }
    }
    if( left->class==TN_BINARY && left->u2.t.op==O_LSHIFT && tipe==left->tipe ) {
        if( !HasBigConst( tipe ) && left->u2.t.rite->class == TN_CONS ) {
            li = 1;
            for( log = left->u2.t.rite->u.name->c.lo.int_value; log > 0; --log ) {
                li <<= 1;
            }
            BurnTree( left->u2.t.rite );
            left->u2.t.rite = CFToType( CFCnvU32F( li ), tipe );
            left->u2.t.op = O_TIMES;
            left->u2.t.rite = FoldTimes( left->u2.t.rite, rite, tipe );
            return( left );
        }
    }
    rv = rite->u.name->c.value;
    if( left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        if( !HasBigConst( tipe ) && CFIs32( lv ) && CFIs32( rv ) ) {
            li = CFConvertByType( lv, tipe );
            ri = CFConvertByType( rv, tipe );
            ri = li * ri;
            fold = IntToType( ri, tipe );
        } else {
            fold = CFToType( CFMul( left->u.name->c.value, rv ), tipe );
        }
        BurnTree( left );
        BurnTree( rite );
        return( fold );
    }
    if( !HasBigConst( tipe ) && rite->u.name->c.lo.int_value == 1 ) {
        BurnTree( rite );
        return( left );
    }
    test = CFTest( rv );
    if( test == 0 ) {
        return( TGBinary( O_COMMA, TGTrash( left ), rite, tipe ) );
    }
    if( HasBigConst( tipe ) ) return( NULL );
    if( test < 0 ) {
        CFNegate( rv );
    }
    fold = NULL;
    if( CFIsU32( rv ) ) {
        log = GetLog2( CFConvertByType( rv, tipe ) );
        if( log != -1 ) {
            fold = TGBinary( O_LSHIFT, left,
                          IntToType( log, TypeInteger ), tipe );
            if( test < 0 ) {
                fold = TGUnary( O_UMINUS, fold, tipe );
            }
            BurnTree( rite );
        }
    }
    if( test < 0 ) {
        CFNegate( rv );
    }
    return( fold );
}


float_handle OkToNegate( float_handle value, type_def *tipe )
/**********************************************************/
/* make sure we don't negate an unsigned and get out of range */
/* for example -MAX_LONG is no longer an integer type */
{
    float_handle    neg;

    if( HasBigConst( tipe ) && ( tipe->attr & TYPE_FLOAT ) == 0 ) return( NULL );
    neg = CFCopy( value );
    CFNegate( neg );
    if( HasBigConst( tipe ) ) return( neg );
    if( tipe->attr & TYPE_SIGNED ) return( neg );
    if( CFIsSize( neg,   tipe->length ) ) return( neg );
    CFFree( neg );
    return( NULL );
}

tn      FoldMinus( tn left, tn rite, type_def *tipe )
/***********************************************************/
{
    tn              fold;
    float_handle    lv;
    float_handle    rv;
    unsigned_32     li;
    unsigned_32     ri;

    fold = NULL;
    if( left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        if( rite->class == TN_CONS ) {
            rv = rite->u.name->c.value;
            if( !HasBigConst( tipe ) && CFIs32( lv ) && CFIs32( rv ) ) {
                li = CFConvertByType( lv, tipe );
                ri = CFConvertByType( rv, tipe );
                ri = li - ri;
                fold = IntToType( ri, tipe );
            } else {
                fold = CFToType( CFSub( lv, rite->u.name->c.value ),
                                  tipe );
            }
            BurnTree( rite );
            BurnTree( left );
        } else if( CFTest( lv ) == 0 ) {
            fold = TGUnary( O_UMINUS, rite, tipe );
            BurnTree( left );
        }
    } else if( rite->class == TN_CONS ) {
        rv = OkToNegate( rite->u.name->c.value, tipe );
        if( rv != NULL ) {
            fold = TGBinary( O_PLUS, left, TGConst( rv, rite->tipe ),
                              tipe );
            BurnTree( rite );
        }
    } else if( rite->class == TN_UNARY && rite->u2.t.op == O_UMINUS ) {
        fold = TGBinary( O_PLUS, left, rite->u.left, tipe );
        rite->u.left = NULL;
        BurnTree( rite );
    }
    return( fold );
}


static type_def *FixAddType( tn left, tn rite, type_def *tipe )
{
    if( left->tipe != tipe ) {
        if( left->tipe == rite->tipe ) {
            tipe = left->tipe;
        }
    }
    return( tipe );
}


tn      FoldPlus( tn left, tn rite, type_def *tipe )
/**********************************************************/
{
    tn              fold;
    tn              temp;
    float_handle    lv;
    float_handle    rv;
    unsigned_32     li;
    unsigned_32     ri;

    if( left->class == TN_CONS ) {
        temp = left;
        left = rite;
        rite = temp;
    }
    fold = NULL;
    if( left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        if( rite->class == TN_CONS ) {
            rv = rite->u.name->c.value;
            if( !HasBigConst( tipe ) && CFIs32( lv ) && CFIs32( rv ) ) {
                li = CFConvertByType( lv, tipe );
                ri = CFConvertByType( rv, tipe );
                ri = li + ri;
                fold = IntToType( ri, tipe );
            } else {
                fold = CFToType( CFAdd( lv, rite->u.name->c.value ),
                                  tipe );
            }
            BurnTree( rite );
            BurnTree( left );
        } else if( CFTest( lv ) == 0 ) {
            fold = TGConvert( rite, tipe );
            BurnTree( left );
        }
    } else if( rite->class == TN_CONS ) {
        if( CFTest( rite->u.name->c.value ) == 0 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        } else if( left->class == TN_BINARY && left->u2.t.op == O_PLUS &&
                   tipe == left->tipe && !HasBigConst( tipe ) ) {
            if( left->u.left->class == TN_CONS ) {
                tipe = FixAddType( left->u.left, rite, tipe );
                if( left->u.left->tipe == tipe ) {
                    fold = FoldPlus( left->u.left, rite, tipe );
                    left->u.left = fold;
                    fold = left;
                }
            } else if( left->u2.t.rite->class == TN_CONS ) {
                tipe = FixAddType( left->u2.t.rite, rite, tipe );
                if( left->u2.t.rite->tipe == tipe ) {
                    fold = FoldPlus( left->u2.t.rite, rite, tipe );
                    left->u2.t.rite = fold;
                    fold = left;
                }
            }
        }
    } else if( rite->class == TN_UNARY && rite->u2.t.op == O_UMINUS ) {
        fold = TGBinary( O_MINUS, left, rite->u.left, tipe );
        rite->u.left = NULL;
        BurnTree( rite );
    } else if( left->class == TN_UNARY && left->u2.t.op == O_UMINUS ) {
        fold = TGBinary( O_MINUS, rite, left->u.left, tipe );
        left->u.left = NULL;
        BurnTree( left );
    }
    return( fold );
}


static  tn      Halve( tn left, type_def *tipe )
/**********************************************/
{
#define ONE_HALF "0.5"

    return( TGBinary( O_TIMES, left, TGConst( CFCnvSF( ONE_HALF ), tipe ), tipe ) );
}

tn      FoldPow( tn left, tn rite, type_def *tipe )
/*********************************************************/
{
    tn          fold;

    fold = NULL;
    if( left->class == TN_UNARY && left->u2.t.op == O_SQRT ) {
        fold = Halve( rite, tipe );
        fold = TGBinary( O_POW, left->u.left, fold, tipe );
        left->u.left = NULL;
        BurnTree( left );
    } else if( left->class == TN_UNARY && left->u2.t.op == O_EXP ) {
        left->u.left = TGBinary( O_TIMES, left->u.left, rite, tipe );
        fold = left;
    } else if( left->class == TN_BINARY && left->u2.t.op == O_POW ) {
        left->u2.t.rite = TGBinary( O_TIMES, left->u2.t.rite, rite, tipe );
        fold = left;
    }
    return( fold );
}


tn      FoldAnd( tn left, tn rite, type_def *tipe )
/*********************************************************/
{
    tn              fold;
    float_handle    rv;
    float_handle    lv;

    if( left->class == TN_CONS ) {
        fold = left;
        left = rite;
        rite = fold;
    }
    fold = NULL;
    if( left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        rv = rite->u.name->c.value;
        if( CFIs32( lv ) && CFIs32( rv ) ) {
            unsigned_32     and;

            and = CFConvertByType( rv, tipe ) & CFConvertByType( lv, tipe );
            fold = IntToType( and, tipe );
            BurnTree( left );
            BurnTree( rite );
        } else if( CFIs64( lv ) && CFIs64( rv ) ) {
            unsigned_64     and;
            unsigned_64     li;
            unsigned_64     ri;

            li = CFGetInteger64Value( lv );
            ri = CFGetInteger64Value( rv );

            U64And( &li, &ri, &and );
            fold = Int64ToType( and, tipe );
            BurnTree( left );
            BurnTree( rite );
        }
    } else if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        /* For any X: X & 0 = 0, and X & ~0 = X */
        if( CFTest( rv ) == 0 ) {
            left = TGTrash( left );
            fold = TGBinary( O_COMMA, left, IntToType( 0, tipe ), tipe );
            BurnTree( rite );
        } else if( !HasBigConst( tipe ) && rite->u.name->c.lo.int_value == -1 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        }
    }
    return( fold );
}


tn      FoldOr( tn left, tn rite, type_def *tipe )
/********************************************************/
{
    tn              fold;
    float_handle    rv;
    float_handle    lv;

    if( left->class == TN_CONS ) {
        fold = left;
        left = rite;
        rite = fold;
    }
    fold = NULL;
    if( left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        rv = rite->u.name->c.value;
        if( CFIs32( lv ) && CFIs32( rv ) ) {
            unsigned_32     or;

            or = CFConvertByType( rv, tipe ) | CFConvertByType( lv, tipe );
            fold = IntToType( or, tipe );
            BurnTree( left );
            BurnTree( rite );
        } else if( CFIs64( lv ) && CFIs64( rv ) ) {
            unsigned_64     or;
            unsigned_64     li;
            unsigned_64     ri;

            li = CFGetInteger64Value( lv );
            ri = CFGetInteger64Value( rv );

            U64Or( &li, &ri, &or );
            fold = Int64ToType( or, tipe );
            BurnTree( left );
            BurnTree( rite );
        }
    } else if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        /* For any X: X | 0 = X, and X | ~0 = ~0 */
        if( CFTest( rv ) == 0 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        } else if( !HasBigConst( tipe ) && rite->u.name->c.lo.int_value == -1 ) {
            left = TGTrash( left );
            fold = TGBinary( O_COMMA, left, IntToType( -1, tipe ), tipe );
            BurnTree( rite );
        }
    }
    return( fold );
}


tn      FoldXor( tn left, tn rite, type_def *tipe )
/*********************************************************/
{
    tn              fold;
    float_handle    rv;
    float_handle    lv;

    if( left->class == TN_CONS ) {
        fold = left;
        left = rite;
        rite = fold;
    }
    fold = NULL;
    if( left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        rv = rite->u.name->c.value;
        if( CFIs32( lv ) && CFIs32( rv ) ) {
            unsigned_32     li;
            unsigned_32     ri;

            li = CFConvertByType( lv, tipe );
            ri = CFConvertByType( rv, tipe );
            ri = li ^ ri;
            fold = IntToType( ri, tipe );
            BurnTree( left );
            BurnTree( rite );
        } else if( CFIs64( lv ) && CFIs64( rv ) ) {
            unsigned_64     xor;
            unsigned_64     li;
            unsigned_64     ri;

            li = CFGetInteger64Value( lv );
            ri = CFGetInteger64Value( rv );

            U64Xor( &li, &ri, &xor );
            fold = Int64ToType( xor, tipe );
            BurnTree( left );
            BurnTree( rite );
        }
    } else if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        /* For any X: X ^ 0 = X, and X ^ ~0 = ~X */
        if( CFTest( rv ) == 0 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        } else if( !HasBigConst( tipe ) && rite->u.name->c.lo.int_value == -1 ) {
            fold = TGUnary( O_COMPLEMENT, left, tipe );
            BurnTree( rite );
        }
    }
    return( fold );
}


tn      FoldRShift( tn left, tn rite, type_def *tipe )
/************************************************************/
{
    tn              fold;
    float_handle    rv;
    float_handle    lv;
    signed_32       ri;

    fold = NULL;
    if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        ri = CFConvertByType( rv, tipe );
        if( left->class == TN_CONS ) {
            bool    done = false;

            if( ri >= (tipe->length * 8) ) {
                if( tipe->attr & TYPE_SIGNED ) {
                    // For signed shifts, reduce the shift amount and
                    // then do the math
                    ri = (tipe->length * 8) - 1;
                } else {
                    fold = IntToType( 0, tipe );
                    done = true;
                }
            }
            if( !done ) {
                lv = left->u.name->c.value;
                if( !HasBigConst( tipe ) && CFIs32( lv ) && CFIs32( rv ) ) {
                    signed_32       li;
                    unsigned_32     shft;

                    li = CFConvertByType( lv, tipe );
                    if( tipe->attr & TYPE_SIGNED ) {
                        shft = li >> ri;
                    } else {
                        shft = (unsigned_32)li >> (unsigned_32)ri;
                    }
                    fold = IntToType( shft, tipe );
                } else if( CFIs64( lv ) && CFIs64( rv ) ) {
                    signed_64       rsh;
                    signed_64       li;

                    li = CFGetInteger64Value( lv );

                    if( tipe->attr & TYPE_SIGNED ) {
                        I64ShiftR( &li, ri, &rsh );
                    } else {
                        U64ShiftR( &li, ri, &rsh );
                    }
                    fold = Int64ToType( rsh, tipe );
                }
            }
            BurnTree( left );
            BurnTree( rite );
        } else if( ri == 0 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        } else if( ri >= (tipe->length * 8) && (tipe->attr & TYPE_SIGNED) == 0 ) {
            fold = TGBinary( O_COMMA, TGTrash( left ), IntToType( 0, tipe ), tipe );
            BurnTree( rite );
        }
    }
    return( fold );
}


tn      FoldLShift( tn left, tn rite, type_def *tipe )
/************************************************************/
{
    tn              fold;
    float_handle    rv;
    float_handle    lv;
    signed_32       ri;

    fold = NULL;
    ri = 0;
    if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        if( CFIs32( rv ) ) {
            ri = CFConvertByType( rv, tipe );
        } else if( CFIs64( rv ) ) {
            /* If shift amount won't fit into 32 bits, anything big enough will do */
            ri = 0xffff;
        }
        if( left->class == TN_CONS ) {
            if( ri >= (tipe->length * 8) ) {
                fold = IntToType( 0, tipe );
            } else {
                lv = left->u.name->c.value;
                if( !HasBigConst( tipe ) && CFIs32( lv ) && CFIs32( rv ) ) {
                    signed_32       li;

                    li = CFConvertByType( lv, tipe );
                    fold = IntToType( li << ri, tipe );
                } else if( CFIs64( lv ) && CFIs64( rv ) ) {
                    signed_64       lsh;
                    signed_64       li;

                    li = CFGetInteger64Value( lv );

                    U64ShiftL( &li, ri, &lsh );
                    fold = Int64ToType( lsh, tipe );
                }
            }
            BurnTree( left );
            BurnTree( rite );
        } else if( ri == 0 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        } else if( ri >= (tipe->length * 8) ) {
            fold = TGBinary( O_COMMA, TGTrash( left ), IntToType( 0, tipe ), tipe );
            BurnTree( rite );
        }
    }
    return( fold );
}


tn      FoldDiv( tn left, tn rite, type_def *tipe )
/*********************************************************/
{
    tn              fold;
    float_handle    rv;
    float_handle    lv;
    float_handle    tmp;
    int             log;

    fold = NULL;
    if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        if( HasBigConst( tipe ) ) {
            lv = left->u.name->c.value;
            if( CFTest( rv ) != 0 && left->class == TN_CONS ) {
                if( tipe->attr & TYPE_FLOAT ) {
                    fold = CFToType( CFDiv( lv, rv ), tipe );
                } else {    /* Must be a 64-bit integer. */
                    signed_64       div;
                    signed_64       rem;
                    signed_64       li;
                    signed_64       ri;

                    assert( CFIs64( lv ) );
                    assert( CFIs64( rv ) );
                    li = CFGetInteger64Value( lv );
                    ri = CFGetInteger64Value( rv );

                    if( tipe->attr & TYPE_SIGNED ) {
                        I64Div( &li, &ri, &div, &rem );
                    } else {
                        U64Div( &li, &ri, &div, &rem );
                    }
                    fold = Int64ToType( div, tipe );
                }
                BurnTree( left );
                BurnTree( rite );
            } else if( ( tipe->attr & TYPE_FLOAT ) &&
                ( _IsModel( FP_UNSTABLE_OPTIMIZATION ) ||
                ( CFIsU32( rv ) && GetLog2( CFConvertByType( rv, tipe ) ) != -1 ) ) ) {
                if( CFTest( rv ) != 0 ) {
                    tmp = CFInverse( rv );
                    if( tmp != NULL ) {
                        fold = TGBinary( O_TIMES, left, TGConst( tmp, tipe ), tipe );
                        BurnTree( rite );
                    }
                }
            }
        } else if( CFTest( rv ) != 0 && left->class == TN_CONS ) {
            lv = left->u.name->c.value;
            if( tipe->attr & TYPE_SIGNED ) {
                if( CFIsI32( lv ) && CFIsI32( rv ) ) {
                    signed_32       li;
                    signed_32       ri;

                    li = CFConvertByType( lv, tipe );
                    ri = CFConvertByType( rv, tipe );
                    fold = IntToType( li / ri, tipe );
                    BurnTree( left );
                    BurnTree( rite );
                }
            } else {
                if( CFIsU32( lv ) && CFIsU32( rv ) ) {
                    unsigned_32     li;
                    unsigned_32     ri;

                    li = CFCnvF32( lv );
                    ri = CFCnvF32( rv );
                    U32ModDiv( &li, ri );
                    fold = IntToType( li, tipe );
                    BurnTree( left );
                    BurnTree( rite );
                }
            }
        } else if( rite->u.name->c.lo.int_value == 1 ) {
            fold = TGConvert( left, tipe );
            BurnTree( rite );
        } else if( !HasBigConst( tipe ) && ( left->tipe->attr & TYPE_SIGNED ) == 0 ) {
            if( CFIsU32( rv ) ) {
                log = GetLog2( rite->u.name->c.lo.int_value );
                if( log != -1 ) {
                    fold = TGBinary( O_RSHIFT, left, IntToType( log, TypeInteger ), tipe );
                    BurnTree( rite );
                }
            }
        }
    } else if( left->class == TN_CONS ) {
        if( CFTest( left->u.name->c.value ) == 0 ) {
            rite = TGTrash( rite );
            fold = TGBinary( O_COMMA, rite, TGConvert( left, tipe ), tipe );
        }
    }
    return( fold );
}


tn      FoldMod( tn left, tn rite, type_def *tipe )
/*********************************************************/
{
    tn              fold;
    float_handle    rv;
    float_handle    lv;
    int             log;

    fold = NULL;
    if( rite->class == TN_CONS ) {
        rv = rite->u.name->c.value;
        if( CFTest( rv ) != 0 && left->class == TN_CONS ) {
            lv = left->u.name->c.value;
            if( CFIs64( lv ) && CFIs64( rv ) ) {
                signed_64       div;
                signed_64       rem;
                signed_64       li;
                signed_64       ri;

                li = CFGetInteger64Value( lv );
                ri = CFGetInteger64Value( rv );

                if( tipe->attr & TYPE_SIGNED ) {
                    I64Div( &li, &ri, &div, &rem );
                } else {
                    U64Div( &li, &ri, &div, &rem );
                }
                fold = Int64ToType( rem, tipe );
                BurnTree( left );
                BurnTree( rite );
            } else {
                if( tipe->attr & TYPE_SIGNED ) {
                    if( CFIsI32( lv ) && CFIsI32( rv ) ) {
                        signed_32     ri;
                        signed_32     li;

                        li = CFConvertByType( lv, tipe );
                        ri = CFConvertByType( rv, tipe );
                        fold = IntToType( li % ri, tipe );
                        BurnTree( left );
                        BurnTree( rite );
                    }
                } else {
                    if( CFIsU32( lv ) && CFIsU32( rv ) ) {
                        unsigned_32     ri;
                        unsigned_32     li;

                        li = CFConvertByType( lv, tipe );
                        ri = CFConvertByType( rv, tipe );
                        li = U32ModDiv( &li, ri );
                        fold = IntToType( li, tipe );
                        BurnTree( left );
                        BurnTree( rite );
                    }
                }
            }
        } else if( !HasBigConst( tipe ) && rite->u.name->c.lo.int_value == 1 ) {
            fold = CFToType( CFCnvIF( 0 ), tipe );
            fold = TGBinary( O_COMMA, left, fold, tipe );
            BurnTree( rite );
        } else if( !HasBigConst( tipe ) ) {
            if( ( left->tipe->attr & TYPE_SIGNED ) == 0 ) {
                if( CFIsU32( rv ) ) {
                    unsigned_32 ri = CFConvertByType( rv, tipe );
                    log = GetLog2( ri );
                    if( log != -1 ) {
                        fold = TGBinary( O_AND, left, IntToType( ri - 1, tipe ), tipe );
                        BurnTree( rite );
                    }
                }
            } else if( _IsntModel( NO_OPTIMIZATION ) && _IsntModel( DBG_LOCALS ) && TGCanDuplicate( left ) ) {
                /* signed int a; optimize a % rv like IBM does:
                   int b = a >> 31;
                   t1 = (a ^ b) - b;
                   t2 = t & (rv - 1);
                   fold = (t2 ^ b) - b;
                 */
                signed_32 ri = CFConvertByType( rv, tipe );
                if( ri < 0 )    /* sign of constant doesn't matter */
                    ri = -ri;
                log = GetLog2( ri );
                if( log != -1 && ri > 1 ) {
                    tn b1, b2, b3, b4; /* 4 copies of b - will be treated as CSE */
                    tn left1, t1, t2;

                    left1 = TGDuplicate( left );
                    b1 = TGBinary( O_RSHIFT, left1, IntToType( 31, tipe ), tipe );
                    b2 = TGDuplicate( b1 );
                    b3 = TGDuplicate( b1 );
                    b4 = TGDuplicate( b1 );
                    t1 = TGBinary( O_MINUS, TGBinary( O_XOR, left, b1, tipe ), b2, tipe );
                    t2 = TGBinary( O_AND, t1, IntToType( ri-1, tipe ), tipe );
                    fold = TGBinary( O_MINUS, TGBinary( O_XOR, t2, b3, tipe ), b4, tipe );
                    BurnTree( rite );
                }
            }
        }
    } else if( left->class == TN_CONS ) {
        if( CFTest( left->u.name->c.value ) == 0 ) {
            rite = TGTrash( rite );
            fold = TGBinary( O_COMMA, rite, IntToType( 0, tipe ), tipe );
            BurnTree( left );
        }
    }
    return( fold );
}


tn      Fold1sComp( tn left, type_def *tipe )
/***************************************************/
{
    tn              new;
    float_handle    lv;

    new = NULL;
    if( !HasBigConst( tipe ) && left->class == TN_CONS ) {
        lv = left->u.name->c.value;
        if( CFIs32( lv ) ) {
            new = IntToType( ~CFConvertByType( lv, tipe ), tipe );
            BurnTree( left );
        }
    }
    return( new );
}


tn      FoldUMinus( tn left, type_def *tipe )
/***************************************************/
{
    tn              new;
    float_handle    lv;

    new = NULL;
    if( left->class == TN_CONS ) {
        lv = OkToNegate( left->u.name->c.value, tipe );
        if( lv != NULL ) {
            new = CFToType( lv, tipe );
            BurnTree( left );
        }
    } else if( left->class == TN_UNARY && left->u2.t.op == O_UMINUS ) {
        new = left->u.left;
        left->u.left = NULL;
        BurnTree( left );
    }
    return( new );
}


tn      FoldSqrt( tn left, type_def *tipe )
/*************************************************/
{
    tn          fold;

    fold = NULL;
    if( left->class == TN_UNARY && left->u2.t.op == O_EXP ) {
        left->u.left = Halve( left->u.left, tipe );
        fold = left;
    } else if( left->class == TN_BINARY && left->u2.t.op == O_POW ) {
        left->u2.t.rite = Halve( left->u2.t.rite, tipe );
        fold = left;
    }
    return( fold );
}


tn      FoldLog( cg_op op, tn left, type_def *tipe )
/**********************************************************/
{
    tn          fold;

    fold = NULL;
    if( left->class == TN_UNARY && left->u2.t.op == O_SQRT ) {
        fold = TGUnary( op, left->u.left, tipe );
        fold = Halve( fold, tipe );
        left->u.left = NULL;
        BurnTree( left );
    } else if( left->class == TN_BINARY && left->u2.t.op == O_POW ) {
        fold = TGUnary( op, left->u.left, tipe );
        fold = TGBinary( O_TIMES, left->u2.t.rite, fold, tipe );
        left->u.left = NULL;
        left->u2.t.rite = NULL;
        BurnTree( left );
    }
    return( fold );
}


tn      FoldFlAnd( tn left, tn rite )
/*******************************************/
{
    tn  fold;

    fold = NULL;
    if( left->class == TN_CONS ) {
        if( rite->class == TN_CONS ) {
            if( CFTest( left->u.name->c.value ) && CFTest( rite->u.name->c.value ) ) {
                fold = IntToType( FETrue(), TypeInteger );
            } else {
                fold = IntToType( 0, TypeInteger );
            }
            BurnTree( left );
            BurnTree( rite );
        } else if( CFTest( left->u.name->c.value ) == 0 ) {
            // ( 0 && expr ) -> 0
            fold = IntToType( 0, TypeInteger );
            BurnTree( left );
            BurnTree( rite );
        } else {
            // ( 1 && expr ) -> ( expr )
            fold = rite;
            BurnTree( left );
        }
    } else if( rite->class == TN_CONS ) {
        if( CFTest( rite->u.name->c.value ) ) {
            // ( expr && 1 ) -> ( expr )
            fold = left;
            BurnTree( rite );
        } else {
            // ( expr && 0 ) -> ( expr, 0 )
            fold = TGNode( TN_COMMA, O_NOP, left, IntToType( 0, TypeInteger ), TypeInteger );
            BurnTree( rite );
        }
    }
    return( fold );
}


tn      FoldFlOr( tn left, tn rite )
/******************************************/
{
    tn  fold;

    fold = NULL;
    if( left->class == TN_CONS ) {
        if( CFTest( left->u.name->c.value ) ) {
            // ( 1 || expr ) -> ( true )
            fold = IntToType( FETrue(), TypeInteger );
            BurnTree( left );
            BurnTree( rite );
        } else {
            // ( 0 || expr ) -> ( expr )
            fold = rite;
            BurnTree( left );
        }
    } else if( rite->class == TN_CONS ) {
        if( CFTest( rite->u.name->c.value ) ) {
            // ( expr || 1 ) -> ( expr, true )
            fold = TGNode( TN_COMMA, O_NOP, left, IntToType( FETrue(), TypeInteger ), TypeInteger );
            BurnTree( rite );
        } else {
            // ( expr || 0 ) -> ( expr )
            fold = left;
            BurnTree( rite );
        }
    }
    return( fold );
}


tn      FoldFlNot( tn left )
/**********************************/
{
    tn          fold;
    int         result;


    fold = NULL;
    if( left->class == TN_CONS ) {
        if( CFTest( left->u.name->c.value ) == 0 ) {
            result = FETrue();
        } else {
            result = 0;
        }
        fold = IntToType( result, TypeInteger );
        BurnTree( left );
    }
    return( fold );
}

tn      FoldBitCompare( cg_op op, tn left, tn rite )
/**********************************************************/
{
    tn              fold;
    unsigned_32     new_cons;
    unsigned_32     mask;

    if( left->class == TN_CONS ) {
        fold = left;
        left = rite;
        rite = fold;
        op = RevOpcode[op - O_EQ];
    }
    fold = NULL;
    if( rite->class == TN_CONS && left->class == TN_BIT_RVALUE && !left->u2.b.is_signed && !HasBigConst( left->tipe ) ) {
        new_cons = rite->u.name->c.lo.int_value;
        new_cons <<= left->u2.b.start;
        mask = TGMask32( left );
        if( ( new_cons & ~mask ) == 0 ) { /* idiot comparing out of range*/
            fold = TGUnary( O_POINTS, left->u.left, left->tipe );
            fold = TGBinary( O_AND, fold, IntToType( mask, left->tipe ), left->tipe );
            fold = TGCompare( op, fold, IntToType( new_cons, left->tipe ), left->tipe );
            left->u.left = NULL; /* so no recursion*/
            BurnTree( left );
            BurnTree( rite );
        }
    }
    return( fold );
}


float_handle CnvCFToType( float_handle cf, type_def *tipe )
/*****************************************************************/
{
    if( ( tipe->attr & TYPE_FLOAT ) == EMPTY ) {
        cf = IntToCF( CFGetInteger64Value( cf ), tipe );
    } else {
        cf = CFCopy( cf );
    }
    return( cf );
}

static  tn      FindBase( tn tree, bool op_eq )
/*********************************************/
{
    type_attr           child_attr;
    type_attr           this_attr;

    for( ;; ) {
        if( tree->class != TN_UNARY ) break;
        if( tree->u2.t.op != O_CONVERT ) break;
        if( tree->u.left->tipe->length > tree->tipe->length ) break;
        child_attr = tree->u.left->tipe->attr;
        this_attr = tree->tipe->attr;
        if( op_eq ) {
            // if we are doing a EQ/NE comparison we can ignore sign changes
            child_attr &= ~TYPE_SIGNED;
            this_attr  &= ~TYPE_SIGNED;
        } else if( this_attr & TYPE_SIGNED && tree->u.left->tipe->length < tree->tipe->length ) {
            // if we went from smaller unsigned to larger signed type,
            // sign change isn't a problem either
            child_attr |= TYPE_SIGNED;
        }
        if( child_attr != this_attr ) break;
        tree = tree->u.left;
    }
    return( tree );
}

static  void    BurnToBase( tn root, tn base )
/********************************************/
{
    tn          next;
    tn          curr;

    for( curr = root; curr != base; curr = next ) {
        assert( curr->class == TN_UNARY && curr->u2.t.op == O_CONVERT );
        next = curr->u.left;
        curr->u.left = NULL;
        BurnTree( curr );
    }
}

static bool IsObjectAddr( tn tree )
/*********************************/
{
    if( tree->class == TN_LEAF && tree->u.addr->format == NF_ADDR ) {
        switch( tree->u.addr->class ) {
        case CL_ADDR_GLOBAL:
        case CL_ADDR_TEMP:
            return( true );
        default:
            break;
        }
    }
    return( false );
}

tn  FoldCompare( cg_op op, tn left, tn rite, type_def *tipe )
/*******************************************************************/
{
    int             compare;
    tn              temp;
    int             result;
    int             true_value;
    float_handle    lv;
    float_handle    rv;
    tn              base_r;
    tn              base_l;
    bool            op_eq;

    if( left->class == TN_CONS ) {
        temp = left;
        left = rite;
        rite = temp;
        op = RevOpcode[op - O_EQ];
    }
    op_eq = false;
    if( ( op == O_EQ ) || ( op == O_NE ) ) {
        op_eq = true;
    }
    true_value = FETrue();
    result = 0;
    if( left->class == TN_CONS ) {
        lv = CnvCFToType( left->u.name->c.value, tipe );
        rv = CnvCFToType( rite->u.name->c.value, tipe );
        compare = CFCompare( lv, rv );
        CFFree( lv );
        CFFree( rv );
        switch( op ) {
        case O_EQ:
            if( compare == 0 ) {
                result = true_value;
            }
            break;
        case O_NE:
            if( compare != 0 ) {
                result = true_value;
            }
            break;
        case O_GT:
            if( compare > 0 ) {
                result = true_value;
            }
            break;
        case O_LT:
            if( compare < 0 ) {
                result = true_value;
            }
            break;
        case O_GE:
            if( compare >= 0 ) {
                result = true_value;
            }
            break;
        case O_LE:
            if( compare <= 0 ) {
                result = true_value;
            }
            break;
        }
        BurnTree( left );
        BurnTree( rite );
        return( IntToType( result, TypeInteger ) );
    } else if( rite->class == TN_CONS ) {
        if( left->class != TN_BINARY ) {
            base_l = FindBase( left, false );
            if( base_l != left ) {
                /* For folding comparisons, consider the variable's original type. If eg. a short
                 * was converted to long, we know its value has to be in the short's range.
                 */
                tipe = base_l->tipe;
            }
            if( (tipe->attr & (TYPE_FLOAT | TYPE_POINTER )) == 0 ) {
                cmp_result  cmp;

                cmp = CheckCmpRange( op, CmpType( tipe ), rite->u.name->c.value );
                if( cmp != CMP_VOID ) {
                    if( cmp == CMP_TRUE ) {
                        result = true_value;
                    }
                    /* Throw away constant but keep non-const part */
                    BurnTree( rite );
                    left = TGTrash( left );
                    return( TGBinary( O_COMMA, left, IntToType( result, TypeInteger ), TypeInteger ) );
                }
                if( _IsntModel( NULL_DEREF_OK ) && IsObjectAddr( left ) && ( CFTest( rite->u.name->c.value ) == 0 ) ) {
                    /* Addresses of globals or local variables are guaranteed not to be null
                     * unless NULL_DEREF_OK is in effect
                     */
                    BurnTree( left );
                    BurnTree( rite );
                    return( FoldCompare( op, IntToType( 1, TypeInteger ), IntToType( 0, TypeInteger ), TypeInteger ) );
                }
            }
            if( ( base_l != left ) && (tipe->attr & TYPE_FLOAT) == 0 ) {
                // If we couldn't fold the comparison, get rid of some lame converts
                // the C++ compiler likes to emit. Careful with floats!
                BurnToBase( left, base_l );
                return( TGNode( TN_COMPARE, op, base_l, rite, TypeBoolean ) );
            }
            return( NULL );
        }
#if _TARGET & _TARG_370
        if( left->u2.t.rite->class != TN_CONS ) return( NULL );
#endif
        if( left->u2.t.op != O_AND ) return( NULL );
        if( op != O_EQ && op != O_NE ) return( NULL );
        if( left->u.left->class == TN_CONS ) {
            temp = left->u.left;
            left->u.left = left->u2.t.rite;
            left->u2.t.rite = temp;
        }
        if( left->u2.t.rite->class == TN_CONS
         && left->u2.t.rite->u.left == rite->u.left
         && !HasBigConst( tipe )
         && GetLog2( rite->u.name->c.lo.int_value ) != -1 ) {
            rite->u.name = AllocIntConst( 0 );
            if( op == O_EQ ) {
                op = O_NE;
            } else {
                op = O_EQ;
            }
        }
        if( CFTest( rite->u.name->c.value ) != 0 ) return( NULL );
        BurnTree( rite );
        left->class = TN_COMPARE;
        left->tipe = TypeBoolean;
        if( op == O_EQ ) {
            left->u2.t.op = (cg_op)OP_BIT_TEST_FALSE;
        } else {
            left->u2.t.op = (cg_op)OP_BIT_TEST_TRUE;
        }
        return( left );
    } else {
        base_r = FindBase( rite, op_eq );
        base_l = FindBase( left, op_eq );
        if( base_r != rite || base_l != left ) {
            if( ( base_r->tipe == base_l->tipe ) && ( ( op == O_EQ ) || ( op == O_NE ) ) ) {
                BurnToBase( rite, base_r );
                BurnToBase( left, base_l );
                return( TGNode( TN_COMPARE, op, base_l, base_r, TypeBoolean ) );
            }
        }
    }
    return( NULL );
}

static  bool    SimpleLeaf( tn tree )
/***********************************/
{
    if( tree->class == TN_UNARY && tree->u2.t.op == O_POINTS ) {
        tree = tree->u.left;
    }
    return( tree->class == TN_LEAF );
}


tn      FoldPostGetsCompare( cg_op op, tn left, tn rite, type_def *tipe )
/*******************************************************************************/
{
//    tn              compare;
    tn              temp;
    signed_32       ri;
    signed_32       li;
    signed_32       value;
    float_handle    rv;
    float_handle    lv;

///    compare = NULL;
    if( left->class == TN_CONS ) {
        temp = left;
        left = rite;
        rite = temp;
        op = RevOpcode[op - O_EQ];
    }

    if( op == O_GT && ( ( tipe->attr & TYPE_SIGNED ) == EMPTY ) ) {
        if( rite->class == TN_CONS ) {
            if( CFTest( rite->u.name->c.value ) == 0 ) {
                op = O_NE;
            }
        }
    }

    /*
     * This is a little sick - basically, we have some code which looks
     * like "while( n-- ) {" and we don't want it to mess up our dataflo
     * so we do a little tree re-write here to smooth things over.
     */

    if( op != O_EQ && op != O_NE ) return( NULL );
    if( rite->class == TN_CONS &&
        left->class == TN_POST_GETS &&
        ( left->u2.t.op == O_MINUS || left->u2.t.op == O_PLUS ) ) {
        if( left->u2.t.rite->class == TN_CONS && SimpleLeaf( left->u.left ) ) {
            if( tipe == left->tipe ) {
                rv = rite->u.name->c.value;
                lv = left->u2.t.rite->u.name->c.value;
                if( !HasBigConst( tipe ) && CFIs32( lv ) && CFIs32( rv ) ) {
                    li = CFConvertByType( lv, tipe );
                    ri = CFConvertByType( rv, tipe );
                    value = ( left->u2.t.op == O_MINUS ) ? ( ri - li ) : ( ri + li );
                    temp = IntToType( value, tipe );
                    left->class = TN_PRE_GETS;
                    left->optipe = left->tipe;
                    temp = TGNode( TN_COMPARE, op, left, temp, TypeBoolean );
                    BurnTree( rite );
                    return( temp );
                }
            }
        }
    }
    return( NULL );
}


/* routines above here are called while building the tree */
/* routines below here are called while tearing the tree apart */

static  an Flip( an name, bool op_false, bool op_true )
/*****************************************************/
{
    label_handle    *temp;

    if( op_false ) {
        if( op_true ) {
            BGDone( name );
            name = BGInteger( FETrue(), TypeInteger );
        } else {
            temp = name->u.b.f;
            name->u.b.f = name->u.b.t;
            name->u.b.t = temp;
        }
    } else {
        if( op_true ) {
            /* nothing*/
        } else {
            BGDone( name );
            name = BGInteger( 0, TypeInteger );
        }
    }
    return( name );
}



an FoldConsCompare( cg_op op, tn left, tn rite, type_def *tipe )
/**********************************************************************/
{
    tn              temp;
    an              fold;
    int             compare;
    int             compare_true;
    float_handle    f;
    float_handle    t;
    float_handle    rv;

    if( left->class == TN_CONS ) {
        temp = left;
        left = rite;
        rite = temp;
        op = RevOpcode[op - O_EQ];
    }
    fold = NULL;
    if( rite->class == TN_CONS ) {
        if( left->class == TN_FLOW_OUT ) {
            f = CFCnvIF( 0 );
            t = CFCnvIF( FETrue() );
            rv = CnvCFToType( rite->u.name->c.value, tipe );
            compare = CFCompare( f, rv );
            compare_true = CFCompare( t, rv );
            CFFree( rv );
            CFFree( f );
            CFFree( t );
            fold = TreeGen( left->u.left );
            switch( op ) {
            case O_EQ:
                fold = Flip( fold, compare == 0, compare_true == 0 );
                break;
            case O_NE:
                fold = Flip( fold, compare != 0, compare_true != 0 );
                break;
            case O_GT:
                fold = Flip( fold, compare > 0, compare_true > 0 );
                break;
            case O_LT:
                fold = Flip( fold, compare < 0, compare_true < 0 );
                break;
            case O_GE:
                fold = Flip( fold, compare >= 0, compare_true >= 0 );
                break;
            case O_LE:
                fold = Flip( fold, compare <= 0, compare_true <= 0 );
                break;
            case OP_BIT_TEST_TRUE:
                if( !HasBigConst( tipe ) ) {
                    fold = Flip( fold, false,
                            ( rite->u.name->c.lo.int_value & FETrue() ) != 0 );
                }
                break;
            case OP_BIT_TEST_FALSE:
                if( !HasBigConst( tipe ) ) {
                    fold = Flip( fold, true,
                            ( rite->u.name->c.lo.int_value & FETrue() ) == 0 );
                }
                break;
            }
            BurnTree( rite );
            left->u.left = NULL;
            BurnTree( left );
        }
    }
    return( fold );
}


bool    FoldIfTrue( tn left, label_handle lbl )
/*****************************************************/
{
    bool        folded;

    folded = false;
    if( left->class == TN_CONS ) {
        if( CFTest( left->u.name->c.value ) != 0 ) {
            BGGenCtrl( O_GOTO, NULL, lbl, true );
        }
        folded = true;
        BurnTree( left );
    }
    return( folded );
}


bool    FoldIfFalse( tn left, label_handle lbl )
/******************************************************/
{
    bool        folded;

    folded = false;
    if( left->class == TN_CONS ) {
        if( CFTest( left->u.name->c.value ) == 0 ) {
            BGGenCtrl( O_GOTO, NULL, lbl, true );
        }
        folded = true;
        BurnTree( left );
    }
    return( folded );
}
