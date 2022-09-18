/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Preprocessor expression evaluator for C compiler.
*
****************************************************************************/


#include "cvars.h"
#include <limits.h>
#include "i64.h"
#include "cmacadd.h"
#include "ppexpn.h"
#include "cgdefs.h"
#include "feprotos.h"


#define L                   I64LO32
#define H                   I64HI32

#define I64Zero( a )        ( (a).u.uval.u._32[L] == 0 && (a).u.uval.u._32[H] == 0 )
#define I64NonZero( a )     ( !I64Zero(a) )
#define U64Zero( a )        ( (a).u.uval.u._32[L] == 0 && (a).u.uval.u._32[H] == 0 )
#define U64NonZero( a )     ( !U64Zero(a) )
#define U64Low( a )         ( (a).u.uval.u._32[L] )
#define U64High( a )        ( (a).u.uval.u._32[H] )

#define I64Low( a )         ( (signed_32)(a).u.uval.u._32[L] )
#define U32ToU64Set( a, b ) U64Set( &((a).u.uval), b, 0 )

#define U64LT( a, b )       ( U64Cmp( &((a).u.uval), &((b).u.uval) ) < 0 )
#define U64GT( a, b )       ( U64Cmp( &((a).u.uval), &((b).u.uval) ) > 0 )
#define U64LE( a, b )       ( U64Cmp( &((a).u.uval), &((b).u.uval) ) <= 0 )
#define U64GE( a, b )       ( U64Cmp( &((a).u.uval), &((b).u.uval) ) >= 0 )
#define U64EQ( a, b )       ( U64Cmp( &((a).u.uval), &((b).u.uval) ) == 0 )
#define U64NE( a, b )       ( U64Cmp( &((a).u.uval), &((b).u.uval) ) != 0 )

#define I64LT( a, b )       ( I64Cmp( &((a).u.sval), &((b).u.sval) ) < 0 )
#define I64GT( a, b )       ( I64Cmp( &((a).u.sval), &((b).u.sval) ) > 0 )
#define I64LE( a, b )       ( I64Cmp( &((a).u.sval), &((b).u.sval) ) <= 0 )
#define I64GE( a, b )       ( I64Cmp( &((a).u.sval), &((b).u.sval) ) >= 0 )
#define I64EQ( a, b )       ( I64Cmp( &((a).u.sval), &((b).u.sval) ) == 0 )
#define I64NE( a, b )       ( I64Cmp( &((a).u.sval), &((b).u.sval) ) != 0 )

#define U64AddEq(a,b)       U64Add( &((a).u.uval), &((b).u.uval), &((a).u.uval) );
#define U64SubEq(a,b)       U64Sub( &((a).u.uval), &((b).u.uval), &((a).u.uval) );
#define U64MulEq(a,b)       U64Mul( &((a).u.uval), &((b).u.uval), &((a).u.uval) );
#define U64AndEq(a,b)       U64And( &((a).u.uval), &((b).u.uval), &((a).u.uval) );
#define U64OrEq(a,b)        U64Or(  &((a).u.uval), &((b).u.uval), &((a).u.uval) );
#define U64XOrEq(a,b)       U64Xor( &((a).u.uval), &((b).u.uval), &((a).u.uval) );

#define I64SetZero( a )     ( I32ToI64( 0, &(a).u.sval) );
#define U64SetZero( a )     ( U32ToU64( 0, &(a).u.uval) );

#define LAST_TOKEN_PREC     ARRAY_SIZE( Prec )

#define IS_OPERAND( token ) ( IS_ID_OR_KEYWORD( token ) || token == T_CONSTANT )

#ifndef NDEBUG
    #define __xstr(x)   #x
    #define __location " (" __FILE__ "," __xstr(__LINE__) ")"
    #define DbgDefault( msg )   default: CFatal( msg __location )
#else
    #define DbgDefault( msg )
#endif

#define Stack_forall( hdr, i ) \
    for( i = hdr; i != NULL; i = *((void **)i) )

typedef struct ppvalue {
    union {
        unsigned_64     uval;
        signed_64       sval;
    } u;
    unsigned            no_sign : 1;
} ppvalue;

typedef struct loc_info {
    int         pos;        // incremented when token read
    source_loc  locn;       // token location in source file
} loc_info;

typedef struct operand_stack {
    struct operand_stack *next;
    ppvalue     value;
    loc_info    loc;
} PPEXPN_OPERAND_STACK;     // stack to store operands

