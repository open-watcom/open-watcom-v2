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


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <malloc.h>

#include "watcom.h"
#include "dw.h"
#include "dwarf.h"
#include "client.h"

typedef struct {
    uint_32     value;
    char *      name;
} readable_name;

#define table( x )      { x, #x }

static readable_name readableTAGs[] = {
    table( DW_TAG_padding ),
    table( DW_TAG_array_type ),
    table( DW_TAG_class_type ),
    table( DW_TAG_entry_point ),
    table( DW_TAG_enumeration_type ),
    table( DW_TAG_formal_parameter ),
    table( DW_TAG_imported_declaration ),
    table( DW_TAG_label ),
    table( DW_TAG_lexical_block ),
    table( DW_TAG_member ),
    table( DW_TAG_pointer_type ),
    table( DW_TAG_reference_type ),
    table( DW_TAG_compile_unit ),
    table( DW_TAG_string_type ),
    table( DW_TAG_structure_type ),
    table( DW_TAG_subroutine_type ),
    table( DW_TAG_typedef ),
    table( DW_TAG_union_type ),
    table( DW_TAG_unspecified_parameters ),
    table( DW_TAG_variant ),
    table( DW_TAG_common_block ),
    table( DW_TAG_common_inclusion ),
    table( DW_TAG_inheritance ),
    table( DW_TAG_inlined_subroutine ),
    table( DW_TAG_module ),
    table( DW_TAG_ptr_to_member_type ),
    table( DW_TAG_set_type ),
    table( DW_TAG_subrange_type ),
    table( DW_TAG_with_stmt ),
    table( DW_TAG_access_declaration ),
    table( DW_TAG_base_type ),
    table( DW_TAG_catch_block ),
    table( DW_TAG_const_type ),
    table( DW_TAG_constant ),
    table( DW_TAG_enumerator ),
    table( DW_TAG_file_type ),
    table( DW_TAG_friend ),
    table( DW_TAG_namelist ),
    table( DW_TAG_namelist_item ),
    table( DW_TAG_packed_type ),
    table( DW_TAG_subprogram ),
    table( DW_TAG_template_type_param ),
    table( DW_TAG_template_value_param ),
    table( DW_TAG_thrown_type ),
    table( DW_TAG_try_block ),
    table( DW_TAG_variant_part ),
    table( DW_TAG_variable ),
    table( DW_TAG_volatile_type )
};

#define NUM_TAGS        ( sizeof( readableTAGs ) / sizeof( readableTAGs[0] ) )

static readable_name readableFORMs[] = {
    table( DW_FORM_addr ),
    table( DW_FORM_block ),
    table( DW_FORM_block1 ),
    table( DW_FORM_block2 ),
    table( DW_FORM_block4 ),
    table( DW_FORM_data1 ),
    table( DW_FORM_data2 ),
    table( DW_FORM_data4 ),
    table( DW_FORM_data8 ),
    table( DW_FORM_flag ),
    table( DW_FORM_sdata ),
    table( DW_FORM_string ),
    table( DW_FORM_strp ),
    table( DW_FORM_udata ),
    table( DW_FORM_ref_addr ),
    table( DW_FORM_ref1 ),
    table( DW_FORM_ref2 ),
    table( DW_FORM_ref4 ),
    table( DW_FORM_ref8 ),
    table( DW_FORM_ref_udata ),
    table( DW_FORM_indirect )
};
#define NUM_FORMS       ( sizeof( readableFORMs ) / sizeof( readableFORMs[0] ) )

