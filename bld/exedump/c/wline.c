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
* Description:  DWARF line information processing.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>

#include "wdglb.h"
#include "wdfunc.h"

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


static void dump_state( state_info *state )
/*****************************************/
{
    Wdputs( "-- file " );
    Putdec( state->file );
    Wdputs( " addr  " );
    if( state->segment != 0 ) {
        Puthex( state->segment, 4 );
        Wdputs( ":" );
    }
    Puthex( state->address, 8 );
    Wdputs( " line " );
    Putdec( state->line );
    Wdputs( " column " );
    Putdec( state->column );
    if( state->is_stmt ) {
        Wdputs( " is_stmt" );
    }
    if( state->basic_block ) {
        Wdputs( " basic_block" );
    }
    if( state->end_sequence ) {
        Wdputs( " end_sequence" );
    }
    Wdputslc( "\n" );
}


static void get_standard_op( uint_8 value )
/*****************************************/
{
    const char      *result;
    int             i;

    result = Getname( value, readableStandardOps, NUM_STANDARD_OPS );
    if( result == NULL ) {
        Wdputs( "OP_" );
        Puthex( value, 2 );
        Wdputs( "                   " );
    } else {
        Wdputs( result );
        for( i = strlen( result ); i < 24; i++ ) {
            Wdputc( ' ' );
        }
    }
}


