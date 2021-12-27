/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "bdiff.h"
#include "pathgrp2.h"

#include "clibext.h"


static void Usage( void )
{
    puts( "Usage: bplevel <executable> <patch_file>" );
    puts( "       Set the executable's patch level to that indicated" );
    puts( "       by the patch file." );
    exit( EXIT_FAILURE );
}

int main( int argc, char **argv )
{
    FILE            *fd;
    unsigned long   pos;
    char            buffer[sizeof( PATCH_LEVEL )];
    static char     LevelBuff[] = PATCH_LEVEL;
    struct stat     info;
    struct utimbuf  uinfo;
    pgroup2         pg;

    if( argc != 3 )
        Usage();

    stat( argv[1], &info );
    fd = fopen( argv[1], "wb" );
    if( fd == NULL ) {
        puts( "Can not open executable" );
        return( EXIT_FAILURE );
    }
    if( fseek( fd, -(long)sizeof( PATCH_LEVEL ), SEEK_END ) != 0  ) {
        puts( "Error seeking on executable" );
        return( EXIT_FAILURE );
    }
    pos = ftell( fd );
    if( fread( buffer, 1, sizeof( PATCH_LEVEL ), fd ) != sizeof( PATCH_LEVEL )
      || memcmp( buffer, LevelBuff, PATCH_LEVEL_HEAD_SIZE ) != 0 ) {
        pos += sizeof( PATCH_LEVEL );
    }
    fseek( fd, pos, SEEK_SET );
    _splitpath2( argv[2], pg.buffer, NULL, NULL, NULL, &pg.ext );
    strcpy( LevelBuff + PATCH_LEVEL_HEAD_SIZE, pg.ext );
    fwrite( LevelBuff, 1, sizeof( LevelBuff ), fd );
    fclose( fd );
    uinfo.actime = info.st_atime;
    uinfo.modtime = info.st_mtime;
    utime( argv[1], &uinfo );

    return( EXIT_SUCCESS );
}
