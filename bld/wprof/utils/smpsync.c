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


#if defined( __SMALL__ ) || defined( __MEDIUM )
#error "must be compiled with a large data model"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include <direct.h>

#include "common.h"
#include "sample.h"

#define OUTPUT_NAME     "$$SYNC$$"

FILE *sfp;
FILE *tfp;

char path[_MAX_PATH];
char drive[_MAX_DRIVE];
char dir[_MAX_DIR];
char name[_MAX_FNAME];
char ext[_MAX_EXT];

char sample_file[_MAX_PATH];
char exe_file[_MAX_PATH];
char new_file[_MAX_PATH];

static void quit( char *msg )
{
    puts( msg );
    exit( 1 );
}

static void makeName( char *original, char *extension, char *target )
{
    getcwd( path, _MAX_PATH );
    strcat( path, "\\dummy.ext" );
    _splitpath( original, drive, dir, name, ext );
    if( drive[0] == '\0' ) {
        _splitpath( path, drive, NULL, NULL, NULL );
    }
    if( dir[0] == '\0' ) {
        _splitpath( path, NULL, dir, NULL, NULL );
    }
    if( ext[0] == '\0' ) {
        strcpy( ext, extension );
    }
    _makepath( target, drive, dir, name, ext );
    strupr( target );
}

static void transferUpTo( fpos_t from, fpos_t to )
{
    fpos_t curr;

    fsetpos( sfp, &from );
    for(;;) {
        fgetpos( sfp, &curr );
        if( curr == to ) break;
        fputc( fgetc( sfp ), tfp );
    }
}

void check( int b )
{
    if( ! b ) {
        quit( "I/O error" );
    }
}

