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
* Description:  Generate file and line number information.
*
****************************************************************************/


#include <stddef.h>
#include <stdlib.h>
#include <limits.h>

#ifndef _MAX_PATH
#define _MAX_PATH (PATH_MAX + 1)
#endif

#include "dwpriv.h"
#include "dwutils.h"
#include "dwmem.h"
#include "dwline.h"

static void writeFileName( dw_client cli, const char *name, size_t len ) // len of name including terminator
{
    uint_8                      buf[1 + MAX_LEB128 + 1 + _MAX_PATH + 1 + MAX_LEB128 * 3];
    uint_8                      attribBuf[MAX_LEB128];
    uint_8                      *end;
    size_t                      bufSize = 0;

    buf[0] = 0;         // identifies extended opcode
    bufSize = 2 + len;  // size of sub-opcode, name+terminator, & path size

    // find out size of file time/size leb128's:
    end = ULEB128( attribBuf, 0 );  //NYI: replace 0 with time/date stamp of file
    bufSize += end - attribBuf;     // add on size of time stamp val
    end = ULEB128( attribBuf, 0 );  //NYI: replace 0 with file size
    bufSize += end - attribBuf;     // add on size of file size val

    end = ULEB128( buf + 1, (dw_uconst)bufSize );   // write the opcode size

    *end = DW_LNE_define_file;      // write in the sub-opcode
    end++;

    end = (uint_8 *)strncpy( (char *)end, name, len ); // write the filename
    end += len;

    end = ULEB128( end, 0 );    // not using a path index
                                // write the file attributes
    end = ULEB128( end, 0 );    // NYI: replace 0 with time/date stamp of file
    end = ULEB128( end, 0 );    // NYI: replace 0 with size file

    CLIWrite( DW_DEBUG_LINE, buf, end - buf );
}


uint GetFileNumber( dw_client cli, const char *name )
{
    size_t                      len;
    dw_include                  *walk;

    len = strlen( name ) + 1;
    for( walk = cli->debug_line.files; walk != NULL; walk = walk->next ) {
        if( memcmp( name, walk->name, len ) == 0 ) {
            break;
        }
    }
    if( walk == NULL ) {
        walk = CLIAlloc( ( sizeof( dw_include ) - 1 ) + len );
        memcpy( walk->name, name, len );
        walk->next = cli->debug_line.files;
        cli->debug_line.files = walk;
        if( walk->next == NULL ) {
            walk->number = 1;
        } else {
            walk->number = walk->next->number + 1;
        }
        writeFileName( cli, name, len );
    }
    return( walk->number );
}


void DWENTRY DWSetFile( dw_client cli, const char *filename )
{
    uint_8                      buf[1 + MAX_LEB128];
    uint_8                      *end;

    _Validate( filename != NULL );

    buf[0] = DW_LNS_set_file;
    end = LEB128( buf + 1, GetFileNumber( cli, filename ) );
    CLIWrite( DW_DEBUG_LINE, buf, end - buf );
}


void DWENTRY DWLineNum( dw_client cli, uint info, dw_linenum line_num, dw_column column, dw_addr_offset addr )
{
    uint_8              buf[3 + 2 * MAX_LEB128];
    uint_8              *end;
    size_t              size;

    /* set the basic_block register properly */
    if( info & DW_LN_BLK ) {
        buf[0] = DW_LNS_set_basic_block;
        CLIWrite( DW_DEBUG_LINE, buf, 1 );
    }

    /* set the is_stmt register properly */
    if( info & DW_LN_STMT ) {
        if( !cli->debug_line.is_stmt ) {
            cli->debug_line.is_stmt = 1;
            buf[0] = DW_LNS_negate_stmt;
            CLIWrite( DW_DEBUG_LINE, buf, 1 );
        }
    } else if( cli->debug_line.is_stmt ) {
        cli->debug_line.is_stmt = 0;
        buf[0] = DW_LNS_negate_stmt;
        CLIWrite( DW_DEBUG_LINE, buf, 1 );
    }

    if( column != cli->debug_line.column ) {
        cli->debug_line.column = column;
        buf[0] = DW_LNS_set_column;
        end = LEB128( buf + 1, column );
        CLIWrite( DW_DEBUG_LINE, buf, end - buf );
    }

    size = DWLineGen( line_num - cli->debug_line.line,
                      addr - cli->debug_line.addr, buf );
    CLIWrite( DW_DEBUG_LINE, buf, size );
    cli->debug_line.addr = addr;
    cli->debug_line.line = line_num;
}

