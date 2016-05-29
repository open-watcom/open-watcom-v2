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
* Description:  Directory-based patch creation utility.
*
****************************************************************************/


#include "bdiff.h"
#include <direct.h>
#include "wpatchio.h"
#include "wpatch.h"
#include "newfile.h"
#include "msg.h"

#include "clibext.h"


struct {
    size_t  origSrcDirLen;
    size_t  origTgtDirLen;
} glob;

int     cmpStrings( const void *, const void * );
void    WPatchCreate( const char *SrcDirName, const char *TgtDirName, const char *patch_name );
void    DirRecurse( const char *srcDir, const char *tgtDir );
void    DirGetFiles( DIR *dirp, char *Files[], char *Dirs[] );
void    DirCmpFiles( const char *srcDir, char *srcFiles[], const char *tgtDir, char *tgtFiles[], int Dirflag );


void main( int argc, char *argv[] )
{
    MsgInit();
    if( argc != 4 ) {
        printf( "Usage: WCPATCH source-dir target-dir patchfile\n" );
        printf( "where source-dir is the directory containing the original files,\n" );
        printf( "target-dir is the directory containing the modified files,\n" );
        printf( "and patchfile is the path to store the resulting patchfile in.\n\n" );
        exit( -2 );
    } else {
        printf( "Watcom Create Patch (WCPATCH) version 11.0\n" );
        printf( "Copyright (c) 1996 by Sybase, Inc., and its subsidiaries.\n");
        printf( "All rights reserved.  Watcom is a trademark of Sybase, Inc.\n\n");
    }

    glob.origSrcDirLen = strlen( argv[1] );
    glob.origTgtDirLen = strlen( argv[2] );
    WPatchCreate( argv[1], argv[2], argv[3] );
    MsgFini();
}

void WPatchCreate( const char *SrcDirName, const char *TgtDirName, const char *patch_name )
{
    PatchWriteOpen( patch_name );
    DirRecurse( SrcDirName, TgtDirName );
    PatchWriteClose();

}

void DirRecurse( const char *srcDir, const char *tgtDir )
{
    DIR     *srcdirp;
    DIR     *tgtdirp;

    char **srcFiles = bdiff_malloc( 1000 * sizeof( char * ) );
    char **srcDirs = bdiff_malloc( 500 * sizeof( char * ) );
    char **tgtFiles = bdiff_malloc( 1000 * sizeof( char * ) );
    char **tgtDirs = bdiff_malloc( 500 * sizeof( char * ) );

    srcdirp = opendir( srcDir );
    tgtdirp = opendir( tgtDir );
    if( srcdirp == NULL || tgtdirp == NULL ) {
        perror( "" );
    }
    DirGetFiles( srcdirp, srcFiles, srcDirs );
    DirGetFiles( tgtdirp, tgtFiles, tgtDirs );
    closedir( srcdirp );
    closedir( tgtdirp );
    DirCmpFiles( srcDir, srcFiles, tgtDir, tgtFiles, 0 );
    DirCmpFiles( srcDir, srcDirs,  tgtDir, tgtDirs,  1 );
}

