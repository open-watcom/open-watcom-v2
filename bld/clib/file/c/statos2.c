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
* Description:  OS/2 implementation of stat().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#undef __INLINE_FUNCTIONS__
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <direct.h>
#include <dos.h>
#include <mbstring.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#else
    #include <ctype.h>
#endif
#define INCL_LONGLONG
#include "rterrno.h"
#include "i64.h"
#include "d2ttime.h"
#include "thread.h"
#include "os2fil64.h"
#include "find.h"


#define HAS_DRIVE(p)    (__F_NAME(isalpha,iswalpha)( (UCHAR_TYPE)p[0] ) && p[1] == STRING( ':' ))

static unsigned short at2mode( OS_UINT attr, char *fname ) {

    register unsigned short mode;
    register unsigned char  *ext;

    if( attr & _A_SUBDIR ) {
        mode = S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
    } else if( attr & 0x40 ) {
        mode = S_IFCHR;
    } else {
        mode = S_IFREG;
        /* determine if file is executable, very PC specific */
        if( (ext = _mbschr( (unsigned char *)fname, '.' )) != NULL ) {
            ++ext;
            if( _mbscmp( ext, (unsigned char *)"EXE" ) == 0 ) {
                mode |= S_IXUSR | S_IXGRP | S_IXOTH;
            }
        }
    }
    mode |= S_IRUSR | S_IRGRP | S_IROTH;
    if( !(attr & _A_RDONLY) )                   /* if file is not read-only */
        mode |= S_IWUSR | S_IWGRP | S_IWOTH;    /* - indicate writeable     */
    return( mode );
}


#ifdef __INT64__
 _WCRTLINK int __F_NAME(_stati64,_wstati64)( CHAR_TYPE const *path, struct _stati64 *buf )
#else
 _WCRTLINK int __F_NAME(stat,_wstat)( CHAR_TYPE const *path, struct __F_NAME(stat,_stat) *buf )
