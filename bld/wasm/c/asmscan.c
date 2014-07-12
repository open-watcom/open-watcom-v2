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


#include "asmglob.h"
#if defined( _STANDALONE_ )
#include "directiv.h"
#endif
#include <ctype.h>

#ifdef DEBUG_OUT
const char              *CurrString; // Current Input Line
#endif

extern asmins_idx       get_instruction_position( char *string );

#if defined( _STANDALONE_ )

bool                    EnumDirective;

#endif

typedef union {
    float   f;
    long    l;
} NUMBERFL;

static bool get_float( asm_tok *buf, const char **input, char **output )
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
    got_e = FALSE;
    got_decimal = FALSE;
    for( ptr = *input; (c = *ptr) != '\0'; ptr++ ) {
        if( isdigit( c ) )
            continue;
        if( isspace( c ) )
            break;
        if( c == '.' ) {
            if( got_decimal || got_e )
                break;
            got_decimal = TRUE;
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
        got_e = TRUE;
        /* accept e+2 / e-4 /etc. */
        c = *(ptr + 1);
        if( c == '+' || c == '-' ) {
            ptr++;
        }
    }
    buf->class = TC_FLOAT;
    /* copy the string, fix input & output pointers */
    buf->string_ptr = *output;
    len = ptr - *input + extra;
    memcpy( *output, *input, len );
    *output += len;
    *(*output)++ = '\0';
    *input = ptr + extra;

    buf->u.float_value = (float)atof( buf->string_ptr );
    return( RC_OK );
}

static void array_mul_add( unsigned char *buf, unsigned base, unsigned num, unsigned size )
{
    while( size-- > 0 ) {
        num += *buf * base;
        *(buf++) = (unsigned char)num;
        num >>= 8;
    }
}

static bool get_string( asm_tok *buf, const char **input, char **output )
/***********************************************************************/
{
    char    symbol_o;
    char    symbol_c;
    int     count;
    int     level;

    buf->string_ptr = *output;

    symbol_o = **input;

    buf->class = TC_STRING;
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
        buf->u.value = count;
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
    buf->u.value = count;
    return( RC_OK );
}

