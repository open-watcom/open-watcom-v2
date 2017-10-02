/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Linux multi-threading functions
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <float.h>
#include <unistd.h>
#include <sched.h>
#include <process.h>
#include <semaphore.h>
#include "rtdata.h"
#include "liballoc.h"
#include "linuxsys.h"
#include "mthread.h"
#include "cthread.h"


static volatile struct __lnx_tls_entry {
    pid_t    id;
    void    *tls;
    volatile struct __lnx_tls_entry *next;
} *__tls;

extern sem_t *__tls_sem;

struct __lnx_thread {
    __thread_fn *start_addr;
    void        *args;
};

void *__LinuxGetThreadData( void )
{
    volatile struct __lnx_tls_entry *walker;
    void *ret;

    ret = NULL;

    sem_wait( __tls_sem );

        walker = __tls;
        while( walker != NULL ) {
            if( walker->id == gettid() ) {
                ret = walker->tls;
                break;
            }
            walker = walker->next;
        }

    sem_post( __tls_sem );

    return ret;
}

void __LinuxSetThreadData( void *__data )
{
    volatile struct __lnx_tls_entry *walker;
    volatile struct __lnx_tls_entry *previous;

    sem_wait( __tls_sem );
        walker = __tls;
        previous = NULL;
        while( walker != NULL ) {
            if( walker->id == gettid() )
                break;

            previous = walker;
            walker = walker->next;
        }

        if( walker == NULL && __data != NULL ) {
            walker = (struct __lnx_tls_entry *)lib_malloc( sizeof( struct __lnx_tls_entry * ) );
            walker->tls = __data;
            walker->id = gettid();
            walker->next = NULL;

            if( previous == NULL ) {
                __tls = walker;
            } else {
                previous->next = walker;
            }
        } else if( walker != NULL && __data == NULL ) {
            if( previous != NULL ) {
                previous->next = walker->next;
            } else {
                __tls = walker->next;
            }
            lib_free((void *)walker);
        } else if( walker != NULL && __data != NULL ) {
            walker->tls = __data;
        }
    sem_post( __tls_sem );
}

static void __cloned_lnx_start_fn( void *thrvoiddata )
{

    struct __lnx_thread *thrdata;

    __LinuxAddThread(NULL);

    thrdata = (struct __lnx_thread *)thrvoiddata;
    (*thrdata->start_addr)( thrdata->args );
    free( thrvoiddata );

    _sys_exit( 0 );
    // never return
}

int __CBeginThread( thread_fn *start_addr, void *stack_bottom,
                    unsigned stack_size, void *arglist )
/******************************************************/
{
    pid_t               pid;
    struct __lnx_thread *thrdata;
    unsigned            flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND
                            | CLONE_THREAD | CLONE_SYSVSEM | CLONE_PTRACE | CLONE_IO
                            | CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID | CLONE_DETACHED;

    if( stack_size == 0 && stack_bottom == NULL ) {
        stack_size = 16 * 4096; /* Docs and other platforms suggest this is
                                 * the OpenWatcom default
                                 */
        stack_bottom = malloc( stack_size );
    }

    if( start_addr == NULL || stack_bottom == NULL || stack_size == 0 ) {
        return( -1 );
    }

    thrdata = (struct __lnx_thread *)malloc( sizeof( struct __lnx_thread ) );
    if( thrdata == NULL ) {
        _RWD_errno = ENOMEM;
        return( -1 );
    }
    thrdata->start_addr = (__thread_fn *)start_addr;
    thrdata->args = arglist;

    pid = clone( (int(*)(void *))__cloned_lnx_start_fn, (void *)( (int)stack_bottom + stack_size ), flags, thrdata );

    return( (int)pid );
}

void __CEndThread( void )
/***********************/
{
    __LinuxSetThreadData(NULL);
    _sys_exit( 0 );
    // never return
}
