/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Win32 implementation of stat().
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "variety.h"
#include "widechar.h"
#include "seterrno.h"
/* most includes should go after this line */
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <direct.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#else
    #include <ctype.h>
#endif
#include <dos.h>
#include <mbstring.h>
#include <windows.h>
#include "libwin32.h"
#include "ntext.h"
#include "osver.h"
#include "thread.h"
#include "pathmac.h"
#include "i64.h"


#define MAKE_SIZE64(__x,__hi,__lo)    ((unsigned_64 *)&__x)->u._32[I64LO32] = __lo; ((unsigned_64 *)&__x)->u._32[I64HI32] = __hi

static DWORD at2mode( DWORD attr, CHAR_TYPE *fname, CHAR_TYPE const *orig_path )
{
    DWORD               mode = 0L;
    CHAR_TYPE *         ext;
    DWORD               attrmask;

    if( WIN32_IS_NT ) {
        /*
         * Apparently the only way to tell that we are dealing with a device
         * (CON, LPT1, ...) is that they all have the ARCHIVE (0x20) bit set.
         * Thus, if the archive bit is set, we want to see if the "file" we
         * are dealing with is really a device in which case we want to set
         * the S_IFCHR flag in the mode.
         */
        attrmask = FILE_ATTRIBUTE_ARCHIVE;              /* NT */
    } else {
        /*
         * I don't know what this bit is called, but under Win95 it seems
         * to be set if and only if the file is a device.
         */
        attrmask = 0x40;                                /* Win95 */
    }

    if( attr & attrmask ) {
        HANDLE          osfh;
        ULONG           type;
        CHAR_TYPE const *tmp;

        /*
         * NT likes to refer to CON as CONIN$ or CONOUT$.
         */
        if( __F_NAME(_stricmp,_wcsicmp)( fname, STRING( "con" ) ) == 0 ) {
            tmp = STRING( "conin$" );
        } else {
            tmp = orig_path;  /* Need full name with path for CreateFile */
        }

        osfh = __lib_CreateFile( tmp, 0, 0, NULL, OPEN_EXISTING, 0, NULL );
        if( osfh != INVALID_HANDLE_VALUE ) {
            type = GetFileType( osfh );
            if( type == FILE_TYPE_CHAR ) {
                mode = S_IFCHR;
            } else if( type == FILE_TYPE_PIPE ) {
                mode = S_IFIFO;
            }
            CloseHandle( osfh );
        } else {
            if( WIN32_IS_NT ) {
                if( GetLastError() == ERROR_ACCESS_DENIED ) {
                    mode = S_IFCHR;
                }
            } else {
                /*
                 * Under Win95, GetLastError returns bogus values if we use
                 * it as we do under NT above.  Since we got this far, we
                 * know that something with the right name exists in the
                 * system; chances are that it's a device, since we can't
                 * open it as a file.  Yuck.
                 */
                mode = S_IFCHR;
            }
        }
    } /* if */

    if( attr & _A_SUBDIR ) {
        mode &= ~S_IFMT;
        mode |= S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
    } else if( (mode & S_IFCHR) == 0 && (mode & S_IFIFO) == 0 ) {
        /* name can't be a FIFO or character device and a regular file */
        mode |= S_IFREG;
        /* determine if file is executable, very PC specific */
        if( (ext = __F_NAME(strchr,wcschr)( fname, EXT_SEP )) != NULL ) {
            ++ext;
            if( __F_NAME(_stricmp,_wcsicmp)( ext, STRING( "exe" ) ) == 0 ) {
                mode |= S_IXUSR | S_IXGRP | S_IXOTH;
            }
        }
    }
    mode |= S_IRUSR | S_IRGRP | S_IROTH;
    if( (attr & _A_RDONLY) == 0 ) {
        mode |= S_IWUSR | S_IWGRP | S_IWOTH;
    }
    return( mode );
}


 #ifdef __INT64__
  _WCRTLINK int __F_NAME(_stati64,_wstati64)( const CHAR_TYPE *path, struct _stati64 *buf )
 #else
  _WCRTLINK int __F_NAME(stat,_wstat)( const CHAR_TYPE *path, struct stat *buf )
 #endif
{
    WIN32_FIND_DATA     ffd;
    const CHAR_TYPE     *ptr;
    CHAR_TYPE           cwd[_MAX_PATH];
    WORD                d,t;
    WORD                md,mt;
    HANDLE              osffh;
    CHAR_TYPE           fullpath[_MAX_PATH];
    int                 isrootdir = 0;

    /* reject null string and names that has wildcard */
#ifdef __WIDECHAR__
    if( *path == NULLCHAR || wcspbrk( path, L"*?" ) != NULL ) {
#else
    if( *path == NULLCHAR || _mbspbrk( (unsigned char *)path, (unsigned char *)"*?" ) != NULL ) {
#endif
        lib_set_errno( ENOENT );
        return( -1 );
    }

    __F_NAME(getcwd,_wgetcwd)( cwd, _MAX_PATH );

    /*** Determine if 'path' refers to a root directory ***/
    /* FindFirstFile can not be used on root directories! */
    if( __F_NAME(_fullpath,_wfullpath)( fullpath, path, _MAX_PATH ) != NULL ) {
        if( HAS_DRIVE( fullpath ) && fullpath[2] == DIR_SEP && fullpath[3] == NULLCHAR ) {
            isrootdir = 1;
        }
    }

    ptr = path;
#ifdef __WIDECHAR__
    if( path[1] == DRV_SEP )
#else
    if( *_mbsinc( (unsigned char *)path ) == DRV_SEP )
#endif
        ptr += 2;
    if( IS_DIR_SEP( ptr[0] ) && ptr[1] == NULLCHAR || isrootdir ) {
        /* check validity of specified root */
        if( __lib_GetDriveType( fullpath ) == DRIVE_UNKNOWN ) {
            lib_set_errno( ENOENT );
            return( -1 );
        }

        memset( &ffd, 0, sizeof( ffd ) );
        d = t = md = mt = 0;
        ffd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
    } else {
        osffh = __lib_FindFirstFile( path, &ffd );
        if( osffh == INVALID_HANDLE_VALUE ) {
            return( __set_errno_nt() );
        }
        FindClose( osffh );
    }

    /* process drive number */
#ifdef __WIDECHAR__
    if( path[1] == DRV_SEP ) {
#else
    if( *_mbsinc( (unsigned char *)path ) == DRV_SEP ) {
#endif
        buf->st_dev = (CHAR_TYPE)__F_NAME(tolower,towlower)( (UCHAR_TYPE)*path ) - STRING( 'a' );
    } else {
        buf->st_dev = (CHAR_TYPE)__F_NAME(tolower,towlower)( (UCHAR_TYPE)cwd[0] ) - STRING( 'a' );
    }
    buf->st_rdev = buf->st_dev;

#ifdef __INT64__
    MAKE_SIZE64( buf->st_size, ffd.nFileSizeHigh, ffd.nFileSizeLow );
#else
    buf->st_size = ffd.nFileSizeLow;
#endif
    buf->st_mode = at2mode( ffd.dwFileAttributes, ffd.cFileName, path );
    buf->st_mtime = __NT_filetime_to_timet( &ffd.ftLastWriteTime );
    buf->st_ctime = __NT_filetime_to_timet( &ffd.ftCreationTime );
    buf->st_atime = __NT_filetime_to_timet( &ffd.ftLastAccessTime );
    buf->st_nlink = 1;
    buf->st_ino = buf->st_uid = buf->st_gid = 0;

    buf->st_attr = ffd.dwFileAttributes;
    buf->st_archivedID = 0;
    buf->st_updatedID = 0;
    buf->st_inheritedRightsMask = 0;
    buf->st_originatingNameSpace = 0;
    return( 0 );
}
