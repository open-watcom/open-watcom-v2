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


#include <string.h>
#include <limits.h>

#include "plusplus.h"
#include "errdefns.h"
#include "preproc.h"
#include "fold.h"
#include "stack.h"
#include "initdefs.h"
#include "carve.h"

#define L       I64LO32
#define H       I64HI32

#define I64Zero( a ) ( (a).uval.u._32[L] == 0 && (a).uval.u._32[H] == 0 )
#define I64NonZero(a) ( ! I64Zero(a)  )
#define U64Zero( a ) ( (a).uval.u._32[L] == 0 && (a).uval.u._32[H] == 0 )
#define U64NonZero(a) ( ! U64Zero(a)  )
#define U64Low( a ) ( a.uval.u._32[L] )
#define U64High( a ) ( a.uval.u._32[H] )

#define U64LT( a, b ) ( U64Cmp( &((a).uval), &((b).uval) ) < 0 )
#define U64GT( a, b ) ( U64Cmp( &((a).uval), &((b).uval) ) > 0 )
#define U64LE( a, b ) ( U64Cmp( &((a).uval), &((b).uval) ) <= 0 )
#define U64GE( a, b ) ( U64Cmp( &((a).uval), &((b).uval) ) >= 0 )
#define U64EQ( a, b ) ( U64Cmp( &((a).uval), &((b).uval) ) == 0 )
#define U64NE( a, b ) ( U64Cmp( &((a).uval), &((b).uval) ) != 0 )

#define I64LT( a, b ) ( I64Cmp( &((a).sval), &((b).sval) ) < 0 )
#define I64GT( a, b ) ( I64Cmp( &((a).sval), &((b).sval) ) > 0 )
#define I64LE( a, b ) ( I64Cmp( &((a).sval), &((b).sval) ) <= 0 )
#define I64GE( a, b ) ( I64Cmp( &((a).sval), &((b).sval) ) >= 0 )
#define I64EQ( a, b ) ( I64Cmp( &((a).sval), &((b).sval) ) == 0 )
#define I64NE( a, b ) ( I64Cmp( &((a).sval), &((b).sval) ) != 0 )

#define U64AddEq(a,b) U64Add( &((a).uval), &((b).uval), &((a).uval) );
#define U64SubEq(a,b) U64Sub( &((a).uval), &((b).uval), &((a).uval) );
#define U64MulEq(a,b) U64Mul( &((a).uval), &((b).uval), &((a).uval) );
#define U64AndEq(a,b) U64And( &((a).uval), &((b).uval), &((a).uval) );
#define U64OrEq(a,b)  U64Or( &((a).uval), &((b).uval), &((a).uval) );
#define U64XOrEq(a,b) U64Xor( &((a).uval), &((b).uval), &((a).uval) );


#define I64SetZero( a ) ( I32ToI64( 0, &(a).sval) );
#define U64SetZero( a ) ( U32ToU64( 0, &(a).uval) );