static readable_name readableATs[] = {
    table( DW_AT_sibling ),
    table( DW_AT_location ),
    table( DW_AT_name ),
    table( DW_AT_ordering ),
    table( DW_AT_byte_size ),
    table( DW_AT_bit_offset ),
    table( DW_AT_bit_size ),
    table( DW_AT_stmt_list ),
    table( DW_AT_low_pc ),
    table( DW_AT_high_pc ),
    table( DW_AT_language ),
    table( DW_AT_discr ),
    table( DW_AT_discr_value ),
    table( DW_AT_visibility ),
    table( DW_AT_import ),
    table( DW_AT_string_length ),
    table( DW_AT_common_reference ),
    table( DW_AT_comp_dir ),
    table( DW_AT_const_value ),
    table( DW_AT_containing_type ),
    table( DW_AT_default_value ),
    table( DW_AT_inline ),
    table( DW_AT_is_optional ),
    table( DW_AT_lower_bound ),
    table( DW_AT_producer ),
    table( DW_AT_prototyped ),
    table( DW_AT_return_addr ),
    table( DW_AT_start_scope ),
    table( DW_AT_stride_size ),
    table( DW_AT_upper_bound ),
    table( DW_AT_abstract_origin ),
    table( DW_AT_accessibility ),
    table( DW_AT_address_class ),
    table( DW_AT_artificial ),
    table( DW_AT_base_types ),
    table( DW_AT_calling_convention ),
    table( DW_AT_count ),
    table( DW_AT_data_member_location ),
    table( DW_AT_decl_column ),
    table( DW_AT_decl_file ),
    table( DW_AT_decl_line ),
    table( DW_AT_declaration ),
    table( DW_AT_discr_list ),
    table( DW_AT_encoding ),
    table( DW_AT_external ),
    table( DW_AT_frame_base ),
    table( DW_AT_friend ),
    table( DW_AT_identifier_case ),
    table( DW_AT_macro_info ),
    table( DW_AT_namelist_item ),
    table( DW_AT_priority ),
    table( DW_AT_segment ),
    table( DW_AT_specification ),
    table( DW_AT_static_link ),
    table( DW_AT_type ),
    table( DW_AT_use_location ),
    table( DW_AT_variable_parameter ),
    table( DW_AT_virtuality ),
    table( DW_AT_vtable_elem_location ),
    table( DW_AT_hi_user ),
};
#define NUM_ATS         ( sizeof( readableATs ) / sizeof( readableATs[0] ) )


static readable_name readableStandardOps[] = {
    table( DW_LNS_copy ),
    table( DW_LNS_advance_pc ),
    table( DW_LNS_advance_line ),
    table( DW_LNS_set_file ),
    table( DW_LNS_set_column ),
    table( DW_LNS_negate_stmt ),
    table( DW_LNS_set_basic_block ),
    table( DW_LNS_const_add_pc ),
    table( DW_LNS_fixed_advance_pc )
};
#define NUM_STANDARD_OPS \
    ( sizeof( readableStandardOps ) / sizeof( readableStandardOps[0] ) )

static readable_name readableReferenceOps[] = {
    table( REF_BEGIN_SCOPE ),
    table( REF_END_SCOPE ),
    table( REF_SET_FILE ),
    table( REF_SET_LINE ),
    table( REF_SET_COLUMN ),
    table( REF_ADD_LINE ),
    table( REF_ADD_COLUMN ),
    table( REF_COPY )
};
#define NUM_REFERENCE_OPS \
    ( sizeof( readableReferenceOps ) / sizeof( readableReferenceOps[0] ) )

static char *sectionNames[] = {
    ".debug_info",
    ".debug_pubnames",
    ".debug_aranges",
    ".debug_line",
    ".debug_loc",
    ".debug_abbrev",
    ".debug_macinfo",
    ".debug_ref"
};


static int compareTable( readable_name *a, readable_name *b ) {

    if( a->value > b->value ) {
        return( 1 );
    } else if( a->value == b->value ) {
        return( 0 );
    }
    return( -1 );
}


static void sortTables( void ) {

    qsort( readableTAGs, NUM_TAGS, sizeof( readable_name ), compareTable );
    qsort( readableFORMs, NUM_FORMS, sizeof( readable_name ), compareTable );
    qsort( readableATs, NUM_ATS, sizeof( readable_name ), compareTable );
}


