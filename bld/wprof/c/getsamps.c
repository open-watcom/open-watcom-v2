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
* Description:  Read sample data from a file.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include "wio.h"
#include "common.h"
#include "dip.h"
#include "aui.h"
#include "machtype.h"
#include "msg.h"
#include "sampinfo.h"
#include "memutil.h"
#include "clibext.h"


extern void ClearSample(sio_data *curr_sio);
extern unsigned int BigRead(int ,void *,unsigned int );
extern void ErrorMsg(char *msg,... );
extern void SetSampleInfo(sio_data *curr_sio);
extern bool LoadImageOverlays(void );
extern void SetCurrentMAD( mad_handle );

extern char             SamplePath[];
extern system_config    DefSysConfig;

sio_data                *SIOData;
sio_data                *CurrSIOData;



STATIC bool initCurrSIO( void )
/*****************************/
{
    image_info      *new_image;
    file_handle     fh;
    int             name_len;

    fh = open( SamplePath, O_RDONLY | O_BINARY, S_IREAD );
    if( fh == (file_handle) -1 ) {
        ErrorMsg( LIT( Cannot_Open_Smp_File ), SamplePath );
        return( false );
    }
    CurrSIOData = ProfCAlloc( sizeof(sio_data) );
    CurrSIOData->fh = fh;
    name_len = strlen( SamplePath ) + 1;
    CurrSIOData->samp_file_name = ProfAlloc( name_len );
    memcpy( CurrSIOData->samp_file_name, SamplePath, name_len );
    CurrSIOData->images = ProfAlloc( 2*sizeof(pointer) );
    new_image = ProfCAlloc( sizeof(image_info) );
    name_len = strlen( LIT( Unknown_Image ) ) + 1;
    new_image->name = ProfAlloc( name_len );
    memcpy( new_image->name, LIT( Unknown_Image ), name_len );
    new_image->unknown_image = true;
    CurrSIOData->images[0] = new_image;
    CurrSIOData->curr_image = new_image;
    new_image = ProfCAlloc( sizeof(image_info) );
    name_len = strlen( LIT( Gathered_Images ) ) + 1;
    new_image->name = ProfAlloc( name_len );
    memcpy( new_image->name, LIT( Gathered_Images ), name_len );
    new_image->ignore_gather = true;
    new_image->gather_image = true;
    CurrSIOData->images[1] = new_image;
    CurrSIOData->image_count = 2;
    CurrSIOData->curr_display_row = -WND_MAX_ROW;
    return( true );
}



STATIC bool verifyHeader( void )
/******************************/
{
    file_handle     fh;
    off_t           header_position;
    off_t           tmp_position;

    fh = CurrSIOData->fh;
    header_position = lseek( fh, - ( (off_t)SIZE_HEADER ), SEEK_END );
    if( header_position == (off_t)-1 ) {
        ErrorMsg( LIT( Smp_File_IO_Err ), CurrSIOData->samp_file_name );
        return( false );
    }
    if( read( fh, &CurrSIOData->header, SIZE_HEADER ) != SIZE_HEADER ) {
        ErrorMsg( LIT( Smp_File_IO_Err ), CurrSIOData->samp_file_name );
        return( false );
    }
    if( CurrSIOData->header.signature != SAMP_SIGNATURE ) {
        ErrorMsg( LIT( Invalid_Smp_File ), CurrSIOData->samp_file_name );
        return( false );
    }
    if( CurrSIOData->header.major_ver != SAMP_MAJOR_VER
     || CurrSIOData->header.minor_ver > SAMP_MINOR_VER ) {
        ErrorMsg( LIT( Incompat_Smp_File ), CurrSIOData->samp_file_name );
        return( false );
    }
    tmp_position = lseek( fh, CurrSIOData->header.sample_start, SEEK_SET );
    if( tmp_position == (off_t) -1 ) {
        ErrorMsg( LIT( Smp_File_IO_Err ), CurrSIOData->samp_file_name );
        return( false );
    }
    return( true );
}



