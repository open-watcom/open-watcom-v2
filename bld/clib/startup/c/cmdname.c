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
* Description:  Implementation of _cmdname().
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include <unistd.h>
#include "rterrno.h"

extern char **_argv;    /* argument vector */

/* NOTE: This file isn't used for QNX. It's got its own version. */

#ifdef __APPLE__

#include <mach-o/dyld.h>

/* No procfs on Darwin, have to use special API */ 

_WCRTLINK char *_cmdname( char *name )
{
    uint32_t    len = 4096;

    _NSGetExecutablePath( name, &len );
    return( name );
}

#elif defined __LINUX__

_WCRTLINK char *_cmdname( char *name )
{
    int save_errno = _RWD_errno;
    int result = readlink( "/proc/self/exe", name, PATH_MAX );
    if( result == -1 ) {
        /* try another way for BSD */
        result = readlink( "/proc/curproc/file", name, PATH_MAX );
    }
    _RWD_errno = save_errno;

    /* fall back to argv[0] if readlink doesn't work */
    if( result == -1 || result == PATH_MAX )
        return( strcpy( name, _argv[0] ) );

    /* readlink does not add a NUL so we need to do it ourselves */
    name[result] = '\0';
    return( name );
}

#else

_WCRTLINK char *_cmdname( char *name )
{
    return( strcpy( name, _argv[0] ) );
}

#endif
