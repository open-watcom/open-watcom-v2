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
#include "widechar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#if defined(__QNX__)
    #include <dirent.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/io_msg.h>
    #include <fcntl.h>
#else
    #include <direct.h>
    #if defined(__OS2__)
        #include <wos2.h>
    #elif defined(__NT__)
        #include <windows.h>
        #include "libwin32.h"
    #elif !defined(__NETWARE__)
        #include "tinyio.h"
    #endif
#endif
#include "liballoc.h"
#include "rtdata.h"
#include "seterrno.h"
#if defined(__WIDECHAR__) || defined(__WARP__)
    #include <mbstring.h>
#endif


#define _WILL_FIT( c )  if(( (c) + 1 ) > size ) {       \
                            __set_errno( ERANGE );      \
                            return( NULL );             \
                        }                               \
                        size -= (c);

#define _IS_SLASH( c )  (( (c) == '/' ) || ( (c) == '\\' ))

#if !defined( __NT__ ) && !defined( __NETWARE__ )
#pragma on (check_stack);
#endif

#ifdef __NETWARE__
extern char *ConvertNameToFullPath( const char *, char * );
#endif

#if defined(__QNX__)
static char *my_qnx_fullpath(char *fullpath, const char *path)
{
    struct {
            struct _io_open _io_open;
            char  m[_QNX_PATH_MAX];
    } msg;
    int             fd;

    msg._io_open.oflag = _IO_HNDL_INFO;
    fd = __resolve_net( _IO_HANDLE, 1, &msg._io_open, path, 0, fullpath );
    if( fd != -1) {
        close(fd);
    } else if (errno != ENOENT) {
        return 0;
    } else {
        __resolve_net( 0, 0, &msg._io_open, path, 0, fullpath );
    }
    return fullpath;
}
#endif


#if 0
#if defined(__NT__) || defined(__WARP__)

/*
 * Try to find the exact case for the fully qualified filename given in
 * 'buff'.  If successful, the proper name will be in 'buff' (of size
 * 'size') on return.  If unsuccessful, the original name will be unmodified
 * in 'buff'.  There is no indication of success versus failure because
 * fix_case is purely cosmetic; NTFS and HPFS preserve case, but are case-
 * insensitive, so the full pathname passed to this function would still
 * be sufficient.
 *
 * The 'filepart' parameter is either NULL (in which case it will be
 * calculated) or else points the first character of the filename portion
 * of the input pathname.
 */
