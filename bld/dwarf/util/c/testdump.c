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
* Description:  Dump the contents of DWARF debug sections.
*
****************************************************************************/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "walloca.h"

#include "watcom.h"
#include "bool.h"
#include "dw.h"
#include "dwarf.h"
#include "client.h"

extern bool byte_swap;

typedef struct {
    uint_32     value;
    char *      name;
} readable_name;

#include "pushpck1.h"
typedef struct arange_header {
    uint_32     len;
    uint_16     version;
    uint_32     dbg_pos;
    uint_8      addr_size;
    uint_8      seg_size;
} _WCUNALIGNED arange_header;
#include "poppck.h"

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
    table( DW_TAG_volatile_type ),
    table( DW_TAG_WATCOM_address_class_type ),
    table( DW_TAG_WATCOM_namespace )
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
    table( DW_AT_WATCOM_memory_model ),
    table( DW_AT_WATCOM_parm_entry ),
    table( DW_AT_WATCOM_references_start )
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
    ".debug_str",
    ".debug_ref"
};


static uint_32  getU32( uint_32 *src )
{
    if( byte_swap ) {
#ifdef __BIG_ENDIAN__
        return( GET_LE_32( *src ) );
#else
        return( GET_BE_32( *src ) );
#endif
    } else {
        return( *src );
    }
}

static uint_16  getU16( uint_16 *src )
{
    if( byte_swap ) {
#ifdef __BIG_ENDIAN__
        return( GET_LE_16( *src ) );
#else
        return( GET_BE_16( *src ) );
#endif
    } else {
        return( *src );
    }
}