void Dump_lines( const uint_8 *input, uint length )
/*************************************************/
{
    const uint_8                *p;
    const uint_8                *stmt_start;
    uint                        opcode_base;
    uint                        *opcode_lengths;
    uint                        u;
    uint                        file_index;
    const uint_8                *name;
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
    const uint_8                *unit_base;

    p = input;
    while( p - input < length ) {

        unit_length = get_u32( (uint_32 *)p );
        p += sizeof( uint_32 );
        unit_base = p;

        Wdputs( "total_length: " );
        Puthex( unit_length, 8 );

        Wdputslc( "\nversion: " );
        Puthex( get_u16( (uint_16 *)p ), 4 );
        p += sizeof( uint_16 );

        Wdputslc( "\nprologue_length: " );
        Puthex( get_u32( (uint_32 *)p ), 8 );
        stmt_start = p;
        stmt_start += get_u32( (uint_32 *)p );
        p += sizeof( uint_32 );
        stmt_start += sizeof( uint_32 );
        min_instr = *p;
        Wdputslc( "\nminimum_instruction_length: " );
        Puthex( min_instr, 2 );
        p += 1;

        default_is_stmt = *p;
        Wdputslc( "\ndefault_is_stmt: " );
        Puthex( default_is_stmt, 2 );
        p += 1;

        line_base = *(int_8 *)p;
        Wdputslc( "\nline_base: " );
        Puthex( line_base, 2 );
        p += 1;

        line_range = *(uint_8 *)p;
        Wdputslc( "\nline_range: " );
        Puthex( line_range, 2 );
        p += 1;

        opcode_base = *p;
        Wdputslc( "\nopcode_base: " );
        Puthex( opcode_base, 2 );
        Wdputslc( "\n" );
        p += 1;
        opcode_lengths = malloc( sizeof( uint ) * opcode_base );
        Wdputslc( "standard_opcode_lengths:\n" );
        for( u = 0; u < opcode_base - 1; ++u ) {
            opcode_lengths[ u ] = *p;
            ++p;
            Putdecl( u, 4 );
            Wdputs( ": " );
            Putdec( opcode_lengths[ u ] );
            Wdputslc( "\n" );
        }

        Wdputs( "-- current_offset = " );
        Puthex( p - input, 8 );
        Wdputslc( "\n" );

        if( p - input >= length ) return;

        Wdputslc( "include directories\n" );
        file_index = 0;
        while( *p != 0 ) {
            ++file_index;
            name = p;
            p += strlen( (char *)p ) + 1;
            Wdputs( "path " );
            Putdec( file_index );
            Wdputs( ": '" );
            Wdputs( (char *)name );
            Wdputslc( "'\n" );
            if( p - input >= length ) return;
        }
        p++;
        Wdputslc( "file names\n" );
        file_index = 0;
        while( *p != 0 ) {
            ++file_index;
            name = p;
            p += strlen( (char *)p ) + 1;
            p = DecodeULEB128( p, &directory );
            p = DecodeULEB128( p, &mod_time );
            p = DecodeULEB128( p, &file_length );
            Wdputs( "file " );
                Putdec( file_index );
            Wdputs( ": '" );
                Wdputs( (char *)name );
            Wdputs( "' directory " );
                Putdec( directory );
            Wdputs( " mod_time " );
                Puthex( mod_time, 8 );
            Wdputs( " length " );
                Puthex( file_length, 8 );
            Wdputslc( "\n" );
            if( p - input >= length ) return;
        }
        p++;
        init_state( &state, default_is_stmt );
        Wdputs( "-- current_offset = " );
        Puthex( p - input, 8 );
        if( p != stmt_start ) {
            Wdputs( ":***Prologue length off***" );
        }
        Wdputslc( "\n" );
        while( p - unit_base < unit_length ) {
            op_code = *p;
            ++p;
            if( op_code == 0 ) {
                /* extended op_code */
                p = DecodeULEB128( p, &op_len );
                Wdputs( "len: " );
                Putdecl( op_len, 3 );
                Wdputc( ' ' );
                op_code = *p;
                ++p;
                --op_len;
                switch( op_code ) {
                case DW_LNE_end_sequence:
                    Wdputslc( "END_SEQUENCE\n" );
                    state.end_sequence = 1;
                    dump_state( &state );
                    init_state( &state, default_is_stmt );
                    p+= op_len;
                    break;
                case DW_LNE_set_address:
                    Wdputs( "SET_ADDRESS " );
                    if( op_len == 4 ) {
                        tmp = get_u32( (uint_32 *)p );
                    } else if( op_len == 2 ) {
                        tmp = get_u16( (uint_16 *)p );
                    } else {
                        tmp = 0xffffffff;
                    }
                    state.address = tmp;
                    Puthex( tmp, op_len * 2 );
                    Wdputslc( "\n" );
                    p += op_len;
                    break;
                case DW_LNE_set_segment_OLD:
                case DW_LNE_set_segment:
                    Wdputs( "SET_SEGMENT " );
                    if( op_len == 4 ) {
                        tmp = get_u32( (uint_32 *)p );
                    } else if( op_len == 2 ) {
                        tmp = get_u16( (uint_16 *)p );
                    } else {
                        tmp = 0xffffffff;
                    }
                    state.segment = tmp;
                    Puthex( tmp, op_len*2 );
                    Wdputslc( "\n" );
                    p += op_len;
                    break;
                case DW_LNE_define_file:
                    ++file_index;
                    name = p;
                    p += strlen( (char *)p ) + 1;
                    p = DecodeULEB128( p, &directory );
                    p = DecodeULEB128( p, &mod_time );
                    p = DecodeULEB128( p, &file_length );
                    Wdputs( "DEFINE_FILE " );
                    Putdec( file_index );
                    Wdputs( ": '" );
                    Wdputs( (char *)name );
                    Wdputs( "' directory " );
                    Putdec( directory );
                    Wdputs( " mod_time " );
                    Puthex( mod_time, 8 );
                    Wdputs( " length " );
                    Puthex( file_length, 8 );
                    break;
                default:
                    Wdputs( "** unknown extended opcode: " );
                    Puthex( op_code, 2 );
                    Wdputslc( "\n" );
                    p += op_len;
                    break;
                }
            } else if( op_code < opcode_base ) {
                get_standard_op( op_code );
                switch( op_code ) {
                case DW_LNS_copy:
                    Wdputslc( "\n" );
                    dump_state( &state );
                    state.basic_block = 0;
                    break;
                case DW_LNS_advance_pc:
                    p = DecodeULEB128( p, &tmp );
                    Putdec( tmp );
                    state.address += tmp * min_instr;
                    break;
                case DW_LNS_advance_line:
                    p = DecodeSLEB128( p, &itmp );
                    Putdecs( itmp );
                    state.line += itmp;
                    break;
                case DW_LNS_set_file:
                    p = DecodeULEB128( p, &tmp );
                    Putdec( tmp );
                    state.file = tmp;
                    break;
                case DW_LNS_set_column:
                    p = DecodeULEB128( p, &tmp );
                    Putdec( tmp );
                    state.column = tmp;
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
                    tmp = get_u16( (uint_16 *)p );
                    p += sizeof( uint_16 );
                    Puthex( tmp, 4 );
                    state.address += tmp;
                    break;
                default:
                    for( u = 0; u < opcode_lengths[ op_code - 1 ]; ++u ) {
                        p = DecodeULEB128( p, &tmp );
                        Puthex( tmp, 8 );
                    }
                }
            } else {
                Wdputs( "SPECIAL " );
                Puthex( op_code, 2 );
                itmp = op_code - opcode_base;
                Wdputs( ": addr incr: " );
                Putdecbz( ( itmp / line_range ) * min_instr, 2 );
                Wdputs( "  line incr: " );
                Putdecsbz( line_base + itmp % line_range, 2 );
                state.line += line_base + itmp % line_range;
                state.address += ( itmp / line_range ) * min_instr;
                Wdputslc( "\n" );
                dump_state( &state );
                state.basic_block = 0;
            }
            Wdputslc( "\n" );
        }
        free( opcode_lengths  );
        Wdputs( "-- current_offset = " );
        Puthex( p - input, 8 );
        Wdputslc( "\n" );
    }
}
