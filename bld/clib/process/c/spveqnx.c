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
#include <sys/types.h>
#include <process.h>
#include <sys/qnx_glob.h>
#include <sys/wait.h>
#include <stddef.h>
#include <signal.h>
#include "rtdata.h"

#define SPAWN   0
#define EXEC    1

/* P_WAIT       = 0 wait for spawned task to complete                         */
/* P_NOWAIT     = 1 resume execution immediately after spawning task          */
/* P_OVERLAY    = 2 replace current task with new task, equivalent to exec... */
/* P_NOWAITO    = 3 resume execution immediately, no result code              */

int __p_overlay = 2; /* defined here because of a trick in DOS version  */


_WCRTLINK int (spawnve)( mode, path, argv, envp )
    int             mode;       /* wait, nowait or overlay(==exec) */
    const char      *path;      /* path name of file to be executed */
    const char *const argv[];   /* array of pointers to arguments       */
    const char *const envp[];   /* array of pointers to environment strings */
{
    pid_t               pid;
    int                 err;
    int                 status;
    struct sigaction    old;
    struct sigaction    new;

    old.sa_handler = SIG_DFL;
    if( mode == P_WAIT ) {
        sigaction( SIGCHLD, NULL, &old );
        if( old.sa_handler == SIG_IGN ) {
            new = old;
            new.sa_handler = SIG_DFL;
            sigaction( SIGCHLD, &new, NULL );
        }
    }
    err = pid = qnx_spawn( mode == P_OVERLAY ? EXEC : SPAWN,
                    qnx_spawn_options.msgbuf,
                    (nid_t) qnx_spawn_options.node,
                    qnx_spawn_options.priority,
                    qnx_spawn_options.sched_algo,
                    qnx_spawn_options.flags
                        | ( mode == P_NOWAITO ? _SPAWN_NOZOMBIE : 0),
                    path, (char **)argv, (char **)envp,
                    qnx_spawn_options.iov,
                    qnx_spawn_options.ctfd );
    /* EXEC's don't return, only SPAWN does */
    if( err != -1 ) {
        /* if P_WAIT return invoked task's status otherwise P_NOWAIT so
           return pid and let user do the wait */
        if( mode == P_WAIT ) {
            do {
                err = waitpid( pid, &status, 0 );
            } while( err == -1 && _RWD_errno == EINTR );
            if( err == pid ) err = status;
        }
    }
    if( old.sa_handler == SIG_IGN ) {
        sigaction( SIGCHLD, &old, NULL );
    }
    return( err );
}

