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


#include <stddef.h>
#include <stdlib.h>

#include "dwpriv.h"
#include "dwutils.h"
#include "dwmem.h"
#include "dwline.h"

static void writeFileName(
    dw_client                   cli,
    const char *                name,
    size_t                      len ) // len of name including terminator
{
    char                        buf[1+MAX_LEB128+1+_MAX_PATH+1+MAX_LEB128*3];
    char                        attribBuf[MAX_LEB128];
    char *                      end;
    int                         bufSize=0;

    buf[0] = 0;  // identifies extended opcode
    bufSize = 2+len; // size of sub-opcode, name+terminator, & path size

    // find out size of file time/size leb128's:
    end = ULEB128( attribBuf, 0 ); //NYI: replace 0 with time/date stamp of file
    bufSize += end-attribBuf; // add on size of time stamp val
    end = ULEB128( attribBuf, 0 ); //NYI: replace 0 with file size
    bufSize += end-attribBuf; // add on size of file size val

    end = ULEB128(buf+1,bufSize); // write the opcode size

    *end = DW_LNE_define_file; // write in the sub-opcode
    end++;

    end = strncpy(end,name,len); // write the filename
    end += len;

    end = ULEB128( end, 0 );    // not using a path index
        // write the file attributes
    end = ULEB128( end, 0 ); // NYI: replace 0 with time/date stamp of file
    end = ULEB128( end, 0 ); // NYI: replace 0 with size file

    CLIWrite( DW_DEBUG_LINE, buf, end-buf );
}


uint GetFileNumber(
    dw_client                   cli,
    const char *                name )
{
    size_t                      len;
    dw_include *                walk;

    len = strlen( name ) + 1;
    walk = cli->debug_line.files;
    while( walk != NULL ) {
        if( memcmp( name, walk->name, len ) == 0 ) break;
        walk = walk->next;
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


void DWENTRY DWSetFile(
    dw_client                   cli,
    const char *                filename )
{
    char                        buf[ 1 + MAX_LEB128 ];
    char *                      end;

    _Validate( filename != NULL );

    buf[0] = DW_LNS_set_file;
    end = LEB128( buf + 1, GetFileNumber( cli, filename ) );
    CLIWrite( DW_DEBUG_LINE, buf, end - buf );
}


void DWENTRY DWLineNum(
    dw_client           cli,
    uint                info,
    dw_linenum          line_num,
    dw_column           column,
    dw_addr_offset      addr )
{
    char                buf[ 3 + 2 * MAX_LEB128 ];
    char *              end;
    unsigned            size;

    /* set the basic_block register properly */
    if( info & DW_LN_BLK ) {
        buf[ 0 ] = DW_LNS_set_basic_block;
        CLIWrite( DW_DEBUG_LINE, buf, 1 );
    }

    /* set the is_stmt register properly */
    if( info & DW_LN_STMT ) {
        if( !cli->debug_line.is_stmt ) {
            cli->debug_line.is_stmt = 1;
            buf[ 0 ] = DW_LNS_negate_stmt;
            CLIWrite( DW_DEBUG_LINE, buf, 1 );
        }
    } else if( cli->debug_line.is_stmt ) {
        cli->debug_line.is_stmt = 0;
        buf[ 0 ] = DW_LNS_negate_stmt;
        CLIWrite( DW_DEBUG_LINE, buf, 1 );
    }

    if( column != cli->debug_line.column ) {
        cli->debug_line.column = column;
        buf[ 0 ] = DW_LNS_set_column;
        end = LEB128( buf + 1, column );
        CLIWrite( DW_DEBUG_LINE, buf, end - buf );
    }

    size = DWLineGen( line_num - cli->debug_line.line,
                      addr - cli->debug_line.addr, buf );
    CLIWrite( DW_DEBUG_LINE, buf, size );
    cli->debug_line.addr = addr;
    cli->debug_line.line = line_num;
}

void DWLineAddr(  dw_client  cli, dw_sym_handle sym, dw_addr_offset addr )
{
    char                        buf[1+MAX_LEB128+sizeof(dw_targ_addr )];
    char *                      end;

    buf[ 0 ] = 0;  //extended
    end = ULEB128(buf+1, 1+cli->offset_size ); // write the opcode size
    *end = DW_LNE_set_address;
    ++end;
    CLIWrite( DW_DEBUG_LINE, buf, end-buf );
    CLIReloc3( DW_DEBUG_LINE, DW_W_LABEL, sym );
    cli->debug_line.addr = addr;
}

void DWLineSeg(  dw_client  cli, dw_sym_handle sym )
{
    char                        buf[1+MAX_LEB128+sizeof(dw_targ_addr )];
    char *                      end;

    if( cli->segment_size != 0 ){
        buf[ 0 ] = 0;  //extended
        end = ULEB128(buf+1, 1+cli->segment_size ); // write the opcode size
        *end = DW_LNE_set_segment;
        ++end;
        CLIWrite( DW_DEBUG_LINE, buf, end-buf );
        CLIReloc3( DW_DEBUG_LINE, DW_W_LABEL_SEG, sym );
    }
}

void InitDebugLine(
    dw_client                   cli,
    const char *                source_filename,
    char *                      inc_list,
    unsigned                    inc_list_len )
{
    stmt_prologue prol = {
        0,
        2,
        sizeof( stmt_prologue )
                - offsetof( stmt_prologue, minimum_instruction_length ),
        DW_MIN_INSTR_LENGTH,
        0,
        DWLINE_BASE,
        DWLINE_RANGE,
        DWLINE_OPCODE_BASE,
        { 0, 1, 1, 1, 1, 0, 0, 0, 0 }
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
    CLIWrite( DW_DEBUG_LINE, (char*)&prol, sizeof( prol ) );

    if( inc_list != 0 ) {       // write the include list
        CLIWrite( DW_DEBUG_LINE, inc_list, inc_list_len );
    }

    CLIWrite( DW_DEBUG_LINE, terminators, sizeof(terminators) );

    /* and put out the source filename */
    GetFileNumber( cli, source_filename );
}


void FiniDebugLine(
    dw_client                   cli )
{
    char                        buf[ sizeof( uint_32 ) ];
    long                        size;

    buf[ 0 ] = 0;
    buf[ 1 ] = 1;
    buf[ 2 ] = DW_LNE_end_sequence;
    CLIWrite( DW_DEBUG_LINE, buf, 3 );
    size = CLITell( DW_DEBUG_LINE ) - sizeof( uint_32 )
        - cli->section_base[ DW_DEBUG_LINE ];
    WriteU32( buf, size );
    CLISeek( DW_DEBUG_LINE, cli->section_base[ DW_DEBUG_LINE ], DW_SEEK_SET );
    CLIWrite( DW_DEBUG_LINE, buf, sizeof(uint_32) );
    CLISeek( DW_DEBUG_LINE, 0, DW_SEEK_END );
    FreeChain( cli, cli->debug_line.files );
}

