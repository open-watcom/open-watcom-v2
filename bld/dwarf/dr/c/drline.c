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
* Description:  DWARF statement program virtual machine.
*
****************************************************************************/


#include "drpriv.h"
#include "drutils.h"
#include <string.h>
#include "walloca.h"

#include "clibext.h"


static void InitState( dr_line_data *state, uint_16 seg, bool is_stmt )
/*********************************************************************/
// Start state
{
    state->seg = seg;
    state->offset = 0;
    state->file = 1;
    state->line = 1;
    state->col = 0;
    state->is_stmt = is_stmt;
    state->basic_blk = FALSE;
    state->end_seq = FALSE;
    state->addr_set = FALSE;
}

static dr_handle  DefineFile( dr_handle start, dr_line_file *df )
/***************************************************************/
{

    df->name = DWRVMCopyString( &start );
    df->dir = (uint_16)DWRVMReadULEB128( &start );  // directory index
    df->time = DWRVMReadULEB128( &start );          // time
    df->len = DWRVMReadULEB128( &start );           // length
    return( start );
}

typedef struct { /* stmt program read info */
    dr_handle   start;
    dr_handle   curr;
    dr_handle   finish;
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


static bool WlkStateProg( line_info *info, DRCUEWLK cue, void *cue_data,
                                          DRLFILEWLK file, void *file_data )
/**************************************************************************/
// Run the statement program
// On each row append (as blathered about in DWARF spec) call the wlk
{
    dr_handle       curr;
    dr_handle       finish;
    unsigned_8      value;
    unsigned        length;
    unsigned_8      min_ins_len;
    signed_8        line_base;
    unsigned_8      line_range;
    unsigned_8      opcode_base;
    dr_line_file    df;
    bool            cont;
    dw_lns          value_lns;
    dw_lne          value_lne;

    curr = info->rdr.curr;
    finish = info->rdr.finish;
    min_ins_len = info->rdr.min_ins_len;
    line_base = info->rdr.line_base;
    line_range = info->rdr.line_range;
    opcode_base = info->rdr.opcode_base;
    info->state.addr_set = TRUE;  // address starts at 0
    cont = TRUE;
    while( curr < finish ) {    // now go through the statement program
        value_lns = DWRVMReadByte( curr );
        curr++;
        if( value_lns == 0 ) {      // it's an extended opcode
            length = DWRVMReadULEB128( &curr );
            value_lne = DWRVMReadByte( curr );
            switch( value_lne ) {
            case DW_LNE_end_sequence:
                curr++;
                info->state.end_seq = TRUE;
                /* append a row */
                if( cue != NULL ) {
                    cont = cue( cue_data, &info->state );
                    if( !cont ) {
                        goto end_loop;
                    }
                }
                InitState( &info->state, info->rdr.seg, info->rdr.def_is_stmt );
                break;
            case DW_LNE_set_address:
                curr++;
                --length;
                info->state.offset = DWRReadInt( curr, length );
                info->state.addr_set = TRUE;
                curr += length;
                break;
            case DW_LNE_set_segment_OLD:
            case DW_LNE_set_segment:
                curr++;
                --length;
                info->state.seg = (uint_16)DWRReadInt( curr, length );
                curr += length;
                break;
            case DW_LNE_define_file:
                /* call file walker */
                ++info->rdr.file_idx;
                if( file != NULL ) {
                    DefineFile( curr + 1, &df );
                    df.index = (filetab_idx)info->rdr.file_idx;
                    cont = file( file_data, &df );
                    if( !cont ) {
                        goto end_loop;
                    }
                }
                curr += length;
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
                        goto end_loop;
                    }
                }
                info->state.addr_set = FALSE;
                info->state.basic_blk = FALSE;
                break;
            case DW_LNS_advance_pc:
                info->state.offset += DWRVMReadULEB128( &curr ) * min_ins_len;
                break;
            case DW_LNS_advance_line:
                info->state.line += DWRVMReadSLEB128( &curr );
                break;
            case DW_LNS_set_file:
                info->state.file = (uint_16)DWRVMReadULEB128( &curr );
                break;
            case DW_LNS_set_column:
                info->state.col = (uint_16)DWRVMReadULEB128( &curr );
                break;
            case DW_LNS_negate_stmt:
                info->state.is_stmt = !info->state.is_stmt;
                break;
            case DW_LNS_set_basic_block:
                info->state.basic_blk = TRUE;
                break;
            case DW_LNS_const_add_pc:
                value = 255 - opcode_base;
                info->state.offset += value / line_range * min_ins_len;
                break;
            case DW_LNS_fixed_advance_pc:
                info->state.offset += DWRVMReadWord( curr );
                curr += 2;
                break;
            default: //op codes not processed
                value = info->rdr.op_lens[value_lns];
                while( value > 0 ) {
                    DWRVMSkipLEB128( &curr );
                    value--;
                }
            }
        } else { /* special opcodes */
            value = (uint_8)( value_lns - opcode_base );
            info->state.offset += value / line_range * min_ins_len;
            info->state.line += value % line_range + line_base;
            /* append a row */
            if( cue != NULL ) {
                cont = cue( cue_data, &info->state );
                if( !cont ) {
                    goto end_loop;
                }
            }
            info->state.addr_set = FALSE;
            info->state.basic_blk = FALSE;
        }
    } end_loop:;
    info->rdr.curr = curr;
    return( cont );
}


