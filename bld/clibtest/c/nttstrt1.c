/* nttstsrt1.c - NT Startup Tests 1  */
/*
 * Code to verify the movement of thread data heap allocation
 * for DLL's from __NTInit to _LibMain (where it really belong).
 *
 */

#include <stddef.h>
#include <stdio.h>

#ifdef __NT__

#include <process.h>
#include <dos.h>

#if defined(QA_MAKE_DLL)

#include <windows.h>

void threadfunc_dll( void *private_data )
{
    static int counter = 0;

    ++counter;
    printf( "DLL threadfunc entered %2d time. This time during %s\n",
            counter, private_data );
}

void do_start_threads( const char *const szMsg )
{
    int i;

    printf( "In DLL do_start_threads called from %s\n", szMsg );
    for( i = 0; i < 10; i++ ) {
        _beginthread( &threadfunc_dll, 4096, /* remove const */ (void *)szMsg );
    }
}

static const char *const rgMsgs[4] =
{
    "DLL_PROCESS_ATTACH\n",
    "DLL_PROCESS_DETACH\n",
    "DLL_THREAD_ATTACH\n",
    "DLL_THREAD_DETACH\n"
};

int __stdcall DllMain( HANDLE hdll, DWORD reason, LPVOID reserved )
{
    /* unused parameters */ (void)hdll; (void)reserved;

    switch( reason ) {
    case DLL_PROCESS_ATTACH:
        printf( rgMsgs[0] );
        break;

    case DLL_PROCESS_DETACH:
        printf( rgMsgs[1] );
        break;

    case DLL_THREAD_ATTACH:
        printf( rgMsgs[2] );
        break;

    case DLL_THREAD_DETACH:
        printf( rgMsgs[3] );
        break;
    }
    return( TRUE );
}

__declspec(dllexport)
void QA_func1( void )
{
    do_start_threads( "QA_func1" );
    sleep( 1 ); // sleep 1 second, Let'em die
}

#else /* QA_MAKE_DLL */

__declspec(dllimport)
void QA_func1( void );

// check that threading works at all in the exe
void exe_threadfunc( void *private_data )
{
    static int counter = 0;

    /* unused parameters */ (void)private_data;

    ++counter;

    printf( ".exe threadfunc entered %2d times.\n", counter );
}

void do_start_threads( void )
{
    int i;

    printf( "in .exe: do_start_threads\n" );
    for ( i = 0; i < 10; i++) {
        _beginthread( &exe_threadfunc, 4096, 0 );
    }
}


int main( void )
{
    do_start_threads();
    sleep( 1 ); // sleep 1 second, Let'em die
    QA_func1();
    return( 0 );
}

#endif /* !QA_MAKE_DLL */

#else /* !__NT__ */

#ifdef __SW_BW
    #include <wdefwin.h>
#endif

int main( void )
{
#ifdef __SW_BW
    FILE    *my_stdout;

    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        return( -1 );
    }
    fclose( my_stdout );
    _dwShutDown();
#endif
    return( 0 );
}

#endif
