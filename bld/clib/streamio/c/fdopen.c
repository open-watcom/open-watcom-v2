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
* Description:  Platform independent fdopen() implementation.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#endif
#include <fcntl.h>
#ifdef __NT__
    #include <windows.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#elif defined( __NETWARE__ )
    #include "nw_lib.h"
#endif
#include "rtdata.h"
#include "seterrno.h"
#include "iomode.h"
#include "streamio.h"
#include "thread.h"
#include "openflag.h"


#ifndef __NETWARE__

static int __iomode( int handle, unsigned amode )
{
    int             __errno;
#ifdef __UNIX__
    int             flags;

    if( (flags = fcntl( handle, F_GETFL )) == -1 ) {
        return( -1 );
    }

    __errno = EOK;
    if( (flags & O_APPEND)
      && (amode & _APPEND) == 0 ) {
        __errno = EACCES;
    }
    if( (flags & O_ACCMODE) == O_RDONLY ) {
        if( amode & _WRITE ) {
            __errno = EACCES;
        }
    } else if( (flags & O_ACCMODE) == O_WRONLY ) {
        if( amode & _READ ) {
            __errno = EACCES;
        }
    }
#else
    unsigned        iomode_flags;

    /* make sure the handle has the same text/binary mode */
    iomode_flags = __GetIOMode( handle );
    __errno = 0;
    if( (amode ^ iomode_flags) & (_BINARY | _APPEND) ) {
        __errno = EACCES;
    }
    if( (amode & _READ)
      && (iomode_flags & _READ) == 0 ) {
        __errno = EACCES;
    }
    if( (amode & _WRITE)
      && (iomode_flags & _WRITE) == 0 ) {
        __errno = EACCES;
    }
#endif
    if( __errno == EACCES ) {
        _RWD_errno = __errno;
        return( -1 );
    }
    return( 0 );
}

#endif

_WCRTLINK FILE *__F_NAME(fdopen,_wfdopen)( int handle, const CHAR_TYPE *access_mode )
{
    unsigned        file_flags;
    FILE            *fp;
    int             extflags;

    if( handle == -1 ) {
        _RWD_errno = EBADF;
        return( NULL );
    }
    file_flags = __F_NAME(__open_flags,__wopen_flags)( access_mode, &extflags );
    if( file_flags == 0 )
        return( NULL );

#ifndef __NETWARE__
    /* make sure the handle has the same text/binary mode */
    if( __iomode( handle, file_flags ) == -1 ) {
        return( NULL );
    }
#endif
    fp = __allocfp();
    if( fp != NULL ) {
        fp->_flag |= file_flags;
        fp->_cnt = 0;
        _FP_BASE( fp ) = NULL;
        fp->_bufsize = 0;                   /* was BUFSIZ */
#ifndef __NETWARE__
        _FP_ORIENTATION( fp ) = _NOT_ORIENTED; /* initial orientation */
        _FP_EXTFLAGS( fp ) = extflags;
#endif
#if defined( __NT__ ) || defined( __OS2__ ) || defined(__UNIX__)
        _FP_PIPEDATA( fp ).isPipe = 0;    /* not a pipe */
#endif
        fp->_handle = handle;
        if( __F_NAME(tolower,towlower)( (UCHAR_TYPE)*access_mode ) == STRING( 'a' ) ) {
            fseek( fp, 0, SEEK_END );
        }
        __chktty( fp );
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
        __SetIOMode_grow( handle, file_flags );
#endif
    }
    return( fp );
}
