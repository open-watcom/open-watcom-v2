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
#include <stdlib.h>
#ifndef _WASM_
    #include <malloc.h>
#endif

#include "asmglob.h"
#include "asmdefs.h"
#include "asmops1.h"//
#include "asmops2.h"
#include "asmerr.h"
#include "asmsym.h"
#include "asmalloc.h"
#include "asmins1.h"
#include "asmopnds.h"
#ifdef _WASM_
    #include "directiv.h"
    #include "expand.h"
    #include "fixup.h"
#endif

extern int              Token_Count;
extern struct asm_code  *Code;

extern unsigned char    More_Array_Element;
extern unsigned char    Last_Element_Size;

extern struct asm_tok   *AsmBuffer[MAX_TOKEN];

/* structure stuff from asmstruct */
extern int              InitializeStructure( asm_sym *, int );
extern int              AddFieldToStruct( int );
extern int              GetStructSize( int );

extern void             AsmCodeByte( char );
extern void             AsmDataByte( char );
extern void             AsmByte( char );
extern void             double2tbyte( double, char * );

#ifdef _WASM_
    extern int          ChangeCurrentLocation( bool, int_32 );
#endif

extern int dup_array( asm_sym *sym, char start_pos, char no_of_bytes );

/* from asmins.c */
extern int              check_override( int * );

#ifdef _WASM_
    extern int_8                PhaseError;

    /* static globals */
    /* is this data element a field in a structure definition? */
    static bool         struct_field;
    /* is this the first initializer for this field? */
    static bool         first;
#endif

/* data initialization stuff */

static void little_endian( char *string, char no_of_bytes )
/********************************************************/
/* convert a string into little endian format - ( LSB 1st, LSW 1st ... etc ) */
{
    if( no_of_bytes >= 2 ) {
        strrev( string );
    }
    return;

}

static void output_float( char index, char no_of_bytes, char negative )
/*********************************************************************/
{
    double              double_value;
    float               float_value;
    char                *char_ptr;
    uint_8              count;

    double_value = strtod( AsmBuffer[index]->string_ptr, NULL );

    if( negative ) {
        double_value *= -1;
    }

    switch( no_of_bytes ) {
    case BYTE_4:
        float_value = double_value;
        char_ptr = (char*)&float_value;
        break;
    case BYTE_8:
        char_ptr = (char *)&double_value;
        break;
    case BYTE_10:
        char_ptr = AsmTmpAlloc( BYTE_10 + 1 );
        for( count = 0; count <= BYTE_10; count++ ) {
            char_ptr[ count ] = '\0';
        }
        double2tbyte( double_value, char_ptr );
        break;
    }

    count = 0;
    while( count < no_of_bytes ) {
        AsmDataByte( *char_ptr );
        char_ptr++;
        count++;
    }
    return;
}

