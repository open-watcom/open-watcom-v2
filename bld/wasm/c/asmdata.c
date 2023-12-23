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
* Description:  data directive DB,DW,DD,... and structure processing
*
****************************************************************************/


#include "asmglob.h"
#include "asmexpnd.h"
#include "tbyte.h"
#include "asmfixup.h"
#if defined( _STANDALONE_ )
  #include "directiv.h"
  #include "asmstruc.h"
  #include "queues.h"
#endif

#include "clibext.h"


#if defined( _STANDALONE_ )

/* static globals */
/* is this data element a field in a structure definition? */
static bool             struct_field;
/* is this the first initializer for this field? */
static bool             first_init;

#endif

static token_idx dup_array( asm_sym *sym, asm_sym *struct_sym, token_buffer *tokbuf, token_idx start_pos, unsigned no_of_bytes );

static bool             More_Array_Element = false;
static unsigned         Last_Element_Size;

/* data initialization stuff */

static void little_endian( char *string, unsigned no_of_bytes )
/*************************************************************/
/* convert a string into little endian format - ( LSB 1st, LSW 1st ... etc ) */
{
    if( no_of_bytes > 1 ) {
        strrev( string );
    }
    return;
}

static void output_float( token_buffer *tokbuf, token_idx index, unsigned no_of_bytes, bool negative )
/****************************************************************************************************/
{
    double              double_value;
    float               float_value;
    char                *char_ptr = NULL;
    unsigned            count;
    TB_LD               tbyte;

    if( no_of_bytes == BYTE_10 ) {
        char_ptr = (char *)strtotb( tokbuf->tokens[index].string_ptr, &tbyte, negative );
    } else {
        double_value = strtod( tokbuf->tokens[index].string_ptr, NULL );
        if( negative )
            double_value = -double_value;
        switch( no_of_bytes ) {
        case BYTE_1:
        case BYTE_2:
        default:
#if defined( _STANDALONE_ )
            AsmWarn( 4, FLOAT_OPERAND );
#endif
            char_ptr = (char *)&tokbuf->tokens[index].u.value;
            break;
        case BYTE_4:
            float_value = (float)double_value;
            char_ptr = (char *)&float_value;
            break;
        case BYTE_8:
            char_ptr = (char *)&double_value;
            break;
        }
    }

    for( count = 0; count < no_of_bytes; ++count ) {
        AsmDataByte( *char_ptr++ );
    }
    return;
}

#if defined( _STANDALONE_ )
static void update_sizes( asm_sym *sym, bool first, unsigned no_of_bytes )
/************************************************************************/
{
    sym->total_length++;
    sym->total_size += no_of_bytes;
    if( first ) {
        sym->first_length++;
        sym->first_size += no_of_bytes;
    }
}

static bool check_override( token_buffer *tokbuf, token_idx *i )
/***************************************************************
 * Check if there is a register, segment or group override
 */
{
    token_idx   index;

    index = *i;

    if( ( index + 2 ) < tokbuf->count ) {
        if( tokbuf->tokens[index+1].class == TC_COLON ) {
            switch( tokbuf->tokens[index].class ) {
            case TC_REG:
                Code->prefix.seg =
                    AsmOpTable[AsmOpcode[tokbuf->tokens[index].u.token].position].opcode;
                (*i) += 2;
                if( *i >= tokbuf->count ) {
                    AsmError( LABEL_EXPECTED_AFTER_COLON );
                    return( RC_ERROR );
                }
                break;
            case TC_ID:      // Segment or Group override
                if( FixOverride( tokbuf, *i ) ) {
                    return( RC_ERROR );
                }
                (*i) += 2;
                if( *i >= tokbuf->count ) {
                    AsmError( LABEL_EXPECTED_AFTER_COLON );
                    return( RC_ERROR );
                }
                break;
            default:
                break;
            }
        }
    }
    return( RC_OK );
}
#endif