static char *getName( uint_32 value, readable_name *table, size_t size ) {

    readable_name       dummy;
    readable_name *     result;

    dummy.value = value;
    result = bsearch( &dummy, table, size, sizeof( readable_name ),
        compareTable );
    if( result == NULL ) return( NULL );
    return( result->name );
}


static char *getTAG( uint_32 value ) {

    static char                 buf[ 30 ];
    char *                      result;

    result = getName( value, readableTAGs, NUM_TAGS );
    if( result == NULL ) {
        sprintf( buf, "TAG_%08lx", value );
        return( buf );
    }
    return( result );
}


static char *getFORM( uint_32 value ) {

    static char                 buf[ 30 ];
    char *                      result;

    result = getName( value, readableFORMs, NUM_FORMS );
    if( result == NULL ) {
        sprintf( buf, "FORM_%08lx", value );
        return( buf );
    }
    return( result );
}


static char *getAT( uint_32 value ) {

    static char                 buf[ 30 ];
    char *                      result;

    result = getName( value, readableATs, NUM_ATS );
    if( result == NULL ) {
        sprintf( buf, "AT_%08lx", value );
        return( buf );
    }
    return( result );
}


static void dumpHex( const char *input, uint length ) {

    char        *p;
    int         i;
    uint        offset;
    uint        old_offset;
    char        hex[ 80 ];
    char        printable[ 17 ];
    int         ch;

    offset = 0;
    for(;;) {
        i = 0;
        p = hex;
        old_offset = offset;
        for(;;) {
            if( offset == length ) break;
            if( i > 0xf ) break;
            if( i == 0x8 ) {
                *p++ = ' ';
            }
            ch = input[ offset ];
            p += sprintf( p, " %02x", ch );
            printable[ i ] = isprint( ch ) ? ch : '.';
            ++i;
            ++offset;
        }
        *p = 0;
        printable[ i ] = 0;
        printf( "%08lx:%-49s <%s>\n", old_offset, hex, printable );
        p = printable;
        i = 0;
        if( offset == length ) break;
    }
}

uint_8 *DecodeULEB128( const uint_8 *input, uint_32 *value ) {

    uint_32     result;
    uint        shift;
    uint_8      byte;

    result = 0;
    shift = 0;
    for(;;) {
        byte = *input++;
        result |= ( byte & 0x7f ) << shift;
        if( ( byte & 0x80 ) == 0 ) break;
        shift += 7;
    }
    *value = result;
    return( (uint_8 *)input );
}


uint_8 *DecodeLEB128( const uint_8 *input, int_32 *value ) {

    int_32      result;
    uint        shift;
    uint_8      byte;

    result = 0;
    shift = 0;
    for(;;) {
        byte = *input++;
        result |= ( byte & 0x7f ) << shift;
        shift += 7;
        if( ( byte & 0x80 ) == 0 ) break;
    }
    if( ( shift < 32 ) && ( byte & 0x40 ) ) {
        result |= - ( 1 << shift );
    }
    *value = result;
    return( (uint_8 *)input );
}

uint_8 *findAbbrev( uint_32 code ) {

    uint_8      *p;
    uint_8      *stop;
    uint_32     tmp;
    uint_32     attr;

    p = Sections[ DW_DEBUG_ABBREV ].data;
    stop = p + Sections[ DW_DEBUG_ABBREV ].max_offset;
    for(;;) {
        if( p >= stop ) return( NULL );
        p = DecodeULEB128( p, &tmp );
        if( tmp == code ) return( p );
        if( p >= stop ) return( NULL );
        p = DecodeULEB128( p, &tmp );
        if( p >= stop ) return( NULL );
        p++;
        for(;;) {
            p = DecodeULEB128( p, &attr );
            if( p >= stop ) return( NULL );
            p = DecodeULEB128( p, &tmp );
            if( p >= stop ) return( NULL );
            if( attr == 0 ) break;
        }
    }
}


