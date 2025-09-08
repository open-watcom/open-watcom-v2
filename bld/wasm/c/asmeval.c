/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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

static struct {
    token_idx           idx;
    bool                close_bracket;
} TakeOut[MAX_TOKEN + 1];

static token_idx        TokCnt;

static asm_tok          Store[MAX_TOKEN + 1];

static int              op_sq_bracket_level;
static bool             error_msg;

typedef enum process_flag {
    PROC_BRACKET,
    PROC_OPERAND
} process_flag;

static bool evaluate( expr_list *, token_buffer *tokbuf, token_idx *, token_idx, process_flag, bool (*)(token_buffer *, token_idx) );

static void init_expr( expr_list *new )
/*************************************/
{
    new->empty    = true;
    new->type     = EXPR_EMPTY;
    new->label    = INVALID_IDX;
    new->override = INVALID_IDX;
    new->instr    = T_NULL;
    new->base_reg = INVALID_IDX;
    new->idx_reg  = INVALID_IDX;
    new->indirect = false;
    new->explicit = false;
    new->abs      = false;
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

static int get_precedence( token_buffer *tokbuf, token_idx i )
/************************************************************/
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

    switch( tokbuf->tokens[i].class ) {
    case TC_UNARY_OPERATOR:
        switch( tokbuf->tokens[i].u.token ) {
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
        switch( tokbuf->tokens[i].u.token ) {
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
        switch( tokbuf->tokens[i].u.token ) {
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
    return( -1 );
}

static bool get_operand( expr_list *new, token_buffer *tokbuf, token_idx *start, token_idx end, bool (*is_expr)(token_buffer *, token_idx) )
/******************************************************************************************************************************************/
{
    char        *tmp;
    token_idx   i = *start;

    init_expr( new );
    switch( tokbuf->tokens[i].class ) {
    case TC_NUM:
        new->empty = false;
#if defined( _STANDALONE_ )
        if( (Options.mode & MODE_IDEAL)
          && ( op_sq_bracket_level ) ) {
            new->type = EXPR_ADDR;
            new->indirect = true;
        } else {
            new->type = EXPR_CONST;
        }
#else
        new->type = EXPR_CONST;
#endif
        new->value = tokbuf->tokens[i].u.value;
        break;
    case TC_STRING:
        new->empty = false;
        new->type = EXPR_CONST;
        new->string = tokbuf->tokens[i].string_ptr;
        new->value = 0;
        for( tmp = new->string; *tmp != '\0'; tmp++ ) {
            new->value <<= 8;
            new->value |= (*tmp);
        }
        break;
    case TC_REG:
        new->empty = false;
        new->type = EXPR_REG;
        new->base_reg = i;
        if( op_sq_bracket_level > 0 ) {
            switch( tokbuf->tokens[i].u.token ) {
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
                new->indirect = true;
                break;
            case T_DS:
            case T_CS:
            case T_ES:
            case T_SS:
            case T_FS:
            case T_GS:
                if( tokbuf->tokens[i+1].class != TC_COLON ) {
                    if( error_msg )
                        AsmError( ILLEGAL_USE_OF_REGISTER );
                    new->type = EXPR_UNDEF;
                    return( RC_ERROR );
                }
                break;
            default:
                if( error_msg )
                    AsmError( ILLEGAL_USE_OF_REGISTER );
                new->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
        } else if( tokbuf->tokens[i].u.token == T_ST ) {

            expr_list   sti;

            // read st(i), put i into idx_reg
            i++;
            switch( tokbuf->tokens[i].class ) {
            case TC_OP_BRACKET:
            case TC_OP_SQ_BRACKET:
            case TC_NUM:
                *start = i;
                init_expr( &sti );
                if( evaluate( &sti, tokbuf, start, end, PROC_OPERAND, is_expr ) ) {
                    new->type = EXPR_UNDEF;
                    return( RC_ERROR );
                }
                if( sti.type != EXPR_CONST ) {
                    if( error_msg )
                        AsmError( CONSTANT_EXPECTED );
                    new->type = EXPR_UNDEF;
                    return( RC_ERROR );
                }
                new->idx_reg = (token_idx)sti.value;
                return( RC_OK );
            default:
                new->idx_reg = (token_idx)0; // st = st(0)
                break;
            }
        }
        break;
    case TC_ID:
#if defined( _STANDALONE_ )
        if( Parse_Pass == PASS_1 ) {
            new->sym = AsmLookup( tokbuf->tokens[i].string_ptr );
            if( new->sym == NULL ) {
                new->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
        } else {
            new->sym = AsmGetSymbol( tokbuf->tokens[i].string_ptr );
            if( new->sym == NULL ) {
                if( error_msg )
                    AsmErr( SYMBOL_NOT_DEFINED, tokbuf->tokens[i].string_ptr );
                new->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
#if 0
// FIXME !!!!!
// problem with aliases and equ directive
            if( ( new->sym == NULL )
              || ( new->sym->state == SYM_UNDEFINED ) ) {
                if( error_msg )
                    AsmErr( SYMBOL_NOT_DEFINED, tokbuf->tokens[i].string_ptr );
                new->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
#endif
        }
        if( new->sym != NULL ) {
            new->sym->referenced = true;
            if( ( new->sym->state == SYM_STRUCT  )
              || (Options.mode & MODE_IDEAL)
              && ( new->sym->mem_type == MT_STRUCT ) ) {
                new->empty = false;
                new->value = new->sym->offset;
                new->mbr = new->sym;
                new->sym = NULL;
                new->type = EXPR_ADDR;
                if( (Options.mode & MODE_IDEAL)
                  && ( op_sq_bracket_level ) ) {
                    Definition.struct_depth++;
                    if( new->mbr->state == SYM_STRUCT ) {
                        Definition.curr_struct = (dir_node_handle)new->mbr;
                        (*start)++; /* Skip structure override and process next token */
                        return( get_operand( new, tokbuf, start, end, is_expr ) );
                    } else {
                        new->indirect = true;
                        Definition.curr_struct = (dir_node_handle)new->mbr->structure;
                    }
                }
                break;
            } else if( new->sym->state == SYM_STRUCT_FIELD ) {
                if( (Options.mode & MODE_IDEAL)
                  && ( Definition.struct_depth ) ) {
                    Definition.struct_depth--;
                    new->indirect = true;
                }
                new->empty = false;
                new->mem_type = new->sym->mem_type;
                new->value = new->sym->offset;
                new->mbr = new->sym;
                new->sym = NULL;
                new->type = EXPR_ADDR;
                break;
            }
            if( new->sym->mem_type == MT_ABS ) {
                new->abs = true;
            } else {
                new->mem_type = new->sym->mem_type;
            }
        }
#else
        new->sym = AsmLookup( tokbuf->tokens[i].string_ptr );
        new->mem_type = new->sym->mem_type;
#endif
        new->empty = false;
        new->type = EXPR_ADDR;
        new->label = i;
        break;
    case TC_RES_ID:
        new->sym = AsmLookup( tokbuf->tokens[i].string_ptr );
        new->empty = false;
        new->type = EXPR_ADDR;
        new->label = i;
        break;
    default:
        new->type = EXPR_UNDEF;
        return( RC_ERROR );
    }
    (*start)++;
    return( RC_OK );
}

static bool is_optr( token_buffer *tokbuf, token_idx i )
/*******************************************************
 * determine if it is an operator
 */
{
    switch( tokbuf->tokens[i].class ) {
    case TC_REG:
    case TC_NUM:
    case TC_ID:
    case TC_RES_ID:
    case TC_STRING:
    case TC_PATH:
    case TC_OP_BRACKET:
        return( false );
    }
    return( true );
}

static bool is_unary( token_buffer *tokbuf, token_idx i, bool sign )
/*******************************************************************
 * determine if it is an unary operand
 */
{
    switch( tokbuf->tokens[i].class ) {
    case TC_UNARY_OPERATOR:
        return( true );
    case TC_INSTR:
        if( tokbuf->tokens[i].u.token == T_NOT )
            return( true );
        break;
    case TC_POSITIVE:
    case TC_NEGATIVE:
        return( true );
    case TC_PLUS:
        if( sign ) {
            tokbuf->tokens[i].class = TC_POSITIVE;
            return( true );
        }
        break;
    case TC_MINUS:
        if( sign ) {
            tokbuf->tokens[i].class = TC_NEGATIVE;
            return( true );
        }
        break;
    case TC_RES_ID:
        switch( tokbuf->tokens[i].u.token ) {
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
            return( true );
        }
        break;
    default:
        break;
    }
    return( false );
}

static bool cmp_token_cls( token_buffer *tokbuf, token_idx i, tok_class cls )
/****************************************************************************
 * compare tokbuf->tokens[i] and tok
 */
{
    return( tokbuf->tokens[i].class == cls );
}

static bool check_same( expr_list *tok_1, expr_list *tok_2, exprtype type )
/*************************************************************************/
/* Check if both tok_1 and tok_2 equal type */
{
    return( tok_1->type == type && tok_2->type == type );
}

static bool check_both( expr_list *tok_1, expr_list *tok_2, exprtype type1, exprtype type2 )
/******************************************************************************************/
/* Check if tok_1 == type1 and tok_2 == type2 or vice versa */
{
    if( tok_1->type == type1
      && tok_2->type == type2 ) {
        return( true );
    } else if( tok_1->type == type2
      && tok_2->type == type1 ) {
        return( true );
    } else {
        return( false );
    }
}

static void index_connect( expr_list *tok_1, expr_list *tok_2 )
/*************************************************************/
/* Connects the register lists */
{
    if( ISINVALID_IDX( tok_1->base_reg ) ) {
        if( ISVALID_IDX( tok_2->base_reg ) ) {
            tok_1->base_reg = tok_2->base_reg;
            tok_2->base_reg = INVALID_IDX;
        } else if( ISVALID_IDX( tok_2->idx_reg )
          && ( tok_2->scale == 1 ) ) {
            tok_1->base_reg = tok_2->idx_reg;
            tok_2->idx_reg = INVALID_IDX;
        }
    }
    if( ISINVALID_IDX( tok_1->idx_reg ) ) {
        if( ISVALID_IDX( tok_2->idx_reg ) ) {
            tok_1->idx_reg = tok_2->idx_reg;
            tok_1->scale = tok_2->scale;
        } else if( ISVALID_IDX( tok_2->base_reg ) ) {
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
    token->label = INVALID_IDX;
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
    if( ISVALID_IDX( token->base_reg ) )
        return;
    if( ISVALID_IDX( token->idx_reg ) )
        return;
    if( ISVALID_IDX( token->override ) )
        return;
    token->instr = T_NULL;
    token->type = EXPR_CONST;
    token->indirect = false;
    token->explicit = false;
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
#else
    /* unused parameters */ (void)token;
#endif
}

static bool check_direct_reg( expr_list *token_1, expr_list *token_2 )
/********************************************************************/
{
    return( ( token_1->type == EXPR_REG ) && !token_1->indirect
        || ( token_2->type == EXPR_REG ) && !token_2->indirect );
}

static bool calculate( expr_list *token_1, expr_list *token_2, token_buffer *tokbuf, token_idx index )
/*****************************************************************************************************
 * Perform the operation between token_1 and token_2
 */
{
    asm_sym_handle  sym;
    asm_token       reg_token;

    token_1->string = NULL;

    switch( tokbuf->tokens[index].class ) {
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
            return( RC_ERROR );
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
            return( RC_ERROR );
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

        if( check_direct_reg( token_1, token_2 ) ) {
            if( error_msg )
                AsmError( ILLEGAL_USE_OF_REGISTER );
            token_1->type = EXPR_UNDEF;
            return( RC_ERROR );
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
                    return( RC_ERROR );
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
                switch( token_2->instr ) {
                case T_OFFSET:
                case T_NULL:
                    break;
                default:
                    if( error_msg )
                        AsmError( LABEL_IS_EXPECTED );
                    token_1->type = EXPR_UNDEF;
                    return( RC_ERROR );
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
                token_2->base_reg = INVALID_IDX;
                token_2->idx_reg = INVALID_IDX;
            }
            token_1->value += token_2->value;
            token_1->indirect |= token_2->indirect;
            token_1->type = EXPR_ADDR;
        } else {
            /* Error */
            if( error_msg )
                AsmError( ADDITION_CONSTANT_EXPECTED );
            token_1->type = EXPR_UNDEF;
            return( RC_ERROR );
        }
        break;
    case TC_DOT:
        /*
         * The only formats allowed are:
         *        register . address       ( only inside [] )
         *        address  . address
         *        address  . constant
         */

        if( check_direct_reg( token_1, token_2 ) ) {
            if( error_msg )
                AsmError( ILLEGAL_USE_OF_REGISTER );
            token_1->type = EXPR_UNDEF;
            return( RC_ERROR );
        }
        if( check_same( token_1, token_2, EXPR_ADDR ) ) {

            index_connect( token_1, token_2 );
            token_1->indirect |= token_2->indirect;
            if( token_1->sym != NULL ) {
                if( token_2->sym != NULL ) {
                    if( error_msg )
                        AsmError( SYNTAX_ERROR );
                    token_1->type = EXPR_UNDEF;
                    return( RC_ERROR );
                }
            } else if( token_2->sym != NULL ) {
                token_1->label = token_2->label;
                token_1->sym = token_2->sym;
            }
            if( token_2->mbr != NULL ) {
                token_1->mbr = token_2->mbr;
            }
            token_1->value += token_2->value;
            if( !token_1->explicit ) {
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
                if( token_2->instr != T_NULL ) {
                    if( error_msg )
                        AsmError( LABEL_IS_EXPECTED );
                    token_1->type = EXPR_UNDEF;
                    return( RC_ERROR );
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
                token_2->base_reg = INVALID_IDX;
                token_2->idx_reg = INVALID_IDX;
            }
            token_1->value += token_2->value;
            token_1->indirect |= token_2->indirect;
            token_1->type = EXPR_ADDR;
        } else {
            /* Error */
            if( error_msg )
                AsmError( SYNTAX_ERROR );
            token_1->type = EXPR_UNDEF;
            return( RC_ERROR );
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

        if( check_direct_reg( token_1, token_2 ) ) {
            if( error_msg )
                AsmError( ILLEGAL_USE_OF_REGISTER );
            token_1->type = EXPR_UNDEF;
            return( RC_ERROR );
        }
        MakeConst( token_2 );
        if( check_same( token_1, token_2, EXPR_CONST ) ) {

            token_1->value -= token_2->value;

        } else if( token_1->type == EXPR_ADDR
          && token_2->type == EXPR_CONST ) {

            token_1->value -= token_2->value;
            fix_struct_value( token_1 );

        } else if( check_same( token_1, token_2, EXPR_ADDR ) ){

            fix_struct_value( token_1 );
            fix_struct_value( token_2 );
            if( ISVALID_IDX( token_2->base_reg )
              || ISVALID_IDX( token_2->idx_reg ) ) {
                if( error_msg )
                    AsmError( ILLEGAL_USE_OF_REGISTER );
                token_1->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
            if( ISINVALID_IDX( token_2->label ) ) {
                token_1->value -= token_2->value;
                token_1->indirect |= token_2->indirect;
            } else {
                if( ISINVALID_IDX( token_1->label ) ) {
                    if( error_msg )
                        AsmError( SYNTAX_ERROR );
                    token_1->type = EXPR_UNDEF;
                    return( RC_ERROR );
                }
                sym = token_1->sym;
                if( sym == NULL )
                    return( RC_ERROR );
#if defined( _STANDALONE_ )
                if( Parse_Pass > PASS_1
                  && sym->state == SYM_UNDEFINED ) {
                    if( error_msg )
                        AsmError( LABEL_NOT_DEFINED );
                    token_1->type = EXPR_UNDEF;
                    return( RC_ERROR );
                }
                token_1->value += sym->offset;
#else
                token_1->value += sym->addr;
#endif
                sym = token_2->sym;
                if( sym == NULL )
                    return( RC_ERROR );

#if defined( _STANDALONE_ )
                if( Parse_Pass > PASS_1
                  && sym->state == SYM_UNDEFINED ) {
                    if( error_msg )
                        AsmError( LABEL_NOT_DEFINED );
                    token_1->type = EXPR_UNDEF;
                    return( RC_ERROR );
                }
                token_1->value -= sym->offset;
#else
                token_1->value -= sym->addr;
#endif
                token_1->value -= token_2->value;
                token_1->label = INVALID_IDX;
                token_1->sym = NULL;
                if( ISINVALID_IDX( token_1->base_reg )
                  && ISINVALID_IDX( token_1->idx_reg ) ) {
                    token_1->type = EXPR_CONST;
                    token_1->indirect = false;
                } else {
                    token_1->type = EXPR_ADDR;
                    token_1->indirect |= token_2->indirect;
                }
                token_1->explicit = false;
                token_1->mem_type = MT_EMPTY;
            }

        } else if( token_1->type == EXPR_REG
          && token_2->type == EXPR_CONST ) {

            token_1->value = -token_2->value;
            token_1->indirect |= token_2->indirect;
            token_1->type = EXPR_ADDR;

        } else {
            /* Error */
            if( error_msg )
                AsmError( SUBTRACTION_CONSTANT_EXPECTED );
            token_1->type = EXPR_UNDEF;
            return( RC_ERROR );
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
        if( ISVALID_IDX( token_2->override ) ) {
            /* Error */
            if( error_msg )
                AsmError( MORE_THAN_ONE_OVERRIDE );
            token_1->type = EXPR_UNDEF;
            return( RC_ERROR );
        }

        if( token_1->type == EXPR_REG ) {

            if( ISVALID_IDX( token_1->base_reg )
              && ISVALID_IDX( token_1->idx_reg ) ) {
                if( error_msg )
                    AsmError( ILLEGAL_USE_OF_REGISTER );
                token_1->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
            /* Quick fix for MOVS/CMPS/SCAS. These instruction do not allow
             * a segment override for the the destination. If ES is used for
             * the [E]DI operand, ignore it; actual overrides are rejected.
             */
            switch( Code->info.token ) {
            case T_MOVS:
            case T_CMPS:
            case T_SCAS:
                if( ISVALID_IDX( token_2->base_reg ) ) {
                    reg_token = tokbuf->tokens[token_2->base_reg].u.token;
                    if( (reg_token == T_DI)
                      || (reg_token == T_EDI) ) {
                        if( ISVALID_IDX( token_1->base_reg )
                          && tokbuf->tokens[token_1->base_reg].u.token == T_ES ) {
                            token_1->base_reg = token_2->override;
                            break;
                        }
                        if( error_msg )
                            AsmError( ILLEGAL_USE_OF_REGISTER );
                        token_1->type = EXPR_UNDEF;
                        return( RC_ERROR );
                    }
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
            && token_1->override == INVALID_IDX
            && token_1->instr == T_NULL
            && token_1->value == 0
            && token_1->base_reg == INVALID_IDX
            && token_1->idx_reg == INVALID_IDX ) {

            sym = token_1->sym;
            if( sym == NULL )
                return( RC_ERROR );

#if defined( _STANDALONE_ )
            if( tokbuf->tokens[token_1->label].class == TC_RES_ID ) {
                /* Kludge for "FLAT" */
                tokbuf->tokens[token_1->label].class = TC_ID;
            }
            if( sym->state == SYM_GRP
              || sym->state == SYM_SEG ) {
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
                return( RC_ERROR );
            }
#else
            if( error_msg )
                AsmError( ONLY_SEG_OR_GROUP_ALLOWED );
            token_1->type = EXPR_UNDEF;
            return( RC_ERROR );
#endif
        } else {
            if( error_msg )
                AsmError( REG_OR_LABEL_EXPECTED_IN_OVERRIDE );
            token_1->type = EXPR_UNDEF;
            return( RC_ERROR );
        }
        break;
    case TC_RES_ID:
        switch( tokbuf->tokens[index].u.token ) {
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
            if( ( ( tokbuf->tokens[index + 1].class != TC_RES_ID )
              || ( tokbuf->tokens[index + 1].u.token != T_PTR ) )
              && ( (Options.mode & MODE_IDEAL) == 0 ) ) {
#else
            if( ( tokbuf->tokens[index + 1].class != TC_RES_ID )
              || ( tokbuf->tokens[index + 1].u.token != T_PTR ) ) {
#endif
                // Missing PTR operator
                if( error_msg )
                    AsmError( MISSING_PTR_OPERATOR );
                token_1->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
            TokenAssign( token_1, token_2 );
            token_1->explicit = true;
            switch( tokbuf->tokens[index].u.token ) {
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
                asm_token   token = tokbuf->tokens[index - 1].u.token;

                if( tokbuf->tokens[index - 1].class != TC_RES_ID )
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
                    token_1->explicit = true;
                    if( token_1->instr == T_OFFSET )
                        token_1->instr = T_NULL;
                    break;
                default:
                    // find 'ptr' but no 'byte', 'word' etc in front of it
                    if( error_msg )
                        AsmError( NO_SIZE_GIVEN_BEFORE_PTR_OPERATOR );
                    token_1->type = EXPR_UNDEF;
                    return( RC_ERROR );
                }
            }
            break;
        case T_SHORT:
            TokenAssign( token_1, token_2 );
            token_1->explicit = true;
            token_1->mem_type = MT_SHORT;
            break;
        }
        break;
    case TC_INSTR:
        MakeConst( token_1 );
        MakeConst( token_2 );
        if( tokbuf->tokens[index].u.token == T_NOT ) {
            if( token_2->type != EXPR_CONST ) {
                if( error_msg )
                    AsmError( CONSTANT_EXPECTED );
                token_1->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
            token_1->type = EXPR_CONST;
        } else {
            if( !check_same( token_1, token_2, EXPR_CONST ) ) {
                if( error_msg )
                    AsmError( CONSTANT_EXPECTED );
                token_1->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
        }
        switch( tokbuf->tokens[index].u.token ) {
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
        if( ( tokbuf->tokens[index].u.token == T_OFFSET )
          && ( token_2->type == EXPR_CONST ) ) {
        } else if( token_2->type != EXPR_ADDR ) {
            if( error_msg )
                AsmError( LABEL_IS_EXPECTED );
            token_1->type = EXPR_UNDEF;
            return( RC_ERROR );
        } else if( token_2->instr != T_NULL ) {
            if( error_msg )
                AsmError( LABEL_IS_EXPECTED );
            token_1->type = EXPR_UNDEF;
            return( RC_ERROR );
        }
#if defined( _STANDALONE_ )
        switch( tokbuf->tokens[index].u.token ) {
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
                return( RC_ERROR );
            }
            switch( tokbuf->tokens[index].u.token ) {
            case T_LENGTH:
                if( sym->mem_type == MT_STRUCT ) {
                    token_1->value = sym->count;
                } else if( sym->mem_type == MT_EMPTY ) {
                    token_1->value = 0;
                } else {
                    token_1->value = ( sym->first_length ) ? sym->first_length : 1;
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
                        if( ((dir_node_handle)sym->segment)->e.seginfo->use32 ) {
                            token_1->value = 0xFF04;
                        } else {
                            token_1->value = 0xFF02;
                        }
                    } else {
                        token_1->value = 0xFF02;
                    }
                } else if( sym->mem_type == MT_FAR ) {
                    if( sym->segment ) {
                        if( ((dir_node_handle)sym->segment)->e.seginfo->use32 ) {
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
            if( Parse_Pass != PASS_1
              && token_1->value == 0 ) {
                if( error_msg )
                    AsmError( DATA_LABEL_IS_EXPECTED );
                token_1->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
            token_1->label = INVALID_IDX;
            token_1->sym = NULL;
            token_1->base_reg = INVALID_IDX;
            token_1->idx_reg = INVALID_IDX;
            token_1->override = INVALID_IDX;
            token_1->instr = T_NULL;
            token_1->type = EXPR_CONST;
            token_1->indirect = false;
            token_1->explicit = false;
            token_1->mem_type = MT_EMPTY;
            break;
        default:
            TokenAssign( token_1, token_2 );
            token_1->instr = tokbuf->tokens[index].u.token;
            break;
        }
#else
        TokenAssign( token_1, token_2 );
        token_1->instr = tokbuf->tokens[index].u.token;
#endif
        break;
    case TC_ARITH_OPERATOR:
        MakeConst( token_1 );
        MakeConst( token_2 );
        switch( tokbuf->tokens[index].u.token ) {
        case T_MOD:
            /*
             * The only formats allowed are:
             *        constant MOD constant
             */
            if( !check_same( token_1, token_2, EXPR_CONST ) ) {
                if( error_msg )
                    AsmError( CONSTANT_EXPECTED );
                token_1->type = EXPR_UNDEF;
                return( RC_ERROR );
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
                    token_1->base_reg = INVALID_IDX;
                    token_1->scale = (uint_8)token_1->value;
                    token_1->value = 0;
                    token_2->base_reg = INVALID_IDX;
                } else {
                    /* reg * scale */
                    token_1->idx_reg = token_1->base_reg;
                    token_1->base_reg = INVALID_IDX;
                    token_1->scale = (uint_8)token_2->value;
                }
                token_1->indirect |= token_2->indirect;
                token_1->type = EXPR_ADDR;
            } else {
                if( error_msg )
                    AsmError( MULTIPLICATION_CONSTANT_EXPECTED );
                token_1->type = EXPR_UNDEF;
                return( RC_ERROR );
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
                return( RC_ERROR );
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
            return( RC_ERROR );
        }
        switch( tokbuf->tokens[index].u.token ) {
        case T_EQ:
            token_1->value = ( token_1->value == token_2->value ) ? -1 : 0;
            break;
        case T_NE:
            token_1->value = ( token_1->value != token_2->value ) ? -1 : 0;
            break;
        case T_LT:
            token_1->value = ( token_1->value < token_2->value ) ? -1 : 0;
            break;
        case T_LE:
            token_1->value = ( token_1->value <= token_2->value ) ? -1 : 0;
            break;
        case T_GT:
            token_1->value = ( token_1->value > token_2->value ) ? -1 : 0;
            break;
        case T_GE:
            token_1->value = ( token_1->value >= token_2->value ) ? -1 : 0;
            break;
        }
        break;
#endif
    }
    token_1->empty = false;
    return( RC_OK );
}

static bool evaluate(
    expr_list *operand1,
    token_buffer *tokbuf,
    token_idx *i,
    token_idx end,
    process_flag proc_flag,
    bool (*is_expr)(token_buffer *, token_idx) )
/**********************************************/
{
    expr_list           operand2;
    bool                token_needed;
    token_idx           curr_operator;
    bool                next_operator;
    int                 op_sq_bracket;

    token_needed = false;
    curr_operator = INVALID_IDX;
    op_sq_bracket = op_sq_bracket_level;

    /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
    /* Look at first token, which may be an unary operator or an operand */
    /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

    if( operand1->empty ) {
        if( cmp_token_cls( tokbuf, *i, TC_OP_BRACKET ) ) {
            (*i)++;
            if( *i > end ) {
                if( error_msg )
                    AsmError( OPERAND_EXPECTED );
                operand1->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
            if( evaluate( operand1, tokbuf, i, end, PROC_BRACKET, is_expr ) ) {
                return( RC_ERROR );
            }
            if( cmp_token_cls( tokbuf, *i, TC_CL_SQ_BRACKET ) ) {
                // error open ( close ]
                if( error_msg )
                    AsmError( BRACKETS_NOT_BALANCED );
                operand1->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
            (*i)++;
        } else if( is_unary( tokbuf, *i, true ) ) {
            token_needed = true;
        } else if( cmp_token_cls( tokbuf, *i, TC_OP_SQ_BRACKET ) ) {
            if( *i == 0 ) {
                return( RC_ERROR );
            }
            /**/myassert( !cmp_token_cls( tokbuf, (*i) - 1, TC_CL_BRACKET ) );
            (*i)++;
            if( *i > end ) {
                if( error_msg )
                    AsmError( OPERAND_EXPECTED );
                operand1->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
            op_sq_bracket_level++;
            if( evaluate( operand1, tokbuf, i, end, PROC_BRACKET, is_expr ) ) {
                return( RC_ERROR );
            }
            if( cmp_token_cls( tokbuf, *i, TC_CL_BRACKET ) ) {
                // error open [ close )
                if( error_msg )
                    AsmError( BRACKETS_NOT_BALANCED );
                operand1->type = EXPR_UNDEF;
                return( RC_ERROR );
            }
            if( cmp_token_cls( tokbuf, *i, TC_CL_SQ_BRACKET ) ) {
                op_sq_bracket_level--;
            }
            (*i)++;
        } else if( get_operand( operand1, tokbuf, i, end, is_expr ) ) {
            return( RC_ERROR );
        }
    } else {
        token_needed = false;
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
                return( RC_ERROR );
            } else {
                return( RC_OK );
            }
        }
        /* Read the operator */
        if( cmp_token_cls( tokbuf, *i, TC_CL_BRACKET ) ) {
            if( op_sq_bracket_level != op_sq_bracket ) {
                // error close ) but [ is open
                if( error_msg )
                    AsmError( BRACKETS_NOT_BALANCED );
                operand1->type = EXPR_UNDEF;
                return( RC_ERROR );
            } else {
                return( RC_OK );
            }
        } else if( cmp_token_cls( tokbuf, *i, TC_CL_SQ_BRACKET ) ) {
            return( RC_OK );
        } else if( cmp_token_cls( tokbuf, *i, TC_OP_SQ_BRACKET ) ) {
            tokbuf->tokens[*i].class = TC_PLUS;
            op_sq_bracket_level++;
        } else if( !is_optr( tokbuf, *i ) ) {
            if( error_msg )
                AsmError( OPERATOR_EXPECTED );
            operand1->type = EXPR_UNDEF;
            return( RC_ERROR );
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
            return( RC_ERROR );
        }

        init_expr( &operand2 );
        if( cmp_token_cls( tokbuf, *i, TC_OP_BRACKET ) ) {
            (*i)++;
            if( evaluate( &operand2, tokbuf, i, end, PROC_BRACKET, is_expr ) ) {
                return( RC_ERROR );
            }
            if( cmp_token_cls( tokbuf, *i, TC_CL_BRACKET ) ) {
                (*i)++;
            }
        } else if( cmp_token_cls( tokbuf, *i, TC_OP_SQ_BRACKET ) ) {
            op_sq_bracket_level++;
            (*i)++;
            if( evaluate( &operand2, tokbuf, i, end, PROC_BRACKET, is_expr ) ) {
                return( RC_ERROR );
            }
        } else if( is_unary( tokbuf, *i, true ) ) {
            if( evaluate( &operand2, tokbuf, i, end, PROC_OPERAND, is_expr ) ) {
                return( RC_ERROR );
            }
        } else if( is_optr( tokbuf, *i ) ) {
            if( error_msg )
                AsmError( OPERAND_EXPECTED );
            operand1->type = EXPR_UNDEF;
            return( RC_ERROR );
        } else if( get_operand( &operand2, tokbuf, i, end, is_expr ) ) {
            return( RC_ERROR );
        }

        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
        /* Close all available brackets                                    */
        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

        while( ( *i <= end )
            && ( op_sq_bracket_level > op_sq_bracket )
            && cmp_token_cls( tokbuf, *i, TC_CL_SQ_BRACKET ) ) {
            (*i)++;
            op_sq_bracket_level--;
        }

        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
        /* Look at the next operator and compare its priority with 1st one */
        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

        next_operator = false;
        if( *i <= end ) {
            if( !is_optr( tokbuf, *i )
              || is_unary( tokbuf, *i, false )
              || cmp_token_cls( tokbuf, *i, TC_OP_BRACKET ) ) {
                if( error_msg )
                    AsmError( OPERATOR_EXPECTED );
                operand1->type = EXPR_UNDEF;
                return( RC_ERROR );
            } else if( !cmp_token_cls( tokbuf, *i, TC_CL_BRACKET )
              && !cmp_token_cls( tokbuf, *i, TC_CL_SQ_BRACKET ) ) {
                if( cmp_token_cls( tokbuf, *i, TC_OP_SQ_BRACKET ) ) {
                    if( ( PLUS_PRECEDENCE <= get_precedence( tokbuf, curr_operator ) )
                        || ( proc_flag == PROC_BRACKET ) ) {
                        tokbuf->tokens[*i].class = TC_PLUS;
                        op_sq_bracket_level++;
                        next_operator = true;
                    }
                } else {
                    if( get_precedence( tokbuf, *i ) < get_precedence( tokbuf, curr_operator ) ) {
                        if( evaluate( &operand2, tokbuf, i, end, PROC_OPERAND, is_expr ) )
                            return( RC_ERROR );
                        while( ( *i <= end )
                            && ( op_sq_bracket_level > op_sq_bracket )
                            && cmp_token_cls( tokbuf, *i, TC_CL_SQ_BRACKET ) ) {
                            (*i)++;
                            op_sq_bracket_level--;
                        }
                        if( cmp_token_cls( tokbuf, *i, TC_OP_SQ_BRACKET ) ) {
                            if( proc_flag == PROC_BRACKET ) {
                                tokbuf->tokens[*i].class = TC_PLUS;
                                op_sq_bracket_level++;
                                next_operator = true;
                            }
                        }
                    } else if( proc_flag == PROC_BRACKET ) {
                        next_operator = true;
                    }
                }
            }
        }

        /*::::::::::::::*/
        /* Now evaluate */
        /*::::::::::::::*/

        if( calculate( operand1, &operand2, tokbuf, curr_operator ) ) {
            return( RC_ERROR );
        }

    } while( next_operator
          || ( ( proc_flag == PROC_BRACKET )
            && !cmp_token_cls( tokbuf, *i, TC_CL_BRACKET )
            && !cmp_token_cls( tokbuf, *i, TC_CL_SQ_BRACKET )
            && ( *i < end ) ) );
    if( op_sq_bracket_level != op_sq_bracket ) {
        if( error_msg )
            AsmError( BRACKETS_NOT_BALANCED );
        operand1->type = EXPR_UNDEF;
        return( RC_ERROR );
    }
    return( RC_OK );
}

static bool is_expr1( token_buffer *tokbuf, token_idx i )
/********************************************************
 * Check if the token is part of an expression
 */
{
    switch( tokbuf->tokens[i].class ) {
    case TC_INSTR:
        switch( tokbuf->tokens[i].u.token ) {
        case T_SHL:
        case T_SHR:
        case T_NOT:
        case T_AND:
        case T_OR:
        case T_XOR:
            if( i == 0 ) {
                /* It is an instruction instead */
                break;
            } else if( tokbuf->tokens[i-1].class == TC_COLON ) {
                /* It is an instruction instead */
                break;
            } else if( tokbuf->tokens[i-1].u.token == T_LOCK ) {
                /* It is an instruction:
                         lock and dword ptr [ebx], 1
                */
                break;
            } else {
                return( true );
            }
        default:
            break;
        }
        break;
    case TC_UNARY_OPERATOR:
//        case T_SEG:
//        case T_OFFSET:
        if( i + 1 < TokCnt )
            return( true );
        break;
    case TC_ARITH_OPERATOR:
        return( true );
#if defined( _STANDALONE_ )
    case TC_RELATION_OPERATOR:
        return( true );
    case TC_RES_ID:
        switch( tokbuf->tokens[i].u.token ) {
        case T_FLAT:
            DefFlatGroup();
            return( true );
        }
        break;
#endif
    case TC_REG:
        return( true );
    case TC_PLUS:
    case TC_MINUS:
        /* hack to stop asmeval from hanging on floating point numbers */
        if( tokbuf->tokens[i+1].class == TC_FLOAT )
            break;
        return( true );
    case TC_NUM:
    case TC_OP_BRACKET:
    case TC_CL_BRACKET:
    case TC_OP_SQ_BRACKET:
    case TC_CL_SQ_BRACKET:
        return( true );
    case TC_COLON:
#if defined( _STANDALONE_ )
        if( i == 1
          || ( tokbuf->tokens[i+1].class == TC_DIRECTIVE
          && tokbuf->tokens[i+1].u.token == T_EQU2 ) ) {
            /* It is the colon following the label or it is a := */
            break;
        } else {
            return( true );
        }
#else
        return( true );
#endif
    case TC_ID:
        if( i != 0 )
            /* It is not a label */
            return( true );
        break;
    case TC_STRING:
        return( true );
    case TC_DOT:
        return( true );
    case TC_PATH:
    default:
        break;
    }
    return( false );
}

static bool is_expr2( token_buffer *tokbuf, token_idx i )
/*******************************************************/
/* Check if the token is part of an expression */
{
    switch( tokbuf->tokens[i].class ) {
    case TC_INSTR:
        switch( tokbuf->tokens[i].u.token ) {
        case T_SHL:
        case T_SHR:
        case T_NOT:
        case T_AND:
        case T_OR:
        case T_XOR:
            if( i == 0 ) {
                /* It is an instruction instead */
                break;
            } else if( tokbuf->tokens[i-1].class == TC_COLON ) {
                /* It is an instruction instead */
                break;
            } else if( tokbuf->tokens[i-1].u.token == T_LOCK ) {
                /* It is an instruction:
                         lock and dword ptr [ebx], 1
                */
                break;
            } else {
                return( true );
            }
        default:
            break;
        }
        break;
    case TC_UNARY_OPERATOR:
        return( true );
    case TC_ARITH_OPERATOR:
        return( true );
    case TC_RELATION_OPERATOR:
        return( true );
    case TC_RES_ID:
        switch( tokbuf->tokens[i].u.token ) {
#if defined( _STANDALONE_ )
        case T_FLAT:
            DefFlatGroup();
            return( true );
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
            return( true );
        default:
            tokbuf->tokens[i].class = TC_ID;
            return( true );
        }
        break;
    case TC_REG:
        return( true );
    case TC_PLUS:
    case TC_MINUS:
        /* hack to stop asmeval from hanging on floating point numbers */
        if( tokbuf->tokens[i+1].class == TC_FLOAT )
            break;
        return( true );
    case TC_NUM:
    case TC_OP_BRACKET:
    case TC_CL_BRACKET:
    case TC_OP_SQ_BRACKET:
    case TC_CL_SQ_BRACKET:
        return( true );
    case TC_COLON:
#if defined( _STANDALONE_ )
        if( ( tokbuf->tokens[i+1].class == TC_DIRECTIVE )
            && ( tokbuf->tokens[i+1].u.token == T_EQU2 ) )
            /* It is a := */
            break;
#endif
        return( true );
    case TC_ID:
        return( true );
    case TC_STRING:
        return( true );
    case TC_DOT:
        return( true );
    case TC_PATH:
    default:
        break;
    }
    return( false );
}

static bool fix_parens( token_buffer *tokbuf )
/*********************************************
 * Take out those brackets which may surround a non-expression, e.g.
 * Right now only 'dup' requires a pair of parentheses, which should be
 * taken out temporarily
 */
{
    token_idx   i;
    int         store;
    int         dup_count = 0;
    int         non_dup_bracket_count = 0;

    store = 0;
    for( i = 0; i < TokCnt; i++ ) {

        if( tokbuf->tokens[i].class == TC_RES_ID
          && tokbuf->tokens[i].u.token == T_DUP ) {
            dup_count++;
            if( tokbuf->tokens[++i].class != TC_OP_BRACKET ) {
                if( error_msg )
                    AsmError( BRACKET_EXPECTED );
                return( RC_ERROR );
            }
            TakeOut[store].idx = i;               // Store the location
            TakeOut[store++].close_bracket = false;
            tokbuf->tokens[i].class = TC_NOOP;
        } else if( tokbuf->tokens[i].class == TC_OP_BRACKET ) {
            non_dup_bracket_count++;
        } else if( tokbuf->tokens[i].class == TC_CL_BRACKET ) {
            if( non_dup_bracket_count ) {
                non_dup_bracket_count--;
            } else {
                dup_count--;
                TakeOut[store].idx = i;  // store close brackets as -ve values
                TakeOut[store++].close_bracket = true;
                tokbuf->tokens[i].class = TC_NOOP;
            }
        }
    }
    if( dup_count != 0 ) {
        if( error_msg )
            AsmError( BRACKETS_NOT_BALANCED );
        return( RC_ERROR );
    }
    TakeOut[store].idx = INVALID_IDX;        // Mark the end
    TakeOut[store].close_bracket = false;
    return( RC_OK );
}

static token_idx fix( expr_list *res, token_buffer *tokbuf, token_idx start, token_idx end )
/*******************************************************************************************
 * Convert the result in res into tokens and put them back in tokbuf->tokens[]
 */
{
    token_idx           size = 0;
    token_idx           diff;
    token_idx           i;
    token_idx           old_start;
//    int                 old_end;
    bool                need_number;

    MakeConst( res );
    if( res->type == EXPR_CONST ) {
        if( ISVALID_IDX( res->override ) ) {
            tokbuf->tokens[start++] = tokbuf->tokens[res->override];
            tokbuf->tokens[start++].class = TC_COLON;
        }
        if( res->string == NULL ) {
            tokbuf->tokens[ start ].class = TC_NUM;
            tokbuf->tokens[ start ].u.value = res->value;
            tokbuf->tokens[ start++ ].string_ptr = "";
        } else {
            tokbuf->tokens[ start ].class = TC_STRING;
            tokbuf->tokens[ start++ ].string_ptr = res->string;
        }

    } else if( res->type == EXPR_REG
      && !res->indirect ) {

        if( ISVALID_IDX( res->override ) ) {
            tokbuf->tokens[start++] = tokbuf->tokens[res->override];
            tokbuf->tokens[start++].class = TC_COLON;
            tokbuf->tokens[ start++ ].class = TC_OP_SQ_BRACKET;
        }
        tokbuf->tokens[ start ].class = TC_REG;
        tokbuf->tokens[ start ].string_ptr = tokbuf->tokens[ res->base_reg ].string_ptr;
        tokbuf->tokens[ start++ ].u.value = tokbuf->tokens[ res->base_reg ].u.value;
        if( tokbuf->tokens[ res->base_reg ].u.token == T_ST
          && (int)res->idx_reg > 0 ) {
//            tokbuf->tokens[start].string_ptr = "(";
//            tokbuf->tokens[start++].class = TC_OP_BRACKET;
            tokbuf->tokens[start].class = TC_NUM;
            tokbuf->tokens[start].u.value = (int)res->idx_reg;
            tokbuf->tokens[start++].string_ptr = "";
//            tokbuf->tokens[start].string_ptr = ")";
//            tokbuf->tokens[start++].class = TC_CL_BRACKET;
        }
        if( ISVALID_IDX( res->override ) ) {
            tokbuf->tokens[ start++ ].class = TC_CL_SQ_BRACKET;
        }

    } else {

        if( res->instr != T_NULL ) {
            size++;
        } else if( res->mbr != NULL
          && res->mbr->mem_type != MT_EMPTY ) {
            size += 2;
        }

        if( res->type != EXPR_REG ) {
            size++;
        }

        if( ISVALID_IDX( res->override ) ) {
            size += 2;
        }
        need_number = true;
        if( res->scale != 1 ) {
            size += 2;          // [ reg * 2 ] == 2 tokens more than [ reg ]
        }
        if( ISVALID_IDX( res->base_reg ) ) {
            size += 3;                  // e.g. [ ax ] == 3 tokens
            need_number = false;
        }
        if( ISVALID_IDX( res->idx_reg ) ) {
            size += 3;                  // e.g. [ ax ] == 3 tokens
            need_number = false;
        }
        if( ISVALID_IDX( res->label ) ) {
            need_number = false;
        }
        if( res->value != 0 ) {
            need_number = true;
        }
        if( need_number ) {
            size += 3;                  // [ value ] == 3 tokens
        }

        old_start = start;
//        old_end = end;

        for( i = start; i <= end; i++ ) {
            /* Store the original tokbuf->tokens[] data */
            Store[i - start] = tokbuf->tokens[i];
        }

        if( start + 1 + size > end ) {
            diff = start + 1 + size - end;

            for( i = TokCnt - 1; i > end; i-- ) {
                tokbuf->tokens[i + diff] = tokbuf->tokens[i];
            }

            for( i = 0; ISVALID_IDX( TakeOut[i].idx ); i++ ) {
                if( TakeOut[i].idx > end ) {
                    TakeOut[i].idx += diff;
                }
            }

            end += diff;
            TokCnt += diff;
        }

        if( res->instr != T_NULL ) {
            tokbuf->tokens[start].class = TC_UNARY_OPERATOR;
            tokbuf->tokens[start++].u.token = res->instr;
        } else if( res->mbr != NULL
          && res->mbr->mem_type != MT_EMPTY ) {
            tokbuf->tokens[start].class = TC_RES_ID;
            switch( res->mbr->mem_type ) {
            case MT_BYTE:
                tokbuf->tokens[start++].u.token = T_BYTE;
                break;
            case MT_WORD:
                tokbuf->tokens[start++].u.token = T_WORD;
                break;
            case MT_DWORD:
                tokbuf->tokens[start++].u.token = T_DWORD;
                break;
            case MT_FWORD:
                tokbuf->tokens[start++].u.token = T_FWORD;
                break;
            case MT_QWORD:
                tokbuf->tokens[start++].u.token = T_QWORD;
                break;
            case MT_TBYTE:
                tokbuf->tokens[start++].u.token = T_TBYTE;
                break;
            case MT_OWORD:
                tokbuf->tokens[start++].u.token = T_OWORD;
                break;
            case MT_SHORT:
                tokbuf->tokens[start++].u.token = T_SHORT;
                break;
            case MT_NEAR:
                tokbuf->tokens[start++].u.token = T_NEAR;
                break;
            case MT_FAR:
                tokbuf->tokens[start++].u.token = T_FAR;
                break;
#if defined( _STANDALONE_ )
            case MT_SBYTE:
                tokbuf->tokens[start++].u.token = T_SBYTE;
                break;
            case MT_SWORD:
                tokbuf->tokens[start++].u.token = T_SWORD;
                break;
            case MT_SDWORD:
                tokbuf->tokens[start++].u.token = T_SDWORD;
                break;
#endif
            default:
                break;
            }
//            tokbuf->tokens[start++].value = res->mbr->mem_type;
            tokbuf->tokens[start].class = TC_RES_ID;
            tokbuf->tokens[start++].u.token = T_PTR;
        }

        if( ISVALID_IDX( res->override ) ) {
            tokbuf->tokens[start++] = Store[res->override - old_start];
            tokbuf->tokens[start++].class = TC_COLON;
        }

        if( ISVALID_IDX( res->label )
          && res->type != EXPR_REG ) {
            tokbuf->tokens[start++] = Store[res->label - old_start];
        }

        if( ISVALID_IDX( res->base_reg ) ) {
            tokbuf->tokens[start++].class = TC_OP_SQ_BRACKET;
            tokbuf->tokens[start].class = TC_REG;
            tokbuf->tokens[start].string_ptr = Store[res->base_reg-old_start].string_ptr;
            tokbuf->tokens[start++].u.value = Store[res->base_reg-old_start].u.value;
            tokbuf->tokens[start++].class = TC_CL_SQ_BRACKET;
        }
        if( ISVALID_IDX( res->idx_reg ) ) {
            tokbuf->tokens[start++].class = TC_OP_SQ_BRACKET;
            tokbuf->tokens[start].class = TC_REG;
            tokbuf->tokens[start].string_ptr = Store[res->idx_reg-old_start].string_ptr;
            tokbuf->tokens[start++].u.value = Store[res->idx_reg-old_start].u.value;
            if( res->scale != 1 ) {
                tokbuf->tokens[start].string_ptr = "*";
                tokbuf->tokens[start].u.token = T_OP_TIMES;
                tokbuf->tokens[start++].class = TC_ARITH_OPERATOR;
                tokbuf->tokens[start].class = TC_NUM;
                tokbuf->tokens[start].u.value = res->scale;
                tokbuf->tokens[start++].string_ptr = "";
                res->scale = 1;
            }
            tokbuf->tokens[start++].class = TC_CL_SQ_BRACKET;
        }

        if( need_number ) {
            tokbuf->tokens[start++].class = TC_OP_SQ_BRACKET;
            tokbuf->tokens[start].class = TC_NUM;
            tokbuf->tokens[start].u.value = res->value;
            tokbuf->tokens[start++].string_ptr = "";
            tokbuf->tokens[start++].class = TC_CL_SQ_BRACKET;
        }

        /**/myassert( ( start-1 ) <= end );
    }

    for( i = start; i <= end; i++ ) {
        tokbuf->tokens[i].class = TC_NOOP;
    }
    return( end );
}

static void fix_final( token_buffer *tokbuf )
/********************************************
 * Put back those brackets taken out by fix_parens() and take out all TC_NOOP tokens
 */
{
    token_idx   start;
    token_idx   end;
    token_idx   i;
    token_idx   diff;
    int         dup_count = 0;

    for( i = 0;; i++ ) {
        if( dup_count == 0
          && ISINVALID_IDX( TakeOut[i].idx ) )
            break;
        if( TakeOut[i].close_bracket ) {
            tokbuf->tokens[TakeOut[i].idx].class = TC_CL_BRACKET;
            dup_count--;
        } else {
            tokbuf->tokens[TakeOut[i].idx].class = TC_OP_BRACKET;
            dup_count++;
        }
        myassert( i < MAX_TOKEN );
    }

    for( start = 0; start < TokCnt; start++ ) {
        if( tokbuf->tokens[start].class == TC_NOOP ) {
            for( end = start + 1;; end++ ) {
                if( tokbuf->tokens[end].class != TC_NOOP
                  || end == TokCnt ) {
                    break;
                }
            }
            if( end == TokCnt ) {
                TokCnt = start;
                return;
            } else {
                diff = end - start;
                for( i = end; i < TokCnt; i++ ) {
                    tokbuf->tokens[i - diff] = tokbuf->tokens[i];
                }
            }
            TokCnt -= diff;
        }
    }
}

token_idx EvalExpr( token_buffer *tokbuf, token_idx start_tok, token_idx end_tok, bool flag_msg )
/***********************************************************************************************/
{
    token_idx   i = start_tok;
    token_idx   start;          // position of first token of an expression
    token_idx   num;            // number of tokens in the expression
    bool        final = false;
    expr_list   result;
    token_idx   count;

    if( tokbuf->tokens[end_tok].class == TC_FINAL )
        final = true;

    TokCnt = count = tokbuf->count;

    if( fix_parens( tokbuf ) ) {
        // take out those parentheses which are not part of an expression
        return( INVALID_IDX );
    }

    while( i < TokCnt && i <= end_tok ) {
        if( is_expr1( tokbuf, i ) ) {
            start = i++;
            num = 0;
            for( ;; ) {
                if( i >= TokCnt )
                    break;
                if( i > end_tok )
                    break;
                if( !is_expr1( tokbuf, i ) )
                    break;
                i++;
                num++;
            }
            if( num == 1
              && tokbuf->tokens[start].class == TC_REG
              && tokbuf->tokens[start+1].class == TC_COLON ) {
                // Massive kludge ahead:
                // skip register+colon
            } else if( num == 0
              && tokbuf->tokens[start].class == TC_REG ) {
                // skip register
            } else if( num == 0
              && tokbuf->tokens[start].class == TC_NUM ) {
                // skip number
            } else {
                i = start;
                init_expr( &result );
                op_sq_bracket_level = 0;
                error_msg = flag_msg;
                if( evaluate( &result, tokbuf, &i, i + num, PROC_BRACKET, is_expr1 ) ) {
                    return( INVALID_IDX );
                }
                i = fix( &result, tokbuf, start, start + num );
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
    fix_final( tokbuf );

    if( tokbuf->tokens[TokCnt].class == TC_NOOP
      || final ) {
        tokbuf->tokens[TokCnt].class = TC_FINAL;
        tokbuf->tokens[TokCnt].string_ptr = NULL;
    }

    return( TokCnt );
}

bool EvalOperand( token_buffer *tokbuf, token_idx *start_tok, token_idx count, expr_list *result, bool flag_msg )
/***************************************************************************************************************/
{
    token_idx   i = *start_tok;
    token_idx   num;            // number of tokens in the expression
    bool        rc;

    init_expr( result );
    if( tokbuf->tokens[i].class == TC_FINAL )
        return( RC_OK );
    if( !is_expr2( tokbuf, i ) )
        return( RC_OK );

    num = 0;
    for( ;; ) {
        i++;
        if( i >= count )
            break;
        if( !is_expr2( tokbuf, i ) )
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
    rc = evaluate( result, tokbuf, start_tok, *start_tok + num, PROC_BRACKET, is_expr2 );
#if defined( _STANDALONE_ )
    if( Options.mode & MODE_IDEAL ) {
        Definition.struct_depth = 0;
    }
#endif
    return( rc );
}

#if 0
static bool is_expr_const( token_idx i )
/********************************/
/* Check if the token is part of constant expression */
{
    switch( tokbuf->tokens[i].class ) {
    case TC_INSTR:
        switch( tokbuf->tokens[i].u.token ) {
        case T_SHL:
        case T_SHR:
        case T_NOT:
        case T_AND:
        case T_OR:
        case T_XOR:
            if( i == 0 ) {
                /* It is an instruction instead */
                return( false );
            } else if( tokbuf->tokens[i-1].class == TC_COLON ) {
                /* It is an instruction instead */
                return( false );
            } else if( tokbuf->tokens[i-1].u.token == T_LOCK ) {
                /* It is an instruction:
                         lock and dword ptr [ebx], 1
                */
                return( false );
            } else {
                return( true );
            }
        default:
            return( false );
        }
    case TC_ARITH_OPERATOR:
        return( true );
    case TC_RELATION_OPERATOR:
        return( true );
    case TC_PLUS:
    case TC_MINUS:
        /* hack to stop asmeval from hanging on floating point numbers */
        if( tokbuf->tokens[i+1].class == TC_FLOAT )
            return( false );
    case TC_NUM:
    case TC_OP_BRACKET:
    case TC_CL_BRACKET:
    case TC_OP_SQ_BRACKET:
    case TC_CL_SQ_BRACKET:
        return( true );
    case TC_ID:
        if( i == 0 ) {
            /* It is a label */
            return( false );
        } else {
            return( true );
        }
    case TC_STRING:
        return( true );
    default:
        return( false );
    }
}
#endif

#if defined( _STANDALONE_ )

token_idx EvalConstant( token_buffer *tokbuf, token_idx start_tok, token_idx end_tok, bool flag_msg )
/***************************************************************************************************/
{
    token_idx   i;
//    bool        const_expr = true;

    TokCnt = tokbuf->count;
    error_msg = flag_msg;
    for( i = start_tok; i < TokCnt && i <= end_tok; ++i ) {
        if( !is_expr1( tokbuf, i ) ) {
//            const_expr = false;
            break;
        }
    }
    return( TokCnt );
}

#endif