STATIC void procInfoBlock( clicks_t ticks, samp_data *data )
/**********************************************************/
{
    /* is this for backward compatability */
//    if( pref->length >= SIZE_INFO + SIZE_PREFIX &&
//                               data->info.count[ SAMP_CALLGRAPH ].number ) {
//        CallGraph = true;     /* sample file includes callgraph records */
//    } else {
//        CallGraph = false;    /* sample file doesn't have callgraph info */
//    }
    if( ticks == 0 ) {
        ticks = 1;
    }
    if( CurrSIOData->header.major_ver == 2 && CurrSIOData->header.minor_ver < 2 ) {
        /* pre-MAD sample file */
        CurrSIOData->config = DefSysConfig;
    } else {
        CurrSIOData->config = data->info.config;
        /* sanity check for non-MADified sampler */
        if( CurrSIOData->config.mad == MAD_NIL ) {
            CurrSIOData->config = DefSysConfig;
        }
    }
    SetCurrentMAD( CurrSIOData->config.mad );

    CurrSIOData->timer_rate = data->info.timer_rate;
}



STATIC void procMarkBlock( clicks_t tick, samp_data *data )
/*********************************************************/
{
    mark_data       *new_mark;
    int             name_len;

    name_len = strlen( data->mark.mark_string );
    new_mark = ProfCAlloc( sizeof(mark_data)+name_len );
    new_mark->tick = tick;
    new_mark->thread = data->mark.thread_id;
    /* make sure to handle overlay resolution */
    new_mark->addr.mach.segment = data->mark.addr.segment;
    new_mark->addr.mach.offset = data->mark.addr.offset;
    memcpy( new_mark->name, data->mark.mark_string, name_len );
    if( CurrSIOData->marks == NULL ) {
        new_mark->next = new_mark;
    } else {
        new_mark->next = CurrSIOData->marks->next;
        CurrSIOData->marks->next = new_mark;
    }
    CurrSIOData->marks = new_mark;
}



STATIC void procOverlayBlock( clicks_t tick, samp_data * data )
/*************************************************************/
{
    overlay_data *  new_ovl;

    new_ovl = ProfCAlloc( sizeof(overlay_data) );
    new_ovl->tick = tick;
    new_ovl->section = data->ovl.req_section;
    if( new_ovl->section & OVL_RETURN ) {
        new_ovl->overlay_return = true;
        new_ovl->section &= ~OVL_RETURN;
    } else {
        new_ovl->overlay_return = false;
    }
    /* make sure to handle overlay resolution */
    new_ovl->req_addr.mach.segment = data->ovl.addr.segment;
    new_ovl->req_addr.mach.offset = data->ovl.addr.offset;
    if( CurrSIOData->ovl_loads == NULL ) {
        new_ovl->next = new_ovl;
    } else {
        new_ovl->next = CurrSIOData->ovl_loads->next;
        CurrSIOData->ovl_loads->next = new_ovl;
    }
    CurrSIOData->ovl_loads = new_ovl;
}



STATIC void procImageBlock( samp_data *data, bool main_exe )
/**********************************************************/
{
    image_info      *new_image;
    int             name_len;
    int             image_index;

    name_len = strlen( data->code.name ) + 1;
    new_image = ProfCAlloc( sizeof(image_info) );
    image_index = CurrSIOData->image_count;
    CurrSIOData->image_count++;
    CurrSIOData->images = ProfRealloc( CurrSIOData->images,
                                     CurrSIOData->image_count*sizeof(pointer));
    CurrSIOData->images[image_index] = new_image;
    CurrSIOData->curr_image = new_image;
    new_image->name = ProfAlloc( name_len );
    memcpy( new_image->name, data->code.name, name_len );
    new_image->overlay_table.mach.segment = data->code.ovl_tab.segment;
    new_image->overlay_table.mach.offset = data->code.ovl_tab.offset;
    new_image->time_stamp = data->code.time_stamp;
    new_image->main_load = main_exe;
}



STATIC void procAddrBlock( uint_16 total_len, samp_data * data )
/**************************************************************/
{
    image_info *    curr_image;
    int             map_count;
    int             new_count;
    int             i;
    int             j;
    map_to_actual   *map;

    total_len -= offsetof( samp_block, d.map );
    new_count = total_len / sizeof( mapping );
    curr_image = CurrSIOData->curr_image;
    map_count = curr_image->map_count + new_count;
    map = curr_image->map_data;
    if( map == NULL ) {
        map = ProfAlloc( map_count * sizeof(*map) );
    } else {
        map = ProfRealloc( map, map_count * sizeof(*map) );
    }
    for( i = 0, j = curr_image->map_count; i < new_count; ++i, ++j ) {
        map[j].map.mach.segment    = data->map.data[i].map.segment;
        map[j].map.mach.offset     = data->map.data[i].map.offset;
        map[j].actual.mach.segment = data->map.data[i].actual.segment;
        map[j].actual.mach.offset  = data->map.data[i].actual.offset;
        map[j].length              = 0xffff; //NYI: get from executable....
    }
    curr_image->map_count = map_count;
    curr_image->map_data = map;
}



