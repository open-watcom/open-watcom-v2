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
* Description:  POSIX thread condition variables
*
* Author: J. Armstrong
*
****************************************************************************/

#include <semaphore.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

_WCRTLINK int pthread_cond_init(pthread_cond_t *__cond, const pthread_condattr_t *__attr)
{
    if( __cond == NULL )
        return( EINVAL );
    
    pthread_mutex_init( &__cond->waiting_mutex, NULL );
    
    sem_init( &__cond->wait_block, 0, 1 );
    
    pthread_mutex_lock( &__cond->waiting_mutex );
    __cond->waiters = 0;
    pthread_mutex_unlock( &__cond->waiting_mutex );

    return( 0 );   
}

_WCRTLINK int pthread_cond_destroy(pthread_cond_t *__cond)
{
    if( __cond == NULL )
        return( EINVAL );
    
    pthread_mutex_lock( &__cond->waiting_mutex );
    __cond->waiters = 0;
    pthread_mutex_unlock( &__cond->waiting_mutex );
    
    sem_destroy( &__cond->wait_block );
    
    pthread_mutex_destroy( &__cond->waiting_mutex );
    
    return( 0 );   
}

_WCRTLINK int pthread_cond_timedwait(pthread_cond_t *__cond, 
                                     pthread_mutex_t *__mutex, 
                                     const struct timespec *abstime)
{
int res;

    pthread_mutex_lock( &__cond->waiting_mutex );
    
    /* If we're the first waiter, increment the semaphore now */
    if(__cond->waiters == 0)
        sem_wait(&__cond->wait_block);
    __cond->waiters++;
    
    pthread_mutex_unlock( &__cond->waiting_mutex );

    pthread_mutex_unlock(__mutex);
    res = sem_timedwait( &__cond->wait_block, abstime );

    pthread_mutex_lock( &__cond->waiting_mutex );
    __cond->waiters--;
    
    /* If we're the last waiter, release the semaphore */
    if(__cond->waiters == 0)
        sem_post(&__cond->wait_block);
        
    pthread_mutex_unlock( &__cond->waiting_mutex );

    if(res == 0)
        pthread_mutex_lock(__mutex);

    return( res );
}

_WCRTLINK int pthread_cond_wait(pthread_cond_t *__cond, 
                                pthread_mutex_t *__mutex)
{
int res;

    pthread_mutex_lock( &__cond->waiting_mutex );
    
    /* If we're the first waiter, increment the semaphore now */
    if(__cond->waiters == 0)
        sem_wait(&__cond->wait_block);
    __cond->waiters++;
    
    pthread_mutex_unlock( &__cond->waiting_mutex );

    pthread_mutex_unlock(__mutex);
    res = sem_wait( &__cond->wait_block );
    
    pthread_mutex_lock( &__cond->waiting_mutex );
    __cond->waiters--;
    
    /* If we're the last waiter, release the semaphore */
    if(__cond->waiters == 0)
        sem_post(&__cond->wait_block);
        
    pthread_mutex_unlock( &__cond->waiting_mutex );
    
    if(res == 0)
        pthread_mutex_lock(__mutex);

    return( res );
}

_WCRTLINK int pthread_cond_signal(pthread_cond_t *__cond)
{
    return( sem_post( &__cond->wait_block ) );
}

_WCRTLINK int pthread_cond_broadcast(pthread_cond_t *__cond)
{
int waiters;

    do {
        pthread_mutex_lock( &__cond->waiting_mutex );
        waiters = __cond->waiters;
        pthread_mutex_unlock( &__cond->waiting_mutex );
        
        sem_post( &__cond->wait_block );
    } while(waiters > 0);
    
    return( 0 );
}
