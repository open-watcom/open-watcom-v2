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


#include "variety.h"
#include "int64.h"
#include "widechar.h"
#include <windows.h>
#undef __INLINE_FUNCTIONS__
/* gross hack for building 11.0 libraries with 10.6 compiler */
#ifndef __WATCOM_INT64__
    #include <limits.h>         /* a gross hack to make a gross hack work */
    #define __WATCOM_INT64__
    #define __int64             double
#endif
/* most includes should go after this line */
#include <stddef.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <errno.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>
#include <dos.h>
#include <mbstring.h>
#include "libwin32.h"
#include "rtdata.h"
#include "ntex.h"
#include "osver.h"
#include "seterrno.h"

extern time_t _d2ttime();

static DWORD at2mode( DWORD attr, CHAR_TYPE *fname )
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
        HANDLE          h;
        ULONG           type;
        CHAR_TYPE *     tmp;

        /*
         * NT likes to refer to CON as CONIN$ or CONOUT$.
         */
        if( !__F_NAME(stricmp,_wcsicmp)( fname, __F_NAME("con",L"con") ) )
            tmp = __F_NAME("conin$",L"conin$");
        else
            tmp = fname;

        #ifdef __WIDECHAR__
            h = __lib_CreateFileW( tmp, 0, 0, NULL, OPEN_EXISTING, 0, NULL );
        #else
            h = CreateFileA( tmp, 0, 0, NULL, OPEN_EXISTING, 0, NULL );
        #endif
        if( h != INVALID_HANDLE_VALUE ) {
            type = GetFileType(h);
            if( type == FILE_TYPE_CHAR ) {
                mode = S_IFCHR;
            } else if( type == FILE_TYPE_PIPE ) {
                mode = S_IFFIFO;
            }
            CloseHandle(h);
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
    } else if( !(mode&S_IFCHR) && !(mode&S_IFFIFO) ) {
        /* name can't be a FIFO or character device and a regular file */
        mode |= S_IFREG;
        /* determine if file is executable, very PC specific */
        if( (ext = __F_NAME(strchr,wcschr)( fname, __F_NAME('.',L'.') )) != NULL ) {
            ++ext;
            if( __F_NAME(strcmp,wcscmp)( ext, __F_NAME("EXE",L"EXE") ) == 0 ) {
                mode |= S_IXUSR | S_IXGRP | S_IXOTH;
            }
        }
    }
    mode |= S_IRUSR | S_IRGRP | S_IROTH;
    if( !(attr & _A_RDONLY) ) {
        mode |= S_IWUSR | S_IWGRP | S_IWOTH;
    }
    return( mode );
}


#ifdef __WIDECHAR__
 #ifdef __INT64__
  _WCRTLINK int _wstati64( wchar_t const *path, struct _wstati64 *buf )
 #else
  _WCRTLINK int _wstat( wchar_t const *path, struct _wstat *buf )
 #endif
#else
 #ifdef __INT64__
  _WCRTLINK int _stati64( char const *path, struct _stati64 *buf )
 #else
  _WCRTLINK int stat( char const *path, struct stat *buf )
 #endif
#endif
{
    WIN32_FIND_DATA     ffb;
    CHAR_TYPE const *   ptr;
    CHAR_TYPE           cwd[_MAX_PATH];
    WORD                d,t;
    WORD                md,mt;
    HANDLE              h;
    CHAR_TYPE           fullpath[_MAX_PATH];
    int                 isrootdir = 0;

    /* reject null string and names that has wildcard */
    #ifdef __WIDECHAR__
    if( *path == L'\0' || wcspbrk( path, L"*?" ) != NULL )
    #else
    if( *path == '\0' || _mbspbrk( path, "*?" ) != NULL )
    #endif
    {
        __set_errno( ENOENT );
        return( -1 );
    }

    __F_NAME(getcwd,_wgetcwd)( cwd, _MAX_PATH );

    /*** Determine if 'path' refers to a root directory ***/
    if( __F_NAME(_fullpath,_wfullpath)( fullpath, path, _MAX_PATH ) != NULL ) {
        #ifdef __WIDECHAR__
        if( iswalpha( fullpath[0] )  &&  fullpath[1] == L':'  &&
            fullpath[2] == L'\\'  &&  fullpath[3] == L'\0' )
        #else
        if( isalpha( fullpath[0] )  &&  fullpath[1] == ':'  &&
            fullpath[2] == '\\'  &&  fullpath[3] == '\0' )
        #endif
        {
            isrootdir = 1;
        }
    }

    ptr = path;
    if( __F_NAME(*_mbsinc(path),path[1]) == __F_NAME(':',L':') )  ptr += 2;
    #ifdef __WIDECHAR__
    if( ( (ptr[0] == L'\\' || ptr[0] == L'/') && ptr[1] == L'\0' )  ||  isrootdir )
    #else
    if( ( (ptr[0] == '\\' || ptr[0] == '/') && ptr[1] == '\0' )  ||  isrootdir )
    #endif
    {
        /* try to change to specified root */
        if( __F_NAME(chdir,_wchdir)( path ) != 0 )  return( -1 );

        /* restore current directory */
        __F_NAME(chdir,_wchdir)( cwd );
        memset( &ffb, 0, sizeof( ffb ) );
        ffb.dwFileAttributes = _A_SUBDIR;
    } else {
        h = __F_NAME(FindFirstFileA,__lib_FindFirstFileW)( path, &ffb );
        if( h == INVALID_HANDLE_VALUE ) {
            return( __set_errno_nt() );
        }
        FindClose( h );
    }

    /* process drive number */
    if( __F_NAME(*_mbsinc(path),path[1]) == __F_NAME(':',L':') ) {
        buf->st_dev = __F_NAME(tolower,towlower)( *path ) - __F_NAME('a',L'a');
    } else {
        buf->st_dev = __F_NAME(tolower,towlower)( cwd[0] ) - __F_NAME('a',L'a');
    }
    buf->st_rdev = --(buf->st_dev);

    #ifdef __INT64__
    {
        INT_TYPE        tmp;

        MAKE_INT64(tmp,ffb.nFileSizeHigh,ffb.nFileSizeLow);
        buf->st_size = GET_REALINT64(tmp);
    }
    #else
        buf->st_size = ffb.nFileSizeLow;
    #endif
    buf->st_mode = at2mode( ffb.dwFileAttributes, ffb.cFileName );
    __MakeDOSDT( &ffb.ftLastWriteTime, &md, &mt );
    buf->st_mtime = _d2ttime( md, mt );
    __MakeDOSDT( &ffb.ftCreationTime, &d, &t );
    if( d == md && t == mt ) {
        buf->st_ctime = buf->st_mtime;
    } else {
        buf->st_ctime = _d2ttime( d, t );
    }
    __MakeDOSDT( &ffb.ftLastAccessTime, &d, &t );
    if( d == md && t == mt ) {
        buf->st_atime = buf->st_mtime;
    } else {
        buf->st_atime = _d2ttime( d, t );
    }
    buf->st_nlink = 1;
    buf->st_ino = buf->st_uid = buf->st_gid = 0;

    buf->st_attr = ffb.dwFileAttributes;
    buf->st_archivedID = 0;
    buf->st_updatedID = 0;
    buf->st_inheritedRightsMask = 0;
    buf->st_originatingNameSpace = 0;
    __F_NAME(strncpy,wcsncpy)( buf->st_name, ffb.cFileName, _MAX_NAME );
    return( 0 );
}
