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
* Description:  Profiler disassembly view routines.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include "walloca.h"

#include "common.h"
#include "aui.h"
#include "wpaui.h"
#include "dip.h"
#include "mad.h"
#include "sampinfo.h"
#include "wpasmfil.h"
#include "wpsrcfil.h"
#include "_srcmgt.h"
#include "srcmgt.h"
#include "msg.h"
#include "memutil.h"
#include "setsamps.h"
#include "support.h"


extern massgd_sample_addr *WPGetMassgdSampData(sio_data *curr_sio,clicks_t click_index);

#define MAX_ASM_BUFF_LEN    256

wp_asmline * WPGetAsmLoc( wp_asmfile * wpasm_file, int row,
                                    int * group_loc, int * row_loc )
/******************************************************************/
{
    wp_asmline *    asm_line;
    int             asm_row;
    int             asm_group;

    asm_group = row / MAX_ASM_LINE_INDEX;
    asm_row = row - (asm_group * MAX_ASM_LINE_INDEX);
    if( asm_group != 0 && asm_row == 0 && asm_group > wpasm_file->asm_groups ) {
        wpasm_file->asm_data = ProfRealloc( wpasm_file->asm_data,
                                     sizeof(wp_asm_groups)*(asm_group+1) );
        wpasm_file->asm_data[asm_group].asm_lines
                 = ProfAlloc( MAX_ASM_LINE_SIZE );
        wpasm_file->asm_groups++;
    }
    *group_loc = asm_group;
    *row_loc = asm_row;
    asm_line = &wpasm_file->asm_data[asm_group].asm_lines[asm_row];
    return( asm_line );
}



wp_asmfile *WPAsmOpen( sio_data * curr_sio, int src_row, bool quiet )
/*******************************************************************/
{
    wp_asmfile *        wpasm_file;
    cue_handle *        ch;
    cue_handle *        ch2;
    mod_info *          curr_mod;
    file_info *         curr_file;
    massgd_sample_addr * samp_data;
    wp_asmline *        asm_line;
    mod_handle          mh;
    file_handle         fh;
    address             addr;
    cue_fileid          fid;
    search_result       cue_find;
    int                 rows;
    int                 asm_group;
    int                 asm_row;
    int                 file_index;
    int                 addr_cmp;
    clicks_t            addr_tick_index;

    quiet=quiet;
    ch = alloca( DIPHandleSize( HK_CUE, false ) );
    ch2 = alloca( DIPHandleSize( HK_CUE, false ) );
    curr_file = curr_sio->curr_file;
    curr_mod = curr_sio->curr_mod;
    if( curr_file->fid == 0 || LineCue( curr_mod->mh, curr_sio->curr_file->fid,
                                        src_row, 0, ch2 ) == SR_NONE ) {
        ch2 = NULL;
    }
    fh = ExeOpen( curr_sio->curr_image->name );
    if( fh == -1 ) {
        ErrorMsg( LIT( Exe_Not_Found ), curr_sio->curr_image->name );
        return( NULL );
    }
    wpasm_file = ProfCAlloc( sizeof(wp_asmfile) );
    curr_sio->asm_file = wpasm_file;
    wpasm_file->asm_buff = ProfAlloc( MAX_ASM_BUFF_LEN );
    wpasm_file->asm_buff_len = MAX_ASM_BUFF_LEN;
    SetNumBytes( 0 );
    SetExeFile( fh, false );
    wpasm_file->fh = fh;
    addr = ModAddr( curr_mod->mh );
    SetExeOffset( addr );
    wpasm_file->max_time = 0;
    addr_tick_index = curr_mod->first_tick_index - 1;
    samp_data = WPGetMassgdSampData( curr_sio, addr_tick_index++ );
    wpasm_file->asm_data = ProfAlloc( sizeof(wp_asm_groups) );
    wpasm_file->asm_data[0].asm_lines = ProfAlloc( MAX_ASM_LINE_SIZE );
    wpasm_file->asm_groups = 0;
    rows = 0;
    for( ;; ) {
        mh = curr_mod->mh;
        if( EndOfSegment()
            || AddrMod( addr, &mh ) == SR_NONE || mh != curr_mod->mh ) break;
        cue_find = (AddrCue( curr_mod->mh, addr, ch ) == SR_EXACT);
        if( ch2 != NULL && CueCmp( ch, ch2 ) == 0 ) {
            wpasm_file->entry_line = rows;
            ch2 = NULL;
        }
        asm_line = WPGetAsmLoc( wpasm_file, rows, &asm_group, &asm_row );
        if( cue_find ) {
            asm_line->source_line = true;
            asm_line->u.src.line = CueLine( ch );
            asm_line->u.src.src_file = NULL;
            if( !curr_file->unknown_file ) {
                fid = CueFileId( ch );
                file_index = 0;
                while( file_index < curr_mod->file_count ) {
                    curr_file = curr_mod->mod_file[file_index];
                    if( curr_file->fid == fid ) {
                        asm_line->u.src.src_file =
                                FOpenSource( curr_file->name, mh, fid );
                        break;
                    }
                    file_index++;
                }
            }
            rows++;
            asm_line = WPGetAsmLoc( wpasm_file, rows, &asm_group, &asm_row );
        }
        asm_line = &wpasm_file->asm_data[asm_group].asm_lines[asm_row];
        asm_line->source_line = false;
        asm_line->u.asm_line.addr = addr;
        asm_line->u.asm_line.tick_count = 0;
        for( ;; ) {
            if( samp_data == NULL ) break;
            addr_cmp = AddrCmp( &addr, samp_data->raw );
            if( addr_cmp < 0 ) break;
            if( addr_cmp == 0 ) {
                asm_line->u.asm_line.tick_count = samp_data->hits;
                if( asm_line->u.asm_line.tick_count > wpasm_file->max_time ) {
                    wpasm_file->max_time = asm_line->u.asm_line.tick_count;
                }
            }
            samp_data = WPGetMassgdSampData( curr_sio, addr_tick_index++ );
        }
        rows++;
        CodeAdvance( &addr );
    }
    WPGetAsmLoc( wpasm_file, rows, &asm_group, &asm_row );
    wpasm_file->asm_data[asm_group].asm_lines =
            ProfRealloc( wpasm_file->asm_data[asm_group].asm_lines,
                         sizeof(wp_asmline)*(asm_row+1) );
    wpasm_file->asm_rows = rows;
    return( wpasm_file );
}



