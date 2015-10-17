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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utime.h>

void Usage( const char *name )
{
    printf( "Usage: %s <executable> <patch_file>\n", name );
    printf( "       Set the executable's patch level to that indicated\n" );
    printf( "       by the patch file.\n" );
    exit( EXIT_FAILURE );
}

void main( int argc, char **argv )
{
    int             io;
    unsigned long   pos;
    char            buffer[sizeof( PATCH_LEVEL )];
    static char     LevelBuff[] = PATCH_LEVEL;
    struct stat     info;
    struct utimbuf  uinfo;

    if( argc != 3 )
        Usage( argv[0] );

    stat( argv[1], &info );
    io = open( argv[1], O_BINARY | O_RDWR );
    if( io == -1 ) {
        printf( "Can not open executable\n" );
        exit( EXIT_FAILURE );
    }
    pos = lseek( io, -(long)sizeof( PATCH_LEVEL ), SEEK_END );
    if( pos == (unsigned long)-1L  ) {
        printf( "Error seeking on executable\n" );
        exit( EXIT_FAILURE );
    }
    if( read( io, buffer, sizeof( PATCH_LEVEL ) ) != sizeof( PATCH_LEVEL ) ||
        memcmp( buffer, LevelBuff, PATCH_LEVEL_HEAD_SIZE ) != 0 ) {
        pos += sizeof( PATCH_LEVEL );
    }
    lseek( io, pos, SEEK_SET );
    _splitpath( argv[2], NULL, NULL, NULL, LevelBuff + PATCH_LEVEL_HEAD_SIZE );
    write( io, LevelBuff, sizeof( LevelBuff ) );
    close( io );
    uinfo.actime = info.st_atime;
    uinfo.modtime = info.st_mtime;
    utime( argv[1], &uinfo );

    exit( EXIT_SUCCESS );
}
