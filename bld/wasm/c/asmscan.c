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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "asmglob.h"
#include "asmops1.h"//
#include "asmops2.h"
#include "asmopnds.h"
#include "asmins1.h"
#include "asmerr.h"
#include "asmsym.h"
#include "asmalloc.h"
#include "asmdefs.h"

#ifdef _WASM_

#include "directiv.h"
#include "watcom.h"
#include "womp.h"
#include "objrec.h"
#include "pcobj.h"
#include "fixup.h"
#include "namemgr.h"

#endif

extern void             AsmError( int );

char                                *CurrString; // Current Input Line
extern struct AsmCodeName           AsmOpcode[];
extern const struct asm_ins ASMFAR  AsmOpTable[];
extern char                         AsmChars[];
extern struct asm_tok               *AsmBuffer[];

extern int get_instruction_position( char *string );

#ifdef _WASM_

extern global_options   Options;

void GetInsString( enum asm_token token, char *string, int len )
/**************************************************************/
{
    int index;

    if( len > AsmOpcode[ token ].len ) {
        len = AsmOpcode[ token ].len;
        index = AsmOpcode[ token ].index;
        if( AsmChars[index]== '.' ) {
            index++;
            len--;
        }
        strncpy( string, &(AsmChars[ index ]), len );
        string[ len ] = '\0';
    } else {
        *string='\0';
    }
    return;
}
#endif

static int get_float( struct asm_tok *buf, char **input, char **output )
/**********************************************************************/
{
    /* valid floats look like:  (int)[.(int)][e(int)] */

    char    got_decimal = FALSE;
    char    got_e = FALSE;
    char    *ptr = *input;

    for( ; *ptr != '\0'; ptr++ ) {
        if( isdigit( *ptr ) ) continue;
        if( isspace( *ptr ) ) break;
        switch( tolower( *ptr ) ) {
        case '.':
            got_decimal = TRUE;
            continue;
        case 'r':
            *ptr=' ';
            goto done_scanning_float;
        case 'e':
            if( !got_e ) {
                got_e = TRUE;
                /* accept e+2 / e-4 /etc. */

                switch( *(ptr+1) ) {
                case '+':
                case '-':
                    ptr++;
                default:
                    continue;
                }
            }
        default:
            return( get_string( buf, input, output ) );
        }
    }

done_scanning_float:

    buf->token = T_FLOAT;
    /* copy the string, fix input & output pointers */
    strncpy( *output, *input, ptr - *input );
    buf->string_ptr = *output;
    *output += ( ptr - *input );
    **output = '\0';
    (*output)++;
    *input = ptr;

    buf->value = 0;
    return( NOT_ERROR );
}

