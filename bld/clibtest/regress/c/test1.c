/* test1.c (OS/2 Startup Test 1)  */

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

#include <os2.h>

void dll_threadfunc( void* private_data )
{
    static int counter = 0;

    ++counter;
    printf( "DLL threadfunc entered %2d time. This time during %s\n",
            counter, private_data );
}

void do_start_threads( const char* const szMsg )
{
    int i;

    printf( "In DLL do_start_threads called from %s\n", szMsg );
    for( i = 0; i < 10; i++ ) {
        _beginthread( &dll_threadfunc, NULL, 8192, (void*)szMsg );
    }
}

static const char* const rgMsgs[2] =
{
    "Process initialization\n",
    "Process termination\n"
};

unsigned APIENTRY LibMain( unsigned hmod, unsigned termination )
{
    if( termination )
        printf( rgMsgs[1] );
    else
        printf( rgMsgs[0] );
    return( 1 );
}

void __export QA_func1( void )
{
    do_start_threads( "QA_func1" );
    sleep( 1 /* second */); // Let'em die
}

#endif /* QA_MAKE_DLL */

#if defined(QA_MAKE_EXE)

extern void QA_func1( void );

// check that threading works at all in the exe
void exe_threadfunc( void* private_data )
{
    static int counter = 0;

    ++counter;
    printf( ".exe threadfunc entered %2d times.\n", counter );
}

void do_start_threads( void )
{
    int i;

    printf( "in .exe: do_start_threads\n" );
    for( i = 0; i < 10; ++i ) {
        _beginthread( &exe_threadfunc, NULL, 8192, 0 );
    }
}


int main()
{
    do_start_threads();
    sleep( 1 /* second */); // Let'em die
    QA_func1();
    return 0;
}

#endif /* QA_MAKE_EXE */
