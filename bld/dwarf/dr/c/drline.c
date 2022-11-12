/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DWARF statement program virtual machine (.debug_line section).
*
****************************************************************************/


#include "drpriv.h"
#include <string.h>
#include "drutils.h"
#include "walloca.h"

#include "clibext.h"


typedef struct { /* stmt program read info */
    drmem_hdl   start;
    drmem_hdl   curr;
    drmem_hdl   finish;
    unsigned_16 seg;
    signed_8    line_base;
    unsigned_8  line_range;
    unsigned_8  min_ins_len;
    unsigned_8  def_is_stmt;
    unsigned_8  opcode_base;
    unsigned_8  *op_lens; /* NULL if no new opcodes */
    dr_fileidx  file_idx;
    dr_fileidx  dir_idx;
} prog_rdr;

typedef struct line_info {
    dr_line_data state;
    prog_rdr     rdr;
} line_info;


static void InitState( line_info *info )
/**************************************/
// Start state
{
    info->state.seg = info->rdr.seg;
    info->state.offset = 0;
    info->state.file = 1;
    info->state.line = 1;
    info->state.column = 0;
    info->state.is_stmt = info->rdr.def_is_stmt;
    info->state.basic_blk = false;
    info->state.end_seq = false;
    info->state.addr_set = true;  // address starts at 0
}

static bool WlkStateProg( line_info *info, DRCUEWLK cue, void *cue_data,
                                          DRLFILEWLK file, void *file_data )