static int get_number( struct asm_tok *buf, char **input, char **output )
/***********************************************************************/
{
    char                *ptr = *input;
    char                *dig_start;
    char                first_char_0 = FALSE;
    unsigned            extra;
    unsigned            len;
    unsigned            base = 0;
    unsigned            digits_seen;
    unsigned long       val;
    unsigned            down_shift;

#define VALID_BINARY    0x0003
#define VALID_OCTAL     0x00ff
#define VALID_DECIMAL   0x03ff
#define OK_NUM( t )     ((digits_seen & ~VALID_##t) == 0)
    digits_seen = 0;

    extra = 0;
    if( *ptr == '0' ) {
        if( tolower( *(ptr+1) ) == 'x' ) {
            ptr+=2;
            base = 16;
        } else {
            ptr += 1;
            first_char_0 = TRUE;
        }
    }
    dig_start = ptr;
    for( ;; ) {
        switch( tolower( *ptr ) ) {
        case '9':
        case '8':
        case '7':
        case '6':
        case '5':
        case '4':
        case '3':
        case '2':
        case '1':
        case '0':
            digits_seen |= 1 << (*ptr - '0');
            break;
        case 'a':
            digits_seen |= 1 << 10;
            break;
        case 'b':
            if( base == 0
             && OK_NUM( BINARY )
             && !isxdigit( ptr[1] )
             && tolower( ptr[1] ) != 'h' ) {
                base = 2;
                extra = 1;
                goto done_scan;
            }
            digits_seen |= 1 << 11;
            break;
        case 'c':
            digits_seen |= 1 << 12;
            break;
        case 'd':
            if( base == 0
             && OK_NUM( DECIMAL )
             && !isxdigit( ptr[1] )
             && tolower( ptr[1] ) != 'h' ) {
                if( !isalnum( ptr[1] ) && ptr[1] != '_' ) {
                    base = 10;
                    extra = 1;
                }
                goto done_scan;
            }
            digits_seen |= 1 << 13;
            break;
        case 'e': /* note that this could NOT be part of a float */
            digits_seen |= 1 << 14;
            break;
        case 'f':
            digits_seen |= 1U << 15;
            break;
        case 'y':
            base = 2;
            extra = 1;
            goto done_scan;
        case 'h':
            base = 16;
            extra = 1;
            goto done_scan;
        case 'o':
            base = 8;
            extra = 1;
            goto done_scan;
        case '.':
        case 'r':
            /* note that a float MUST contain a dot
             * OR be ended with an "r"
             * 1234e78 is NOT a valid float */

            return( get_float( buf, input, output ) );
         default:
             goto done_scan;
        }
        ++ptr;
    }
done_scan:
    if( digits_seen == 0 ) {
        if( !first_char_0 ) {
            return( get_string( buf, input, output ) );
        }
        digits_seen |= 1;
        first_char_0 = FALSE;
        dig_start = *input;
    }
#ifdef _WASM_
    if( !Options.allow_c_octals ) {
        first_char_0 = FALSE;
    }
#endif
    buf->token = T_NUM;
    if( base == 0 ) {
        base = first_char_0 ? 8 : 10;
    }
    switch( base ) {
    case 10:
        if( OK_NUM( DECIMAL ) ) break;
        /* fall through */
    case 8:
        if( OK_NUM( OCTAL ) ) break;
        /* fall through */
    case 2:
        if( OK_NUM( BINARY ) ) break;
        /* fall through */
        //AsmError( INVALID_NUMBER_DIGIT );
        /* swallow remainder of token */
        while( isalnum( *ptr )
            || *ptr == '_'
            || *ptr == '$'
            || *ptr == '@'
            || *ptr == '?' ) ++ptr;
        buf->token = T_BAD_NUM;
        break;
    }
    /* copy the string, fix input & output pointers */
    len = ptr - *input + extra;
    strncpy( *output, *input, len );
    buf->string_ptr = *output;
    *output += len;
    **output = '\0';
    (*output)++;
    *input = ptr + extra;
    val = 0;
    buf->extra_value = 0;
    switch( base ) {
    case 2:     down_shift = 31; break;
    case 8:     down_shift = 29; break;
    case 16:    down_shift = 28; break;
    default:    down_shift = 0;  break;
    }
    while( dig_start < ptr ) {
        if( down_shift != 0 ) {
            /* for DQ initializations */
            buf->extra_value *= base;
            buf->extra_value |= val >> down_shift;
        }
        val *= base;
        if( isdigit( *dig_start ) ) {
            val += *dig_start - '0';
        } else {
            val += tolower( *dig_start ) - 'a' + 10;
        }
        ++dig_start;
    }
    buf->value = val;
    return( NOT_ERROR );
} /* get_number */

static int get_id_in_backquotes( struct asm_tok *buf, char **input, char **output )
/*********************************************************************************/
{
    buf->string_ptr = *output;
    buf->token = T_ID;
    buf->value = 0;

    /* copy char from input to output & inc both */
    (*input)++;             // strip off the backquotes
    for( ; **input != '`'; ) {
        *(*output)++ = *(*input)++;
        if( **input == '\0' || **input == ';' ) {
            AsmError( SYNTAX_ERROR );
            return( ERROR );
        }
    }
    (*input)++;         /* don't output the last '`' */
    *(*output)++ = '\0';
    return( NOT_ERROR );
}