STATIC void procRemapBlock( clicks_t tick, uint_16 total_len,
                                             samp_data *data )
/************************************************************/
{
    remap_data      *new_remap;
    int             count;
    int             index;

    index = 0;
    total_len -= offsetof( samp_block, d.remap );
    count = total_len / sizeof( remapping );
    while( count-- > 0 ) {
        new_remap = ProfCAlloc( sizeof(remap_data) );
        if( CurrSIOData->remaps == NULL ) {
            new_remap->next = new_remap;
        } else {
            new_remap->next = CurrSIOData->remaps->next;
            CurrSIOData->remaps->next = new_remap;
        }
        CurrSIOData->remaps = new_remap;
        new_remap->tick = tick;
        new_remap->section = data->remap.data[index].section;
        new_remap->segment = data->remap.data[index].segment;
        index++;
    }
}



STATIC bool procSampleBlock( clicks_t tick, uint_16 total_len,
                                              samp_data *data )
/*************************************************************/
{
    thread_id   thread;
    unsigned    data_index;
    unsigned    index;
    unsigned    index2;
    unsigned    count;
    unsigned    buckets;
    clicks_t    end_tick;
    ldiv_t      div_result;
    thread_data *thd;
    thread_data **owner;
    address     *samp;

    thread = data->sample.thread_id;
    total_len -= offsetof( samp_block, d.sample );
    count = total_len / sizeof( samp_address );
    CurrSIOData->total_samples += count;
    end_tick = tick + count;
    owner = &CurrSIOData->samples;
    for( ;; ) {
        thd = *owner;
        if( thd == NULL ) {
            thd = ProfAlloc( sizeof( *thd ) );
            *owner = thd;
            thd->next = NULL;
            thd->thread = thread;
            thd->start_time = tick;
            thd->end_time = end_tick;
            buckets = RAW_BUCKET_IDX( count ) + 1;
            thd->raw_bucket = ProfAlloc( buckets * sizeof( *thd->raw_bucket ) );
            for( index = 0; index < buckets; ++index ) {
                thd->raw_bucket[index] = ProfCAlloc( MAX_RAW_BUCKET_SIZE );
            }
            break;

        }
        if( thd->thread == thread )
            break;
        owner = &thd->next;
    }
    if( end_tick > thd->end_time ) {
        index = RAW_BUCKET_IDX( thd->end_time - thd->start_time ) + 1;
        buckets = RAW_BUCKET_IDX( end_tick - thd->start_time ) + 1;
        if( buckets > index ) {
            thd->raw_bucket = ProfRealloc( thd->raw_bucket, buckets * sizeof( *thd->raw_bucket ) );
            for( ; index < buckets; ++index ) {
                thd->raw_bucket[index] = ProfCAlloc( MAX_RAW_BUCKET_SIZE );
            }
        }
        thd->end_time = end_tick;
    }


    div_result = ldiv( tick - thd->start_time, MAX_RAW_BUCKET_INDEX );
    index = div_result.quot;
    index2 = div_result.rem;
    data_index = 0;
    while( count > 0 ) {
        samp = &thd->raw_bucket[index][index2];
        samp->mach.segment = data->sample.sample[data_index].segment;
        samp->mach.offset  = data->sample.sample[data_index].offset;
        if( ++index2 >= MAX_RAW_BUCKET_INDEX ) {
            index2 = 0;
            ++index;
        }
        ++data_index;
        --count;
    }
    return( true );
}



