/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "asmglob.h"
#if defined( _STANDALONE_ )
#include "directiv.h"
#endif
#include <ctype.h>


typedef union {
    float   f;
    long    l;
} NUMBERFL;

#ifdef DEBUG_OUT
const char              *CurrString; // Current Input Line
#endif

#if defined( _STANDALONE_ )

bool                    EnumDirective;

#endif

static unsigned         radix_base = 10;

static bool get_float( asm_tok *tok, const char **input, char **output )
/**********************************************************************/
{
    /* valid floats look like:  (int)[.(int)][e(int)][r] */

    bool            got_decimal;
    bool            got_e;
    const char      *ptr;
    size_t          len;
    int             c;
    unsigned char   extra;

    extra = 0;
    got_e = false;
    got_decimal = false;
    for( ptr = *input; (c = *ptr) != '\0'; ptr++ ) {
        if( isdigit( c ) )
            continue;
        if( isspace( c ) )
            break;
        if( c == '.' ) {
            if( got_decimal || got_e )
                break;
            got_decimal = true;
            continue;
        }
        c = tolower( c );
        if( c == 'r' ) {
            extra = 1;
            break;
        }
        if( c != 'e' || got_e )
            break;
        // c == 'e'
        got_e = true;
        /* accept e+2 / e-4 /etc. */
        c = *(ptr + 1);
        if( c == '+' || c == '-' ) {
            ptr++;
        }
    }
    tok->class = TC_FLOAT;
    /* copy the string, fix input & output pointers */
    tok->string_ptr = *output;
    len = ptr - *input + extra;
    memcpy( *output, *input, len );
    *output += len;
    *(*output)++ = '\0';
    *input = ptr + extra;

    tok->u.float_value = (float)atof( tok->string_ptr );
    return( RC_OK );
}

static void array_mul_add( unsigned char *buf, unsigned base, const char *dig, unsigned size )
{
    unsigned long   num;

    if( isdigit( *dig ) ) {
        num = *dig - '0';
    } else {
        num = tolower( *dig ) - 'a' + 10;
    }
    while( size-- > 0 ) {
        num = num + *buf * base;
        *buf = (unsigned char)num;
        num >>= 8;
        buf++;
    }
}

static bool get_string( asm_tok *tok, const char **input, char **output )
/***********************************************************************/
{
    char    symbol_o;
    char    symbol_c;
    int     count;
    int     level;

    tok->string_ptr = *output;

    symbol_o = **input;

    tok->class = TC_STRING;
    switch( symbol_o ) {
    case '"':
    case '\'':
        symbol_c = 0;
        break;  // end of string marker is the same
    case '<':
        symbol_c = '>';
        break;
    case '{':
        symbol_c = '}';
        break;
    default:
        /* this is an undelimited string,
         * so just copy it until we hit something that looks like the end
         */

        for( count = 0; **input != '\0' && !isspace( **input ) && **input != ','; count++ ) {
            *(*output)++ = *(*input)++; /* keep the 2nd one */
        }
        *(*output)++ = '\0';
        tok->u.value = count;
        return( RC_OK );
    }
    (*input)++;
    count = 0;
    level = 0;
    for( count = 0; count < MAX_TOK_LEN; count++ ) {
        if( **input == symbol_o ) {
            if( symbol_c ) {
                level++;
            } else if( *( *input + 1 ) == symbol_o ) {
                /* if we see "" in a " delimited string,
                 * treat it as a literal " */
                (*input)++; /* skip the 1st one and keep the 2nd one */
            } else {
                (*input)++; /* skip the closing delimiter */
                break;
            }
        } else if( symbol_c && **input == symbol_c ) {
            if( level ) {
                level--;
            } else {
                (*input)++; /* skip the closing delimiter */
                break;
            }
        } else if( **input == '\0' || **input == '\n' ) {
            *(*output)++ = '\0';
            AsmError( SYNTAX_ERROR );
            return( RC_ERROR );
        }
        *(*output)++ = *(*input)++;
    }
    *(*output)++ = '\0';
    tok->u.value = count;
    return( RC_OK );
}