static void dumpInfo( const char *input, uint length ) {

    const uint_8 *p;
    uint_32     abbrev_code;
    uint_8 *    abbrev;
    uint_32     tag;
    uint_32     attr;
    uint_32     form;
    uint_32     len;
    uint_32     tmp;
    int_32      stmp;
    uint_32     unit_length;
    const uint_8 *unit_base;

    p = input;
    while( p - input < length ) {
        unit_length = *(uint_32 *)p;
        unit_base = p + sizeof( uint_32 );
        printf( "Length: %08lx\nVersion: %04lx\nAbbrev: %08lx\nAddress Size %02lx\n",
            unit_length, *(uint_16 *)(p+4), *(uint_32 *)(p+6), *(p+10) );
        p += 11;
        while( p - unit_base < unit_length ) {
            printf( "offset %08lx: ", p - input );
            p = DecodeULEB128( p, &abbrev_code );
            printf( "Code: %08lx\n", abbrev_code );
            if( abbrev_code == 0 ) continue;
            abbrev = findAbbrev( abbrev_code );
            if( abbrev == NULL ) {
                printf( "can't find abbreviation %08lx\n", abbrev_code );
                break;
            }
            if( p >= input + length ) break;
            abbrev = DecodeULEB128( abbrev, &tag );
            printf( "\t%s\n", getTAG( tag ) );
            abbrev++;
            for(;;) {
                abbrev = DecodeULEB128( abbrev, &attr );
                abbrev = DecodeULEB128( abbrev, &form );
                if( attr == 0 ) break;
                printf( "\t%-20s", getAT( attr ) );
    decode_form:
                switch( form ) {
                case DW_FORM_addr:
#ifdef __ADDR_IS_32
                    tmp = *(uint_32 *)p;
                    p += sizeof( uint_32 );
                    tmp_seg = *(uint_16 *)p;
                    p += sizeof( uint_16 );
                    printf( "\t%04lx:%08lx\n", tmp_seg, tmp );
#else
                    tmp = *(uint_16 *)p;
                    p += sizeof( uint_16 );
                    printf( "\t%04xl\n", tmp );
#endif
                    break;
                case DW_FORM_block:
                    p = DecodeULEB128( p, &len );
                    printf( "\n" );
                    dumpHex( p, len );
                    p += len;
                    break;
                case DW_FORM_block1:
                    len = *p++;
                    printf( "\n" );
                    dumpHex( p, len );
                    p += len;
                    break;
                case DW_FORM_block2:
                    len = *(uint_16 *)p;
                    p += sizeof( uint_16 );
                    printf( "\n" );
                    dumpHex( p, len );
                    p += len;
                    break;
                case DW_FORM_block4:
                    len = *(uint_32 *)p;
                    p += sizeof( uint_32 );
                    printf( "\n" );
                    dumpHex( p, len );
                    p += len;
                    break;
                case DW_FORM_data1:
                case DW_FORM_ref1:
                    printf( "\t%02x\n", *p++ );
                    break;
                case DW_FORM_data2:
                case DW_FORM_ref2:
                    printf( "\t%04x\n", *(uint_16 *)p );
                    p += sizeof( uint_16 );
                    break;
                case DW_FORM_data4:
                case DW_FORM_ref4:
                    printf( "\t%08lx\n", *(uint_32 *)p );
                    p += sizeof( uint_32 );
                    break;
                case DW_FORM_flag:
                    printf( "\t%s\n", *p++ ? "True" : "False" );
                    break;
                case DW_FORM_indirect:
                    p = DecodeULEB128( p, &form );
                    printf( "\t(%s)", getFORM( form ) );
                    goto decode_form;
                case DW_FORM_sdata:
                    p = DecodeLEB128( p, &stmp );
                    printf( "\t%08lx\n", stmp );
                    break;
                case DW_FORM_string:
                    printf( "\t\"%s\"\n", p );
                    p += strlen( p ) + 1;
                    break;
                case DW_FORM_strp:
                    abort();
                    break;
                case DW_FORM_udata:
                case DW_FORM_ref_udata:
                    p = DecodeULEB128( p, &tmp );
                    printf( "\t%08lx\n", tmp );
                    break;
                case DW_FORM_ref_addr:  //KLUDGE should really check addr_size
                    printf( "\t%08lx\n", *((uint_32 *)p) );
                    p += sizeof(uint_32);
                    break;
                default:
                    printf( "unknown form\n" );
                    return;
                }
            }
        }
    }
}


