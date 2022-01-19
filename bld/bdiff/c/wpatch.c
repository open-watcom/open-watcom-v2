/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#ifdef __UNIX__
#include <dirent.h>
#else
#include <direct.h>
#endif
#include "wpatchio.h"
#include "msg.h"

#include "clibext.h"


#define SKIP_ENTRY(e)   ((e->d_attr & _A_SUBDIR) && e->d_name[0] == '.' && (e->d_name[1] == '\0' || dire->d_name[1] == '.' && e->d_name[2] == '\0'))

struct {
    size_t origSrcDirLen;
    size_t origTgtDirLen;
} glob;

static void DirDelete( char *tgtDir );

static void DirDelFiles( char *tgtDir, char *tgtFiles[], int Dirflag )
{
    int     indexTgt;
    char    FullTgtPath[PATCH_MAX_PATH_SIZE];

    for( indexTgt = 0; tgtFiles[indexTgt] != NULL; ++indexTgt ){
        strcpy( FullTgtPath, tgtDir );
        strcat( FullTgtPath, "\\" );
        strcat( FullTgtPath, tgtFiles[indexTgt] );
        if( Dirflag == 1 ) {
            DirDelete( FullTgtPath );
        } else {
            remove( FullTgtPath );
        }
    }
}

static int cmpStrings( const void *op1, const void *op2 )
{
    const char **p1 = (const char **) op1;
    const char **p2 = (const char **) op2;
    return( strcmp( *p1, *p2 ) );
}

static void DirGetFiles( DIR *dirp, char *Files[], char *Dirs[] )
{
    struct dirent   *dire;
    int             file = 0;
    int             dir  = 0;
    char            *diritem;

    for( ; (dire = readdir( dirp )) != NULL; ) {
        if( SKIP_ENTRY( dire ) )
            continue;
        diritem = (char *)bdiff_malloc( strlen( dire->d_name ) + 1 );
        strcpy( diritem, dire->d_name );
        if(( dire->d_attr & _A_SUBDIR ) == 0 ) {
            /* must be a file */
            Files[file++] = diritem;
            if( file >= 1000 ) {
                perror( "File limit in directory is 1000." );
            }
        } else {
            /* must be a directory */
            Dirs[dir++] = diritem;
            if( dir >= 500 ) {
                perror( "Subdirectory limit is 500." );
            }
        }
    }
    Files[file] = NULL;
    Dirs[dir] = NULL;
    qsort( Files, file, sizeof( char * ), cmpStrings );
    qsort( Dirs, dir, sizeof( char * ), cmpStrings );
}

static void DirDelete( char *tgtDir )
{
    DIR     *tgtdirp;

    char **tgtFiles = bdiff_malloc( 1000 * sizeof( char * ) );
    char **tgtDirs = bdiff_malloc( 500 * sizeof( char * ) );

    tgtdirp = opendir( tgtDir );
    if( tgtdirp == NULL ) {
        perror( "" );
    }
    DirGetFiles( tgtdirp, tgtFiles, tgtDirs );
    closedir( tgtdirp );
    DirDelFiles( tgtDir, tgtFiles, 0 );
    DirDelFiles( tgtDir, tgtDirs,  1 );
    remove( tgtDir );
}

static void WPatchApply( const char *patch_name, const char *TgtPath )
{
    short   flag;
    char    RelPath[PATCH_MAX_PATH_SIZE];
    char    FullPath[PATCH_MAX_PATH_SIZE];

    PatchReadOpen( patch_name );
    for( ;; ) {
        PatchReadFile( &flag, RelPath );
        if( flag == PATCH_EOF )
            break;
        strcpy( FullPath, TgtPath );
        strcat( FullPath, "\\" );
        strcat( FullPath, RelPath );
        switch( flag ) {
            case PATCH_FILE_PATCHED:
                printf( "Patching file %s\n", FullPath );
                DoPatch( "", 0, 0, 0, FullPath );
                break;
            case PATCH_DIR_DELETED:
                printf( "Deleting directory %s\n", FullPath );
                DirDelete( FullPath );
                break;
            case PATCH_FILE_DELETED:
                printf( "Deleting file %s\n", FullPath );
                remove( FullPath );
                break;
            case PATCH_DIR_ADDED:
                printf( "Adding directory %s\n", FullPath );
                mkdir( FullPath );
                break;
            case PATCH_FILE_ADDED:
                printf( "Adding file %s\n", FullPath );
                PatchGetFile( FullPath );
                break;
        }
    }
    PatchReadClose();
}

int main( int argc, char *argv[] )
{
    MsgInit();
    if( argc != 3 ) {
        puts( "Usage: WPATCH patchfile target-dir" );
        puts( "    where target-dir is the directory containing files to be modified" );
        puts( "    and patchfile contains patch information for modifying target-dir" );
        puts( "    (as created by WCPATCH)" );
        puts( "" );
        exit( -2 );
    } else {
        puts( "Watcom Patch version 11.0" );
        puts( "Copyright (c) 1996 by Sybase, Inc., and its subsidiaries." );
        puts( "All rights reserved.  Watcom is a trademark of Sybase, Inc." );
        puts( "" );
    }
    WPatchApply( argv[1], argv[2] );
    MsgFini();
    return( EXIT_SUCCESS );
}
