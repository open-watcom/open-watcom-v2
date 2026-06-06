/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  internal pthread declarations
*
****************************************************************************/


#ifndef _PT_INTERNAL_H_INCLUDED
#define _PT_INTERNAL_H_INCLUDED

#include <sys/types.h>
#include <pthread.h>

/* Cancellation signal, not to be used outside runtime lib */
#define SIGCANCEL 36

/* Thread bookkeeping */
extern pthread_t        __register_thread( void );
extern pthread_t        __get_thread( pid_t tid );
extern pthread_t        __get_current_thread( void );
extern void             __unregister_thread( pthread_t thread );
extern void             __unregister_current_thread( void );

/* Thread-specific keys bookkeeping */
extern pthread_key_t    __register_pkey( void (*destructor)(void *) );
extern void             __destroy_pkey( pthread_key_t id );
extern int              __valid_pkey_id( pthread_key_t id );
extern int              __set_pkey_value( pthread_key_t id, void *value );
extern void             *__get_pkey_value( pthread_key_t id );

/* Thread-specific cleanup bookkeeping */
extern int              __push_pthread_cleaner( void (*__routine)(void *), void *__arg );
extern int              __pop_pthread_cleaner( int __execute );
extern int              __call_all_pthread_cleaners( void );

/* Accessing internal thread data */
extern void             __set_thread_return_value( pthread_t thread, void *value );
extern void             *__get_thread_return_value( pthread_t thread );
extern pthread_mutex_t  *__get_thread_running_mutex( pthread_t thread );
extern pthread_mutex_t  *__get_thread_waiting_mutex( pthread_t thread );
extern int              __increment_thread_waiters( pthread_t thread );
extern int              __decrement_thread_waiters( pthread_t thread );
extern int              __get_thread_waiters_count( pthread_t thread );
extern pid_t            __get_thread_id( pthread_t thread );
extern int              __set_thread_cancel_status( pthread_t thread, int status );
extern int              __get_thread_cancel_status( pthread_t thread );
extern int              __set_thread_detached( pthread_t thread );
extern int              __get_thread_detached( pthread_t thread );

/* Check if a mutex is owned by the current thread
 * 0  = yes
 * -1 = no
 * >0 = error (no)
 */
extern int              __pthread_mutex_mylock(pthread_mutex_t *__mutex);

#endif /* _PT_INTERNAL_H_INCLUDED */
