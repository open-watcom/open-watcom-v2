/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS and Windows 3.x implementation of stat().
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <direct.h>
#include <dosfunc.h>
#include <mbstring.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#else
    #include <ctype.h>
#endif
#include "rtdata.h"
#include "rterrno.h"
#include "tinyio.h"
#include "_doslfn.h"
#include "_dtaxxx.h"
#include "dosdir.h"
#include "d2ttime.h"
#include "thread.h"
#include "pathmac.h"


#define ALL_ATTRIB      (_A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_SUBDIR | _A_ARCH)

#ifdef __INT64__

 _WCRTLINK int __F_NAME(_stati64,_wstati64)( CHAR_TYPE const *path, struct _stati64 *buf )
{
    struct stat         buf32;
    int                 rc;

    /*** Get the info using non-64bit version ***/
    rc = __F_NAME(stat,_wstat)( path, &buf32 );
    if( rc != -1 ) {
        /*** Convert the info to 64-bit equivalent ***/
        buf->st_dev = buf32.st_dev;
        buf->st_ino = buf32.st_ino;
        buf->st_mode = buf32.st_mode;
        buf->st_nlink = buf32.st_nlink;
        buf->st_uid = buf32.st_uid;
        buf->st_gid = buf32.st_gid;
        buf->st_rdev = buf32.st_rdev;
        buf->st_size = (unsigned long)buf32.st_size;
        buf->st_atime = buf32.st_atime;
        buf->st_mtime = buf32.st_mtime;
        buf->st_ctime = buf32.st_ctime;
        buf->st_btime = buf32.st_btime;
        buf->st_attr = buf32.st_attr;
        buf->st_archivedID = buf32.st_archivedID;
        buf->st_updatedID = buf32.st_updatedID;
        buf->st_inheritedRightsMask = buf32.st_inheritedRightsMask;
        buf->st_originatingNameSpace = buf32.st_originatingNameSpace;
    }
    return( rc );
}

#else

static unsigned short at2mode( int attr, char *fname )
/****************************************************/
{
    unsigned short  mode;
    unsigned char   *ext;

    if( attr & _A_SUBDIR ) {
        mode = S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
    } else if( attr & AT_ISCHR ) {
        mode = S_IFCHR;
    } else {
        mode = S_IFREG;
        /* determine if file is executable, very PC specific */
        if( (ext = _mbschr( (unsigned char *)fname, '.' )) != NULL ) {
            ++ext;
            if( _mbscmp( ext, (unsigned char *)"EXE" ) == 0 || _mbscmp( ext, (unsigned char *)"COM" ) == 0 ) {
                mode |= S_IXUSR | S_IXGRP | S_IXOTH;
            }
        }
    }
    mode |= S_IRUSR | S_IRGRP | S_IROTH;
    if( !(attr & _A_RDONLY) )                   /* if file is not read-only */
        mode |= S_IWUSR | S_IWGRP | S_IWOTH;    /* - indicate writeable     */
    return( mode );
}

_WCRTLINK int __F_NAME(stat,_wstat)( CHAR_TYPE const *path, struct stat *buf )
/****************************************************************************/
{
    struct find_t       fdta;
    const CHAR_TYPE     *ptr;
    unsigned            rc;
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
        if( HAS_DRIVE( fullpath ) && fullpath[2] == DIR_SEP && fullpath[3] == NULLCHAR ) {
            isrootdir = 1;
        }
    }

    ptr = path;
    if( HAS_DRIVE( path ) )
        ptr += 2;
    if( IS_DIR_SEP( ptr[0] ) && ptr[1] == NULLCHAR || isrootdir ) {
        /* handle root directory */
        CHAR_TYPE       cwd[_MAX_PATH];

        /* save current directory */
        __F_NAME(getcwd,_wgetcwd)( cwd, _MAX_PATH );

        /* try to change to specified root */
        if( __F_NAME(chdir,_wchdir)( path ) != 0 )
            return( -1 );

        /* restore current directory */
        __F_NAME(chdir,_wchdir)( cwd );

        fdta.attrib    = _A_SUBDIR;      /* fill in DTA */
        fdta.wr_time   = 0;
        fdta.wr_date   = 0;
        fdta.size      = 0;
        fdta.name[0]   = '\0';
#ifdef __WATCOM_LFN__
        DTALFN_SIGN_OF( fdta.reserved )   = 0;
        DTALFN_HANDLE_OF( fdta.reserved ) = 0;
#endif
    } else {                            /* not a root directory */
#if defined(__WIDECHAR__)
        char    mbPath[MB_CUR_MAX * _MAX_PATH];

        if( wcstombs( mbPath, path, sizeof( mbPath ) ) == (size_t)-1 ) {
            mbPath[0] = '\0';
        }
#endif
        if( _dos_findfirst( __F_NAME(path,mbPath), ALL_ATTRIB, &fdta ) != 0 ) {
            int         handle;
            int         canread = 0;
            int         canwrite = 0;
            int         fstatok = 0;

            // Try getting information another way.
            rc = 0;
            handle = __F_NAME(open,_wopen)( path, O_WRONLY );
            if( handle != -1 ) {
                canwrite = 1;
                if( fstat( handle, buf ) == -1 ) {
                    rc = _RWD_errno;
                } else {
                    fstatok = 1;
                }
            }
            close( handle );
            handle = __F_NAME(open,_wopen)( path, O_RDONLY );
            if( handle != -1 ) {
                canread = 1;
                if( !fstatok ) {
                    if( fstat( handle, buf ) == -1 ) {
                        rc = _RWD_errno;
                    }
                }
            }
            close( handle );
            if( !canread && !canwrite ) {
                _RWD_errno = ENOENT;
                return( -1 );
            }
            _RWD_errno = rc;
            if( rc != 0 ) {
                return( -1 );
            }
            if( canread ) {
                buf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
            }
            if( canwrite ) {
                buf->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
            }
            return( 0 );
        }
        _dos_findclose( &fdta );
    }

    /* process drive number */
    if( HAS_DRIVE( path ) ) {
        buf->st_dev = (CHAR_TYPE)__F_NAME(tolower,towlower)( (UCHAR_TYPE)*path ) - STRING( 'a' );
    } else {
        buf->st_dev = TinyGetCurrDrive();
    }
    buf->st_rdev = buf->st_dev;

    buf->st_size = fdta.size;
    buf->st_mode = at2mode( fdta.attrib, fdta.name );

    buf->st_mtime = _d2ttime( fdta.wr_date, fdta.wr_time );
    buf->st_btime = buf->st_mtime;
#ifdef __WATCOM_LFN__
    if( IS_LFN( fdta.reserved ) && DTALFN_CRTIME_OF( fdta.reserved ) ) {
        buf->st_atime = _d2ttime( DTALFN_ACDATE_OF( fdta.reserved ), DTALFN_ACTIME_OF( fdta.reserved ) );
        buf->st_ctime = _d2ttime( DTALFN_CRDATE_OF( fdta.reserved ), DTALFN_CRTIME_OF( fdta.reserved ) );
    } else {
#endif
        buf->st_atime = buf->st_mtime;
        buf->st_ctime = buf->st_mtime;
#ifdef __WATCOM_LFN__
    }
#endif
    buf->st_nlink = 1;
    buf->st_ino = buf->st_uid = buf->st_gid = 0;

    buf->st_attr = fdta.attrib;
    buf->st_archivedID = 0;
    buf->st_updatedID = 0;
    buf->st_inheritedRightsMask = 0;
    buf->st_originatingNameSpace = 0;

    return( 0 );
}

#endif
