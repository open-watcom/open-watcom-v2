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
* Description:  Profiler source file management.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "walloca.h"

#include "common.h"
#include "aui.h"
#include "wpaui.h"
#include "dip.h"
#include "sampinfo.h"
#include "wpsrcfil.h"
#include "_srcmgt.h"
#include "srcmgt.h"
#include "msg.h"
#include "memutil.h"


STATIC void setSrcLineData( wp_srcfile *, sio_data *, mod_info *,
                            file_info *, rtn_info *  );

extern char * WPSourceGetLine( a_window * wnd, int line )
/*******************************************************/
{
    sio_data *      curr_sio;
    wp_srcfile *    wp_src;
    int             buff_len;

    curr_sio = WndExtra( wnd );
    wp_src = curr_sio->src_file;
    if( wp_src->src_buff_len == 0 ) {
        wp_src->src_buff = ProfAlloc( 100 );
        wp_src->src_buff_len = 100;
    }
    for( ;; ) {
        buff_len = FReadLine( wp_src->src_file, line, 0, wp_src->src_buff,
                              wp_src->src_buff_len );
        if( buff_len != wp_src->src_buff_len ) break;
        wp_src->src_buff_len += 120;
        wp_src->src_buff = ProfRealloc( wp_src->src_buff,
                                        wp_src->src_buff_len );
    }
    if( buff_len < 0 ) {
        wp_src->src_eof = true;
        return( NULL );
    }
    wp_src->src_eof = false;
    wp_src->src_buff[buff_len] = NULLCHAR;
    return( wp_src->src_buff );
}



extern wp_srcfile * WPSourceOpen( sio_data * curr_sio, bool quiet )
/*****************************************************************/
{
    file_info *         curr_file;
    rtn_info *          curr_rtn;
    wp_srcfile *        wpsrc_file;
    void *              src_file;
    mod_info *          curr_mod;
    cue_handle *        ch;
    location_list       ll;
    int                 line;

    curr_mod = curr_sio->curr_mod;
    curr_file = curr_sio->curr_file;
    curr_rtn = curr_sio->curr_rtn;
    if( curr_file->unknown_file ) {
        src_file = NULL;
    } else {
        src_file = FOpenSource( curr_file->name, curr_mod->mh, curr_file->fid );
    }
    if( src_file == NULL ) {
        curr_sio->src_file = NULL;
        if( !quiet ) {
            if( curr_file->unknown_file ) {
                ErrorMsg( LIT( Src_File_Not_Known ) );
            } else {
                ErrorMsg( LIT( Src_File_Not_Found ), curr_file->name );
            }
        }
        return( NULL );
    }
    wpsrc_file = ProfCAlloc( sizeof( wp_srcfile ) );
    wpsrc_file->src_file = src_file;
    curr_sio->src_file = wpsrc_file;
    if( DIPSymLocation( curr_rtn->sh, NULL, &ll ) == DS_OK ) {
        ch = alloca( DIPHandleSize( HK_CUE, false ) );
        DIPAddrCue( curr_mod->mh, ll.e[0].u.addr, ch );
        wpsrc_file->rtn_line = DIPCueLine( ch );
    }
    setSrcLineData( wpsrc_file, curr_sio, curr_mod, curr_file, curr_rtn );
    line = 1;
    for( ;; ) {
        WPSourceGetLine( curr_sio->sample_window, line );
        if( wpsrc_file->src_eof )
            break;
        line++;
    }
    wpsrc_file->src_rows = line - 1;
    return( wpsrc_file );
}



extern void WPSourceClose( wp_srcfile * wpsrc_file )
/**************************************************/
{
    if( wpsrc_file != NULL ) {
        FDoneSource( wpsrc_file->src_file );
        if( wpsrc_file->src_buff != NULL ) {
            ProfFree( wpsrc_file->src_buff );
        }
        ProfFree( wpsrc_file );
    }
}



extern massgd_sample_addr * WPGetMassgdSampData( sio_data * curr_sio,
                                                 clicks_t click_index )
/*********************************************************************/
{
    ldiv_t              ans;

    if( click_index >= curr_sio->number_massaged ) return( NULL );
    ans = ldiv( click_index, MAX_MASSGD_BUCKET_INDEX );
    return( &curr_sio->massaged_sample[ans.quot][ans.rem] );
}



STATIC void setSrcLineData( wp_srcfile * wpsrc_file, sio_data * curr_sio,
                            mod_info * curr_mod, file_info * curr_file,
                                                        rtn_info * curr_rtn )
/***************************************************************************/
{
    massgd_sample_addr *    samp_data;
    wp_srcline *            lines;
    rtn_info *              rtn_rover;
    cue_handle *            ch;
    clicks_t                click_index;
    unsigned long           last_srcline;
    unsigned long           new_line;
    int                     line_count;
    int                     line_index;
    int                     count;
    int                     count2;

    ch = alloca( DIPHandleSize( HK_CUE, false ) );
    lines = NULL;
    line_index = -1;
    last_srcline = 0;
    new_line = 0;
    line_count = 0;
    wpsrc_file->max_time = 0;
    count = 0;
    while( count < curr_file->rtn_count ) {
        rtn_rover = curr_file->routine[count];
        if( rtn_rover->tick_count == 0 ) {
            count2 = 0;
        } else {
            click_index = rtn_rover->first_tick_index - 1;
            count2 = rtn_rover->last_tick_index - click_index;
            line_count += count2;
            lines = ProfRealloc( lines, sizeof(wp_srcline)*line_count );
        }
        while( count2-- > 0 ) {
            samp_data = WPGetMassgdSampData( curr_sio, click_index );
            if( DIPAddrCue( curr_mod->mh, *samp_data->raw, ch ) != SR_NONE ) {
                if( DIPCueFileId( ch ) == curr_file->fid ) {
                    new_line = DIPCueLine( ch );
                }
            }
            if( last_srcline != new_line || line_index == -1 ) {
                line_index++;
                lines[line_index].line = new_line;
                lines[line_index].tick_count = 0;
                last_srcline = new_line;
                if( line_index == 0 && curr_rtn == rtn_rover ) {
                    wpsrc_file->samp_line = new_line;
                }
            }
            lines[line_index].tick_count += samp_data->hits;
            if( lines[line_index].tick_count > wpsrc_file->max_time ) {
                wpsrc_file->max_time = lines[line_index].tick_count;
            }
            click_index++;
        }
        line_count = line_index + 1;
        count++;
    }
    wpsrc_file->wp_line_count = line_count;
    if( line_count != 0 ) {
        wpsrc_file->src_lines
                = ProfRealloc( lines, sizeof(wp_srcline)*line_count );
    }
}