extern void dumpAbbrevs( const char *input, uint length ) {

    const uint_8 *p;
    uint_32     tmp;
    uint_32     attr;

    p = input;
    for(;;) {
        if( p > input + length ) break;
        p = DecodeULEB128( p, &tmp );
        printf( "Code: %08lx\n", tmp );
        if( p >= input + length ) break;
        p = DecodeULEB128( p, &tmp );
        printf( "\t%s\n", getTAG( tmp ) );
        if( *p == DW_children_yes ) {
            printf( "has children\n" );
        } else {
            printf( "childless\n" );
        }
        p++;
        for(;;) {
            if( p > input + length ) break;
            p = DecodeULEB128( p, &attr );
            printf( "\t%-20s", getAT( attr ) );
            if( p > input + length ) break;
            p = DecodeULEB128( p, &tmp );
            printf( "\t%-15s\n", getFORM( tmp ) );
            if( attr == 0 ) break;
        }
    }
}


static char *getStandardOp( uint_8 value ) {

    static char                 buf[ 30 ];
    char *                      result;

    result = getName( value, readableStandardOps, NUM_STANDARD_OPS );
    if( result == NULL ) {
        sprintf( buf, "OP_%02x", value );
        return( buf );
    }
    return( result );
}

typedef struct {
    uint_32                     address;
    uint                        file;
    uint_32                     line;
    uint_32                     column;
    uint_8                      is_stmt : 1;
    uint_8                      basic_block : 1;
    uint_8                      end_sequence : 1;
} state_info;


static void initState( state_info *state, int default_is_stmt ) {

    state->address = 0;
    state->file = 1;
    state->line = 1;
    state->column = 0;
    state->is_stmt = default_is_stmt;
    state->basic_block = 0;
    state->end_sequence = 0;
}


static void dumpState( state_info *state ) {

    printf( "-- file %d addr %08lx line %d column %d",
        state->file, state->address, state->line, state->column );
    if( state->is_stmt ) printf( " is_stmt" );
    if( state->basic_block ) printf( " basic_block" );
    if( state->end_sequence ) printf( " end_sequence" );
    printf( "\n" );
}