static token_idx array_element( asm_sym *sym, asm_sym *struct_sym, token_buffer *tokbuf, token_idx start_pos, unsigned no_of_bytes )
/***********************************************************************************************************************************
 * - parse an array and initialize the number;
 * - call by dup_array() only;
 */
{
    token_idx           cur_pos = start_pos;
    unsigned            count;
    char                *char_ptr = NULL;
    bool                negative = false;

#if defined( _STANDALONE_ )
    asm_sym             *the_struct;
    token_idx           tmp;

    the_struct = (asm_sym*)Definition.curr_struct;

    if( sym != NULL ) {
        sym->count++;
    }
#endif

    for( cur_pos = start_pos;
        ( cur_pos < tokbuf->count ) && ( tokbuf->tokens[cur_pos].class != TC_FINAL );
        cur_pos++ ) {
#if defined( _STANDALONE_ )
        if( tokbuf->tokens[cur_pos].class == TC_RES_ID )
            continue;
        tmp = cur_pos;
        if( check_override( tokbuf, &tmp ) )
            return( INVALID_IDX );
        cur_pos = tmp;
#endif
        if(( cur_pos == tokbuf->count - 1 )
            && ( tokbuf->tokens[cur_pos].class == TC_CL_BRACKET ))
            break;
        switch( tokbuf->tokens[cur_pos].class ) {
        case TC_QUESTION_MARK:
            if( cur_pos != start_pos ) {
                if( tokbuf->tokens[cur_pos - 1].class != TC_COMMA ) {
                    AsmError( EXPECTING_COMMA );
                    return( INVALID_IDX );
                }
            }
#if defined( _STANDALONE_ )
            if( !struct_field ) {
                ChangeCurrentLocation( true, no_of_bytes,
                      ( ( CurrSeg != NULL ) && SEGISCODE( CurrSeg ) ) );
            } else {
                Definition.curr_struct->e.structinfo->size += no_of_bytes;
                the_struct->total_size += no_of_bytes;
                the_struct->total_length++;
                the_struct->first_size += no_of_bytes;
                the_struct->first_length++;
            }

            if( sym != NULL && Parse_Pass == PASS_1 ) {
                update_sizes( sym, first_init, no_of_bytes );
            }
#else
            for( count = 0; count < no_of_bytes; ++count ) {
                AsmDataByte( 0 );
            }
#endif
            break;
        case TC_MINUS:
            switch( tokbuf->tokens[cur_pos+1].class ) {
            case TC_NUM:
                tokbuf->tokens[cur_pos+1].u.value = -tokbuf->tokens[cur_pos+1].u.value;
                tokbuf->tokens[cur_pos].class = TC_PLUS;
                break;
            case TC_FLOAT:
                negative = true;
                break;
            default:
                AsmError( EXPECTING_NUMBER );
                return( INVALID_IDX );
            }
            break;
        case TC_PLUS:
        case TC_DOT:
            break; // go around again
        case TC_NUM:
        case TC_FLOAT:
            if( tokbuf->tokens[cur_pos+1].class == TC_RES_ID &&
                tokbuf->tokens[cur_pos+1].u.token == T_DUP ) {
                cur_pos = dup_array( sym, struct_sym, tokbuf, cur_pos, no_of_bytes );
                if( ISINVALID_IDX( cur_pos ) )
                    return( INVALID_IDX );
                break;
            }

            if( cur_pos != start_pos ) {
                switch( tokbuf->tokens[cur_pos - 1].class ) {
                case TC_COMMA:
                    break;
                case TC_PLUS:
                case TC_MINUS:
                    if( tokbuf->tokens[cur_pos - 2].class == TC_COMMA )
                        break;
                    if( cur_pos - 1 == start_pos )
                        break;
                    /* fall through */
                default:
                    AsmError( EXPECTING_COMMA );
                    return( INVALID_IDX );
                }
            }
            if( tokbuf->tokens[cur_pos].class == TC_FLOAT ) {
                output_float( tokbuf, cur_pos, no_of_bytes, negative );
                negative = false;
                break;
            }
            char_ptr = (char *)tokbuf->tokens[cur_pos].u.bytes;
#if defined( _STANDALONE_ )
            if( sym != NULL && Parse_Pass == PASS_1 ) {
                update_sizes( sym, first_init, no_of_bytes );
            }
            if( !struct_field ) {
#endif
                for( count = 0; count < no_of_bytes; ++count ) {
                    AsmDataByte( *char_ptr++ );
                }
#if defined( _STANDALONE_ )
            } else {
                if( the_struct == NULL )
                    break;
                Definition.curr_struct->e.structinfo->size += no_of_bytes;
                update_sizes( the_struct, first_init, no_of_bytes );
            }
#endif
            break;
        case TC_COMMA:
#if defined( _STANDALONE_ )
            first_init = false;
#endif
            if( cur_pos != start_pos ) {
                if( tokbuf->tokens[cur_pos - 1].class == TC_COMMA ) {
                    AsmError( EXPECTING_NUMBER );
                    return( INVALID_IDX );
                    /********SHOULD WE DO IT THIS WAY?*********
                    for( count = 0; count < no_of_bytes; count++ ) {
                        AsmDataByte( 0x00 );
                    }
                    ******************************************/
                }
            } else {
                AsmError( EXPECTING_NUMBER );
                return( INVALID_IDX );
            }
            if( cur_pos == ( tokbuf->count - 1 ) ) {
                More_Array_Element = true;
                Last_Element_Size = no_of_bytes;
            }
            break;
        case TC_STRING:
#if defined( _STANDALONE_ )
            if( struct_sym != NULL ) {
                InitializeStructure( sym, struct_sym, tokbuf, cur_pos );
                break;
            }
#endif
            if( no_of_bytes != 1 ) {
                if( (unsigned)tokbuf->tokens[cur_pos].u.value > no_of_bytes ) {
                    AsmError( INITIALIZER_OUT_OF_RANGE );
                    return( INVALID_IDX );
                }
            }
            char_ptr = tokbuf->tokens[cur_pos].string_ptr;

            /* anything bigger than a byte must be stored in little-endian
            * format -- LSB first */
            little_endian( char_ptr, no_of_bytes );
#if defined( _STANDALONE_ )
            if( no_of_bytes == 1 && struct_field ) {
                no_of_bytes = tokbuf->tokens[cur_pos].u.value;
            }
            if( sym != NULL && Parse_Pass == PASS_1 ) {
                update_sizes( sym, first_init, no_of_bytes );
            }
            if( !struct_field ) {
#endif
                for( count = tokbuf->tokens[cur_pos].u.value; count > 0 ; --count ) {
                    AsmDataByte( *char_ptr++ );
                }
                for( count = tokbuf->tokens[cur_pos].u.value; count < no_of_bytes; ++count ) {
                    AsmDataByte( 0 );
                    char_ptr++;
                }
#if defined( _STANDALONE_ )
            } else {
                if( the_struct == NULL )
                    break;
                Definition.curr_struct->e.structinfo->size += no_of_bytes;
                update_sizes( the_struct, first_init, no_of_bytes );
            }
#endif
            break;
        case TC_ID:
          {
            token_idx           i;
            enum fixup_types    fixup_type = 0;
            asm_sym             *init_sym;
            char                *ptr;
            long                data = 0;
            struct asmfixup     *fixup;
#if defined( _STANDALONE_ )
            bool                expanded;
#endif

            /* temporary test .. if this works, combine code for id & resid */
#if defined( _STANDALONE_ )
            i = ++cur_pos;
            if( check_override( tokbuf, &i ) ) {
                return( INVALID_IDX );
            }
            i--;
            cur_pos = i;

            if( ExpandSymbol( tokbuf, i, false, &expanded ) )
                return( INVALID_IDX );
            if( expanded ) {
                continue;
            }
#endif

            init_sym = AsmLookup( tokbuf->tokens[cur_pos].string_ptr );

            if( init_sym == NULL )
                return( INVALID_IDX );

#if defined( _STANDALONE_ )
            switch( init_sym->state ) {
            case SYM_STRUCT_FIELD:
                tokbuf->tokens[cur_pos].class = TC_NUM;
                tokbuf->tokens[cur_pos].u.value = init_sym->offset;
                continue;
            case SYM_GRP:
            case SYM_SEG:
                fixup_type = FIX_SEG;
                break;
            default:
#endif
                switch( no_of_bytes ) {
                case 1:
                    AsmError( OFFSET_TOO_SMALL ); // fixme
                    return( INVALID_IDX );
                case 2:
                    fixup_type = FIX_OFF16;
                    break;
                case 4:
                    if( Code->use32 ) {
                        fixup_type = FIX_OFF32;
                    } else {
                        fixup_type = FIX_PTR16;
                    }
                    break;
                case 6:
                    // fixme -- this needs work .... check USE32, etc
                    fixup_type = FIX_PTR32;
                    Code->info.opnd_type[OPND1] = OP_J48;
                    break;
                default:
                    AsmError( NOT_IMPLEMENTED );
                    return( INVALID_IDX );
                }
#if defined( _STANDALONE_ )
                /* switch( init_sym->state ) from above */
            }
            find_frame( init_sym );
#endif
            fixup = AddFixup( init_sym, fixup_type, OPTJ_NONE );
            //          if( fixup == NULL ) return( INVALID_IDX );
            // fixme
            InsFixups[OPND1] = fixup;
            data += fixup->u_offset;

            for( cur_pos++;
                ( cur_pos < tokbuf->count ) && ( tokbuf->tokens[cur_pos].class != TC_FINAL )
                    && ( tokbuf->tokens[cur_pos].class != TC_COMMA )
                    && ( tokbuf->tokens[cur_pos].class != TC_CL_BRACKET );
                cur_pos++ ) {
                switch( tokbuf->tokens[cur_pos].class ) {
                case TC_PLUS:
                case TC_DOT:
                case TC_OP_SQ_BRACKET:
                    break;
#if defined( _STANDALONE_ )
                case TC_ID:
                    init_sym = AsmLookup( tokbuf->tokens[cur_pos].string_ptr );
                    data += init_sym->offset;
                    break;
#endif
                case TC_MINUS:
                    if( tokbuf->tokens[cur_pos+1].class != TC_NUM ) {
                        AsmError( EXPECTING_NUMBER );
                        return( INVALID_IDX );
                    }
                    tokbuf->tokens[cur_pos+1].u.value = -tokbuf->tokens[cur_pos+1].u.value;
                    break;
                case TC_NUM:
                    data += tokbuf->tokens[cur_pos].u.value;
                }
            }

#if defined( _STANDALONE_ )
            if( store_fixup( OPND1 ) )
                return( INVALID_IDX );
#endif
            /* now actually output the data */
            ptr = (char *)&data;
#if defined( _STANDALONE_ )
            if( sym != NULL && Parse_Pass == PASS_1 ) {
                update_sizes( sym, first_init, no_of_bytes );
            }
            if( !struct_field ) {
#endif
                /* only output up to 4 bytes of offset (segment is on fixup) */
                for( i = 0; i < no_of_bytes; i++ ) {
                    if( i < 4 ) {
                        /* copy offset */
                        AsmDataByte( *ptr++ );
                    } else {
                        /* leave space for segment */
                        AsmDataByte( 0 );
                    }
                }
#if defined( _STANDALONE_ )
            } else if( the_struct != NULL ) {
                Definition.curr_struct->e.structinfo->size += no_of_bytes;
                update_sizes( the_struct, first_init, no_of_bytes );
            }
#endif
            // set position back to main loop worked correctly
            cur_pos--;
            break;
          }
        case TC_UNARY_OPERATOR:
          {
            token_idx           i;
            enum fixup_types    fixup_type = 0;
            token_idx           seg_off_operator_loc = 0;
            asm_sym             *init_sym;
            char                *ptr;
            long                data = 0;
            struct asmfixup     *fixup;

            if( tokbuf->tokens[cur_pos].u.token == T_OFFSET ||
                tokbuf->tokens[cur_pos].u.token == T_SEG ) {
                // see asmins.h about T_SEG
                if( no_of_bytes < 2 ) {
                    AsmError( OFFSET_TOO_SMALL );
                    return( INVALID_IDX );
                }
            }
            seg_off_operator_loc = cur_pos;
#if defined( _STANDALONE_ )
            i = ++cur_pos;
            if( i + 2 < tokbuf->count ) {
                if( ( tokbuf->tokens[i].class == TC_RES_ID )
                    && ( tokbuf->tokens[i + 1].class == TC_RES_ID )
                    && ( tokbuf->tokens[i + 1].u.token == T_PTR ) ) {
                    i += 2;
                }
            }
            if( check_override( tokbuf, &i ) ) {
                return( INVALID_IDX );
            }
            i--;
            cur_pos = i;
#endif
            if( cur_pos + 1 < tokbuf->count ) {
                if( tokbuf->tokens[++cur_pos].class == TC_ID ) {
                    init_sym = AsmLookup( tokbuf->tokens[cur_pos].string_ptr );
                    if( init_sym == NULL )
                        return( INVALID_IDX );

                    if( tokbuf->tokens[seg_off_operator_loc].u.token == T_OFFSET ) {
                        if( init_sym->state == SYM_STACK ) {
                            AsmError( CANNOT_OFFSET_AUTO );
                            return( INVALID_IDX );
#if defined( _STANDALONE_ )
                        } else if( init_sym->state == SYM_GRP ) {
                            AsmError( CANNOT_OFFSET_GRP );
                            return( INVALID_IDX );
#endif
                        }
                        switch( no_of_bytes ) {
                        case 1:
                            AsmError( OFFSET_TOO_SMALL ); // fixme
                            return( INVALID_IDX );
                        case 2:
                            fixup_type = FIX_OFF16;
                            break;
                        case 4:
                            fixup_type = FIX_OFF32;
#if defined( _STANDALONE_ )
                            if( !SymIs32( init_sym ) ) {
                                fixup_type = FIX_OFF16;
                            }
#endif
                            break;
                        default:
                            AsmError( NOT_IMPLEMENTED );
                            return( INVALID_IDX );
                        }
                    } else if( tokbuf->tokens[seg_off_operator_loc].u.token == T_SEG ) {
                        if( init_sym->state == SYM_STACK ) {
                            AsmError( CANNOT_SEG_AUTO );
                        }
                        fixup_type = FIX_SEG;
                    }

                    switch( tokbuf->tokens[seg_off_operator_loc].u.token ) {
                    case T_OFFSET:
#if defined( _STANDALONE_ )
                        if( init_sym->state == SYM_STRUCT_FIELD ) {
                            data = init_sym->offset;
                            break;
                        }
#endif
                    case T_SEG:
#if defined( _STANDALONE_ )
                        find_frame( init_sym );
#endif
                        fixup = AddFixup( init_sym, fixup_type, OPTJ_NONE );
                        InsFixups[OPND1] = fixup;
                        if( tokbuf->tokens[seg_off_operator_loc].u.token == T_OFFSET ) {
                            data += fixup->u_offset;
                        }
#if defined( _STANDALONE_ )
                        if( store_fixup( OPND1 ) )
                            return( INVALID_IDX );
#endif
                        break;
#if defined( _STANDALONE_ )
                    case T_LENGTH:
                        data = init_sym->first_length;
                        break;
                    case T_LENGTHOF:
                        data = init_sym->total_length;
                        break;
                    case T_SIZE:
                        data = init_sym->first_size;
                        break;
                    case T_SIZEOF:
                        data = init_sym->total_size;
                        break;
#endif
                    default:
                        AsmError( SYNTAX_ERROR );
                        return( INVALID_IDX );
                    }
                    for( cur_pos++;
                        ( cur_pos < tokbuf->count ) && ( tokbuf->tokens[cur_pos].class != TC_FINAL)
                        && ( tokbuf->tokens[cur_pos].class != TC_COMMA );
                        cur_pos++ ) {
                        switch( tokbuf->tokens[cur_pos].class ) {
                        case TC_PLUS:
                        case TC_DOT:
                        case TC_OP_SQ_BRACKET:
                            break;
#if defined( _STANDALONE_ )
                        case TC_ID:
                            init_sym = AsmLookup( tokbuf->tokens[cur_pos].string_ptr );
                            data += init_sym->offset;
                            break;
#endif
                        case TC_MINUS:
                            if( tokbuf->tokens[cur_pos+1].class != TC_NUM ) {
                                AsmError( EXPECTING_NUMBER );
                                return( INVALID_IDX );
                            }
                            tokbuf->tokens[cur_pos+1].u.value = -tokbuf->tokens[cur_pos+1].u.value;
                            break;
                        case TC_NUM:
                            data += tokbuf->tokens[cur_pos].u.value;
                        }
                    }

                    /* now actually output the data */
                    ptr = (char *)&data;
#if defined( _STANDALONE_ )
                    if( sym != NULL && Parse_Pass == PASS_1 ) {
                        update_sizes( sym, first_init, no_of_bytes );
                    }
                    if( !struct_field ) {
#endif
                        for( i = 0; i < no_of_bytes; i++ ) {
                            AsmDataByte( *ptr );
                            ptr++;
                        }
#if defined( _STANDALONE_ )
                    } else {
                        if( the_struct == NULL )
                            break;
                        Definition.curr_struct->e.structinfo->size += no_of_bytes;
                        update_sizes( the_struct, first_init, no_of_bytes );
                    }
#endif
                } else {
                    AsmError( SYNTAX_ERROR );
                    return( INVALID_IDX );
                }
            }
            // set position back to main loop worked correctly
            cur_pos--;
            // AsmError( NOT_IMPLEMENTED );
            break;
          }
        case TC_CL_BRACKET:
            return( cur_pos );
        default:
            AsmError( EXPECTING_NUMBER );
            return( INVALID_IDX );
        }
    }
    return( cur_pos );
}

