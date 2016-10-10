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
* Description:  POSIX thread creation function and related internal calls
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <pthread.h>
#include <process.h>
#include <malloc.h>
#include <sched.h>
#include <string.h>
#include "_ptint.h"
#include "rterrno.h"
#include "thread.h"


/* By default, allow OpenWatcom's thread library
 * to handle this...
 */
#define STACK_SIZE  0

struct __thread_pass {
    void      *(*start_routine)(void*); 
    void      *arg;
    pthread_t  thread;
    sem_t      registered;
};

static void __thread_start( void *data )
{
    struct __thread_pass    *passed;
    void                    *ret;

    void                    *(*start_routine)(void*);
    void                    *arg;

    passed = (struct __thread_pass *)data;
    
    passed->thread = __register_thread();

    sem_post(&passed->registered);

    start_routine = passed->start_routine;
    arg = passed->arg;
    
    /* Lock our running mutex to allow for future joins */
    pthread_mutex_lock(__get_thread_running_mutex(passed->thread));
    
    /* Call the user routine */
    ret = start_routine(arg);
    
    /* The pointer 'ret' must be returned to any waiting
     * "join" operations
     */
    pthread_exit(ret);
}

_WCRTLINK int pthread_create( pthread_t *thread, const pthread_attr_t *attr,
                              void *(*start_routine)(void*), void *arg )
{
    int ret;
    size_t stack_size;
    char *stack;
    struct __thread_pass *passed;
    
    if(thread == NULL) {
        _RWD_errno = EINVAL;
        return( -1 );
    }
    
    stack_size = STACK_SIZE;
    stack = NULL;
    
    if(attr != NULL) {
        if(attr->stack_size > STACK_SIZE)
            stack_size = attr->stack_size;
            
        if(attr->stack_addr != NULL)
            stack = (char *)attr->stack_addr;
    }
    
    passed = (struct __thread_pass *)malloc(sizeof(struct __thread_pass));
    if(passed == NULL) {
        _RWD_errno = ENOMEM;
        return( -1 );
    }
    
    passed->start_routine = start_routine;
    passed->arg = arg;
    passed->thread = (pthread_t)-1;
    
    if(stack == NULL && stack_size > 0) {
        stack = (char *)malloc(stack_size*sizeof(char *));
        if(stack == NULL) {
            free(passed);
            _RWD_errno = ENOMEM;
            return( -1 );
        }
    }
    
    if(sem_init(&passed->registered, 0, 1) != 0) {
        return( -1 );
    }
    
    sem_wait(&passed->registered);
    
    ret = _beginthread( __thread_start, NULL, 0, (void *)passed );
    
    /* Wait for registration */
    sem_wait(&passed->registered);
    
    /* Apply a few more attributes if necessary */
    if(attr != NULL) {
        if(attr->sched_inherit == PTHREAD_EXPLICIT_SCHED)
            sched_setscheduler(__get_thread_id(passed->thread), attr->sched_policy, attr->sched_params);
        
        if(attr->detached == PTHREAD_CREATE_DETACHED)
            pthread_detach(passed->thread);
    }
    
    if(ret >= 0) {
        *thread = passed->thread;
        ret = 0;
    }
    
    /* Destroy the registration semaphore */
    sem_destroy(&passed->registered);

    /* Destroy the passing structure */
    free(passed);

    return( ret );
}
