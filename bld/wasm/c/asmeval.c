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

#include "asmglob.h"
#include "asmops1.h"
#include "asmops2.h"
#include "asmerr.h"
#include "asmsym.h"
#include "asmalloc.h"
#include "asmeval.h"

#include "watcom.h"
#include "myassert.h"

extern void             AsmError( int );
extern void             DefFlatGroup();

extern struct asm_tok   *AsmBuffer[];
extern char             Parse_pass;

static int              TakeOut[ MAX_TOKEN ];
static int              TokCnt;

static struct asm_tok   *Store;
static unsigned         StoreNum;
static expr_list        *ExprListCache;

extern char             Parse_Pass;     // phase of parsing

static void expr_free( expr_list *expr )
{
    if( expr != NULL ) {
        expr->string = (void *)ExprListCache;
        ExprListCache = expr;
    }
}

static expr_list *create_expr( void )
{
    expr_list   *new;

    if( ExprListCache != NULL ) {
        new = ExprListCache;
        ExprListCache = (void *)new->string;
    } else {
        new = AsmAlloc( sizeof( expr_list ) );
    }
    new->label    = EMPTY;
    new->override = EMPTY;
    new->instr    = EMPTY;
    new->base_reg = EMPTY;
    new->idx_reg  = EMPTY;
    new->indirect = FALSE;
    new->explicit = FALSE;
    new->expr_type= EMPTY;
    new->value    = 0;
    new->scale    = 1;
    new->string   = NULL;
    return( new );
}

static int get_precedence( int i )
{
    /* Base on MASM 6.0 pg.18 Table 1.3 */

    char        token;

    token = AsmBuffer[i]->token;

    switch( token ) {
        case T_UNARY_OPERATOR:
            switch( AsmBuffer[i]->value ) {
                case T_LENGTH:
                case T_SIZE:
                case T_LENGTHOF:
                case T_SIZEOF:
                    return( 2 );
                case T_SEG2:
                case T_OFFSET:
                    return( 5 );
            }
            break;
        case T_INS:
            switch( AsmBuffer[i]->value ) {
                case T_MOD:
                case T_SHL:
                case T_SHR:
                    return( 8 );
                case T_EQ:
                case T_NE:
                case T_LT:
                case T_LE:
                case T_GT:
                case T_GE:
                    return( 10 );
                case T_NOT:
                    return( 11 );
                case T_AND:
                    return( 12 );
                case T_OR:
                case T_XOR:
                    return( 13 );
            }
        case T_COLON:
            return( 4 );
        case T_POSITIVE:
        case T_NEGATIVE:
            return( 7 );
        case '*':
        case '/':
            return( 8 );
        case '+':
        case '-':
            return( 9 );
        default:
            /**/myassert( 0 );
    }
    return( ERROR );
}

static expr_list *get_operand( int i )
{
    expr_list   *new;
    char        *tmp;

    new = create_expr();
    switch( AsmBuffer[i]->token ) {
    case T_NUM:
        new->type = EXPR_CONST;
        new->value = AsmBuffer[i]->value;
        break;
    case T_STRING:
        new->type = EXPR_CONST;
        new->string = AsmBuffer[i]->string_ptr;
        new->value = 0;
        for( tmp = AsmBuffer[i]->string_ptr; *tmp != '\0'; tmp++ ) {
            new->value <<= 8;
            new->value |= (*tmp);
        }
        break;
    case T_REG:
        new->type = EXPR_REG;
        new->base_reg = i;
        break;
    case T_ID:
    case T_RES_ID:
        new->type = EXPR_ADDR;
        new->label = i;
        break;
    default:
        expr_free( new );
        new = NULL;
        break;
    }
    return( new );
}

static void optr_enqueue( int index, int_8 queue[] )
{
    int_8               i;

    if( queue[0] == EMPTY ) {
        i = 0;
    } else {
        /**/myassert( queue[1] == EMPTY );
        i = 1;
    }
    queue[i] = index;
}

static void optr_dequeue( int_8 queue[] )
{
    queue[0] = queue[1];
    queue[1] = EMPTY;
}

static int_8 is_optr( int i )
/* determine if it is an operator */
{
    switch( AsmBuffer[i]->token ) {
    case T_REG:
    case T_NUM:
    case T_ID:
    case T_RES_ID:
    case T_STRING:
    case T_PATH:
    case T_OP_BRACKET:
        return( FALSE );
    }
    return( TRUE );
}