static int compareTable( const void *_a, const void *_b ) {
    readable_name *a = (readable_name *)_a;
    readable_name *b = (readable_name *)_b;

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


static void dumpHex( const unsigned_8 *input, uint length, int offsets )
{
    char        *p;
    int         i;
    uint        offset;
    uint        old_offset;
    char        hex[ 80 ];
    char        printable[ 17 ];
    int         ch;

    if( offsets ) {
        printf( "          00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f\n" );
        printf( "          ------------------------------------------------\n" );
    }

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
        printf( "%08x:%-49s <%s>\n", old_offset, hex, printable );
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

uint_8 *findAbbrev( uint_32 code, uint_32 start ) {

    uint_8      *p;
    uint_8      *stop;
    uint_32     tmp;
    uint_32     attr;

    p = Sections[ DW_DEBUG_ABBREV ].data + start;
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

static void printf_debug_str( unsigned int offset )
{
    if( offset > Sections[ DW_DEBUG_STR ].max_offset ) {
        printf( "\tstring @ .debug_str+%u (invalid offset)\n",
               offset );
    } else {
        printf( "\t\"%s\"\n", Sections[ DW_DEBUG_STR ].data + offset );
    }
}

static void dumpInfo( const uint_8 *input, uint length ) {

    const uint_8 *p;
    uint_32     abbrev_code;
    uint_32     abbrev_offset;
    uint_8 *    abbrev;
    uint_32     tag;
    uint_32     attr;
    uint_32     form;
    uint_32     len;
    uint_32     tmp;
    int_32      stmp;
    uint_32     unit_length;
    int         address_size;
    const uint_8 *unit_base;

    p = input;
    while( p - input < length ) {
        unit_length = getU32( (uint_32 *)p );
        unit_base = p + sizeof( uint_32 );
        address_size = *(p+10);
        abbrev_offset = getU32( (uint_32 *)(p+6) );
        printf( "Length: %08lx\nVersion: %04x\nAbbrev: %08lx\nAddress Size %02x\n",
            unit_length, getU16( (uint_16 *)(p+4) ), abbrev_offset, address_size );
        p += 11;
        while( p - unit_base < unit_length ) {
            printf( "offset %08x: ", p - input );
            p = DecodeULEB128( p, &abbrev_code );
            printf( "Code: %08lx\n", abbrev_code );
            if( abbrev_code == 0 ) continue;
            abbrev = findAbbrev( abbrev_code, abbrev_offset );
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
                    switch( address_size ) {
                    case 4:
                        tmp = getU32( (uint_32 *)p );
                        p += sizeof( uint_32 );
                        printf( "\t%08lx\n", tmp );
                        break;
                    case 2:
                        tmp = getU16( (uint_16 *)p );
                        p += sizeof( uint_16 );
                        printf( "\t%04lx\n", tmp );
                        break;
                    default:
                        printf( "Unknown address size\n" );
                        p += address_size;
                        break;
                    }
                    break;
                case DW_FORM_block:
                    p = DecodeULEB128( p, &len );
                    printf( "\n" );
                    dumpHex( p, len, 0 );
                    p += len;
                    break;
                case DW_FORM_block1:
                    len = *p++;
                    printf( "\n" );
                    dumpHex( p, len, 0 );
                    p += len;
                    break;
                case DW_FORM_block2:
                    len = getU16( (uint_16 *)p );
                    p += sizeof( uint_16 );
                    printf( "\n" );
                    dumpHex( p, len, 0 );
                    p += len;
                    break;
                case DW_FORM_block4:
                    len = getU32( (uint_32 *)p );
                    p += sizeof( uint_32 );
                    printf( "\n" );
                    dumpHex( p, len, 0 );
                    p += len;
                    break;
                case DW_FORM_data1:
                case DW_FORM_ref1:
                    printf( "\t%02x\n", *p++ );
                    break;
                case DW_FORM_data2:
                case DW_FORM_ref2:
                    printf( "\t%04x\n", getU16( (uint_16 *)p ) );
                    p += sizeof( uint_16 );
                    break;
                case DW_FORM_data4:
                case DW_FORM_ref4:
                    printf( "\t%08lx\n", getU32( (uint_32 *)p ) );
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
                    p += strlen( (const char *)p ) + 1;
                    break;
                case DW_FORM_strp:  /* 4 byte index into .debug_str */
                    printf_debug_str( getU32( (unsigned long *)p ) );
                    p += 4;
                    break;
                case DW_FORM_udata:
                case DW_FORM_ref_udata:
                    p = DecodeULEB128( p, &tmp );
                    printf( "\t%08lx\n", tmp );
                    break;
                case DW_FORM_ref_addr:  //KLUDGE should really check addr_size
                    printf( "\t%08lx\n", getU32( ((uint_32 *)p) ) );
                    p += sizeof(uint_32);
                    break;
                default:
                    printf( "unknown form!\n" );
                    return;
                }
            }
        }
    }
}


extern void dumpAbbrevs( const unsigned_8 *input, uint length ) {

    const uint_8 *p;
    uint_32     tmp;
    uint_32     attr;

    if( (NULL == input) || (0 == length) )
        return;

    p = input;
    for(;;) {
        if( p > input + length ) break;
        p = DecodeULEB128( p, &tmp );
        printf( "Code: %08lx\n", tmp );
    if( tmp == 0 ) continue;
        if( p >= input + length ) break;
        p = DecodeULEB128( p, &tmp );
        printf( "\t%s\n", getTAG( tmp ) );
        if( *p == DW_CHILDREN_yes ) {
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
    const uint_8 *              input,
    uint                        length )
{
    const uint_8 *              p;
    uint                        opcode_base;
    uint *                      opcode_lengths;
    uint                        u;
    uint                        file_index;
    const uint_8 *              name;
    uint_32                     dir_index;
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
        unit_length = getU32( (uint_32 *)p );
        p += sizeof( uint_32 );
        unit_base = p;

        printf( "total_length: 0x%08lx (%u)\n", unit_length, unit_length );
        
        printf( "=== unit dump start ===\n" );
        dumpHex( unit_base - sizeof( uint_32 ), unit_length + sizeof (uint_32 ), 1 );
        printf( "=== unit dump end ===\n" );

        printf( "version: 0x%04x\n", getU16( (uint_16 *)p ) );
        p += sizeof( uint_16 );

        printf( "prologue_length: 0x%08lx (%u)\n", getU32( (uint_32 *)p ), getU32( (uint_32 *)p ) );
        p += sizeof( uint_32 );

        min_instr = *p;
        printf( "minimum_instruction_length: 0x%02x (%u)\n", min_instr, min_instr );
        p += 1;

        default_is_stmt = *p;
        printf( "default_is_stmt: 0x%02x (%u)\n", default_is_stmt, default_is_stmt );
        p += 1;

        line_base = *(int_8 *)p;
        printf( "line_base: 0x%02x (%d)\n", (unsigned char)line_base, line_base );
        p += 1;

        line_range = *(uint_8 *)p;
        printf( "line_range: 0x%02x (%u)\n", line_range, line_range );
        p += 1;

        opcode_base = *p;
        printf( "opcode_base: 0x%02x (%u)\n", opcode_base, opcode_base );
        p += 1;
        opcode_lengths = alloca( sizeof( uint ) * opcode_base );
        printf( "standard_opcode_lengths:\n" );
        for( u = 0; u < opcode_base - 1; ++u ) {
            opcode_lengths[ u ] = *p;
            ++p;
            printf( "%4u: %u\n", u + 1, opcode_lengths[ u ] );
        }

        printf( "-- current_offset = %08x\n", p - input );

        if( p - input >= length ) return;

        printf( "-- start include paths --\n");
        file_index = 0;
        while( *p != 0 ) {
            ++file_index;
            name = p;
            p += strlen( (const char *)p ) + 1;
            printf( "path %u: '%s'\n", file_index, name );
            if( p - input >= length ) return;
        }
        printf( "-- end include paths --\n");
        p++;
        printf( "-- start files --\n");
        file_index = 0;
        while( *p != 0 ) {
            ++file_index;
            name = p;
            p += strlen( (const char *)p ) + 1;
            p = DecodeULEB128( p, &dir_index );
            p = DecodeULEB128( p, &mod_time );
            p = DecodeULEB128( p, &file_length );
            printf( "file %u: '%s' dir_index %08lx mod_time %08lx length %08lx\n",
                file_index, name, dir_index, mod_time, file_length );
            if( p - input >= length ) return;
        }
        printf( "-- end files --\n");
        p++;
        initState( &state, default_is_stmt );

        while( p - unit_base < unit_length ) {
            op_code = *p;
            ++p;
            if( op_code == 0 ) {
                printf( "EXTENDED 0x%02x: ", op_code );
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
                    if( op_len == 3 ) {
                        tmp = getU16( (uint_16 *)p );
                        p += sizeof( uint_16 );
                    } else {
                        tmp = getU32( (uint_32 *)p );
                        p += sizeof( uint_32 );
                    }
#if 0   /* Why did they choose 6 byte here?  */
                    tmp_seg = getU16( (uint_16 *)p );
                    p += sizeof( uint_16 );
                    printf( "SET_ADDRESS %04x:%08lx\n", tmp_seg, tmp );
#else
                    tmp_seg = 0;    /* stop warning */
                    printf( "SET_ADDRESS %08lx\n", tmp );
#endif
                    break;
                case DW_LNE_WATCOM_set_segment_OLD:
                case DW_LNE_WATCOM_set_segment:
                    tmp_seg = getU16( (uint_16 *)p );
                    p += sizeof( uint_16 );
                    printf( "SET_ADDRESS_SEG %04x\n", tmp_seg );
                    break;
                case DW_LNE_define_file:
                    ++file_index;
                    name = p;
                    p += strlen( (const char *)p ) + 1;
                    p = DecodeULEB128( p, &directory );
                    p = DecodeULEB128( p, &mod_time );
                    p = DecodeULEB128( p, &file_length );
                    printf( "DEFINE_FILE %u: '%s' directory %ld mod_time %08lx length %08lx\n",
                        file_index, name, directory, mod_time, file_length );
                    break;
                default:
                    printf( "** unknown extended opcode: %02x - %u bytes\n", op_code, op_len );
                    printf( "** losing %u bytes\n", unit_length - ( p - unit_base ));
                    
                    dumpHex( p-3, (unit_length - ( p - unit_base )) + 3, 1 );
                    
                    p = unit_base + unit_length;
                    goto hacky;                    
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
                    tmp = getU16( (uint_16 *)p );
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
                printf( "SPECIAL 0x%02x:", op_code );
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
hacky:
        printf( "-- current_offset = %08x\n", p - input );
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
    const uint_8 *              input,
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
        unit_length = getU32( (uint_32 *)p );
        p += sizeof( uint_32 );
        unit_base = p;
        printf( "total_length: %08lx\n", getU32( (uint_32 *)p ) );

        while( p - unit_base < unit_length ) {
            op_code = *p++;
            if( op_code < REF_CODE_BASE ) {
                printf( "%s", getReferenceOp( op_code ) );
                switch( op_code ) {
                case REF_BEGIN_SCOPE:
                    printf( " %08lx\n", getU32( (uint_32 *)p ) );
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
                    getU32( (uint_32 *)p )
                );
                p += sizeof( uint_32 );
            }
        }
    }
}


static const uint_8 *dumpSegAddr( const uint_8 *input, uint_8 addrsize, uint_8 segsize )
{
    const uint_8 *          p;
    uint_32                 addr;
    uint_32                 seg;

    p = input;

    switch( addrsize ) {
    case 4:
        addr = getU32( (uint_32 *)p );
        p += sizeof( uint_32 );
        break;
    case 2:
        addr = getU16( (uint_16 *)p );
        p += sizeof( uint_16 );
        break;
    default:
        printf( "Unknown address size\n" );
        addr = 0;
        p += addrsize;
        break;
    }
    switch( segsize ) {
    case 4:
        seg = getU32( (uint_32 *)p );
        p += sizeof( uint_32 );
        printf( "%08lx:", seg );
        break;
    case 2:
        seg = getU16( (uint_16 *)p );
        p += sizeof( uint_16 );
        printf( "%04lx:", seg );
        break;
    case 0:
        break;
    default:
        printf( "Unknown address size\n" );
        p += addrsize;
        break;
    }
    switch( addrsize ) {
    case 4:
        printf( "%08lx", addr );
        break;
    case 2:
        printf( "%04lx", addr );
        break;
    }
    return( p );
}


static void dumpARanges( const uint_8 *input, uint length )
{
    const uint_8 *              p;
    const uint_8 *              q;
    const uint_8 *              cu_ar_end;
    uint_32                     cu;
    uint_32                     tmp;
    uint_32                     tuple_size;
    uint_32                     padding;
    arange_header               ar_header;

    p = input;

    if( (input == NULL) || (length == 0) )
        return;

    if( length < sizeof( ar_header ) ) {
        printf( ".debug_aranges section too small!\n" );
        return;
    }

    cu = 0;

    while( p - input < length ) {
        ++cu;
        printf( "Compilation Unit %d\n", cu );

        ar_header.len = getU32( (uint_32 *)p );
        p += sizeof( uint_32 );
        ar_header.version = getU16( (uint_16 *)p );
        p += sizeof( uint_16 );
        ar_header.dbg_pos = getU32( (uint_32 *)p );
        p += sizeof( uint_32 );
        ar_header.addr_size = *p;
        p += sizeof( uint_8 );
        ar_header.seg_size = *p;
        p += sizeof( uint_8 );

        printf( "  length:    %08lx\n", ar_header.len );
        printf( "  version:   %04x\n", ar_header.version );
        printf( "  dbg_pos:   %08lx\n", ar_header.dbg_pos );
        printf( "  addr_size: %02x\n", ar_header.addr_size );
        printf( "  seg_size:  %02x\n", ar_header.seg_size );

        /* tuples should be aligned on twice the size of a tuple; some Watcom
         * versions didn't do this right!
         */
        tuple_size = (ar_header.addr_size + ar_header.seg_size) * 2;
        q = (const uint_8 *)(((uint_32)p + tuple_size - 1) & ~(tuple_size - 1));
        padding = q - p;

        /* check if padding contains non-zero data */
        if( padding ) {
            switch( ar_header.addr_size ) {
            case 4:
                tmp = getU32( (uint_32 *)p );
                break;
            case 2:
                tmp = getU16( (uint_16 *)p );
                break;
            default:
                printf( "Unknown address size\n" );
                return;
            }
            /* padding is missing! */
            if( tmp )
                padding = 0;
        }

        p += padding;
        cu_ar_end = p - sizeof( ar_header ) + sizeof( ar_header.len )
            + ar_header.len - padding;

        while( p < cu_ar_end ) {
            /* range address */
            printf( "\t" );
            p = dumpSegAddr( p, ar_header.addr_size, ar_header.seg_size );
            /* range length */
            printf( "\t" );
            switch( ar_header.addr_size ) {
            case 4:
                tmp = getU32( (uint_32 *)p );
                p += sizeof( uint_32 );
                printf( "%08lx\n", tmp );
                break;
            case 2:
                tmp = getU16( (uint_16 *)p );
                p += sizeof( uint_16 );
                printf( "%04lx\n", tmp );
                break;
            default:
                printf( "Unknown address size\n" );
                p += ar_header.addr_size;
                return;
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
        case DW_DEBUG_ARANGES:
            dumpARanges( Sections[ sect ].data, Sections[ sect ].max_offset );
            break;
        case DW_DEBUG_STR:
            // Strings are displayed when dumping other sections
            break;
        default:
            dumpHex( Sections[ sect ].data, Sections[ sect ].max_offset, 0 );
            break;
        }
        ++sect;
        if( sect == DW_DEBUG_MAX ) break;
        printf( "\n" );
    }
}