/**************************************************************************/
// Run the statement program
// On each row append (as blathered about in DWARF spec) call the wlk
{
    drmem_hdl       curr;
    drmem_hdl       finish;
    int             value;
    unsigned        length;
    int             min_ins_len;
    int             line_base;
    int             line_range;
    int             opcode_base;
    dr_line_file    df;
    bool            cont;
    dw_lns          value_lns;
    dw_lne          value_lne;
    char            *name_buf;
    size_t          name_buf_len;
    size_t          curr_len;

    name_buf = NULL;
    name_buf_len = 0;
    InitState( info );
    curr = info->rdr.curr;
    finish = info->rdr.finish;
    min_ins_len = info->rdr.min_ins_len;
    line_base = info->rdr.line_base;
    line_range = info->rdr.line_range;
    opcode_base = info->rdr.opcode_base;
    cont = true;
    while( cont && curr < finish ) {    // now go through the statement program
        value_lns = DWRVMReadByte( curr );
        curr++;
        if( value_lns == 0 ) {      // it's an extended opcode
            length = DWRVMReadULEB128( &curr );
            value_lne = DWRVMReadByte( curr );
            curr++;
            --length;
            if( DWRCurrNode->wat_producer_ver == VER_V1 || DWRCurrNode->wat_producer_ver == VER_V2 ) {
                if( value_lne == DW_LNE_WATCOM_set_segment_OLD ) {
                    value_lne = DW_LNE_WATCOM_set_segment;
                }
            }
            switch( value_lne ) {
            case DW_LNE_end_sequence:
                info->state.end_seq = true;
                /* append a row */
                if( cue != NULL ) {
                    cont = cue( cue_data, &info->state );
                    if( !cont ) {
                        break;
                    }
                }
                InitState( info );
                break;
            case DW_LNE_set_address:
                info->state.offset = DWRReadInt( curr, length );
                info->state.addr_set = true;
                curr += length;
                break;
            case DW_LNE_set_discriminator:
                // it is used by Watcom and OW until OW1.9 for DW_LNE_WATCOM_set_segment
                // it is in colission with Dwarf specification V4 and above
                // following code must handle this for backward compatibility
                break;
            case DW_LNE_WATCOM_set_segment:
                info->state.seg = (uint_16)DWRReadInt( curr, length );
                curr += length;
                break;
            case DW_LNE_define_file:
                ++info->rdr.file_idx;
                if( file == NULL ) {
                    curr += length;
                } else {
                    curr_len = DWRVMGetStrBuff( curr, name_buf, name_buf_len );
                    if( curr_len > name_buf_len ) {
                        /* extend name buffer */
                        if( name_buf != NULL )
                            DWRFREE( name_buf );
                        name_buf_len = curr_len;
                        if( name_buf_len < 256 )
                            name_buf_len = 256;
                        name_buf = DWRALLOC( name_buf_len );
                        curr_len = DWRVMGetStrBuff( curr, name_buf, name_buf_len );
                    }
                    curr += curr_len;
                    df.name = name_buf;                             // directory path
                    df.dir = DWRVMReadULEB128( &curr );             // directory index
                    df.time = DWRVMReadULEB128( &curr );            // time
                    df.len = DWRVMReadULEB128( &curr );             // length
                    df.index = (filetab_idx)info->rdr.file_idx;     // index
                    /* call file walker */
                    cont = file( file_data, &df );
                }
                break;
            default:
                curr += length;
            }
        } else if( value_lns < opcode_base ) {  // it is a standard opcode
            switch( value_lns ) {
            case DW_LNS_copy:
                /* append a row */
                if( cue != NULL ) {
                    cont = cue( cue_data, &info->state );
                    if( !cont ) {
                        break;
                    }
                }
                info->state.addr_set = false;
                info->state.basic_blk = false;
                break;
            case DW_LNS_advance_pc:
                info->state.offset += DWRVMReadULEB128( &curr ) * min_ins_len;
                break;
            case DW_LNS_advance_line:
                info->state.line += DWRVMReadSLEB128( &curr );
                break;
            case DW_LNS_set_file:
                info->state.file = DWRVMReadULEB128( &curr );
                break;
            case DW_LNS_set_column:
                info->state.column = DWRVMReadULEB128( &curr );
                break;
            case DW_LNS_negate_stmt:
                info->state.is_stmt = !info->state.is_stmt;
                break;
            case DW_LNS_set_basic_block:
                info->state.basic_blk = true;
                break;
            case DW_LNS_const_add_pc:
                value = 255 - opcode_base;
                info->state.offset += ( value / line_range ) * min_ins_len;
                break;
            case DW_LNS_fixed_advance_pc:
                info->state.offset += DWRVMReadWord( curr );
                curr += 2;
                break;
            default: //op codes not processed
                for( value = info->rdr.op_lens[value_lns - 1]; value > 0; --value ) {
                    DWRVMSkipLEB128( &curr );
                }
            }
        } else { /* special opcodes */
            value = value_lns - opcode_base;
            info->state.offset += value / line_range * min_ins_len;
            info->state.line += ( value % line_range ) + line_base;
            /* append a row */
            if( cue != NULL ) {
                cont = cue( cue_data, &info->state );
                if( !cont ) {
                    break;
                }
            }
            info->state.addr_set = false;
            info->state.basic_blk = false;
        }
    }
    info->rdr.curr = curr;
    DWRFREE( name_buf );
    return( cont );
}


static drmem_hdl InitProgInfo( prog_rdr *rdr, drmem_hdl start, uint_16 seg )
/**************************************************************************/
// Init statement program info
{
    unsigned        len;
    drmem_hdl       pos;
    unsigned        index;

    rdr->seg = seg;
    rdr->finish = start + 4 + DWRVMReadDWord( start );
    rdr->start = start + offsetof( stmt_prologue, prologue_length ) + 4 + DWRVMReadDWord( start + offsetof( stmt_prologue, prologue_length ) );
    rdr->curr = rdr->start;
    rdr->min_ins_len = DWRVMReadByte( start + offsetof( stmt_prologue, minimum_instruction_length ) );
    rdr->def_is_stmt = DWRVMReadByte( start + offsetof( stmt_prologue, default_is_stmt ) );
    rdr->line_base   = DWRVMReadByte( start + offsetof( stmt_prologue, line_base ) );
    rdr->line_range  = DWRVMReadByte( start + offsetof( stmt_prologue, line_range ) );
    rdr->opcode_base = DWRVMReadByte( start + offsetof( stmt_prologue, opcode_base ) );
    len = rdr->opcode_base - 1;
    rdr->op_lens = DWRALLOC( len );
    pos = start + offsetof( stmt_prologue, standard_opcode_lengths );
    for( index = 0; index < len; index++ ) {
        rdr->op_lens[index] = DWRVMReadByte( pos++ );
    }
    rdr->dir_idx = 0;
    rdr->file_idx = 0;
    return( pos );
}

