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
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include "stat2.h"


#define SET_ATTRIBS(m)  (( S_ISDIR( m ) ) ? _A_SUBDIR : 0)

/*
 * GetFileInfo - get info from a directory entry
 */
void GetFileInfo( direct_ent *tmp, struct dirent *dire, const char *path )
{
    if( (dire->d_stat.st_status & _FILE_USED) == 0 ) {
        _stat2( path, dire->d_name, &dire->d_stat );
    }
    tmp->attr = SET_ATTRIBS( dire->d_stat.st_mode );
    tmp->fsize = dire->d_stat.st_size;
    tmp->time = dire->d_stat.st_mtime;
    tmp->st_mode = dire->d_stat.st_mode;

} /* GetFileInfo */

/*
 * MyGetFileSize - do just that
 */
int MyGetFileSize( const char *inname, long *size )
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
bool IsDirectory( const char *name )
{
    struct stat sb;

    if( stat( name, &sb ) < 0 ) {
        return( false );
    }
    if( S_ISDIR( sb.st_mode ) ) {
        return( true );
    }
    return( false );

} /* IsDirectory */

/*
 * FormatFileEntry - print a file entry
 */
void FormatFileEntry( direct_ent *file, char *res )
{
    char        buff[11];
    char        tmp[50];
    long        size;
    struct tm   *tm;
    time_t      tt;

    size = file->fsize;
    MySprintf( tmp, "  %S", file->name );
    if( IS_SUBDIR( file ) ) {
        tmp[1] = FILE_SEP;
        size = 0;
    } else if( !IsTextFile( file->name ) ) {
        tmp[1] = '*';
    }

    /*
     * build attributeibutes
     */
    strcpy( buff, "----------" );
    if( IS_SUBDIR( file ) ) {
        buff[0] = 'd';
    }
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

    tmp[NAMEWIDTH] = '\0';

    tt = file->time;
    tm = localtime( &tt );

    MySprintf(res, "%s %s %L %D/%D/%d %D:%D",
            tmp,
            buff,
            size,
            (int)tm->tm_mon + 1,
            (int)tm->tm_mday,
            (int)tm->tm_year + 80,
            (int)tm->tm_hour,
            (int)tm->tm_min);

} /* FormatFileEntry */