static int array_element( asm_sym *sym, char start_pos, char no_of_bytes )
/************************************************************************/
/*
- parse an array and initialize the number;
- call by dup_array() only;
*/
{
    int                 cur_pos = start_pos;
    char                count;
    long                value;
    char                *char_ptr;
    char                negative = FALSE;

    #ifdef _WASM_
        asm_sym     *the_struct;

        the_struct = (asm_sym*)Definition.curr_struct;
    #endif

    while( AsmBuffer[cur_pos]->token != T_FINAL ) {
        #ifdef _WASM_
            int                 tmp;

            tmp = cur_pos;
            if( check_override( &tmp ) == ERROR ) {
                return( ERROR );
            }
            cur_pos = tmp;
        #endif
        if( cur_pos == Token_Count - 1 &&
            AsmBuffer[cur_pos]->token == T_CL_BRACKET ) break;
        switch( AsmBuffer[cur_pos]->token ) {
        case T_QUESTION_MARK:
            if( cur_pos != start_pos ) {
                if( AsmBuffer[cur_pos - 1]->token != T_COMMA ) {
                    AsmError( EXPECTING_COMMA );
                    return( ERROR );
                }
            }
            #ifdef _WASM_
                if( !struct_field ) {
                    ChangeCurrentLocation( TRUE, no_of_bytes );
                } else {
                    Definition.curr_struct->e.structinfo->size += no_of_bytes;
                    the_struct->total_size+=no_of_bytes;
                    the_struct->total_length++;
                    if( first ) {
                        the_struct->first_size+=no_of_bytes;
                        the_struct->first_length++;
                    }
                }

                if( sym && Parse_Pass == PASS_1 ) {
                    sym->total_size+=no_of_bytes;
                    if( first ) sym->first_size+=no_of_bytes;
                }
            #else
                count = 0;
                while( count < no_of_bytes ) {
                    AsmDataByte( 0 );
                    count++;
                }
            #endif
            break;
        case T_MINUS:
            switch( AsmBuffer[cur_pos+1]->token ) {
            case T_NUM:
                AsmBuffer[cur_pos+1]->value *= -1;
                AsmBuffer[cur_pos]->token = T_PLUS;
                break;
            case T_FLOAT:
                negative = TRUE;
                break;
            default:
                AsmError( EXPECTING_NUMBER );
                return( ERROR );
            }
            break;
        case T_PLUS:
        case T_DOT:
            break; // go around again
        case T_NUM:
        case T_FLOAT:
            if( AsmBuffer[cur_pos+1]->token == T_RES_ID &&
                AsmBuffer[cur_pos+1]->value == T_DUP ) {
                cur_pos = dup_array( sym, cur_pos, no_of_bytes );
                if( cur_pos == ERROR ) return( ERROR );
                break;
            }

            if( cur_pos != start_pos ) {
                switch( AsmBuffer[cur_pos - 1]->token ) {
                case T_COMMA:
                    break;
                case T_PLUS:
                case T_MINUS:
                    if( AsmBuffer[cur_pos - 2]->token == T_COMMA ) break;
                    if( cur_pos - 1 == start_pos ) break;
                default:
                    AsmError( EXPECTING_COMMA );
                    return( ERROR );
                }
            }
            if( AsmBuffer[cur_pos]->token == T_FLOAT ) {
                output_float( cur_pos, no_of_bytes, negative );
                negative = FALSE;
                break;
            }
            value = AsmBuffer[cur_pos]->value;
            count = 0;
            #ifdef _WASM_
            if( sym && Parse_Pass == PASS_1 ) {
                sym->total_length++;
                sym->total_size += no_of_bytes;
                if( first ) {
                    sym->first_length++;
                    sym->first_size += no_of_bytes;
                }
            }
            if( !struct_field ) {
            #endif
                while( count < no_of_bytes ) {
                    AsmDataByte( value );
                    value >>= 8;
                    count++;
                    if( count == sizeof( value ) ) {
                        value = AsmBuffer[cur_pos]->extra_value;
                    }
                }
            #ifdef _WASM_
            } else {
                if( the_struct == NULL ) break;
                Definition.curr_struct->e.structinfo->size += no_of_bytes;
                the_struct->total_size+=no_of_bytes;
                the_struct->total_length++;
                if( first ) {
                    the_struct->first_size+=no_of_bytes;
                    the_struct->first_length++;
                }
            }
            #endif
            break;
        case T_COMMA:
            #ifdef _WASM_
            first = FALSE;
            #endif
            if( cur_pos != start_pos ) {
                if( AsmBuffer[cur_pos - 1]->token == T_COMMA ) {
                    AsmError( EXPECTING_NUMBER );
                    return( ERROR );
                    /********SHOULD WE DO IT THIS WAY?*********
                    for( count = 0; count < no_of_bytes; count++ ) {
                        AsmDataByte( 0x00 );
                    }
                    ******************************************/
                }
            } else {
                AsmError( EXPECTING_NUMBER );
                return( ERROR );
            }
            if( cur_pos == ( Token_Count - 1 ) ) {
                More_Array_Element = TRUE;
                Last_Element_Size = no_of_bytes;
            }
            break;
        case T_STRING:
            if( no_of_bytes != 1 ) {
                if( AsmBuffer[cur_pos]->value > no_of_bytes ) {
                    AsmError( INITIALIZER_OUT_OF_RANGE );
                    return( ERROR );
                }
            }
            count = 0;
            char_ptr = AsmBuffer[cur_pos]->string_ptr;

            /* anything bigger than a byte must be stored in little-endian
             * format -- LSB first */
            little_endian( char_ptr, no_of_bytes );
            #ifdef _WASM_
            if( no_of_bytes == 1 && struct_field ) {
                no_of_bytes = AsmBuffer[cur_pos]->value;
            }
            if( sym && Parse_Pass == PASS_1 ) {
                sym->total_length++;
                sym->total_size += no_of_bytes;
                if( first ) {
                    sym->first_length++;
                    sym->first_size += no_of_bytes;
                }
            }
            if( !struct_field ) {
            #endif
                while( count < AsmBuffer[cur_pos]->value ) {
                    AsmDataByte( *char_ptr );
                    char_ptr++;
                    count++;
                }
            #ifdef _WASM_
            } else {
                if( the_struct == NULL ) break;
                Definition.curr_struct->e.structinfo->size += no_of_bytes;
                the_struct->total_size+=no_of_bytes;
                the_struct->total_length++;
                if( first ) {
                    the_struct->first_size+=no_of_bytes;
                    the_struct->first_length++;
                }
            }
            #endif
            break;
        case T_ID:
        {
            int i;
            int temp;
            asm_sym *init_sym;
            char *ptr;
            long data = 0;
            struct asmfixup     *fixup;
            /* temporary test .. if this works, combine code for id & resid */
#ifdef _WASM_
            i = ++cur_pos;
            if( check_override( &i ) == ERROR ) {
                return( ERROR );
            }
            i--;
            cur_pos = i;

            switch( ExpandSymbol( i, FALSE ) ) {
            case ERROR:
                return( ERROR );
            case STRING_EXPANDED:
                continue;
            }
#endif

            init_sym = AsmLookup( AsmBuffer[cur_pos]->string_ptr );

            if( init_sym == NULL ) return( ERROR );

            #ifdef _WASM_
            switch( init_sym->state ) {
            case SYM_STRUCT_FIELD:
                AsmBuffer[cur_pos]->token = T_NUM;
                AsmBuffer[cur_pos]->value = init_sym->offset;
                continue;
            case SYM_GRP:
            case SYM_SEG:
                temp = FIX_SEG;
                break;
            default:
            #endif

            switch( no_of_bytes ) {
            case 1:
                AsmError( OFFSET_TOO_SMALL ); // fixme
                return( ERROR );
            case 2:
                temp = FIX_OFF16;
                break;
            case 4:
                if( Code->use32 ) {
                    temp = FIX_OFF32;
                } else {
                    temp = FIX_PTR16;
                }
                break;
            case 6:
                // fixme -- this needs work .... check USE_32, etc
                temp = FIX_PTR32;
                Code->info.opnd_type[0] = OP_J48;
                break;
            default:
                AsmError( NOT_IMPLEMENTED );
                return( ERROR );
            }
            #ifdef _WASM_
                /* switch( init_sym->state ) from above */
            }
            #endif

            fixup = AddFixup( init_sym, temp );
//          if( fixup == NULL ) return( ERROR );
            // fixme
            InsFixups[0] = fixup;
            data += fixup->offset;

            for( cur_pos++;
                AsmBuffer[cur_pos]->token != T_FINAL && AsmBuffer[cur_pos]->token != T_COMMA;
                cur_pos++ ) {
                switch( AsmBuffer[cur_pos]->token ) {
                case T_PLUS:
                case T_DOT:
                case T_OP_SQ_BRACKET:
                    break;
                #ifdef _WASM_
                case T_ID:
                    init_sym = AsmLookup( AsmBuffer[cur_pos]->string_ptr );
                    data += init_sym->offset;
                    break;
                #endif
                case T_MINUS:
                    if( AsmBuffer[cur_pos+1]->token != T_NUM ) {
                        AsmError( EXPECTING_NUMBER );
                        return( ERROR );
                    }
                    AsmBuffer[cur_pos+1]->value *=-1;
                    break;
                case T_NUM:
                    data += AsmBuffer[cur_pos]->value;
                }
            }
            cur_pos--;

            #ifdef _WASM_
                if( store_fixup( 0 ) == ERROR ) return( ERROR );
            #endif
            /* now actually output the data */
            ptr = (char *)&data;
            #ifdef _WASM_
            if( sym && Parse_Pass == PASS_1 ) {
                sym->total_length++;
                sym->total_size += no_of_bytes;
                if( first ) {
                    sym->first_length++;
                    sym->first_size += no_of_bytes;
                }
            }
            if( !struct_field ) {
            #endif
                /* only output up to 4 bytes of offset (segment is on fixup) */
                for( i = 0; i < min( no_of_bytes, 4 ); i++ ) {
                    AsmDataByte( *ptr );
                    ptr++;
                }
                /* leave space for segment */
                for( ; i < no_of_bytes; i++ ) {
                    AsmDataByte( 0 );
                }
            #ifdef _WASM_
            } else {
                if( the_struct == NULL ) break;
                Definition.curr_struct->e.structinfo->size += no_of_bytes;
                the_struct->total_size+=no_of_bytes;
                the_struct->total_length++;
                if( first ) {
                    the_struct->first_size+=no_of_bytes;
                    the_struct->first_length++;
                }
            }
            #endif
            break;
        }
        case T_UNARY_OPERATOR:
        {
            int i;
            int temp;
            int seg_off_operator_loc = 0;
            asm_sym *init_sym;
            char *ptr;
            long data = 0;
            struct asmfixup     *fixup;

            if( AsmBuffer[cur_pos]->value == T_OFFSET ||
                AsmBuffer[cur_pos]->value == T_SEG2 ) {
                // see asmins.h about T_SEG2
                if( no_of_bytes < 2 ) {
                    AsmError( OFFSET_TOO_SMALL );
                    return( ERROR );
                }
            }
            seg_off_operator_loc = cur_pos;
#ifdef _WASM_
            i = ++cur_pos;
            if( check_override( &i ) == ERROR ) {
                return( ERROR );
            }
            i--;
            cur_pos = i;
#endif
            if( cur_pos + 1 < Token_Count ) {
                if( AsmBuffer[++cur_pos]->token == T_ID ) {
                    init_sym = AsmLookup( AsmBuffer[cur_pos]->string_ptr );
                    if( init_sym == NULL ) return( ERROR );

                    if( AsmBuffer[seg_off_operator_loc]->value == T_OFFSET ) {
                        if( init_sym->state == SYM_STACK ) {
                            AsmError( CANNOT_OFFSET_AUTO );
                            return( ERROR );
#ifdef _WASM_
                        } else if( init_sym->state == SYM_GRP ) {
                            AsmError( CANNOT_OFFSET_GRP );
                            return( ERROR );
#endif
                        }
                        switch( no_of_bytes ) {
                        case 1:
                            AsmError( OFFSET_TOO_SMALL ); // fixme
                            return( ERROR );
                        case 2:
                            temp = FIX_OFF16;
                            break;
                        case 4:
                            temp = FIX_OFF32;
                            break;
                        default:
                            AsmError( NOT_IMPLEMENTED );
                            return( ERROR );
                        }
                    } else if( AsmBuffer[seg_off_operator_loc]->value == T_SEG2 ) {
                        if( init_sym->state == SYM_STACK ) {
                            AsmError( CANNOT_SEG_AUTO );
                        }
                        temp = FIX_SEG;
                    }

                    switch( AsmBuffer[seg_off_operator_loc]->value ) {
                    case T_OFFSET:
                        #ifdef _WASM_
                            if( init_sym->state == SYM_STRUCT_FIELD ) {
                                data = init_sym->offset;
                                break;
                            }
                        #endif
                    case T_SEG2:

                        fixup = AddFixup( init_sym, temp );
                        InsFixups[0] = fixup;
                        if( AsmBuffer[seg_off_operator_loc]->value == T_OFFSET ) {
                            data += fixup->offset;
                        }
                        #ifdef _WASM_
                            if( store_fixup( 0 ) == ERROR ) return( ERROR );
                        #endif
                        break;
                    #ifdef _WASM_
                    case T_LENGTH:
                        data = sym->first_length;
                        break;
                    case T_LENGTHOF:
                        data = sym->total_length;
                        break;
                    case T_SIZE:
                        data = sym->first_size;
                        break;
                    case T_SIZEOF:
                        data = sym->total_size;
                        break;
                    #endif
                    default:
                        AsmError( SYNTAX_ERROR );
                        return( ERROR );
                    }
                    for( cur_pos++; AsmBuffer[cur_pos]->token != T_FINAL; cur_pos++ ) {
                        switch( AsmBuffer[cur_pos]->token ) {
                        case T_PLUS:
                        case T_DOT:
                        case T_OP_SQ_BRACKET:
                            break;
                        #ifdef _WASM_
                        case T_ID:
                            init_sym = AsmLookup( AsmBuffer[cur_pos]->string_ptr );
                            data += init_sym->offset;
                            break;
                        #endif
                        case T_MINUS:
                            if( AsmBuffer[cur_pos+1]->token != T_NUM ) {
                                AsmError( EXPECTING_NUMBER );
                                return( ERROR );
                            }
                            AsmBuffer[cur_pos+1]->value *=-1;
                            break;
                        case T_NUM:
                            data += AsmBuffer[cur_pos]->value;
                        }
                        if( AsmBuffer[cur_pos]->token == T_COMMA ) break;
                    }
                    cur_pos--;

                    /* now actually output the data */
                    ptr = (char *)&data;
                    #ifdef _WASM_
                    if( sym && Parse_Pass == PASS_1 ) {
                        sym->total_length++;
                        sym->total_size += no_of_bytes;
                        if( first ) {
                            sym->first_length++;
                            sym->first_size += no_of_bytes;
                        }
                    }
                    if( !struct_field ) {
                    #endif
                        for( i = 0; i < no_of_bytes; i++ ) {
                            AsmDataByte( *ptr );
                            ptr++;
                        }
                    #ifdef _WASM_
                    } else {
                        if( the_struct == NULL ) break;
                        Definition.curr_struct->e.structinfo->size += no_of_bytes;
                        the_struct->total_size+=no_of_bytes;
                        the_struct->total_length++;
                        if( first ) {
                            the_struct->first_size+=no_of_bytes;
                            the_struct->first_length++;
                        }
                    }
                    #endif
                } else {
                    AsmError( SYNTAX_ERROR );
                    return( ERROR );
                }
            }
            // AsmError( NOT_IMPLEMENTED );
            break;
        }
        case T_CL_BRACKET:
            return( cur_pos );
        default:
            AsmError( EXPECTING_NUMBER );
            return( ERROR );
        }
        cur_pos++;
    }
    return( cur_pos );
}

