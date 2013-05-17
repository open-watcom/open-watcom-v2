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
* Description:  Directory support functions for *nix
*
****************************************************************************/


#include "vi.h"
#include <dirent.h>
#include <time.h>
#include "wio.h"


/*
 * GetFileInfo - get info from a directory entry
 */
void GetFileInfo( direct_ent *tmp, struct dirent *nd, char *path )
{
    char        *tmpname;
    int         len;
    struct stat st;

    tmpname = malloc( strlen( path ) + strlen( nd->d_name ) + 3 );
    strcpy( tmpname, path );
    len = strlen( tmpname );
    if( tmpname[len - 1] != FILE_SEP ) {
            tmpname[len] = FILE_SEP;
            tmpname[len + 1] = 0;
    }
    strcat( tmpname, nd->d_name );
    stat( tmpname, &st );
    free( tmpname );
    tmp->attr = 0;
    if( S_ISDIR( st.st_mode ) ) {
        tmp->attr |= _A_SUBDIR;
    }
    tmp->fsize = st.st_size;
    tmp->time = st.st_mtime;
    tmp->st_mode = st.st_mode;

} /* GetFileInfo */

/*
 * MyGetFileSize - do just that
 */
vi_rc MyGetFileSize( char *inname, long *size )
{
    struct stat sb;

    if( stat( inname, &sb ) < 0 ) {
        return( ERR_FILE_NOT_FOUND );
    }
    *size = sb.st_size;
    return( ERR_NO_ERR );

} /* MyGetFileSize */

/*
 * IsDirectory - check if a specified path is a directory
 */
bool IsDirectory( char *name )
{
    struct stat sb;

    if( stat( name, &sb ) < 0 ) {
        return( FALSE );
    }
    if( S_ISDIR( sb.st_mode ) ) {
        return( TRUE );
    }
    return( FALSE );

} /* IsDirectory */

/*
 * FormatFileEntry - print a file entry
 */
void FormatFileEntry( direct_ent *file, char *res )
{
    char        *tmp;
    char        buff[11];
    long        size;
    struct tm   *tm;
    time_t      tt;

    size = strlen( file->name ) + 4;
    tmp = malloc( max( size, NAMEWIDTH + 1 ) );

    strcpy( buff, "----------" );
    size = file->fsize;
    if( file->attr & _A_SUBDIR ) {
        MySprintf( tmp, " " FILE_SEP_STR "%S", file->name );
        buff[0] = 'd';
        size = 0;
    } else {
        if( !IsTextFile( file->name ) ) {
            MySprintf( tmp, " *%S", file->name );
        } else {
            MySprintf( tmp, "  %S", file->name );
        }
    }

    /*
     * build attributes
     */
    if( file->st_mode & S_IWUSR ) {
        buff[1] = 'r';
    }
    if( file->st_mode & S_IRUSR ) {
        buff[2] = 'w';
    }
    if( file->st_mode & S_IXUSR ) {
        tmp[1] = '*';
        buff[3] = 'x';
    }
    if( file->st_mode & S_IWGRP ) {
        buff[4] = 'r';
    }
    if( file->st_mode & S_IRGRP ) {
        buff[5] = 'w';
    }
    if( file->st_mode & S_IXGRP ) {
        tmp[1] = '*';
        buff[6] = 'x';
    }
    if( file->st_mode & S_IWOTH ) {
        buff[7] = 'r';
    }
    if( file->st_mode & S_IROTH ) {
        buff[8] = 'w';
    }
    if( file->st_mode & S_IXOTH ) {
        tmp[1] = '*';
        buff[9] = 'x';
    }

    tmp[NAMEWIDTH] = 0;

    tt = file->time;
    tm = localtime( &tt );

    MySprintf( res, "%s %s %L %D/%D/%d %D:%D",
               tmp,
               buff,
               size,
               (int)tm->tm_mon + 1,
               (int)tm->tm_mday,
               (int)tm->tm_year + 1900,
               (int)tm->tm_hour,
               (int)tm->tm_min );

} /* FormatFileEntry */