typedef struct operator_stack {
    struct operator_stack *next;
    TOKEN       token;
    loc_info    loc;
    int         prec;
} PPEXPN_OPERATOR_STACK;    // stack to store operators

typedef struct stack {
    struct stack    *next;
} STACK;

static PPEXPN_OPERAND_STACK     *HeadOperand = NULL;
static PPEXPN_OPERATOR_STACK    *HeadOperator = NULL;
static int                      Pos = 0;                // position of CurToken in parsing

/* include _ctokens.h for the precedence values */
static int  Prec[] = {     // table of token precedences
    #define pick(token,string,class,oper,prec) prec,
    #define OPERATORS_ONLY
    #include "_ctokens.h"
    #undef OPERATORS_ONLY
    #undef pick
};

#ifndef NDEBUG
static void CFatal( char *msg )
/*****************************/
{
    FEMessage( MSG_FATAL, msg );
}
#endif

static void *StackPush( void *hdr, void *elt )
/********************************************/
{
    void    **header = hdr;
    STACK   *e;

    e = elt;
    e->next = *header;
    *header = e;
    return( e );
}

static void *StackPop( void *hdr )
/********************************/
{
    void    **header = hdr;
    STACK   *e;

    e = *header;
    if( e == NULL ) {
        return( NULL );
    }
    *header = e->next;
    return( e );
}

static void PushOperator( TOKEN token, loc_info *loc, int prec )
/**************************************************************/
{
    PPEXPN_OPERATOR_STACK *stack_entry;

    stack_entry = (PPEXPN_OPERATOR_STACK *)CMemAlloc( sizeof( PPEXPN_OPERATOR_STACK ) );
    stack_entry->token = token;
    stack_entry->loc = *loc;
    stack_entry->prec = prec;
    StackPush( &HeadOperator, stack_entry );
}

static void PushCurToken( int prec )
/**********************************/
{
    loc_info loc;

    loc.locn = SrcFileLoc;
    loc.pos = Pos;
    PushOperator( CurToken, &loc, prec );
}


static bool PopOperator( TOKEN *token, loc_info *loc )
/****************************************************/
{
    PPEXPN_OPERATOR_STACK *stack_entry;

    stack_entry = StackPop( &HeadOperator );
    if( stack_entry != NULL ) {
        if( token != NULL ) {
            *token = stack_entry->token;
        }
        if( loc != NULL ) {
            *loc = stack_entry->loc;
        }
        CMemFree( stack_entry );
        return( true );
    }
    return( false );
}

static bool TopOperator( TOKEN *token, int *prec )
/************************************************/
{
    PPEXPN_OPERATOR_STACK *stack_entry;

    stack_entry = StackPop( &HeadOperator );
    if( stack_entry != NULL ) {
        if( token != NULL ) {
            *token = stack_entry->token;
        }
        if( prec != NULL ) {
            *prec = stack_entry->prec;
        }
        StackPush( &HeadOperator, stack_entry );
        return( true );
    }
    return( false );
}

static void PushOperand( ppvalue p, loc_info *loc )
/*************************************************/
{
    PPEXPN_OPERAND_STACK *stack_entry;

    stack_entry = (PPEXPN_OPERAND_STACK *)CMemAlloc( sizeof( PPEXPN_OPERAND_STACK ) );
    stack_entry->value = p;
    stack_entry->loc = *loc;
    StackPush( &HeadOperand, stack_entry );
}

static void PushOperandCurLocation( ppvalue p )
/*********************************************/
{
    loc_info loc;

    loc.locn = SrcFileLoc;
    loc.pos = Pos;
    PushOperand( p, &loc );
}

static bool PopOperand( ppvalue *p, loc_info *loc )
/*************************************************/
{
    PPEXPN_OPERAND_STACK *stack_entry;

    stack_entry = StackPop( &HeadOperand );
    if( stack_entry != NULL ) {
        if( p != NULL ) {
            *p = stack_entry->value;
        }
        if( loc != NULL ) {
            *loc = stack_entry->loc;
        }
        CMemFree( stack_entry );
        return( true );
    }
    return( false );
}

static bool CheckToken( TOKEN prev_token )
/****************************************/
{
    if( IS_OPERAND( prev_token ) && IS_OPERAND( CurToken ) ) {
        CErr1( ERR_CONSECUTIVE_OPERANDS );  // can't have 2 operands in a row
        return( true );
    }
    if( ( CurToken == T_PLUS ) || ( CurToken == T_MINUS ) ) {
        if( ( prev_token != T_RIGHT_PAREN )
         && ( !IS_OPERAND( prev_token ) || prev_token == T_START ) ) {
            if( CurToken == T_PLUS ) {
                CurToken = T_UNARY_PLUS;
            } else {
                CurToken = T_UNARY_MINUS;
            }
        }
    }
    return( false );
}

