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
* Description:  Determine the type of an executable.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <direct.h>
#include <string.h>
#include <io.h>
#include "wstd.h"
#include "exedos.h"


#if defined( EXETYPE_MAIN )

int main( int argc, char *argv[] )
{
    char                *pattern;
    char                dir[_MAX_DIR];
    char                drive[_MAX_DRIVE];
    DIR                 *dirp;
    struct dirent       *direntp;

    if( argc != 2 ) {
        printf( "Usage: EXETYPE file-pattern\n" );
        return( 1 );
    }
    pattern = argv[1];
    dirp = opendir( pattern );
    if( dirp == NULL ) {
        printf( "No files found matching '%s'\n", pattern );
        return( 1 );
    }
    _splitpath( pattern, drive, dir, NULL, NULL );
    for( ;; ) {
        direntp = readdir( dirp );
        if( direntp == NULL ) {
            break;
        }
        CheckFile( direntp->d_name, drive, dir );
    }
    closedir( dirp );
    return( 0 );
}


static void CheckFile( char *fname, char *drive, char *dir )
{
    int                 ok;
    char                path[_MAX_PATH];
    char                exe_type[3];

    _makepath( path, drive, dir, fname, NULL );
    printf( "%s\t", path );
    ok = ExeType( path, exe_type );
    if( !ok ) {
        printf( " not an executable\n" );
    } else {
        printf( " executable type is '%s'\n", exe_type );
    }
}

#endif


int ExeType( char *fname, char *exe_type )
{
    int                 fp;
    int                 len;
    unsigned long       offset;
    dos_exe_header      exe_header;
    char                local_type[3];

    fp = open( fname, O_RDONLY + O_BINARY, 0 );
    if( fp == -1 ) {
        return( FALSE );
    }

    // read executable header
    len = read( fp, &exe_header, sizeof( dos_exe_header ) );
    if( len < sizeof( dos_exe_header ) || exe_header.signature != DOS_SIGNATURE ) {
        close( fp );
        return( FALSE );
    }

    // at this point, its a valid executable - assume DOS
    strcpy( exe_type, "MZ" );

    // get offset of extended header
    if( lseek( fp, NH_OFFSET, SEEK_SET ) == -1 ) {
        close( fp );
        return( TRUE );
    }
    len = read( fp, &offset, sizeof( long ) );
    if( len != sizeof( long ) ) {
        close( fp );
        return( TRUE );
    }

    // determine type of extended executable
    if( lseek( fp, offset, SEEK_SET ) == -1 ) {
        close( fp );
        return( TRUE );
    }
    len = read( fp, local_type, 2 * sizeof( char ) );
    if( len != 2 * sizeof( char ) ) {
        close( fp );
        return( TRUE );
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
    return( TRUE );
}

