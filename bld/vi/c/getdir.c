/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#include "vi.h"
#include <stddef.h>
#include "posix.h"
#include "stat2.h"

#include "clibext.h"


/*
 * compare - quicksort comparison
 */
static int compare( const void *p1, const void *p2 )
{
    return( strcmp( (*(direct_ent * const *)p1)->name,
                    (*(direct_ent * const *)p2)->name ) );

} /* compare */

static bool skipEntry( const char *path, struct dirent *dire, bool want_all_dirs )
{
#ifdef __UNIX__
  #if defined( __QNX__ )
    if( (dire->d_stat.st_status & _FILE_USED) == 0 )
        _stat2( path, dire->d_name, &dire->d_stat );
    if( S_ISDIR( dire->d_stat.st_mode ) ) {
  #else
    struct stat     st;

    _stat2( path, dire->d_name, &st );
    if( S_ISDIR( st.st_mode ) ) {
  #endif
#else
    /* unused parameters */ (void)path;

    if( dire->d_attr & _A_VOLID )
        return( true );
    if( dire->d_attr & _A_SUBDIR ) {
#endif
        return( !want_all_dirs );
    }
    return( !FileMatch( dire->d_name ) );
}

/*
 * getDir - get current directory list (no sorting)
 */
static vi_rc getDir( const char *fullmask, bool want_all_dirs )
{
    DIR                 *dirp;
    struct dirent       *dire;
    direct_ent          *tmp;
    size_t              i;
    size_t              j;
    size_t              len;
    char                wild[FILENAME_MAX];
    char                path[FILENAME_MAX];
    char                fullname[FILENAME_MAX];
    char                ch;
    vi_rc               rc;

    /*
     * initialize for file scan
     */
    len = strlen( fullmask );
    for( i = len; i > 0; i-- ) {
        ch = fullmask[i - 1];
#ifdef __UNIX__
        if( ch == FILE_SEP ) {
#else
        if( ch == ALT_FILE_SEP || ch == FILE_SEP || ch == DRV_SEP ) {
#endif
            break;
        }
    }
    for( j = 0; j < i; j++ ) {
        path[j] = fullmask[j];
    }
    path[i] = '\0';
    if( i > 0 ) {
        ch = path[i - 1];
    } else {
        ch = '\0';
    }
    for( ; j < len; j++ ) {
        wild[j - i] = fullmask[j];
    }
    wild[j - i] = '\0';
    rc = FileMatchInit( wild );
    if( rc == ERR_NO_ERR ) {
#ifndef __UNIX__
        if( ch != FILE_SEP && ch != ALT_FILE_SEP && ch != DRV_SEP && ch != '\0' ) {
#else
        if( ch != FILE_SEP && ch != '\0' ) {
#endif
            strcat( path, FILE_SEP_STR );
        }
        strcpy( fullname, path );
#ifndef __UNIX__
        strcat( fullname, ALL_FILES_WILD_CARD );
#else
        if( ch == '\0' ) {
            fullname[0] = '.';
            fullname[1] = '\0';
        }
#endif
        dirp = opendir( fullname );
        if( dirp == NULL ) {
            rc = ERR_FILE_NOT_FOUND;
        } else {
            /*
             * loop through all directory entries
             */
            while( (dire = readdir( dirp )) != NULL ) {
                if( skipEntry( path, dire, want_all_dirs ) )
                    continue;
                if( DirFileCount >= MAX_FILES ) {
                    break;
                }
                len = strlen( dire->d_name );
                tmp = MemAlloc( offsetof( direct_ent, name ) + len + 1 );
                GetFileInfo( tmp, dire, path );
                memcpy( tmp->name, dire->d_name, len + 1 );
#ifndef __UNIX__
                FileLower( tmp->name );
#endif
                DirFiles[DirFileCount++] = tmp;
            }
            closedir( dirp );
        }
    }
    FileMatchFini();
    return( rc );

} /* getDir */

/*
 * GetSortDir - get a directory contents by file name mask and sort it
 */
vi_rc GetSortDir( const char *fullmask, bool want_all_dirs )
{
    vi_rc       rc;

    DirFini();

    rc = getDir( fullmask, want_all_dirs );
    if( rc == ERR_NO_ERR ) {
        if( DirFileCount > 0 ) {
            qsort( DirFiles, DirFileCount, sizeof( direct_ent * ), compare );
        }
    }
    return( rc );

} /* GetSortDir */

void DirFini( void )
{
    list_linenum    i;

    for( i = 0; i < DirFileCount; i++ ) {
        MemFree( DirFiles[i] );
        DirFiles[i] = NULL;
    }
    DirFileCount = 0;
}
