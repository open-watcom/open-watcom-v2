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
#include <unistd.h>

#include "common.h"
#include "aui.h"
#include "dip.h"
#include "sampinfo.h"
#include "wpsrcfil.h"
#include "wpasmfil.h"

//#include "memutil.def"
//#include "wpsrcfil.def"
//#include "wpasmfil.def"
//#include "dipinter.def"
extern void WPSourceClose(wp_srcfile *wpsrc_file);
extern void WPDipDestroyProc(process_info *dip_proc);
extern void WPAsmClose(wp_asmfile *wpasm_file);
extern void ProfFree(void *ptr);

extern sio_data *       SIOData;



extern void ClearAllSamples()
/***************************/
{
    while( SIOData != NULL ) {
        ClearSample( SIOData->next );
    }
}



extern void ClearSample( sio_data * curr_sio )
/********************************************/
{
    sio_data *          prev_sio;
    mark_data *         marks;
    mark_data *         next_mark;
    overlay_data *      ovl;
    overlay_data *      next_ovl;
    image_info *        image;
    remap_data *        remap;
    remap_data *        next_remap;
    void *              cursor_type;
    unsigned            index;
    unsigned            buckets;
    thread_data         *thd;
    thread_data         *next;

    cursor_type = WndHourGlass( NULL );
    if( curr_sio->fh != NULL ) {
        close( curr_sio->fh );
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
    index = 0;
    while( index < curr_sio->image_count ) {
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
        index++;
    }
    ProfFree( curr_sio->images );
    ovl = curr_sio->ovl_loads;
    while( ovl != NULL ) {
        next_ovl = ovl->next;
        ProfFree( ovl );
        ovl = next_ovl;
        if( ovl == curr_sio->ovl_loads ) break;
    }
    remap = curr_sio->remaps;
    while( remap != NULL ) {
        next_remap = remap->next;
        ProfFree( remap );
        remap = next_remap;
        if( remap == curr_sio->remaps ) break;
    }
    marks = curr_sio->marks;
    while( marks != NULL ) {
        next_mark = marks->next;
        ProfFree( marks );
        marks = next_mark;
        if( marks == curr_sio->marks ) break;
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
        for(;;) {
            if( prev_sio->next == curr_sio ) break;
            prev_sio = prev_sio->next;
        }
        prev_sio->next = curr_sio->next;
        if( curr_sio == SIOData ) {
            SIOData = prev_sio;
        }
    }
    ProfFree( curr_sio );
    WndHourGlass( cursor_type );
}



extern void ClearMassaged( sio_data * curr_sio )
/**********************************************/
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
    curr_sio->massaged_mapped = B_FALSE;
}



extern void ClearModuleInfo( image_info * curr_image )
/****************************************************/
{
    int             count;

    if( curr_image->module == NULL ) {
        return;
    }
    count = 0;
    while( count < curr_image->mod_count ) {
        if( curr_image->module[count] != NULL ) {
            ClearFileInfo( curr_image->module[count] );
            ProfFree( curr_image->module[count] );
        }
        count++;
    }
    ProfFree( curr_image->module );
    curr_image->module = NULL;
    curr_image->mod_count = 0;
}



extern void ClearFileInfo( mod_info * curr_mod )
/**********************************************/
{
    int             count;

    if( curr_mod->mod_file == NULL ) {
        return;
    }
    count = 0;
    while( count < curr_mod->file_count ) {
        if( curr_mod->mod_file[count] != NULL ) {
            ClearRoutineInfo( curr_mod->mod_file[count] );
            ProfFree( curr_mod->mod_file[count] );
        }
        count++;
    }
    ProfFree( curr_mod->mod_file );
    curr_mod->mod_file = NULL;
    curr_mod->file_count = 0;
}



extern void ClearRoutineInfo( file_info * curr_file )
/***************************************************/
{
    rtn_info *      curr_rtn;
    int             count;

    if( curr_file->routine == NULL ) {
        return;
    }
    count = 0;
    while( count < curr_file->rtn_count ) {
        curr_rtn = curr_file->routine[count];
        if( curr_rtn != NULL ) {
            if( curr_rtn->sh != NULL ) {
                ProfFree( curr_rtn->sh );
            }
            ProfFree( curr_rtn );
        }
        count++;
    }
    ProfFree( curr_file->routine );
    curr_file->routine = NULL;
    curr_file->rtn_count = 0;
}