static bool get_number( asm_tok *tok, const char **input, char **output, bool base10 )
/************************************************************************************/
{
    const char          *ptr;
    const char          *dig_start;
    bool                first_char_0;
    unsigned char       extra;
    size_t              len;
    unsigned            base = 0;
    unsigned            digits_seen;
    int                 c;
    int                 c2;

#define VALID_BINARY    0x0003
#define VALID_OCTAL     0x00ff
#define VALID_DECIMAL   0x03ff
#define OK_NUM( t )     ((digits_seen & ~VALID_##t) == 0)

    digits_seen = 0;
    first_char_0 = false;
    extra = 0;
    ptr = *input;
    if( *(unsigned char *)ptr == '0' ) {
        ++ptr;
        if( tolower( *(unsigned char *)ptr ) == 'x' ) {
            ++ptr;
            base = 16;
        } else {
            first_char_0 = true;
        }
    }
    dig_start = ptr;
    for( ; (c = *(unsigned char *)ptr) != '\0'; ++ptr ) {
        if( isdigit( c ) ) {
            digits_seen |= 1 << (c - '0');
            continue;
        } else if( isspace( c ) ) {
            break;
        } else if( c == '.' ) {
            /* note that a float MUST contain a dot
             * OR be ended with an "r"
             * 1234e78 is NOT a valid float */
            return( get_float( tok, input, output ) );
        }
        c = tolower( c );
        if( isxdigit( c ) ) {
            /*
             * process only a-fA-F characters, 0-9 are already processed
             */
            if( base != 16 && radix_base != 16 ) {
                if( c == 'b' ) {
                    if( base == 0 && OK_NUM( BINARY ) ) {
                        c2 = ((unsigned char *)ptr)[1];
                        if( !isxdigit( c2 ) && tolower( c2 ) != 'h' ) {
                            base = 2;
                            extra = 1;
                            break;
                        }
                    }
                } else if( c == 'd' ) {
                    if( base == 0 && OK_NUM( DECIMAL ) ) {
                        c2 = ptr[1];
                        if( !isxdigit( c2 ) && tolower( c2 ) != 'h' ) {
                            if( !isalnum( c2 ) && c2 != '_' ) {
                                base = 10;
                                extra = 1;
                            }
                            break;
                        }
                    }
                }
            }
            digits_seen |= 1 << (c - 'a' + 10);
            continue;
        } else if( c == 'h' ) {
            base = 16;
            extra = 1;
        } else if( c == 't' ) {
            base = 10;
            extra = 1;
        } else if( c == 'o' || c == 'q' ) {
            base = 8;
            extra = 1;
        } else if( c == 'r' ) {
            /* note that a float MUST contain a dot
             * OR be ended with an "r"
             * 1234e78 is NOT a valid float */
            return( get_float( tok, input, output ) );
        } else if( c == 'y' ) {
            base = 2;
            extra = 1;
        }
        break;
    }
    if( digits_seen == 0 ) {
        if( !first_char_0 ) {
            return( get_string( tok, input, output ) );
        }
        digits_seen |= 1;
        first_char_0 = false;
        dig_start = *input;
    }
#if defined( _STANDALONE_ )
    if( !Options.allow_c_octals ) {
        first_char_0 = false;
    }
#endif
    tok->class = TC_NUM;
    if( base == 0 ) {
        if( base10 ) {
            base = 10;
        } else {
            base = radix_base;
        }
    }
    switch( base ) {
    case 10:
        if( OK_NUM( DECIMAL ) )
            break;
        /* fall through */
    case 8:
        if( OK_NUM( OCTAL ) )
            break;
        /* fall through */
    case 2:
        if( OK_NUM( BINARY ) )
            break;
        /* fall through */
        //AsmError( INVALID_NUMBER_DIGIT );
        /* swallow remainder of token */
        c = *(unsigned char *)ptr;
        while( IS_VALID_ID_CHAR( c ) ) {
            c = *(unsigned char *)(++ptr);
        }
        tok->class = TC_BAD_NUM;
        break;
    }
    /* copy the string, fix input & output pointers */
    tok->string_ptr = *output;
    len = ptr - *input + extra;
    memcpy( *output, *input, len );
    *output += len;
    *(*output)++ = '\0';
    *input = ptr + extra;
    memset( tok->u.bytes, 0, sizeof( tok->u.bytes ) );
    while( dig_start < ptr ) {
        array_mul_add( tok->u.bytes, base, dig_start, sizeof( tok->u.bytes ) );
        ++dig_start;
    }
    return( RC_OK );
} /* get_number */

