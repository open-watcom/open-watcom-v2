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
* Description:  POSIX thread mutual exclusion handling
*
* Author: J. Armstrong
*
****************************************************************************/

#define MUTEX_STATUS_READY      -1
#define MUTEX_STATUS_DESTROYED  -2

#include "variety.h"
#include <semaphore.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include "rterrno.h"
#include "thread.h"
#include "atomic.h"

#include "_ptint.h"

_WCRTLINK int pthread_mutex_init(pthread_mutex_t *__mutex, const pthread_mutexattr_t *__attr)
{
    if(__mutex == NULL)
        return( EINVAL );

    if(sem_init(&__mutex->mutex, 0, 1) != 0) {
        return( _RWD_errno );
    }

    __mutex->owner = (pid_t)MUTEX_STATUS_READY;
    __mutex->type = PTHREAD_MUTEX_DEFAULT;

    if(__attr != NULL)
        __mutex->type = __attr->type;

    return( 0 );
}

_WCRTLINK int pthread_mutex_destroy(pthread_mutex_t *__mutex)
{
int res;

    /* Need to ensure the mutex isn't currently locked */
    res = pthread_mutex_trylock(__mutex);
    if(res != 0)
        return( res );

    __atomic_compare_and_swap( &__mutex->owner, gettid(), MUTEX_STATUS_DESTROYED );

    /* Need to release the mutex semaphore now */
    sem_post(&__mutex->mutex);
    if(sem_destroy(&__mutex->mutex) != 0)
        return( _RWD_errno );

    return( 0 );
}

#define LOCK_PROCEED        0
#define LOCK_ERROR_OWNED    1

static int __pthread_check_lock_type(pthread_mutex_t *__mutex)
{
int ret;

    ret = LOCK_PROCEED;

    if(__mutex->owner == MUTEX_STATUS_READY) {
        ret = LOCK_PROCEED;
    } else if(__mutex->owner == gettid()) {
        /* For a "normal" mutex, proceed with deadlock... */
        if(__mutex->type == PTHREAD_MUTEX_NORMAL) {
            ret = LOCK_PROCEED;
        } else if(__mutex->type == PTHREAD_MUTEX_ERRORCHECK) {
            ret = LOCK_ERROR_OWNED;
        }
    }

    return( ret );
}

_WCRTLINK int pthread_mutex_trylock(pthread_mutex_t *__mutex)
{
int ret;

    if(__mutex == NULL || __mutex->owner == MUTEX_STATUS_DESTROYED)
        return( EINVAL );

    if(__mutex->owner == -1) {
        sem_wait(&__mutex->mutex);
        __atomic_compare_and_swap( &__mutex->owner, -1, gettid());
        ret = 0;
    } else {
        ret = EBUSY;
    }
    return( ret );
}

_WCRTLINK int pthread_mutex_lock(pthread_mutex_t *__mutex)
{
int ret;
int res;

    if(__mutex == NULL || __mutex->owner == MUTEX_STATUS_DESTROYED)
        return( EINVAL );

    ret = -1;

    res = __pthread_check_lock_type(__mutex);
    if(res == LOCK_ERROR_OWNED)
        return( EDEADLK );

    if(sem_wait(&__mutex->mutex) == 0) {

        res = __atomic_compare_and_swap( &__mutex->owner, MUTEX_STATUS_READY, gettid());
        if(res == 0) {
            ret = EPERM;
        } else {
            ret = 0;
        }
    }

    return( ret );
}

_WCRTLINK int pthread_mutex_unlock(pthread_mutex_t *__mutex)
{
int ret;

    if(__mutex == NULL || __mutex->owner == MUTEX_STATUS_DESTROYED)
        return( EINVAL );

    ret = -1;

    if(__mutex->owner == MUTEX_STATUS_READY) {
        return EPERM;
    }

    if(__mutex->owner == gettid()) {
        if(!__atomic_compare_and_swap( &__mutex->owner, gettid(), MUTEX_STATUS_READY))
            return( EPERM );
        ret = 0;
        sem_post(&__mutex->mutex);
    } else {
        ret = EPERM;
    }
    return( ret );
}

int __pthread_mutex_mylock(pthread_mutex_t *__mutex)
{
    int ret;

    if(__mutex == NULL || __mutex->owner == MUTEX_STATUS_DESTROYED)
        return( EINVAL );

    ret = -1;
    if(__mutex->owner == gettid()) {
        ret = 0;
    } else if(__mutex->owner != MUTEX_STATUS_READY) {
        ret = -1;
    } else {
        ret = EPERM;
    }

    return( ret );
}

_WCRTLINK int pthread_mutex_setprioceiling(pthread_mutex_t *__mutex, int __prioceiling, int *__old_ceiling)
{
    /* unused parameters */ (void)__mutex; (void)__prioceiling; (void)__old_ceiling;

    return( ENOSYS );
}

_WCRTLINK int pthread_mutex_getprioceiling(const pthread_mutex_t *__mutex, int *__prioceiling)
{
    /* unused parameters */ (void)__mutex; (void)__prioceiling;

    return( ENOSYS );
}
