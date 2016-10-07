#ifndef _PT_INTERNAL_H_INCLUDED
#define _PT_INTERNAL_H_INCLUDED

#include <sys/types.h>
#include <pthread.h>

/* Thread bookkeeping */
pthread_t __register_thread();
pthread_t __get_thread( pid_t tid );
pthread_t __get_current_thread( );
void __unregister_thread( pthread_t thread );
void __unregister_current_thread( );

/* Thread-specific keys bookkeeping */
pthread_key_t __register_pkey( void (*destructor)(void*) );
void __destroy_pkey( pthread_key_t id );
int __valid_pkey_id( pthread_key_t id );
int __set_pkey_value( pthread_key_t id, void *value );
void *__get_pkey_value( pthread_key_t id );

/* Thread-specific cleanup bookkeeping */
int __push_pthread_cleaner( void (*__routine)(void*), void *__arg );
int __pop_pthread_cleaner( int __execute );
int __call_all_pthread_cleaners( );

/* Accessing internal thread data */
void __set_thread_return_value( pthread_t thread, void *value );
void *__get_thread_return_value( pthread_t thread );
pthread_mutex_t *__get_thread_running_mutex( pthread_t thread );
pthread_mutex_t *__get_thread_waiting_mutex( pthread_t thread );
int __increment_thread_waiters( pthread_t thread );
int __decrement_thread_waiters( pthread_t thread );
int __get_thread_waiters_count( pthread_t thread );
pid_t __get_thread_id( pthread_t thread );
int __set_thread_cancel_status( pthread_t thread, int status );
int __get_thread_cancel_status( pthread_t thread );

/* Check if a mutex is owned by the current thread 
 * 0  = yes
 * -1 = no
 * >0 = error (no)
 */
int __pthread_mutex_mylock(pthread_mutex_t *__mutex);

/* Cancellation signal, not to be used outside runtime lib */
#define SIGCANCEL 36

#endif /* _PT_INTERNAL_H_INCLUDED */
