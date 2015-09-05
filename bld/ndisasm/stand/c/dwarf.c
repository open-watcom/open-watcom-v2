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
#include <string.h>
#include "watcom.h"
#include "global.h"
#include "dwarf.h"
#include "orl.h"
#include "mydwarf.h"
#include "memfuncs.h"
#include "hashtabl.h"

#define LINES_ARRAY_SIZE_INC    64

extern char *                   SourceFileInDwarf;
extern orl_linnum *             lines;
static long                     currlinesize;
extern hash_table               HandleToRefListTable;

extern orl_sec_handle           debugHnd;

static int ConvertLines( const uint_8 * input, uint length, uint limit );

static void fixupLines( uint_8 *relocContents, orl_sec_handle sec )
{
    hash_data *                 data_ptr;
    ref_list                    sec_ref_list;
    ref_entry                   r_entry;
    int                         i;

    data_ptr = HashTableQuery( HandleToRefListTable, (hash_value) sec );
    if( data_ptr ) {
        sec_ref_list = (ref_list) *data_ptr;
    } else {
        sec_ref_list = NULL;
    }
    r_entry = sec_ref_list->first;
    while( r_entry ) {
        switch( r_entry->type ) {
            // is this the only one?
            case ORL_RELOC_TYPE_WORD_32:
                if( r_entry->label->shnd == sec ) {
                    for( i=0; i<4; i++ ) {
                        relocContents[i+r_entry->offset] = ((char *)&(r_entry->label->offset))[i];
                    }
                } else {
                    for( i=0; i<3; i++ ) {
                        relocContents[i+r_entry->offset] = 0;
                    }
// Whatever this was supposed to do, it's killing the -s option for object files
#if 0
                    relocContents[3+r_entry->offset] = 0x80;
#else
                    relocContents[3+r_entry->offset] = 0;
#endif
                }
                break;
        }
        r_entry = r_entry->next;
    }
}

extern orl_table_index GetDwarfLines( section_ptr sec )
{
    uint                size;
    uint                limit;
    unsigned_8          *contents;
    unsigned_8          *relocContents;
    orl_table_index     numlines;

    if( lines ) {
        MemFree( lines );
        lines = NULL;
    }
    currlinesize = 0;
    if( debugHnd ) {
        ORLSecGetContents( debugHnd, &contents );
        size = ORLSecGetSize( debugHnd );
        limit = ORLSecGetSize( sec->shnd );
        relocContents = MemAlloc( size );
        memcpy( relocContents, contents, size );

        fixupLines( relocContents, debugHnd );

        numlines = ConvertLines( relocContents, size, limit );

        MemFree( relocContents );
        return numlines;
    } else {
        return 0;
    }
}

typedef struct {
    long        value;
    char *      name;
} readable_name;