void DirGetFiles( DIR *dirp, char *Files[], char *Dirs[] )
{
    struct dirent   *direntp;
    int             file = 0;
    int             dir  = 0;

    for( ;; ) {
        direntp = readdir( dirp );
        if( direntp == NULL )
            break;
        if( ( direntp->d_attr & _A_SUBDIR ) == 0 ) {
            /* must be a file */
            Files[file] = (char *)bdiff_malloc( strlen( direntp->d_name ) + 1 );;
            strcpy( Files[file], direntp->d_name );
            strlwr( Files[file] );
            file += 1;
            if( file >= 1000 ) {
                perror( "File limit in directory is 1000." );
            }
        } else {
            /* must be a directory */
            Dirs[dir] = (char *)bdiff_malloc( strlen( direntp->d_name ) + 1 );
            strcpy( Dirs[dir], direntp->d_name );
            strlwr( Dirs[dir] );
            if( strcmp( Dirs[dir], "." ) != 0 && strcmp( Dirs[dir], ".." ) != 0 ) {
                dir += 1;
            }
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

int cmpStrings( const void *op1, const void *op2 )
{
    const char **p1 = (const char **) op1;
    const char **p2 = (const char **) op2;
    return( strcmp( *p1, *p2 ) );
}

static void FileCmp( const char *SrcPath, const char *TgtPath, const char *name )
{
    FILE    *srcF;
    FILE    *tgtF;
    int     srcchar;
    int     tgtchar;
    bool    different = false;

    srcF = fopen( SrcPath, "rb" );
    tgtF = fopen( TgtPath, "rb" );
    do {
        srcchar = fgetc( srcF );
        tgtchar = fgetc( tgtF );
        if( srcchar != tgtchar ) {
            different = true;
        }
    } while( !feof( srcF ) && !feof( tgtF ) && !different );
    fclose( srcF );
    fclose( tgtF );

    if( different ) {
        printf( "%s is different.  Patching...\n", name );
        PatchWriteFile( PATCH_FILE_PATCHED, &TgtPath[glob.origTgtDirLen + 1] );
        DoBdiff( SrcPath, TgtPath, name );
    }
}

static void DirMarkDeleted( const char *Path )
{
    printf( "Deleting directory %s\n", Path );
    PatchWriteFile( PATCH_DIR_DELETED, Path );
}

static void FileMarkDeleted( const char *Path )
{
    printf( "Deleting file %s\n", Path );
    PatchWriteFile( PATCH_FILE_DELETED, Path );
}

static void FileMarkAdded( const char *Path, size_t start )
{
    printf( "Adding file %s\n", Path );
    PatchWriteFile( PATCH_FILE_ADDED, &Path[start] );
    PatchAddFile( Path );
}

static void DirMarkAdded( const char *Path, size_t start )
{
    DIR     *dirp;
    char    *Files[1000];
    char    *Dirs [500];
    char    FullPath[PATCH_MAX_PATH_SIZE];
    int     index;

    printf( "Adding directory %s\n", Path );
    PatchWriteFile( PATCH_DIR_ADDED, &Path[start] );

    dirp = opendir( Path );
    if( dirp == NULL ) {
        perror( "" );
    }
    DirGetFiles( dirp, Files, Dirs );
    closedir( dirp );
    for( index = 0; Files[index] != NULL; ++index ) {
        strcpy( FullPath, Path );
        strcat( FullPath, "\\" );
        strcat( FullPath, Files[index] );
        FileMarkAdded( FullPath, start );
    }
    for( index = 0; Dirs[index] != NULL; ++index ) {
        strcpy( FullPath, Path );
        strcat( FullPath, "\\" );
        strcat( FullPath, Dirs[index] );
        DirMarkAdded( FullPath, start );
    }
}

/*************************
    DirCmpFiles

This takes the sorted lists of files and directories in source and target
directories, and attempts to match them.  If files do not match, they are
marked to be added or deleted, as appropriate.  If they do match, they are
compared, and a patch is made if they do not match.

*/

void DirCmpFiles( const char *srcDir, char *srcFiles[],
                  const char *tgtDir, char *tgtFiles[], int Dirflag )
{
    int     indexSrc = 0;
    int     indexTgt = 0;
    int     test;
    char    FullSrcPath[PATCH_MAX_PATH_SIZE];
    char    FullTgtPath[PATCH_MAX_PATH_SIZE];

    while( srcFiles[indexSrc] != NULL || tgtFiles[indexTgt] != NULL ) {
        if( srcFiles[indexSrc] == NULL ) {
            test = 1;
        } else if( tgtFiles[indexTgt] == NULL ) {
            test = -1;
        } else {
            test = strcmp( srcFiles[indexSrc], tgtFiles[indexTgt] );
        }
        if( test == 0 ) {
            strcpy( FullSrcPath, srcDir );
            strcat( FullSrcPath, "\\" );
            strcat( FullSrcPath, srcFiles[indexSrc] );
            strcpy( FullTgtPath, tgtDir );
            strcat( FullTgtPath, "\\" );
            strcat( FullTgtPath, tgtFiles[indexTgt] );
            if( Dirflag == 1 ) {
                DirRecurse( FullSrcPath, FullTgtPath );
            } else {
                FileCmp( FullSrcPath, FullTgtPath, tgtFiles[indexTgt] );
            }
            indexSrc += 1;
            indexTgt += 1;
        } else if( test < 0 ) { /* file deleted */
            strcpy( FullSrcPath, srcDir );
            strcat( FullSrcPath, "\\" );
            strcat( FullSrcPath, srcFiles[indexSrc] );
            if( Dirflag == 1 ) {
                DirMarkDeleted( &FullSrcPath[glob.origSrcDirLen + 1] );
            } else {
                FileMarkDeleted( &FullSrcPath[glob.origSrcDirLen + 1] );
            }
            indexSrc += 1;
        } else { /* file added */
            strcpy( FullTgtPath, tgtDir );
            strcat( FullTgtPath, "\\" );
            strcat( FullTgtPath, tgtFiles[indexTgt] );
            if( Dirflag == 1 ) {
                DirMarkAdded( FullTgtPath, glob.origTgtDirLen + 1 );
            } else {
                FileMarkAdded( FullTgtPath, glob.origTgtDirLen + 1 );
            }
            indexTgt += 1;
        }
    }
}
