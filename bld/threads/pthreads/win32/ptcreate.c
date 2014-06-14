
#include <errno.h>
#include <process.h>
#include <pthread.h>
#include <windows.h>

struct thread_information {
    int     used;
    void   *(*thread_function)( void * );
    void   *argument;
    void   *result;
    HANDLE  thread_handle;
    HANDLE  result_semaphore;
};

static struct thread_information threads[PTHREAD_THREADS_MAX];
static CRITICAL_SECTION threads_lock;

void _pthread_initialize( )
{
    InitializeCriticalSection( &threads_lock );
}


void _pthread_cleanup( )
{
    DeleteCriticalSection( &threads_lock );
}


static void dispatching_function( void *argument )
{
    struct thread_information *my_information = (struct thread_information *)argument;
    my_information->result = my_information->thread_function( my_information-> argument );
    ReleaseSemaphore( my_information->result_semaphore, 1, NULL );
    _endthread( );
}


int pthread_create
    ( pthread_t      *thread_id,
      pthread_attr_t *attributes,
      void           *(*thread_function)( void * ),
      void           *argument )
{
    int           i;
    int           return_value = 0;
    unsigned long thread_status;

    EnterCriticalSection( &threads_lock );
    for( i = 0; i < PTHREAD_THREADS_MAX; ++i ) {
        if( threads[i].used == 0 ) break;
    }
    if( i == PTHREAD_THREADS_MAX ) {
        return_value = EAGAIN;
    }
    else {
        threads[i].used             = 1;
        threads[i].thread_function  = thread_function;
        threads[i].argument         = argument;
        threads[i].result_semaphore = CreateSemaphore( NULL, 0, 1, NULL );  /* TODO: check! */

        /* Note that the new thread might execute for a while before this function returns. */
        thread_status = _beginthread( dispatching_function, 65536, &threads[i] );
        if( thread_status != -1 ) {
            threads[i].thread_handle = (HANDLE)thread_status;
            *thread_id = i;
        }
        else {
            CloseHandle( threads[i].result_semaphore );
            threads[i].used = 0;
            return_value    = EAGAIN;
        }
    }
    LeaveCriticalSection( &threads_lock );
    return( return_value );
}


int pthread_join( pthread_t thread_id, void **result )
{
    WaitForSingleObject( threads[thread_id].result_semaphore, INFINITE );  /* TODO: check! */
    if( result != NULL ) *result = threads[thread_id].result;
    threads[thread_id].used = 0;
    return 0;
}
