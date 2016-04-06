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
* Description:  Dump formatted profiler data to a file.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "wio.h"
#include "common.h"
#include "dip.h"
#include "machtype.h"
#include "msg.h"
#include "sampinfo.h"
#include "support.h"


extern sio_data         *SIOData;

STATIC FILE             *df;

STATIC void dumpSampleImages( bool all_info, sio_data * curr_sio )
/****************************************************************/
{
    location_list   ll;
    image_info      *curr_image;
    mod_info        *curr_mod;
    file_info       *curr_file;
    rtn_info        *curr_rtn;
    int             image_index;
    int             mod_count;
    int             file_count;
    int             rtn_count;
    clicks_t        ticks_left;
    char            buff[80];

    ticks_left = curr_sio->total_samples;
    image_index = 0;
    while( image_index < curr_sio->image_count ) {
        curr_image = curr_sio->images[image_index];
        fprintf( df, "image '%s'\n", curr_image->name );
        if( curr_image->dip_handle == NO_MOD ) {
            fprintf( df, "  has no dip information\n" );
        } else {
            fprintf( df, "  is using the '%s' DIP\n", ImageDIP( curr_image->dip_handle ) );
        }
        if( curr_image->agg_count > 0 ) {
            fprintf( df, "  tick count = %lu\n", curr_image->agg_count );
        }
        mod_count = 0;
        while( mod_count < curr_image->mod_count ) {
            curr_mod = curr_image->module[mod_count];
            if( (all_info && curr_mod->mh != 0)
             || curr_mod->agg_count != 0) {
                fprintf( df, "  module name '%s'   (%s language)\n", curr_mod->name,
                        ModSrcLang( curr_mod->mh ) );
                if( curr_mod->agg_count > 0 ) {
                    fprintf( df, "    tick count = %lu\n", curr_mod->agg_count );
                }
            }
            file_count = 0;
            while( file_count < curr_mod->file_count ) {
                curr_file = curr_mod->mod_file[file_count];
                if( (all_info && curr_file->fid != 0)
                 || curr_file->agg_count != 0) {
                    fprintf( df, "    source file '%s'\n", curr_file->name );
                    fprintf( df, "      tick count = %lu\n", curr_file->agg_count );
                }
                rtn_count = 0;
                while( rtn_count < curr_file->rtn_count ) {
                    curr_rtn = curr_file->routine[rtn_count];
                    if( (all_info && curr_rtn->sh != NULL)
                     || curr_rtn->tick_count != 0 ) {
                        fprintf( df, "      symbol '%s'\n", curr_rtn->name );
                        if( curr_rtn->sh != NULL
                         && SymLocation( curr_rtn->sh, NULL, &ll ) == DS_OK ) {
                            if( ll.e[0].type == LT_ADDR ) {
                                FormatAddr( ll.e[0].u.addr, buff, sizeof( buff ) );
                                fprintf( df, "          address = [%d]%s\n",
                                        ll.e[0].u.addr.sect_id, buff );
                            }
                        }
                        if( curr_rtn->tick_count > 0 ) {
                            fprintf( df, "          tick count = %lu\n",
                                    curr_rtn->tick_count );
                            ticks_left -= curr_rtn->tick_count;
                        }
                    }
                    rtn_count++;
                }
                file_count++;
            }
            mod_count++;
        }
        image_index++;
    }
    fprintf( df, "\nticks left over = %lu\n", ticks_left );
}



