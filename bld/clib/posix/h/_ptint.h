#ifndef _PT_INTERNAL_H_INCLUDED
#define _PT_INTERNAL_H_INCLUDED

#include <sys/types.h>
#include <pthread.h>

/* Cancellation signal, not to be used outside runtime lib */
#define SIGCANCEL 36

/* Thread bookkeeping */
extern pthread_t        __register_thread();
extern pthread_t        __get_thread( pid_t tid );
extern pthread_t        __get_current_thread( );
extern void             __unregister_thread( pthread_t thread );
extern void             __unregister_current_thread( );

/* Thread-specific keys bookkeeping */
extern pthread_key_t    __register_pkey( void (*destructor)(void *) );
extern void             __destroy_pkey( pthread_key_t id );
extern int              __valid_pkey_id( pthread_key_t id );
extern int              __set_pkey_value( pthread_key_t id, void *value );
extern void             *__get_pkey_value( pthread_key_t id );

/* Thread-specific cleanup bookkeeping */
extern int              __push_pthread_cleaner( void (*__routine)(void *), void *__arg );
extern int              __pop_pthread_cleaner( int __execute );
extern int              __call_all_pthread_cleaners( );

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

/* Check if a mutex is owned by the current thread 
 * 0  = yes
 * -1 = no
 * >0 = error (no)
 */
extern int              __pthread_mutex_mylock(pthread_mutex_t *__mutex);

#endif /* _PT_INTERNAL_H_INCLUDED */
