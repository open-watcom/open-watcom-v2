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
* Description:  Platform independent fdopen() implementation.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <ctype.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#endif
#include <errno.h>
#include <fcntl.h>
#ifdef __NT__
    #include <windows.h>
#endif
#include "iomode.h"
#include "rtdata.h"
#include "seterrno.h"
#include "streamio.h"


extern int      __F_NAME(__open_flags,__wopen_flags)( const CHAR_TYPE *, int * );


#ifndef __NETWARE__

static int __iomode( int handle, int amode )
{
    int flags;
    int __errno;

#ifdef __UNIX__
    if( (flags = fcntl( handle, F_GETFL )) == -1 ) {
        return( -1 );
    }

    __errno = EOK;
    if( (flags & O_APPEND) && !(amode & _APPEND) ) {
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
    /* make sure the handle has the same text/binary mode */
    flags = __GetIOMode( handle );
    __errno = 0;
    if( (amode ^ flags) & (_BINARY | _APPEND) ) {
        __errno = EACCES;
    }
    if( ( amode & _READ )  && !(flags & _READ) ) {
        __errno = EACCES;
    }
    if( ( amode & _WRITE ) && !(flags & _WRITE) ) {
        __errno = EACCES;
    }
#endif
    if( __errno == EACCES ) {
        __set_errno( __errno );
        return( -1 );
    }
    return( 0 );
}

#endif

_WCRTLINK FILE *__F_NAME(fdopen,_wfdopen)( int handle, const CHAR_TYPE *access_mode )
{
    unsigned        flags;
    FILE            *fp;
    int             extflags;

    if( handle == -1 ) {
        __set_errno( EBADF );           /* 5-dec-90 */
        return( NULL );                 /* 19-apr-90 */
    }
    flags = __F_NAME(__open_flags,__wopen_flags)( access_mode, &extflags );
    if( flags == 0 ) return( NULL );

#ifndef __NETWARE__
    /* make sure the handle has the same text/binary mode */
    if( __iomode( handle, flags ) == -1 ) {
        return( NULL );
    }
#endif
    fp = __allocfp( handle );               /* JBS 30-aug-91 */
    if( fp ) {
        fp->_flag &= ~(_READ | _WRITE); /* 2-dec-90 */
        fp->_flag |= flags;
        fp->_cnt = 0;
        _FP_BASE(fp) = NULL;
        fp->_bufsize = 0;                   /* was BUFSIZ JBS 91/05/31 */
#ifndef __NETWARE__
        _FP_ORIENTATION(fp) = _NOT_ORIENTED; /* initial orientation */
        _FP_EXTFLAGS(fp) = extflags;
#endif
#if defined( __NT__ ) || defined( __OS2__ )
        _FP_PIPEDATA(fp).isPipe = 0;    /* not a pipe */
#endif
        fp->_handle = handle;               /* BJS 91-07-23 */
        if( __F_NAME(tolower,towlower)( *access_mode ) == 'a' ) {
            fseek( fp, 0, SEEK_END );
        }
        __chktty( fp );                     /* JBS 31-may-91 */
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
        __SetIOMode( handle, flags );
#endif
    }
    return( fp );
}
