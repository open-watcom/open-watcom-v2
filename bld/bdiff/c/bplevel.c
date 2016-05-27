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


#include "bdiff.h"

static void Usage( const char *name )
{
    printf( "Usage: %s <executable> <patch_file>\n", name );
    printf( "       Set the executable's patch level to that indicated\n" );
    printf( "       by the patch file.\n" );
    exit( EXIT_FAILURE );
}

void main( int argc, char **argv )
{
    FILE            *fd;
    unsigned long   pos;
    char            buffer[sizeof( PATCH_LEVEL )];
    static char     LevelBuff[] = PATCH_LEVEL;
    struct stat     info;
    struct utimbuf  uinfo;

    if( argc != 3 )
        Usage( argv[0] );

    stat( argv[1], &info );
    fd = fopen( argv[1], "wb" );
    if( fd == NULL ) {
        printf( "Can not open executable\n" );
        exit( EXIT_FAILURE );
    }
    if( fseek( fd, -(long)sizeof( PATCH_LEVEL ), SEEK_END ) != 0  ) {
        printf( "Error seeking on executable\n" );
        exit( EXIT_FAILURE );
    }
    pos = ftell( fd );
    if( fread( buffer, 1, sizeof( PATCH_LEVEL ), fd ) != sizeof( PATCH_LEVEL ) ||
        memcmp( buffer, LevelBuff, PATCH_LEVEL_HEAD_SIZE ) != 0 ) {
        pos += sizeof( PATCH_LEVEL );
    }
    fseek( fd, pos, SEEK_SET );
    _splitpath( argv[2], NULL, NULL, NULL, LevelBuff + PATCH_LEVEL_HEAD_SIZE );
    fwrite( LevelBuff, 1, sizeof( LevelBuff ), fd );
    fclose( fd );
    uinfo.actime = info.st_atime;
    uinfo.modtime = info.st_mtime;
    utime( argv[1], &uinfo );

    exit( EXIT_SUCCESS );
}
