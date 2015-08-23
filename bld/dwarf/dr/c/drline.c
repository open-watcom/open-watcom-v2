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
    int             name_buf_len;
    int             curr_len;

    name_buf = NULL;
    name_buf_len = 0;
    curr = info->rdr.curr;
    finish = info->rdr.finish;
    min_ins_len = info->rdr.min_ins_len;
    line_base = info->rdr.line_base;
    line_range = info->rdr.line_range;
    opcode_base = info->rdr.opcode_base;
    info->state.addr_set = TRUE;  // address starts at 0
    cont = TRUE;
    while( cont && curr < finish ) {    // now go through the statement program
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
                        break;
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
                ++info->rdr.file_idx;
                if( file == NULL ) {
                    curr += length;
                } else {
                    curr++;
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
                    df.dir = (uint_16)DWRVMReadULEB128( &curr );    // directory index
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
            info->state.addr_set = FALSE;
            info->state.basic_blk = FALSE;
        }
    }
    info->rdr.curr = curr;
    DWRFREE( name_buf );
    return( cont );
}


static dr_handle InitProgInfo( prog_rdr *rdr, dr_handle start, uint_16 seg )
/**************************************************************************/
// Init statement program info
{
    unsigned_32         len;
    dr_handle           pos;
    int                 index;

    rdr->seg = seg;
    len = DWRVMReadDWord( start );
    rdr->finish = start + 4 + len;
    len = DWRVMReadDWord( start + STMT_PROLOGUE_HDR_PROLOGUE_LEN );
    rdr->start = start + STMT_PROLOGUE_HDR_PROLOGUE_LEN + 4 + len;
    rdr->curr = rdr->start;
    rdr->min_ins_len = DWRVMReadByte( start + STMT_PROLOGUE_HDR_MIN_INS_LEN );
    rdr->def_is_stmt = DWRVMReadByte( start + STMT_PROLOGUE_HDR_DEF_IN_STMT );
    rdr->line_base   = DWRVMReadByte( start + STMT_PROLOGUE_HDR_LINE_BASE );
    rdr->line_range  = DWRVMReadByte( start + STMT_PROLOGUE_HDR_LINE_RANGE );
    len = DWRVMReadByte( start + STMT_PROLOGUE_HDR_OPCODE_BASE );
    rdr->opcode_base = len--;
    rdr->op_lens = DWRALLOC( len );
    pos = start + STMT_PROLOGUE_STANDARD_OPCODE_LENGTHS;
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
    bool        ret;

    stmt = InitProgInfo( &info.rdr, stmt, seg );
    InitState( &info.state, seg, info.rdr.def_is_stmt );
    ret = WlkStateProg( &info, wlk, d, NULL, NULL );
    FiniProgInfo( &info.rdr );
    return( ret );
}

bool DRWalkLFiles( dr_handle stmt, DRLFILEWLK file, void *file_data,
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
    int             name_buf_len;
    int             curr_len;

    name_buf = NULL;
    name_buf_len = 0;
    stmt = InitProgInfo( &info.rdr, stmt, 0 );
    cont = TRUE;
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
        df.dir = (uint_16)DWRVMReadULEB128( &stmt );    // directory index
        df.time = DWRVMReadULEB128( &stmt );            // time
        df.len = DWRVMReadULEB128( &stmt );             // length
        df.index = (filetab_idx)info.rdr.file_idx;      // index
        /* call file walker */
        cont = file( file_data, &df );
    }
    if( cont ) {
        InitState( &info.state, 0, info.rdr.def_is_stmt );
        WlkStateProg( &info, NULL, NULL, file, file_data );
    }
    DWRFREE( name_buf );
    FiniProgInfo( &info.rdr );
    return( cont );
}