static void dumpLines(
    const char *                input,
    uint                        length )
{
    const uint_8 *              p;
    uint                        opcode_base;
    uint *                      opcode_lengths;
    uint                        u;
    uint                        file_index;
    const uint_8 *              name;
    uint_32                     mod_time;
    uint_32                     file_length;
    uint_32                     directory;
    uint_8                      op_code;
    uint_8                      op_len;
    uint_32                     tmp;
    uint_16                     tmp_seg;
    uint                        line_range;
    int                         line_base;
    int_32                      itmp;
    int                         default_is_stmt;
    state_info                  state;
    uint                        min_instr;
    uint_32                     unit_length;
    const uint_8 *              unit_base;

    p = input;
    while( p - input < length ) {
        unit_length = *(uint_32 *)p;
        p += sizeof( uint_32 );
        unit_base = p;

        printf( "total_length: %08lx\n", unit_length );

        printf( "version: %04x\n", *(uint_16 *)p );
        p += sizeof( uint_16 );

        printf( "prologue_length: %08lx\n", *(uint_32 *)p );
        p += sizeof( uint_32 );

        min_instr = *p;
        printf( "minimum_instruction_length: %02x\n", min_instr );
        p += 1;

        default_is_stmt = *p;
        printf( "default_is_stmt: %02x\n", default_is_stmt );
        p += 1;

        line_base = *(int_8 *)p;
        printf( "line_base: %02x\n", line_base );
        p += 1;

        line_range = *(uint_8 *)p;
        printf( "line_range: %02x\n", line_range );
        p += 1;

        opcode_base = *p;
        printf( "opcode_base: %02x\n", opcode_base );
        p += 1;
        opcode_lengths = alloca( sizeof( uint ) * opcode_base );
        printf( "standard_opcode_lengths:\n" );
        for( u = 0; u < opcode_base - 1; ++u ) {
            opcode_lengths[ u ] = *p;
            ++p;
            printf( "%4u: %u\n", u, opcode_lengths[ u ] );
        }

        printf( "-- current_offset = %08lx\n", p - input );

        if( p - input >= length ) return;

        file_index = 0;
        while( *p != 0 ) {
            ++file_index;
            name = p;
            p += strlen( p ) + 1;
            printf( "path %u: '%s'\n", file_index, name );
            if( p - input >= length ) return;
        }
        p++;
        file_index = 0;
        while( *p != 0 ) {
            ++file_index;
            name = p;
            p += strlen( p ) + 1;
            p = DecodeULEB128( p, &mod_time );
            p = DecodeULEB128( p, &file_length );
            printf( "file %u: '%s' mod_time %08lx length %08lx\n",
                file_index, name, mod_time, file_length );
            if( p - input >= length ) return;
        }
        p++;
        initState( &state, default_is_stmt );

        while( p - unit_base < unit_length ) {
            op_code = *p;
            ++p;
            if( op_code == 0 ) {
                /* extended op_code */
                op_len = *p;
                ++p;
                printf( "len: %03d ", op_len );
                op_code = *p;
                ++p;
                switch( op_code ) {
                case DW_LNE_end_sequence:
                    printf( "END_SEQUENCE\n" );
                    state.end_sequence = 1;
                    dumpState( &state );
                    initState( &state, default_is_stmt );
                    break;
                case DW_LNE_set_address:
                    tmp = *(uint_32 *)p;
                    p += sizeof( uint_32 );
                    tmp_seg = *(uint_16 *)p;
                    p += sizeof( uint_16 );
                    printf( "SET_ADDRESS %04x:%08lx\n", tmp_seg, tmp );
                    break;
                case DW_LNE_define_file:
                    ++file_index;
                    name = p;
                    p += strlen( p ) + 1;
                    p = DecodeULEB128( p, &directory );
                    p = DecodeULEB128( p, &mod_time );
                    p = DecodeULEB128( p, &file_length );
                    printf( "DEFINE_FILE %u: '%s' directory %ld mod_time %08lx length %08lx\n",
                        file_index, name, directory, mod_time, file_length );
                    break;
                default:
                    printf( "** unknown extended opcode: %02x\n", op_code );
                    return;
                }
            } else if( op_code < opcode_base ) {
                printf( "%s", getStandardOp( op_code ) );
                switch( op_code ) {
                case DW_LNS_copy:
                    printf( "\n" );
                    dumpState( &state );
                    state.basic_block = 0;
                    break;
                case DW_LNS_advance_pc:
                    p = DecodeLEB128( p, &itmp );
                    printf( " %ld\n", itmp );
                    state.address += itmp * min_instr;
                    break;
                case DW_LNS_advance_line:
                    p = DecodeLEB128( p, &itmp );
                    printf( " %ld\n", itmp );
                    state.line += itmp;
                    break;
                case DW_LNS_set_file:
                    p = DecodeLEB128( p, &itmp );
                    printf( " %ld\n", itmp );
                    state.file = itmp;
                    break;
                case DW_LNS_set_column:
                    p = DecodeLEB128( p, &itmp );
                    printf( " %ld\n", itmp );
                    state.column = itmp;
                    break;
                case DW_LNS_negate_stmt:
                    printf( "\n" );
                    state.is_stmt = !state.is_stmt;
                    break;
                case DW_LNS_set_basic_block:
                    printf( "\n" );
                    state.basic_block = 1;
                    break;
                case DW_LNS_const_add_pc:
                    printf( "\n" );
                    state.address += ( ( 255 - opcode_base ) / line_range ) * min_instr;
                    break;
                case DW_LNS_fixed_advance_pc:
                    tmp = *(uint_16 *)p;
                    p += sizeof( uint_16 );
                    printf( " %04x\n", tmp );
                    state.address += tmp;
                    break;
                default:
                    for( u = 0; u < opcode_lengths[ op_code - 1 ]; ++u ) {
                        p = DecodeLEB128( p, &itmp );
                        printf( " %08lx", itmp );
                    }
                    printf( "\n" );
                }
            } else {
                printf( "SPECIAL %02x:", op_code );
                op_code -= opcode_base;
                printf( " addr incr: %d  line incr: %d\n",
                    op_code / line_range,
                    line_base + op_code % line_range );
                state.line += line_base + op_code % line_range;
                state.address += ( op_code / line_range ) * min_instr;
                dumpState( &state );
                state.basic_block = 0;
            }
        }
        printf( "-- current_offset = %08lx\n", p - input );
    }
}


