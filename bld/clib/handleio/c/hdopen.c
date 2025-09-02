/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#if defined(__OS2__)
    #define INCL_DOS
    #include <wos2.h>
#elif defined(__NT__)
    #include <windows.h>
#endif
#include "rterrno.h"
#include "openmode.h"
#include "iomode.h"
#include "rtcheck.h"
#include "fileacc.h"
#include "thread.h"


#if !defined(__NETWARE__)

static int _WCNEAR check_mode( int handle, int mode )
/***************************************************/
{
  #if defined(__UNIX__)
    int     __errno;
    int     flags;

    if( (flags = fcntl( handle, F_GETFL )) == -1 ) {
        return( -1 );
    }
    __errno = EOK;
    if( (flags & O_APPEND)
      && (mode & O_APPEND) == 0 ) {
        __errno = EACCES;
    }
    if( (flags & O_ACCMODE) == O_RDONLY ) {
        if( ( (mode & O_RDWR)
          && (mode & O_RDONLY) == 0 )
          || (mode & O_WRONLY) ) {
            __errno = EACCES;
        }
    } else if( (flags & O_ACCMODE) == O_WRONLY ) {
        if( ( (mode & O_RDWR)
          && (mode & O_WRONLY) == 0 )
          || (mode & O_RDONLY) ) {
            __errno = EACCES;
        }
    }
    if( __errno == EACCES ) {
        _RWD_errno = __errno;
        return( -1 );
    }
  #elif defined(__OS2_32BIT__)
    int     __errno;
    ULONG   state;
    int     rc;

    __errno = EZERO;
    rc = DosQueryFHState( (HFILE)handle, &state );
    if( rc == 0 ) {
        if( (state & OPENMODE_ACCESS_MASK) == OPENMODE_ACCESS_RDONLY ) {
            if( ( (mode & O_RDWR)
              && (mode & O_RDONLY) == 0 )
              || (mode & O_WRONLY) ) {
                __errno = EACCES;
            }
        }
        if( (state & OPENMODE_ACCESS_MASK) == OPENMODE_ACCESS_WRONLY ) {
            if( ( (mode & O_RDWR)
              && (mode & O_WRONLY) == 0 )
              || (mode & O_RDONLY) ) {
                __errno = EACCES;
            }
        }
    } else {
        __errno = EACCES;
    }
    if( __errno == EACCES ) {
        _RWD_errno = __errno;
        return( -1 );
    }
  #elif defined(__OS2_16BIT__)
    // there is support under OS/2 1.x, but Pharlap's run286 doesn't
    // support the DosQFHandState() function
    (void)handle;
    (void)mode;
  #elif defined(__NT__)
    // there doesn't appear to be any support under NT for interrogating
    // how a file handle was opened
    (void)handle;
    (void)mode;
  #else
    // other systems are MS-DOS and NETWARE...
    // these systems have no support for interrogating how a file handle
    // was opened.
    (void)handle;
    (void)mode;
  #endif
    return( 0 );
}

#endif

/*
 * It is questionable, whether this routine does anything useful
 * except under Win32.
 */
_WCRTLINK int _open_osfhandle( long osf_handle, int flags )
{
    int handle;

    /*
     * Under Win32, we get an OS handle argument
     * Under everything else, we get a POSIX handle argument
     */
#if defined(__NT__)
    handle = __allocPOSIXHandle( (HANDLE)osf_handle );
    if( handle == -1 ) {
        return( -1 );
    }
#else
    handle = osf_handle;
#endif
#if defined(__NETWARE__)
    (void)flags;
#else
    if( check_mode( handle, flags ) ) {
        return( -1 );
    }
  #if !defined(__UNIX__)
    {
        int         rwmode;
        unsigned    iomode_flags;

        rwmode = flags & ( O_RDONLY | O_WRONLY | O_RDWR );
        iomode_flags = __GetIOMode( handle ) & ~(_READ | _WRITE | _APPEND | _BINARY);
        if( rwmode == O_RDWR )
            iomode_flags |= _READ | _WRITE;
        if( rwmode == O_RDONLY )
            iomode_flags |= _READ;
        if( rwmode == O_WRONLY )
            iomode_flags |= _WRITE;
        if( flags & O_APPEND )
            iomode_flags |= _APPEND;
        if( flags & O_BINARY ) {
            iomode_flags |= _BINARY;
        }
    #if defined(__NT__)
        __SetIOMode( handle, iomode_flags );
    #else
        __SetIOMode_grow( handle, iomode_flags );
    #endif
    }
  #endif
#endif
    return( handle );
}

_WCRTLINK int _hdopen( int os_handle, int mode )
{
    #define OS_TO_OSFHANDLE(h)  (long)(h)
    return( _open_osfhandle( OS_TO_OSFHANDLE( os_handle ), mode ) );
    #undef OS_TO_OSFHANDLE
}