static bool PpNextToken( void )
/******************************
 * scan the next token and check for errors
 */
{
    static TOKEN prev_token;

    prev_token = CurToken;
    NextToken();
    Pos++;
    return( CheckToken( prev_token ) );
}

static void unexpectedCurToken( void )
/************************************/
{
    CErr2p( ERR_UNEXPECTED_IN_CONSTANT_EXPRESSION, Tokens[CurToken] );
}

static double SafeAtof( char *p )
/*******************************/
{
    double r;

    errno = 0;
    r = atof( p );
    if( errno ) {
        if( r == 0 && errno == ERANGE ) {
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );
        } else {
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        }
    }
    return( r );
}

static bool COperand( void )
/**************************/
{
    ppvalue p;
    loc_info loc;
    source_loc left_loc;
    bool done;

    done = false;
    switch( CurToken ) {
    case T_ID:
        loc.locn = SrcFileLoc; // need this to store result
        loc.pos = Pos;
        Pos++;
        if( IS_PPOPERATOR_DEFINED( Buffer ) ) {
            ppctl_t old_ppctl;

            old_ppctl = PPControl;
            PPCTL_DISABLE_MACROS();
            NextToken();        // Don't error check: can have T_ID T_ID here
            if( CurToken == T_LEFT_PAREN ) {
                left_loc = SrcFileLoc;
                NextToken();    // no need to error check or advance Pos
                PPControl = old_ppctl;
                U32ToU64Set( p, MacroLookup( Buffer ) != NULL );
                NextToken();    // no need to error check or advance Pos
                if( CurToken != T_RIGHT_PAREN ) {
                    SetErrLoc( &left_loc );
                    CErr1( ERR_UNMATCHED_LEFT_PAREN );
                    done = true;
                }
            } else {
                PPControl = old_ppctl;
                U32ToU64Set( p, MacroLookup( Buffer ) != NULL );
            }
        } else {
            CWarn2p( WARN_UNDEFD_MACRO_IS_ZERO, ERR_UNDEFD_MACRO_IS_ZERO, Buffer );
            I64SetZero( p );
        }
        p.no_sign = 0;
        if( !done ) {
            PushOperand( p, &loc );
            done = PpNextToken();
        }
        break;
#if 0
    case T_FALSE:
    case T_TRUE:
        I32ToI64( CurToken == T_TRUE, &(p.u.sval) );
        p.no_sign = 0;
        PushOperandCurLocation( p );
        done = PpNextToken();
        break;
#endif
    case T_CONSTANT:
        switch( ConstType ) {
        case TYP_FLOAT:
        case TYP_DOUBLE:
        case TYP_LONG_DOUBLE:
            CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
            done = true;
            if( CompFlags.c99_extensions ) {
                I32ToI64( SafeAtof( Buffer ), &(p.u.sval) );
            } else {
                U32ToU64Set( p, SafeAtof( Buffer ) );
            }
            // add long double support if available
            p.no_sign = 0;
            break;
        case TYP_WCHAR:
        case TYP_UCHAR:
        case TYP_USHORT:
        case TYP_UINT:
        case TYP_ULONG:
            U32ToU64Set( p, Constant );
            p.no_sign = 1;
            break;
        case TYP_ULONG64:
            if( CompFlags.c99_extensions ) {
                p.u.uval = Constant64;
            } else {
                U32ToU64Set( p, U32FetchTrunc( Constant64 ) );
            }
            p.no_sign = 1;
            break;
        case TYP_LONG64:
            if( CompFlags.c99_extensions ) {
                p.u.uval = Constant64;
            } else {
                U32ToU64Set( p, U32FetchTrunc( Constant64 ) );
            }
            p.no_sign = 0;
            break;
        default:
            if( CompFlags.c99_extensions ) {
                I32ToI64( Constant, &(p.u.sval) );
            } else {
                U32ToU64Set( p, Constant );
            }
            p.no_sign = 0;
        }
        if( !done ) {
            PushOperandCurLocation( p );
            done = PpNextToken();
        }
        break;
    default:
        CErr2p( ERR_UNDEFD_MACRO_IS_ZERO, Buffer );
        I64SetZero( p );
        p.no_sign = 0;
        PushOperandCurLocation( p );
        done = PpNextToken();
    }
    return( done );
}

