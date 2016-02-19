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
* Description:  POSIX thread read-write lock handling
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include "_ptint.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>

_WCRTLINK int pthread_rwlock_init( pthread_rwlock_t *__rwlock, 
                           const pthread_rwlockattr_t *__attr )
{
int res;

    if(__rwlock == NULL) 
        return( EINVAL );

    res = pthread_mutex_init(&__rwlock->block_mutex, NULL);
    if(res != 0)
        return( res );
        
    __rwlock->read_waiters = 0;

    return( 0 );
}

_WCRTLINK int pthread_rwlock_destroy(pthread_rwlock_t *__rwlock)
{
int res;

    if(__rwlock == NULL)
        return( EINVAL );
        
    res = pthread_rwlock_trywrlock(__rwlock);
    if(res != 0)
        return( res );
    
    pthread_rwlock_unlock(__rwlock);
    
    res = pthread_mutex_destroy(&__rwlock->block_mutex);

    return( res );
}

_WCRTLINK int pthread_rwlock_unlock(pthread_rwlock_t *__rwlock)
{
    if(__rwlock == NULL)
        return( EINVAL );
    
    /* If this thread is locking the lock's mutex, it is a write lock */
    if(__pthread_mutex_mylock(&__rwlock->block_mutex) == 0) {
        pthread_mutex_unlock(&__rwlock->block_mutex);
    
    /* Otherwise, a read lock */
    } else {
        pthread_mutex_lock(&__rwlock->block_mutex);
        
        /* Note that we're just decrementing.  This implementation
         * will not ever return a EPERM error on unlock attempts.
         */
        __rwlock->read_waiters--;
        
        /* If we make this correction, things have gone wrong */
        if(__rwlock->read_waiters < 0)
            __rwlock->read_waiters = 0; 
        
        pthread_mutex_unlock(&__rwlock->block_mutex);
    }
    
    return( 0 );
}

_WCRTLINK int pthread_rwlock_tryrdlock(pthread_rwlock_t *__rwlock)
{
    if(__rwlock == NULL)
        return( EINVAL );
        
    if(pthread_mutex_trylock(&__rwlock->block_mutex) == 0) {
        __rwlock->read_waiters++;
        pthread_mutex_unlock(&__rwlock->block_mutex);
        return( 0 );
    } 
    
    return( EBUSY );
}

_WCRTLINK int pthread_rwlock_rdlock(pthread_rwlock_t *__rwlock)
{
int res;

    if(__rwlock == NULL)
        return( EINVAL );
    
    res = pthread_mutex_lock(&__rwlock->block_mutex);
    if(res == 0) {
        __rwlock->read_waiters++;
        pthread_mutex_unlock(&__rwlock->block_mutex);
        return( 0 );
    } 
    
    return( res );
}

_WCRTLINK int pthread_rwlock_trywrlock(pthread_rwlock_t *__rwlock)
{
int res;

    if(__rwlock == NULL)
        return( EINVAL );
    
    res = pthread_mutex_trylock(&__rwlock->block_mutex);
    if(res == 0) {
        if(__rwlock->read_waiters > 0) {
            pthread_mutex_unlock(&__rwlock->block_mutex);
            return( EBUSY);
        }
        
        return( 0 );
    } 
    
    return( res );
}

_WCRTLINK int pthread_rwlock_wrlock(pthread_rwlock_t *__rwlock)
{
int res;
struct timespec sleeper;

    if(__rwlock == NULL)
        return( EINVAL );
    
    do {
        res = pthread_mutex_lock(&__rwlock->block_mutex);
        if(__rwlock->read_waiters > 0) {
            pthread_mutex_unlock(&__rwlock->block_mutex);
            
            /* Sleep for a bit */
            sleeper.tv_sec = 0;
            sleeper.tv_nsec = 10000000L;
            nanosleep(&sleeper, NULL);
        } else
            break;
    } while(res == 0);
    
    
    return( 0 );
}
