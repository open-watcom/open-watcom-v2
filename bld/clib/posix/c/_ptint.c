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
* Description:  Internal bookkeeping for POSIX threads
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <pthread.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "rterrno.h"
#include "thread.h"

#include "_ptint.h"


/* Per-thread key lists */
struct __ptkeys {
    pthread_key_t   id;
    void            *value;
    struct __ptkeys  *next;
};

/* Per-thread cleanup proc lists */
struct __ptcleaners {
    void (*routine)(void*);
    void                *arg;
    struct __ptcleaners *next;
};

/* Thread registry */
static struct __ptcatalog_struct {
    pid_t                       tid;
    pthread_t                   pt;
 
    pthread_mutex_t            *running_mutex;
    pthread_mutex_t            *waiting_mutex;

    int                         waiters;
    void                       *return_value;
    int                         cancel_status;

    struct __ptkeys            *keys;
    struct __ptcleaners        *cleaners;
    
    struct __ptcatalog_struct  *next;
} *__ptcatalog;

static pthread_mutex_t *__ptcatalog_mutex;

/* Thread-specific key registry */
static struct __ptkeylist_struct {
    pthread_key_t   id;
    void (*destructor)(void*);
    struct __ptkeylist_struct *next;
} *__ptkeylist;

static long keyid_count;
static pthread_mutex_t *__ptkeylist_mutex;

pthread_t __get_thread( pid_t tid )
{
pthread_t ret;
struct __ptcatalog_struct *walker;

    ret = 0;

    walker = __ptcatalog;
    while(walker != NULL) {
        if(walker->tid == tid) {
            ret = walker->pt;
            break;
        }
        walker = walker->next;
    }
    
    return( ret );
}

pthread_key_t __register_pkey( void (*destructor)(void*) )
{
struct __ptkeylist_struct *newkey;
struct __ptkeylist_struct *walker;
    
    newkey = (struct __ptkeylist_struct *)malloc(sizeof(struct __ptkeylist_struct));
    if( newkey == NULL ) {
        _RWD_errno = ENOMEM;
        return( (pthread_key_t)(-1) );
    }
    
    newkey->id = (pthread_key_t)keyid_count++;
    newkey->destructor = destructor;
    newkey->next = NULL;
    
     if(__ptkeylist_mutex == NULL) {
        __ptkeylist_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        if(__ptkeylist_mutex != NULL)
            pthread_mutex_init(__ptkeylist_mutex, NULL);
    }
    
    if(pthread_mutex_lock(__ptkeylist_mutex) == 0) {
        if(__ptkeylist == NULL)
            __ptkeylist = newkey;
        else {
            walker = __ptkeylist;
            while(walker->next != NULL) walker = walker->next;
            walker->next = newkey;
        }
        pthread_mutex_unlock(__ptkeylist_mutex);
    }
    
    return( newkey->id );
}

void __destroy_pkey( pthread_key_t id )
{
struct __ptkeylist_struct *walker, *previous;

    walker = NULL;
    previous = NULL;

    if(pthread_mutex_lock(__ptkeylist_mutex) == 0) {
        walker = __ptkeylist;
        while(walker != NULL) {
            if(walker->id == id) {
                /* First, remove from the master list */
                if(previous == NULL)
                    __ptkeylist = walker->next;
                else
                    previous->next = walker->next;
                    
                free(walker);
                break;
            }
        
            walker = walker->next;
        }
        pthread_mutex_unlock(__ptkeylist_mutex);
    }
}

int __valid_pkey_id( pthread_key_t id )
{
struct __ptkeylist_struct *walker;
int ret;

    ret = EINVAL;
    if(pthread_mutex_lock(__ptkeylist_mutex) == 0) {
        walker = __ptkeylist;
        while(walker != NULL) {
            if(walker->id == id) {
                ret = 0;
                break;
            }
            walker = walker->next;
        }
        pthread_mutex_unlock(__ptkeylist_mutex);
    }
    
    return( ret );
}

