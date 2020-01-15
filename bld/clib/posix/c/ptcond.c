/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  POSIX thread condition variables
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <semaphore.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <sched.h>
#include "atomic.h"
#include "rterrno.h"
#include "thread.h"

_WCRTLINK int pthread_cond_init(pthread_cond_t *__cond, const pthread_condattr_t *__attr)
{
    int res;

    /* unused parameters */ (void)__attr;

    if( __cond == NULL )
        return( EINVAL );

    res = sem_init( &__cond->wait_block, 0, 0 );
    if(res != 0)
        return( res );

    res = sem_init( &__cond->clear_block, 0, 1 );
    if(res != 0)
        return( res );

    __cond->waiters = 0;

    return( 0 );
}

_WCRTLINK int pthread_cond_destroy(pthread_cond_t *__cond)
{
    if( __cond == NULL )
        return( EINVAL );

    __cond->waiters = 0;

    sem_destroy( &__cond->wait_block );
    sem_destroy( &__cond->clear_block );

    return( 0 );
}

_WCRTLINK int pthread_cond_timedwait(pthread_cond_t *__cond,
                                     pthread_mutex_t *__mutex,
                                     const struct timespec *abstime)
{
int res;

    sem_wait( &__cond->clear_block );
    __atomic_increment(&__cond->waiters);
    sem_post( &__cond->clear_block );

    pthread_mutex_unlock( __mutex );

    res = sem_timedwait( &__cond->wait_block, abstime );

    __atomic_decrement(&__cond->waiters);

    sem_wait( &__cond->clear_block );
    sched_yield();
    sem_post( &__cond->clear_block );

    pthread_mutex_lock( __mutex );

    return( res );
}

_WCRTLINK int pthread_cond_wait(pthread_cond_t *__cond,
                                pthread_mutex_t *__mutex)
{
int res;

    sem_wait( &__cond->clear_block );
    __atomic_increment(&__cond->waiters);
    sem_post( &__cond->clear_block );

    pthread_mutex_unlock( __mutex );

    res = sem_wait( &__cond->wait_block );

    __atomic_decrement(&__cond->waiters);

    pthread_mutex_lock( __mutex );

    return( res );
}

_WCRTLINK int pthread_cond_signal(pthread_cond_t *__cond)
{
int ret;
int waiters;

    sem_wait( &__cond->clear_block );

    ret = 0;
    if( __cond->waiters > 0 ) {
        waiters = __cond->waiters;
        ret = sem_post( &__cond->wait_block );
        while( __cond->waiters == waiters && waiters != 0 ) {
            sched_yield( );
        }
    }

    sem_post( &__cond->clear_block );

    return ret;
}

_WCRTLINK int pthread_cond_broadcast(pthread_cond_t *__cond)
{
int waiters;

    sem_wait( &__cond->clear_block );

    while(__cond->waiters > 0) {
        waiters = __cond->waiters;
        sem_post( &__cond->wait_block );
        while( __cond->waiters == waiters && waiters != 0 ) {
            sched_yield( );
        }
    }

    sem_post( &__cond->clear_block );
    return( 0 );
}
