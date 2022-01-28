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
* Description:  Directory-based patch creation utility.
*
****************************************************************************/


#include "bdiff.h"
#ifdef __UNIX__
    #include <dirent.h>
#else
    #include <direct.h>
#endif
#include "diff.h"
#include "wpatchio.h"
#include "wpatch.h"
#include "newfile.h"
#include "msg.h"

#include "clibext.h"


#define SKIP_ENTRY(e)   ((e->d_attr & _A_SUBDIR) && e->d_name[0] == '.' && (e->d_name[1] == '\0' || dire->d_name[1] == '.' && e->d_name[2] == '\0'))

struct {
    size_t  origSrcDirLen;
    size_t  origTgtDirLen;
} glob;

static region       *SimilarRegions;
static region       *DiffRegions;
static region       *HoleRegions;
static foff         SimilarSize;
static foff         NumDiffs;
static foff         HolesInRegion;
static foff         HoleCount[3];
static foff         HoleHeaders;

static int DirRecurse( const char *srcDir, const char *tgtDir );

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
        strlwr( diritem );
        if( ( dire->d_attr & _A_SUBDIR ) == 0 ) {
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

void FindRegionsAlg( algorithm alg )
{
    /* unused parameters */ (void)alg;

    FindRegions();
}

static int _DoBdiff( const char *srcPath, const char *tgtPath, const char *new_name )
{
    int         i;

    /* initialize static variables each time */
    SimilarRegions = NULL;
    DiffRegions = NULL;
    HoleRegions = NULL;
    SimilarSize = 0;
    NumHoles = 0;
    NumDiffs = 0;
    DiffSize = 0;
    HolesInRegion = 0;
    HoleHeaders = 0;
    for( i = 0; i < 3; i += 1 ) {
        HoleCount[i] = 0;
    }

    init_diff();

    return( DoBdiff( srcPath, tgtPath, new_name, "", ALG_NOTHING ) );
}


static int FileCmp( const char *SrcPath, const char *TgtPath, const char *name )
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
        return( _DoBdiff( SrcPath, TgtPath, name ) );
    }
    return( 0 );
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

static int DirCmpFiles( const char *srcDir, char *srcFiles[],
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
                if( DirRecurse( FullSrcPath, FullTgtPath ) ) {
                    return( 1 );
                }
            } else {
                if( FileCmp( FullSrcPath, FullTgtPath, tgtFiles[indexTgt] ) ) {
                    return( 1 );
                }
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
    return( 0 );
}

static int DirRecurse( const char *srcDir, const char *tgtDir )
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
    if( DirCmpFiles( srcDir, srcFiles, tgtDir, tgtFiles, 0 ) )
        return( 1 );
    return( DirCmpFiles( srcDir, srcDirs,  tgtDir, tgtDirs,  1 ) );
}

static void WPatchCreate( const char *SrcDirName, const char *TgtDirName, const char *patch_name )
{
    PatchWriteOpen( patch_name );
    DirRecurse( SrcDirName, TgtDirName );
    PatchWriteClose();
}

int main( int argc, char *argv[] )
{
    MsgInit();
    if( argc != 4 ) {
        puts( "Usage: wcpatch source-dir target-dir patchfile" );
        puts( "where" );
        puts( "    source-dir   the directory containing the original files" );
        puts( "    target-dir   the directory containing the modified files" );
        puts( "    patchfile    the path to store the resulting patchfile in" );
        puts( "" );
        exit( -2 );
    } else {
        puts( "Watcom Create Patch (WCPATCH) version 11.0" );
        puts( "Copyright (c) 1996 by Sybase, Inc., and its subsidiaries.");
        puts( "All rights reserved.  Watcom is a trademark of Sybase, Inc.");
        puts( "" );
    }

    glob.origSrcDirLen = strlen( argv[1] );
    glob.origTgtDirLen = strlen( argv[2] );
    WPatchCreate( argv[1], argv[2], argv[3] );
    MsgFini();
    return( EXIT_SUCCESS );
}
