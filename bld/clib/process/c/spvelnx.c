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
* Description:  Implementation of spawnve() for Linux.
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "variety.h"
#include <sys/types.h>
#include <process.h>
#include <sys/wait.h>
#include <stddef.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "_process.h"
#include "rterrno.h"
#include "thread.h"


#define SPAWN   0
#define EXEC    1

/* P_WAIT       = 0 wait for spawned task to complete                         */
/* P_NOWAIT     = 1 resume execution immediately after spawning task          */
/* P_OVERLAY    = 2 replace current task with new task, equivalent to exec... */
/* P_NOWAITO    = 3 resume execution immediately, no result code              */

int __p_overlay = 2; /* defined here because of a trick in DOS version  */


_WCRTLINK int (spawnve)( int mode, const char *path, const char *const argv[], const char *const envp[] )
{
    pid_t               pid;
    int                 err;
    int                 status;
    struct sigaction    old;
    struct sigaction    new;
    int                 status_pipe[2];

    if( mode == P_OVERLAY )
        return( execve( path, argv, envp ) );

    if( pipe( status_pipe ) == -1 )
        return( -1 );

    if( fcntl( status_pipe[1], F_SETFD, FD_CLOEXEC ) ) {
        close( status_pipe[0] );
        close( status_pipe[1] );
        return( -1 );
    }

    old.sa_handler = SIG_DFL;
    if( mode == P_WAIT ) {
        sigaction( SIGCHLD, NULL, &old );
        if( old.sa_handler == SIG_IGN ) {
            new = old;
            new.sa_handler = SIG_DFL;
            sigaction( SIGCHLD, &new, NULL );
        }
    }
    err = pid = fork();
    if( pid == 0 ) {
        close( status_pipe[0] );
        execve( path, argv, envp );
        write( status_pipe[1], &_RWD_errno, sizeof( _RWD_errno ) );
        _exit( 127 );
    }
    close( status_pipe[1] );
    /* EXEC's don't return, only SPAWN does */
    if( err != -1 )
        err = read( status_pipe[0], &_RWD_errno, sizeof( _RWD_errno ) );
    if( err != -1 ) {
        if( err > 0 ) {
            err = _RWD_errno;
            waitpid( pid, NULL, 0 );
            _RWD_errno = err;
            err = -1;
        } else if ( mode == P_WAIT ) {
           /* if P_WAIT return invoked task's status otherwise P_NOWAIT so
              return pid and let user do the wait */
            do {
                err = waitpid( pid, &status, 0 );
            } while( err == -1 && _RWD_errno == EINTR );
            if( err == pid )
                err = WEXITSTATUS( status );
        }
    }
    if( old.sa_handler == SIG_IGN ) {
        sigaction( SIGCHLD, &old, NULL );
    }
    close( status_pipe[0] );
    return( err );
}