static uint_8 *DecodeULEB128( const uint_8 *input, uint_32 *value )
/**********************************************************/
{
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

static uint_8 *DecodeLEB128( const uint_8 *input, int_32 *value )
/********************************************************/
{
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
typedef struct {
    uint_32                     address;
    uint                        file;
    uint_32                     line;
    uint_32                     column;
    uint_16                     segment;
    uint_8                      is_stmt : 1;
    uint_8                      basic_block : 1;
    uint_8                      end_sequence : 1;
} state_info;

static void init_state( state_info *state, int default_is_stmt )
/**************************************************************/
{
    state->address = 0;
    state->segment = 0;
    state->file = 1;
    state->line = 1;
    state->column = 0;
    state->is_stmt = default_is_stmt;
    state->basic_block = 0;
    state->end_sequence = 0;
}

static void dump_state( state_info *state, int *numlines, uint limit )
/*****************************************/
{
    if( state->address < limit ) {
        if( *numlines >= currlinesize ) {
            currlinesize += LINES_ARRAY_SIZE_INC;
            if( lines ) {
                lines = (orl_linnum *)MemRealloc( lines, currlinesize*sizeof(orl_linnum) );
            } else {
                lines = (orl_linnum *)MemAlloc( currlinesize*sizeof(orl_linnum) );
            }
        }
        lines[*numlines].linnum = state->line;
        lines[*numlines].off = state->address;
        (*numlines)++;
    }
}

static int ConvertLines( const uint_8 * input, uint length, uint limit )
/********************************************************/
{
    const uint_8 *              p;
    const uint_8 *              stmt_start;
    uint                        opcode_base;
    uint *                      opcode_lengths;
    uint                        u;
    uint                        file_index;
    const char *                name;
    uint_32                     mod_time;
    uint_32                     file_length;
    uint_32                     directory;
    uint_8                      op_code;
    uint_32                     op_len;
    uint_32                     tmp;
    uint                        line_range;
    int                         line_base;
    int_32                      itmp;
    int                         default_is_stmt;
    state_info                  state;
    uint                        min_instr;
    uint_32                     unit_length;
    const uint_8 *              unit_base;
    int                         numlines;

    numlines = 0;
    p = input;
    while( p - input < length ) {

        unit_length = *(uint_32 *)p;
        p += sizeof( uint_32 );
        unit_base = p;

        p += sizeof( uint_16 );

        stmt_start = p;
        stmt_start += *(uint_32 *)p;
        p += sizeof( uint_32 );
        stmt_start += sizeof( uint_32 );
        min_instr = *p;
        p += 1;

        default_is_stmt = *p;
        p += 1;

        line_base = *(int_8 *)p;
        p += 1;

        line_range = *(uint_8 *)p;
        p += 1;

        opcode_base = *p;
        p += 1;
        opcode_lengths = MemAlloc( sizeof( uint ) * opcode_base );
        for( u = 0; u < opcode_base - 1; ++u ) {
            opcode_lengths[ u ] = *p;
            ++p;
        }

        if( p - input >= length ) return 0;

        file_index = 0;
        while( *p != 0 ) {
            ++file_index;
            name = (char *)p;
            p += strlen( (char *)p ) + 1;
            if( p - input >= length ) return 0;
        }
        p++;
        file_index = 0;
        while( *p != 0 ) {
            ++file_index;
            name = (char *)p;
            p += strlen( (char *)p ) + 1;
            p = DecodeULEB128( p, &directory );
            p = DecodeULEB128( p, &mod_time );
            p = DecodeULEB128( p, &file_length );
            if( !SourceFileInDwarf ) {
                SourceFileInDwarf = MemAlloc( strlen( name ) + 1 );
                strcpy( SourceFileInDwarf, name );
            }
            if( p - input >= length ) return 0;
        }
        p++;
        init_state( &state, default_is_stmt );
        while( p - unit_base < unit_length ) {
            op_code = *p;
            ++p;
            if( op_code == 0 ) {
                /* extended op_code */
                p = DecodeULEB128( p, &op_len );
                op_code = *p;
                ++p;
                --op_len;
                switch( op_code ) {
                case DW_LNE_end_sequence:
                    state.end_sequence = 1;
                    dump_state( &state, &numlines, limit );
                    init_state( &state, default_is_stmt );
                    p+= op_len;
                    break;
                case DW_LNE_set_address:
                    if( op_len == 4 ){
                        tmp = *(uint_32 *)p;
                    }else if( op_len == 2 ){
                        tmp = *(uint_16 *)p;
                    }else{
                        tmp = 0xffffffff;
                    }
                    state.address = tmp;
                    p += op_len;
                    break;
                case DW_LNE_WATCOM_set_segment_OLD:
                case DW_LNE_WATCOM_set_segment:
                    if( op_len == 4 ){
                        tmp = *(uint_32 *)p;
                    }else if( op_len == 2 ){
                        tmp = *(uint_16 *)p;
                    }else{
                        tmp = 0xffffffff;
                    }
                    state.segment = tmp;
                    p += op_len;
                    break;
                case DW_LNE_define_file:
                    ++file_index;
                    name = (char *)p;
                    p += strlen( (char *)p ) + 1;
                    p = DecodeULEB128( p, &directory );
                    p = DecodeULEB128( p, &mod_time );
                    p = DecodeULEB128( p, &file_length );
                    if( SourceFileInDwarf ) {
                        MemFree( SourceFileInDwarf );
                    }
                    SourceFileInDwarf = MemAlloc( strlen( name) + 1 );
                    strcpy( SourceFileInDwarf, name );
                    break;
                default:
                    p += op_len;
                    break;
                }
            } else if( op_code < opcode_base ) {
                switch( op_code ) {
                case DW_LNS_copy:
                    dump_state( &state, &numlines, limit );
                    state.basic_block = 0;
                    break;
                case DW_LNS_advance_pc:
                    p = DecodeLEB128( p, &itmp );
                    state.address += itmp * min_instr;
                    break;
                case DW_LNS_advance_line:
                    p = DecodeLEB128( p, &itmp );
                    state.line += itmp;
                    break;
                case DW_LNS_set_file:
                    p = DecodeLEB128( p, &itmp );
                    state.file = itmp;
                    break;
                case DW_LNS_set_column:
                    p = DecodeLEB128( p, &itmp );
                    state.column = itmp;
                    break;
                case DW_LNS_negate_stmt:
                    state.is_stmt = !state.is_stmt;
                    break;
                case DW_LNS_set_basic_block:
                    state.basic_block = 1;
                    break;
                case DW_LNS_const_add_pc:
                    state.address += ( ( 255 - opcode_base ) / line_range ) * min_instr;
                    break;
                case DW_LNS_fixed_advance_pc:
                    tmp = *(uint_16 *)p;
                    p += sizeof( uint_16 );
                    state.address += tmp;
                    break;
                default:
                    for( u = 0; u < opcode_lengths[ op_code - 1 ]; ++u ) {
                        p = DecodeLEB128( p, &itmp );
                    }
                }
            } else {
                op_code -= opcode_base;
                state.line += line_base + op_code % line_range;
                state.address += ( op_code / line_range ) * min_instr;
                dump_state( &state, &numlines, limit );
                state.basic_block = 0;
            }
        }
        MemFree( opcode_lengths  );
    }
    return numlines;
}
