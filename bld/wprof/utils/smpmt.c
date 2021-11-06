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


static char path[_MAX_PATH];

static char sample_file[_MAX_PATH];

static void quit( char *msg )
{
    puts( msg );
    exit( 1 );
}

static void makeName( const char *original, const char *ext, char *target )
{
    pgroup2     pg1;
    pgroup2     pg2;

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

#if 0
static void check( int b )
{
    if( !b ) {
        quit( "I/O error" );
    }
}
#endif

void main( int argc, char **argv )
{
    samp_header         header;
    samp_block_prefix   prefix;
    long                curr_position;
    FILE                *in;
    uint_16             new_thread;

    if( argc != 2 ) {
        puts( "usage: SMPMT <sample_file>" );
        puts( "will make a multi-thread sample file" );
        exit( 1 );
    }
puts( "WATCOM Sample File Thread Creation Utility  Version 1.0" );
puts( "Copyright by WATCOM Systems Inc. 1990, 1991.  All rights reserved." );
puts( "WATCOM is a trademark of WATCOM Systems Inc." );

    makeName( argv[1], "SMP", sample_file );
    in = fopen( sample_file, "r+b" );
    if( in == NULL ) {
        quit( "cannot open sample file" );
    }
    fseek( in, -(long)sizeof( header ), SEEK_END );
    fread( &header, 1, sizeof( header ), in );
    if( header.signature != SAMP_SIGNATURE ) {
        fclose( in );
        quit( "invalid sample file" );
    }
    fseek( in, header.sample_start, SEEK_SET );
    new_thread = 0;
    for( ;; ) {
        curr_position = ftell( in );
        fread( &prefix, 1, sizeof( prefix ), in );
        if( prefix.kind == SAMP_LAST )
            break;
        if( prefix.kind == SAMP_SAMPLES ) {
            fwrite( &new_thread, 1, sizeof( new_thread ), in );
            ++new_thread;
            new_thread &= 0x0f;
        }
        fseek( in, curr_position + prefix.length, SEEK_SET );
    }
    fclose( in );
    exit( 0 );
}
