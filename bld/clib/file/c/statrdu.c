/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of stat() for RDOS.
*
****************************************************************************/

#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <rdos.h>
#include <ctype.h>
#include <time.h>
#include "rtdata.h"
#include "pathmac.h"
#include "find.h"


static unsigned short _WCNEAR at2mode( int attr, char *fname )
{
    register unsigned short mode;
    register char           *ext;

    if( attr & _A_SUBDIR )
        mode = S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
    else {
        mode = S_IFREG;
        /* determine if file is executable, very PC specific */
        if( (ext = strchr( fname, '.' )) != NULL ) {
            ++ext;
            if( strcmp( ext, "EXE" ) == 0 || strcmp( ext, "COM" ) == 0 ) {
                mode |= S_IXUSR | S_IXGRP | S_IXOTH;
            }
        }
    }
    mode |= S_IRUSR | S_IRGRP | S_IROTH;
    if( (attr & _A_RDONLY) == 0 )               /* if file is not read-only */
        mode |= S_IWUSR | S_IWGRP | S_IWOTH;    /* - indicate writeable     */
    return( mode );
}

_WCRTLINK int stat( CHAR_TYPE const *path, struct stat *buf )
{
    const CHAR_TYPE     *ptr;
    CHAR_TYPE            fullpath[_MAX_PATH];
    CHAR_TYPE           *fullp;
    struct RdosDirInfo   dinf;
    struct RdosDirEntry *dirent;
    bool                 isrootdir;
    int                  dir_handle;
    bool                 found;
    CHAR_TYPE            check[_MAX_PATH];
    CHAR_TYPE            name[_MAX_PATH];
    char                *chainptr;
    int                  attrib;
    int                  i;

    /* reject null string and names that has wildcard */
    if( *path == NULLCHAR || strpbrk( path, "*?" ) != NULL )
        return( -1 );

    /*** Determine if 'path' refers to a root directory ***/
    isrootdir = false;
    if( _fullpath( fullpath, path, _MAX_PATH ) != NULL ) {
        if( HAS_DRIVE( fullpath ) && fullpath[2] == DIR_SEP && fullpath[3] == NULLCHAR ) {
            isrootdir = true;
        }
    } else {
        return( -1 );
    }

    ptr = path;
    if( path[1] == DRV_SEP )
        ptr += 2;
    if( IS_DIR_SEP( ptr[0] ) && ptr[1] == NULLCHAR || isrootdir ) {
        /* handle root directory */
        CHAR_TYPE       cwd[_MAX_PATH];

        /* save current directory */
        getcwd( cwd, _MAX_PATH );

        /* try to change to specified root */
        if( chdir( path ) != 0 )
            return( -1 );

        /* restore current directory */
        chdir( cwd );

        attrib   = _A_SUBDIR;
        name[0] = NULLCHAR;
        dir_handle = 0;
    } else {                            /* not a root directory */
        fullp = fullpath + strlen( fullpath ) - 1;
        while( !IS_DIR_SEP( *fullp ) && fullp != fullpath )
            fullp--;

        fullp++;
        strcpy( check, fullp );
        _strlwr( check );

        fullp -= 2;
        if( *fullp == ':' )
            fullp++;
        fullp++;
        *fullp = 0;

        if( strlen( fullpath ) == 0 )
            return( -1 );

        dinf.Count = 0;
        dir_handle = RdosOpenDir( fullpath, &dinf );

        found = false;
        i = 0;
        chainptr = (char *)dinf.Entry;
        while( i < dinf.Count ) {
            dirent = (struct RdosDirEntry *)chainptr;
            strcpy( name, dirent->PathName );
            _strlwr( name );
            if( strcmp( check, name ) == 0 ) {
                found = true;
                break;
            }
            i++;
            chainptr += dinf.HeaderSize;
            chainptr += dirent->PathNameSize;
        }

        if( !found ) {
            RdosCloseDir( dir_handle );
            return( -1 );
        }

        attrib = 0;
        if( dirent->Attrib & FILE_ATTRIBUTE_ARCHIVE ) {
            attrib |= _A_ARCH;
        }
        if( dirent->Attrib & FILE_ATTRIBUTE_DIRECTORY ) {
            attrib |= _A_SUBDIR;
        }
        if( dirent->Attrib & FILE_ATTRIBUTE_HIDDEN ) {
            attrib |= _A_HIDDEN;
        }
        if( dirent->Attrib & FILE_ATTRIBUTE_NORMAL ) {
            attrib |= _A_NORMAL;
        }
        if( dirent->Attrib & FILE_ATTRIBUTE_READONLY ) {
            attrib |= _A_RDONLY;
        }
        if( dirent->Attrib & FILE_ATTRIBUTE_SYSTEM ) {
            attrib |= _A_SYSTEM;
        }
    }

    /* process drive number */
    if( path[1] == DRV_SEP ) {
        buf->st_dev = tolower( (UCHAR_TYPE)fullpath[0] ) - STRING( 'a' );
    } else {
        buf->st_dev = RdosGetCurDrive();
    }
    buf->st_rdev = buf->st_dev;

    if( dir_handle ) {
        buf->st_size = dirent->Size;
    } else {
        buf->st_size = 0;
    }
    buf->st_mode = at2mode( attrib, name );

    if( dir_handle ) {
        buf->st_ctime = __rdos_filetime_cvt( dirent->CreateTime );
        buf->st_mtime = __rdos_filetime_cvt( dirent->ModifyTime );
        buf->st_atime = __rdos_filetime_cvt( dirent->AccessTime );
        buf->st_ino = dirent->Inode;
        buf->st_uid = dirent->Uid;
        buf->st_gid = dirent->Gid;
        RdosCloseDir( dir_handle );
    } else {
        buf->st_ctime = -1;
        buf->st_mtime = -1;
        buf->st_atime = -1;
        buf->st_ino = -1;
        buf->st_uid = -1;
        buf->st_gid = -1;
    }

    buf->st_btime = -1;

    buf->st_nlink = 1;

    buf->st_attr = attrib;
    buf->st_archivedID = 0;
    buf->st_updatedID = 0;
    buf->st_inheritedRightsMask = 0;
    buf->st_originatingNameSpace = 0;

    return( 0 );
}
