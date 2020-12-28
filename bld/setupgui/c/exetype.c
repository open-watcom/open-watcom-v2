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
* Description:  Determine the type of an executable.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#ifdef __UNIX__
    #include <dirent.h>
#else
    #include <direct.h>
#endif
#include "wio.h"
#include "wstd.h"
#include "exedos.h"
#include "exetype.h"


#if defined( EXETYPE_MAIN )

static void CheckFile( const char *path )
{
    bool            ok;
    char            exe_type[3];

    printf( "%s\t", path );
    ok = ExeType( path, exe_type );
    if( !ok ) {
        printf( " not an executable\n" );
    } else {
        printf( " executable type is '%s'\n", exe_type );
    }
}

int main( int argc, char *argv[] )
{
    char            *pattern;
    char            path[_MAX_PATH];
    pgroup2         pg;
    DIR             *dirp;
    struct dirent   *dire;

    if( argc != 2 ) {
        printf( "Usage: EXETYPE file-pattern\n" );
        return( 1 );
    }
    pattern = argv[1];
    dirp = opendir( pattern );
    if( dirp != NULL ) {
        _splitpath2( pattern, pg.buffer, &pg.drive, &pg.dir, NULL, NULL );
        for( ; (dire = readdir( dirp )) != NULL; ) {
            _makepath( path, pg.drive, pg.dir, dire->d_name, NULL );
            CheckFile( path );
        }
        closedir( dirp );
        return( 0 );
    }
    printf( "No files found matching '%s'\n", pattern );
    return( 1 );
}

#endif


bool ExeType( const char *fname, char *exe_type )
{
    int                 fp;
    int                 len;
    unsigned long       offset;
    dos_exe_header      exe_header;
    char                local_type[3];

    fp = open( fname, O_RDONLY | O_BINARY );
    if( fp == -1 ) {
        return( false );
    }

    // read executable header
    len = read( fp, &exe_header, sizeof( dos_exe_header ) );
    if( len < sizeof( dos_exe_header ) || exe_header.signature != DOS_SIGNATURE ) {
        close( fp );
        return( false );
    }

    // at this point, its a valid executable - assume DOS
    strcpy( exe_type, "MZ" );

    // get offset of extended header
    if( lseek( fp, NH_OFFSET, SEEK_SET ) == -1 ) {
        close( fp );
        return( true );
    }
    len = read( fp, &offset, sizeof( long ) );
    if( len != sizeof( long ) ) {
        close( fp );
        return( true );
    }

    // determine type of extended executable
    if( lseek( fp, offset, SEEK_SET ) == -1 ) {
        close( fp );
        return( true );
    }
    len = read( fp, local_type, 2 * sizeof( char ) );
    if( len != 2 * sizeof( char ) ) {
        close( fp );
        return( true );
    }
    local_type[2] = '\0';
    if( strcmp( local_type, "PE" ) == 0 ) {             // Windows NT
        strcpy( exe_type, local_type );
    } else if( strcmp( local_type, "NE" ) == 0 ) {      // Windows or OS/2 1.x
        strcpy( exe_type, local_type );
    } else if( strcmp( local_type, "LE" ) == 0 ) {      // DOS/4G
        strcpy( exe_type, local_type );
    } else if( strcmp( local_type, "LX" ) == 0 ) {      // OS/2 2.x
        strcpy( exe_type, local_type );
    }
    close( fp );
    return( true );
}