static bool get_id_in_backquotes( asm_tok *tok, const char **input, char **output )
/*********************************************************************************/
{
    tok->string_ptr = *output;
    tok->class = TC_ID;
    tok->u.value = 0;

    /* copy char from input to output & inc both */
    (*input)++;             // strip off the backquotes
    for( ; **input != '`'; ) {
        *(*output)++ = *(*input)++;
        if( **input == '\0' || **input == ';' ) {
            AsmError( SYNTAX_ERROR );
            return( RC_ERROR );
        }
    }
    (*input)++;         /* don't output the last '`' */
    *(*output)++ = '\0';
    return( RC_OK );
}

static bool get_id( asm_tok *tok, const char **input, char **output )
/********************************************************************
 * get_id could change buf_index, if a COMMENT directive is found
 */
{
    int             c;
    const asm_ins   ASMI86FAR *ins;

    tok->string_ptr = *output;
    if( **input == '\\' ) {
        tok->class = TC_PATH;
    } else {
        tok->class = TC_ID;
    }
    tok->u.value = 0;

    *(*output)++ = *(*input)++;
    for( ; ; ) {
        c = *(unsigned char *)*input;
        /* if character is part of a valid name, add it */
        if( IS_VALID_ID_CHAR( c ) ) {
            *(*output)++ = *(*input)++;
        } else if( c == '\\' ) {
            *(*output)++ = *(*input)++;
            tok->class = TC_PATH;
        } else  {
            break;
        }
    }
    *(*output)++ = '\0';

    /* now decide what to do with it */

    if( tok->class == TC_PATH )
        return( RC_OK );
    ins = get_instruction( tok->string_ptr );
    if( ins == NULL ) {
        if( tok->string_ptr[1] == '\0' && tok->string_ptr[0] == '?' ) {
            tok->class = TC_QUESTION_MARK;
        }
    } else {
        tok->u.token = ins->token;
#if defined( _STANDALONE_ )
        switch( ins->token ) {
        // MASM6 keywords
        case T_FOR:
        case T_FORC:
            if( Options.mode & (MODE_MASM5 | MODE_TASM) ) {
                return( RC_OK );
            }
            break;
        // TASM keywords - initialization
        case T_MASM:
        case T_IDEAL:
            if( (Options.mode_init & MODE_TASM) == 0 ) {
                return( RC_OK );
            }
            break;
        // TASM keywords
        case T_ARG:
        case T_ENUM:
        case T_LOCALS:
        case T_NOLOCALS:
        case T_NOWARN:
        case T_SMART:
        case T_WARN:
            if( (Options.mode & MODE_TASM) == 0 ) {
                return( RC_OK );
            }
            break;
        // TASM IDEAL keywords
        case T_CODESEG:
        case T_CONST:
        case T_DATASEG:
        case T_ERR:
        case T_ERRIFB:
        case T_ERRIFDEF:
        case T_ERRIFDIF:
        case T_ERRIFDIFI:
        case T_ERRIFE:
        case T_ERRIFIDN:
        case T_ERRIFIDNI:
        case T_ERRIFNB:
        case T_ERRIFNDEF:
        case T_EXITCODE:
        case T_FARDATA:
        case T_MODEL:
        case T_P186:
        case T_P286:
        case T_P286N:
        case T_P286P:
        case T_P287:
        case T_P386:
        case T_P386P:
        case T_P387:
        case T_P486:
        case T_P486P:
        case T_P586:
        case T_P586P:
        case T_P686:
        case T_P686P:
        case T_P8086:
        case T_P8087:
        case T_PK3D:
        case T_PMMX:
        case T_PXMM:
        case T_PXMM2:
        case T_PXMM3:
        case T_STACK:
        case T_STARTUPCODE:
        case T_UDATASEG:
        case T_UFARDATA:
            if( (Options.mode & MODE_IDEAL) == 0 ) {
                return( RC_OK );
            }
            break;
        }
#endif

        if( ins->opnd_type1 == OP_SPECIAL ) {
            if( ins->rm_byte == OP_REGISTER ) {
                tok->class = TC_REG;
            } else if( ins->rm_byte == OP_RES_ID ) {
                tok->class = TC_RES_ID;
                if( tok->u.token == T_DP ) {
                    tok->u.token = T_DF;
                }
            } else if( ins->rm_byte == OP_RES_ID_PTR_MODIF ) {
                tok->class = TC_RES_ID;
                if( tok->u.token == T_PWORD ) {
                    tok->u.token = T_FWORD;
                }
            } else if( ins->rm_byte == OP_UNARY_OPERATOR ) {
                tok->class = TC_UNARY_OPERATOR;
#if defined( _STANDALONE_ )
            } else if( ins->rm_byte == OP_RELATION_OPERATOR ) {
                tok->class = TC_RELATION_OPERATOR;
#endif
            } else if( ins->rm_byte == OP_ARITH_OPERATOR ) {
                tok->class = TC_ARITH_OPERATOR;
            } else if( ins->rm_byte == OP_DIRECTIVE ) {
                tok->class = TC_DIRECTIVE;
#if defined( _STANDALONE_ )
                if( ins->token == T_ENUM ) {
                    EnumDirective = true;
                }
            } else if( ins->rm_byte == OP_DIRECT_EXPR ) {
                tok->class = TC_DIRECT_EXPR;
#endif
            } else {
                tok->class = TC_INSTR;
            }
        } else {
            tok->class = TC_INSTR;
        }
    }
    return( RC_OK );
}