static void CErrCheckpoint( unsigned *save )
/******************************************/
{
    *save = ErrCount;
}

static bool CErrOccurred( unsigned *previous_state )
/**************************************************/
{
    if( *previous_state != ErrCount ) {
        return( true );
    }
    return( false );
}

static bool CRightParen( void )
/******************************
 * reduce extra )
 */
{
    TOKEN right_paren;
    loc_info right_info;

    // as ) doesn't get pushed for (expr), this must be an unmatched )
    if( PopOperator( &right_paren, &right_info ) ) {
        SetErrLoc( &right_info.locn );
        CErr1( ERR_UNMATCHED_RIGHT_PAREN );
    }
    return( false );
}

static bool CLeftParen( void )
/*****************************
 * reduce (expr)
 */
{
    TOKEN left_paren;
    loc_info left_info;
    loc_info e1_info;
    ppvalue e1;

    PopOperator( &left_paren, &left_info );
    if( CurToken != T_RIGHT_PAREN ) {   // expect ) as current token
        SetErrLoc( &left_info.locn );
        CErr1( ERR_UNMATCHED_LEFT_PAREN );
        return( true );
    }
    if( PopOperand( &e1, &e1_info ) ) {
        if( ( e1_info.pos < Pos ) && ( e1_info.pos > left_info.pos ) ) {
            PushOperand( e1, &e1_info );
            return( PpNextToken() );
        }
    }
    SetErrLoc( &left_info.locn );
    CErr1( ERR_EMPTY_PAREN );
    return( true );
}

static bool CConditional( void )
/*******************************
 * reduce an a?b:c expression
 */
{
    loc_info e1_info;
    loc_info e2_info;
    loc_info e3_info;
    loc_info op1_info;
    loc_info op2_info;
    ppvalue e1;
    ppvalue e2;
    ppvalue e3;
    TOKEN op1;
    TOKEN op2;

    PopOperator( &op2, &op2_info );
    if( op2 != T_COLON ) {
        if( ( op2 != T_QUESTION && op2 != T_START ) || CurToken == T_NULL ) {
            SetErrLoc( &op2_info.locn );
            CErr1( ERR_CONDITIONAL_MISSING_COLON );
            return( true );
        } else {
            PushOperator( op2, &op2_info, Prec[op2] );
            PushCurToken( Prec[CurToken] );
            return( PpNextToken() );
        }
    }
    // always something to pop next, even if its T_START
    PopOperator( &op1, &op1_info );
    if( op1 != T_QUESTION ) {
        if( ( op1 != T_COLON && op1 != T_START ) || ( CurToken == T_NULL ) ) {
            SetErrLoc( &op1_info.locn );
            CErr1( ERR_CONDITIONAL_MISSING_QUESTION );
            return( true );
        }
    }
    if( op2 != T_COLON || op1 != T_QUESTION ) {
        PushOperator( op1, &op1_info, Prec[op1] );
        PushOperator( op2, &op2_info, Prec[op2] );
        PushCurToken( Prec[CurToken] );
        return( PpNextToken() );
    }
    if( PopOperand( &e3, &e3_info ) && ( e3_info.pos > op2_info.pos ) ) {
        if( PopOperand( &e2, &e2_info ) && ( e2_info.pos < op2_info.pos ) &&
            ( e2_info.pos > op1_info.pos ) ) {
            if( PopOperand( &e1, &e1_info ) &&
                ( e1_info.pos < op1_info.pos ) ) {
                if( I64NonZero( e1 ) ) {
                    e1.u.sval = e2.u.sval;
                } else {
                    e1.u.sval = e3.u.sval;
                }
                e1.no_sign = e2.no_sign | e3.no_sign;
                PushOperand( e1, &e1_info );
                return( false );
            } else {
                SetErrLoc( &op1_info.locn );    // missing a in a?b:c
                CErr1( ERR_CONDITIONAL_MISSING_FIRST_OPERAND );
            }
        } else {
            SetErrLoc( &op1_info.locn );        // missing b in a?b:c
            CErr1( ERR_CONDITIONAL_MISSING_SECOND_OPERAND );
        }
    } else {
        SetErrLoc( &op2_info.locn );            // missing c in a?b:c
        CErr1( ERR_CONDITIONAL_MISSING_THIRD_OPERAND );
    }
    return( true );
}

