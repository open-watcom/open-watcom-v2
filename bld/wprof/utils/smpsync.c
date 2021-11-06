/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
//#include <malloc.h>
#include <direct.h>
#include "common.h"
#include "sample.h"
#include "pathgrp2.h"

#include "clibext.h"


#define OUTPUT_NAME     "$$SYNC$$.SMP"

static FILE *sfp;
static FILE *tfp;

static char path[_MAX_PATH];
static char sample_file[_MAX_PATH];
static char exe_file[_MAX_PATH];
static char new_file[_MAX_PATH];

static pgroup2     pg1;
static pgroup2     pg2;

static void quit( char *msg )
{
    puts( msg );
    exit( 1 );
}

static void makeName( const char *original, const char *ext, char *target )
{
    getcwd( path, _MAX_PATH );
    strcat( path, "\\dummy.ext" );
    _splitpath2( original, pg1.buffer, &pg1.drive, &pg1.dir, &pg1.fname, &pg1.ext );
    _splitpath2( path, pg2.buffer, &pg2.drive, &pg2.dir, NULL, NULL );
    if( pg1.drive[0] == '\0' ) {
        pg1.drive = pg2.drive;
    }
    if( pg1.dir[0] == '\0' ) {
        pg1.dir = pg2.dir;
    }
    if( pg1.ext[0] != '\0' ) {
        ext = pg1.ext;
    }
    _makepath( target, pg1.drive, pg1.dir, pg1.fname, ext );
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

static void check( int b )
{
    if( ! b ) {
        quit( "I/O error" );
    }
}

void main( int argc, char **argv )
{
    samp_header         header;
    samp_code_load      code_load;
    samp_block_prefix   prefix;
    unsigned            adjust;
    size_t              len;
    fpos_t              header_position;
    fpos_t              curr_position;
    fpos_t              last_position;
    fpos_t              start_position;
    fpos_t              end_position;
    struct stat         file_status;
    int                 st;
    long                bias;
    int                 num_samps;
    int                 samp_count;
    samp_address        sample;
    uint_16             thread_id;

    bias = 0;
    if( argc == 4 && ( argv[1][0] == '-' || argv[1][0] == '/' ) && argv[1][1] == 'b' ) {
        sscanf( &argv[1][2], "%lx", &bias ); /* read in bias offset */
        makeName( argv[2], "SMP", sample_file );
        makeName( argv[3], "EXE", exe_file );
    } else if( argc == 3 ) {
        makeName( argv[1], "SMP", sample_file );
        makeName( argv[2], "EXE", exe_file );
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
    _splitpath2( sample_file, pg1.buffer, &pg1.drive, &pg1.dir, NULL, NULL );
    _makepath( new_file, pg1.drive, pg1.dir, OUTPUT_NAME, NULL );
    makeName( new_file, "SMP", new_file );
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
    st = fseek( sfp, -(long)sizeof( header ), SEEK_END );
    check( st == 0 );
    st = fgetpos( sfp, &header_position );
    check( st == 0 );
    check( fread( &header, sizeof( header ), 1, sfp ) == 1 );
    if( header.signature != SAMP_SIGNATURE ) {
        quit( "invalid sample file" );
    }
    printf("executable file:     %s\n", exe_file );
    printf("sample file:         %s\n", sample_file );
    printf("sample file version: %u.%u\n", header.major_ver, header.minor_ver );
    printf("new sample file:     " OUTPUT_NAME "\n" );
    st = fseek( sfp, header.sample_start, SEEK_SET );
    check( st == 0 );
    st = fgetpos( sfp, &curr_position );
    check( st == 0 );
    transferUpTo( start_position, curr_position );

    do {
        last_position = curr_position;
        check( fread( &prefix, sizeof( prefix ), 1, sfp ) == 1 );
        if( prefix.kind == SAMP_MAIN_LOAD ) {
            check( fread( &code_load, sizeof( code_load ) - 1, 1, sfp ) == 1 );
            adjust = sizeof( prefix ) + ( sizeof( code_load ) - 1 );
            st = fseek( sfp, prefix.length - adjust, SEEK_CUR );
            check( st == 0 );
            st = fgetpos( sfp, &curr_position );
            check( st == 0 );
            st = stat( exe_file, &file_status );
            check( st == 0 );
            code_load.time_stamp = (unsigned long)file_status.st_mtime;
            len = strlen( exe_file ) + 1;
            prefix.length = (uint_16)( len + sizeof( prefix ) + ( sizeof( code_load ) - 1 ) );
            check( fwrite( &prefix, sizeof( prefix ), 1, tfp ) == 1 );
            check( fwrite( &code_load, sizeof( code_load ) - 1, 1, tfp ) == 1 );
            check( fwrite( exe_file, len, 1, tfp ) == 1 );
        } else if( prefix.kind == SAMP_SAMPLES ) {
            /* write prefix & thread_id: they're unchanged */
            check( fwrite( &prefix, sizeof( prefix ), 1, tfp ) == 1 );
            check( fread( &thread_id, sizeof( thread_id ), 1, sfp ) == 1 );
            check( fwrite( &thread_id, sizeof( thread_id ), 1, tfp ) == 1 );
            /* compute number of samples to modify */
            num_samps = ( prefix.length - sizeof( prefix ) - sizeof( uint_16 ) ) / sizeof( samp_address );
            for( samp_count = 0; samp_count < num_samps; samp_count++ ) {
                /* modify sample addresses according to given bias */
                check( fread( &sample, sizeof( samp_address ), 1, sfp ) == 1 );
                sample.offset += bias;
                check( fwrite( &sample, sizeof( samp_address ), 1, tfp ) == 1 );
            }
            /* reset curr_position */
            st = fgetpos( sfp, &curr_position );
            check( st == 0 );
        } else if( prefix.kind == SAMP_MARK ) {
            /* write prefix: it's unchanged */
            check( fwrite( &prefix, sizeof( prefix ), 1, tfp ) == 1 );
            /* modify mark address according to given bias */
            check( fread( &sample, sizeof( samp_address ), 1, sfp ) == 1 );
            sample.offset += bias;
            check( fwrite( &sample, sizeof( samp_address ), 1, tfp ) == 1 );
            /* copy over remaining information & update curr_position */
            st = fseek( sfp, prefix.length - sizeof( samp_block_prefix ) - sizeof( samp_address ), SEEK_CUR );
            check( st == 0 );
            st = fgetpos( sfp, &curr_position );
            check( st == 0 );
            transferUpTo( last_position, curr_position );
        } else if( prefix.kind == SAMP_ADDR_MAP ) {
            /* write prefix: it's unchanged */
            check( fwrite( &prefix, sizeof( prefix ), 1, tfp ) == 1 );
            /* compute number of sample addresses to modify */
            num_samps = ( prefix.length - sizeof(prefix) ) / sizeof(mapping);
            for( samp_count = 0; samp_count < num_samps; samp_count++ ) {
                /* skip over mapped address ... */
                check( fread( &sample, sizeof( samp_address ), 1, sfp ) == 1 );
                check( fwrite( &sample, sizeof( samp_address ), 1, tfp ) == 1 );
                /* ... and modify actual address */
                check( fread( &sample, sizeof( samp_address ), 1, sfp ) == 1 );
                sample.offset += bias;
                check( fwrite( &sample, sizeof( samp_address ), 1, tfp ) == 1 );
            }
            /* reset curr_position */
            st = fgetpos( sfp, &curr_position );
            check( st == 0 );
        } else {
            st = fseek( sfp, prefix.length - sizeof( samp_block_prefix ), SEEK_CUR );
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