static void FiniProgInfo( prog_rdr *rdr )
{
    DWRFREE( rdr->op_lens );
}

drmem_hdl  DRGetStmtList( drmem_hdl ccu )
/***************************************/
// Return the start of the statement list or 0 if no lines
{
    drmem_hdl   abbrev;

    abbrev = DWRSkipTag( &ccu ) + 1;
    if( DWRScanForAttrib( &abbrev, &ccu, DW_AT_stmt_list ) ) {
        ccu = DWRCurrNode->sections[DR_DEBUG_LINE].base + DWRReadConstant( abbrev, ccu );
    } else {
        ccu = DRMEM_HDL_NULL;
    }
    return( ccu );
}

bool DRWalkLines( drmem_hdl stmt, uint_16 seg, DRCUEWLK wlk, void *d )
/********************************************************************/
// Run the statement program
{
    line_info   info;
    bool        ret;

    stmt = InitProgInfo( &info.rdr, stmt, seg );
    ret = WlkStateProg( &info, wlk, d, NULL, NULL );
    FiniProgInfo( &info.rdr );
    return( ret );
}

bool DRWalkLFiles( drmem_hdl stmt, DRLFILEWLK file, void *file_data,
                                        DRLDIRWLK dir, void *dir_data )
/*********************************************************************/
// Run the statement program
{
    line_info       info;
    unsigned_8      value;
    dr_line_dir     dd;
    dr_line_file    df;
    bool            cont;
    char            *name_buf;
    size_t          name_buf_len;
    size_t          curr_len;

    name_buf = NULL;
    name_buf_len = 0;
    stmt = InitProgInfo( &info.rdr, stmt, 0 );
    cont = true;
    while( cont && stmt < info.rdr.start ) {    // get directory table
        value = DWRVMReadByte( stmt );
        if( value == 0 ) {
            stmt++;
            break;
        }
        curr_len = DWRVMGetStrBuff( stmt, name_buf, name_buf_len );
        if( curr_len > name_buf_len ) {
            /* extend name buffer */
            if( name_buf != NULL )
                DWRFREE( name_buf );
            name_buf_len = curr_len;
            if( name_buf_len < 256 )
                name_buf_len = 256;
            name_buf = DWRALLOC( name_buf_len );
            curr_len = DWRVMGetStrBuff( stmt, name_buf, name_buf_len );
        }
        stmt += curr_len;
        info.rdr.dir_idx++;
        dd.name = name_buf;
        dd.index = (filetab_idx)info.rdr.dir_idx;
        /* call directory walker */
        cont = dir( dir_data, &dd );
    }
    while( cont && stmt < info.rdr.start ) {    // get filename table
        value = DWRVMReadByte( stmt );
        if( value == 0 )
            break;
        curr_len = DWRVMGetStrBuff( stmt, name_buf, name_buf_len );
        if( curr_len > name_buf_len ) {
            /* extend name buffer */
            if( name_buf != NULL )
                DWRFREE( name_buf );
            name_buf_len = curr_len;
            if( name_buf_len < 256 )
                name_buf_len = 256;
            name_buf = DWRALLOC( name_buf_len );
            curr_len = DWRVMGetStrBuff( stmt, name_buf, name_buf_len );
        }
        stmt += curr_len;
        info.rdr.file_idx++;
        df.name = name_buf;                             // directory path
        df.dir = DWRVMReadULEB128( &stmt );             // directory index
        df.time = DWRVMReadULEB128( &stmt );            // time
        df.len = DWRVMReadULEB128( &stmt );             // length
        df.index = (filetab_idx)info.rdr.file_idx;      // index
        /* call file walker */
        cont = file( file_data, &df );
    }
    if( cont ) {
        WlkStateProg( &info, NULL, NULL, file, file_data );
    }
    DWRFREE( name_buf );
    FiniProgInfo( &info.rdr );
    return( cont );
}