static token_idx dup_array( asm_sym *sym, asm_sym *struct_sym, token_buffer *tokbuf, token_idx start_pos, unsigned no_of_bytes )
/*******************************************************************************************************************************
 * parse array with DUP operator;
 */
{
    token_idx           cur_pos;
    token_idx           returned_pos = 0;
    unsigned            count;
#if defined( _STANDALONE_ )
    bool                was_first;
#endif

    ExpandTheWorld( tokbuf, start_pos, false, true );
    for( cur_pos = start_pos; cur_pos + 2 < tokbuf->count; ) {
        if(( tokbuf->tokens[cur_pos + 1].class == TC_RES_ID )
          && ( tokbuf->tokens[cur_pos + 1].u.token == T_DUP )) {
            if( tokbuf->tokens[cur_pos].class != TC_NUM ) {
                AsmError( SYNTAX_ERROR );
                return( INVALID_IDX );
            }
            count = tokbuf->tokens[cur_pos].u.value;
#if defined( _STANDALONE_ )
            if( sym != NULL )
                sym->count = 0;
#endif
            cur_pos += 2;
            if( tokbuf->tokens[cur_pos].class != TC_OP_BRACKET ) {
                AsmError( SYNTAX_ERROR );
                return( INVALID_IDX );
            }

            cur_pos++;
#if defined( _STANDALONE_ )
            was_first = first_init;
#endif
            if( count == 0 ) {
                int     level;
                /* zero count is valid, needs special processing */
                for( level = 0; tokbuf->tokens[cur_pos].class != TC_FINAL; cur_pos++ ) {
                    if( tokbuf->tokens[cur_pos].class == TC_OP_BRACKET ) {
                        level++;
                    } else if( tokbuf->tokens[cur_pos].class == TC_CL_BRACKET ) {
                        level--;
                    }
                    if( level < 0 ) {
                        break;
                    }
                }
                returned_pos = cur_pos;
            }
            for( ; count > 0; --count ) {
                /* in case there was a "," inside the dup */
#if defined( _STANDALONE_ )
                first_init = was_first;
#endif
                returned_pos = array_element( sym, struct_sym, tokbuf, cur_pos, no_of_bytes );
                if( ISINVALID_IDX( returned_pos ) ) {
                    return( INVALID_IDX );
                }
            }
            if( tokbuf->tokens[returned_pos].class != TC_CL_BRACKET ) {
                AsmError( BRACKET_EXPECTED );
                return( INVALID_IDX );
            }
        } else {
            returned_pos = array_element( sym, struct_sym, tokbuf, cur_pos, no_of_bytes );
            if( ISINVALID_IDX( returned_pos ) )
                return( INVALID_IDX );
            if( tokbuf->tokens[returned_pos].class != TC_CL_BRACKET ) {
                /* array_element hit TC_FINAL so stop */
                return( returned_pos );
            }
        }
        if( tokbuf->tokens[returned_pos + 1].class != TC_COMMA ) {
            return( returned_pos );
        }
        cur_pos = returned_pos + 2;
#if defined( _STANDALONE_ )
        first_init = false;
#endif
    }
    return( array_element( sym, struct_sym, tokbuf, cur_pos, no_of_bytes ) );
}

