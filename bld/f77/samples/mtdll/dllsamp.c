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


#include <io.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <process.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if defined( __cplusplus )
extern "C" {
#endif
#if defined( __NT__ )
#include <windows.h>
#else
#define INCL_DOS
#include <os2.h>
#endif
#if defined( __cplusplus )
}
#endif

#define STACK_SIZE      8192

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


#if defined( __cplusplus )
extern "C" {
#endif
void DLLEXPORT dll_entry_1() {
//=====================

    print( "hi from dll entry #1\n" );
}


void DLLEXPORT dll_entry_2() {
//=====================

    print( "hi from dll entry #2\n" );
}


void DLLEXPORT dll_print( int i ) {
//==========================

    printf( "Hi from thread %d counter = %d\n", *_threadid, i );
}

static volatile int     WaitForThread;

typedef void (*PFUN)( void );
static void dll_thread( void *args ) {
    PFUN fun;
    fun = (PFUN) args;
    printf( "Hi from thread %d started in DLL\n", *_threadid );
    (*fun)();
    WaitForThread = 0;
    #ifndef __cplusplus
        _endthread();
    #endif
}

void DLLEXPORT dll_begin_thread( PFUN fun ) {
//==========================

    void        *stk;
    int         rc;
    printf( "Hi from thread %d starting new thread in DLL\n", *_threadid );
    #if defined( __386__ ) || defined( __AXP__ )
        stk = NULL;
    #else
        stk = malloc( STACK_SIZE );
        if( stk == NULL ) {
            printf( "Stack allocation failed for thread in DLL\n" );
        }
    #endif
    WaitForThread = 1;
    #if defined( __NT__ )
        rc = (int)_beginthread( dll_thread, STACK_SIZE, (void *)fun );
    #else
        rc = _beginthread( dll_thread, stk, STACK_SIZE, (void *)fun );
    #endif
    if( rc == -1 ) {
        printf( "Creation of DLL thread failed\n" );
    } else {
        while( WaitForThread ) {
            #if defined( __NT__ )
                Sleep( 0 );
            #else
                DosSleep( 0 );
            #endif
        }
        printf( "DLL thread %d terminated\n", rc );
    }
}

static void _dll_print2( char *p, int i ) {
//==========================

    printf( "%s, thread %d difference = %d\n", p, *_threadid, i );
}

#if defined( __cplusplus )
}

#include "testmemb.h"

DLLEXPORT __cdecl TestMember::TestMember( char *p ) {
    id = p;
}

DLLEXPORT __pascal TestMember::~TestMember() {
    printf( "TestMember Dtor: %s\n", id );
}

void DLLEXPORT TestMember::Member1( int a, int b ) {
    _dll_print2( id, a - b );
}

void DLLEXPORT __cdecl TestMember::Member2( int a, int b ) {
    _dll_print2( id, a - b );
}

void DLLEXPORT __pascal TestMember::Member3( int a, int b ) {
    _dll_print2( id, a - b );
}

#endif


#if defined( __NT__ )

INT     APIENTRY LibMain( HANDLE hdll, ULONG reason, LPVOID reserved ) {
//======================================================================

    hdll = hdll; reserved = reserved;
    switch( reason ) {
    case DLL_PROCESS_ATTACH:
        #ifdef __cplusplus
            if( Instance.xx == 0x1234 ) {
                printf( "DLL File scope ctor ran\n" );
            } else {
                printf( "DLL File scope ctor did NOT run!\n" );
            }
        #endif
        printf( "DLL initialized\n" );
        return( 1 );
    case DLL_PROCESS_DETACH:
        printf( "DLL terminated\n" );
        return( 1 );
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
        return( 1 );
    }
}


#else

#if defined( __386__ ) || defined( __AXP__ )
void my_shutdown( void ) {
    printf( "DLL terminated\n" );
}

unsigned __syscall DllMain( unsigned hmod, unsigned termination ) {

    if( !termination ) {
        atexit( &my_shutdown );
        printf( "DLL initialized\n" );
    }
    return( 1 );
}
#else
#if defined( __cplusplus )
extern "C" {
#endif
int     __dll_initialize() {
//==========================

    printf( "DLL initialized\n" );
    return( 1 );
}


void DLLEXPORT dll_entry_terminate(void) {
//=================================

    int file;

    file = open( "CON", O_WRONLY );
    if( file != -1 ) {
        #define MSG "DLL terminated\n"
        write( file, MSG, sizeof( MSG )-1 );
        close( file );
    }
}
#if defined( __cplusplus )
}
#endif
#endif


#endif