int __set_pkey_value( pthread_key_t id, void *value )
{
pid_t tid;
struct __ptcatalog_struct *walker;
struct __ptkeys *keywalker, *keylast, *keynew;
int ret;

    walker = NULL;
    tid = gettid();
    ret = 0;    
    
    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {
        
        /* Find the thread */
        walker = __ptcatalog;
        while(walker != NULL) {
            if(walker->tid == tid) {
            
                /* Now find the key */
                keywalker = walker->keys;
                keylast = NULL;
                while(keywalker != NULL) {
                    if(keywalker->id == id) {
                        keywalker->value = value;
                        break;
                    }
                    keylast = keywalker;
                    keywalker = keywalker->next;
                }
                
                /* We never found a matching key, so add it now */
                if(keywalker == NULL) {
                    keynew = (struct __ptkeys *)malloc(sizeof(struct __ptkeys));
                    if(keynew != NULL) {
                        keynew->id = id;
                        keynew->value = value;
                        keynew->next = NULL;
                        
                        if(keylast != NULL)
                            keylast->next = keynew;
                        else
                            walker->keys = keynew;
                            
                    } else {
                        ret = ENOMEM;
                    }
                }
                
                break;
            }
            walker = walker->next;
        }
        
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    return( ret );
}

void *__get_pkey_value( pthread_key_t id )
{
pid_t tid;
struct __ptcatalog_struct *walker;
struct __ptkeys *keywalker;
void *ret;

    walker = NULL;
    tid = gettid();
    ret = NULL;    
    
    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {
        
        /* Find the thread */
        walker = __ptcatalog;
        while(walker != NULL) {
            if(walker->tid == tid) {
            
                /* Now find the key */
                keywalker = walker->keys;
                while(keywalker != NULL) {
                    if(keywalker->id == id) {
                        ret = keywalker->value;
                        break;
                    }
                    keywalker = keywalker->next;
                }
                
                break;
            }
            walker = walker->next;
        }
        
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    return( ret );
}

void __call_pkey_destructor( pthread_key_t id, void *value )
{
struct __ptkeylist_struct *walker;
    
    if(pthread_mutex_lock(__ptkeylist_mutex) == 0) {
        
        walker = __ptkeylist;
        while(walker != NULL) {
            if(walker->id == id && walker->destructor != NULL) {
                walker->destructor(value);
                break;
            }
            walker = walker->next;
        }
    
        pthread_mutex_unlock(__ptkeylist_mutex);
    }
}

int __call_all_pthread_cleaners( )
{
struct __ptcatalog_struct *myself;
struct __ptcleaners *cleaner_stack;
struct __ptcleaners *previous;

    cleaner_stack = NULL;

    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {

        myself = __ptcatalog;
        previous = NULL;
        while(myself != NULL) {
            if(myself->tid == gettid()) {
                break;
            }
            myself = myself->next;
        }
        
        if(myself == NULL)
            return( EPERM );
            
        /* While we have the lock, detach the list of cleaners */
        cleaner_stack = myself->cleaners;
        myself->cleaners = NULL;
        
        /* Release the catalog now - no longer accessing it */
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    previous = NULL;
    while(cleaner_stack != NULL) {
        cleaner_stack->routine(cleaner_stack->arg);
        
        previous = cleaner_stack;
        cleaner_stack = cleaner_stack->next;
        free(previous);
    }
    
    return( 0 );
}

int __pop_pthread_cleaner( int __execute )
{
struct __ptcatalog_struct *myself;
struct __ptcleaners *popped;

    popped = NULL;
    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {

        myself = __ptcatalog;
        while(myself != NULL) {
            if(myself->tid == gettid()) {
                break;
            }
            myself = myself->next;
        }
        
        if(myself == NULL)
            return( EPERM );
            
        popped = myself->cleaners;
        if(popped != NULL) {
            myself->cleaners = popped->next;
            popped->next = NULL;
        }
        
        /* Release the catalog now - no longer accessing it */
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    if(popped != NULL) {
        if(__execute != 0)
            popped->routine(popped->arg);
        free(popped);
    }
    
    return( 0 );
}

int __push_pthread_cleaner( void (*__routine)(void*), void *__arg )
{
struct __ptcatalog_struct *myself;
struct __ptcleaners *newcleaner;

    if(__routine == NULL)
        return( EINVAL );

    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {

        myself = __ptcatalog;
        while(myself != NULL) {
            if(myself->tid == gettid()) {
                break;
            }
            myself = myself->next;
        }
        
        if(myself == NULL)
            return( EPERM );
            
        newcleaner = (struct __ptcleaners *)malloc(sizeof(struct __ptcleaners));
        if(newcleaner == NULL)
            return( ENOMEM );
            
        newcleaner->routine = __routine;
        newcleaner->arg = __arg;
        newcleaner->next = myself->cleaners;
        
        myself->cleaners = newcleaner;
        
        /* Release the catalog now - no longer accessing it */
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    return( 0 );
}

pthread_t __register_thread( )
{
struct __ptcatalog_struct *newthread;
struct __ptcatalog_struct *walker;

    newthread = (struct __ptcatalog_struct *)malloc(sizeof(struct __ptcatalog_struct));
    if( newthread == NULL ) {
        _RWD_errno = ENOMEM;
        return( (pthread_t)-1 );
    }
    
    newthread->tid = gettid();
    
    /* Initialize some aspects of our pthread object */
    newthread->pt = newthread->tid;
    
    newthread->running_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if(newthread->running_mutex != NULL)
         pthread_mutex_init(newthread->running_mutex, NULL);

    newthread->waiting_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if(newthread->waiting_mutex != NULL)
         pthread_mutex_init(newthread->waiting_mutex, NULL);
    
    newthread->waiters = 0;
    
    newthread->cancel_status = PTHREAD_CANCEL_ENABLE + PTHREAD_CANCEL_DEFERRED;
    
    /* Only store to the linked list of keys when requested */
    newthread->keys = NULL;
    
    /* Initialize the list of cleaners */
    newthread->cleaners = NULL;
    
    newthread->return_value = NULL;
    /* Thread internal data is now initialized */


    /* Check if we've ever created a thread */
    if(__ptcatalog_mutex == NULL) {
        __ptcatalog_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        if(__ptcatalog_mutex != NULL)
            pthread_mutex_init(__ptcatalog_mutex, NULL);
    }

    
    /* Attach to the linked list */
    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {
        if(__ptcatalog == NULL)
            __ptcatalog = newthread;
        else {
            walker = __ptcatalog;
            while(walker->next != NULL) walker = walker->next;
            walker->next = newthread;
        }
        pthread_mutex_unlock(__ptcatalog_mutex);
    } else {
        _RWD_errno = EBUSY;
        return( (pthread_t)-1 );
    }
    
    return( newthread->pt );
}

static struct __ptcatalog_struct *__remove_thread( pid_t tid )
{
struct __ptcatalog_struct *walker, *previous;
struct __ptkeys *keywalker, *keynext;

    walker = NULL;

    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {

        walker = __ptcatalog;
        previous = NULL;
        while(walker != NULL) {
            if(walker->tid == tid) {
                break;
            }
            previous = walker;
            walker = walker->next;
        }
        
        /* Remove it from the linked list */
        if(walker != NULL && previous != NULL)
            previous->next = walker->next;
        else if(walker != NULL)
            __ptcatalog = walker->next;
        
        /* Release the catalog now - no longer accessing it */
        pthread_mutex_unlock(__ptcatalog_mutex);
        
        /* Remove its last link and free*/
        if(walker != NULL) {
            walker->next = NULL;
            
            /* Clean up and destroy appropriate keys */
            keywalker = walker->keys;
            keynext = NULL;
            while(keywalker != NULL) {
                if(keywalker->value != NULL) {
                    __call_pkey_destructor(keywalker->id, keywalker->value);
                }
                keynext = keywalker->next;
                free(keywalker);
                keywalker = keynext;
            }
            
            /* And free the memory */
            free(walker);
        }
        
    }
    return( __ptcatalog );
}

/* NOTE: Catalog must be locked first! */
static struct __ptcatalog_struct *__get_thread_catalog_entry( pthread_t thread )
{
struct __ptcatalog_struct *walker;
    
    walker = __ptcatalog;
    while(walker != NULL) {
        if(pthread_equal(walker->pt, thread) == 1)
            break;
        
        walker = walker->next;
    }
    
    return( walker );
}


void __set_thread_return_value( pthread_t thread, void *value )
{
struct __ptcatalog_struct *walker;

    walker = NULL;

    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {
        walker = __get_thread_catalog_entry(thread);
        if(walker != NULL)
            walker->return_value = value;
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
}

void *__get_thread_return_value( pthread_t thread )
{
struct __ptcatalog_struct *walker;
void *ret;

    ret = NULL;

    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {
        walker = __get_thread_catalog_entry(thread);
        if(walker != NULL)
            ret = walker->return_value;
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    return( ret );
}

pthread_mutex_t *__get_thread_waiting_mutex( pthread_t thread )
{
struct __ptcatalog_struct *walker;
pthread_mutex_t *ret;

    ret = NULL;

    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {
        walker = __get_thread_catalog_entry(thread);
        if(walker != NULL)
            ret = walker->waiting_mutex;
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    return( ret );
}

pthread_mutex_t *__get_thread_running_mutex( pthread_t thread )
{
struct __ptcatalog_struct *walker;
pthread_mutex_t *ret;

    ret = NULL;

    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {
        walker = __get_thread_catalog_entry(thread);
        if(walker != NULL)
            ret = walker->running_mutex;
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    return( ret );
}

int __increment_thread_waiters( pthread_t thread )
{
struct __ptcatalog_struct *walker;
int ret;

    ret = 0;

    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {
        walker = __get_thread_catalog_entry(thread);
        if(walker != NULL) {
            walker->waiters++;
            ret = walker->waiters;
        }
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    return( ret );
}

int __decrement_thread_waiters( pthread_t thread )
{
struct __ptcatalog_struct *walker;
int ret;

    ret = 0;

    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {
        walker = __get_thread_catalog_entry(thread);
        if(walker != NULL) {
            walker->waiters--;
            ret = walker->waiters;
        }
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    return( ret );
}

int __get_thread_waiters_count( pthread_t thread )
{
struct __ptcatalog_struct *walker;
int ret;

    ret = 0;

    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {
        walker = __get_thread_catalog_entry(thread);
        if(walker != NULL) {
            ret = walker->waiters;
        }
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    return( ret );
}

pid_t __get_thread_id( pthread_t thread )
{
struct __ptcatalog_struct *walker;
pid_t ret;

    ret = 0;

    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {
        walker = __get_thread_catalog_entry(thread);
        if(walker != NULL) {
            ret = walker->tid;
        }
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    return( ret );
}

int __set_thread_cancel_status( pthread_t thread, int status )
{
struct __ptcatalog_struct *walker;
int ret;

    ret = 0;

    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {
        walker = __get_thread_catalog_entry(thread);
        if(walker != NULL) {
            walker->cancel_status = status;
            ret = status;
        }
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    return( ret );
}

int __get_thread_cancel_status( pthread_t thread )
{
struct __ptcatalog_struct *walker;
int ret;

    ret = 0;

    if(pthread_mutex_lock(__ptcatalog_mutex) == 0) {
        walker = __get_thread_catalog_entry(thread);
        if(walker != NULL)
            ret = walker->cancel_status;
        pthread_mutex_unlock(__ptcatalog_mutex);
    }
    
    return( ret );
}

void __unregister_thread( pthread_t thread )
{
    if(thread != NULL) {
        __ptcatalog = __remove_thread(thread);
    }
}

void __unregister_current_thread( )
{
    __unregister_thread( __get_current_thread() );
}

pthread_t __get_current_thread( )
{
#ifdef __LINUX__
    return( __get_thread(gettid()) );
#else
    return( NULL );
#endif
}
