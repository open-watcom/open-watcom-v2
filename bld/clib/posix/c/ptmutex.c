/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2016 Open Watcom Contributors.
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
* Description:  POSIX thread mutual exclusion handling
*
* Author: J. Armstrong
*
****************************************************************************/

#define MUTEX_STATUS_READY      100
#define MUTEX_STATUS_LOCKED     101
#define MUTEX_STATUS_DESTROYED  200

#include "variety.h"
#include <semaphore.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <process.h>
#include "rterrno.h"
#include "thread.h"


_WCRTLINK int pthread_mutex_init(pthread_mutex_t *__mutex, const pthread_mutexattr_t *__attr)
{
    if(__mutex->status == MUTEX_STATUS_READY || __mutex->status == MUTEX_STATUS_LOCKED)
        return( EINVAL );

    if(sem_init(&__mutex->access, 0, 1) != 0) {
        return( _RWD_errno );
    }
    
    if(sem_init(&__mutex->mutex, 0, 1) != 0) {
        return( _RWD_errno );
    }
    
    __mutex->status = MUTEX_STATUS_READY;
    __mutex->owner = (pid_t)-1;
    __mutex->type = PTHREAD_MUTEX_DEFAULT;
    
    if(__attr != NULL)
        __mutex->type = __attr->type;
    
    return( 0 );   
}

_WCRTLINK int pthread_mutex_destroy(pthread_mutex_t *__mutex)
{
int res;

    if(__mutex->status != MUTEX_STATUS_READY)
        return( EINVAL );
    
    /* Need to ensure the mutex isn't currently locked */
    res = pthread_mutex_trylock(__mutex);
    if(res != 0)
        return( res );

    if(sem_destroy(&__mutex->access) != 0)
        return( _RWD_errno );
    
    /* Need to release the mutex semaphore now */
    sem_post(&__mutex->mutex);
    if(sem_destroy(&__mutex->mutex) != 0)
        return( _RWD_errno );
        
    __mutex->status = MUTEX_STATUS_DESTROYED;
    return( 0 );
}

#define LOCK_PROCEED        0
#define LOCK_ERROR_OWNED    1

static int __pthread_check_lock_type(pthread_mutex_t *__mutex)
{
int ret;

    ret = LOCK_PROCEED;

    sem_wait(&__mutex->access);
    if(__mutex->status == MUTEX_STATUS_READY)
        ret = LOCK_PROCEED;
    else if(__mutex->status == MUTEX_STATUS_LOCKED && __mutex->owner == gettid()) {
    
        /* For a "normal" mutex, proceed with deadlock... */
        if(__mutex->type == PTHREAD_MUTEX_NORMAL)
            ret = LOCK_PROCEED;
        
        else if(__mutex->type == PTHREAD_MUTEX_ERRORCHECK)
            ret = LOCK_ERROR_OWNED;
    }    
    sem_post(&__mutex->access);
    
    return( ret );
}

_WCRTLINK int pthread_mutex_trylock(pthread_mutex_t *__mutex)
{
int ret;

    if(__mutex == NULL)
        return( EINVAL );

    sem_wait(&__mutex->access);
    if(__mutex->status == MUTEX_STATUS_READY) {
        sem_wait(&__mutex->mutex);
        __mutex->status = MUTEX_STATUS_LOCKED;
        __mutex->owner = gettid();
        ret = 0;
    } else 
        ret = EBUSY;
    
    sem_post(&__mutex->access);
    return( ret );
}

_WCRTLINK int pthread_mutex_lock(pthread_mutex_t *__mutex)
{
int ret;
int res;
    
    if(__mutex == NULL)
        return( EINVAL );
    
    ret = -1;
    
    res = __pthread_check_lock_type(__mutex);
    if(res == LOCK_ERROR_OWNED)
        return( EDEADLK );
    
    if(sem_wait(&__mutex->mutex) == 0) {
        sem_wait(&__mutex->access);
        
        __mutex->status = MUTEX_STATUS_LOCKED;
        __mutex->owner = gettid();
        ret = 0;
        
        sem_post(&__mutex->access);
    }
    
    return( ret );
}

_WCRTLINK int pthread_mutex_unlock(pthread_mutex_t *__mutex)
{
int ret;
    
    if(__mutex == NULL)
        return( EINVAL );
    
    ret = -1;
    
    if(sem_wait(&__mutex->access) == 0) {
        if(__mutex->status == MUTEX_STATUS_READY)
            ret = 0;
        else if(__mutex->owner == gettid()) {
            __mutex->status = MUTEX_STATUS_READY;
            __mutex->owner = (pid_t)-1;
            sem_post(&__mutex->mutex);
        } else if(__mutex->status == MUTEX_STATUS_LOCKED)
            ret = EPERM;
        else if(__mutex->status == MUTEX_STATUS_DESTROYED)
            ret = EINVAL;
        else
            ret = EBUSY;
        
        sem_post(&__mutex->access);
    }
    
    return( ret );
}

_WCRTLINK int __pthread_mutex_mylock(pthread_mutex_t *__mutex)
{
int ret;

    if(__mutex == NULL)
        return( EINVAL );

    ret = -1;
    if(sem_wait(&__mutex->access) == 0) {
        if(__mutex->status == MUTEX_STATUS_READY)
            ret = -1;
        else if(__mutex->owner == gettid()) {
            ret = 0;
        } else if(__mutex->status == MUTEX_STATUS_DESTROYED)
            ret = EINVAL;
        else
            ret = EBUSY;
        
        sem_post(&__mutex->access);
    }
    
    return( ret );
}

_WCRTLINK int pthread_mutex_setprioceiling(pthread_mutex_t *__mutex, int __prioceiling, int *__old_ceiling)
{
    return( ENOSYS );
}

_WCRTLINK int pthread_mutex_getprioceiling(const pthread_mutex_t *__mutex, int *__prioceiling)
{
    return( ENOSYS );
}