STATIC void dumpSampleInfo( void )
/********************************/
{
    sio_data            *sio_rover;
    mark_data           *mark;
    overlay_data        *ovl;
    image_info          *image;
    remap_data          *remap;
    ovl_entry           *ovl_entry;
    int                 image_index;
    long int            count;
    int                 index;
    thread_data         *thd;
    address             *samp;
    massgd_sample_addr  *massgd;
    char                buff1[40];
    char                buff2[40];

    sio_rover = SIOData;
    while( sio_rover != NULL ) {
        sio_rover = sio_rover->next;

        fprintf( df, "\nSample File \"%s\"\n\n", sio_rover->samp_file_name );
        fprintf( df, "  total samples = %lu\n", sio_rover->total_samples );
        fprintf( df, "  timer rate    = %lu\n", sio_rover->timer_rate );
        if( sio_rover->image_count != 0 ) {
            fprintf( df, "\n  Images\n" );
        }

        image_index = 0;
        while( image_index < sio_rover->image_count ) {
            image = sio_rover->images[image_index];
            fprintf( df, "    name = \"%s\"\n", image->name );
            fprintf( df, "    main load = " );
            if( image->main_load ) {
                fprintf( df, "TRUE\n" );
            } else {
                fprintf( df, "FALSE\n" );
            }
            fprintf( df, "    time stamp = %lx\n", image->time_stamp );
            FormatAddr( image->overlay_table, buff1, sizeof( buff1 ) );
            fprintf( df, "    overlay table = %s\n", buff1 );
            count = image->ovl_count;
            if( count != 0 ) {
                fprintf( df, "\n    Overlay table\n" );
                ovl_entry = image->ovl_data;
            }
            index = 0;
            while( count-- > 0 ) {
                fprintf( df, "      section #%d\n", index+1 );
                fprintf( df, "        file containing overlay = '%s'\n",
                        ovl_entry->fname );
                fprintf( df, "        separate overlay = " );
                if( ovl_entry->separate_overlay ) {
                    fprintf( df, "TRUE\n" );
                } else {
                    fprintf( df, "FALSE\n" );
                }
                fprintf( df, "        disk address %.8lx\n", ovl_entry->disk_addr );
                fprintf( df, "        overlay segment = %.4x, mapped value = %.4x\n",
                        ovl_entry->start_para, ovl_entry->base_para );
                ovl_entry++;
                index++;
            }
            fprintf( df, "\n" );
            count = image->map_count;
            if( count != 0 ) {
                fprintf( df, "    Address mappings\n" );
            }
            index = 0;
            while( count-- > 0 ) {
                FormatAddr( image->map_data[index].map,    buff1, sizeof( buff1 ) );
                FormatAddr( image->map_data[index].actual, buff2, sizeof( buff2 ) );
                fprintf( df, "      %s -> %s (length 0x%8.8lx)\n", buff1, buff2, (unsigned long)image->map_data[index].length );
                index++;
            }
            fprintf( df, "\n" );
            image_index++;
        }

        ovl = sio_rover->ovl_loads;
        if( ovl != NULL ) {
            fprintf( df, "  Overlay loads\n" );
        }
        while( ovl != NULL ) {
            ovl = ovl->next;
            fprintf( df, "    tick = %lu\n", ovl->tick );
            fprintf( df, "    section to load = %d\n", ovl->section );
            fprintf( df, "    requesting section = %d\n", ovl->req_addr.sect_id );
            FormatAddr( ovl->req_addr, buff1, sizeof( buff1 ) );
            fprintf( df, "    requesting address = %s\n", buff1 );
            fprintf( df, "    overlay return = " );
            if( ovl->overlay_return ) {
                fprintf( df, "TRUE\n\n" );
            } else {
                fprintf( df, "FALSE\n\n" );
            }
            if( ovl == sio_rover->ovl_loads ) break;
        }
        remap = sio_rover->remaps;
        if( remap != NULL ) {
            fprintf( df, "  Remaps\n" );
        }
        while( remap != NULL ) {
            remap = remap->next;
            fprintf( df, "    tick = %lu\n", remap->tick );
            fprintf( df, "    section = %d\n", remap->section );
            fprintf( df, "      -> segment = %.4x\n\n", remap->segment );
            if( remap == sio_rover->remaps ) break;
        }

        mark = sio_rover->marks;
        if( mark != NULL ) {
            fprintf( df, "  Marks\n" );
        }
        while( mark != NULL ) {
            mark = mark->next;
            fprintf( df, "    tick = %lu\n", mark->tick );
            fprintf( df, "    thread = %u\n", mark->thread );
            fprintf( df, "    section = %d\n", mark->addr.sect_id );
            FormatAddr( mark->addr, buff1, sizeof( buff1 ) );
            fprintf( df, "    address = %s\n", buff1 );
            fprintf( df, "    name = <%s>\n\n", mark->name );
            if( mark == sio_rover->marks ) break;
        }

        fprintf( df, "  Sample Addresses\n" );
        fprintf( df, "    # of samples = %lu\n", sio_rover->total_samples );
        for( thd = sio_rover->samples; thd != NULL; thd = thd->next ) {
            fprintf( df, "\n    Thread = %u, start tick = %lu, end tick = %lu\n",
                        thd->thread, thd->start_time, thd->end_time );
            count = thd->end_time - thd->start_time;
            for( index = 0; index < count; ++index ) {
                if( (index % 4) == 0 ) {
                    fprintf( df, "\n%.10lu ", (long int)index );
                }
                samp = &thd->raw_bucket[index / MAX_RAW_BUCKET_INDEX][index % MAX_RAW_BUCKET_INDEX];
                FormatAddr( *samp, buff1, sizeof( buff1 ) );
                fprintf( df, " [%d]%s", samp->sect_id, buff1 );
            }
            fprintf( df, "\n" );
        }

        fprintf( df, "\n  Sample Aggregates\n\n" );
        for( count = 0; count < sio_rover->number_massaged; ++count ) {
            massgd = &sio_rover->massaged_sample[count/MAX_MASSGD_BUCKET_INDEX][count%MAX_MASSGD_BUCKET_INDEX];
            FormatAddr( *massgd->raw, buff1, sizeof( buff1 ) );
            fprintf( df, "[%d]%s  %8lu\n",
                    massgd->raw->sect_id, buff1, massgd->hits );
        }
        if( sio_rover == SIOData ) break;
    }
}



extern void ReportSampleInfo( void )
/**********************************/
{
    sio_data        *curr_sio;

    if( SIOData == NULL ) return;
    df = fopen( "report.dmp", "w" );
    if( df != NULL ) {
        curr_sio = SIOData;
        for( ;; ) {
            curr_sio = curr_sio->next;
            dumpSampleInfo();
            fprintf( df, "\n" );
            dumpSampleImages( true, curr_sio );
            if( curr_sio == SIOData ) break;
        }
        fclose( df );
    }
}
