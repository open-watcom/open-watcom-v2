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
* Description:  WASM expression evaluator.
*
****************************************************************************/


#include "asmglob.h"
#include "asmeval.h"

#if defined( _STANDALONE_ )
#include "directiv.h"
#include "myassert.h"
#else
//  FIXME!!
#define myassert(x)
#endif

extern void             DefFlatGroup( void );

static int              TakeOut[ MAX_TOKEN ];
static int              TokCnt;

static asm_tok          Store[ MAX_TOKEN ];

static int              op_sq_bracket_level;
static bool             error_msg;

enum process_flag {
    PROC_BRACKET,
    PROC_OPERAND
};

static int evaluate( expr_list *, int *, int, enum process_flag, bool (*)(int) );

static void init_expr( expr_list *new )
/*************************************/
{
    new->empty    = TRUE;
    new->type     = EMPTY;
    new->label    = EMPTY;
    new->override = EMPTY;
    new->instr    = EMPTY;
    new->base_reg = EMPTY;
    new->idx_reg  = EMPTY;
    new->indirect = FALSE;
    new->explicit = FALSE;
    new->abs      = FALSE;
    new->mem_type = MT_EMPTY;
    new->value    = 0;
    new->scale    = 1;
    new->string   = NULL;
    new->sym      = NULL;
    new->mbr      = NULL;
}

static void TokenAssign( expr_list *t1, expr_list *t2 )
/*****************************************************/
{
    t1->empty    = t2->empty;
    t1->type     = t2->type;
    t1->label    = t2->label;
    t1->override = t2->override;
    t1->instr    = t2->instr;
    t1->base_reg = t2->base_reg;
    t1->idx_reg  = t2->idx_reg;
    t1->indirect = t2->indirect;
    t1->explicit = t2->explicit;
    t1->abs      = t2->abs;
    t1->mem_type = t2->mem_type;
    t1->value    = t2->value;
    t1->scale    = t2->scale;
    t1->string   = t2->string;
    t1->sym      = t2->sym;
    t1->mbr      = t2->mbr;
}

#define PLUS_PRECEDENCE 9

static int get_precedence( int i )
/********************************/
{
    /* The following table is taken verbatim from MASM 6.1 Programmer's Guide,
     * page 14, Table 1.3. Sadly, it flatly contradicts QuickHelp online
     * documentation shipped with said product and should not be taken as gospel.
     */

//    1             (), []
//    2             LENGTH, SIZE, WIDTH, MASK, LENGTHOF, SIZEOF
//    3             . (structure-field-name operator)
//    4             : (segment override operator), PTR
//    5             LROFFSET, OFFSET, SEG, THIS, TYPE
//    6             HIGH, HIGHWORD, LOW, LOWWORD
//    7             +, - (unary)
//    8             *, /, MOD, SHL, SHR
//    9             +, - (binary)
//    10            EQ, NE, LT, LE, GT, GE
//    11            NOT
//    12            AND
//    13            OR, XOR
//    14            OPATTR, SHORT, .TYPE

    /* The following table appears in QuickHelp online documentation for
     * both MASM 6.0 and 6.1. Typical Microsoft mess.
     */

//    1             LENGTH, SIZE, WIDTH, MASK
//    2             (), []
//    3             . (structure-field-name operator)
//    4             : (segment override operator), PTR
//    5             THIS, OFFSET, SEG, TYPE
//    6             HIGH, LOW
//    7             +, - (unary)
//    8             *, /, MOD, SHL, SHR
//    9             +, - (binary)
//    10            EQ, NE, LT, LE, GT, GE
//    11            NOT
//    12            AND
//    13            OR, XOR
//    14            SHORT, OPATTR, .TYPE, ADDR

    switch( AsmBuffer[i]->class ) {
    case TC_UNARY_OPERATOR:
        switch( AsmBuffer[i]->u.token ) {
#if defined( _STANDALONE_ )
        case T_LENGTH:
        case T_SIZE:
        case T_LENGTHOF:
        case T_SIZEOF:
            return( 3 );
#endif
        case T_SEG:
        case T_OFFSET:
            return( 5 );
        }
        break;
    case TC_ARITH_OPERATOR:
        return( 8 );
#if defined( _STANDALONE_ )
    case TC_RELATION_OPERATOR:
        return( 10 );
#endif
    case TC_INSTR:
        switch( AsmBuffer[i]->u.token ) {
        case T_SHL:
        case T_SHR:
            return( 8 );
        case T_NOT:
            return( 11 );
        case T_AND:
            return( 12 );
        case T_OR:
        case T_XOR:
            return( 13 );
        }
        break;
    case TC_RES_ID:
        switch( AsmBuffer[i]->u.token ) {
        case T_SHORT:
            return( 14 );
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_FWORD:
        case T_QWORD:
        case T_TBYTE:
        case T_OWORD:
        case T_NEAR:
        case T_FAR:
#if defined( _STANDALONE_ )
        case T_SBYTE:
        case T_SWORD:
        case T_SDWORD:
#endif
        case T_PTR:
            return( 5 );
        }
        break;
    case TC_COLON:
        return( 4 );
    case TC_POSITIVE:
    case TC_NEGATIVE:
        return( 7 );
    case TC_PLUS:
    case TC_MINUS:
        return( 9 );
    case TC_DOT:
        return( 2 );
    default:
        /**/myassert( 0 );
        break;
    }
    return( ERROR );
}