/* include ctokens.h for the precedence values */
#define prec(value) value
#define pick(token,string,class)
#define no_keywords
static  int    Prec[] = {     // table of token precedences
#include "ctokens.h"
#undef no_keywords

#ifndef NDEBUG
#define prec(value)
#define pick(token,string,class) string
#define no_keywords
static char * TokenNames[] = {
#include "ctokens.h"
#undef no_keywords
#endif

#define NUM_PREC (sizeof(Prec) / sizeof(int))

#define IS_OPERAND( token ) ( ( token == T_ID ) || ( token == T_CONSTANT ) )

typedef struct ppvalue {
    union {
        unsigned_64    uval;
        signed_64      sval;
    };
    unsigned            no_sign : 1;
} ppvalue;

typedef struct loc_info {
    int pos;                    // incremented when token read
    TOKEN_LOCN  locn;           // token location in source file
} loc_info;

typedef struct operand_stack PPEXPN_OPERAND_STACK; // stack to store operands
struct operand_stack {
    PPEXPN_OPERAND_STACK *next;
    ppvalue     value;
    loc_info    loc;
};
static PPEXPN_OPERAND_STACK *HeadOperand = NULL;


typedef struct token_stack PPEXPN_OPERATOR_STACK; // stack to store operators
struct token_stack {
    PPEXPN_OPERATOR_STACK *next;
    int token;
    loc_info loc;
    int prec;
};
static PPEXPN_OPERATOR_STACK *HeadOperator = NULL;

static carve_t carve_ppoperand_stack;    // carver: operand stack entries
static carve_t carve_pptoken_stack;      // carver: operator stack entries

static int Pos = 0;                     // position of CurToken in parsing

#ifdef NDEBUG
    #define DbgDumpOperatorStack()
    #define DbgDumpOperandStack()
#else

#include "pragdefn.h"

void DbgDumpOperatorStack()            // dump PPEXPN_OPERATOR_STACK
{
    PPEXPN_OPERATOR_STACK *cur;

    if( PragDbgToggle.ppexpn ) {
        Stack_forall( HeadOperator, cur ) {
            printf("Token: %d %s Pos %d\n", cur->token
                                          , TokenNames[cur->token]
                                          , cur->loc.pos );
        }
    }
}

void DbgDumpToken( int token )        // dump PPEXPN_OPERAND_STACK
{
    printf("Token: %s\n", TokenNames[token] );
}

void DbgDumpOperandStack()            // dump PPEXPN_OPERAND_STACK
{
    PPEXPN_OPERAND_STACK *cur;

    if( PragDbgToggle.ppexpn ) {
        Stack_forall( HeadOperand, cur ) {
            printf("uval: %d sval %d no_sign %d Pos %d\n", cur->value.uval,
                    cur->value.sval, cur->value.no_sign, cur->loc.pos );
        }
    }
}
#endif

static void PrecedenceParse( ppvalue * );
static boolean CStart( void );
static boolean CRightParen( void );
static boolean CLeftParen( void );
static boolean CConditional( void );
static boolean CLogicalOr( void );
static boolean CLogicalAnd(void );
static boolean COr( void );
static boolean CXOr( void );
static boolean CAnd( void );
static boolean CEquality( void );
static boolean CRelational( void );
static boolean CShift( void );
static boolean CAdditive( void );
static boolean CMultiplicative( void );
static boolean CUnary( void );

boolean ( *CExpr[] )(void) = { // table of functions to reduce expressions
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
#define UNARY_PREC (14)

static void ppexpnInit(         // INITIALIZATION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;

    carve_ppoperand_stack = CarveCreate( sizeof( PPEXPN_OPERAND_STACK ), 16 );
    HeadOperand = NULL;

    carve_pptoken_stack = CarveCreate( sizeof( PPEXPN_OPERATOR_STACK ), 16 );
    HeadOperator = NULL;
}

static void ppexpnFini( // COMPLETION FOR MODULE
    INITFINI* defn ) // - definition
{
    defn = defn;

    CarveDestroy( carve_ppoperand_stack );
    HeadOperand = NULL;

    CarveDestroy( carve_pptoken_stack );
    HeadOperator = NULL;
}

INITDEFN( ppexpn, ppexpnInit, ppexpnFini );


static void PushOperator( int token, loc_info *loc, int prec )
{
    PPEXPN_OPERATOR_STACK *stack_entry;

    stack_entry = CarveAlloc( carve_pptoken_stack );
    stack_entry->token = token;
    stack_entry->loc = *loc;
    stack_entry->prec = prec;
    StackPush( &HeadOperator, stack_entry );
}

static void PushCurToken( int prec )
{
    loc_info loc;

    SrcFileGetTokenLocn( &loc.locn );
    loc.pos = Pos;
    PushOperator( CurToken, &loc, prec );
}


static boolean PopOperator( int *token, loc_info *loc )
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
        CarveFree( carve_pptoken_stack, stack_entry );
        return( TRUE );
    }
    return( FALSE );
}

static boolean TopOperator( int *token, int *prec )
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
        return( TRUE );
    }
    return( FALSE );
}

static void PushOperand( ppvalue p, loc_info *loc )
{
    PPEXPN_OPERAND_STACK *stack_entry;

    stack_entry = CarveAlloc( carve_ppoperand_stack );
    stack_entry->value = p;
    stack_entry->loc = *loc;
    StackPush( &HeadOperand, stack_entry );
}

static void PushOperandCurLocation( ppvalue p )
{
    loc_info loc;

    SrcFileGetTokenLocn( &loc.locn );
    loc.pos = Pos;
    PushOperand( p, &loc );
}

static boolean PopOperand( ppvalue *p, loc_info *loc )
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
        CarveFree( carve_ppoperand_stack, stack_entry );
        return( TRUE );
    }
    return( FALSE );
}