int dup_array( asm_sym *sym, char start_pos, char no_of_bytes )
/*************************************************************/
/*
  parse array with DUP operator;
*/
{
    int                 cur_pos = start_pos;
    int                 returned_pos;
    int                 count;

    #ifdef _WASM_
        bool            was_first;
        ExpandTheWorld( start_pos, FALSE );
    #endif
    while( cur_pos + 2 < Token_Count ) {
        if( AsmBuffer[cur_pos + 1]->token == T_RES_ID  &&
            AsmBuffer[cur_pos + 1]->value == T_DUP ) {
            if( AsmBuffer[cur_pos]->token != T_NUM ) {
                AsmError( SYNTAX_ERROR );
                return( ERROR );
            }
            count = AsmBuffer[cur_pos]->value;
            cur_pos += 2;
            if( AsmBuffer[cur_pos]->token != T_OP_BRACKET ) {
                AsmError( SYNTAX_ERROR );
                return( ERROR );
            }

            cur_pos++;
            #ifdef _WASM_
            was_first = first;
            #endif
            while( count > 0 ) {
                /* in case there was a "," inside the dup */
                #ifdef _WASM_
                first = was_first;
                #endif
                returned_pos = array_element(sym,cur_pos,no_of_bytes );
                if( returned_pos == ERROR ) return( ERROR );
                count--;
            }
            if( AsmBuffer[returned_pos]->token != T_CL_BRACKET ) {
                AsmError( BRACKET_EXPECTED );
                return( ERROR );
            }
        } else {
            returned_pos = array_element(sym,cur_pos,no_of_bytes );
            if( returned_pos == ERROR ) return( ERROR );
            if( AsmBuffer[returned_pos]->token != T_CL_BRACKET ) {
                /* array_element hit T_FINAL so stop */
                return( returned_pos );
            }
        }
        if( AsmBuffer[returned_pos+1]->token != T_COMMA ) {
            return( returned_pos );
        }
        cur_pos = returned_pos + 2;
        #ifdef _WASM_
        first = FALSE;
        #endif
    }
    return( array_element(sym,cur_pos,no_of_bytes ) );
}