STATIC bool readSampleFile( void )
/********************************/
{
    file_handle             fh;
    uint_16                 size;
    void                    *buff;
    int                     buff_len;
    off_t                   start_position;
    bool                    main_exe;
    samp_block_prefix       prefix;
    samp_block_prefix       *next_prefix;

/* we can add error checking for things like */
/**/
/*    - number of samples match the info # of samples */
/*    - main exe load if there is an overlay table */
/**/
    fh = CurrSIOData->fh;
    start_position = lseek( fh, CurrSIOData->header.sample_start, SEEK_SET );
    if( start_position == (off_t) -1 ) {
        ErrorMsg( LIT( Smp_File_IO_Err ), CurrSIOData->samp_file_name );
        return( false );
    }
    if( read( fh, &prefix, SIZE_PREFIX ) != SIZE_PREFIX ) {
        ErrorMsg( LIT( Smp_File_IO_Err ), CurrSIOData->samp_file_name );
        return( false );
    }
    buff = ProfAlloc( SIZE_DATA );
    buff_len = SIZE_DATA;
    main_exe = false;
    while( prefix.kind != SAMP_LAST ) {
        size = prefix.length;
        if( buff_len < size ) {
            buff = ProfRealloc( buff, size );
            buff_len = size;
        }
        /* reads data & next prefix */
        if( BigRead( fh, buff, size ) != size ) {
            ErrorMsg( LIT( Smp_File_IO_Err ), CurrSIOData->samp_file_name );
            ProfFree( buff );
            return( false );
        }
        next_prefix = (void *)( ((char *) buff) + ( size - SIZE_PREFIX ));

        /* if we're reading a sample record from a callgraph sample */
        /* file, the next record should contain callgraph information */
        /* which we will also want stored in memory for processing */
        /* 16-jul-92 CMS */

//        if( CallGraph && prefix.kind == SAMP_SAMPLES &&
//                         next_prefix->kind == SAMP_CALLGRAPH ) {
//            size = next_prefix->length;
//            /* reads callgraph data & next prefix   */
//            if( BigRead( fh, next_prefix, size ) != size ) {
//                errorIO();
//                ProfFree( buff );
//                ErrorMsg( LIT( Smp_File_IO_Err ), CurrSIOData->samp_file_name );
//                return( false );
//            }
//            next_prefix = (void *)( ((char *) next_prefix) + ( size - SIZE_PREFIX ));
//        }

        switch( prefix.kind ) {
        case SAMP_INFO:
            procInfoBlock( prefix.tick, buff );
            break;
        case SAMP_SAMPLES:
            if( !procSampleBlock( prefix.tick, prefix.length, buff ) ) {
                return( false );
            }
            break;
        case SAMP_MARK:
            procMarkBlock( prefix.tick, buff );
            break;
        case SAMP_OVL_LOAD:
            procOverlayBlock( prefix.tick, buff );
            break;
        case SAMP_ADDR_MAP:
            procAddrBlock( prefix.length, buff );
            break;
        case SAMP_MAIN_LOAD:
            main_exe = true;
            /* fall through */
        case SAMP_CODE_LOAD:
            procImageBlock( buff, main_exe );
            main_exe = false;
            break;
        case SAMP_REMAP_SECTION:
            procRemapBlock( prefix.tick, prefix.length, buff );
            break;
        case SAMP_CALLGRAPH:
//            printf( "sample callgraph\n" );
            break;
        }
        prefix = *next_prefix;
    }
    ProfFree( buff );
    return( true );
}



bool GetSampleInfo( void )
/************************/
{
    if( !initCurrSIO() ) {
        return( false );
    }
    if( !verifyHeader() ) {
        ClearSample( CurrSIOData );
        return( false );
    }
    if( !readSampleFile() ) {
        ClearSample( CurrSIOData );
        return( false );
    }
    /* there must be at least one address map for the first module, or */
    /* we cannot resolve overlays and do mapping.  Should generate an error*/
    if( !LoadImageOverlays() ) {
        ClearSample( CurrSIOData );
        return( false );
    }
    close( CurrSIOData->fh );
    /* do the SIOData sets near the end to make it easier to de-link */
    /* the new data if we have an error */
    if( SIOData == NULL ) {
        CurrSIOData->next = CurrSIOData;
    } else {
        CurrSIOData->next = SIOData->next;
        SIOData->next = CurrSIOData;
    }
    SIOData = CurrSIOData;
    SetSampleInfo( CurrSIOData );
    SamplePath[0] = 0;
    return( true );
}