static dr_handle InitProgInfo( prog_rdr *rdr, dr_handle start, uint_16 seg )
/**************************************************************************/
// Init statement program info
{
    unsigned_32 len;

    rdr->seg = seg;
    len = DWRVMReadDWord( start );
    start += 4;
    rdr->finish = start + len;
    start += 2; /*skip version*/
    len = DWRVMReadDWord( start );
    start += 4;
    rdr->start = start + len;
    rdr->curr = start + len;
    rdr->min_ins_len = DWRVMReadByte( start );
    start += 1;
    rdr->def_is_stmt = DWRVMReadByte( start );
    start += 1;
    rdr->line_base = DWRVMReadByte( start );
    start += 1;
    rdr->line_range = DWRVMReadByte( start );
    start += 1;
    rdr->opcode_base = DWRVMReadByte( start );
    start += 1;
    rdr->op_lens = NULL;
    rdr->file_idx = 0;
    rdr->dir_idx = 0;
    return( start );
}

extern dr_handle  DRGetStmtList( dr_handle ccu )
/**********************************************/
// Return the start of the statement list or 0 if no lines
{
    dr_handle   abbrev;

    abbrev = DWRGetAbbrev( &ccu );
    if( DWRScanForAttrib( &abbrev, &ccu, DW_AT_stmt_list ) ) {
        ccu = DWRCurrNode->sections[DR_DEBUG_LINE].base + DWRReadConstant( abbrev, ccu );
    } else {
        ccu = DR_HANDLE_NUL;
    }
    return( ccu );
}

bool DRWalkLines( dr_handle stmt, uint_16 seg, DRCUEWLK wlk, void *d )
/********************************************************************/
// Run the statement program
{
    line_info   info;
    int         index;
    bool        ret;

    stmt = InitProgInfo( &info.rdr, stmt, seg );
    info.rdr.op_lens = __alloca( info.rdr.opcode_base - 1 );
    for( index = 0; index < info.rdr.opcode_base - 1; index++ ) {
        info.rdr.op_lens[index] = DWRVMReadByte( stmt );
        stmt++;
    }
    InitState( &info.state, seg, info.rdr.def_is_stmt );
    ret = WlkStateProg( &info, wlk, d, NULL, NULL );
    return( ret );
}

bool DRWalkLFiles( dr_handle stmt, DRLFILEWLK file, void *file_data,
                                        DRLDIRWLK dir, void *dir_data )
/*********************************************************************/
// Run the statement program
{
    line_info       info;
    dr_fileidx      index;
    unsigned_8      value;
    dr_line_dir     dd;
    dr_line_file    df;
    bool            cont;
    int             i;

    stmt = InitProgInfo( &info.rdr, stmt, 0 );
    info.rdr.op_lens = __alloca( info.rdr.opcode_base - 1 );
    for( i = 0; i < info.rdr.opcode_base - 1; i++ ) {
        info.rdr.op_lens[i] = DWRVMReadByte( stmt );
        stmt++;
    }
    cont = TRUE;
    index = 0;
    while( stmt < info.rdr.start ) {            // get directory table
        value = DWRVMReadByte( stmt );
        if( value == 0 )
            break;
        ++index;
        dd.name = DWRVMCopyString( &stmt );
        dd.index = (filetab_idx)index;
        cont = dir( dir_data, &dd );
        if( !cont ) {
            return( cont );
        }
    }
    info.rdr.dir_idx = index;
    stmt++;
    index = 0;
    while( stmt < info.rdr.start ) {            // get filename table
        value = DWRVMReadByte( stmt );
        if( value == 0 )
            break;
        ++index;
        stmt = DefineFile( stmt, &df );
        df.index = (filetab_idx)index;
        cont = file( file_data, &df );
        if( !cont ) {
            return( cont );
        }
    }
    info.rdr.file_idx = index;
    InitState( &info.state, 0, info.rdr.def_is_stmt );
    WlkStateProg( &info, NULL, NULL, file, file_data );
    return( cont );
}