static int get_id( unsigned int *buf_index, char **input, char **output )
/***********************************************************************/
/* get_id could change buf_index, if a COMMENT directive is found */
{
    struct asm_tok  *buf;
    char            cur_char;
    int             count;

    buf = AsmBuffer[ *buf_index ];

    buf->string_ptr = *output;
    if( **input != '\\' ) {
        buf->token = T_ID;
    } else {
        buf->token = T_PATH;
    }
    buf->value = 0;

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
            buf->token = T_PATH;
        } else  {
            break;
        }
    }
    *(*output)++ = '\0';

    /* now decide what to do with it */

    if( buf->token == T_PATH ) return( NOT_ERROR );
    count = get_instruction_position( buf->string_ptr );
    if( count == EMPTY ) {
        buf->token = T_ID;
        if( buf->string_ptr[1] == '\0' && buf->string_ptr[0] == '?' ) {
            buf->token = T_QUESTION_MARK;
        }
    } else {
        buf->value = AsmOpTable[count].token;
        // count = AsmOpcode[count].position;

        if( AsmOpTable[count].opnd_type[0] == OP_SPECIAL ) {
            if( AsmOpTable[count].rm_byte & OP_REGISTER ) {
                buf->token = T_REG;
                #if 0
                // fixme
                if( buf_index > 0 ) {   // 30-07-92
                    if( AsmBuffer[buf_index-1]->token == '+' ||
                        AsmBuffer[buf_index-1]->token == '-' ) {
                        if( sq_bracket_flag == 0 ) {
                            /* Disallow addition or subtraction of
                               registers out of square brackets */
                            AsmError( ILLEGAL_USE_OF_REGISTER );
                            return( ERROR );
                        }
                    }
                }
                #endif
            } else if( AsmOpTable[count].rm_byte & OP_RES_ID ) {
                buf->token = T_RES_ID;
            } else if( AsmOpTable[count].rm_byte & OP_UNARY_OPERATOR ) {
                buf->token = T_UNARY_OPERATOR;
            } else if( AsmOpTable[count].rm_byte & OP_DIRECTIVE ) {
                #if 0
                if( buf_index >= 2 ) {
                    /* it is too late in the line to be a directive,
                     * so just treat it as a res_id
                     */
                    buf->token = T_RES_ID;
                }
                #endif
                buf->token = T_DIRECTIVE;

#ifdef _WASM_
                switch( AsmOpTable[count].token ) {
                case T_COMMENT:
                    /* save the whole line .. we need to check
                     * if the delim. char shows up 2 times */
                    (*output)++;
                    *(*output) = '\0';
                    (*output)++;
                    (*buf_index)++;
                    buf = AsmBuffer[ *buf_index ];
                    buf->string_ptr = *output;
                    strcpy( buf->string_ptr, *input );
                    (*output) += strlen( *input );
                    *(*output) = '\0';
                    (*output)++;
                    (*input) += strlen( *input );
                    buf->token = T_STRING;
                    buf->value = 0;
                    break;
                } /* default do nothing */
#endif
            } else if( AsmOpTable[count].rm_byte & OP_DIRECT_EXPR ) {
                buf->token = T_DIRECT_EXPR;
            } else {
                buf->token = T_INS;
            }
        } else {
            buf->token = T_INS;
        }
    }
    return( NOT_ERROR );
}

static int get_string( struct asm_tok *buf, char **input, char **output )
/***********************************************************************/
{
    char    symbol;
    int     count;

    buf->string_ptr = *output;

    symbol = **input;

    buf->token = T_STRING;
    switch( symbol ) {
    case '"':
    case '\'':
        break;  // end of string marker is the same
    case '<':
        symbol = '>';
        break;
    case '{':
        symbol = '}';
        break;
    default:
        /* this is an undelimited string,
         * so just copy it until we hit something that looks like the end
         */

        for( count = 0; **input != '\0' && !isspace( **input ) && **input != ','; count++ ) {
            *(*output)++ = *(*input)++; /* keep the 2nd one */
        }
        *(*output)++ = '\0';
        buf->value = count;
        return( NOT_ERROR );
    }
    (*input)++;

    count = 0;
    while( count < MAX_TOK_LEN ) {
        if( **input == symbol ) {
            if( *( *input + 1 ) == symbol ) {
                /* if we see "" in a " delimited string,
                 * treat it as a literal " */
                (*input)++; /* skip the 1st one */
                *(*output)++ = *(*input)++; /* keep the 2nd one */
                count++;
            } else {
                *(*output)++ = '\0';
                (*input)++; /* skip the closing delimiter */
                buf->value = count;
                break;
            }
        } else if( **input == '\0' || **input == '\n' ) {
            AsmError( SYNTAX_ERROR );
            return( ERROR );
        } else {
            *(*output)++ = *(*input)++;
            count++;
        }
    }
    return( NOT_ERROR );
}

static int get_special_symbol( struct asm_tok *buf,
                                char **input, char **output )
