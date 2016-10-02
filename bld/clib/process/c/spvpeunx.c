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
* Description:  Implementation of spawnvpe() for UNIX.
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "variety.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include "rterrno.h"
#include "thread.h"


/*
 * if program name empty then quit
 * if program name contains '/' or PATH not defined
 *    attempt to spawn requested task
 * otherwise
 *    attempt to spawn task from one of the paths listed in the PATH
 *    environment variable (set PATH=/bin:/home/jack/bin:/etc/bin).
 */

static char *__last_path;


_WCRTLINK int (spawnvpe)( int mode, const char *path, const char *const argv[], const char *const envp[] )
{
    char    *p;
    char    *p2;
    int     retval, err;
    char    buffer[_POSIX_PATH_MAX];
    int     trailer = 0;

    __last_path = "";
    if( *path == '\0' ) {
        _RWD_errno = ENOENT;
        return( -1 );
    }
    p = (char *)getenv( "PATH" );
    for( p2 = (char *)path; *p2 != '\0'; p2++ ) {   /* POSIX check for / in file name */
        if( *p2 == '/' )
            break;
    }
    if( p == NULL || *p2 == '/' )
        return( spawnve( mode, path, argv, envp ) );
    err = _RWD_errno;
    for( retval = -1; ; ) {
        if( *p == '\0' )
            break;
        for( __last_path = p, p2 = buffer; *p && *p != ':';  )
            *p2++ = *p++;
        if( p2 > buffer && p2[-1] != '/' )
            *p2++ = '/';
        strcpy( p2, path );
        retval = spawnve( mode, buffer, argv, envp );
        if( retval != -1 )
            break;
        if( !(_RWD_errno == ENOENT || _RWD_errno == EACCES || _RWD_errno == ENOTDIR) )
            break;
        if( *p == '\0' )
            break;
/*
 * Search current directory once if PATH has a trailling ':'
 */
        if( trailer )
            break;
        if( *++p == '\0' ) {
            --p;
            trailer++;
        }
        _RWD_errno = err;
    }
    return( retval );
}