static bool Binary( TOKEN *token, ppvalue *e1, ppvalue *e2, loc_info *loc )
/**************************************************************************
 * pop binary operand and two operands, error check
 */
{
    loc_info e1_info;
    loc_info e2_info;


    PopOperator( token, loc );
    if( PopOperand( e2, &e2_info ) && ( e2_info.pos > loc->pos ) ) {
        if( PopOperand( e1, &e1_info ) && ( e1_info.pos < loc->pos ) ) {
            return( true );
        } else {
            SetErrLoc( &loc->locn );
            CErr2p( ERR_BINARY_MISSING_LEFT_OPERAND, Tokens[*token] );
        }
    } else {
        SetErrLoc( &loc->locn );
        CErr2p( ERR_BINARY_MISSING_RIGHT_OPERAND, Tokens[*token] );
    }
    return( false );
}

static bool CLogicalOr( void )
/*****************************
 * reduce a || b
 */
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    TOKEN token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        if( I64Zero( e1 ) ) {               // e1 is zero, so consider e2
            if( I64NonZero( e2 ) ) {        // e2 non-zero
                U32ToU64Set( e1, 1 );       // answer is 1
            } else {
                I64SetZero( e1 );
            }
        } else {
            U32ToU64Set( e1, 1 );           // answer is 1
        }
        e1.no_sign = 0;
        PushOperand( e1, &loc );
        return( false );
    }
    return( true );
}

static bool CLogicalAnd( void )
/******************************
 * reduce a && b
 */
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    TOKEN token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        if( I64NonZero( e1 ) ) {            // e1 is non-zero
            if( I64Zero( e2 ) ) {           // e2 is zero
                e1.u.sval = e2.u.sval;
            } else {
                U32ToU64Set( e1, 1 );       // answer is 1
            }
        }
        // else e1 is already zero
        e1.no_sign = 0;
        PushOperand( e1, &loc );
        return( false );
    }
    return( true );
}

static bool COr( void )
/**********************
 * reduce a|b
 */
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    TOKEN token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        if( CompFlags.c99_extensions ) {
            U64OrEq( e1, e2 );
        } else {
            U32ToU64Set( e1, U64Low( e1 ) | U64Low( e2 ) );
        }
        e1.no_sign |= e2.no_sign;
        PushOperand( e1, &loc );
        return( false );
    }
    return( true );
}

static bool CXOr( void )
/***********************
 * reduce a^b
 */
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    TOKEN token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        if( CompFlags.c99_extensions ) {
            U64XOrEq( e1, e2 );
        } else {
            U32ToU64Set( e1, U64Low( e1 ) ^ U64Low( e2 ) );
        }
        e1.no_sign |= e2.no_sign;
        PushOperand( e1, &loc );
        return( false );
    }
    return( true );
}

static bool CAnd( void )
/***********************
 * reduce a&b
 */
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    TOKEN token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        if( CompFlags.c99_extensions ) {
            U64AndEq( e1, e2 );
        } else {
            U32ToU64Set( e1, U64Low( e1 ) & U64Low( e2 ) );
        }
        e1.no_sign |= e2.no_sign;
        PushOperand( e1, &loc );
        return( false );
    }
    return( true );
}

static bool CEquality( void )
/****************************
 * reduce a == b or a != b
 */
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    TOKEN token;
    int val;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        if( token == T_EQ ) {
            val = I64EQ( e1, e2 );
        } else {
            val = I64NE( e1, e2 );
        }
        U32ToU64Set( e1, val );
        e1.no_sign = 0;
        PushOperand( e1, &loc );
        return( false );
    }
    return( true );
}