static char *getReferenceOp( uint_8 value ) {

    static char                 buf[ 30 ];
    char *                      result;

    result = getName( value, readableReferenceOps, NUM_REFERENCE_OPS );
    if( result == NULL ) {
        sprintf( buf, "REF_%02x", value );
        return( buf );
    }
    return( result );
}


static void dumpRef(
    const char *                input,
    uint                        length )
{
    const uint_8 *              p;
    uint_8                      op_code;
    uint_32                     tmp;
    int_32                      itmp;
    uint_32                     unit_length;
    const uint_8 *              unit_base;

    p = input;

    while( p - input < length ) {
        unit_length = *(uint_32 *)p;
        p += sizeof( uint_32 );
        unit_base = p;
        printf( "total_length: %08lx\n", *(uint_32 *)p );

        while( p - unit_base < unit_length ) {
            op_code = *p++;
            if( op_code < REF_CODE_BASE ) {
                printf( "%s", getReferenceOp( op_code ) );
                switch( op_code ) {
                case REF_BEGIN_SCOPE:
                    printf( " %08lx\n", *(uint_32 *)p );
                    p += sizeof( uint_32 );
                    break;
                case REF_END_SCOPE:
                case REF_COPY:
                    printf( "\n" );
                    break;
                case REF_SET_FILE:
                case REF_SET_LINE:
                case REF_SET_COLUMN:
                    p = DecodeULEB128( p, &tmp );
                    printf( " %lu\n", tmp );
                    break;
                case REF_ADD_LINE:
                case REF_ADD_COLUMN:
                    p = DecodeLEB128( p, &itmp );
                    printf( " %ld\n", itmp );
                    break;
                }
            } else {
                op_code -= REF_CODE_BASE;
                printf( "REF line += %d, column += %d, %08lx\n",
                    op_code / REF_COLUMN_RANGE, op_code % REF_COLUMN_RANGE,
                    *(uint_32 *)p
                );
                p += sizeof( uint_32 );
            }
        }
    }
}


void DumpSections( void ) {

    uint        sect;

    sortTables();
    sect = 0;
    for(;;) {
        printf( "%s:\n", sectionNames[ sect ] );
        switch( sect ) {
        case DW_DEBUG_ABBREV:
            dumpAbbrevs( Sections[ sect ].data, Sections[ sect ].max_offset );
            break;
        case DW_DEBUG_INFO:
            dumpInfo( Sections[ sect ].data, Sections[ sect ].max_offset );
            break;
        case DW_DEBUG_LINE:
            dumpLines( Sections[ sect ].data, Sections[ sect ].max_offset );
            break;
        case DW_DEBUG_REF:
            dumpRef( Sections[ sect ].data, Sections[ sect ].max_offset );
            break;
        default:
            dumpHex( Sections[ sect ].data, Sections[ sect ].max_offset );
            break;
        }
        ++sect;
        if( sect == DW_DEBUG_MAX ) break;
        printf( "\n" );
    }
}
