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


#undef __INLINE_FUNCTIONS__
#include "variety.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <process.h>

/*
 * if program name empty then quit
 * if program name contains '/' or PATH not defined
 *    attempt to spawn requested task
 * otherwise
 *    attempt to spawn task from one of the paths listed in the PATH
 *    environment variable (set PATH=/bin:/usr/jack/bin:/etc/bin).
 */

static char *__last_path;


_WCRTLINK int (spawnvpe)( mode, path, argv, envp )
    int             mode;       /* wait, nowait or overlay(==exec) */
    const char      *path;      /* path name of file to be executed */
    const char *const argv[];   /* array of pointers to arguments       */
    const char *const envp[];   /* array of pointers to environment strings */
{
    register char *p;
    register char *p2;
    register int retval, err;
    auto char buffer[_POSIX_PATH_MAX];
    int trailler=0;

    __last_path = "";
    if( *path == '\0' ) {
        errno = ENOENT;
        return( -1 );
    }
    p = (char *) getenv( "PATH" );
    for( p2 = (char *)path; *p2 != '\0'; p2++ ) {   /* POSIX check for / in file name */
        if( *p2 == '/' ) break;
    }
    if( p == NULL || *p2 == '/' ) return( spawnve( mode, path, argv, envp ) );
    err = errno;
    for( retval = -1; ; ) {
        if( *p == '\0' ) break;
        for( __last_path = p, p2 = buffer; *p && *p != ':';  )
            *p2++ = *p++;
        if( p2 > buffer && p2[-1] != '/' )
            *p2++ = '/';
        strcpy( p2, path );
        retval = spawnve( mode, buffer, argv, envp );
        if( retval != -1 ) break;
        if( !(errno == ENOENT || errno == EACCES || errno == ENOTDIR) ) break;
        if( *p == '\0' ) break;
/*
 * Search current directory once if PATH has a trailling ':'
 */
        if( trailler ) break;
        if( *++p == '\0' ) {
            --p;
            trailler++;
        }
        errno = err;
    }
    return( retval );
}