static bool get_number( asm_tok *buf, const char **input, char **output )
/***********************************************************************/
{
    const char          *ptr;
    const char          *dig_start;
    bool                first_char_0;
    unsigned char       extra;
    size_t              len;
    unsigned            base = 0;
    unsigned            digits_seen;
    unsigned long       val;
    int                 c;
    int                 c2;

#define VALID_BINARY    0x0003
#define VALID_OCTAL     0x00ff
#define VALID_DECIMAL   0x03ff
#define OK_NUM( t )     ((digits_seen & ~VALID_##t) == 0)

    digits_seen = 0;
    first_char_0 = FALSE;
    extra = 0;
    ptr = *input;
    if( *ptr == '0' ) {
        ++ptr;
        if( tolower( *ptr ) == 'x' ) {
            ++ptr;
            base = 16;
        } else {
            first_char_0 = TRUE;
        }
    }
    dig_start = ptr;
    for( ; (c = *ptr) != '\0'; ++ptr ) {
        if( isdigit( c ) ) {
            digits_seen |= 1 << (c - '0');
            continue;
        } else if( isspace( c ) ) {
            break;
        } else if( c == '.' ) {
            /* note that a float MUST contain a dot
             * OR be ended with an "r"
             * 1234e78 is NOT a valid float */
            return( get_float( buf, input, output ) );
        }
        c = tolower( c );
        if( isxdigit( c ) ) {
            if( c == 'b' ) {
                if( base == 0 && OK_NUM( BINARY ) ) {
                    c2 = ptr[1];
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
            digits_seen |= 1 << (c - 'a' + 10);
            continue;
        } else if( c == 'h' ) {
            base = 16;
            extra = 1;
        } else if( c == 'o' ) {
            base = 8;
            extra = 1;
        } else if( c == 'r' ) {
            /* note that a float MUST contain a dot
             * OR be ended with an "r"
             * 1234e78 is NOT a valid float */
            return( get_float( buf, input, output ) );
        } else if( c == 'y' ) {
            base = 2;
            extra = 1;
        }
        break;
    }
    if( digits_seen == 0 ) {
        if( !first_char_0 ) {
            return( get_string( buf, input, output ) );
        }
        digits_seen |= 1;
        first_char_0 = FALSE;
        dig_start = *input;
    }
#if defined( _STANDALONE_ )
    if( !Options.allow_c_octals ) {
        first_char_0 = FALSE;
    }
#endif
    buf->class = TC_NUM;
    if( base == 0 ) {
        base = first_char_0 ? 8 : 10;
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
        while( isalnum( *ptr )
            || *ptr == '_'
            || *ptr == '$'
            || *ptr == '@'
            || *ptr == '?' ) {
            ++ptr;
        }
        buf->class = TC_BAD_NUM;
        break;
    }
    /* copy the string, fix input & output pointers */
    buf->string_ptr = *output;
    len = ptr - *input + extra;
    memcpy( *output, *input, len );
    *output += len;
    *(*output)++ = '\0';
    *input = ptr + extra;
    memset( buf->u.bytes, 0, sizeof( buf->u.bytes ) );
    while( dig_start < ptr ) {
        if( isdigit( *dig_start ) ) {
            val = *dig_start - '0';
        } else {
            val = tolower( *dig_start ) - 'a' + 10;
        }
        array_mul_add( buf->u.bytes, base, val, sizeof( buf->u.bytes ) );
        ++dig_start;
    }
    return( RC_OK );
} /* get_number */

static bool get_id_in_backquotes( asm_tok *buf, const char **input, char **output )
/*********************************************************************************/
{
    buf->string_ptr = *output;
    buf->class = TC_ID;
    buf->u.value = 0;

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

static bool get_id( token_idx *buf_index, const char **input, char **output )
/***************************************************************************/
/* get_id could change buf_index, if a COMMENT directive is found */
{
    asm_tok     *buf;
    char        cur_char;
    asmins_idx  ins_pos;

    buf = AsmBuffer[ *buf_index ];

    buf->string_ptr = *output;
    if( **input != '\\' ) {
        buf->class = TC_ID;
    } else {
        buf->class = TC_PATH;
    }
    buf->u.value = 0;

    *(*output)++ = *(*input)++;
    for( ; ; ) {
        cur_char = **input;
        /* if character is part of a valid name, add it */
        if( isalpha( cur_char )
            || isdigit( cur_char )
            || cur_char == '_'
            || cur_char == '$'
            || cur_char == '@'
            || cur_char == '?'  ) {
            *(*output)++ = *(*input)++;
        } else if( cur_char == '\\' ) {
            *(*output)++ = *(*input)++;
            buf->class = TC_PATH;
        } else  {
            break;
        }
    }
    *(*output)++ = '\0';

    /* now decide what to do with it */

    if( buf->class == TC_PATH )
        return( RC_OK );
    ins_pos = get_instruction_position( buf->string_ptr );
    if( ins_pos == INVALID_POS ) {
        if( buf->string_ptr[1] == '\0' && buf->string_ptr[0] == '?' ) {
            buf->class = TC_QUESTION_MARK;
        }
    } else {
        buf->u.token = AsmOpTable[ins_pos].token;
#if defined( _STANDALONE_ )
        switch( AsmOpTable[ins_pos].token ) {
        // MASM6 keywords
        case T_FOR:
        case T_FORC:
            if( Options.mode & (MODE_MASM5 | MODE_TASM) ) {
                return( RC_OK );
            }
            break;
        // TASM keywords
        case T_ARG:
        case T_ENUM:
        case T_IDEAL:
        case T_LOCALS:
        case T_MASM:
        case T_NOLOCALS:
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
        case T_NOWARN:
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
        case T_WARN:
            if( (Options.mode & MODE_IDEAL) == 0 ) {
                return( RC_OK );
            }
            break;
        }
#endif

        if( AsmOpTable[ins_pos].opnd_type[OPND1] == OP_SPECIAL ) {
            if( AsmOpTable[ins_pos].rm_byte == OP_REGISTER ) {
                buf->class = TC_REG;
            } else if( AsmOpTable[ins_pos].rm_byte == OP_RES_ID ) {
                buf->class = TC_RES_ID;
                if( buf->u.token == T_DP ) {
                    buf->u.token = T_DF;
                }
            } else if( AsmOpTable[ins_pos].rm_byte == OP_RES_ID_PTR_MODIF ) {
                buf->class = TC_RES_ID;
                if( buf->u.token == T_PWORD ) {
                    buf->u.token = T_FWORD;
                }
            } else if( AsmOpTable[ins_pos].rm_byte == OP_UNARY_OPERATOR ) {
                buf->class = TC_UNARY_OPERATOR;
#if defined( _STANDALONE_ )
            } else if( AsmOpTable[ins_pos].rm_byte == OP_RELATION_OPERATOR ) {
                buf->class = TC_RELATION_OPERATOR;
#endif
            } else if( AsmOpTable[ins_pos].rm_byte == OP_ARITH_OPERATOR ) {
                buf->class = TC_ARITH_OPERATOR;
            } else if( AsmOpTable[ins_pos].rm_byte == OP_DIRECTIVE ) {
                buf->class = TC_DIRECTIVE;
#if defined( _STANDALONE_ )
                if( AsmOpTable[ins_pos].token == T_ENUM ) {
                    EnumDirective = TRUE;
                } else if( AsmOpTable[ins_pos].token == T_COMMENT ) {
                    size_t  len;
                    /* save the whole line .. we need to check
                     * if the delim. char shows up 2 times */
                    (*buf_index)++;
                    buf = AsmBuffer[ *buf_index ];
                    buf->string_ptr = *output;
                    len = strlen( *input );
                    memcpy( buf->string_ptr, *input, len );
                    (*output) += len;
                    *(*output)++ = '\0';
                    *input += len;
                    buf->class = TC_STRING;
                    buf->u.value = 0;
                }
            } else if( AsmOpTable[ins_pos].rm_byte == OP_DIRECT_EXPR ) {
                buf->class = TC_DIRECT_EXPR;
#endif
            } else {
                buf->class = TC_INSTR;
            }
        } else {
            buf->class = TC_INSTR;
        }
    }
    return( RC_OK );
}

static bool get_special_symbol( asm_tok *buf, const char **input, char **output )
/*******************************************************************************/
{
    char        symbol;
    tok_class   cls;

    buf->string_ptr = *output;

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
        buf->u.token = T_OP_TIMES;
        cls = TC_ARITH_OPERATOR;
        break;
    case '/' :
        buf->u.token = T_OP_DIVIDE;
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
        buf->u.token = T_EQU2;
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
        return( get_string( buf, input, output ) );
    }
    buf->class = cls;
    *(*output)++ = *(*input)++;
    *(*output)++ = '\0';
    return( RC_OK );
}

#if defined( _STANDALONE_ )
static bool get_inc_path( asm_tok *buf, const char **input, char **output )
/*************************************************************************/
{
    char symbol;

    buf->class = TC_PATH;
    buf->u.value = 0;
    buf->string_ptr = *output;

    while( isspace( **input ) )
        (*input)++;

    symbol = **input;

    switch( symbol ) {
    case '\'' :
    case '"' :
    case '<' :
    case '{' :
        /* string delimiters -- just get the path as a string */
        return( get_string( buf, input, output ) );
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

token_idx AsmScan( const char *string )
/*************************************/
/*
- perform syntax checking on scan line;
- pass back tokens for later use;
- string contains the WHOLE line to scan
*/
{
    const char                  *ptr;
    char                        *output_ptr;
    token_idx                   buf_index = 0;
    // stringbuf - buffer in which to store strings
    static char                 stringbuf[MAX_LINE_LEN];

#ifdef DEBUG_OUT
    CurrString = string;
#endif
    output_ptr = stringbuf;
#if defined( _STANDALONE_ )
    EnumDirective = FALSE;
#endif

    ptr = string;
// FIXME !!
    /* skip initial spaces and expansion codes */
    while( isspace( *ptr ) || (*ptr == '%') ) {
        ptr++;
    }

    for( ;; ) {
        AsmBuffer[buf_index]->string_ptr = output_ptr;

        while( isspace( *ptr ) ) {
            ptr++;
        }
        if( *ptr == NULLC )
            break;

        if( isalpha( *ptr )
            || *ptr == '_'
            || *ptr == '$'
            || *ptr == '@'
            || *ptr == '?'
            || *ptr == '\\'
            || ( *ptr == '.' && buf_index == 0 ) ) {
            if( get_id( &buf_index, &ptr, &output_ptr ) ) {
                return( INVALID_IDX );
            }
#if defined( _STANDALONE_ )
            // this mess allows include directives with undelimited file names
            if( AsmBuffer[buf_index]->class == TC_DIRECTIVE &&
                ( AsmBuffer[buf_index]->u.token == T_INCLUDE ||
                AsmBuffer[buf_index]->u.token == T_INCLUDELIB ) ) {
                buf_index++;
                get_inc_path( AsmBuffer[buf_index], &ptr, &output_ptr );
            }
#endif
        } else if( isdigit( *ptr ) ) {
            if( get_number( AsmBuffer[buf_index], &ptr, &output_ptr ) ) {
                return( INVALID_IDX );
            }
        } else if( *ptr == '`' ) {
            if( get_id_in_backquotes( AsmBuffer[buf_index], &ptr, &output_ptr ) ) {
                return( INVALID_IDX );
            }
        } else {
            if( get_special_symbol( AsmBuffer[buf_index], &ptr, &output_ptr ) ) {
                return( INVALID_IDX );
            }
        }
        buf_index++;
        if( buf_index >= MAX_TOKEN ) {
            AsmError( TOO_MANY_TOKENS );
            return( INVALID_IDX );
        }
    }
    AsmBuffer[buf_index]->class = TC_FINAL;
    *output_ptr='\0';
    return( buf_index );
}