static void fix_case( CHAR_TYPE *buff, size_t size, CHAR_TYPE *filepart )
{
    #ifdef __NT__
        WIN32_FIND_DATA findbuf;
        HANDLE          handle;
        CHAR_TYPE *     curdir;
    #else
        APIRET          rc;
        FILEFINDBUF3    findbuf;
        HDIR            handle = HDIR_CREATE;
        ULONG           searchcount = 1;
        #ifdef __WIDECHAR__
            char *      mbBuff;
            size_t      cvt;
        #endif
    #endif
    CHAR_TYPE *         tmpbuff;
    CHAR_TYPE           ch;
    size_t              len;

    #ifdef __NT__
        /*** Always initialized under NT, so avoid code bloat ***/
        if( filepart == NULL ) {
            return;             /* shouldn't happen, but play it safe */
        }
    #else
        /*** Initialize filepart if it wasn't supplied ***/
        if( filepart == NULL ) {
            filepart = __F_NAME(_mbsrchr,wcsrchr)( buff, '\\' );
            if( filepart == NULL ) {
                return;         /* shouldn't happen, but play it safe */
            }
            filepart++;
        }
    #endif

    /*** Make a temporary buffer ***/
    tmpbuff = lib_malloc( _MAX_PATH * CHARSIZE );
    if( tmpbuff == NULL ) {
        return;
    }

    #ifdef __NT__
        /*
         * Note: This part of the code is broken.  It may also be necessary
         * to change drives, which is gross.  Additionally, there may be
         * issues with things like the full path of a:\foo.bar when there's
         * no disk in drive A, and similar problems for CD-ROM drives.
         * It seems to work ok if the directory is on the current drive,
         * though.
         *
         * This whole exact case thing is gross.  Yuck.
         */

        /*** Remember the current directory ***/
        curdir = __F_NAME(getcwd,_wgetcwd)( NULL, 0 );
        if( curdir == NULL ) {
            lib_free( tmpbuff );
            return;
        }

        /*** Get the correct case for the path portion ***/
        ch = *(filepart-1);                     /* smite backslash */
        *(filepart-1) = NULLCHAR;
        if( __F_NAME(chdir,_wchdir)( buff ) == -1 ) {
            __F_NAME(chdir,_wchdir)( curdir );
            free( curdir );
            lib_free( tmpbuff );
            return;
        }
        if( __F_NAME(getcwd,_wgetcwd)( tmpbuff, _MAX_PATH ) == NULL ) {
            __F_NAME(chdir,_wchdir)( curdir );
            free( curdir );
            lib_free( tmpbuff );
            return;
        }
        __F_NAME(strcat,wcscat)( tmpbuff, __F_NAME("\\",L"\\") );
        *(filepart-1) = ch;                     /* restore backslash */
        __F_NAME(chdir,_wchdir)( curdir );
        free( curdir );
    #else
        /*** For some reason the above technique doesn't work under OS/2 ***/
        ch = *(filepart-1);                     /* smite backslash */
        *(filepart-1) = NULLCHAR;
        __F_NAME(strcpy,wcscpy)( tmpbuff, buff );
        __F_NAME(strcat,wcscat)( tmpbuff, __F_NAME("\\",L"\\") );
        *(filepart-1) = ch;                     /* restore backslash */
    #endif

    /*** Get the correct case for the filename portion ***/
    #ifdef __NT__
        #ifdef __WIDECHAR__
            handle = __lib_FindFirstFileW( buff, &findbuf );
        #else
            handle = FindFirstFileA( buff, &findbuf );
        #endif
        if( handle == INVALID_HANDLE_VALUE ) {
            lib_free( tmpbuff );
            return;
        }
        __F_NAME(strcat,wcscat)( tmpbuff, findbuf.cFileName );
        FindClose( handle );
    #else
        #ifdef __WIDECHAR__
            mbBuff = lib_malloc( _MAX_PATH );
            if( mbBuff == NULL ) {
                lib_free( tmpbuff );
                return;
            }
            cvt = wcstombs( mbBuff, buff, _MAX_PATH );
            if( cvt == (size_t)-1 ) {
                lib_free( tmpbuff );
                return;
            }
            rc = DosFindFirst( mbBuff, &handle, 0, &findbuf,
                               sizeof( findbuf ), &searchcount,
                               FIL_STANDARD );
            lib_free( mbBuff );
        #else
            rc = DosFindFirst( buff, &handle, 0, &findbuf, sizeof( findbuf ),
                               &searchcount, FIL_STANDARD );
        #endif
        if( rc != NO_ERROR ) {
            lib_free( tmpbuff );
            return;
        }
        DosFindClose( handle );
        #ifdef __WIDECHAR__
            cvt = mbstowcs( tmpbuff, findbuf.achName,
                            _MAX_PATH - wcslen( tmpbuff ) );
            if( cvt == (size_t)-1 ) {
                lib_free( tmpbuff );
                return;
            }
        #else
            strcat( tmpbuff, findbuf.achName );
        #endif
    #endif

    len = __F_NAME(strlen,wcslen)( tmpbuff ) + 1;
    if( len <= size ) {
        __F_NAME(strcpy,wcscpy)( buff, tmpbuff );
        lib_free( tmpbuff );
    }
}

#endif  /* defined(__NT__) || defined(__WARP__) */
#endif  /* 0 */

_WCRTLINK CHAR_TYPE *__F_NAME(_sys_fullpath,_sys_wfullpath)
                ( CHAR_TYPE *buff, const CHAR_TYPE *path, size_t size )
