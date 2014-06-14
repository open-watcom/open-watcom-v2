
#include <stdio.h>
#include <pthread.h>
#include <windows.h>

void *thread_function( void *argument )
{
    printf("I'm in the thread and I want to sleep!\n");
    Sleep(20000);
    printf("I'm in the thread and I want to end!\n");
    return NULL;
}

int main( void )
{
    pthread_t thread_ID;

    _pthread_initialize( );
    pthread_create( &thread_ID, NULL, thread_function, NULL );
    printf("I'm in main and I want to sleep!\n");
    Sleep(10000);
    printf("I'm in main and I want to join!\n");
    pthread_join( thread_ID, NULL );
    _pthread_cleanup( );
    return 0;
}