void DWLineAddr( dw_client cli, dw_sym_handle sym, dw_addr_offset addr )
{
    uint_8      buf[1 + MAX_LEB128 + sizeof( dw_targ_addr )];
    uint_8      *end;

    buf[0] = 0;  //extended
    end = ULEB128( buf + 1, 1 + cli->offset_size ); // write the opcode size
    *end = DW_LNE_set_address;
    ++end;
    CLIWrite( DW_DEBUG_LINE, buf, end - buf );
    CLIReloc3( DW_DEBUG_LINE, DW_W_LABEL, sym );
    cli->debug_line.addr = addr;
}

void DWLineSeg( dw_client cli, dw_sym_handle sym )
{
    uint_8                      buf[1 + MAX_LEB128 + sizeof( dw_targ_addr )];
    uint_8                      *end;

    if( cli->segment_size != 0 ) {
        buf[0] = 0;  //extended
        end = ULEB128( buf + 1, 1 + cli->segment_size ); // write the opcode size
        *end = DW_LNE_WATCOM_set_segment_OLD;
//        *end = DW_LNE_WATCOM_set_segment;
        ++end;
        CLIWrite( DW_DEBUG_LINE, buf, end - buf );
        CLIReloc3( DW_DEBUG_LINE, DW_W_LABEL_SEG, sym );
    }
}

void InitDebugLine( dw_client cli, const char *source_filename, const char *inc_list, unsigned inc_list_len )
{
    stmt_prologue prol = {
        0,
        DWARF_IMPL_VERSION,
        sizeof( stmt_prologue ) - offsetof( stmt_prologue, minimum_instruction_length ),
        DW_MIN_INSTR_LENGTH,
        0,
        DWLINE_BASE,
        DWLINE_RANGE,
        DWLINE_OPCODE_BASE,
        {   /* LEB128 args  - Instruction op-code       */
            0,              /* DW_LNS_copy              */
            1,              /* DW_LNS_advance           */
            1,              /* DW_LNS_advance_line      */
            1,              /* DW_LNS_set_file          */
            1,              /* DW_LNS_set_column        */
            0,              /* DW_LNS_negate_stmt       */
            0,              /* DW_LNS_set_basic_block   */
            0,              /* DW_LNS_const_add_pc      */
            0               /* DW_LNS_fixed_advance_pc  */
            /*
            //  GNU sets the last entry to 1. This is (maybe?) incorrect as the DW_LNS_fixed_advance_pc
            //  opcode has a fixed uhalf (uint_16)argument, not a (U)LEB128 argument.
            */
        }
    };

    static uint_8 const terminators[] = {0,0};

    prol.prologue_length += inc_list_len + 2;   // +2 for 2 list terminators
    cli->debug_line.files = NULL;
    cli->debug_line.addr = 0;
    cli->debug_line.line = 1;
    cli->debug_line.column = 0;
    cli->debug_line.is_stmt = 0;
    cli->debug_line.end_sequence = 0;

    /* write the prologue */
    CLIWrite( DW_DEBUG_LINE, (char *)&prol, sizeof( prol ) );

    if( inc_list != 0 ) {       // write the include list
        CLIWrite( DW_DEBUG_LINE, inc_list, inc_list_len );
    }

    CLIWrite( DW_DEBUG_LINE, terminators, sizeof( terminators ) );

    /* and put out the source filename */
    GetFileNumber( cli, source_filename );
}


void FiniDebugLine( dw_client cli )
{
    char                        buf[sizeof( uint_32 )];
    long                        size;

    buf[0] = 0;
    buf[1] = 1;
    buf[2] = DW_LNE_end_sequence;
    CLIWrite( DW_DEBUG_LINE, buf, 3 );
    size = CLITell( DW_DEBUG_LINE ) - sizeof( uint_32 ) - cli->section_base[DW_DEBUG_LINE];
    WriteU32( buf, size );
    CLISeek( DW_DEBUG_LINE, cli->section_base[DW_DEBUG_LINE], DW_SEEK_SET );
    CLIWrite( DW_DEBUG_LINE, buf, sizeof( uint_32 ) );
    CLISeek( DW_DEBUG_LINE, 0, DW_SEEK_END );
    FreeChain( cli, cli->debug_line.files );
}