void WPAsmClose( wp_asmfile * wpasm_file )
/****************************************/
{
    wp_asmline *        asm_line;
    int                 row;
    int                 asm_group;
    int                 asm_row;

    if( wpasm_file != NULL ) {
        if( wpasm_file->asm_data != NULL ) {
            row = 0;
            while( row < wpasm_file->asm_rows ) {
                asm_line = WPGetAsmLoc( wpasm_file, row, &asm_group, &asm_row );
                if( asm_line->source_line ) {
                    FDoneSource( asm_line->u.src.src_file );
                }
                row++;
            }
            row = wpasm_file->asm_rows;
            asm_group = 0;
            for( ;; ) {
                ProfFree( wpasm_file->asm_data[asm_group++].asm_lines );
                row -= MAX_ASM_LINE_INDEX;
                if( row < 0 ) break;
            }
            ProfFree( wpasm_file->asm_data );
        }
        if( wpasm_file->asm_buff != NULL ) {
            ProfFree( wpasm_file->asm_buff );
        }
        if( wpasm_file->fh != 0 ) {
            ExeClose( wpasm_file->fh );
        }
        ProfFree( wpasm_file );
    }
}



char * WPAsmGetLine( a_window * wnd, int line )
/*********************************************/
{
    sio_data *      curr_sio;
    wp_asmfile *    wpasm_file;
    src_info *      src;
    wp_asmline *    asm_line;
    int             asm_group;
    int             asm_row;
    int             buff_len;

    curr_sio = WndExtra( wnd );
    wpasm_file = curr_sio->asm_file;
    if( line >= wpasm_file->asm_rows ) {
        return( NULL );
    }
    asm_line = WPGetAsmLoc( wpasm_file, line, &asm_group, &asm_row );
    if( asm_line->source_line ) {
        src = &asm_line->u.src;
        if( src->src_file == NULL ) {
            strcpy( wpasm_file->asm_buff, LIT( Unable_To_Open_Src ) );
        } else {
            for( ;; ) {
                buff_len = FReadLine( src->src_file, src->line, 0,
                                      wpasm_file->asm_buff,
                                      wpasm_file->asm_buff_len );
                if( buff_len != wpasm_file->asm_buff_len ) break;
                wpasm_file->asm_buff_len += 120;
                wpasm_file->asm_buff = ProfRealloc( wpasm_file->asm_buff,
                                                    wpasm_file->asm_buff_len );
            }
            if( buff_len < 0 ) {
                buff_len = 0;
            }
            wpasm_file->asm_buff[buff_len] = NULLCHAR;
        }
    } else {
        SetNumBytes( 0 );
        SetExeFile( wpasm_file->fh, false );
        SetExeImage( curr_sio->curr_image );
        GetFullInstruct( asm_line->u.asm_line.addr, wpasm_file->asm_buff, wpasm_file->asm_buff_len-1 );
    }
    return( wpasm_file->asm_buff );
}



int WPAsmFindSrcLine( sio_data * curr_sio, int line )
/***************************************************/
{
    wp_asmfile *    wpasm_file;
    wp_asmline *    asm_line;
    int             asm_group;
    int             asm_row;

    wpasm_file = curr_sio->asm_file;
    while( line >= 0 ) {
        asm_line = WPGetAsmLoc( wpasm_file, line, &asm_group, &asm_row );
        if( asm_line->source_line ) {
            return( asm_line->u.src.line );
        }
        line--;
    }
    return( 0 );
}