int data_init( int sym_loc, int initializer_loc )
/***********************************************/
/*
  parse data initialization assembly line;
*/
{
    char                no_of_bytes;
    int                 mem_type;
    struct asm_sym      *sym = NULL;
#ifdef _WASM_
    uint                old_offset;
    char                label_dir = FALSE;

    struct_field = FALSE;
    first = TRUE;
#endif

    if( sym_loc >= 0 ) {
        sym = AsmLookup( AsmBuffer[sym_loc]->string_ptr );
        if( sym == NULL ) return( ERROR );
    }

    switch( AsmBuffer[initializer_loc]->value ) {
#ifdef _WASM_
    case T_SBYTE:                       // 20-Aug-92
        mem_type = T_SBYTE;
        no_of_bytes = BYTE_1;
        break;
    case T_SWORD:                       // 20-Aug-92
        mem_type = T_SWORD;
        no_of_bytes = BYTE_2;
        break;
    case T_SDWORD:                      // 20-Aug-92
        mem_type = T_SDWORD;
        no_of_bytes = BYTE_4;
        break;
    case T_DQ:
    case T_QWORD:
        mem_type = T_QWORD;
        no_of_bytes = BYTE_8;
        break;
    case T_DT:
    case T_TBYTE:                       // 20-Aug-92
        mem_type = T_TBYTE;
        no_of_bytes = BYTE_10;
        break;
    case T_STRUC:
    case T_STRUCT:
        mem_type = T_STRUCT;
        no_of_bytes = GetStructSize( initializer_loc );
        if( Definition.struct_depth == 0 ) {
            InitializeStructure( sym, initializer_loc );
        }
        break;
#endif
    case T_DB:
    case T_BYTE:
        mem_type = T_BYTE;
        no_of_bytes = BYTE_1;
        break;
    case T_DW:
    case T_WORD:
        mem_type = T_WORD;
        no_of_bytes = BYTE_2;
        break;
    case T_DD:
    case T_DWORD:
        mem_type = T_DWORD;
        no_of_bytes = BYTE_4;
        break;
    case T_DF:                          // 20-Aug-92
    case T_FWORD:
    case T_DP:
    case T_PWORD:
        mem_type = T_FWORD;
        no_of_bytes = BYTE_6;
        break;
    default:
        AsmError( INVALID_LABEL_DEFINITION );
        return( ERROR );
    }
    if( AsmBuffer[ initializer_loc + 1 ]->token == T_FINAL ) {
        AsmError( SYNTAX_ERROR );
        return( ERROR );
    }

    #ifdef _WASM_
        if( sym_loc >= 0 && AsmBuffer[ sym_loc ]->value == T_LABEL ) {
            label_dir = TRUE;
            sym_loc--;
        }
        if( sym_loc < 0 ) {
            if( Definition.struct_depth != 0 ) {
                if( Parse_Pass == PASS_1 ) {
                    AddFieldToStruct( initializer_loc );
                    struct_field = TRUE;
                } else {
                    return( NOT_ERROR );
                }
            }
        }
    #endif

    if( More_Array_Element == TRUE ) {
        More_Array_Element = FALSE;
    } else if( sym_loc >= 0 ) {
        #ifdef _WASM_
            /* defining a field in a structure */
            if( Definition.struct_depth != 0 ) {
                if( Parse_Pass == PASS_1 ) {
                    sym->offset = AddFieldToStruct( initializer_loc );
                    struct_field = TRUE;
                    sym->state = SYM_STRUCT_FIELD;
                    sym->mem_type = mem_type;
                    if( dup_array( sym, initializer_loc + 1, no_of_bytes ) == ERROR ) {
                        return( ERROR );
                    }
                }
                return( NOT_ERROR );
            }

            if( Parse_Pass == PASS_1 ) {
                if( sym->state != SYM_UNDEFINED ) {
                    // redefine label
                    AsmError( SYMBOL_ALREADY_DEFINED );
                    return( ERROR );
                }
            } else {
                old_offset = sym->offset;
            }
            GetSymInfo( sym );
            if( Parse_Pass != PASS_1 && sym->offset != old_offset ) {
                PhaseError = TRUE;
            }
        #else
            if( sym->state != SYM_UNDEFINED ) {
                // redefine label
                AsmError( SYMBOL_ALREADY_DEFINED );
                return( ERROR );
            }
        #endif
        sym->state = SYM_INTERNAL;
        sym->mem_type = mem_type;
        BackPatch( sym );
    }
    #ifdef _WASM_
        if( mem_type == T_STRUCT ) return( NOT_ERROR );
        if( label_dir ) return( NOT_ERROR );
    #endif
    if( dup_array( sym, initializer_loc + 1, no_of_bytes ) == ERROR ) {
        return( ERROR );
    }
    return( NOT_ERROR );
}
