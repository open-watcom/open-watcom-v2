::
:: POSIX Threads typedef declaration 2
::
:segment LINUX
:segment INITIALIZERS
/* Initializers */
:elsesegment
#ifndef _PTHREAD_X_DEFINED_
 #define _PTHREAD_X_DEFINED_
:endsegment
::
:segment !INITIALIZERS
 typedef struct {
     sem_t           mutex;
     volatile pid_t  owner;
     int             type;
 } pthread_mutex_t;
:elsesegment INITIALIZERS
#define PTHREAD_MUTEX_INITIALIZER   { { 1, 1 }, (pid_t)(-1), PTHREAD_MUTEX_DEFAULT }
:endsegment
::
:segment !INITIALIZERS
 typedef struct {
     int         type;
 } pthread_mutexattr_t;
:endsegment
::
:segment !INITIALIZERS
 typedef struct {
    sem_t           wait_block;
    sem_t           clear_block;
    volatile int    waiters;
 } pthread_cond_t;
:elsesegment INITIALIZERS
#define PTHREAD_COND_INITIALIZER    { { 1, 0 }, { 1, 1 }, 0 }
:endsegment
::
:segment !INITIALIZERS
 typedef struct pthread_condattr_t pthread_condattr_t;
 typedef int     pthread_key_t;
:endsegment
::
:segment !INITIALIZERS
 typedef struct {
     pthread_mutex_t access;
     int             executed;
 } pthread_once_t;
:elsesegment INITIALIZERS
#define PTHREAD_ONCE_INIT           { PTHREAD_MUTEX_INITIALIZER, 0 }
:endsegment
::
:segment !INITIALIZERS
 typedef struct {
     pthread_mutex_t block_mutex;
     int         read_waiters;
 } pthread_rwlock_t;
:elsesegment INITIALIZERS
#define PTHREAD_RWLOCK_INITIALIZER  { PTHREAD_MUTEX_INITIALIZER, 0 }
:endsegment
::
:segment !INITIALIZERS
 typedef struct pthread_rwlockattr_t pthread_rwlockattr_t;
 typedef struct {
     int         *value;
 } pthread_spinlock_t;
 typedef struct {
     pthread_mutex_t *access;
     pthread_cond_t  *cond;
     unsigned        count;
     unsigned        limit;
 } pthread_barrier_t;
 typedef struct pthread_barrierattr_t pthread_barrierattr_t;
:endsegment
::
:segment !INITIALIZERS
#endif
:endsegment
:elsesegment QNX
:elsesegment
:endsegment
