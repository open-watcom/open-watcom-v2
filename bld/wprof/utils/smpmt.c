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

char path[_MAX_PATH];
char drive[_MAX_DRIVE];
char dir[_MAX_DIR];
char name[_MAX_FNAME];
char ext[_MAX_EXT];

char sample_file[_MAX_PATH];

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

void check( int b )
{
    if( ! b ) {
        quit( "I/O error" );
    }
}

void main( int argc, char **argv )
{
    samp_header header;
    samp_block_prefix prefix;
    fpos_t header_position;
    fpos_t curr_position;
    fpos_t start_position;
    int in;
    uint_16 new_thread;

    if( argc != 2 ) {
        puts( "usage: SMPMT <sample_file>" );
        puts( "will make a multi-thread sample file" );
        exit( 1 );
    }
puts( "WATCOM Sample File Thread Creation Utility  Version 1.0" );
puts( "Copyright by WATCOM Systems Inc. 1990, 1991.  All rights reserved." );
puts( "WATCOM is a trademark of WATCOM Systems Inc." );
    makeName( argv[1], ".SMP", sample_file );
    in = open( sample_file, O_RDWR | O_BINARY );
    if( in == -1 ) {
        quit( "cannot open sample file" );
    }
    start_position = tell( in );
    lseek( in, - (int) sizeof( header ), SEEK_END );
    header_position = tell( in );
    read( in, &header, sizeof( header ) );
    if( header.signature != SAMP_SIGNATURE ) {
        quit( "invalid sample file" );
    }
    lseek( in, header.sample_start, SEEK_SET );
    new_thread = 0;
    for(;;) {
        curr_position = tell( in );
        read( in, &prefix, sizeof( prefix ) );
        if( prefix.kind == SAMP_LAST ) break;
        if( prefix.kind == SAMP_SAMPLES ) {
            write( in, &new_thread, sizeof( new_thread ) );
            ++new_thread;
            new_thread &= 0x0f;
        }
        lseek( in, curr_position + prefix.length, SEEK_SET );
    }
    exit( 0 );
}
