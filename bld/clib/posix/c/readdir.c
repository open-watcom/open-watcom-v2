/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation for readdir(64) for Linux.
*
****************************************************************************/


#include "variety.h"
#include "seterrno.h"
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include "linuxsys.h"
#include "dirstrea.h"
#include "filei64.h"


_WCRTLINK struct __64_NAME(dirent,dirent64) *__64_NAME(readdir,readdir64)( DIR *dirp )
{
    struct __64_NAME(dirent,dirent64)   *dir;
    long                                rc;

    dir = (struct __64_NAME(dirent,dirent64) *)(&dirp->dirent_buf[dirp->bufofs]);
    if( dirp->bufofs == 0 ) {
        rc = __64_NAME(sys_getdents,sys_getdents64)( dirp->fd, (void *)dirp->dirent_buf, sizeof( dirp->dirent_buf ) );
        if( rc == 0 || rc == -1 ) {
            return( NULL );
        }
        dirp->bufsize = rc;
    }

    dirp->bufofs += dir->d_reclen;
    if( dirp->bufofs >= dirp->bufsize )
        dirp->bufofs = 0;

    return( dir );
}
