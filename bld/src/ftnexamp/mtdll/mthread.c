/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <malloc.h>
#include <process.h>
#include <stdio.h>
#include <stddef.h>
#if defined( __cplusplus )
extern "C" {
#endif
#if defined( __NT__ )
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #define TID                   DWORD
#elif defined( __QNX__ )
  #include <semaphor.h>
  #include <sys/types.h>
  #define TID                   pid_t
#else
  #define INCL_DOS
  #include <os2.h>
#endif
#if defined( __cplusplus )
}
#endif

static volatile int     NumThreads;
static volatile int     HoldThreads;
#if defined( __NT__ )
    static      HANDLE  ThreadSem;
#elif defined( __QNX__ )
    static      sem_t   ThreadSem;
#endif

#define STACK_SIZE      8192

unsigned        __MaxThreads = { 32 };

typedef void (*PFUN)( void );

#if defined( __cplusplus )
#include <iostream.h>
#include <iomanip.h>

extern "C" {
#endif
#if !defined( __QNX__ )
  extern        void            dll_print(int);
  extern        void            dll_entry_1(void);
  extern        void            dll_entry_2(void);
  extern        void            dll_begin_thread(PFUN);
  #define do_print(x)           dll_print(x)
  #define do_entry_1()          dll_entry_1()
  #define do_entry_2()          dll_entry_2()
  #define do_begin_thread(x)    dll_begin_thread(x)
  #if !defined( __386__ ) && !defined( __AXP__ )
    extern void                 dll_entry_terminate(void);
  #endif
#else
  extern        void            local_print(int);
  extern        void            local_entry_1(void);
  extern        void            local_entry_2(void);
  #define do_print(x)           local_print(x)
  #define do_entry_1()          local_entry_1()
  #define do_entry_2()          local_entry_2()
  #define do_begin_thread(x)    ((void)0)
#endif
#if defined( __cplusplus )
}
#include "testmemb.h"

TestMember Instance( "File Scope Object" );

#endif

#if (defined( __386__ ) || defined( __AXP__ )) && !defined( __QNX__ )
static void     dll_thread( void ) {
/**********************************/
    #if defined( __cplusplus )
        char buf[80];
        sprintf( buf, "Local Scope in EXE from DLL object %d", *_threadid );
        TestMember Instance( buf );
        printf( "TestMember Ctor: %s\n", Instance.id );
    #endif
    printf( "Hi from thread %d in user code called by DLL thread\n", *_threadid );
}
#endif


static  void    a_thread( void *args ) {
/**************************************/

    int         i;

    args = args;

    #if defined( __cplusplus )
        char buf[80];
        sprintf( buf, "Local Scope object %d", *_threadid );
        TestMember Instance( buf );
        printf( "TestMember Ctor: %s\n", Instance.id );
    #endif

    while( HoldThreads ) {
        #if defined( __NT__ )
            Sleep( 0 );
        #elif defined( __QNX__ )
            sleep( 0 );
        #else
            DosSleep( 0 );
        #endif
    }
    #if defined( __cplusplus )
        Instance.Member1( 2, 1 );
        Instance.Member2( 3, 1 );
        Instance.Member3( 4, 1 );
    #endif
    for( i = 1; i <= 10; ++i ) {
        do_print( i );
        #if defined( __386__ ) || defined( __AXP__ )
            do_begin_thread( dll_thread );
        #endif
    }
    #if defined( __NT__ )
        WaitForSingleObject( ThreadSem, -1 );
    #elif defined( __QNX__ )
        sem_wait( &ThreadSem );
    #else
        DosEnterCritSec();
    #endif
    --NumThreads;
    #if defined( __NT__ )
        ReleaseMutex( ThreadSem );
    #elif defined( __QNX__ )
        sem_post( &ThreadSem );
    #else
        DosExitCritSec();
    #endif
    #if !defined( __cplusplus )
        _endthread();
    #endif
}