static bool CRelational( void )
/******************************
 * reduce a<b, a>b, a<=b, or a>=b
 */
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    TOKEN token;
    int val;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        switch( token ) {
        case T_LT:
            if( CompFlags.c99_extensions ) {
                if( e1.no_sign || e2.no_sign ) {
                    val = U64LT( e1, e2 );
                } else {
                    val = I64LT( e1, e2 );
                }
            } else {
                if( e1.no_sign || e2.no_sign ) {
                    val = U64Low( e1 ) < U64Low( e2 );
                } else {
                    val = I64Low( e1 ) < I64Low( e2 );
                }
            }
            U32ToU64Set( e1, val );
            break;
        case T_LE:
            if( CompFlags.c99_extensions ) {
                if( e1.no_sign || e2.no_sign ) {
                    val = U64LE( e1, e2 );
                } else {
                    val = I64LE( e1, e2 );
                }
            } else {
                if( e1.no_sign || e2.no_sign ) {
                    val = U64Low( e1 ) <= U64Low( e2 );
                } else {
                    val = I64Low( e1 ) <= I64Low( e2 );
                }
            }
            U32ToU64Set( e1, val );
            break;
        case T_GT:
            if( CompFlags.c99_extensions ) {
                if( e1.no_sign || e2.no_sign ) {
                    val = U64GT( e1, e2 );
                } else {
                    val = I64GT( e1, e2 );
                }
            } else {
                if( e1.no_sign || e2.no_sign ) {
                    val = U64Low( e1 ) > U64Low( e2 );
                } else {
                    val = I64Low( e1 ) > I64Low( e2 );
                }
            }
            U32ToU64Set( e1, val );
            break;
        case T_GE:
            if( CompFlags.c99_extensions ) {
                if( e1.no_sign || e2.no_sign ) {
                    val = U64GE( e1, e2 );
                } else {
                    val = I64GE( e1, e2 );
                }
            } else {
                if( e1.no_sign || e2.no_sign ) {
                    val = U64Low( e1 ) >= U64Low( e2 );
                } else {
                    val = I64Low( e1 ) >= I64Low( e2 );
                }
            }
            U32ToU64Set( e1, val );
            break;
        DbgDefault( "Default in CRelational\n" );
        }
        e1.no_sign = 0;
        PushOperand( e1, &loc );
        return( false );
    }
    return( true );
}

static bool CShift( void )
/*************************
 * reduce a<<b or a>>b
 */
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    TOKEN token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        switch( token ) {
        case T_RSHIFT:
            if( CompFlags.c99_extensions ) {
                if( U64Low( e2 ) > 64 || ( U64High( e2 ) != 0 ) ) {
                    if( e1.no_sign ) {
                        U64SetZero( e1 );
                    } else {
                        if( (signed int)U64Low( e1 ) < 0 ) {
                            U32ToU64Set( e1, -1 );
                        } else {
                            U64SetZero( e1 );
                        }
                    }
                } else {
                    if( e1.no_sign ) {
                        U64ShiftR( &(e1.u.uval), U64Low( e2 ), &e1.u.uval );
                    } else {
                        I64ShiftR( &(e1.u.sval), U64Low( e2 ), &e1.u.sval );
                    }
                }
            } else {
                if( U64Low( e2 ) > 32 || ( U64High( e2 ) != 0 ) ) {
                    if( e1.no_sign ) {
                        U64SetZero( e1 );
                    } else {
                        if( (signed int)U64Low( e1 ) < 0 ) {
                            U32ToU64Set( e1, -1 );
                        } else {
                            U64SetZero( e1 );
                        }
                    }
                } else {
                    if( e1.no_sign ) {
                        U32ToU64Set( e1, U64Low( e1 ) >> U64Low( e2 ) );
                    } else {
                        U32ToU64Set( e1, I64Low( e1 ) >> U64Low( e2 ) );
                    }
                }
            }
            break;
        case T_LSHIFT:
            if( CompFlags.c99_extensions ) {
                if( U64Low( e2 ) > 64 || ( U64High( e2 ) != 0 ) ) {
                    U64SetZero( e1 );
                } else {
                    U64ShiftL( &(e1.u.uval), U64Low( e2 ), &e1.u.uval );
                }
            } else {
                if( U64Low( e2 ) > 32 || ( U64High( e2 ) != 0 ) ) {
                    U64SetZero( e1 );
                } else {
                    U32ToU64Set( e1, U64Low( e1 ) << U64Low( e2 ) );
                }
            }
            break;
        DbgDefault( "Default in CShift\n" );
        }
        PushOperand( e1, &loc );
        return( false );
    }
    return( true );
}

static bool CAdditive( void )
/****************************
 * reduce a+b or a-b
 */
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    TOKEN token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        switch( token ) {
        case T_PLUS:
            if( CompFlags.c99_extensions ) {
                U64AddEq( e1, e2 );
            } else {
                U32ToU64Set( e1, U64Low( e1 ) + U64Low( e2 ) );
            }
            e1.no_sign |= e2.no_sign;
            break;
        case T_MINUS:
            if( CompFlags.c99_extensions ) {
                U64SubEq( e1, e2 );
            } else {
                U32ToU64Set( e1, U64Low( e1 ) - U64Low( e2 ) );
            }
            e1.no_sign = 0;
            break;
        DbgDefault( "Default in CAdditive\n" );
        }
        PushOperand( e1, &loc );
        return( false );
    }
    return( true );
}