static int get_operand( expr_list *new, int *start, int end, bool (*is_expr)(int) )
/*********************************************************************************/
{
    char        *tmp;
    int         i = *start;

    init_expr( new );
    switch( AsmBuffer[i]->class ) {
    case TC_NUM:
        new->empty = FALSE;
#if defined( _STANDALONE_ )
        if( (Options.mode & MODE_IDEAL) && ( op_sq_bracket_level ) ) {
            new->type = EXPR_ADDR;
            new->indirect = TRUE;
        } else {
            new->type = EXPR_CONST;
        }
#else
        new->type = EXPR_CONST;
#endif
        new->value = AsmBuffer[i]->u.value;
        break;
    case TC_STRING:
        new->empty = FALSE;
        new->type = EXPR_CONST;
        new->string = AsmBuffer[i]->string_ptr;
        new->value = 0;
        for( tmp = new->string; *tmp != '\0'; tmp++ ) {
            new->value <<= 8;
            new->value |= (*tmp);
        }
        break;
    case TC_REG:
        new->empty = FALSE;
        new->type = EXPR_REG;
        new->base_reg = i;
        if( op_sq_bracket_level > 0 ) {
            switch( AsmBuffer[i]->u.token ) {
            case T_EAX:
            case T_EBX:
            case T_ECX:
            case T_EDX:
            case T_EDI:
            case T_ESI:
            case T_EBP:
            case T_ESP:
            case T_BX:
            case T_BP:
            case T_DI:
            case T_SI:
                new->indirect = TRUE;
                break;
            case T_DS:
            case T_CS:
            case T_ES:
            case T_SS:
            case T_FS:
            case T_GS:
                if( AsmBuffer[i+1]->class != TC_COLON ) {
                    if( error_msg )
                        AsmError( ILLEGAL_USE_OF_REGISTER );
                    new->type = EXPR_UNDEF;
                    return( ERROR );
                }
                break;
            default:
                if( error_msg )
                    AsmError( ILLEGAL_USE_OF_REGISTER );
                new->type = EXPR_UNDEF;
                return( ERROR );
            }
        } else if( AsmBuffer[i]->u.token == T_ST ) {

            expr_list   sti;

            // read st(i), put i into idx_reg
            i++;
            switch( AsmBuffer[i]->class ) {
            case TC_OP_BRACKET:
            case TC_OP_SQ_BRACKET:
            case TC_NUM:
                *start = i;
                init_expr( &sti );
                if( evaluate( &sti, start, end, PROC_OPERAND, is_expr ) == ERROR ) {
                    new->type = EXPR_UNDEF;
                    return( ERROR );
                }
                if( sti.type != EXPR_CONST ) {
                    if( error_msg )
                        AsmError( CONSTANT_EXPECTED );
                    new->type = EXPR_UNDEF;
                    return( ERROR );
                }
                new->idx_reg = sti.value;
                return( NOT_ERROR );
            default:
                new->idx_reg = 0; // st = st(0)
                break;
            }
        }
        break;
    case TC_ID:
#if defined( _STANDALONE_ )
        if( Parse_Pass == PASS_1 ) {
            new->sym = AsmLookup( AsmBuffer[i]->string_ptr );
            if( new->sym == NULL ) {
                new->type = EXPR_UNDEF;
                return( ERROR );
            }
        } else {
            new->sym = AsmGetSymbol( AsmBuffer[i]->string_ptr );
            if( new->sym == NULL ) {
                if( error_msg )
                    AsmErr( SYMBOL_NOT_DEFINED, AsmBuffer[i]->string_ptr );
                new->type = EXPR_UNDEF;
                return( ERROR );
            }
#if 0
// FIXME !!!!!
// problem with aliases and equ directive
            if( ( new->sym == NULL ) || ( new->sym->state == SYM_UNDEFINED ) ) {
                if( error_msg )
                    AsmErr( SYMBOL_NOT_DEFINED, AsmBuffer[i]->string_ptr );
                new->type = EXPR_UNDEF;
                return( ERROR );
            }
#endif
        }
        if( new->sym != NULL ) {
            new->sym->referenced = TRUE;
            if( ( new->sym->state == SYM_STRUCT  ) ||
                (Options.mode & MODE_IDEAL) && ( new->sym->mem_type == MT_STRUCT ) ) {
                new->empty = FALSE;
                new->value = new->sym->offset;
                new->mbr = new->sym;
                new->sym = NULL;
                new->type = EXPR_ADDR;
                if( (Options.mode & MODE_IDEAL) && ( op_sq_bracket_level ) ) {
                    Definition.struct_depth++;
                    if( new->mbr->state == SYM_STRUCT ) {
                        Definition.curr_struct = (dir_node *)new->mbr;
                        (*start)++; /* Skip structure override and process next token */
                        return( get_operand( new, start, end, is_expr ) );
                    } else {
                        new->indirect = TRUE;
                        Definition.curr_struct = (dir_node *)new->mbr->structure;
                    }
                }
                break;
            } else if( new->sym->state == SYM_STRUCT_FIELD ) {
                if( (Options.mode & MODE_IDEAL) && ( Definition.struct_depth ) ) {
                    Definition.struct_depth--;
                    new->indirect = TRUE;
                }
                new->empty = FALSE;
                new->mem_type = new->sym->mem_type;
                new->value = new->sym->offset;
                new->mbr = new->sym;
                new->sym = NULL;
                new->type = EXPR_ADDR;
                break;
            }
            if( new->sym->mem_type == MT_ABS ) {
                new->abs = TRUE;
            } else {
                new->mem_type = new->sym->mem_type;
            }
        }
#else
        new->sym = AsmLookup( AsmBuffer[i]->string_ptr );
        new->mem_type = new->sym->mem_type;
#endif
        new->empty = FALSE;
        new->type = EXPR_ADDR;
        new->label = i;
        break;
    case TC_RES_ID:
        new->sym = AsmLookup( AsmBuffer[i]->string_ptr );
        new->empty = FALSE;
        new->type = EXPR_ADDR;
        new->label = i;
        break;
    default:
        new->type = EXPR_UNDEF;
        return( ERROR );
    }
    (*start)++;
    return( NOT_ERROR );
}

static bool is_optr( int i )
/**************************/
/* determine if it is an operator */
{
    switch( AsmBuffer[i]->class ) {
    case TC_REG:
    case TC_NUM:
    case TC_ID:
    case TC_RES_ID:
    case TC_STRING:
    case TC_PATH:
    case TC_OP_BRACKET:
        return( FALSE );
    }
    return( TRUE );
}

static bool is_unary( int i, char sign )
/**************************************/
/* determine if it is an unary operand */
{
    switch( AsmBuffer[i]->class ) {
    case TC_UNARY_OPERATOR:
        return( TRUE );
    case TC_INSTR:
        if( AsmBuffer[i]->u.token == T_NOT )
            return( TRUE );
        break;
    case TC_POSITIVE:
    case TC_NEGATIVE:
        return( TRUE );
    case TC_PLUS:
        if( sign ) {
            AsmBuffer[i]->class = TC_POSITIVE;
            return( TRUE );
        }
        break;
    case TC_MINUS:
        if( sign ) {
            AsmBuffer[i]->class = TC_NEGATIVE;
            return( TRUE );
        }
        break;
    case TC_RES_ID:
        switch( AsmBuffer[i]->u.token ) {
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_FWORD:
        case T_QWORD:
        case T_TBYTE:
        case T_OWORD:
        case T_SHORT:
        case T_NEAR:
        case T_FAR:
#if defined( _STANDALONE_ )
        case T_SBYTE:
        case T_SWORD:
        case T_SDWORD:
#endif
        case T_PTR:
            return( TRUE );
        }
        break;
    default:
        break;
    }
    return( FALSE );
}

static bool cmp_token_cls( int i, tok_class cls )
/***********************************************/
/* compare AsmBuffer[i] and tok */
{
    if( AsmBuffer[i]->class == cls ) {
        return( TRUE );
    } else {
        return( FALSE );
    }
}

static bool check_same( expr_list *tok_1, expr_list *tok_2, int_8 type )
/**********************************************************************/
/* Check if both tok_1 and tok_2 equal type */
{
    if( tok_1->type == type &&
        tok_2->type == type ) {
        return( TRUE );
    } else {
        return( FALSE );
    }
}

static bool check_both( expr_list *tok_1, expr_list *tok_2, int_8 type1, int_8 type2 )
/************************************************************************************/
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
/*************************************************************/
/* Connects the register lists */
{
    if( tok_1->base_reg == EMPTY ) {
        if( tok_2->base_reg != EMPTY ) {
            tok_1->base_reg = tok_2->base_reg;
            tok_2->base_reg = EMPTY;
        } else if( ( tok_2->idx_reg != EMPTY ) && ( tok_2->scale == 1 ) ) {
            tok_1->base_reg = tok_2->idx_reg;
            tok_2->idx_reg = EMPTY;
        }
    }
    if( tok_1->idx_reg == EMPTY ) {
        if( tok_2->idx_reg != EMPTY ) {
            tok_1->idx_reg = tok_2->idx_reg;
            tok_1->scale = tok_2->scale;
        } else if( tok_2->base_reg != EMPTY ) {
            tok_1->idx_reg = tok_2->base_reg;
            tok_1->scale = 1;
        }
    }
}

static void MakeConst( expr_list *token )
/***************************************/
{
    if( token->type != EXPR_ADDR )
        return;
    if( token->sym != NULL )
        return;
    token->label = EMPTY;
    if( token->mbr != NULL ) {
#if defined( _STANDALONE_ )
        if( token->mbr->state == SYM_STRUCT_FIELD ) {
        } else if( token->mbr->state == SYM_STRUCT ) {
            token->value += token->mbr->total_size;
            token->mbr = NULL;
        } else {
            return;
        }
#else
        return;
#endif
    }
    if( token->base_reg != EMPTY )
        return;
    if( token->idx_reg != EMPTY )
        return;
    if( token->override != EMPTY )
        return;
    token->instr = EMPTY;
    token->type = EXPR_CONST;
    token->indirect = FALSE;
    token->explicit = FALSE;
    token->mem_type = MT_EMPTY;
}

static void fix_struct_value( expr_list *token )
/**********************************************/
{
#if defined( _STANDALONE_ )
    if( token->mbr != NULL ) {
        if( token->mbr->state == SYM_STRUCT ) {
            token->value += token->mbr->total_size;
            token->mbr = NULL;
        }
    }
#endif
}

static int check_direct_reg( expr_list *token_1, expr_list *token_2 )
/*******************************************************************/
{
    if( ( token_1->type == EXPR_REG ) && ( token_1->indirect == FALSE )
        || ( token_2->type == EXPR_REG ) && ( token_2->indirect == FALSE ) ) {
        return( ERROR );
    } else {
        return( NOT_ERROR );
    }
}