static boolean CheckToken( int prev_token )
{
    if( IS_OPERAND( prev_token ) && IS_OPERAND( CurToken ) ) {
        CErr1( ERR_CONSECUTIVE_OPERANDS ); //  can't have 2 operands in a row
        return( TRUE );
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
    return( FALSE );
}


static boolean PpNextToken() // scan the next token and check for errors
{
    static int prev_token;

    prev_token = CurToken;
    NextToken();
    Pos++;
    return( CheckToken( prev_token ) );
}

long int PpConstExpr() // Entry into ppexpn module
{
    ppvalue val;

    PrecedenceParse( &val );
    return( I64NonZero( val ) );
}

static void unexpectedCurToken( void )
{
    CErr2p( ERR_UNEXPECTED_IN_CONSTANT_EXPRESSION, Tokens[CurToken] );
}

static void PrecedenceParse( ppvalue *p ) // main precedence parse algorithm
{
    int prec_token;
    int prec_operator;
    boolean done;
    int top;
    ppvalue empty;
    loc_info loc;
    error_state_t error_info;

    U32ToU64( 0, &(p->sval) ); //default value

    if( CurToken == T_NULL ) {
        unexpectedCurToken();
        return;
    }
    CErrCheckpoint( &error_info );
    Pos = 0;
    SrcFileGetTokenLocn( &loc.locn );
    loc.pos = Pos;
    PushOperator( T_START, &loc, Prec[T_START] ); // problem because T_START is not a ppvalue
    Pos++;
    CheckToken( T_START ); // check for initial unary + or -
    done = FALSE;
    while( !done ) {
        if( IS_OPERAND( CurToken ) ) {
            done = COperand(); // get operand and read next token
        } else if( CurToken == T_LEFT_PAREN ) {
            PushCurToken( Prec[T_LEFT_PAREN] ); // always push left paren
            done = PpNextToken();
        } else {
            TopOperator( &top, &prec_operator );
            DbgVerify( ( prec_operator >= 0 ) && (prec_operator < NUM_PREC ),
                       "Operator on stack with back precedence " );
            if( CurToken < NUM_PREC ) {
                prec_token = Prec[CurToken];
                if( prec_token < prec_operator ||
                    ( ( prec_token == prec_operator )
                   && ( prec_token != PREC_UNARY ) ) ) {
                    done = CExpr[prec_operator](); // reduce
                } else {
                    PushCurToken( prec_token ); // shift
                    done = PpNextToken();
                }
            } else {
                unexpectedCurToken();
                done = TRUE;
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

static boolean CRightParen() // reduce extra )
{
    int right_paren;
    loc_info right_info;

    // as ) doesn't get pushed for (expr), this must be an unmatched )
    if( PopOperator( &right_paren, &right_info ) ) {
        SetErrLoc( &right_info.locn );
        CErr1( ERR_UNMATCHED_RIGHT_PAREN );
    }
    return( FALSE );
}

static boolean CLeftParen() // reduce (expr)
{
    int left_paren;
    loc_info left_info;
    loc_info e1_info;
    ppvalue e1;

    PopOperator( &left_paren, &left_info );
    if( CurToken != T_RIGHT_PAREN ) { // expect ) as current token
        SetErrLoc( &left_info.locn );
        CErr1( ERR_UNMATCHED_LEFT_PAREN );
        return( TRUE );
    }
    if( PopOperand( &e1, &e1_info ) ) {
        if( ( e1_info.pos < Pos ) && ( e1_info.pos > left_info.pos ) ) {
            PushOperand( e1, &e1_info );
            return( PpNextToken() );
        }
    }
    SetErrLoc( &left_info.locn );
    CErr1( ERR_EMPTY_PAREN );
    return( TRUE );
}

static boolean CConditional() // reduce an a?b:c expression
{
    loc_info e1_info;
    loc_info e2_info;
    loc_info e3_info;
    loc_info op1_info;
    loc_info op2_info;
    ppvalue e1;
    ppvalue e2;
    ppvalue e3;
    int op1;
    int op2;

    PopOperator( &op2, &op2_info );
    if( op2 != T_COLON ) {
        if( ( op2 != T_QUESTION && op2 != T_START ) || CurToken == T_NULL ) {
            SetErrLoc( &op2_info.locn );
            CErr1( ERR_CONDITIONAL_MISSING_COLON );
            return( TRUE );
        } else {
            PushOperator( op2, &op2_info, Prec[op2] );
            PushCurToken( Prec[ CurToken] );
            return( PpNextToken() );
        }
    }
    // always something to pop next, even if its T_START
    PopOperator( &op1, &op1_info );
    if( op1 != T_QUESTION ) {
        if( ( op1 != T_COLON && op1 != T_START ) || ( CurToken == T_NULL ) ) {
            SetErrLoc( &op1_info.locn );
            CErr1( ERR_CONDITIONAL_MISSING_QUESTION );
            return( TRUE );
        }
    }
    if( op2 != T_COLON || op1 != T_QUESTION ) {
        PushOperator( op1, &op1_info, Prec[op1] );
        PushOperator( op2, &op2_info, Prec[op2] );
        PushCurToken( Prec[ CurToken] );
        return( PpNextToken() );
    }
    if( PopOperand( &e3, &e3_info ) && ( e3_info.pos > op2_info.pos ) ) {
        if( PopOperand( &e2, &e2_info ) && ( e2_info.pos < op2_info.pos ) &&
            ( e2_info.pos > op1_info.pos ) ) {
            if( PopOperand( &e1, &e1_info ) &&
                ( e1_info.pos < op1_info.pos ) ) {
                if( I64NonZero( e1 ) ) {
                    e1.sval = e2.sval;
                } else {
                    e1.sval = e3.sval;
                }
                e1.no_sign = e2.no_sign | e3.no_sign;
                PushOperand( e1, &e1_info );
                return( FALSE );
            } else {
                SetErrLoc( &op1_info.locn ); // missing a in a?b:c
                CErr1( ERR_CONDITIONAL_MISSING_FIRST_OPERAND );
            }
        } else {
            SetErrLoc( &op1_info.locn ); // missing b in a?b:c
            CErr1( ERR_CONDITIONAL_MISSING_SECOND_OPERAND );
        }
    } else {
        SetErrLoc( &op2_info.locn ); // missing c in a?b:c
        CErr1( ERR_CONDITIONAL_MISSING_THIRD_OPERAND );
    }
    return( TRUE );
}

static boolean Binary( // pop binary operand and two operands, error check
    int *token, ppvalue *e1, ppvalue *e2, loc_info *loc )
{
    loc_info e1_info;
    loc_info e2_info;


    PopOperator( token, loc );
    if( PopOperand( e2, &e2_info ) && ( e2_info.pos > loc->pos ) ) {
        if( PopOperand( e1, &e1_info ) && ( e1_info.pos < loc->pos ) ) {
            return( TRUE );
        } else {
            SetErrLoc( &loc->locn );
            CErr2p( ERR_BINARY_MISSING_LEFT_OPERAND, Tokens[*token] );
        }
    } else {
        SetErrLoc( &loc->locn );
        CErr2p( ERR_BINARY_MISSING_RIGHT_OPERAND, Tokens[*token] );
    }
    return( FALSE );
}

static boolean CLogicalOr() // reduce a || b
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    int token;

    if( Binary( &token, &e1, &e2, &loc ) ) {

        if( I64Zero( e1 ) ) { // e1 is zero, so consider e2
            if( I64NonZero( e2 ) ) { // e2 non-zero
                I32ToI64( 1, &e1.sval ); // answer is 1
            } else {
                I64SetZero(e1);
            }
        } else {
            I32ToI64( 1, &e1.sval ); // answer is 1
        }
        e1.no_sign = 0;
        PushOperand( e1, &loc );
        return( FALSE );
    }
    return( TRUE );
}

static boolean CLogicalAnd() // reduce a && b
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    int token;

    if( Binary( &token, &e1, &e2, &loc ) ) {

        if( I64NonZero( e1 ) ) { // e1 is non-zero
            if( I64Zero( e2 ) ) { // e2 is zero
                e1.sval = e2.sval;
            } else {
                I32ToI64( 1, &e1.sval ); // answer is 1
            }
        }
        // else e1 is already zero
        e1.no_sign = 0;
        PushOperand( e1, &loc );
        return( FALSE );
    }
    return( TRUE );
}

static boolean COr() // reduce a|b
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    int token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        U64OrEq( e1, e2 );
        e1.no_sign |= e2.no_sign;
        PushOperand( e1, &loc );
        return( FALSE );
    }
    return( TRUE );
}

static boolean CXOr() // reduce a^b
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    int token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        U64XOrEq( e1, e2 );
        e1.no_sign |= e2.no_sign;
        PushOperand( e1, &loc );
        return( FALSE );
    }
    return( TRUE );
}

