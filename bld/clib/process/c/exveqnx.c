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
#include <process.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/qnx_glob.h>

#define SPAWN   0
#define EXEC    1


_WCRTLINK int (execve)( path, argv, envp )
    const char *path;           /* path name of path to be executed */
    const char *const argv[];   /* array of pointers to arguments   */
    const char *const envp[];   /* array of pointers to environment strings */
{
    return( qnx_spawn( EXEC,
                    qnx_spawn_options.msgbuf,
                    (nid_t) qnx_spawn_options.node,
                    qnx_spawn_options.priority,
                    qnx_spawn_options.sched_algo,
                    qnx_spawn_options.flags,
                    path, (char **)argv, (char **)envp,
                    qnx_spawn_options.iov,
                    qnx_spawn_options.ctfd ));
    /* EXEC's shouldn't return unless there was an error */
}
