/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  POSIX thread cancelation functions and signal handler
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "_ptint.h"

#ifdef __UNIX__
#include <signal.h>
#include <errno.h>
#endif

/* Internal flag for indicating defered cancellation */
#define PTHREAD_CANCEL_SET  128

/* Cancellation status */
#define CANCEL_ENABLED(s)   ((s & PTHREAD_CANCEL_ENABLE) == PTHREAD_CANCEL_ENABLE)
#define CANCEL_DEFERED(s)   ((s & PTHREAD_CANCEL_DEFERRED) == PTHREAD_CANCEL_DEFERRED)
#define CANCEL_REQUESTED(s) ((s & PTHREAD_CANCEL_SET) == PTHREAD_CANCEL_SET)

typedef void (*sighandler_t)(int);

static void __thread_handle_cancellation(int signal)
{
    pthread_exit(PTHREAD_CANCELED);
}

int __thread_enable_cancellation(int enable)
{
sighandler_t res;

    if(enable == PTHREAD_CANCEL_ENABLE)
        res = signal(SIGCANCEL, __thread_handle_cancellation);
    else
        res = signal(SIGCANCEL, SIG_IGN);
        
    return( res == __thread_handle_cancellation ? PTHREAD_CANCEL_ENABLE : PTHREAD_CANCEL_DISABLE );
}

_WCRTLINK int pthread_cancel( pthread_t __thread )
{
pid_t internal;
int cancel_status;
int ret;

    cancel_status = __get_thread_cancel_status(__thread);
    
    if(!CANCEL_ENABLED(cancel_status))
        return( EPERM );
    
    if(CANCEL_DEFERED(cancel_status)) {
        cancel_status |= PTHREAD_CANCEL_SET;
        __set_thread_cancel_status(__thread, cancel_status);
    } else {
#ifdef __UNIX__
        internal = __get_thread_id(__thread);
        ret = kill(internal, SIGCANCEL);
        if(ret != 0)
            ret = errno;
    
#else
        ret = ENOSYS;
#endif
    }
    
    return( ret );
}

_WCRTLINK int pthread_setcancelstate(int __state, int *__oldstate)
{
pthread_t internal;
int cancel_status;
int res;

    if(__state != PTHREAD_CANCEL_ENABLE ||
       __state != PTHREAD_CANCEL_DISABLE)
    {
        return( EINVAL );
    }

    internal = __get_current_thread();
    cancel_status = __get_thread_cancel_status(internal);

    res = cancel_status & PTHREAD_CANCEL_ENABLE;
    if(res != __state) {
        if(res == PTHREAD_CANCEL_ENABLE)
            cancel_status -= PTHREAD_CANCEL_ENABLE;
        else
            cancel_status += PTHREAD_CANCEL_ENABLE;

        __set_thread_cancel_status(internal, cancel_status);
    }
    
    if(__oldstate != NULL)
        *__oldstate = res;
        
    return( 0 );
}

_WCRTLINK int pthread_setcanceltype(int __type, int *__oldtype)
{
pthread_t internal;
int cancel_status;
int res;

    internal = __get_current_thread();

    if(__type != PTHREAD_CANCEL_DEFERRED ||
       __type != PTHREAD_CANCEL_ASYNCHRONOUS)
    {
        return( EINVAL );
    }
    
    cancel_status = __get_thread_cancel_status(internal);
    res = CANCEL_DEFERED(cancel_status) ? PTHREAD_CANCEL_DEFERRED : PTHREAD_CANCEL_ASYNCHRONOUS;

    if(res != __type) {
        if(__type == PTHREAD_CANCEL_DEFERRED) {
            cancel_status += PTHREAD_CANCEL_DEFERRED;
            cancel_status -= PTHREAD_CANCEL_ASYNCHRONOUS;
            signal(SIGCANCEL, SIG_IGN);
        } else {
            cancel_status -= PTHREAD_CANCEL_DEFERRED;
            cancel_status += PTHREAD_CANCEL_ASYNCHRONOUS;
            signal(SIGCANCEL, __thread_handle_cancellation);
        }
        __set_thread_cancel_status(internal, cancel_status);
    }

    if(__oldtype != NULL)
        *__oldtype = res;

    return( 0 );
}

_WCRTLINK void pthread_testcancel(void)
{
pthread_t internal;
int cancel_status;

    internal = __get_current_thread();
    cancel_status = __get_thread_cancel_status(internal);
    if(CANCEL_ENABLED(cancel_status) && CANCEL_REQUESTED(cancel_status))
        __thread_handle_cancellation(0);
}