static boolean CAnd() // reduce a&b
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    int token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        U64AndEq( e1, e2 );
        e1.no_sign |= e2.no_sign;
        PushOperand( e1, &loc );
        return( FALSE );
    }
    return( TRUE );
}

static boolean CEquality() // reduce a == b or a != b
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    int token;
    int val;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        if( token == T_EQ ) {
            val = I64EQ( e1, e2 );
        } else {
            val = I64NE( e1, e2 );
        }
        I32ToI64( val, &e1.sval );
        e1.no_sign = 0;
        PushOperand( e1, &loc );
        return( FALSE );
    }
    return( TRUE );
}

static boolean CRelational() // reduce a<b, a>b, a<=b, or a>=b
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    int token;
    int val;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        switch( token ) {
          case T_LT :
            if( e1.no_sign || e2.no_sign ) {
                val = U64LT( e1, e2 );
            } else {
                val = I64LT( e1, e2 );
            }
            I32ToI64( val, &(e1.sval) );
            break;
          case T_LE :
            if( e1.no_sign || e2.no_sign ) {
                val = U64LE( e1, e2 );
            } else {
                val = I64LE( e1, e2 );
            }
            I32ToI64( val, &(e1.sval) );
            break;
          case T_GT :
            if( e1.no_sign || e2.no_sign ) {
                val = U64GT( e1, e2 );
            } else {
                val = I64GT( e1, e2 );
            }
            I32ToI64( val, &(e1.sval) );
            break;
          case T_GE :
            if( e1.no_sign || e2.no_sign ) {
                val = U64GE( e1, e2 );
            } else {
                val = I64GE( e1, e2 );
            }
            I32ToI64( val, &(e1.sval) );
            break;
          DbgDefault( "Default in CRelational\n" );
        }
        e1.no_sign = 0;
        PushOperand( e1, &loc );
        return( FALSE );
    }
    return( TRUE );
}