#if defined( _STANDALONE_ )

static bool get_comment( asm_tok *tok, const char **input, char **output )
{
    size_t  len;
    /* save the whole line .. we need to check
     * if the delim. char shows up 2 times */
    tok->string_ptr = *output;
    len = strlen( *input );
    memcpy( tok->string_ptr, *input, len );
    (*output) += len;
    *(*output)++ = '\0';
    *input += len;
    tok->class = TC_STRING;
    tok->u.value = 0;
    return( RC_OK );
}

#endif

static bool get_special_symbol( asm_tok *tok, const char **input, char **output )
/*******************************************************************************/
{
    char        symbol;
    tok_class   cls;

    tok->string_ptr = *output;

    symbol = **input;
    switch( symbol ) {
    case '.' :
        cls = TC_DOT;
        break;
    case ',' :
        cls = TC_COMMA;
        break;
    case '+' :
        cls = TC_PLUS;
        break;
    case '-' :
        cls = TC_MINUS;
        break;
    case '*' :
        tok->u.token = T_OP_TIMES;
        cls = TC_ARITH_OPERATOR;
        break;
    case '/' :
        tok->u.token = T_OP_DIVIDE;
        cls = TC_ARITH_OPERATOR;
        break;
    case '[' :
        cls = TC_OP_SQ_BRACKET;
        break;
    case ']' :
        cls = TC_CL_SQ_BRACKET;
        break;
    case '(' :
        cls = TC_OP_BRACKET;
        break;
    case ')' :
        cls = TC_CL_BRACKET;
        break;
    case ':' :
        cls = TC_COLON;
        break;
    case '%' :
        cls = TC_PERCENT;
        break;
#if defined( _STANDALONE_ )
    case '=' :
        tok->u.token = T_EQU2;
        cls = TC_DIRECTIVE;
        break;
    case '}' :
        cls = TC_CL_BRACE;
        break;
#endif
    case '{' :
#if defined( _STANDALONE_ )
        if( EnumDirective ) {
            cls = TC_OP_BRACE;
            break;
        }
#endif
    case '\'' :
    case '"' :
    case '<' :
        /* string delimiters */
        /* fall through */
    default:
        /* anything we don't recognise we will consider a string,
         * delimited by space characters, commas, newlines or nulls
         */
        return( get_string( tok, input, output ) );
    }
    tok->class = cls;
    *(*output)++ = *(*input)++;
    *(*output)++ = '\0';
    return( RC_OK );
}

