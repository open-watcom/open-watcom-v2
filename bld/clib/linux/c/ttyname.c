/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2015 Open Watcom Contributors.
*    All Rights Reserved.
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
* Description:  Linux/POSIX ttyname - retrieves the name of a tty
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "rterrno.h"

#define MAX_TTY_NAME    PATH_MAX   
#define BASE_LINK       "/proc/self/fd/"

_WCRTLINK int ttyname_r(int fd, char *buf, size_t buflen)
{
char linkpath[MAX_TTY_NAME];

    if(isatty(fd) == 0) {
        _RWD_errno = ENOTTY;
        return( _RWD_errno );
    }
    
    snprintf(linkpath, MAX_TTY_NAME, "%s%d", BASE_LINK, fd);
    if(readlink(linkpath, buf, buflen) < 0) {
        return( _RWD_errno );
    }
    
    return 0;
}

_WCRTLINK char *ttyname(int fd)
{
static char linkpath[MAX_TTY_NAME];

    if(ttyname_r(fd, linkpath, (size_t)MAX_TTY_NAME) == 0)
        return linkpath;
    else
        return NULL;
}