static int calculate( expr_list *token_1, expr_list *token_2, uint_8 index )
/**************************************************************************/
/* Perform the operation between token_1 and token_2 */
{
    struct asm_sym      *sym;
    asm_token           reg_token;

    token_1->string = NULL;

    switch( AsmBuffer[index]->class ) {
    case TC_POSITIVE:
        /*
         * The only format allowed is:
         *        + constant
         */

        MakeConst( token_2 );
        if( token_2->type != EXPR_CONST ) {
            if( error_msg )
                AsmError( POSITIVE_SIGN_CONSTANT_EXPECTED );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
        }
        token_1->type = EXPR_CONST;
        token_1->value = token_2->value;
        break;
    case TC_NEGATIVE:
        /*
         * The only format allowed is:
         *        - constant
         */

        MakeConst( token_2 );
        if( token_2->type != EXPR_CONST ) {
            if( error_msg )
                AsmError( NEGATIVE_SIGN_CONSTANT_EXPECTED );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
        }
        token_1->type = EXPR_CONST;
        token_1->value = -token_2->value;
        break;
    case TC_PLUS:
        /*
         * The only formats allowed are:
         *        constant + constant
         *        constant + address
         *         address + register       ( only inside [] )
         *        register + register       ( only inside [] )
         *        register + constant       ( only inside [] )
         *        address  + address        ( only inside [] )
         */

        if( check_direct_reg( token_1, token_2 ) == ERROR ) {
            if( error_msg )
                AsmError( ILLEGAL_USE_OF_REGISTER );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
        }
        if( check_same( token_1, token_2, EXPR_CONST ) ) {

            token_1->value += token_2->value;

        } else if( check_same( token_1, token_2, EXPR_ADDR ) ) {

            fix_struct_value( token_1 );
            fix_struct_value( token_2 );
            index_connect( token_1, token_2 );
            token_1->indirect |= token_2->indirect;
            if( token_1->sym != NULL ) {
                if( token_2->sym != NULL ) {
                    if( error_msg )
                        AsmError( SYNTAX_ERROR );
                    token_1->type = EXPR_UNDEF;
                    return( ERROR );
                }
            } else if( token_2->sym != NULL ) {
                token_1->label = token_2->label;
                token_1->sym = token_2->sym;
            }
            token_1->value += token_2->value;

        } else if( check_both( token_1, token_2, EXPR_CONST, EXPR_ADDR ) ) {

            if( token_1->type == EXPR_CONST ) {
                token_2->value += token_1->value;
                token_2->indirect |= token_1->indirect;
                if( token_1->explicit ) {
                    token_2->explicit |= token_1->explicit;
                    token_2->mem_type = token_1->mem_type;
                }
                TokenAssign( token_1, token_2 );
            } else {
                token_1->value += token_2->value;
            }
            fix_struct_value( token_1 );

        } else if( check_both( token_1, token_2, EXPR_ADDR, EXPR_REG ) ) {

            if( token_1->type == EXPR_REG ) {
                if( token_2->instr != EMPTY ) {
                    if( error_msg )
                        AsmError( LABEL_IS_EXPECTED );
                    token_1->type = EXPR_UNDEF;
                    return( ERROR );
                }
                index_connect( token_2, token_1 );
                token_2->indirect |= token_1->indirect;
                TokenAssign( token_1, token_2 );
            } else {
                index_connect( token_1, token_2 );
                token_1->indirect |= token_2->indirect;
            }
            fix_struct_value( token_1 );

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
            if( error_msg )
                AsmError( ADDITION_CONSTANT_EXPECTED );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
        }
        break;
    case TC_DOT:
        /*
         * The only formats allowed are:
         *        register . address       ( only inside [] )
         *        address  . address
         *        address  . constant
         */

        if( check_direct_reg( token_1, token_2 ) == ERROR ) {
            if( error_msg )
                AsmError( ILLEGAL_USE_OF_REGISTER );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
        }
        if( check_same( token_1, token_2, EXPR_ADDR ) ) {

            index_connect( token_1, token_2 );
            token_1->indirect |= token_2->indirect;
            if( token_1->sym != NULL ) {
                if( token_2->sym != NULL ) {
                    if( error_msg )
                        AsmError( SYNTAX_ERROR );
                    token_1->type = EXPR_UNDEF;
                    return( ERROR );
                }
            } else if( token_2->sym != NULL ) {
                token_1->label = token_2->label;
                token_1->sym = token_2->sym;
            }
            if( token_2->mbr != NULL ) {
                token_1->mbr = token_2->mbr;
            }
            token_1->value += token_2->value;
            if( token_1->explicit == FALSE ) {
                token_1->mem_type = token_2->mem_type;
            }

        } else if( check_both( token_1, token_2, EXPR_CONST, EXPR_ADDR ) ) {

            if( token_1->type == EXPR_CONST ) {
                token_2->indirect |= token_1->indirect;
                token_2->value += token_1->value;
                TokenAssign( token_1, token_2 );
            } else {
                token_1->value += token_2->value;
            }

        } else if( check_both( token_1, token_2, EXPR_ADDR, EXPR_REG ) ) {

            if( token_1->type == EXPR_REG ) {
                if( token_2->instr != EMPTY ) {
                    if( error_msg )
                        AsmError( LABEL_IS_EXPECTED );
                    token_1->type = EXPR_UNDEF;
                    return( ERROR );
                }
                index_connect( token_2, token_1 );
                token_2->indirect |= token_1->indirect;
                TokenAssign( token_1, token_2 );
            } else {
                index_connect( token_1, token_2 );
                token_1->indirect |= token_2->indirect;
            }

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
            if( error_msg )
                AsmError( SYNTAX_ERROR );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
        }
        break;
    case TC_MINUS:
        /*
         * The only formats allowed are:
         *        constant - constant
         *         address - constant       ( only in this order )
         *         address - address
         *        register - constant       ( only inside [] and in this
         *                                    order )
         */

        if( check_direct_reg( token_1, token_2 ) == ERROR ) {
            if( error_msg )
                AsmError( ILLEGAL_USE_OF_REGISTER );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
        }
        MakeConst( token_2 );
        if( check_same( token_1, token_2, EXPR_CONST ) ) {

            token_1->value -= token_2->value;

        } else if( token_1->type == EXPR_ADDR &&
                   token_2->type == EXPR_CONST ) {

            token_1->value -= token_2->value;
            fix_struct_value( token_1 );

        } else if( check_same( token_1, token_2, EXPR_ADDR ) ){

            fix_struct_value( token_1 );
            fix_struct_value( token_2 );
            if( token_2->base_reg != EMPTY || token_2->idx_reg != EMPTY ) {
                if( error_msg )
                    AsmError( ILLEGAL_USE_OF_REGISTER );
                token_1->type = EXPR_UNDEF;
                return( ERROR );
            }
            if( token_2->label == EMPTY ) {
                token_1->value -= token_2->value;
                token_1->indirect |= token_2->indirect;
            } else {
                if( token_1->label == EMPTY ) {
                    if( error_msg )
                        AsmError( SYNTAX_ERROR );
                    token_1->type = EXPR_UNDEF;
                    return( ERROR );
                }
                sym = token_1->sym;
                if( sym == NULL )
                    return( ERROR );
#if defined( _STANDALONE_ )
                if( Parse_Pass > PASS_1 && sym->state == SYM_UNDEFINED ) {
                    if( error_msg )
                        AsmError( LABEL_NOT_DEFINED );
                    token_1->type = EXPR_UNDEF;
                    return( ERROR );
                }
                token_1->value += sym->offset;
#else
                token_1->value += sym->addr;
#endif
                sym = token_2->sym;
                if( sym == NULL )
                    return( ERROR );

#if defined( _STANDALONE_ )
                if( Parse_Pass > PASS_1 && sym->state == SYM_UNDEFINED ) {
                    if( error_msg )
                        AsmError( LABEL_NOT_DEFINED );
                    token_1->type = EXPR_UNDEF;
                    return( ERROR );
                }
                token_1->value -= sym->offset;
#else
                token_1->value -= sym->addr;
#endif
                token_1->value -= token_2->value;
                token_1->label = EMPTY;
                token_1->sym = NULL;
                if( token_1->base_reg == EMPTY && token_1->idx_reg == EMPTY ) {
                    token_1->type = EXPR_CONST;
                    token_1->indirect = FALSE;
                } else {
                    token_1->type = EXPR_ADDR;
                    token_1->indirect |= token_2->indirect;
                }
                token_1->explicit = FALSE;
                token_1->mem_type = MT_EMPTY;
            }

        } else if( token_1->type == EXPR_REG &&
                   token_2->type == EXPR_CONST ) {

            token_1->value = -token_2->value;
            token_1->indirect |= token_2->indirect;
            token_1->type = EXPR_ADDR;

        } else {
            /* Error */
            if( error_msg )
                AsmError( SUBTRACTION_CONSTANT_EXPECTED );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
        }
        break;
    case TC_COLON:
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
            if( error_msg )
                AsmError( MORE_THAN_ONE_OVERRIDE );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
        }

        if( token_1->type == EXPR_REG ) {

            if( token_1->base_reg != EMPTY && token_1->idx_reg != EMPTY ) {
                if( error_msg )
                    AsmError( ILLEGAL_USE_OF_REGISTER );
                token_1->type = EXPR_UNDEF;
                return( ERROR );
            }
            /* Quick fix for MOVS/CMPS/SCAS. These instruction do not allow
             * a segment override for the the destination. If ES is used for
             * the [E]DI operand, ignore it; actual overrides are rejected.
             */
            switch( Code->info.token ) {
            case T_MOVS:
            case T_CMPS:
            case T_SCAS:
                reg_token = AsmBuffer[token_2->base_reg]->u.token;
                if( (reg_token == T_DI) || (reg_token == T_EDI) ) {
                    if( AsmBuffer[token_1->base_reg]->u.token == T_ES ) {
                        token_1->base_reg = token_2->override;
                        break;
                    }
                    if( error_msg )
                        AsmError( ILLEGAL_USE_OF_REGISTER );
                    token_1->type = EXPR_UNDEF;
                    return( ERROR );
                }
                break;
            default:
                break;
            }
            token_2->override = token_1->base_reg;
            token_2->indirect |= token_1->indirect;
            token_2->type = EXPR_ADDR;
            if( token_1->explicit ) {
                token_2->explicit = token_1->explicit;
                token_2->mem_type = token_1->mem_type;
            }
            TokenAssign( token_1, token_2 );

        } else if( token_2->type == EXPR_ADDR
            && token_1->type == EXPR_ADDR
            && token_1->override == EMPTY
            && token_1->instr == EMPTY
            && token_1->value == 0
            && token_1->base_reg == EMPTY
            && token_1->idx_reg == EMPTY ) {

            sym = token_1->sym;
            if( sym == NULL )
                return( ERROR );

#if defined( _STANDALONE_ )
            if( AsmBuffer[token_1->label]->class == TC_RES_ID ) {
                /* Kludge for "FLAT" */
                AsmBuffer[token_1->label]->class = TC_ID;
            }
            if( sym->state == SYM_GRP || sym->state == SYM_SEG ) {
                token_2->override = token_1->label;
                token_2->indirect |= token_1->indirect;
                if( token_1->explicit ) {
                    token_2->explicit = token_1->explicit;
                    token_2->mem_type = token_1->mem_type;
                }
                TokenAssign( token_1, token_2 );
            } else if( Parse_Pass > PASS_1 ) {
                if( error_msg )
                    AsmError( ONLY_SEG_OR_GROUP_ALLOWED );
                token_1->type = EXPR_UNDEF;
                return( ERROR );
            }
#else
            if( error_msg )
                AsmError( ONLY_SEG_OR_GROUP_ALLOWED );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
#endif
        } else {
            if( error_msg )
                AsmError( REG_OR_LABEL_EXPECTED_IN_OVERRIDE );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
        }
        break;
    case TC_RES_ID:
        switch( AsmBuffer[index]->u.token ) {
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_FWORD:
        case T_QWORD:
        case T_TBYTE:
        case T_OWORD:
        case T_NEAR:
        case T_FAR:
#if defined( _STANDALONE_ )
        case T_SBYTE:
        case T_SWORD:
        case T_SDWORD:
            if( ( ( AsmBuffer[index + 1]->class != TC_RES_ID ) ||
                  ( AsmBuffer[index + 1]->u.token != T_PTR ) ) &&
                  ( (Options.mode & MODE_IDEAL) == 0 ) ) {
#else
            if( ( AsmBuffer[index + 1]->class != TC_RES_ID ) ||
                ( AsmBuffer[index + 1]->u.token != T_PTR ) ) {
#endif
                // Missing PTR operator
                if( error_msg )
                    AsmError( MISSING_PTR_OPERATOR );
                token_1->type = EXPR_UNDEF;
                return( ERROR );
            }
            TokenAssign( token_1, token_2 );
            token_1->explicit = TRUE;
            switch( AsmBuffer[index]->u.token ) {
            case T_BYTE:
                token_1->mem_type = MT_BYTE;
                break;
            case T_WORD:
                token_1->mem_type = MT_WORD;
                break;
            case T_DWORD:
                token_1->mem_type = MT_DWORD;
                break;
            case T_FWORD:
                token_1->mem_type = MT_FWORD;
                break;
            case T_QWORD:
                token_1->mem_type = MT_QWORD;
                break;
            case T_TBYTE:
                token_1->mem_type = MT_TBYTE;
                break;
            case T_OWORD:
                token_1->mem_type = MT_OWORD;
                break;
            case T_SHORT:
                token_1->mem_type = MT_SHORT;
                break;
            case T_NEAR:
                token_1->mem_type = MT_NEAR;
                break;
            case T_FAR:
                token_1->mem_type = MT_FAR;
                break;
#if defined( _STANDALONE_ )
            case T_SBYTE:
                token_1->mem_type = MT_SBYTE;
                break;
            case T_SWORD:
                token_1->mem_type = MT_SWORD;
                break;
            case T_SDWORD:
                token_1->mem_type = MT_SDWORD;
                break;
#endif
            default:
                break;
            }
            break;
        case T_PTR:
            {
                asm_token   token = AsmBuffer[index - 1]->u.token;

                if( AsmBuffer[index - 1]->class != TC_RES_ID )
                    token = T_NULL;
                switch( token ) {
                case T_BYTE:
                case T_WORD:
                case T_DWORD:
                case T_FWORD:
                case T_QWORD:
                case T_TBYTE:
                case T_OWORD:
                case T_NEAR:
                case T_FAR:
    #if defined( _STANDALONE_ )
                case T_SBYTE:
                case T_SWORD:
                case T_SDWORD:
    #endif
                    TokenAssign( token_1, token_2 );
                    token_1->explicit = TRUE;
                    if( token_1->instr == T_OFFSET )
                        token_1->instr = EMPTY;
                    break;
                default:
                    // find 'ptr' but no 'byte', 'word' etc in front of it
                    if( error_msg )
                        AsmError( NO_SIZE_GIVEN_BEFORE_PTR_OPERATOR );
                    token_1->type = EXPR_UNDEF;
                    return( ERROR );
                }
            }
            break;
        case T_SHORT:
            TokenAssign( token_1, token_2 );
            token_1->explicit = TRUE;
            token_1->mem_type = MT_SHORT;
            break;
        }
        break;
    case TC_INSTR:
        MakeConst( token_1 );
        MakeConst( token_2 );
        if( AsmBuffer[index]->u.token == T_NOT ) {
            if( token_2->type != EXPR_CONST ) {
                if( error_msg )
                    AsmError( CONSTANT_EXPECTED );
                token_1->type = EXPR_UNDEF;
                return( ERROR );
            }
            token_1->type = EXPR_CONST;
        } else {
            if( !check_same( token_1, token_2, EXPR_CONST ) ) {
                if( error_msg )
                    AsmError( CONSTANT_EXPECTED );
                token_1->type = EXPR_UNDEF;
                return( ERROR );
            }
        }
        switch( AsmBuffer[index]->u.token ) {
        case T_SHL:
            token_1->value = token_1->value << token_2->value;
            break;
        case T_SHR:
            token_1->value = (unsigned_32)token_1->value >> token_2->value;
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
    case TC_UNARY_OPERATOR:
        if( ( AsmBuffer[index]->u.token == T_OFFSET ) && ( token_2->type == EXPR_CONST ) ) {
        } else if( token_2->type != EXPR_ADDR ) {
            if( error_msg )
                AsmError( LABEL_IS_EXPECTED );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
        } else if( token_2->instr != EMPTY ) {
            if( error_msg )
                AsmError( LABEL_IS_EXPECTED );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
        }
        switch( AsmBuffer[index]->u.token ) {
#if defined( _STANDALONE_ )
        case T_LENGTH:
        case T_SIZE:
        case T_LENGTHOF:
        case T_SIZEOF:
            sym = token_2->sym;
            if( token_2->mbr != NULL )
                sym = token_2->mbr;
            if( sym == NULL ) {
                if( error_msg ) {
                    AsmError( INVALID_USE_OF_LENGTH_SIZE_OPERATOR );
                }
                return( ERROR );
            }
            switch( AsmBuffer[index]->u.token ) {
            case T_LENGTH:
                if( sym->mem_type == MT_STRUCT ) {
                    token_1->value = sym->count;
                } else if( sym->mem_type == MT_EMPTY ) {
                    token_1->value = 0;
                } else {
                    token_1->value = sym->first_length ? sym->first_length : 1;
                }
                break;
            case T_LENGTHOF:
                if( sym->mem_type == MT_STRUCT ) {
                    token_1->value = sym->count;
                } else if( sym->mem_type == MT_EMPTY ) {
                    token_1->value = 0;
                } else {
                    token_1->value = sym->total_length;
                }
                break;
            case T_SIZE:
                if( sym->mem_type == MT_STRUCT ) {
                    token_1->value = sym->total_size * sym->count;
                } else if( sym->state == SYM_STRUCT ) {
                    token_1->value = sym->total_size;
                } else if( sym->mem_type == MT_NEAR ) {
                    if( sym->segment ) {
                        if( ((dir_node *)sym->segment)->e.seginfo->use_32 ) {
                            token_1->value = 0xFF04;
                        } else {
                            token_1->value = 0xFF02;
                        }
                    } else {
                        token_1->value = 0xFF02;
                    }
                } else if( sym->mem_type == MT_FAR ) {
                    if( sym->segment ) {
                        if( ((dir_node *)sym->segment)->e.seginfo->use_32 ) {
                            token_1->value = 0xFF06;
                        } else {
                            token_1->value = 0xFF05;
                        }
                    } else {
                        token_1->value = 0xFF05;
                    }
                } else {
                    token_1->value = sym->first_size;
                }
                break;
            case T_SIZEOF:
                if( sym->mem_type == MT_STRUCT ) {
                    token_1->value = sym->total_size * sym->count;
                } else {
                    token_1->value = sym->total_size;
                }
                break;
            }
            if( Parse_Pass != PASS_1 && token_1->value == 0 ) {
                if( error_msg )
                    AsmError( DATA_LABEL_IS_EXPECTED );
                token_1->type = EXPR_UNDEF;
                return( ERROR );
            }
            token_1->label = EMPTY;
            token_1->sym = NULL;
            token_1->base_reg = EMPTY;
            token_1->idx_reg = EMPTY;
            token_1->override = EMPTY;
            token_1->instr = EMPTY;
            token_1->type = EXPR_CONST;
            token_1->indirect = FALSE;
            token_1->explicit = FALSE;
            token_1->mem_type = MT_EMPTY;
            break;
#endif
        default:
            TokenAssign( token_1, token_2 );
            token_1->instr = AsmBuffer[index]->u.value;
            break;
        }
        break;
    case TC_ARITH_OPERATOR:
        MakeConst( token_1 );
        MakeConst( token_2 );
        switch( AsmBuffer[index]->u.token ) {
        case T_MOD:
            /*
             * The only formats allowed are:
             *        constant MOD constant
             */
            if( !check_same( token_1, token_2, EXPR_CONST ) ) {
                if( error_msg )
                    AsmError( CONSTANT_EXPECTED );
                token_1->type = EXPR_UNDEF;
                return( ERROR );
            }
            token_1->value %= token_2->value;
            break;
        case T_OP_TIMES:
            /*
             * The only formats allowed are:
             *        constant * constant
             *        register * scaling factor ( 1, 2, 4 or 8 )
             *                   386 only
             */
            if( check_same( token_1, token_2, EXPR_CONST ) ) {
                token_1->value *= token_2->value;
            } else if( check_both( token_1, token_2, EXPR_REG, EXPR_CONST ) ) {
                /* scaling factor */
                if( token_2->type == EXPR_REG ) {
                    /* scale * reg */
                    token_1->idx_reg = token_2->base_reg;
                    token_1->base_reg = EMPTY;
                    token_1->scale = (uint_8)token_1->value;
                    token_1->value = 0;
                    token_2->base_reg = EMPTY;
                } else {
                    /* reg * scale */
                    token_1->idx_reg = token_1->base_reg;
                    token_1->base_reg = EMPTY;
                    token_1->scale = (uint_8)token_2->value;
                }
                token_1->indirect |= token_2->indirect;
                token_1->type = EXPR_ADDR;
            } else {
                if( error_msg )
                    AsmError( MULTIPLICATION_CONSTANT_EXPECTED );
                token_1->type = EXPR_UNDEF;
                return( ERROR );
            }
            break;
        case T_OP_DIVIDE:
            /*
             * The only formats allowed are:
             *        constant / constant
             */
            if( !check_same( token_1, token_2, EXPR_CONST ) ) {
                if( error_msg )
                    AsmError( DIVISION_CONSTANT_EXPECTED );
                token_1->type = EXPR_UNDEF;
                return( ERROR );
            }
            token_1->value /= token_2->value;
            break;
        }
        break;
#if defined( _STANDALONE_ )
    case TC_RELATION_OPERATOR:
        MakeConst( token_1 );
        MakeConst( token_2 );
        if( !check_same( token_1, token_2, EXPR_CONST ) ) {
            if( error_msg )
                AsmError( CONSTANT_EXPECTED );
            token_1->type = EXPR_UNDEF;
            return( ERROR );
        }
        switch( AsmBuffer[index]->u.token ) {
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
        }
        break;
#endif
    }
    token_1->empty = FALSE;
    return( NOT_ERROR );
}

static int evaluate(
    expr_list *operand1,
    int *i,
    int end,
    enum process_flag proc_flag,
    bool (*is_expr)(int) )
/******************************/
{
    expr_list           operand2;
    char                token_needed;
    int                 curr_operator;
    int                 next_operator;
    int                 op_sq_bracket;

    token_needed = FALSE;
    curr_operator = EMPTY;
    op_sq_bracket = op_sq_bracket_level;

    /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
    /* Look at first token, which may be an unary operator or an operand */
    /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

    if( operand1->empty ) {
        if( cmp_token_cls( *i, TC_OP_BRACKET ) ) {
            (*i)++;
            if( *i > end ) {
                if( error_msg )
                    AsmError( OPERAND_EXPECTED );
                operand1->type = EXPR_UNDEF;
                return( ERROR );
            }
            if( evaluate( operand1, i, end, PROC_BRACKET, is_expr ) == ERROR ) {
                return( ERROR );
            }
            if( cmp_token_cls( *i, TC_CL_SQ_BRACKET ) ) {
                // error open ( close ]
                if( error_msg )
                    AsmError( BRACKETS_NOT_BALANCED );
                operand1->type = EXPR_UNDEF;
                return( ERROR );
            }
            (*i)++;
        } else if( is_unary( *i, TRUE ) ) {
            token_needed = TRUE;
        } else if( cmp_token_cls( *i, TC_OP_SQ_BRACKET ) ) {
            if( *i == 0 ) {
                return( ERROR );
            }
            /**/myassert( !cmp_token_cls( (*i)-1, TC_CL_BRACKET ) );
            (*i)++;
            if( *i > end ) {
                if( error_msg )
                    AsmError( OPERAND_EXPECTED );
                operand1->type = EXPR_UNDEF;
                return( ERROR );
            }
            op_sq_bracket_level++;
            if( evaluate( operand1, i, end, PROC_BRACKET, is_expr ) == ERROR ) {
                return( ERROR );
            }
            if( cmp_token_cls( *i, TC_CL_BRACKET ) ) {
                // error open [ close )
                if( error_msg )
                    AsmError( BRACKETS_NOT_BALANCED );
                operand1->type = EXPR_UNDEF;
                return( ERROR );
            }
            if( cmp_token_cls( *i, TC_CL_SQ_BRACKET ) ) {
                op_sq_bracket_level--;
            }
            (*i)++;
        } else if( get_operand( operand1, i, end, is_expr ) == ERROR ) {
            return( ERROR );
        }
    } else {
        token_needed = FALSE;
    }

    /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
    /* If an unary operator is not found, now read the operator */
    /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

    if( !token_needed ) {
        if( *i > end ) {
            /* no operator is found; result is in operand1 */
            if( op_sq_bracket_level != op_sq_bracket ) {
                // error missing ]
                if( error_msg )
                    AsmError( BRACKETS_NOT_BALANCED );
                operand1->type = EXPR_UNDEF;
                return( ERROR );
            } else {
                return( NOT_ERROR );
            }
        }
        /* Read the operator */
        if( cmp_token_cls( *i, TC_CL_BRACKET ) ) {
            if( op_sq_bracket_level != op_sq_bracket ) {
                // error close ) but [ is open
                if( error_msg )
                    AsmError( BRACKETS_NOT_BALANCED );
                operand1->type = EXPR_UNDEF;
                return( ERROR );
            } else {
                return( NOT_ERROR );
            }
        } else if( cmp_token_cls( *i, TC_CL_SQ_BRACKET ) ) {
            return( NOT_ERROR );
        } else if( cmp_token_cls( *i, TC_OP_SQ_BRACKET ) ) {
            AsmBuffer[*i]->class = TC_PLUS;
            op_sq_bracket_level++;
        } else if( !is_optr(*i) ) {
            if( error_msg )
                AsmError( OPERATOR_EXPECTED );
            operand1->type = EXPR_UNDEF;
            return( ERROR );
        }
    }

    do {
        curr_operator = *i;
        (*i)++;

        /*:::::::::::::::::::::::::::*/
        /* Now read the next operand */
        /*:::::::::::::::::::::::::::*/

        if( *i > end ) {
            if( error_msg )
                AsmError( OPERAND_EXPECTED );
            operand1->type = EXPR_UNDEF;
            return( ERROR );
        }

        init_expr( &operand2 );
        if( cmp_token_cls( *i, TC_OP_BRACKET ) ) {
            (*i)++;
            if( evaluate( &operand2, i, end, PROC_BRACKET, is_expr ) == ERROR ) {
                return( ERROR );
            }
            if( cmp_token_cls( *i, TC_CL_BRACKET ) ) {
                (*i)++;
            }
        } else if( cmp_token_cls( *i, TC_OP_SQ_BRACKET ) ) {
            op_sq_bracket_level++;
            (*i)++;
            if( evaluate( &operand2, i, end, PROC_BRACKET, is_expr ) == ERROR ) {
                return( ERROR );
            }
        } else if( is_unary( *i, TRUE ) ) {
            if( evaluate( &operand2, i, end, PROC_OPERAND, is_expr ) == ERROR ) {
                return( ERROR );
            }
        } else if( is_optr( *i ) ) {
            if( error_msg )
                AsmError( OPERAND_EXPECTED );
            operand1->type = EXPR_UNDEF;
            return( ERROR );
        } else if( get_operand( &operand2, i, end, is_expr ) == ERROR ) {
            return( ERROR );
        }

        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
        /* Close all available brackets                                    */
        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

        while( ( *i <= end )
            && ( op_sq_bracket_level > op_sq_bracket )
            && cmp_token_cls( *i, TC_CL_SQ_BRACKET ) ) {
            (*i)++;
            op_sq_bracket_level--;
        }

        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
        /* Look at the next operator and compare its priority with 1st one */
        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

        next_operator = FALSE;
        if( *i <= end ) {
            if( !is_optr( *i )
                || is_unary( *i, FALSE )
                || cmp_token_cls( *i, TC_OP_BRACKET ) ) {
                if( error_msg )
                    AsmError( OPERATOR_EXPECTED );
                operand1->type = EXPR_UNDEF;
                return( ERROR );
            } else if( !cmp_token_cls( *i, TC_CL_BRACKET ) &&
                       !cmp_token_cls( *i, TC_CL_SQ_BRACKET ) ) {
                if( cmp_token_cls( *i, TC_OP_SQ_BRACKET ) ) {
                    if( ( PLUS_PRECEDENCE <= get_precedence( curr_operator ) )
                        || ( proc_flag == PROC_BRACKET ) ) {
                        AsmBuffer[*i]->class = TC_PLUS;
                        op_sq_bracket_level++;
                        next_operator = TRUE;
                    }
                } else {
                    if( get_precedence( *i ) < get_precedence( curr_operator ) ) {
                        if( evaluate( &operand2, i, end, PROC_OPERAND, is_expr ) == ERROR )
                            return( ERROR );
                        while( ( *i <= end )
                            && ( op_sq_bracket_level > op_sq_bracket )
                            && cmp_token_cls( *i, TC_CL_SQ_BRACKET ) ) {
                            (*i)++;
                            op_sq_bracket_level--;
                        }
                        if( cmp_token_cls( *i, TC_OP_SQ_BRACKET ) ) {
                            if( proc_flag == PROC_BRACKET ) {
                                AsmBuffer[*i]->class = TC_PLUS;
                                op_sq_bracket_level++;
                                next_operator = TRUE;
                            }
                        }
                    } else if( proc_flag == PROC_BRACKET ) {
                        next_operator = TRUE;
                    }
                }
            }
        }

        /*::::::::::::::*/
        /* Now evaluate */
        /*::::::::::::::*/

        if( calculate( operand1, &operand2, curr_operator ) == ERROR ) {
            return( ERROR );
        }

    } while ( ( next_operator == TRUE )
        || ( ( proc_flag == PROC_BRACKET )
            && !cmp_token_cls( *i, TC_CL_BRACKET )
            && !cmp_token_cls( *i, TC_CL_SQ_BRACKET )
            && ( *i < end ) ) );
    if( op_sq_bracket_level != op_sq_bracket ) {
        if( error_msg )
            AsmError( BRACKETS_NOT_BALANCED );
        operand1->type = EXPR_UNDEF;
        return( ERROR );
    }
    return( NOT_ERROR );
}

static bool is_expr1( int i )
/***************************/
/* Check if the token is part of an expression */
{
    switch( AsmBuffer[i]->class ) {
    case TC_INSTR:
        switch( AsmBuffer[i]->u.token ) {
        case T_SHL:
        case T_SHR:
        case T_NOT:
        case T_AND:
        case T_OR:
        case T_XOR:
            if( i == 0 ) {
                /* It is an instruction instead */
                break;
            } else if( AsmBuffer[i-1]->class == TC_COLON ) {
                /* It is an instruction instead */
                break;
            } else if( AsmBuffer[i-1]->u.token == T_LOCK ) {
                /* It is an instruction:
                         lock and dword ptr [ebx], 1
                */
                break;
            } else {
                return( TRUE );
            }
        default:
            break;
        }
        break;
    case TC_UNARY_OPERATOR:
//        case T_SEG:
//        case T_OFFSET:
        if( i+1 < TokCnt )
            return( TRUE );
        break;
    case TC_ARITH_OPERATOR:
        return( TRUE );
#if defined( _STANDALONE_ )
    case TC_RELATION_OPERATOR:
        return( TRUE );
    case TC_RES_ID:
        switch( AsmBuffer[i]->u.token ) {
        case T_FLAT:
            DefFlatGroup();
            return( TRUE );
        }
        break;
#endif
    case TC_REG:
        return( TRUE );
    case TC_PLUS:
    case TC_MINUS:
        /* hack to stop asmeval from hanging on floating point numbers */
        if( AsmBuffer[i+1]->class == TC_FLOAT )
            break;
        return( TRUE );
    case TC_NUM:
    case TC_OP_BRACKET:
    case TC_CL_BRACKET:
    case TC_OP_SQ_BRACKET:
    case TC_CL_SQ_BRACKET:
        return(  TRUE );
    case TC_COLON:
#if defined( _STANDALONE_ )
        if( i == 1 || ( AsmBuffer[i+1]->class == TC_DIRECTIVE &&
                        AsmBuffer[i+1]->u.token == T_EQU2 ) ) {
            /* It is the colon following the label or it is a := */
            break;
        } else {
            return( TRUE );
        }
#else
        return( TRUE );
#endif
    case TC_ID:
        if( i != 0 )
            /* It is not a label */
            return( TRUE );
        break;
    case TC_STRING:
        return( TRUE );
    case TC_DOT:
        return( TRUE );
    case TC_PATH:
    default:
        break;
    }
    return( FALSE );
}

static bool is_expr2( int i )
/***************************/
/* Check if the token is part of an expression */
{
    switch( AsmBuffer[i]->class ) {
    case TC_INSTR:
        switch( AsmBuffer[i]->u.token ) {
        case T_SHL:
        case T_SHR:
        case T_NOT:
        case T_AND:
        case T_OR:
        case T_XOR:
            if( i == 0 ) {
                /* It is an instruction instead */
                break;
            } else if( AsmBuffer[i-1]->class == TC_COLON ) {
                /* It is an instruction instead */
                break;
            } else if( AsmBuffer[i-1]->u.token == T_LOCK ) {
                /* It is an instruction:
                         lock and dword ptr [ebx], 1
                */
                break;
            } else {
                return( TRUE );
            }
        default:
            break;
        }
        break;
    case TC_UNARY_OPERATOR:
        return( TRUE );
    case TC_ARITH_OPERATOR:
        return( TRUE );
    case TC_RELATION_OPERATOR:
        return( TRUE );
    case TC_RES_ID:
        switch( AsmBuffer[i]->u.token ) {
#if defined( _STANDALONE_ )
        case T_FLAT:
            DefFlatGroup();
            return( TRUE );
        case T_SBYTE:
        case T_SWORD:
        case T_SDWORD:
#endif
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_FWORD:
        case T_QWORD:
        case T_TBYTE:
        case T_OWORD:
        case T_NEAR:
        case T_FAR:
        case T_PTR:
        case T_SHORT:
            return( TRUE );
        default:
            AsmBuffer[i]->class = TC_ID;
            return( TRUE );
        }
        break;
    case TC_REG:
        return( TRUE );
    case TC_PLUS:
    case TC_MINUS:
        /* hack to stop asmeval from hanging on floating point numbers */
        if( AsmBuffer[i+1]->class == TC_FLOAT )
            break;
        return( TRUE );
    case TC_NUM:
    case TC_OP_BRACKET:
    case TC_CL_BRACKET:
    case TC_OP_SQ_BRACKET:
    case TC_CL_SQ_BRACKET:
        return(  TRUE );
    case TC_COLON:
#if defined( _STANDALONE_ )
        if( ( AsmBuffer[i+1]->class == TC_DIRECTIVE )
            && ( AsmBuffer[i+1]->u.token == T_EQU2 ) )
            /* It is a := */
            break;
#endif
        return( TRUE );
    case TC_ID:
        return( TRUE );
    case TC_STRING:
        return( TRUE );
    case TC_DOT:
        return( TRUE );
    case TC_PATH:
    default:
        break;
    }
    return( FALSE );
}

static int fix_parens( void )
/***************************/
/* Take out those brackets which may surround a non-expression, e.g.
   Right now only 'dup' requires a pair of parentheses, which should be
   taken out temporarily */
{
    int         i;
    int         store;
    int         dup_count = 0;
    int         non_dup_bracket_count = 0;

    store = 0;
    for( i = 0; i < TokCnt; i++ ) {

        if( AsmBuffer[i]->class == TC_RES_ID && AsmBuffer[i]->u.token == T_DUP ) {
            dup_count++;
            if( AsmBuffer[++i]->class != TC_OP_BRACKET ) {
                if( error_msg )
                    AsmError( BRACKET_EXPECTED );
                return( ERROR );
            }
            TakeOut[store++] = i;               // Store the location
            AsmBuffer[i]->class = TC_NOOP;

        } else if( AsmBuffer[i]->class == TC_OP_BRACKET ) {
            non_dup_bracket_count++;
        } else if( AsmBuffer[i]->class == TC_CL_BRACKET ) {
            if( non_dup_bracket_count ) {
                non_dup_bracket_count--;
            } else {
                dup_count--;
                TakeOut[store++] = -i;  // store close brackets as -ve values
                AsmBuffer[i]->class = TC_NOOP;
            }
        }
    }
    if( dup_count != 0 ) {
        if( error_msg )
            AsmError( BRACKETS_NOT_BALANCED );
        return( ERROR );
    }
    TakeOut[store] = -1;        // Mark the end
    return( NOT_ERROR );
}

static int fix( expr_list *res, int start, int end )
/*********************************************************************/
/* Convert the result in res into tokens and put them back in AsmBuffer[] */
{
    int                 size = 0;
    int                 diff;
    int                 i;
    int                 old_start;
    int                 old_end;
    int                 need_number;

    MakeConst( res );
    if( res->type == EXPR_CONST ) {
        if( res->override != EMPTY ) {
            *(AsmBuffer[start++]) = *(AsmBuffer[res->override]);
            AsmBuffer[start++]->class = TC_COLON;
        }
        if( res->string == NULL ) {
            AsmBuffer[ start ]->class = TC_NUM;
            AsmBuffer[ start ]->u.value = res->value;
            AsmBuffer[ start++ ]->string_ptr = "";
        } else {
            AsmBuffer[ start ]->class = TC_STRING;
            AsmBuffer[ start++ ]->string_ptr = res->string;
        }

    } else if( res->type == EXPR_REG && !res->indirect ) {

        if( res->override != EMPTY ) {
            *(AsmBuffer[start++]) = *(AsmBuffer[res->override]);
            AsmBuffer[start++]->class = TC_COLON;
            AsmBuffer[ start++ ]->class = TC_OP_SQ_BRACKET;
        }
        AsmBuffer[ start ]->class = TC_REG;
        AsmBuffer[ start ]->string_ptr = AsmBuffer[ res->base_reg ]->string_ptr;
        AsmBuffer[ start++ ]->u.value = AsmBuffer[ res->base_reg ]->u.value;
        if( AsmBuffer[ res->base_reg ]->u.token == T_ST && res->idx_reg > 0 ) {
//            AsmBuffer[start]->string_ptr = "(";
//            AsmBuffer[start++]->class = TC_OP_BRACKET;
            AsmBuffer[start]->class = TC_NUM;
            AsmBuffer[start]->u.value = res->idx_reg;
            AsmBuffer[start++]->string_ptr = "";
//            AsmBuffer[start]->string_ptr = ")";
//            AsmBuffer[start++]->class = TC_CL_BRACKET;
        }
        if( res->override != EMPTY ) {
            AsmBuffer[ start++ ]->class = TC_CL_SQ_BRACKET;
        }

    } else {

        if( res->instr != EMPTY ) {
            size++;
        } else if( res->mbr != NULL && res->mbr->mem_type != MT_EMPTY ) {
            size += 2;
        }

        if( res->type != EXPR_REG ) {
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
            AsmBuffer[start]->class = TC_UNARY_OPERATOR;
            AsmBuffer[start++]->u.value = res->instr;
        } else if( res->mbr != NULL && res->mbr->mem_type != MT_EMPTY ) {
            AsmBuffer[start]->class = TC_RES_ID;
            switch( res->mbr->mem_type ) {
            case MT_BYTE:
                AsmBuffer[start++]->u.token = T_BYTE;
                break;
            case MT_WORD:
                AsmBuffer[start++]->u.token = T_WORD;
                break;
            case MT_DWORD:
                AsmBuffer[start++]->u.token = T_DWORD;
                break;
            case MT_FWORD:
                AsmBuffer[start++]->u.token = T_FWORD;
                break;
            case MT_QWORD:
                AsmBuffer[start++]->u.token = T_QWORD;
                break;
            case MT_TBYTE:
                AsmBuffer[start++]->u.token = T_TBYTE;
                break;
            case MT_OWORD:
                AsmBuffer[start++]->u.token = T_OWORD;
                break;
            case MT_SHORT:
                AsmBuffer[start++]->u.token = T_SHORT;
                break;
            case MT_NEAR:
                AsmBuffer[start++]->u.token = T_NEAR;
                break;
            case MT_FAR:
                AsmBuffer[start++]->u.token = T_FAR;
                break;
#if defined( _STANDALONE_ )
            case MT_SBYTE:
                AsmBuffer[start++]->u.token = T_SBYTE;
                break;
            case MT_SWORD:
                AsmBuffer[start++]->u.token = T_SWORD;
                break;
            case MT_SDWORD:
                AsmBuffer[start++]->u.token = T_SDWORD;
                break;
#endif
            default:
                break;
            }
//            AsmBuffer[start++]->value = res->mbr->mem_type;
            AsmBuffer[start]->class = TC_RES_ID;
            AsmBuffer[start++]->u.token = T_PTR;
        }

        if( res->override != EMPTY ) {
            *(AsmBuffer[start++]) = Store[res->override-old_start];
            AsmBuffer[start++]->class = TC_COLON;
        }

        if( res->label != EMPTY && res->type != EXPR_REG ) {
            *(AsmBuffer[start++]) = Store[res->label-old_start];
        }

        if( res->base_reg != EMPTY ) {
            AsmBuffer[start++]->class = TC_OP_SQ_BRACKET;
            AsmBuffer[start]->class = TC_REG;
            AsmBuffer[start]->string_ptr = Store[res->base_reg-old_start].string_ptr;
            AsmBuffer[start++]->u.value = Store[res->base_reg-old_start].u.value;
            AsmBuffer[start++]->class = TC_CL_SQ_BRACKET;
        }
        if( res->idx_reg != EMPTY ) {
            AsmBuffer[start++]->class = TC_OP_SQ_BRACKET;
            AsmBuffer[start]->class = TC_REG;
            AsmBuffer[start]->string_ptr = Store[res->idx_reg-old_start].string_ptr;
            AsmBuffer[start++]->u.value = Store[res->idx_reg-old_start].u.value;
            if( res->scale != 1 ) {
                AsmBuffer[start]->string_ptr = "*";
                AsmBuffer[start]->u.token = T_OP_TIMES;
                AsmBuffer[start++]->class = TC_ARITH_OPERATOR;
                AsmBuffer[start]->class = TC_NUM;
                AsmBuffer[start]->u.value = res->scale;
                AsmBuffer[start++]->string_ptr = "";
                res->scale = 1;
            }
            AsmBuffer[start++]->class = TC_CL_SQ_BRACKET;
        }

        if( need_number ) {
            AsmBuffer[start++]->class = TC_OP_SQ_BRACKET;
            AsmBuffer[start]->class = TC_NUM;
            AsmBuffer[start]->u.value = res->value;
            AsmBuffer[start++]->string_ptr = "";
            AsmBuffer[start++]->class = TC_CL_SQ_BRACKET;
        }

        /**/myassert( ( start-1 ) <= end );
    }

    for( i = start; i <= end; i++ ) {
        AsmBuffer[i]->class = TC_NOOP;
    }
    return( end );
}

static void fix_final( void )
/***************************/
/* Put back those brackets taken out by fix_parens() and take out all TC_NOOP
   tokens */
{
    int         start;
    int         end;
    int         i;
    int         diff;
    int         dup_count = 0;

    for( i = 0;; i++ ) {
        if( dup_count == 0 && TakeOut[i] == -1 )
            break;
        if( TakeOut[i] > 0 ) {
            AsmBuffer[ TakeOut[i] ]->class = TC_OP_BRACKET;
            dup_count++;
        } else {
            AsmBuffer[ -( TakeOut[i] ) ]->class = TC_CL_BRACKET;
            dup_count--;
        }
        myassert( i < MAX_TOKEN );
    }

    for( start = 0; start < TokCnt; start++ ) {
        if( AsmBuffer[start]->class == TC_NOOP ) {
            for( end = start + 1;; end++ ) {
                if( AsmBuffer[end]->class != TC_NOOP
                    || end == TokCnt ) {
                    break;
                }
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

extern int EvalExpr( int count, int start_tok, int end_tok, bool flag_msg )
/*************************************************************************/
{
    int         i = start_tok;
    int         start;          // position of first token of an expression
    int         num;            // number of tokens in the expression
    bool        final = FALSE;
    expr_list   result;

    if( AsmBuffer[end_tok]->class == TC_FINAL )
        final = TRUE;

    TokCnt = count;

    if( fix_parens() == ERROR ) {
        // take out those parentheses which are not part of an expression
        return( ERROR );
    }

    while( i < TokCnt && i <= end_tok ) {
        if( is_expr1( i ) ) {
            start = i++;
            num = 0;
            for( ;; ) {
                if( i >= TokCnt )
                    break;
                if( i > end_tok )
                    break;
                if( !is_expr1( i ) )
                    break;
                i++;
                num++;
            }
            if( num == 1 && AsmBuffer[start]->class == TC_REG && AsmBuffer[start+1]->class == TC_COLON ) {
                // Massive kludge ahead:
                // skip register+colon
            } else if( num == 0 && AsmBuffer[start]->class == TC_REG ) {
                // skip register
            } else if( num == 0 && AsmBuffer[start]->class == TC_NUM ) {
                // skip number
            } else {
                i = start;
                init_expr( &result );
                op_sq_bracket_level = 0;
                error_msg = flag_msg;
                if( evaluate( &result, &i, i + num, PROC_BRACKET, is_expr1 ) == ERROR ) {
                    return( ERROR );
                }
                i = fix( &result, start, start + num );
                if( count != TokCnt ) {
                    /* we just changed the number of tokens, so update
                     * count & end_tok appropriately */
                    end_tok += TokCnt - count;
                    count = TokCnt;
                }
            }
        }
        i++;
    }
    fix_final();

    if( AsmBuffer[TokCnt]->class == TC_NOOP || final ) {
        AsmBuffer[ TokCnt ]->class = TC_FINAL;
        AsmBuffer[ TokCnt ]->string_ptr = NULL;
    }

    return( TokCnt );
}

extern int EvalOperand( int *start_tok, int count, expr_list *result, bool flag_msg )
/***********************************************************************************/
{
    int         i = *start_tok;
    int         num;            // number of tokens in the expression

    init_expr( result );
    if( AsmBuffer[i]->class == TC_FINAL )
        return( NOT_ERROR );
    if( !is_expr2( i ) )
        return( NOT_ERROR );

    num = 0;
    for( ;; ) {
        i++;
        if( i >= count )
            break;
        if( !is_expr2( i ) )
            break;
        num++;
    }
    op_sq_bracket_level = 0;
#if defined( _STANDALONE_ )
    if( Options.mode & MODE_IDEAL ) {
        Definition.struct_depth = 0;
    }
#endif
    error_msg = flag_msg;
    i = evaluate( result, start_tok, *start_tok + num, PROC_BRACKET, is_expr2 );
#if defined( _STANDALONE_ )
    if( Options.mode & MODE_IDEAL ) {
        Definition.struct_depth = 0;
    }
#endif
    return( i );
}

#if 0
static int is_expr_const( int i )
/*******************************/
/* Check if the token is part of constant expression */
{
    switch( AsmBuffer[i]->class ) {
    case TC_INSTR:
        switch( AsmBuffer[i]->u.token ) {
        case T_SHL:
        case T_SHR:
        case T_NOT:
        case T_AND:
        case T_OR:
        case T_XOR:
            if( i == 0 ) {
                /* It is an instruction instead */
                return( FALSE );
            } else if( AsmBuffer[i-1]->class == TC_COLON ) {
                /* It is an instruction instead */
                return( FALSE );
            } else if( AsmBuffer[i-1]->u.token == T_LOCK ) {
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
    case TC_ARITH_OPERATOR:
        return( TRUE );
    case TC_RELATION_OPERATOR:
        return( TRUE );
    case TC_PLUS:
    case TC_MINUS:
        /* hack to stop asmeval from hanging on floating point numbers */
        if( AsmBuffer[i+1]->class == TC_FLOAT )
            return( FALSE );
    case TC_NUM:
    case TC_OP_BRACKET:
    case TC_CL_BRACKET:
    case TC_OP_SQ_BRACKET:
    case TC_CL_SQ_BRACKET:
        return(  TRUE );
    case TC_ID:
        if( i == 0 ) {
            /* It is a label */
            return( FALSE );
        } else {
            return( TRUE );
        }
    case TC_STRING:
        return( TRUE );
    default:
        return( FALSE );
    }
}
#endif

#if defined( _STANDALONE_ )

extern int EvalConstant( int count, int start_tok, int end_tok, bool flag_msg )
/*****************************************************************************/
{
    int         i = start_tok;
    bool        const_expr = TRUE;

    TokCnt = count;
    error_msg = flag_msg;
    while( i < TokCnt && i <= end_tok ) {
        if( !is_expr1( i ) ) {
            const_expr = FALSE;
            break;
        }
        i++;
    }
    return( TokCnt );
}

#endif