static bool CMultiplicative( void )
/**********************************
 * reduce a/b or a*b
 */
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    TOKEN token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        switch( token ) {
        case T_TIMES:
            if( CompFlags.c99_extensions ) {
                U64MulEq( e1, e2 );
            } else {
                U32ToU64Set( e1, U64Low( e1 ) * U64Low( e2 ) );
            }
            break;
        case T_DIV:
            if( CompFlags.c99_extensions ) {
                if( U64Zero( e2 ) ) {
                    U64SetZero( e1 );
                } else if( e1.no_sign || e2.no_sign ) {
                    unsigned_64 unused;
                    U64Div( &(e1.u.uval), &(e2.u.uval), &(e1.u.uval), &unused );
                } else {
                    signed_64 unused;
                    I64Div( &((e1).u.sval), &((e2).u.sval), &((e1).u.sval), &unused );
                }
            } else {
                if( U64Zero( e2 ) ) {
                    U64SetZero( e1 );
                } else if( e1.no_sign || e2.no_sign ) {
                    U32ToU64Set( e1, U64Low( e1 ) / U64Low( e2 ) );
                } else {
                    U32ToU64Set( e1, I64Low( e1 ) / I64Low( e2 ) );
                }
            }
            break;
        case T_PERCENT:
            if( CompFlags.c99_extensions ) {
                if( U64Zero( e2 ) ) {
                    U64SetZero( e1 );
                } else if( e1.no_sign || e2.no_sign ) {
                    unsigned_64 unused;
                    U64Div( &(e1.u.uval), &(e2.u.uval), &unused, &e1.u.uval );
                } else {
                    signed_64 unused;
                    I64Div( &(e1.u.sval), &(e2.u.sval), &unused, &e1.u.sval );
                }
            } else {
                if( U64Zero( e2 ) ) {
                    U64SetZero( e1 );
                } else if( e1.no_sign || e2.no_sign ) {
                    U32ToU64Set( e1, U64Low( e1 ) % U64Low( e2 ) );
                } else {
                    U32ToU64Set( e1, I64Low( e1 ) % I64Low( e2 ) );
                }
            }
            break;
        DbgDefault( "Default in CMultiplicative\n" );
        }
        e1.no_sign |= e2.no_sign;
        PushOperand( e1, &loc );
        return( false );
    }
    return( true );
}

static bool CUnary( void )
/*************************
 * reduce +a or -a or !a or ~a
 */
{
    ppvalue p;
    loc_info operator_info;
    loc_info operand_info;
    TOKEN top;

    PopOperator( &top, &operator_info );
    if( PopOperand( &p, &operand_info ) &&
        ( operator_info.pos < operand_info.pos ) ) {
        switch( top ) {
        case T_UNARY_PLUS:
            break;
        case T_UNARY_MINUS:
            if( CompFlags.c99_extensions ) {
                U64Neg( &((p).u.uval), &((p).u.uval ) );
            } else {
                U32ToU64Set( p, - I64Low( p ) );
            }
            break;
        case T_EXCLAMATION:
//        case T_ALT_EXCLAMATION:
            if( I64Zero( p ) ) {
                U32ToU64Set( p, 1 );
            } else {
                I64SetZero( p );
            }
            p.no_sign = 0;
            break;
        case T_TILDE:
//        case T_ALT_TILDE:
            if( CompFlags.c99_extensions ) {
                U64Not( &(p.u.sval), &(p.u.sval) );
            } else {
                U32ToU64Set( p, ~U64Low( p ) );
            }
            break;
        DbgDefault( "Default in CUnary\n" );
        }
        PushOperand( p, &operator_info );
    } else {
        SetErrLoc( &operator_info.locn );
        CErr2p( ERR_UNARY_OPERATOR_MISSING_OPERAND, Tokens[top] );
        return( true );
    }
    return( false );
}

static bool CStart( void )
/************************/
{
    TOKEN top;

    PopOperator( &top, NULL );
    if( CurToken != T_NULL ) {
        unexpectedCurToken();
    }
    return( true );
}

#if 0
static void stringize( char *s )
/******************************/
{
    char    *d;

    d = s;
    while( *s != '\0' ) {
        if( s[0] == '\\' ) {
            if( s[1] == '\\' || s[1] == '\"' ) {
                s++;
            }
        }
        *d++ = *s++;
    }
    *d = '\0';
}

