#include <process.h>
#include <stdio.h>
#include <stddef.h>
#define INCL_DOS
#include <os2.h>

static  volatile int    NumThreads;
static  volatile int    HoldThreads;

#define NUM_THREADS     5
#define STACK_SIZE      32768

static void a_thread( void *arglist )
/***********************************/
{
    while( HoldThreads ) {
        DosSleep( 1 );
    }
    printf( "Hi from thread %d\n", *_threadid );
    DosEnterCritSec();
    --NumThreads;
    DosExitCritSec();
    _endthread();
}

int main( void )
/**************/
{
    int         i;

    printf( "Initial thread id = %d\n", *_threadid );
    NumThreads = 0;
    HoldThreads = 1;
    /* initial thread counts as 1 */
    for( i = 2; i <= NUM_THREADS; ++i ) {
        if( _beginthread( a_thread, NULL, STACK_SIZE, NULL ) == -1 ) {
            printf( "creation of thread %d failed\n", i );
        } else {
            ++NumThreads;
        }
    }
    HoldThreads = 0;
    while( NumThreads != 0 ) {
        DosSleep( 1 );
    }
    return( 0 );
}