/***********************************************************/
{
    char    symbol;

    buf->string_ptr = *output;

    symbol = **input;
    switch( symbol ) {
    case '.' :
    case ',' :
    case '+' :
    case '-' :
    case '*' :
    case '/' :
    case '[' :
    case ']' :
    case '(' :
    case ')' :
    case ':' :
    case '%' :
        /* all of these are themselves a token */

        /* fixme
         * wasm used to do a bunch of error checking dealing with use of
         * registers and +,- inside and outside brackets
         * I am not sure we need to do this here
         * this looks more like the job of the parser
         */

        buf->token = symbol;
        *(*output)++ = *(*input)++;
        *(*output)++ = '\0';
        break;
#ifdef _WASM_
    case '=' :
        buf->token = T_DIRECTIVE;
        buf->value = T_EQU2;
        *(*output)++ = *(*input)++;
        *(*output)++ = '\0';
        break;
#endif
    case '\'' :
    case '"' :
    case '<' :
    case '{' :
        /* string delimiters */
    default:
        /* anything we don't recognise we will consider a string,
         * delimited by space characters, commas, newlines or nulls
         */
        return( get_string( buf, input, output ) );
        break;
    }
    return( NOT_ERROR );
}

#ifdef _WASM_
static int get_inc_path( unsigned int *buf_index, char **input, char **output )
/*****************************************************************************/
{
    char symbol;

    AsmBuffer[*buf_index]->token = T_PATH;
    AsmBuffer[*buf_index]->value = 0;
    AsmBuffer[*buf_index]->string_ptr = *output;

    while( isspace( **input ) ) (*input)++;

    symbol = **input;

    switch( symbol ) {
    case '\'' :
    case '"' :
    case '<' :
    case '{' :
        /* string delimiters -- just get the path as a string */
        if( get_special_symbol( AsmBuffer[*buf_index],input,output )==ERROR ) {
            return( ERROR );
        }
        return( NOT_ERROR );
    default:
        /* otherwise, just copy whatever is here */
        while( **input && !isspace( **input )  ) {
            *(*output)++ = *(*input)++;
        }
        return( NOT_ERROR );
    }
}
#endif

int AsmScan( char *string, char *stringbuf )
/******************************************/
/*
- perform syntax checking on scan line;
- pass back tokens for later use;
- string contains the WHOLE line to scan
- stringbuf - buffer in which to store strings
*/
{
    char                        *ptr;
    char                        *output_ptr;
    unsigned int                buf_index = 0;

    output_ptr = stringbuf;

    for( CurrString = ptr = string; ; ) {
        AsmBuffer[buf_index]->string_ptr = output_ptr;

        while( isspace( *ptr ) ) {
            ptr++;
        }
        if( *ptr == NULLC ) break;

        if( isalpha( *ptr )
            || *ptr == '_'
            || *ptr == '$'
            || *ptr == '@'
            || *ptr == '?'
            || *ptr == '\\'
            || ( *ptr == '.' && buf_index == 0 ) ) {
            if( get_id( &buf_index,&ptr,&output_ptr ) == ERROR ) {
                return( ERROR );
            }
            #ifdef _WASM_
            // this mess allows include directives with undelimited file names
            if( AsmBuffer[buf_index]->token == T_DIRECTIVE &&
                ( AsmBuffer[buf_index]->value == T_INCLUDE ||
                  AsmBuffer[buf_index]->value == T_INCLUDELIB ) ) {
                buf_index++;
                get_inc_path( &buf_index, &ptr, &output_ptr );
            }

            #endif
        } else if( isdigit( *ptr ) ) {
            if( get_number( AsmBuffer[buf_index],&ptr,&output_ptr ) == ERROR ) {
                return( ERROR );
            }
            /* handle negatives here - for inline assembler */
            #ifndef _WASM_
                if( buf_index > 0 && AsmBuffer[buf_index-1]->token == T_MINUS ) {
                    AsmBuffer[buf_index-1]->token = T_PLUS;
                    AsmBuffer[buf_index]->value = -AsmBuffer[buf_index]->value;
                }
            #endif
        } else if( *ptr == '`' ) {
            if( get_id_in_backquotes( AsmBuffer[buf_index],&ptr,&output_ptr ) == ERROR ) {
                return( ERROR );
            }
        } else {
             if( get_special_symbol( AsmBuffer[buf_index],&ptr,&output_ptr )
                 == ERROR ) {
                return( ERROR );
            }
        }
        buf_index++;
        if( buf_index >= MAX_TOKEN ) {
            AsmError( TOO_MANY_TOKENS );
            return( ERROR );
        }
    }
    AsmBuffer[buf_index]->token = T_FINAL;
    *output_ptr='\0';
    return( buf_index );
}
