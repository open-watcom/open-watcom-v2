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
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#if defined(__OS2__)
    #define INCL_DOS
    #include <wos2.h>
#elif defined(__NT__)
    #include <windows.h>
#endif
#include "openmode.h"
#include "iomode.h"
#include "rtcheck.h"
#include "rtdata.h"
#include "seterrno.h"
#include "fileacc.h"


#if !defined(__NETWARE__)

static int check_mode( int handle, int mode ) {
/*********************************************/

    int flags;
    int __errno;

    handle=handle;mode=mode;flags=flags;
    __errno = 0;
    #if defined(__UNIX__)
        if( (flags = fcntl( handle, F_GETFL )) == -1 ) {
            return( -1 );
        }

        __errno = EOK;
        if( (flags & O_APPEND) && !(mode & _APPEND) ) {
            __errno = EACCES;
        }
        if( (flags & O_ACCMODE) == O_RDONLY ) {
            if( ( (mode&O_RDWR) && !(mode&O_RDONLY) ) || (mode&O_WRONLY) ) {
                __errno = EACCES;
            }
        } else if( (flags & O_ACCMODE) == O_WRONLY ) {
            if( ( (mode&O_RDWR) && !(mode&O_WRONLY) ) || (mode&O_RDONLY) ) {
                __errno = EACCES;
            }
        }
    #elif defined(__WARP__)
        {
            ULONG   state;
            int     rc;

            rc = DosQueryFHState((HFILE)handle, &state);

            if( rc == 0 ) {
                if( (state&OPENMODE_ACCESS_MASK) == OPENMODE_ACCESS_RDONLY ) {
                    if( ( (mode&O_RDWR) && !(mode&O_RDONLY) ) || (mode&O_WRONLY) ) {
                        __errno = EACCES;
                    }
                }
                if( (state&OPENMODE_ACCESS_MASK) == OPENMODE_ACCESS_WRONLY ) {
                    if( ( (mode&O_RDWR) && !(mode&O_WRONLY) ) || (mode&O_RDONLY) ) {
                        __errno = EACCES;
                    }
                }
            } else {
                __errno = EACCES;
            }
        }
    #elif defined(__OS2_286__)
        // there is support under OS/2 1.x, but Pharlap's run286 doesn't
        // support the DosQFHandState() function
    #elif defined(__NT__)
        // there doesn't appear to be any support under NT for interrogating
        // how a file handle was opened
    #else
        // other systems are MS-DOS and NETWARE...
        // these systems have no support for interrogating how a file handle
        // was opened.
    #endif
    if( __errno == EACCES ) {
        __set_errno( __errno );
        return( -1 );
    }
    return(0);
}

#endif

// It is questionable, whether this routine does anything useful
// except under Win32. (JBS)

_WCRTLINK int _open_osfhandle( long osfhandle, int flags )
{
    int posix_handle;

    #if defined(__NT__)
        // Under Win32, we get an OS handle argument
        posix_handle = __allocPOSIXHandle( (HANDLE)osfhandle );
        if( posix_handle == -1 ) return( -1 );
    #else
        // Under everything else, we get a POSIX handle argument
        posix_handle = osfhandle;
    #endif
    #if !defined(__NETWARE__)
        if( check_mode( posix_handle, flags ) ) {
            return( -1 );
        }
        #if !defined(__UNIX__)
        {
            int         rwmode;
            unsigned    io_mode;

            rwmode = flags & ( O_RDONLY | O_WRONLY | O_RDWR );
            io_mode = __GetIOMode( posix_handle );
            io_mode &= ~(_READ|_WRITE|_APPEND|_BINARY);
            if( rwmode == O_RDWR )  io_mode |= _READ | _WRITE;
            if( rwmode == O_RDONLY) io_mode |= _READ;
            if( rwmode == O_WRONLY) io_mode |= _WRITE;
            if( flags & O_APPEND )   io_mode |= _APPEND;
            if( flags & O_BINARY ) {
               io_mode |= _BINARY;
            }
            __SetIOMode( posix_handle, io_mode );
        }
        #endif
    #endif

    return( posix_handle );
}

_WCRTLINK int _hdopen( int os_handle, int mode )
{
    return( _open_osfhandle( (long) os_handle, mode ) );
}
