#include <malloc.h>
#include <process.h>
#include <stdio.h>
#include <stddef.h>

#include "fail.h"

#if defined( __NT__ ) && defined( __386__ )
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #define TID                 DWORD

static volatile int     NumThreads;
static volatile int     HoldThreads;
static  HANDLE  ThreadSem;

#define STACK_SIZE      8192
#define _THREADID (*_threadid)

unsigned        __MaxThreads = { 4 };

typedef void (*PFUN)( void );

__declspec(thread) int file_integer = 1;


static  void    a_thread( void *args ) {
/**************************************/

    int         i;
    static __declspec(thread) int auto_integer = 100;

    args = args;

    while( HoldThreads ) {
        Sleep( 0 );
    }
    file_integer++;
    if( file_integer != 2 ) {
        printf( "file_integer is not local to this thread: %d\n", _THREADID );
        _fail;
    }
    auto_integer++;
    if( auto_integer != 101 ) {
        printf( "auto_integer is not local to this thread: %d\n", _THREADID );
        _fail;
    }
    WaitForSingleObject( ThreadSem, -1 );
    --NumThreads;
    ReleaseMutex( ThreadSem );
}

void    test() {
/**************/
    int         i;
    TID         tid;
    unsigned    start_count;

    start_count = __MaxThreads;

    NumThreads = 0;
    HoldThreads = 1;
    ThreadSem = CreateMutex( NULL, FALSE, NULL );
    /* main thread counts as 1 */
    for( i = 2; i <= start_count; ++i ) {
        tid = (TID)_beginthread( a_thread, STACK_SIZE, 0 );
        if( tid == (TID)-1 ) {
            printf( "creation of thread %d failed\n", i );
            _fail;
        } else {
            ++NumThreads;
        }
    }
    HoldThreads = 0;
    while( NumThreads != 0 ) {
        Sleep( 0 );
    }
    CloseHandle( ThreadSem );
}

#else
#define test()
#endif


int main()
{
    test();
    _PASS;
}
