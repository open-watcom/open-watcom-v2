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


#include "vi.h"
#include <stddef.h>
#include "posix.h"
#include "clibext.h"

/*
 * compare - quicksort comparison
 */
static int compare( const void *p1, const void *p2 )
{
    return( strcmp( (*(direct_ent * const *)p1)->name,
                    (*(direct_ent * const *)p2)->name ) );

} /* compare */

/*
 * getDir - get current directory list (no sorting)
 */
static vi_rc getDir( char *dname, bool want_all_dirs )
{
    DIR                 *d;
    struct dirent       *nd;
    direct_ent          *tmp;
    int                 i, j, len;
    char                wild[FILENAME_MAX];
    char                path[FILENAME_MAX];
    char                ch;
    bool                is_subdir;
    vi_rc               rc;

    /*
     * initialize for file scan
     */
    len = strlen( dname );
    for( i = len - 1; i >= 0; i-- ) {
        if( dname[i] == '/' || dname[i] == '\\' || dname[i] == ':' ) {
            break;
        }
    }
    for( j = 0; j < i + 1; j++ ) {
        path[j] = dname[j];
    }
    path[i + 1] = 0;
    if( i >= 0 ) {
        ch = path[i];
    } else {
        ch = 0;
    }
    for( j = i + 1; j <= len; j++ ) {
        wild[j - i - 1] = dname[j];
    }
    rc = FileMatchInit( wild );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
#ifndef __UNIX__
    if( ch != '\\' && ch != '/' && ch != ':' && ch != 0 ) {
        strcat( path, FILE_SEP_STR );
    }
    strcat( path, ALL_FILES_WILD_CARD );
#else
    if( ch == 0 ) {
        path[0] = '.';
        path[1] = 0;
    }
#endif

    for( i = 0; i < DirFileCount; i++ ) {
        MemFreePtr( (void **)&DirFiles[i] );
    }
    DirFileCount = 0;
    d = opendir( path );
    if( d == NULL ) {
        FileMatchFini();
        return( ERR_FILE_NOT_FOUND );
    }

    /*
     * loop through all directory entries
     */
    while( (nd = readdir( d )) != NULL ) {

        if( DirFileCount >= MAX_FILES ) {
            break;
        }
        is_subdir = false;
#if defined( __QNX__ )
        if( S_ISDIR( nd->d_stat.st_mode ) {
            is_subdir = true;
        }
#elif defined( __UNIX__ )
        {
            struct stat st;

            stat( nd->d_name, &st );
            if( S_ISDIR( st.st_mode ) ) {
                is_subdir = true;
            }
        }
#else
        if( nd->d_attr & _A_SUBDIR ) {
            is_subdir = true;
        }
#endif
        if( !(want_all_dirs && is_subdir) ) {
            if( !FileMatch( nd->d_name ) ) {
                continue;
            }
        }

        len = strlen( nd->d_name );
        DirFiles[DirFileCount] = MemAlloc( offsetof( direct_ent, name ) + len + 1 );
        tmp = DirFiles[DirFileCount];
        GetFileInfo( tmp, nd, path );

        memcpy( tmp->name, nd->d_name, len + 1 );
        FileLower( tmp->name );
        DirFileCount++;

    }
    FileMatchFini();
    closedir( d );
    return( ERR_NO_ERR );

} /* getDir */

/*
 * GetSortDir - get a directory and sort it
 */
vi_rc GetSortDir( char *name, bool want_all_dirs )
{
    vi_rc       rc;

    rc = getDir( name, want_all_dirs );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    if( DirFileCount ) {
        qsort( DirFiles, DirFileCount, sizeof( direct_ent * ), compare );
    }
    return( ERR_NO_ERR );

} /* GetSortDir */

void DirFini(void){
    int i;

    for( i = 0; i < DirFileCount; i++ ) {
        MemFree( DirFiles[i] );
    }
    DirFileCount = 0;
}