#if defined( _STANDALONE_ )
static bool get_inc_path( asm_tok *tok, const char **input, char **output )
/*************************************************************************/
{
    char symbol;

    tok->class = TC_PATH;
    tok->u.value = 0;
    tok->string_ptr = *output;

    while( isspace( **input ) )
        (*input)++;

    symbol = **input;

    switch( symbol ) {
    case '\'' :
    case '"' :
    case '<' :
    case '{' :
        /* string delimiters -- just get the path as a string */
        return( get_string( tok, input, output ) );
    default:
        /* otherwise, just copy whatever is here */
        while( **input && !isspace( **input )  ) {
            *(*output)++ = *(*input)++;
        }
        *(*output)++ = '\0';
        return( RC_OK );
    }
}
#endif

static bool endFinalToken( asm_tok *tok, char *buff, bool rc )
{
    tok->string_ptr = buff;
    tok->class = TC_FINAL;
    return( rc );
}

bool AsmScan( token_buffer *tokbuf, const char *string )
/*******************************************************
 * - perform syntax checking on scan line;
 * - pass back tokens for later use;
 * - string contains the WHOLE line to scan
 */
{
    const char          *ptr;
    char                *output_ptr;
    asm_tok             *tok;
    int                 c;
    bool                base10;

#ifdef DEBUG_OUT
    CurrString = string;
#endif
#if defined( _STANDALONE_ )
    EnumDirective = false;
#endif

    base10 = false;
    ptr = string;
// FIXME !!
    /* skip initial spaces and expansion codes */
    while( isspace( *ptr ) || (*ptr == '%') ) {
        ptr++;
    }
    output_ptr = tokbuf->stringbuf;
    /*
     * create blank string on buffer beginning
     * for termination token
     */
    *output_ptr++ = '\0';
    tokbuf->count = 0;
    tok = tokbuf->tokens;
    while( tokbuf->count < MAX_TOKEN ) {
        tok->string_ptr = output_ptr;
        while( isspace( *ptr ) ) {
            ptr++;
        }
        c = *(unsigned char *)ptr;
        if( c == NULLC ) {
            return( endFinalToken( tok, tokbuf->stringbuf, RC_OK ) );
        }

        if( IS_VALID_ID_CHAR_FIRST( c )
          || c == '\\'
          || ( c == '.' && tokbuf->count == 0 ) ) {
            if( get_id( tok, &ptr, &output_ptr ) ) {
                return( endFinalToken( tok + 1, tokbuf->stringbuf, RC_ERROR ) );
            }
#if defined( _STANDALONE_ )
            if( tok->class == TC_DIRECTIVE ) {
                if( tok->u.token == T_COMMENT ) {
                    tok++;
                    tokbuf->count++;
                    if( tokbuf->count >= MAX_TOKEN )
                        break;
                    get_comment( tok, &ptr, &output_ptr );
                } else if( tok->u.token == T_INCLUDE || tok->u.token == T_INCLUDELIB ) {
                    // this mess allows include directives with undelimited file names
                    tok++;
                    tokbuf->count++;
                    if( tokbuf->count >= MAX_TOKEN )
                        break;
                    get_inc_path( tok, &ptr, &output_ptr );
                } else if( tok->u.token == T_DOT_RADIX ) {
                    base10 = true;
                }
            }
#endif
        } else if( isdigit( c ) ) {
            if( get_number( tok, &ptr, &output_ptr, base10 ) ) {
                return( endFinalToken( tok + 1, tokbuf->stringbuf, RC_ERROR ) );
            }
        } else if( c == '`' ) {
            if( get_id_in_backquotes( tok, &ptr, &output_ptr ) ) {
                return( endFinalToken( tok + 1, tokbuf->stringbuf, RC_ERROR ) );
            }
        } else {
            if( get_special_symbol( tok, &ptr, &output_ptr ) ) {
                return( endFinalToken( tok + 1, tokbuf->stringbuf, RC_ERROR ) );
            }
        }
        tok++;
        tokbuf->count++;
    }
    AsmError( TOO_MANY_TOKENS );
    return( endFinalToken( tok, tokbuf->stringbuf, RC_ERROR ) );
}

void RadixSet( unsigned new_base )
{
    radix_base = new_base;
}