static int_8 is_unary( int i, char sign )
/* determine if it is an unary operand */
{
    char        tok;

    tok = AsmBuffer[i]->token;

    if( tok == T_UNARY_OPERATOR  ) {
        return( TRUE );
    }

    switch( tok ) {
        case T_INS:
            if( AsmBuffer[i]->value == T_NOT ) {
                return( TRUE );
            }
            break;
        case T_POSITIVE:
        case T_NEGATIVE:
            return( TRUE );
        case '+':
        case '-':
            if( sign ) {
                // sign operator is allowed
                if( tok == '+' ) {
                    AsmBuffer[i]->token = T_POSITIVE;
                    return( TRUE );
                } else if( tok == '-' ) {
                    AsmBuffer[i]->token = T_NEGATIVE;
                    return( TRUE );
                }
            }
            break;
        default:
            break;
    }
    return( FALSE );
}

static int_8 cmp_token( int i, long tok )
/* compare AsmBuffer[i] and tok */
{
    if( AsmBuffer[i]->token == T_INS ) {
        if( AsmBuffer[i]->value == tok ) {
            return( TRUE );
        }
    } else {
        if( (long)(AsmBuffer[i]->token) == tok ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static int_8 check_same( expr_list *tok_1, expr_list *tok_2, int_8 type )
/* Check if both tok_1 and tok_2 equal type */
{
    if( tok_1->type == type &&
        tok_2->type == type ) {
        return( TRUE );
    } else {
        return( FALSE );
    }
}

static int_8 check_both( expr_list *tok_1, expr_list *tok_2, int_8 type1, int_8 type2 )
/* Check if tok_1 == type1 and tok_2 == type2 or vice versa */
{
    if( tok_1->type == type1 &&
        tok_2->type == type2 ) {
        return( TRUE );
    } else if( tok_1->type == type2 &&
               tok_2->type == type1 ) {
        return( TRUE );
    } else {
        return( FALSE );
    }
}

static void index_connect( expr_list *tok_1, expr_list *tok_2 )
/* Connects the register lists */
{
    int         reg;

    if( tok_2->base_reg != EMPTY ) {
        reg = tok_2->base_reg;
    } else if( tok_2->idx_reg != EMPTY ) {
        reg = tok_2->idx_reg;
    } else {
        return;
    }
    if( tok_1->base_reg == EMPTY && tok_2->scale == 1 ) {
        tok_1->base_reg = reg;
    } else {
        tok_1->idx_reg = reg;
    }
    tok_1->scale *= tok_2->scale;
}

static void MakeConst( expr_list *token )
{
    struct asm_sym      *sym;

    if( token->type != EXPR_ADDR ) return;
    if( token->label == EMPTY ) return;
    sym = AsmLookup( AsmBuffer[token->label]->string_ptr );
    if( sym == NULL ) return;
    if( sym->state != SYM_STRUCT_FIELD ) return;
    token->value += sym->offset;
    token->label = EMPTY;
    if( token->base_reg != EMPTY ) return;
    if( token->idx_reg != EMPTY ) return;
    if( token->override != EMPTY ) return;
    token->instr = EMPTY;
    token->type = EXPR_CONST;
    token->indirect = FALSE;
    token->explicit = FALSE;
    token->expr_type = FALSE;
}

static void TokenAssign( expr_list *t1, expr_list *t2 )
{
    *t1 = *t2;
    t2->base_reg = EMPTY;
    t2->idx_reg = EMPTY;
}

static int_8 calculate( expr_list *token_1,expr_list *token_2, uint_8 index )
/* Perform the operation between token_1 and token_2 */
{
    char                token;
    struct asm_sym      *sym;

    token = AsmBuffer[index]->token;

    token_1->string = NULL;

    switch( token ) {
        case T_POSITIVE:
            /*
             * The only format allowed is:
             *        + constant
             */

            MakeConst( token_2 );
            if( token_2->type != EXPR_CONST ) {
                AsmError( POSITIVE_SIGN_CONSTANT_EXPECTED );
            }
            token_1->type = EXPR_CONST;
            token_1->value = token_2->value;
            break;
        case T_NEGATIVE:
            /*
             * The only format allowed is:
             *        - constant
             */

            MakeConst( token_2 );
            if( token_2->type != EXPR_CONST ) {
                AsmError( NEGATIVE_SIGN_CONSTANT_EXPECTED );
            }
            token_1->type = EXPR_CONST;
            token_1->value = -token_2->value;
            break;
        case '+':
            /*
             * The only formats allowed are:
             *        constant + constant
             *        constant + address
             *         address + register       ( only inside [] )
             *        register + register       ( only inside [] )
             *        register + constant       ( only inside [] )
             *        address  + address        ( only inside [] )
             */

            if( check_same( token_1, token_2, EXPR_CONST ) ) {

                token_1->value += token_2->value;

            } else if( check_same( token_1, token_2, EXPR_ADDR ) ) {

                index_connect( token_1, token_2 );
                if( token_2->label != EMPTY ) {
                    MakeConst( token_1 );
                }
                if( token_1->label == EMPTY ) {
                    token_1->label = token_2->label;
                } else if( token_2->label != EMPTY && Parse_pass > PASS_1 ) {
                    AsmError( LABEL_NOT_DEFINED );
                }
                token_1->indirect |= token_2->indirect;

            } else if( check_both( token_1, token_2, EXPR_CONST, EXPR_ADDR ) ) {

                if( token_1->type == EXPR_CONST ) {
                    token_2->value += token_1->value;
                    token_2->indirect |= token_1->indirect;
                    TokenAssign( token_1, token_2 );
                } else {
                    token_1->value += token_2->value;
                }

            } else if( check_both( token_1, token_2, EXPR_ADDR, EXPR_REG ) ) {

                    if( token_1->type == EXPR_REG ) {
                        if( token_2->instr != EMPTY ) {
                            AsmError( LABEL_IS_EXPECTED );
                            return( ERROR );
                        }
                        index_connect( token_2, token_1 );
                        token_2->indirect |= token_1->indirect;
                        TokenAssign( token_1, token_2 );
                    } else {
                        index_connect( token_1, token_2 );
                        token_1->indirect |= token_2->indirect;
                    }

            } else if( check_same( token_1, token_2, EXPR_REG ) ) {

                    index_connect( token_1, token_2 );
                    token_1->indirect |= token_2->indirect;
                    token_1->type = EXPR_ADDR;

            } else if( check_both( token_1, token_2, EXPR_CONST, EXPR_REG ) ) {

                    if( token_2->type == EXPR_REG ) {
                        token_1->base_reg = token_2->base_reg;
                        token_1->idx_reg = token_2->idx_reg;
                        token_2->base_reg = EMPTY;
                        token_2->idx_reg = EMPTY;
                    }
                    token_1->value += token_2->value;
                    token_1->indirect |= token_2->indirect;
                    token_1->type = EXPR_ADDR;
            } else {
                    /* Error */
                    AsmError( ADDITION_CONSTANT_EXPECTED );
                    return( ERROR );
            }
            break;
        case '-':
            /*
             * The only formats allowed are:
             *        constant - constant
             *         address - constant       ( only in this order )
             *         address - address
             *        register - constant       ( only inside [] and in this
             *                                    order )
             */

            MakeConst( token_2 );
            if( check_same( token_1, token_2, EXPR_CONST ) ) {

                token_1->value -= token_2->value;

            } else if( token_1->type == EXPR_ADDR &&
                       token_2->type == EXPR_CONST ) {

                token_1->value -= token_2->value;

            } else if( check_same( token_1, token_2, EXPR_ADDR ) ){

                if( token_2->label == EMPTY ) {
                    token_1->value -= token_2->value;
                    token_1->base_reg = token_2->base_reg;
                    token_1->idx_reg = token_2->idx_reg;
                    token_1->scale = token_2->scale;
                    token_1->indirect |= token_2->indirect;
                } else {
                    if( token_1->label == EMPTY ) {
                        AsmError( SYNTAX_ERROR );
                        return( ERROR );
                    }
                    sym = AsmLookup( AsmBuffer[token_1->label]->string_ptr );
                    if( sym == NULL ) return( ERROR );

                    if( Parse_Pass > PASS_1 && sym->state == SYM_UNDEFINED ) {
                        AsmError( LABEL_NOT_DEFINED );
                        return( ERROR );
                    }
                    token_1->value += sym->offset;
                    sym = AsmLookup( AsmBuffer[token_2->label]->string_ptr );
                    if( sym == NULL ) return( ERROR );

                    if( Parse_Pass > PASS_1 && sym->state == SYM_UNDEFINED ) {
                        AsmError( LABEL_NOT_DEFINED );
                        return( ERROR );
                    }
                    token_1->value -= sym->offset;
                    token_1->value -= token_2->value;
                    token_1->label = EMPTY;
                    token_1->base_reg = token_2->base_reg;
                    token_1->idx_reg = token_2->idx_reg;
                    token_1->scale = token_2->scale;
                    if( token_1->base_reg == EMPTY && token_1->idx_reg == EMPTY ) {
                        token_1->type = EXPR_CONST;
                        token_1->indirect = FALSE;
                    } else {
                        token_1->type = EXPR_ADDR;
                        token_1->indirect |= token_2->indirect;
                    }
                    token_1->explicit = FALSE;
                    token_1->expr_type = EMPTY;
                }

            } else if( token_1->type == EXPR_REG &&
                        token_2->type == EXPR_CONST ) {

                token_1->value = -1 * token_2->value;
                token_1->indirect |= token_2->indirect;
                token_1->type = EXPR_ADDR;

            } else {
                /* Error */
                AsmError( SUBTRACTION_CONSTANT_EXPECTED );
                return( ERROR );
            }
            break;
        case '*':
            /*
             * The only formats allowed are:
             *        constant * constant
             *        register * scaling factor ( 1, 2, 4 or 8 )
             *                   386 only
             */
            MakeConst( token_1 );
            MakeConst( token_2 );
            if( check_same( token_1, token_2, EXPR_CONST ) ) {
                token_1->value *= token_2->value;
            } else if( check_both( token_1, token_2, EXPR_REG, EXPR_CONST ) ) {
                /* scaling factor */
                if( token_2->type == EXPR_REG ) {
                    /* scale * reg */
                    token_1->idx_reg = token_2->base_reg;
                    token_1->base_reg = EMPTY;
                    token_1->scale = token_1->value;
                    token_1->value = 0;
                    token_2->base_reg = EMPTY;
                } else {
                    /* reg * scale */
                    token_1->idx_reg = token_1->base_reg;
                    token_1->base_reg = EMPTY;
                    token_1->scale = token_2->value;
                }
                token_1->indirect |= token_2->indirect;
                token_1->type = EXPR_ADDR;
            } else {
                AsmError( MULTIPLICATION_CONSTANT_EXPECTED );
                return( ERROR );
            }
            break;
        case '/':
            /*
             * The only formats allowed are:
             *        constant / constant
             */
            MakeConst( token_1 );
            MakeConst( token_2 );
            if( check_same( token_1, token_2, EXPR_CONST ) ) {
                token_1->value /= token_2->value;
            } else {
                AsmError( DIVISION_CONSTANT_EXPECTED );
                return( ERROR );
            }
            break;
        case T_COLON:
            /*
             * The only formats allowed are:
             *        register : anything ----- segment override
             *           label : address ( label = address with no offset
             *                             and no instruction attached;
             *                             also only segment or group is
             *                             allowed. )
             */
            if( token_2->override != EMPTY ) {
                 /* Error */
                 AsmError( MORE_THAN_ONE_OVERRIDE );
                 return( ERROR );
            }

            if( token_1->type == EXPR_REG ) {

                 if( token_1->base_reg != EMPTY && token_1->idx_reg != EMPTY ) {
                     AsmError( ILLEGAL_USE_OF_REGISTER );
                     return( ERROR );
                 }
                 token_2->override = token_1->base_reg;
                 token_2->indirect |= token_1->indirect;
                 token_2->type = EXPR_ADDR;
                 TokenAssign( token_1, token_2 );

            } else if( token_2->type == EXPR_ADDR &&
                        token_1->type == EXPR_ADDR &&
                        token_1->override == EMPTY &&
                        token_1->instr == EMPTY    &&
                        token_1->value == 0        &&
                        token_1->base_reg == EMPTY &&
                        token_1->idx_reg == EMPTY ) {

                sym = AsmLookup( AsmBuffer[token_1->label]->string_ptr );
                if( sym == NULL ) return( ERROR );

                if( AsmBuffer[token_1->label]->token == T_RES_ID ) {
                    /* Kludge for "FLAT" */
                    AsmBuffer[token_1->label]->token = T_ID;
                }

                if( sym->state == SYM_GRP || sym->state == SYM_SEG ) {
                    token_2->override = token_1->label;
                    token_2->indirect |= token_1->indirect;
                    TokenAssign( token_1, token_2 );
                } else {
                    AsmError( ONLY_SEG_OR_GROUP_ALLOWED );
                    return( ERROR );
                }
            } else {
                 AsmError( REG_OR_LABEL_EXPECTED_IN_OVERRIDE );
                 return( ERROR );
            }
            break;
        case T_INS:
            MakeConst( token_1 );
            MakeConst( token_2 );
            if( AsmBuffer[index]->value == T_NOT ) {
                if( token_2->type != EXPR_CONST ) {
                    AsmError( CONSTANT_EXPECTED );
                    return( ERROR );
                }
                token_1->type = EXPR_CONST;
            } else {
                if( !check_same( token_1, token_2, EXPR_CONST ) ) {
                    AsmError( CONSTANT_EXPECTED );
                    return( ERROR );
                }
            }
            switch( AsmBuffer[index]->value ) {
                case T_EQ:
                    token_1->value = ( token_1->value == token_2->value ? -1:0 );
                    break;
                case T_NE:
                    token_1->value = ( token_1->value != token_2->value ? -1:0 );
                    break;
                case T_LT:
                    token_1->value = ( token_1->value < token_2->value ? -1:0 );
                    break;
                case T_LE:
                    token_1->value = ( token_1->value <= token_2->value ? -1:0 );
                    break;
                case T_GT:
                    token_1->value = ( token_1->value > token_2->value ? -1:0 );
                    break;
                case T_GE:
                    token_1->value = ( token_1->value >= token_2->value ? -1:0 );
                    break;
                case T_MOD:
                    token_1->value %= token_2->value;
                    break;
                case T_SHL:
                    token_1->value = token_1->value << token_2->value;
                    break;
                case T_SHR:
                    token_1->value = token_1->value >> token_2->value;
                    break;
                case T_NOT:
                    token_1->value = ~(token_2->value);
                    break;
                case T_AND:
                    token_1->value &= token_2->value;
                    break;
                case T_OR:
                    token_1->value |= token_2->value;
                    break;
                case T_XOR:
                    token_1->value ^= token_2->value;
                    break;
            }
            break;
        case T_UNARY_OPERATOR:
            if( token_2->type != EXPR_ADDR ) {
                AsmError( LABEL_IS_EXPECTED );
                return( ERROR );
            } else if( token_2->instr != EMPTY ) {
                AsmError( LABEL_IS_EXPECTED );
                return( ERROR );
            }
            switch( AsmBuffer[index]->value ) {
            case T_LENGTH:
            case T_SIZE:
            case T_LENGTHOF:
            case T_SIZEOF:
                sym = AsmLookup( AsmBuffer[token_2->label]->string_ptr );
                if( sym == NULL ) return( ERROR );
                switch( AsmBuffer[index]->value ) {
                case T_LENGTH:
                    token_1->value = sym->first_length;
                    break;
                case T_LENGTHOF:
                    token_1->value = sym->total_length;
                    break;
                case T_SIZE:
#if 0 /* this gives a screwy number for structure sizes */
                    token_1->value = sym->first_size;
                    break;
#endif
                case T_SIZEOF:
                    token_1->value = sym->total_size;
                    break;
                }
                token_1->label = EMPTY;
                token_1->base_reg = EMPTY;
                token_1->idx_reg = EMPTY;
                token_1->override = EMPTY;
                token_1->instr = EMPTY;
                token_1->type = EXPR_CONST;
                token_1->indirect = FALSE;
                token_1->explicit = FALSE;
                token_1->expr_type = EMPTY;
                break;
            default:
                TokenAssign( token_1, token_2 );
                token_1->instr = index;
                break;
            }
            break;
    }
    return( NOT_ERROR );
}

static expr_list *evaluate( int *i, int end )
{
    expr_list           *token_1 = NULL;
    expr_list           *token_2 = NULL;
    int_8               start_token_2;
    char                token_needed;
    int_8               queue[2];       // operator queue; store the index
                                        // of the operator token

    token_needed = FALSE;

    queue[0] = queue[1] = EMPTY;

    /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
    /* Look at first token, which may be an unary operator or an operand */
    /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

    if( cmp_token( *i, T_OP_BRACKET ) ) {
        (*i)++;
        if( *i > end ) {
            AsmError( OPERAND_EXPECTED );
            return( NULL );
        }

        token_1 = evaluate( i, end );
        if( token_1 == NULL ) {
            return( NULL );
        }
    } else if( is_unary( *i, TRUE ) ) {
        token_needed = TRUE;
        optr_enqueue( *i, queue );
    } else if( cmp_token( *i, T_OP_SQ_BRACKET ) ) {
        if( *i == 0 ) {
            return( NULL );
        }
        /**/myassert( !cmp_token( (*i)-1, T_CL_BRACKET ) );
        (*i)++;
        token_1 = evaluate( i, end );
        if( token_1 == NULL ) {
            return( NULL );
        }
        token_1->indirect = TRUE;
    } else {
        token_1 = get_operand( *i );
        if( token_1 == NULL ) {
            return( NULL );
        }
    }

    /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
    /* If an unary operator is not found, now read the operator */
    /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

    if( !token_needed ) {

        (*i)++;
        if( *i > end ) {
            /* no operator is found; return token_1 as result */
            return( token_1 );
        }

        /* Read the operator */
        if( cmp_token( *i, T_CL_BRACKET ) ||
            cmp_token( *i, T_CL_SQ_BRACKET ) ) {
            return( token_1 );
        } else if( cmp_token( *i, T_OP_SQ_BRACKET ) ) {
            AsmBuffer[*i]->token = '+';
        } else if( !is_optr(*i) ) {
            AsmError( OPERATOR_EXPECTED );
            expr_free( token_1 );
            return( NULL );
        }
        token_needed = TRUE;
        optr_enqueue( *i, queue );
    }

    while( token_needed ) {

        /*:::::::::::::::::::::::::::*/
        /* Now read the next operand */
        /*:::::::::::::::::::::::::::*/

        (*i)++;
        start_token_2 = *i;
        if( *i > end ) {
            AsmError( OPERAND_EXPECTED );
            expr_free( token_1 );
            return( NULL );
        }

        if( cmp_token( *i, T_OP_BRACKET ) ||
            cmp_token( *i, T_OP_SQ_BRACKET ) ) {
            (*i)++;
            token_2 = evaluate( i, end );
            if( token_2 == NULL ) {
                expr_free( token_1 );
                return( NULL );
            }
        } else if( is_unary( *i, TRUE ) ) {
            token_2 = evaluate( i, end );
            if( token_2 == NULL ) {
                expr_free( token_1 );
                return( NULL );
            }
        } else if( is_optr( *i ) ) {
            AsmError( OPERAND_EXPECTED );
            expr_free( token_1 );
            return( NULL );
        } else {
            token_2 = get_operand( *i );
            if( token_2 == NULL ) {
                expr_free( token_1 );
                return( NULL );
            }
        }

        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
        /* Look at the next operator and compare its priority with 1st one */
        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

        (*i)++;
        if( *i <= end ) {
            if( cmp_token( *i, T_OP_SQ_BRACKET ) ) {
                AsmBuffer[*i]->token = '+';
            } else while( cmp_token( *i, T_CL_SQ_BRACKET ) ) {
                if( *i == end ) break;
                (*i)++;
            }
        }

        if( *i <= end ) {
            if( !is_optr( *i ) || is_unary( *i, FALSE ) ||
                cmp_token( *i, T_OP_BRACKET ) ) {
                AsmError( OPERATOR_EXPECTED );
                expr_free( token_1 );
                expr_free( token_2 );
                return( NULL );
            } else if( !cmp_token( *i, T_CL_BRACKET ) &&
                       !cmp_token( *i, T_CL_SQ_BRACKET ) ) {

                if( cmp_token( *i, T_OP_SQ_BRACKET ) ) {
                    AsmBuffer[*i]->token = '+';
                }
                if( get_precedence( *i ) < get_precedence( queue[0] ) ) {
                    (*i) = start_token_2;
                    expr_free( token_2 );
                    token_2 = evaluate( i, end );
                    if( token_2 == NULL ) {
                        expr_free( token_1 );
                        return( NULL );
                    }
                } else {
                    optr_enqueue( *i, queue );
                }
            }
        }

        /*::::::::::::::*/
        /* Now evaluate */
        /*::::::::::::::*/

        if( token_1 == NULL ) {
            /* the operator is unary */
            token_1 = create_expr();
        }

        if( calculate( token_1, token_2, queue[0] )==ERROR ) {
            expr_free( token_1 );
            expr_free( token_2 );
            return( NULL );
        }

        expr_free( token_2 );
        optr_dequeue( queue );
        if( queue[0] == EMPTY ) {
            token_needed = FALSE;
        }
    }
    return( token_1 );
}

static int is_expr( int i )
/* Check if the token is part of an expression */
{
    switch( AsmBuffer[i]->token ) {
        case T_INS:
        case T_RES_ID:
            switch( AsmBuffer[i]->value ) {
                case T_FLAT:
                    DefFlatGroup();
                    /* fall through */
                case T_EQ:
                case T_NE:
                case T_LT:
                case T_LE:
                case T_GT:
                case T_GE:
                case T_MOD:
                case T_NUM:
                    return( TRUE );
                case T_SHL:
                case T_SHR:
                case T_NOT:
                case T_AND:
                case T_OR:
                case T_XOR:
                    if( i == 0 ) {
                        /* It is an instruction instead */
                        return( FALSE );
                    } else if( AsmBuffer[i-1]->token == T_COLON ) {
                        /* It is an instruction instead */
                        return( FALSE );
                    } else if( AsmBuffer[i-1]->value == T_LOCK ) {
                        /* It is an instruction:
                                 lock and dword ptr [ebx], 1
                        */
                        return( FALSE );
                    } else {
                        return( TRUE );
                    }
                default:
                    return( FALSE );
            }
        case T_UNARY_OPERATOR:
//              case T_SEG2:
//              case T_OFFSET:
            if( i+1 < TokCnt ) {
                return( TRUE );
            }
            return( FALSE );
        case T_REG:
            return( AsmBuffer[i]->value != T_ST );
        case '+':
        case '-':
            /* hack to stop asmeval from hanging on floating point numbers */
            if( AsmBuffer[i+1]->token == T_FLOAT ) return( FALSE );
        case '*':
        case '/':
        case T_NUM:
        case T_OP_BRACKET:
        case T_CL_BRACKET:
        case T_OP_SQ_BRACKET:
        case T_CL_SQ_BRACKET:
            return(  TRUE );
        case T_COLON:
            if( i == 1 || ( AsmBuffer[i+1]->token == T_DIRECTIVE &&
                            AsmBuffer[i+1]->value == T_EQU2 ) ) {
                /* It is the colon following the label or it is a := */
                return( FALSE );
            } else {
                return( TRUE );
            }
        case T_PATH:
            return( FALSE );
        case T_ID:
            if( i == 0 ) {
                /* It is a label */
                return( FALSE );
            } else {
                return( TRUE );
            }
        case T_STRING:
            return( TRUE );
        default:
            return( FALSE );
    }
}

static int fix_parant( void )
/* Take out those brackets which may surround a non-expression, e.g.
   Right now only 'dup' requires a pair of parantheses, which should be
   taken out temporarily */
{
    int         i;
    int         store;
    int         dup_count = 0;
    int         non_dup_bracket_count = 0;

    store = 0;
    for( i = 0; i < TokCnt; i++ ) {

        if( AsmBuffer[i]->token == T_RES_ID && AsmBuffer[i]->value == T_DUP ) {
            dup_count++;
            if( AsmBuffer[++i]->token != T_OP_BRACKET ) {
                AsmError( BRACKET_EXPECTED );
                return( ERROR );
            }
            TakeOut[store++] = i;               // Store the location
            AsmBuffer[i]->token = T_NOOP;

        } else if( AsmBuffer[i]->token == T_OP_BRACKET ) {
            non_dup_bracket_count++;
        } else if( AsmBuffer[i]->token == T_CL_BRACKET ) {
            if( non_dup_bracket_count ) {
                non_dup_bracket_count--;
            } else {
                dup_count--;
                TakeOut[store++] = -i;  // store close brackets as -ve values
                AsmBuffer[i]->token = T_NOOP;
            }
        }
    }
    if( dup_count != 0 ) {
        AsmError( BRACKETS_NOT_BALANCED );
        return( ERROR );
    }
    TakeOut[store] = -1;        // Mark the end
    return( NOT_ERROR );
}

static int fix( expr_list *res, int start, int end )
/* Convert the result in res into tokens and put them back in AsmBuffer[] */
{
    int                 size = 0;
    int                 diff;
    int                 i;
    int                 old_start;
    int                 old_end;
    int                 need_number;
    unsigned            num;

    if( res->type == EXPR_CONST ) {
        if( res->override != EMPTY ) {
            *(AsmBuffer[start++]) = *(AsmBuffer[res->override]);
            AsmBuffer[start++]->token = T_COLON;
        }

        if( res->string == NULL ) {
            AsmBuffer[ start ]->token = T_NUM;
            AsmBuffer[ start++ ]->value = res->value;
        } else {
            AsmBuffer[ start ]->token = T_STRING;
            AsmBuffer[ start++ ]->string_ptr = res->string;
        }

    } else if( res->type == EXPR_REG && !res->indirect ) {

        if( res->override != EMPTY ) {
            *(AsmBuffer[start++]) = *(AsmBuffer[res->override]);
            AsmBuffer[start++]->token = T_COLON;
            AsmBuffer[ start++ ]->token = T_OP_SQ_BRACKET;
        }
        AsmBuffer[ start ]->token = T_REG;
        AsmBuffer[ start++ ]->value = AsmBuffer[ res->base_reg ]->value;
        if( res->override != EMPTY ) {
            AsmBuffer[ start++ ]->token = T_CL_SQ_BRACKET;
        }

    } else {

        if( res->type != EXPR_REG ) {
            size++;
        }

        if( res->instr != EMPTY ) {
            size++;
        }
        if( res->override != EMPTY ) {
            size += 2;
        }
        need_number = 1;
        if( res->scale != 1 ) {
            size += 2;          // [ reg * 2 ] == 2 tokens more than [ reg ]
        }
        if( res->base_reg != EMPTY ) {
            size += 3;                  // e.g. [ ax ] == 3 tokens
            need_number = 0;
        }
        if( res->idx_reg != EMPTY ) {
            size += 3;                  // e.g. [ ax ] == 3 tokens
            need_number = 0;
        }
        if( res->label != EMPTY ) {
            need_number = 0;
        }
        if( res->value != 0 ) {
            need_number = 1;
        }
        if( need_number ) {
            size += 3;                  // [ value ] == 3 tokens
        }

        old_start = start;
        old_end = end;

        num = end - start + 1;
        if( num > StoreNum ) {
            AsmFree( Store );
            Store = AsmAlloc( num * sizeof( Store[0] ) );
        }

        for( i = start; i <= end; i++ ) {
            /* Store the original AsmBuffer[] data */
            Store[i-start] = *(AsmBuffer[i]);
        }

        diff = size - ( end - start + 1 );

        if( diff > 0 ) {

            for( i = TokCnt - 1; i > end; i-- ) {
                *(AsmBuffer[i+diff]) = *(AsmBuffer[i]);
            }

            for( i = 0; TakeOut[i] != -1; i++ ) {
                if( TakeOut[i] > end ) {
                    (TakeOut[i]) += diff;
                }
            }

            end += diff;
            TokCnt += diff;
        }

        if( res->instr != EMPTY ) {
            AsmBuffer[start]->token = T_UNARY_OPERATOR;
            AsmBuffer[start++]->value = Store[res->instr-old_start].value;
        }

        if( res->override != EMPTY ) {
            *(AsmBuffer[start++]) = Store[res->override-old_start];
            AsmBuffer[start++]->token = T_COLON;
        }

        if( res->label != EMPTY && res->type != EXPR_REG ) {
            *(AsmBuffer[start++]) = Store[res->label-old_start];
        }

        if( res->base_reg != EMPTY ) {
            AsmBuffer[start++]->token = T_OP_SQ_BRACKET;
            AsmBuffer[start]->token = T_REG;
            AsmBuffer[start++]->value = Store[res->base_reg-old_start].value;
            AsmBuffer[start++]->token = T_CL_SQ_BRACKET;
        }
        if( res->idx_reg != EMPTY ) {
            AsmBuffer[start++]->token = T_OP_SQ_BRACKET;
            AsmBuffer[start]->token = T_REG;
            AsmBuffer[start++]->value = Store[res->idx_reg-old_start].value;
            if( res->scale != 1 ) {
                AsmBuffer[start++]->token = T_TIMES;
                AsmBuffer[start]->token = T_NUM;
                AsmBuffer[start++]->value = res->scale;
                res->scale = 1;
            }
            AsmBuffer[start++]->token = T_CL_SQ_BRACKET;
        }

        if( need_number ) {
            AsmBuffer[start++]->token = T_OP_SQ_BRACKET;
            AsmBuffer[start]->token = T_NUM;
            AsmBuffer[start++]->value = res->value;
            AsmBuffer[start++]->token = T_CL_SQ_BRACKET;
        }

        /**/myassert( ( start-1 ) <= end );
    }

    for( i = start; i <= end; i++ ) {
        AsmBuffer[i]->token = T_NOOP;
    }
    return( end );
}

static void fix_final( void )
/* Put back those brackets taken out by fix_parant() and take out all T_NOOP
   tokens */
{
    int         start;
    int         end;
    int         i;
    int         diff;
    int         dup_count = 0;

    for( i = 0;; i++ ) {
        if( dup_count == 0 && TakeOut[i] == -1 ) break;
        if( TakeOut[i] > 0 ) {
            AsmBuffer[ TakeOut[i] ]->token = T_OP_BRACKET;
            dup_count++;
        } else {
            AsmBuffer[ -( TakeOut[i] ) ]->token = T_CL_BRACKET;
            dup_count--;
        }
        myassert( i < MAX_TOKEN );
    }

    for( start = 0; start < TokCnt; start++ ) {
        if( AsmBuffer[start]->token == T_NOOP ) {
            for( end = start + 1;; end++ ) {
                if( AsmBuffer[end]->token != T_NOOP ||
                    end == TokCnt ) break;
            }
            if( end == TokCnt ) {
                TokCnt = start;
                return;
            } else {
                diff = end - start;
                /**/myassert( diff >= 0 );
                for( i = end; i < TokCnt; i++ ) {
                    *(AsmBuffer[i-diff]) = *(AsmBuffer[i]);
                }
            }
            TokCnt -= diff;
        }
    }
}

extern int EvalExpr( int count, int start_tok, int end_tok )
/******************************/
{
    int         i = start_tok;
    int         start;          // position of first token of an expression
    int         num;            // number of tokens in the expression
    bool        final = FALSE;
    expr_list   *result;

    if( AsmBuffer[end_tok]->token == T_FINAL ) final = TRUE;

    TokCnt = count;

    if( fix_parant() == ERROR ) {
        // take out those parantheses which are not part of an expression
        return( ERROR );
    }

    while( i < TokCnt && i <= end_tok ) {
        if( is_expr(i) ) {
            start = i++;
            num = 0;
            for( ;; ) {
                if( i >= TokCnt ) break;
                if( i > end_tok ) break;
                if( !is_expr(i) ) break;
                i++;
                num++;
            }
            // Massive kludge ahead:
            // If the thing looks like "<reg>:", then skip it
            if( !(num == 1
             && AsmBuffer[start]->token == T_REG
             && AsmBuffer[start+1]->token == T_COLON) ) {
                i = start;
                result = evaluate( &i, i + num );
                if( result == NULL ) {
    //          AsmError( SYNTAX_ERROR );
                    return( ERROR );
                }
                i = fix( result, start, start + num );
                if( count != TokCnt ) {
                    /* we just changed the number of tokens, so update
                     * count & end_tok appropriately */
                    end_tok += TokCnt - count;
                    count = TokCnt;
                }
                expr_free( result );
            }
        }
        i++;
    }
    fix_final();

    if( AsmBuffer[TokCnt]->token == T_NOOP || final ) {
        AsmBuffer[ TokCnt ]->token = T_FINAL;
        AsmBuffer[ TokCnt ]->string_ptr = NULL;
    }

    return( TokCnt );
}

void AsmEvalInit()
{
    ExprListCache = NULL;
    Store = NULL;
    StoreNum = 0;
}

void AsmEvalFini()
{
    void        *next;

    AsmFree( Store );

    while( ExprListCache != NULL ) {
        next = ExprListCache->string;
        AsmFree( ExprListCache );
        ExprListCache = next;
    }
}
