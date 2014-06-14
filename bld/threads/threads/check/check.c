
#include <stdio.h>
#include <stdlib.h>
#include "threads.h"

mtx_t console_lock;

int thread1( void *arg )
{
    mtx_lock( &console_lock );
    printf("Thread1\n");
    mtx_unlock( &console_lock );
    return 0;
}


int thread2( void *arg )
{
    mtx_lock( &console_lock );
    printf("Thread2\n");
    mtx_unlock( &console_lock );
    return 0;
}


int main( void )
{
    thrd_t thrd1;
    thrd_t thrd2;
    int result;

    mtx_init( &console_lock, mtx_plain );
    thrd_create( &thrd1, thread1, NULL );
    thrd_create( &thrd2, thread2, NULL );
    thrd_join( thrd1, &result );
    thrd_join( thrd2, &result );
    mtx_destroy( &console_lock );
    return EXIT_SUCCESS;
}