static boolean CShift() // reduce a<<b or a>>b
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    int token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        switch( token ) {
          case T_RSHIFT :
            if( U64Low(e2) > 64 || ( U64High( e2 ) != 0 ) ) {
                if( e1.no_sign ) {
                    U64SetZero( e1 );
                } else {
                    if( (signed int)U64Low( e1 ) < 0 ) {
                        U32ToU64( -1, &e1.sval );
                    } else {
                        U64SetZero( e1 );
                    }
                }
            } else {
                if( e1.no_sign ) {
                        U64ShiftR( &(e1.uval), U64Low(e2), &e1.uval );
                } else {
                    I64ShiftR( &(e1.sval), U64Low( e2), &e1.sval );
                }
            }
            break;
          case T_LSHIFT :
            if( U64Low(e2) > 64 || ( U64High( e2 ) != 0 ) ) {
                U64SetZero( e1 );
            } else {
                U64ShiftL( &(e1.uval), U64Low( e2 ), &e1.uval );
            }
            break;
          DbgDefault( "Default in CShift\n" );
        }
        PushOperand( e1, &loc );
        return( FALSE );
    }
    return( TRUE );
}

static boolean CAdditive() // reduce a+b or a-b
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    int token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        switch( token ) {
          case T_PLUS :
            U64AddEq( e1, e2 );
            e1.no_sign |= e2.no_sign;
            break;
          case T_MINUS :
            U64SubEq( e1, e2 );
            e1.no_sign = 0;
            break;
          DbgDefault( "Default in CAdditive\n" );
        }
        PushOperand( e1, &loc );
        return( FALSE );
    }
    return( TRUE );
}


