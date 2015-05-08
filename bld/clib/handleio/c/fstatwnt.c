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
* Description:  Win32 implementation of fstat().
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stddef.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <windows.h>
#include "rtdata.h"
#include "int64.h"
#include "defwin.h"
#include "iomode.h"
#include "fileacc.h"
#include "ntext.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "d2ttime.h"
#include "errorno.h"
#include "thread.h"


/*
    DWORD GetFileSize(
      HANDLE hFile,           // handle to file
      LPDWORD lpFileSizeHigh  // high-order word of file size
    );
 */

#ifdef __INT64__
 _WCRTLINK int _fstati64( int hid, struct _stati64 *buf )
#else
 _WCRTLINK int fstat( int hid, struct stat *buf )
#endif
{
    DWORD                       size;
#ifdef __INT64__
    DWORD                       highorder;
    INT_TYPE                    tmp;
    int                         error;
#endif
    WORD                        d;
    WORD                        t;
    WORD                        md;
    WORD                        mt;
    DWORD                       ftype;
    FILETIME                    ctime, atime, mtime;
    HANDLE                      h;
    unsigned                    iomode_flags;
    BOOL                        osrc;
    BY_HANDLE_FILE_INFORMATION  fileinfo;

    __handle_check( hid, -1 );
    h = __getOSHandle( hid );
    _AccessFileH( hid );

    buf->st_mode = 0;                           /* 12-apr-94 */
#ifdef DEFAULT_WINDOWING
    if( _WindowsIsWindowedHandle != 0 ) {
        if( _WindowsIsWindowedHandle( hid ) != 0 ) {
            buf->st_mode |= S_IFCHR;        /* it's a console */
        }
    }
#endif
    __ChkTTYIOMode( hid );
    iomode_flags = __GetIOMode( hid );
    if( iomode_flags & _READ ) {
        buf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
    }
    if( iomode_flags & _WRITE ) {
        buf->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
    }
    ftype = GetFileType( h );
    if( (iomode_flags & _ISTTY) || /* for default windows */
        (ftype == FILE_TYPE_CHAR) ||
        (ftype == FILE_TYPE_PIPE) ||
         /*
            FILE_TYPE_UNKNOWN is returned when standard output handle
            is specified and it's not a console application - also we
            don't want to call GetFileSize()
         */
        (ftype == FILE_TYPE_UNKNOWN) ) {
#ifdef __INT64__
        _clib_U32ToU64( 0L, tmp );
        buf->st_size = GET_REALINT64(tmp);
#else
        buf->st_size = 0;
#endif
        buf->st_atime = buf->st_ctime = buf->st_mtime = 0;
        buf->st_attr = 0;
        buf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
        if( ftype == FILE_TYPE_PIPE ) {
            buf->st_mode |= S_IFIFO;
        } else if( ftype == FILE_TYPE_CHAR ) {
            buf->st_mode |= S_IFCHR;
        }
        buf->st_dev = buf->st_rdev = 1;
    } else {
        /*** Try to get attributes (can reasonably fail; see Win32 docs) ***/
        osrc = GetFileInformationByHandle( h, &fileinfo );
        if( osrc == TRUE ) {
            buf->st_attr = fileinfo.dwFileAttributes;
            if( fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                buf->st_mode |= S_IFDIR;
            } else {
                buf->st_mode |= S_IFREG;
            }
        } else {
            buf->st_attr = 0;           /* cannot be determined */
        }

        /*** Get the file size ***/
        if( buf->st_mode & S_IFDIR ) {
#ifdef __INT64__
            _clib_U32ToU64( 0L, tmp );
            buf->st_size = GET_REALINT64(tmp);
#else
            buf->st_size = 0;
#endif
        } else {
            size = GetFileSize( h, __I64NAME(NULL,&highorder) );
#ifdef __INT64__
            if( size == -1 ) {
                error = GetLastError();     // check for sure JBS 05-nov-99
                if( error != NO_ERROR ) {
                    _ReleaseFileH( hid );
                    return( __set_errno_dos( error ) );
                }
            }
            MAKE_INT64(tmp,highorder,size);
            buf->st_size = GET_REALINT64(tmp);
#else
            if( size == -1 ) {
                _ReleaseFileH( hid );
                return( __set_errno_nt() );
            }
            buf->st_size = size;
#endif
        }

        /*** Get the file time ***/
        if( !GetFileTime( h, &ctime, &atime, &mtime ) ) {
            _ReleaseFileH( hid );
            return( __set_errno_nt() );
        }
        __MakeDOSDT( &mtime, &md, &mt );
        buf->st_mtime = _d2ttime( md, mt );
        if(( ctime.dwLowDateTime == mtime.dwLowDateTime ) &&
           ( ctime.dwHighDateTime == mtime.dwHighDateTime )) {
            buf->st_ctime = buf->st_mtime;
        } else {
            __MakeDOSDT( &ctime, &d, &t );
            if( d == md && t == mt ) {
                buf->st_ctime = buf->st_mtime;
            } else {
                buf->st_ctime = _d2ttime( d, t );
            }
        }
        if(( atime.dwLowDateTime == mtime.dwLowDateTime ) &&
           ( atime.dwHighDateTime == mtime.dwHighDateTime )) {
            buf->st_atime = buf->st_mtime;
        } else {
            __MakeDOSDT( &atime, &d, &t );
            if( d == md && t == mt ) {
                buf->st_atime = buf->st_mtime;
            } else {
                buf->st_atime = _d2ttime( d, t );
            }
        }

        buf->st_dev = buf->st_rdev = 0;
    }
    _ReleaseFileH( hid );

    buf->st_nlink = 1;
    buf->st_ino = hid;
    buf->st_uid = buf->st_gid = 0;

    buf->st_btime = buf->st_mtime;
    buf->st_archivedID = 0;
    buf->st_updatedID = 0;
    buf->st_inheritedRightsMask = 0;
    buf->st_originatingNameSpace = 0;
    return( 0 );
}