TOKEN Process_Pragma( bool internal )
/***********************************/
{
    PpNextToken();
    if( CurToken == T_LEFT_PAREN ) {
        PpNextToken();
        if( CurToken == T_STRING ) {
            char    *token_buf;

            token_buf = CStrSave( Buffer );
            PpNextToken();
            if( CurToken == T_RIGHT_PAREN ) {
                ppctl_t old_ppctl;

                stringize( token_buf );
                InsertReScanPragmaTokens( token_buf, internal );
                // call CPragma()
                old_ppctl = PPControl;
                PPCTL_ENABLE_EOL();
                CPragma();
                PPControl = old_ppctl;
            } else {
                /* error, incorrect syntax of the operator _Pragma() */
            }
            CMemFree( (void *)token_buf );
            PpNextToken();
        } else {
            /* error, incorrect syntax of the operator _Pragma() */
        }
    } else {
        InsertToken( CurToken, Buffer, internal );
        strcpy( Buffer, PPOPERATOR_PRAGMA );
        TokenLen = LENLIT( PPOPERATOR_PRAGMA );
        CurToken = T_ID;
    }
    return( CurToken );
}
#endif

static bool ( *CExpr[] )(void) = { // table of functions to reduce expressions
    CStart,             /* Level 0 */
    CRightParen,        /* Level 1 */
    CLeftParen,         /* Level 2 */
    CConditional,       /* Level 3 */
    CConditional,       /* Level 4 */
    CLogicalOr,         /* Level 5 */
    CLogicalAnd,        /* Level 6 */
    COr,                /* Level 7 */
    CXOr,               /* Level 8 */
    CAnd,               /* Level 9 */
    CEquality,          /* Level 10 */
    CRelational,        /* Level 11 */
    CShift,             /* Level 12 */
    CAdditive,          /* Level 13 */
    CMultiplicative,    /* Level 14 */
    CUnary              /* Level 15 */
};

static void PrecedenceParse( ppvalue *p )
/****************************************
 * main precedence parse algorithm
 */
{
    int prec_token;
    int prec_operator;
    bool done;
    TOKEN top;
    ppvalue empty;
    loc_info loc;
    unsigned error_info;

    U32ToU64Set( *p, 0 );    //default value

    if( CurToken == T_NULL ) {
        unexpectedCurToken();
        return;
    }
    CErrCheckpoint( &error_info );
    prec_operator = 0;
    Pos = 0;
    loc.locn = SrcFileLoc;
    loc.pos = Pos;
    PushOperator( T_START, &loc, Prec[T_START] ); // problem because T_START is not a ppvalue
    Pos++;
    CheckToken( T_START ); // check for initial unary + or -
    prec_operator = 0;
    done = false;
    while( !done ) {
        if( IS_OPERAND( CurToken ) ) {
            done = COperand(); // get operand and read next token
        } else if( CurToken == T_LEFT_PAREN ) {
            PushCurToken( Prec[T_LEFT_PAREN] ); // always push left paren
            done = PpNextToken();
        } else {
            TopOperator( &top, &prec_operator );
            if( CurToken < LAST_TOKEN_PREC ) {
                prec_token = Prec[CurToken];
                if( prec_token < prec_operator
                  || ( ( prec_token == prec_operator ) && ( prec_token != PREC_UNARY ) ) ) {
                    done = CExpr[prec_operator](); // reduce
                } else {
                    PushCurToken( prec_token ); // shift
                    done = PpNextToken();
                }
            } else {
                unexpectedCurToken();
                done = true;
            }
        }
    }
    if( !CErrOccurred( &error_info ) ) {
        PopOperand( p, NULL );
        if( PopOperand( &empty, &loc ) ) {
            SetErrLoc( &loc.locn );
            CErr1( ERR_EXTRA_OPERAND );
        }
    }
    if( CErrOccurred( &error_info ) ) {
        // scan until EOL (T_NULL) to avoid multiple messages
        while( CurToken != T_NULL ) {
            NextToken();
        }
    }
    // empty stacks
    while( PopOperand( NULL, NULL ) ); // free stack
    while( PopOperator( NULL, NULL ) ); // free stack
}

bool PpConstExpr( void )
/***********************
 * Entry into ppexpn module
 */
{
    ppvalue val;

    PrecedenceParse( &val );
    return( I64NonZero( val ) );
}

void InitPPexpn( void )
/*********************/
{
    HeadOperand = NULL;
    HeadOperator = NULL;
}

void FiniPPexpn( void )
/*********************/
{
    if( HeadOperand != NULL ) {
        CMemFree( HeadOperand );
        HeadOperand = NULL;
    }
    if( HeadOperator != NULL ) {
        CMemFree( HeadOperator );
        HeadOperator = NULL;
    }
}