#endif
{
    CHAR_TYPE const     *ptr;
    ULONG               drvmap;
    OS_UINT             drive;
    APIRET              rc;
    CHAR_TYPE           fullpath[_MAX_PATH];
    int                 isrootdir = 0;

    /* reject null string and names that has wildcard */
#ifdef __WIDECHAR__
    if( *path == NULLCHAR || wcspbrk( path, STRING( "*?" ) ) != NULL ) {
#else
    if( *path == NULLCHAR || _mbspbrk( (unsigned char *)path, (unsigned char *)STRING( "*?" ) ) != NULL ) {
#endif
        _RWD_errno = ENOENT;
        return( -1 );
    }

    /*** Determine if 'path' refers to a root directory ***/
    if( __F_NAME(_fullpath,_wfullpath)( fullpath, path, _MAX_PATH ) != NULL ) {
        if( HAS_DRIVE( fullpath ) && fullpath[2] == STRING( '\\' ) && fullpath[3] == NULLCHAR ) {
            isrootdir = 1;
        }
    }

    ptr = path;
#ifdef __WIDECHAR__
    if( path[1] == STRING( ':' ) )
#else
    if( *_mbsinc( (unsigned char *)path ) == STRING( ':' ) )
#endif
        ptr += 2;
    if( ( ptr[0] == STRING( '\\' ) || ptr[0] == STRING( '/' ) ) && ptr[1] == NULLCHAR || isrootdir ) {
        /* handle root directory */
        int             drv;

        /* check if drive letter is valid */
        drv = __F_NAME(tolower,towlower)( (UCHAR_TYPE)*fullpath ) - STRING( 'a' );
        DosQCurDisk( &drive, &drvmap );
        if( ( drvmap & ( 1UL << drv ) ) == 0 ) {
            _RWD_errno = ENOENT;
            return( -1 );
        }
        /* set attributes */
        buf->st_attr = _A_SUBDIR;
        buf->st_mode = at2mode( _A_SUBDIR, "" );
        /* set timestamps */
        buf->st_ctime = 0;
        buf->st_atime = 0;
        buf->st_mtime = 0;
        buf->st_btime = 0;
        /* set size */
        buf->st_size = 0;
    } else {
        /* handle non-root directory */
        FF_BUFFER   dir_buff;
        HDIR        handle = HDIR_CREATE;
        OS_UINT     searchcount = 1;

#ifdef __WIDECHAR__
        char        mbPath[MB_CUR_MAX * _MAX_PATH];

        if( wcstombs( mbPath, path, sizeof( mbPath ) ) == -1 ) {
            mbPath[0] = '\0';
        }
#endif
        rc = DosFindFirst( (char*)__F_NAME(path,mbPath), &handle, FIND_ATTR,
                    &dir_buff, sizeof( dir_buff ), &searchcount, FF_LEVEL );
        if( rc == ERROR_FILE_NOT_FOUND ) { // appply a bit more persistence
            int handle;

            rc = 0;
            handle = __F_NAME(open,_wopen)( path, O_WRONLY );
            if( handle < 0 ) {
                _RWD_errno = ENOENT;
                return( -1 );
#ifdef __INT64__
            } else if( __F_NAME(_fstati64,_wfstati64)( handle, buf ) == -1 ) {
#else
            } else if( __F_NAME(fstat,_wfstat)( handle, buf ) == -1 ) {
#endif
                rc = _RWD_errno;
            }
            close( handle );
            _RWD_errno = rc;
            if( rc != 0 ) {
                return( -1 );
            }
            return( 0 );
        }
        if( rc == 0 ) {
            DosFindClose( handle );
        }
        if( rc != 0 || searchcount != 1 ) {
            _RWD_errno = ENOENT;
            return( -1 );
        }
        /* set timestamps */
        buf->st_ctime = _d2ttime( TODDATE( dir_buff.fdateCreation ),
                                  TODTIME( dir_buff.ftimeCreation ) );
        buf->st_atime = _d2ttime( TODDATE( dir_buff.fdateLastAccess ),
                                  TODTIME( dir_buff.ftimeLastAccess ) );
        buf->st_mtime = _d2ttime( TODDATE( dir_buff.fdateLastWrite ),
                                  TODTIME( dir_buff.ftimeLastWrite ) );
        buf->st_btime = buf->st_mtime;
#if defined( __INT64__ ) && !defined( _M_I86 )
        if( _FILEAPI64() ) {
#endif
            buf->st_attr = dir_buff.attrFile;
            buf->st_mode = at2mode( dir_buff.attrFile, dir_buff.achName );
            buf->st_size = dir_buff.cbFile;
#if defined( __INT64__ ) && !defined( _M_I86 )
        } else {
            buf->st_attr = ((FF_BUFFER_32 *)&dir_buff)->attrFile;
            buf->st_mode = at2mode( ((FF_BUFFER_32 *)&dir_buff)->attrFile, ((FF_BUFFER_32 *)&dir_buff)->achName );
            buf->st_size = ((FF_BUFFER_32 *)&dir_buff)->cbFile;
        }
#endif
    }

    /* process drive number */
#ifdef __WIDECHAR__
    if( path[1] == STRING( ':' ) ) {
#else
    if( *_mbsinc( (unsigned char *)path ) == STRING( ':' ) ) {
#endif
        buf->st_dev = __F_NAME(tolower,towlower)( (UCHAR_TYPE)*path ) - STRING( 'a' );
    } else {
        DosQCurDisk( &drive, &drvmap );
        buf->st_dev = drive - 1;
    }
    buf->st_rdev = buf->st_dev;

    buf->st_nlink = 1;
    buf->st_ino = buf->st_uid = buf->st_gid = 0;

    buf->st_archivedID = 0;
    buf->st_updatedID = 0;
    buf->st_inheritedRightsMask = 0;
    buf->st_originatingNameSpace = 0;

    return( 0 );
}