static boolean CMultiplicative() // reduce a/b or a*b
{
    ppvalue e1;
    ppvalue e2;
    loc_info loc;
    int token;

    if( Binary( &token, &e1, &e2, &loc ) ) {
        switch( token ) {
          case T_TIMES :
            U64MulEq( e1, e2 );
            break;
          case T_DIVIDE :
            if( U64Zero( e2 ) ) {
                U64SetZero( e1 );
            } else if( e1.no_sign || e2.no_sign ) {
                unsigned_64 unused;
                U64Div( &(e1.uval), &(e2.uval), &(e1.uval), &unused );
            } else {
                signed_64 unused;
                I64Div( &((e1).sval), &((e2).sval), &((e1).sval), &unused );
            }
            break;
          case T_PERCENT :
            if( U64Zero( e2 ) ) {
                U64SetZero( e1 );
            } else if( e1.no_sign || e2.no_sign ) {
                unsigned_64 unused;
                U64Div( &(e1.uval), &(e2.uval), &unused, &e1.uval );
            } else {
                signed_64 unused;
                I64Div( &(e1.sval), &(e2.sval), &unused, &e1.sval );
            }
            break;
          DbgDefault( "Default in CMultiplicative\n" );
        }
        e1.no_sign |= e2.no_sign;
        PushOperand( e1, &loc );
        return( FALSE );
    }
    return( TRUE );
}

static boolean CUnary() // reduce +a or -a or !a or ~a
{
    ppvalue p;
    loc_info operator_info;
    loc_info operand_info;
    int top;

    PopOperator( &top, &operator_info );
    if( PopOperand( &p, &operand_info ) &&
        ( operator_info.pos < operand_info.pos ) ) {
        switch( top ) {
          case T_UNARY_PLUS:
            break;
          case T_UNARY_MINUS:
            U64Neg( &((p).uval), &((p).uval ) );
            break;
          case T_EXCLAMATION:
            if( I64Zero( p ) ) {
                I32ToI64( 1, &(p.sval) );
            } else {
                I64SetZero( p );
            }
            p.no_sign = 0;
            break;
          case T_TILDE:
            U64Not( &(p.sval), &(p.sval) );
            break;
          DbgDefault( "Default in CUnary\n" );
        }
        PushOperand( p, &operator_info );
    } else {
        SetErrLoc( &operator_info.locn );
        CErr2p( ERR_UNARY_OPERATOR_MISSING_OPERAND, Tokens[top] );
        return( TRUE );
    }
    return( FALSE );
}

static boolean CStart()
{
    int top;

    PopOperator( &top, NULL );
    if( CurToken != T_NULL ) {
        unexpectedCurToken();
    }
    return( TRUE );
}

static boolean COperand()
{
    ppvalue p;
    loc_info loc;
    TOKEN_LOCN left_loc;
    boolean done;

    done = FALSE;
    switch( CurToken ) {
      case T_ID:
        SrcFileGetTokenLocn( &loc.locn ); // need this to store result
        loc.pos = Pos;
        Pos++;
        if( strcmp( "defined", Buffer ) == 0 ) {
            PPState = PPS_EOL | PPS_NO_EXPAND;
            NextToken(); // Don't error check: can have T_ID T_ID here
            if( CurToken == T_LEFT_PAREN ) {
                SrcFileGetTokenLocn( &left_loc );
                NextToken(); // no need to error check or advance Pos
                I32ToI64( isMacroDefined(), &(p.sval) );
                NextToken(); // no need to error check or advance Pos
                if( CurToken != T_RIGHT_PAREN ) {
                    SetErrLoc( &left_loc );
                    CErr1( ERR_UNMATCHED_LEFT_PAREN );
                    done = TRUE;
                }
            } else {
                I32ToI64( isMacroDefined(), &(p.sval) );
            }
        } else {
            CErr2p( WARN_UNDEFD_MACRO_IS_ZERO, Buffer );
            I64SetZero( p );
        }
        p.no_sign = 0;
        if( !done ) {
            PushOperand( p, &loc );
            done = PpNextToken();
        }
        break;
      case T_CONSTANT:
        switch( ConstType ) {
          case TYP_FLOAT:
          case TYP_DOUBLE:
          case TYP_LONG_DOUBLE:
            CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
            done = TRUE;
            I32ToI64( SafeAtof( Buffer ), &(p.sval) );
            // LMW add long double support if available
            p.no_sign = 0;
            break;
          case TYP_WCHAR:
          case TYP_UCHAR:
          case TYP_USHORT:
          case TYP_UINT:
          case TYP_ULONG:
          case TYP_ULONG64:
            p.uval = Constant64;
            p.no_sign = 1;
            break;
          default:
            p.sval = Constant64;
            p.no_sign = 0;
        }
        if (!done ) {
            PushOperandCurLocation( p );
            done = PpNextToken();
        }
        break;
      DbgDefault( "Default in COperand\n" );
    }
    return( done );
}

static int isMacroDefined()
{
    PPState = PPS_EOL;
    return MacroDependsDefined();
}
