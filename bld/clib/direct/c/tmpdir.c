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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "_direct.h"

static char *try_one( char *p, char *buff )
{
#ifdef __LINUX__
    // TODO: Needs Linux POSIX library!
    return NULL;
#else
    int         fd;
    mode_t      omask;

    if( p == NULL ) return( NULL );
    strcpy( buff, p );
    p = &buff[ strlen( buff ) ];
    if( p[-1] != '/' ) {
        p[0] = '/';
        ++p;
    }
    p[0] = '?';
    p[1] = '\0';
    omask = umask( 0 );
    fd = open( buff, O_RDONLY|O_CREAT|O_TEMP, 0666 );
    unlink( buff );
    umask( omask );
    if( fd == -1 ) return( NULL );
    close( fd );
    return( p );
#endif
}

char *__tmpdir( char *buff )
{
    char        *p;

    p = try_one( getenv( "TMPDIR" ), buff );
    if( p != NULL ) return( p );
    p = try_one( "/tmp", buff );
    if( p != NULL ) return( p );
    return( buff );
}
