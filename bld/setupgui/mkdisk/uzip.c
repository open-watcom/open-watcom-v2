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
* Description:  Trivial ZIP archiver utility.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined( __UNIX__ )
#include <direct.h>
#endif
#include "wio.h"
#include "setupio.h"
#include "zip.h"


// Maximum length of libzip error string
#define MAX_ZERR_LENGTH     1024


int add_files( struct zip *archive, const char *list_fname, char *dir )
{
    struct zip_source   *zsrc;
    FILE                *f;
    char                srcname[FILENAME_MAX];
    int                 retval;
    char                *dir_fname;

    if( (f = fopen( list_fname, "r" )) == NULL ) {
        fprintf( stderr, "failed to open list '%s': %s\n", list_fname, strerror( errno ) );
        return( -1 );
    }
    dir_fname = dir + strlen( dir );
    /* Loop over list, add individual files */
    retval = 0;
    while( fgets( srcname, sizeof( srcname ), f ) != NULL ) {
        char    *d;
        char    *s;
        char    c;

        d = dir_fname;
        s = srcname;
        while( (c = *s++) != '\0' ) {
            /* remove terminating newline */
            if( c == '\n' ) {
                s[-1] = '\0';
                break;
            }
#if !defined( __UNIX__ )
            if( c == '/' )
                c = '\\';
#endif
            *d++ = c;
        }
        *d = '\0';
        /* Add file to archive */
        if( (zsrc = zip_source_file( archive, dir, 0, 0 )) == NULL || zip_add( archive, srcname, zsrc ) < 0) {
            zip_source_free( zsrc );
            fprintf( stderr, "failed to add '%s' to archive: %s\n", srcname, zip_strerror( archive ) );
            retval = -1;
        }
    }
    fclose( f );
    return( retval );
}

int main( int argc, char **argv )
{
    struct zip          *z;
    int                 zerr;
    char                zerrstr[MAX_ZERR_LENGTH], *zname;
    char                dir[FILENAME_MAX];

    if( argc < 3 ) {
        printf( "Usage: uzip <archive> <file_list> [<files dir>]\n" );
        return( 2 );
    }

    zname = argv[1];

    if( (z = zip_open( zname, ZIP_CREATE, &zerr )) == NULL ) {
        zip_error_to_str( zerrstr, sizeof( zerrstr ), zerr, errno );
        fprintf( stderr, "failed to create archive '%s': %s\n", zname, zerrstr );
        return( 1 );
    }
    /* Process list of source files */
    if( argc > 3 ) {
        strcpy( dir, argv[3] );
    } else {
        getcwd( dir, FILENAME_MAX );
    }
    if( dir[0] != '\0' && ( dir[1] != ':' || dir[2] != '\0' ) ) {
#if defined( __UNIX__ )
        strcat( dir, "/" );
#else
        strcat( dir, "\\" );
#endif
    }
    add_files( z, argv[2], dir );

    if( zip_close( z ) ) {
        fprintf( stderr, "failed to write archive '%s': %s\n", zname, zip_strerror( z ) );
        return( 1 );
    }
    return( 0 );
}
