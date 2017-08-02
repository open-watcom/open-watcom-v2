/* test2.c (OS/2 Threading Test 2)  */

#include <stdio.h>
#include <process.h>
#include <dos.h>

#define INCL_DOS
#include <os2.h>

/* Test thread creation - many very short-lived threads in quick
 * succession, and vary thread priority. If there are problems
 * with threads that are either too short-lived or created too
 * quickly after each other, this test should detect them.
 */

static int counter = 0;

static void exe_threadfunc1( void* private_data )
{
    DosSetPriority( PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, 0 );
    ++counter;
}

static void exe_threadfunc2( void* private_data )
{
    DosSetPriority( PRTYS_THREAD, PRTYC_IDLETIME, 0, 0 );
    ++counter;
}

static void do_start_threads1( void )
{
    int i;

    printf( "calling do_start_threads1\n" );
    for( i = 0; i < 100; ++i ) {
        _beginthread( &exe_threadfunc1, NULL, 8192, 0 );
    }
}

static void do_start_threads2( void )
{
    int i;

    printf( "calling do_start_threads2\n" );
    for( i = 0; i < 100; ++i ) {
        _beginthread( &exe_threadfunc2, NULL, 8192, 0 );
    }
}


int main()
{
    // First try threads that will exit very quickly
    do_start_threads1();
    sleep( 1 /* second */); // Let'em die
    printf( "threadfunc entered %2d times.\n", counter );
    DosSetPriority( PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, 0 );
    // Next try threads that won't finish before all have been created
    do_start_threads2();
    printf( "threadfunc entered %2d times.\n", counter );
    sleep( 1 /* second */); // Let'em die
    printf( "threadfunc entered %2d times.\n", counter );
    return( 0 );
}