void main( int argc, char **argv )
{
    samp_header header;
    samp_code_load code_load;
    samp_block_prefix prefix;
    unsigned adjust;
    unsigned len;
    fpos_t header_position;
    fpos_t curr_position;
    fpos_t last_position;
    fpos_t start_position;
    fpos_t end_position;
    struct stat file_status;
    int st;
    long bias=0;
    int num_samps;
    int samp_count;
    samp_address sample;
    uint_16 thread_id;

    if( argc == 4 && ( argv[1][0] == '-' || argv[1][0] == '/' )
                 && argv[1][1] == 'b' ) {
        sscanf( &argv[1][2], "%lx", &bias ); /* read in bias offset */
        makeName( argv[2], ".SMP", sample_file );
        makeName( argv[3], ".EXE", exe_file );
    } else if( argc == 3 ) {
        makeName( argv[1], ".SMP", sample_file );
        makeName( argv[2], ".EXE", exe_file );
    } else {
        puts( "usage: SMPSYNC { -b<offset bias> } <sample_file> <exe_file>" );
        puts( "where: <offset bias> is the hexadecimal offset" );
        puts( "       <sample_file> and <exe_file> are absolute file names" );
        puts( "          (not relative)" );
        exit( 1 );
    }
    puts( "WATCOM Sample File Synchronization Utility  Version 1.0" );
    puts( "Copyright by WATCOM Systems Inc. 1989, 1992.  All rights reserved." );
    puts( "WATCOM is a trademark of WATCOM Systems Inc.\n" );
    _splitpath( sample_file, drive, dir, NULL, NULL );
    _makepath( new_file, drive, dir, OUTPUT_NAME, ".SMP" );
    makeName( new_file, ".SMP", new_file );
    sfp = fopen( sample_file, "rb" );
    if( sfp == NULL ) {
        quit( "cannot open sample file" );
    }
    tfp = fopen( new_file, "wb" );
    if( tfp == NULL ) {
        quit( "cannot open output file" );
    }

    st = fgetpos( sfp, &start_position );
    check( st == 0 );
    st = fseek( sfp, - (int) sizeof( header ), SEEK_END );
    check( st == 0 );
    st = fgetpos( sfp, &header_position );
    check( st == 0 );
    st = fread( &header, sizeof( header ), 1, sfp );
    check( st == 1 );
    if( header.signature != SAMP_SIGNATURE ) {
        quit( "invalid sample file" );
    }
    printf("executable file:     %s\n", exe_file );
    printf("sample file:         %s\n", sample_file );
    printf("sample file version: %u.%u\n", header.major_ver, header.minor_ver );
    printf("new sample file:     " OUTPUT_NAME ".SMP\n" );
    st = fseek( sfp, header.sample_start, SEEK_SET );
    check( st == 0 );
    st = fgetpos( sfp, &curr_position );
    check( st == 0 );
    transferUpTo( start_position, curr_position );

    do {
        last_position = curr_position;
        st = fread( &prefix, sizeof( prefix ), 1, sfp );
        if( prefix.kind == SAMP_MAIN_LOAD ) {
            st = fread( &code_load, sizeof( code_load ) - 1, 1, sfp );
            check( st == 1 );
            adjust = sizeof( prefix ) + ( sizeof( code_load ) - 1 );
            st = fseek( sfp, prefix.length - adjust, SEEK_CUR );
            check( st == 0 );
            st = fgetpos( sfp, &curr_position );
            check( st == 0 );
            st = stat( exe_file, &file_status );
            check( st == 0 );
            code_load.time_stamp = file_status.st_mtime;
            len = strlen( exe_file ) + 1;
            prefix.length = len + sizeof( prefix ) + (sizeof( code_load ) - 1);
            st = fwrite( &prefix, sizeof( prefix ), 1, tfp );
            check( st == 1 );
            st = fwrite( &code_load, sizeof( code_load ) - 1, 1, tfp );
            check( st == 1 );
            st = fwrite( exe_file, len, 1, tfp );
            check( st == 1 );
        } else if( prefix.kind == SAMP_SAMPLES ) {
            /* write prefix & thread_id: they're unchanged */
            st = fwrite( &prefix, sizeof( prefix ), 1, tfp );
            check( st == 1 );
            st = fread( &thread_id, sizeof( thread_id ), 1, sfp );
            check( st == 1 );
            st = fwrite( &thread_id, sizeof( thread_id ), 1, tfp );
            check( st == 1 );
            /* compute number of samples to modify */
            num_samps = ( prefix.length - sizeof(prefix) - sizeof(uint_16) ) /
                          sizeof( samp_address );
            for( samp_count = 0; samp_count < num_samps; samp_count++ ) {
                /* modify sample addresses according to given bias */
                st = fread( &sample, sizeof( samp_address ), 1, sfp );
                check( st == 1 );
                sample.offset += bias;
                st = fwrite( &sample, sizeof( samp_address ), 1, tfp );
                check( st == 1 );
            }
            /* reset curr_position */
            st = fgetpos( sfp, &curr_position );
            check( st == 0 );
        } else if( prefix.kind == SAMP_MARK ) {
            /* write prefix: it's unchanged */
            st = fwrite( &prefix, sizeof( prefix ), 1, tfp );
            check( st == 1 );
            /* modify mark address according to given bias */
            st = fread( &sample, sizeof( samp_address ), 1, sfp );
            check( st == 1 );
            sample.offset += bias;
            st = fwrite( &sample, sizeof( samp_address ), 1, tfp );
            check( st == 1 );
            /* copy over remaining information & update curr_position */
            st = fseek( sfp, prefix.length-sizeof(samp_block_prefix)
                                      - sizeof(samp_address), SEEK_CUR);
            check( st == 0 );
            st = fgetpos( sfp, &curr_position );
            check( st == 0 );
            transferUpTo( last_position, curr_position );
        } else if( prefix.kind == SAMP_ADDR_MAP ) {
            /* write prefix: it's unchanged */
            st = fwrite( &prefix, sizeof( prefix ), 1, tfp );
            check( st == 1 );
            /* compute number of sample addresses to modify */
            num_samps = ( prefix.length - sizeof(prefix) ) / sizeof(mapping);
            for( samp_count = 0; samp_count < num_samps; samp_count++ ) {
                /* skip over mapped address ... */
                st = fread( &sample, sizeof( samp_address ), 1, sfp );
                check( st == 1 );
                st = fwrite( &sample, sizeof( samp_address ), 1, tfp );
                check( st == 1 );
                /* ... and modify actual address */
                st = fread( &sample, sizeof( samp_address ), 1, sfp );
                check( st == 1 );
                sample.offset += bias;
                st = fwrite( &sample, sizeof( samp_address ), 1, tfp );
                check( st == 1 );
            }
            /* reset curr_position */
            st = fgetpos( sfp, &curr_position );
            check( st == 0 );
        } else {
            st = fseek( sfp, prefix.length-sizeof(samp_block_prefix), SEEK_CUR);
            check( st == 0 );
            st = fgetpos( sfp, &curr_position );
            check( st == 0 );
            transferUpTo( last_position, curr_position );
        }
    } while( curr_position != header_position );
    st = fseek( sfp, 0, SEEK_END );
    check( st == 0 );
    st = fgetpos( sfp, &end_position );
    check( st == 0 );
    transferUpTo( header_position, end_position );
    st = fclose( tfp );
    check( st == 0 );
    st = fclose( sfp );
    check( st == 0 );
    exit( 0 );
}