int     main() {
/**************/

    int         i;
    TID         tid;
    void        *stk;
    unsigned    start_count;

    #if defined( __386__ ) || defined( __AXP__ )
        start_count = __MaxThreads * 2;
    #else
        start_count = __MaxThreads;
    #endif

    #if defined( __cplusplus )
        printf( "TestMember Ctor: %s\n", Instance.id );
        cout << "main thread id = " << *_threadid << endl;
        Instance.Member1( 2, 1 );
        Instance.Member2( 3, 1 );
        Instance.Member3( 4, 1 );
    #else
        printf( "main thread id = %d\n", *_threadid );
    #endif
    NumThreads = 0;
    HoldThreads = 1;
    #if defined( __NT__ )
        ThreadSem = CreateMutex( NULL, FALSE, NULL );
    #elif defined( __QNX__ )
        sem_init( &ThreadSem, 1, 1 );
    #endif
    do_entry_1();
    /* main thread counts as 1 */
    for( i = 2; i <= start_count; ++i ) {
        #if (defined( __386__ ) || defined( __AXP__ )) && !defined( __QNX__ )
            stk = NULL;
        #else
            stk = malloc( STACK_SIZE );
            if( stk == NULL ) {
                printf( "stack allocation failed for %d\n", i );
            }
        #endif
        #if defined( __NT__ )
            tid = (TID)_beginthread( a_thread, STACK_SIZE, NULL );
        #else
            tid = _beginthread( a_thread, stk, STACK_SIZE, NULL );
        #endif
        if( tid == -1 ) {
            printf( "creation of thread %d failed\n", i );
        } else {
            ++NumThreads;
        }
    }
    do_entry_2();
    #if defined( __386__ ) || defined( __AXP__ )
        do_begin_thread( dll_thread );
    #endif
    HoldThreads = 0;
    while( NumThreads != 0 ) {
        #if defined( __NT__ )
            Sleep( 0 );
        #elif defined( __QNX__ )
            sleep( 0 );
        #else
            DosSleep( 0 );
        #endif
    };
    #if defined( __386__ ) || defined( __AXP__ )
        HoldThreads = 1;
        NumThreads = 1;
        #if defined( __NT__ )
            tid = (TID)_beginthread( a_thread, STACK_SIZE, NULL );
        #else
            tid = _beginthread( a_thread, stk, STACK_SIZE, NULL );
        #endif
        if( tid == -1 ) {
            printf( "creation of thread %d failed\n", i );
        } else {
            printf( "Killing thread %d\n", tid );
            #if defined( __NT__ )
                TerminateThread( (HANDLE)tid, 0 );
                HoldThreads = 0;
            #elif defined( __QNX__ )
                HoldThreads = 0;
            #else
                DosKillThread( tid );
                HoldThreads = 0;
            #endif
        }
    #endif
    #if defined( __NT__ )
        CloseHandle( ThreadSem );
    #elif defined( __QNX__ )
        sem_destroy( &ThreadSem );
    #endif
    #if !defined(__386__) && !defined( __AXP__ )
        dll_entry_terminate();
    #endif
    return( 0 );
}

#if defined( __QNX__ )

#if defined( __cplusplus )

#include <iostream.h>
#include <iomanip.h>


static  void    print( char *str ) {
//==================================

    __lock_it( cout.__i_lock );
    cout << str << flush;
}

struct TestCtorDtor {
    TestCtorDtor();
    ~TestCtorDtor();
    int xx;
};
TestCtorDtor::TestCtorDtor() {
    xx = 0x1234;
}
TestCtorDtor::~TestCtorDtor() {
    print( "DLL File scope dtor\n" );
}

TestCtorDtor Instance;

#else

static  void    print( char *str ) {
//==================================

    printf( str );
}

#endif

void local_entry_1() {
//====================

    print( "hi from dll entry #1\n" );
}


void local_entry_2() {
//====================

    print( "hi from dll entry #2\n" );
}


void local_print( int i ) {
//=========================

    printf( "Hi from thread %d counter = %d\n", *_threadid, i );
}
#endif