bool data_init( token_buffer *tokbuf, token_idx sym_loc, token_idx initializer_loc )
/***********************************************************************************
 * parse data initialization assembly line;
 */
{
    unsigned            no_of_bytes;
    memtype             mem_type;
    struct asm_sym      *sym = NULL;
    struct asm_sym      *struct_sym = NULL;
#if defined( _STANDALONE_ )
    uint                old_offset = 0;
    bool                label_dir = false;

    struct_field = false;
    first_init = true;
#endif

    if( ISVALID_IDX( sym_loc ) ) {
        sym = AsmLookup( tokbuf->tokens[sym_loc].string_ptr );
        if( sym == NULL ) {
            return( RC_ERROR );
        }
    }
    switch( tokbuf->tokens[initializer_loc].u.token ) {
#if defined( _STANDALONE_ )
    case T_SBYTE:                       // 20-Aug-92
        mem_type = MT_SBYTE;
        no_of_bytes = BYTE_1;
        break;
    case T_SWORD:                       // 20-Aug-92
        mem_type = MT_SWORD;
        no_of_bytes = BYTE_2;
        break;
    case T_SDWORD:                      // 20-Aug-92
        mem_type = MT_SDWORD;
        no_of_bytes = BYTE_4;
        break;
    case T_DQ:
    case T_QWORD:
        mem_type = MT_QWORD;
        no_of_bytes = BYTE_8;
        break;
    case T_DT:
    case T_TBYTE:                       // 20-Aug-92
        mem_type = MT_TBYTE;
        no_of_bytes = BYTE_10;
        break;
    case T_OWORD:
        mem_type = MT_OWORD;
        no_of_bytes = BYTE_16;
        break;
    case T_STRUC:
    case T_STRUCT:
        mem_type = MT_STRUCT;
        struct_sym = AsmLookup( tokbuf->tokens[initializer_loc].string_ptr );
        no_of_bytes = GetStructSize( struct_sym );
        if( Options.mode & MODE_IDEAL )
            sym->structure = struct_sym;
        break;
#endif
    case T_DB:
    case T_BYTE:
        mem_type = MT_BYTE;
        no_of_bytes = BYTE_1;
        break;
    case T_DW:
    case T_WORD:
        mem_type = MT_WORD;
        no_of_bytes = BYTE_2;
        break;
    case T_DD:
    case T_DWORD:
        mem_type = MT_DWORD;
        no_of_bytes = BYTE_4;
        break;
    case T_DF:                          // 20-Aug-92
    case T_FWORD:
    case T_DP:
    case T_PWORD:
        mem_type = MT_FWORD;
        no_of_bytes = BYTE_6;
        break;
    default:
        AsmError( INVALID_LABEL_DEFINITION );
        return( RC_ERROR );
    }
    if( tokbuf->tokens[initializer_loc + 1].class == TC_FINAL ) {
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }

#if defined( _STANDALONE_ )
    if( ISVALID_IDX( sym_loc ) && tokbuf->tokens[sym_loc].u.token == T_LABEL ) {
        label_dir = true;
        if( sym_loc > 0 ) {
            sym_loc--;
        } else {
            sym_loc = INVALID_IDX;
        }
    }
    if( ISINVALID_IDX( sym_loc ) ) {
        if( Definition.struct_depth != 0 ) {
            if( Parse_Pass == PASS_1 ) {
                AddFieldToStruct( sym, tokbuf, initializer_loc );
                struct_field = true;
            } else {
                return( RC_OK );
            }
        }
    }
#endif

    if( More_Array_Element ) {
        More_Array_Element = false;
    } else if( ISVALID_IDX( sym_loc ) ) {
#if defined( _STANDALONE_ )
        /* defining a field in a structure */
        if( Parse_Pass == PASS_1 ) {
            if( Definition.struct_depth != 0 ) {
                sym->offset = AddFieldToStruct( sym, tokbuf, initializer_loc );
                struct_field = true;
                sym->state = SYM_STRUCT_FIELD;
                sym->mem_type = mem_type;
                if( ISINVALID_IDX( dup_array( sym, NULL, tokbuf, initializer_loc + 1, no_of_bytes ) ) ) {
                    return( RC_ERROR );
                }
                return( RC_OK );
            } else if( sym->state == SYM_EXTERNAL && ((dir_node *)sym)->e.extinfo->global ) {
                dir_to_sym( (dir_node *)sym );
                if( !sym->public ) {
                    AddPublicData( (dir_node *)sym );
                }
                if( sym->mem_type != mem_type ) {
                    AsmErr( SYMBOL_TYPE_DIFF, sym->name );
                }
            } else if( sym->state != SYM_UNDEFINED ) {
                // redefine label
                AsmError( SYMBOL_ALREADY_DEFINED );
                return( RC_ERROR );
            }
        } else if( Definition.struct_depth != 0 ) {
            return( RC_OK );
        } else {
            old_offset = sym->offset;
        }

        GetSymInfo( sym );
        if( Parse_Pass != PASS_1 && sym->offset != old_offset ) {
            PhaseError = true;
        }
#else
        if( sym->state != SYM_UNDEFINED ) {
            // redefine label
            AsmError( SYMBOL_ALREADY_DEFINED );
            return( RC_ERROR );
        }
#endif
        sym->state = SYM_INTERNAL;
        sym->mem_type = mem_type;
        BackPatch( sym );
    }
#if defined( _STANDALONE_ )
    if( label_dir )
        return( RC_OK );
#endif
    if( ISINVALID_IDX( dup_array( sym, struct_sym, tokbuf, initializer_loc + 1, no_of_bytes ) ) ) {
        return( RC_ERROR );
    }
    return( RC_OK );
}

bool NextArrayElement( token_buffer *tokbuf, bool *next )
{
    token_idx   rc;

    *next = false;
    if( More_Array_Element ) {
        More_Array_Element = false;
        rc = dup_array( NULL, NULL, tokbuf, 0, Last_Element_Size );
        if( ISINVALID_IDX( rc ) )
            return( RC_ERROR );
        *next = true;
    }
    return( RC_OK );
}
