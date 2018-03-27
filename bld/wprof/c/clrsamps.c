/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Clear sample data.
*
****************************************************************************/


#include <stdio.h>
#include "wio.h"
#include "common.h"
#include "aui.h"
#include "dip.h"
#include "sampinfo.h"
#include "wpsrcfld.h"
#include "wpasmfil.h"
#include "memutil.h"
#include "dipinter.h"
#include "clrsamps.h"
#include "wpsrcfil.h"
#include "wpdata.h"


void ClearMassaged( sio_data * curr_sio )
/***************************************/
{
    unsigned            index;
    unsigned            buckets;


    if( curr_sio->massaged_sample != NULL ) {
        buckets = MSG_BUCKET_IDX( curr_sio->number_massaged ) + 1;
        for( index = 0; index < buckets; ++index ) {
            ProfFree( curr_sio->massaged_sample[index] );
        }
        ProfFree( curr_sio->massaged_sample );
        curr_sio->massaged_sample = NULL;
    }
    curr_sio->number_massaged = 0;
    curr_sio->massaged_mapped = false;
}



void ClearRoutineInfo( file_info * curr_file )
/********************************************/
{
    rtn_info *      curr_rtn;
    int             count;

    if( curr_file->routine == NULL ) {
        return;
    }
    for( count = 0; count < curr_file->rtn_count; ++count ) {
        curr_rtn = curr_file->routine[count];
        if( curr_rtn != NULL ) {
            if( curr_rtn->sh != NULL ) {
                ProfFree( curr_rtn->sh );
            }
            ProfFree( curr_rtn );
        }
    }
    ProfFree( curr_file->routine );
    curr_file->routine = NULL;
    curr_file->rtn_count = 0;
}



void ClearFileInfo( mod_info * curr_mod )
/***************************************/
{
    int             count;

    if( curr_mod->mod_file == NULL ) {
        return;
    }
    for( count = 0; count < curr_mod->file_count; ++count ) {
        if( curr_mod->mod_file[count] != NULL ) {
            ClearRoutineInfo( curr_mod->mod_file[count] );
            ProfFree( curr_mod->mod_file[count] );
        }
    }
    ProfFree( curr_mod->mod_file );
    curr_mod->mod_file = NULL;
    curr_mod->file_count = 0;
}



void ClearModuleInfo( image_info * curr_image )
/*********************************************/
{
    int             count;

    if( curr_image->module == NULL ) {
        return;
    }
    for( count = 0; count < curr_image->mod_count; ++count ) {
        if( curr_image->module[count] != NULL ) {
            ClearFileInfo( curr_image->module[count] );
            ProfFree( curr_image->module[count] );
        }
    }
    ProfFree( curr_image->module );
    curr_image->module = NULL;
    curr_image->mod_count = 0;
}



void ClearSample( sio_data * curr_sio )
/*************************************/
{
    sio_data *          prev_sio;
    mark_data *         marks;
    mark_data *         next_mark;
    overlay_data *      ovl;
    overlay_data *      next_ovl;
    image_info *        image;
    remap_data *        remap;
    remap_data *        next_remap;
    gui_mcursor_handle  old_cursor;
    unsigned            index;
    unsigned            buckets;
    thread_data         *thd;
    thread_data         *next;

    old_cursor = WndHourGlass( NULL );
    if( curr_sio->fp != NULL ) {
        fclose( curr_sio->fp );
    }
    if( curr_sio->samp_file_name != NULL ) {
        ProfFree( curr_sio->samp_file_name );
    }
    if( curr_sio->sample_window != NULL ) {
        WndExtra( curr_sio->sample_window ) = NULL;
    }
    if( curr_sio->src_file != NULL ) {
        WPSourceClose( curr_sio->src_file );
    }
    if( curr_sio->asm_file != NULL ) {
        WPAsmClose( curr_sio->asm_file );
    }
    for( index = 0; index < curr_sio->image_count; ++index ) {
        image = curr_sio->images[index];
        if( image->map_data != NULL ) {
            ProfFree( image->map_data );
        }
        if( image->ovl_data != NULL ) {
            if( image->ovl_data->fname != NULL ) {
                ProfFree( image->ovl_data->fname );
            }
            ProfFree( image->ovl_data );
        }
        if( image->sym_name != NULL ) {
            ProfFree( image->sym_name );
        }
        if( image->name != NULL ) {
            ProfFree( image->name );
        }
        ClearModuleInfo( image );
        ProfFree( image );
    }
    ProfFree( curr_sio->images );
    for( ovl = curr_sio->ovl_loads; ovl != NULL; ovl = next_ovl ) {
        next_ovl = ovl->next;
        ProfFree( ovl );
        if( next_ovl == curr_sio->ovl_loads ) {
            break;
        }
    }
    for( remap = curr_sio->remaps; remap != NULL; remap = next_remap ) {
        next_remap = remap->next;
        ProfFree( remap );
        if( next_remap == curr_sio->remaps ) {
            break;
        }
    }
    for( marks = curr_sio->marks; marks != NULL; marks = next_mark ) {
        next_mark = marks->next;
        ProfFree( marks );
        if( next_mark == curr_sio->marks ) {
            break;
        }
    }
    for( thd = curr_sio->samples; thd != NULL; thd = next ) {
        next = thd->next;
        buckets = RAW_BUCKET_IDX( thd->end_time - thd->start_time ) + 1;
        for( index = 0; index < buckets; ++index ) {
            ProfFree( thd->raw_bucket[index] );
        }
        ProfFree( thd->raw_bucket );
        ProfFree( thd );
    }
    ClearMassaged( curr_sio );
    WPDipDestroyProc( curr_sio->dip_process );
    if( curr_sio->next == curr_sio ) {
        SIOData = NULL;
    } else if( curr_sio->next != NULL ) {
        prev_sio = SIOData;
        for( ;; ) {
            if( prev_sio->next == curr_sio )
                break;
            prev_sio = prev_sio->next;
        }
        prev_sio->next = curr_sio->next;
        if( curr_sio == SIOData ) {
            SIOData = prev_sio;
        }
    }
    ProfFree( curr_sio );
    WndHourGlass( old_cursor );
}



void ClearAllSamples( void )
/**************************/
{
    while( SIOData != NULL ) {
        ClearSample( SIOData->next );
    }
}
