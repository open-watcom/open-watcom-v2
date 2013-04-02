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
    char                *curr_dir;

    if( (f = fopen( list_fname, "r" )) == NULL ) {
        fprintf( stderr, "failed to open list '%s': %s\n",
                list_fname, strerror( errno ) );
        return( -1 );
    }
    curr_dir = NULL;
    if( dir != NULL ) {
        curr_dir = getcwd( NULL, 0 );
        chdir( dir );
    }
    /* Loop over list, add individual files */
    retval = 0;
    while( fgets( srcname, sizeof( srcname ), f ) != NULL ) {
        size_t  len;

        /* Strip terminating newline */
        len = strlen( srcname );
        if( srcname[len - 1] == '\n' )
            srcname[len - 1] = '\0';

        /* Add file to archive */
        if( (zsrc = zip_source_file( archive, srcname, 0, 0 )) == NULL
            || zip_add( archive, srcname, zsrc ) < 0) {
            zip_source_free( zsrc );
            fprintf( stderr, "failed to add '%s' to archive: %s\n",
                    srcname, zip_strerror( archive ) );
            retval = -1;
        }
    }
    if( curr_dir != NULL ) {
        chdir( curr_dir );
        free( curr_dir );
    }
    fclose( f );
    return( retval );
}

int main( int argc, char **argv )
{
    struct zip          *z;
    int                 zerr;
    char                zerrstr[MAX_ZERR_LENGTH], *zname;

    if( argc < 3 ) {
        printf( "Usage: uzip <archive> <file_list> [<files dir>]\n" );
        return( 2 );
    }

    /* Because libzip keeps all the files in the list open, we'll need
     * a humongous amount of file handles. Not sure if this is intentional
     * or a design defect of the library.
     */
#if defined( __WATCOMC__ ) || defined( __NT__ )
    _grow_handles( 4096 );
#endif

    zname = argv[1];

    if( (z = zip_open( zname, ZIP_CREATE, &zerr )) == NULL ) {
        zip_error_to_str( zerrstr, sizeof( zerrstr ), zerr, errno );
        fprintf( stderr, "failed to create archive '%s': %s\n",
                 zname, zerrstr );
        return( 1 );
    }
    /* Process list of source files */
    if( argc > 3 ) {
        add_files( z, argv[2], argv[3] );
    } else {
        add_files( z, argv[2], NULL );
    }

    if( zip_close( z ) ) {
        fprintf( stderr, "failed to write archive '%s': %s\n",
                 zname, zip_strerror( z ) );
        return( 1 );
    }
    return( 0 );
}