/*********************************************************************/
{

#if defined(__NT__)
    CHAR_TYPE *         filepart;
    DWORD               rc;

    if( __F_NAME(stricmp,_wcsicmp)( path, __F_NAME("con",L"con") ) == 0 ) {
        _WILL_FIT( 3 );
        return( __F_NAME(strcpy,wcscpy)( buff, __F_NAME("con",L"con") ) );
    }

    /*** Get the full pathname ***/
    #ifdef __WIDECHAR__
        rc = __lib_GetFullPathNameW( path, size, buff, &filepart );
    #else
        rc = GetFullPathNameA( path, size, buff, &filepart );
    #endif
    // If the buffer is too small, the return value is the size of
    // the buffer, in TCHARs, required to hold the path.
    // If the function fails, the return value is zero. To get extended error
    // information, call GetLastError.
    if( (rc == 0) || (rc > size) ) {
        __set_errno_nt();
        return( NULL );
    }
    #if 0
        fix_case( buff, size, filepart );
    #endif

    return( buff );
#elif defined(__WARP__)
    APIRET      rc;
    char        root[4];    /* SBCS: room for drive, ':', '\\', and null */
    #ifdef __WIDECHAR__
        char    mbBuff[_MAX_PATH*MB_CUR_MAX];
        char    mbPath[_MAX_PATH*MB_CUR_MAX];
    #endif

    if (__F_NAME(isalpha,iswalpha)( path[0] ) && ( path[1] == ':' )
            && ( path[2] == '\\' ) )
    {
        int i;
        i = __F_NAME(strlen,wcslen)( path );
        _WILL_FIT(i);
        __F_NAME(strcpy,wcscpy)( buff, path );
        #if 0
            fix_case( buff, size, NULL );
        #endif
        return( buff );
    }

    /*
     * Check for x:filename.ext when drive x doesn't exist.  In this
     * case, return x:\filename.ext, not NULL, to be consistent with
     * MS and with the NT version of _fullpath.
     */
    if( __F_NAME(isalpha,iswalpha)( path[0] )  &&  path[1] == ':' ) {
        /*** We got this far, so path can't start with letter:\ ***/
        root[0] = (char) path[0];
        root[1] = ':';
        root[2] = '\\';
        root[3] = NULLCHAR;
        rc = DosQueryPathInfo( root, FIL_QUERYFULLNAME, buff, size );
        if( rc != NO_ERROR ) {
            /*** Drive does not exist; return x:\filename.ext ***/
            _WILL_FIT( __F_NAME(_mbslen,wcslen)( &path[2] ) + 3 );
            buff[0] = root[0];
            buff[1] = ':';
            buff[2] = '\\';
            __F_NAME(_mbscpy,wcscpy)( &buff[3], &path[2] );
            return( buff );
        }
    }

    #ifdef __WIDECHAR__
        if( wcstombs( mbPath, path, _MAX_PATH*MB_CUR_MAX ) == (size_t)-1 ) {
            return( NULL );
        }
        rc = DosQueryPathInfo( (PSZ)mbPath, FIL_QUERYFULLNAME, mbBuff, size );
    #else
        rc = DosQueryPathInfo( (PSZ)path, FIL_QUERYFULLNAME, buff, size );
    #endif
    if( rc != 0 ) {
        __set_errno_dos( rc );
        return( NULL );
    }
    #ifdef __WIDECHAR__
        if( mbstowcs( buff, mbBuff, size ) != (size_t)-1 ) {
            #if 0
                fix_case( buff, size, NULL );
            #endif
            return( buff );
        } else {
            return( NULL );
        }
    #else
        #if 0
            fix_case( buff, size, NULL );
        #endif
        return( buff );
    #endif
#elif defined(__QNX__) || defined( __NETWARE__ )
    size_t len;
    char curr_dir[_MAX_PATH];

    #ifdef __NETWARE__
        if( ConvertNameToFullPath( path, curr_dir ) != 0 ) {
            return( NULL );
        }
    #else
        if( my_qnx_fullpath( curr_dir, path ) == NULL ) {
            return( NULL );
        }
    #endif
    len = strlen( curr_dir );
    if( len >= size ) {
        __set_errno( ERANGE );
        return( NULL );
    }
    return( strcpy( buff, curr_dir ) );
#else
    const CHAR_TYPE *   p;
    CHAR_TYPE *         q;
    size_t              len;
    unsigned            path_drive_idx;
    char                curr_dir[_MAX_PATH];

    p = path;
    q = buff;
    _WILL_FIT( 2 );
    if( __F_NAME(isalpha,iswalpha)( p[0] ) && p[1] == ':' ) {
        path_drive_idx = ( __F_NAME(tolower,towlower)( p[0] ) - 'a' ) + 1;
        q[0] = p[0];
        q[1] = p[1];
        p += 2;
    } else {
  #if defined(__OS2__)
        ULONG   drive_map;
        OS_UINT os2_drive;

        if( DosQCurDisk( &os2_drive, &drive_map ) ) {
            __set_errno( ENOENT );
            return( NULL );
        }
        path_drive_idx = os2_drive;
  #else
        path_drive_idx = TinyGetCurrDrive() + 1;
  #endif
        q[0] = 'A' + ( path_drive_idx - 1 );
        q[1] = ':';
    }
    q += 2;
    if( ! _IS_SLASH( p[0] ) ) {
  #if defined(__OS2__)
        OS_UINT dir_len = sizeof( curr_dir );

        if( DosQCurDir( path_drive_idx, curr_dir, &dir_len ) ) {
            __set_errno( ENOENT );
            return( NULL );
        }
  #else
        tiny_ret_t rc;

        rc = TinyGetCWDir( curr_dir, path_drive_idx );
        if( TINY_ERROR( rc ) ) {
            __set_errno( ENOENT );
            return( NULL );
        }
  #endif
        len = __F_NAME(strlen,_mbslen)( curr_dir );
        if( curr_dir[0] != '\\' ) {
            _WILL_FIT( 1 );
            *(q++) = '\\';
        }
        _WILL_FIT( len );
        #ifdef __WIDECHAR__
            if( mbstowcs( q, curr_dir, len+1 ) == (size_t)-1 ) {
                return( NULL );
            }
        #else
            strcpy( q, curr_dir );
        #endif
        q += len;
        if( q[-1] != '\\' ) {
            _WILL_FIT( 1 );
            *(q++) = '\\';
        }
        for(;;) {
            if( p[0] == '\0' ) break;
            if( p[0] != '.' ) {
                _WILL_FIT( 1 );
                *(q++) = *(p++);
                continue;
            }
            ++p;     // at least '.'
            if( _IS_SLASH( p[0] ) ) {
                /* ignore "./" in directory specs */
                if( ! _IS_SLASH( q[-1] ) ) {            /* 14-jan-93 */
                    *q++ = '\\';
                }
                ++p;
                continue;
            }
            if( p[0] == '\0' ) break;
            if( p[0] == '.' ) {  /* .. */
                ++p;
                if( _IS_SLASH( p[0] ) ){ /* "../" */
                    ++p;
                }
                if( ! _IS_SLASH( q[-1] ) ) {
                    return( NULL );
                }
                q -= 2;
                for(;;) {
                    if( q < buff ) {
                        return( NULL );
                    }
                    if( _IS_SLASH( *q ) ) break;
                    if( *q == ':' ) {
                        ++q;
                        *q = '\\';
                        break;
                    }
                    --q;
                }
                ++q;
                *q = '\0';
                continue;
            }
            _WILL_FIT( 1 );
            *(q++) = '.';
        }
        *q = '\0';
    } else {
        len = __F_NAME(strlen,wcslen)( p );
        _WILL_FIT( len );
        __F_NAME(strcpy,wcscpy)( q, p );
    }
    /* force to all backslashes */
    for( q = buff; *q; ++q ) {
        if( *q == '/' ) {
            *q = '\\';
        }
    }
    return( buff );
#endif
}

_WCRTLINK CHAR_TYPE *__F_NAME(_fullpath,_wfullpath)
                ( CHAR_TYPE *buff, const CHAR_TYPE *path, size_t size )
/*******************************************************************/
{
    CHAR_TYPE *ptr = NULL;

    if( buff == NULL ) {
        #ifdef __WIDECHAR__
            size = _MAX_PATH * sizeof(wchar_t);
        #else
            size = _MAX_PATH;
        #endif
        ptr = lib_malloc( size );
        if( ptr == NULL ) __set_errno( ENOMEM );
        buff = ptr;
    }
    if( buff != NULL ) {
        buff[0] = '\0';
        if( path == NULL || path[0] == '\0' ) {
            buff = __F_NAME(getcwd,_wgetcwd)( buff, size );
        } else {
            buff = __F_NAME(_sys_fullpath,_sys_wfullpath)( buff, path, size );
        }
        if( buff == NULL ) {
            if( ptr != NULL ) lib_free( ptr );
        }
    }
    return buff;
}
