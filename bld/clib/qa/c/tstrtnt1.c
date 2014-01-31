/* tstsrtnt1.c (Tests Startup NT 1)  */
/*
 * Code to verify the movement of thread data heap allocation
 * for DLL's from __NTInit to _LibMain (where it really belong).
 *
 */

#if !defined(QA_MAKE_EXE) && !defined(QA_MAKE_DLL)
#error You must define either of QA_MAKE_EXE or QA_MAKE_DLL
#endif
#if defined(QA_MAKE_EXE) && defined(QA_MAKE_DLL)
#error You can only define one of QA_MAKE_EXE or QA_MAKE_DLL
#endif

#include <stdio.h>
#include <process.h>
#include <dos.h>

#if defined(QA_MAKE_DLL)

#include <windows.h>

void threadfunc_dll( void* private_data )
{
    static int counter = 0;

    ++counter;
    printf( "DLL threadfunc entered %2d time. This time during %s\n",
            counter, private_data);
}

void do_start_threads(const char* const szMsg)
{
    int i;

    printf( "In DLL do_start_threads called from %s\n", szMsg );
    for( i = 0; i < 10; i++) {
        _beginthread( &threadfunc_dll, 4096, /* remove const */ (void*)szMsg );
    }
}

static const char* const rgMsgs[4] =
{
    "DLL_PROCESS_ATTACH\n",
    "DLL_PROCESS_DETACH\n",
    "DLL_THREAD_ATTACH\n",
    "DLL_THREAD_DETACH\n"
};

int __stdcall DllMain( HANDLE hdll, DWORD reason, LPVOID reserved )
{
    switch( reason ) {
    case DLL_PROCESS_ATTACH:
        printf(rgMsgs[0]);
        break;

    case DLL_PROCESS_DETACH:
        printf(rgMsgs[1]);
        break;

    case DLL_THREAD_ATTACH:
        printf(rgMsgs[2]);
        break;

    case DLL_THREAD_DETACH:
        printf(rgMsgs[3]);
        break;
    }
    return( TRUE );
}

__declspec(dllexport)
void QA_func1( void )
{
    do_start_threads( "QA_func1" );
    sleep( 1 /* second */); // Let'em die
}

#endif /* QA_MAKE_DLL */

#if defined(QA_MAKE_EXE)

__declspec(dllimport)
void QA_func1( void );

// check that threading works at all in the exe
void exe_threadfunc( void* private_data )
{
    static counter = 0;
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


int main()
{
    do_start_threads();
    sleep( 1 /* second */); // Let'em die
    QA_func1();
    return( 0 );
}

#endif /* QA_MAKE_EXE */
