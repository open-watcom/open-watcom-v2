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
* Description:  Execution Sampler data file dump utility.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <malloc.h>
#include <time.h>

#include "watcom.h"
#include "banner.h"
#include "bool.h"
#include "sample.h"

char *Types[] = {
        "INFO:",
        "SAMPLES:",
        "MARK:",
        "OVL_LOAD:",
        "CODE_LOAD:",
        "ADDR_MAP:",
        "MAIN_LOAD:",
        "LAST:",
        "REMAP_SECTIONS:",
        "CALLGRAPH:",
        ""
};

#if defined( __SMALL__ ) || defined( __MEDIUM__ )
    #error must be compiled with a large data model
#endif

#define COND_SWAP_16( a )   if( byte_swap ) SWAP_16( a );
#define COND_SWAP_32( a )   if( byte_swap ) SWAP_32( a );

static char path[_MAX_PATH];
static char drv[_MAX_DRIVE];
static char dir[_MAX_DIR];
static char name[_MAX_FNAME];
static char ext[_MAX_EXT];

int main( int argc, char **argv )
{
    char                quiet;
    int                 fd;
    unsigned            wanted;
    unsigned            i, j, k, l;
    int                 length = 0;
    off_t               head_off;
    char *              record_type;
    samp_block *        data;
    auto samp_header    head;
    cgraph_sample *     sptr;
    auto struct stat    file_stat;
    time_t              stamp;
    count_info          *count;
    bool                byte_swap = FALSE;


    puts( banner1w( "Sample File Dump Utility", _WSAMP_VERSION_ ) );
    puts( banner2( "1989" ) );
    puts( banner3 );
    puts( banner3a );

    if( argc != 2 && argc != 3 ) {
        puts( "usage: smpdump <sample_file> [-q]" );
        exit( 1 );
    }
    data = malloc( 63L * 1024 + 512 );
    quiet = 0;
    if( argc == 3 && strcmp( argv[2], "-q" ) == 0 ) {
        quiet = 1;
    }
    _splitpath( argv[ 1 ], drv, dir, name, ext );
    if( ext[ 0 ] == '\0' ) {
        strcpy( ext, ".smp" );
    }
    _makepath( path, drv, dir, name, ext );
    fd = open( path, O_BINARY | O_RDONLY, 0 );
    if( fd == -1 )
        return( 1 );
    head_off = lseek( fd, -(long)SIZE_HEADER, SEEK_END );
    if( head_off == -1 )
        return( 1 );
    read( fd, &head, SIZE_HEADER );
    if( head.signature != SAMP_SIGNATURE ) {
        SWAP_16( head.signature );
        if( head.signature == SAMP_SIGNATURE ) {
            byte_swap = TRUE;
            SWAP_32( head.sample_start );
        } else {
            return( 1 );
        }
    }
    printf( "Sample file version: %u.%u\n", head.major_ver, head.minor_ver );
    lseek( fd, head.sample_start, SEEK_SET );
    for( ;; ) {
        /* read the prefix of record */
        wanted = sizeof( data->pref );
        if( read( fd, &data->pref, wanted ) != wanted ) break;
        COND_SWAP_32( data->pref.tick );
        COND_SWAP_16( data->pref.length );
        COND_SWAP_16( data->pref.kind );

        /* read the rest of the record */
        wanted = data->pref.length - sizeof( data->pref );
        if( read( fd, &data->d, wanted ) != wanted ) break;

        /* dump common record data */
        record_type = "** UNKNOWN **";
        if( data->pref.kind < NUM_BLOCK_KINDS ) {
            record_type = Types[ data->pref.kind ];
        }
        printf( "%s  tick %lu length %u\n", record_type,
                data->pref.tick, data->pref.length );

        /* dump specific record data */
        switch( data->pref.kind ) {
        case SAMP_INFO:
            COND_SWAP_32( data->d.info.timer_rate );
            printf( "  timer rate %ld ms\n", data->d.info.timer_rate / 1000 );
            l = data->pref.length;
            if( head.major_ver == 2 && head.minor_ver <= 1 ) {
                count = &data->d.old_info.count[0];
                l -= offsetof( samp_block, d.old_info.count );
            } else {
                COND_SWAP_16( data->d.info.config.mad );
                printf( "  cpu=%d, fpu=%d, os_maj=%d, os_min=%d, os=%d, mad=%d\n",
                        data->d.info.config.cpu, data->d.info.config.fpu,
                        data->d.info.config.osmajor, data->d.info.config.osminor,
                        data->d.info.config.os, data->d.info.config.mad );
                count = &data->d.info.count[0];
                l -= offsetof( samp_block, d.info.count );
            }
            l /= sizeof( count_info );
            for( i = 0; i < l; ++i ) {
                COND_SWAP_32( count[i].number );
                COND_SWAP_32( count[i].size );
                printf( "    %s number %lu size %lu\n", Types[i],
                        count[i].number, count[i].size );
            }
            break;
        case SAMP_SAMPLES:
            j = data->pref.length;
            j -= offsetof( samp_block, d.sample.sample );
            j /= sizeof( samp_address );
            COND_SWAP_16( data->d.sample.thread_id );
            printf( "  thread id %u  #samples %u\n",
                    data->d.sample.thread_id, j );
            if( quiet ) {
                printf( "    ....\n    ....\n    ....\n" );
            } else {
                for( i = 0; i < j; ++i ) {
                    COND_SWAP_16( data->d.sample.sample[i].segment );
                    COND_SWAP_32( data->d.sample.sample[i].offset );
                    printf( "  - %.4x:%.8lx\n",
                            data->d.sample.sample[i].segment,
                            data->d.sample.sample[i].offset );
                }
            }
            break;
        case SAMP_MARK:
            COND_SWAP_16( data->d.sample.thread_id );
            COND_SWAP_16( data->d.mark.addr.segment );
            COND_SWAP_32( data->d.mark.addr.offset );
            printf( "  thread %u - %.4x:%.8lx  \"%s\"\n",
                    data->d.mark.thread_id, data->d.mark.addr.segment,
                    data->d.mark.addr.offset, data->d.mark.mark_string );
            break;
        case SAMP_OVL_LOAD:
            if( data->d.ovl.req_section & OVL_RETURN ) {
                printf( "  section %u (RETURN)\n",
                        data->d.ovl.req_section & (~ OVL_RETURN) );
            } else {
                printf( "  section %u\n", data->d.ovl.req_section );
            }
            printf( "    - requesting address = %.4x:%.8lx\n",
                    data->d.ovl.addr.segment, data->d.ovl.addr.offset );
            j = data->pref.length;
            j -= offsetof( samp_block, d.ovl.ovl_map );
            j /= sizeof( uint_8 );
            printf( "    - loaded overlay sections = " );
//            printf( "%.2x", data->d.ovl.ovl_map[0] );
            for( i = 0; i < j; ++i ) {
                printf( "%.2x", data->d.ovl.ovl_map[i] );
            }
            printf( "\n" );
            break;
        case SAMP_CODE_LOAD:
        case SAMP_MAIN_LOAD:
            stamp = data->d.code.time_stamp;
            COND_SWAP_32( stamp );
            COND_SWAP_16( data->d.code.ovl_tab.segment );
            COND_SWAP_32( data->d.code.ovl_tab.offset );
            printf( "  name = \"%s\"\n", data->d.code.name );
            printf( "  overlay table = %.4x:%.8lx\n",
                    data->d.code.ovl_tab.segment, data->d.code.ovl_tab.offset );
            printf( "  time stamp %lx -> %s", stamp,
                    ctime( &stamp ) );
            if( stat( data->d.code.name, &file_stat ) == 0 ) {
                printf( "  actual time stamp %lx -> %s", file_stat.st_mtime,
                        ctime( &(file_stat.st_mtime) ) );
            }
            break;
        case SAMP_ADDR_MAP:
            j = data->pref.length;
            j -= offsetof( samp_block, d.map );
            j /= sizeof( data->d.map );
            for( i = 0; i < j; ++i ) {
                COND_SWAP_16( data->d.map.data[i].map.segment );
                COND_SWAP_32( data->d.map.data[i].map.offset );
                COND_SWAP_16( data->d.map.data[i].actual.segment );
                COND_SWAP_32( data->d.map.data[i].actual.offset );
                printf( "  - %.4x:%.8lx -> %.4x:%.8lx\n",
                        data->d.map.data[i].map.segment,
                        data->d.map.data[i].map.offset,
                        data->d.map.data[i].actual.segment,
                        data->d.map.data[i].actual.offset );
            }
            break;
        case SAMP_LAST:
            break;
        case SAMP_REMAP_SECTION:
            j = data->pref.length;
            j -= offsetof( samp_block, d.remap );
            j /= sizeof( data->d.remap );
            for( i = 0; i < j; ++i ) {
                COND_SWAP_16( data->d.remap.data[i].section );
                COND_SWAP_16( data->d.remap.data[i].segment );
                printf( "  - %.4x -> %.4x\n",
                        data->d.remap.data[i].section,
                        data->d.remap.data[i].segment );
            }
            break;
        case SAMP_CALLGRAPH:
            j = data->d.cgraph.number;
            printf( "  thread id %u  #samples %u\n",
                                 data->d.cgraph.thread_id, j );
            if( quiet ) {
                printf( "    ....\n    ....\n    ....\n" );
            } else {
                sptr = &data->d.cgraph.sample[0];
                for( i = 0; i < j; i++ ) {
                    if( sptr->push_n != (uint_16)-1 || sptr->pop_n != (uint_16)-1 ) {
                        length -= sptr->pop_n;
                        if( length < 0 ) {
                            printf( "\n** Error: stack exhausted!\n\n" );
                        } else {
                            for( k = 0; k < length; k++ ) {
                                printf( " -  " );
                            }
                        }
                        for( k=sptr->push_n; k > 0; k-- ) {
                            printf( "%.4x:%.8lx    ",
                                sptr->addr[k - 1].segment,
                                sptr->addr[k - 1].offset );
                        }
                        length += sptr->push_n;
                        printf( "\n" );
                        sptr = (cgraph_sample *) &(sptr->addr[sptr->push_n]);
                    } else {
                        printf( "** Unknown callgraph info\n" );
                        sptr = (cgraph_sample *) &(sptr->addr[0]);
                    }
                }
            }
            break;
        }
        if( data->pref.kind == SAMP_LAST ) break;
    }
    close( fd );
    return( 0 );
}
