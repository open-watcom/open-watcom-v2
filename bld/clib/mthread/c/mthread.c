/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. 
*    Portions Copyright (c) 2016 Open Watcom Contributors. 
*    All Rights Reserved.
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
* Description:  Core functions of the C runtime multithread support.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#if defined( __QNX__ )
  #include <sys/magic.h>
#endif
#if defined (_NETWARE_LIBC)
  #include "nw_libc.h"
#elif defined( __QNX__ ) || defined( __LINUX__ )
  #include "semapsx.h"
#endif
#include "rterrno.h"
#include "liballoc.h"
//#include "stacklow.h"
#include "thread.h"
#include "trdlist.h"
#include "mthread.h"
#include "rtinit.h"
#include "exitwmsg.h"
#include "osver.h"
#include "heapacc.h"
#include "fileacc.h"
#include "trdlstac.h"
#include "maxthrds.h"

#if !defined( _M_I86 )
  #if defined( __NT__ ) || defined( __LINUX__ )
    static semaphore_object FListSemaphore;
  #endif
    void static nullSema4Rtn( semaphore_object *p ) { p = p; }
    _WCRTDATA void (*__AccessSema4)( semaphore_object *) = &nullSema4Rtn;
    _WCRTDATA void (*__ReleaseSema4)( semaphore_object *) = &nullSema4Rtn;
    _WCRTDATA void (*__CloseSema4)( semaphore_object *) = &nullSema4Rtn;
  #if !defined( __NETWARE__ )
    static void __NullAccHeapRtn( void ) {}
  #endif
#endif

extern  int             __Sema4Fini;            // in finalizer segment
#ifdef _M_IX86
#pragma aux             __Sema4Fini "_*";
#endif
extern  void            **__ThreadIDs;

#define MAX_SEMAPHORE   16

static semaphore_object FileSemaphores[ MAX_SEMAPHORE ];

#if !defined( __NETWARE__ )
  static semaphore_object FHeapSemaphore;
  static semaphore_object NHeapSemaphore;
#endif

static semaphore_object IOBSemaphore;

#if !defined( _M_I86 )
  static semaphore_object InitSemaphore;
  static semaphore_object TDListSemaphore;
#endif

#if defined( __NT__ )

#define MAX_CRITICAL_SECTION 64
static CRITICAL_SECTION critsect_cache[MAX_CRITICAL_SECTION];
static int critsect_next;
static CRITICAL_SECTION **critsect_vector;
static int critsect_vectornext;

static CRITICAL_SECTION *__NTGetCriticalSection( void )
{
    CRITICAL_SECTION *ptr;

    if( critsect_next < MAX_CRITICAL_SECTION ) {
        ptr = &(critsect_cache[critsect_next]);
        critsect_next++;
    } else {
        ptr = lib_calloc( 1, sizeof( *ptr ) );
        if( ptr == NULL ) {
            __fatal_runtime_error(
                "Unable to allocate semaphore data", 1 );
        }
        critsect_vector = lib_realloc( critsect_vector,
                (critsect_vectornext+1)*sizeof(CRITICAL_SECTION*));
        if( critsect_vector == NULL ) {
            __fatal_runtime_error(
                "Unable to allocate semaphore data", 1 );
        }
        critsect_vector[critsect_vectornext] = ptr;
        critsect_vectornext++;
    }
    InitializeCriticalSection( ptr );
    return( ptr );
}
static void __NTDeleteCriticalSection( void ) {
    int i;
    for( i = 0 ; i < critsect_next ; i++ ) {
        DeleteCriticalSection( &(critsect_cache[i]) );
    }
}
static void __NTFreeCriticalSection( void ) {
    int i;
    for( i = 0 ; i < critsect_vectornext ; i++ ) {
        DeleteCriticalSection( critsect_vector[i] );
        lib_free( critsect_vector[i] );
    }
    if( critsect_vector ) {
        lib_free( critsect_vector );
    }
}
#endif

_WCRTLINK void __CloseSemaphore( semaphore_object *obj )
{
#if defined( __RUNTIME_CHECKS__ ) && defined( _M_IX86 )
    // 0 is ok
    // 1 is ok  // JBS I don't think so. I would mean a critical section is active.
                // JBS For every lock, there should be an unlock.
//    if( obj->count >= 2 ) {
//        __fatal_runtime_error( "Semaphore locked too many times", 1 );
//    }
    if( obj->count >= 1 ) {
        __fatal_runtime_error( "Semaphore not unlocked", 1 );
    }
#endif
#if !defined( __NT__ )
  #if defined( _M_I86 )
    if( obj->count > 0 ) {
        DosSemClear( &obj->semaphore );
    }
  #else
    if( obj->initialized != 0 ) {
    #if defined( __NETWARE__ )
        obj->semaphore = 0;
    #elif defined( __QNX__ ) || defined( __LINUX__ )
        __posix_sem_destroy( &obj->semaphore );
    #elif defined( __RDOS__ )
        RdosDeleteSection( obj->semaphore );
        obj->semaphore = 0;
    #elif defined( __RDOSDEV__ )
    #else
        DosCloseMutexSem( obj->semaphore );
    #endif
    }
  #endif
    obj->initialized = 0;
    obj->owner = 0;
    obj->count = 0;
#endif
}

_WCRTLINK void __AccessSemaphore( semaphore_object *obj )
{
    TID tid;

    tid = GetCurrentThreadId();
#if defined( _NETWARE_CLIB )
    if( tid == 0 )
        return;
#endif
    if( obj->owner != tid ) {
#if defined( _M_I86 )
        DosSemRequest( &obj->semaphore, -1L );
#else
  #if !defined( __NETWARE__ )
        if( obj->initialized == 0 ) {
    #if defined( __RUNTIME_CHECKS__ ) && defined( _M_IX86 )
            if( obj == &InitSemaphore ) {
                __fatal_runtime_error( "Bad semaphore lock", 1 );
            }
    #endif
            __AccessSemaphore( &InitSemaphore );
            if( obj->initialized == 0 ) {
    #if defined( __NT__ )
                obj->semaphore = __NTGetCriticalSection();
    #elif defined( __QNX__ )
                __posix_sem_init( &obj->semaphore, 1, 1 );
    #elif defined( __LINUX__ )
                __posix_sem_init( &obj->semaphore, 0, 1 );
    #elif defined( __RDOS__ )
                obj->semaphore = RdosCreateSection();
    #elif defined( __RDOSDEV__ )
                RdosInitKernelSection(&obj->semaphore);
    #else
                DosCreateMutexSem( NULL, &obj->semaphore, 0, FALSE );
    #endif
                obj->initialized = 1;
            }
            __ReleaseSemaphore( &InitSemaphore );
        }
  #endif
  #if defined( __NETWARE__ )
        while( obj->semaphore != 0 ) {
    #if defined (_NETWARE_CLIB)
            ThreadSwitch();
    #else
            NXThreadYield();
    #endif
        }

        obj->semaphore = 1;
        obj->initialized = 1;
  #elif defined( __NT__ )
        EnterCriticalSection( obj->semaphore );
  #elif defined( __QNX__ ) || defined( __LINUX__ )
        __posix_sem_wait( &obj->semaphore );
  #elif defined( __RDOS__ )
        RdosEnterSection( obj->semaphore );
  #elif defined( __RDOSDEV__ )
        RdosEnterKernelSection( &obj->semaphore );
  #else
        DosRequestMutexSem( obj->semaphore, SEM_INDEFINITE_WAIT );
  #endif
#endif
        obj->owner = tid;
    }
    obj->count++;
}

_WCRTLINK void __ReleaseSemaphore( semaphore_object *obj )
{
    TID tid;

    tid = GetCurrentThreadId();
#if defined( _NETWARE_CLIB )
    if( tid == 0 )
        return;
#endif
    if( obj->count > 0 ) {
        if( obj->owner != tid ) {
            __fatal_runtime_error( "Semaphore unlocked by wrong owner", 1 );
        }
        if( --obj->count == 0 ) {
            obj->owner = 0;
#if defined( _M_I86 )
            DosSemClear( &obj->semaphore );
#else
  #if defined( __NETWARE__ )
            obj->semaphore = 0;
  #elif defined( __NT__ )
            LeaveCriticalSection( obj->semaphore );
  #elif defined( __QNX__ ) || defined( __LINUX__ )
            __posix_sem_post( &obj->semaphore );
  #elif defined( __RDOS__ )
            RdosLeaveSection( obj->semaphore );
  #elif defined( __RDOSDEV__ )
            RdosLeaveKernelSection( &obj->semaphore );
  #else
            DosReleaseMutexSem( obj->semaphore );
  #endif
#endif
        }
    }
}

#if defined( _M_I86 )

void    __AccessIOB( void )
/*************************/
{
    __AccessSemaphore( &IOBSemaphore );
}

void    __ReleaseIOB( void )
/**************************/
{
    __ReleaseSemaphore( &IOBSemaphore );
}

void __AccessFileH( int handle )
/******************************/
{
    __AccessSemaphore( &FileSemaphores[ (unsigned)handle % MAX_SEMAPHORE ] );
}


void __ReleaseFileH( int handle )
/*******************************/
{
    __ReleaseSemaphore( &FileSemaphores[ (unsigned)handle % MAX_SEMAPHORE ] );
}

void    __AccessNHeap( void )
/***************************/
{
    __AccessSemaphore( &NHeapSemaphore );
}

void    __ReleaseNHeap( void )
/****************************/
{
    __ReleaseSemaphore( &NHeapSemaphore );
}

void    __AccessFHeap( void )
/***************************/
{
    __AccessSemaphore( &FHeapSemaphore );
}

void    __ReleaseFHeap( void )
/****************************/
{
    __ReleaseSemaphore( &FHeapSemaphore );
}

#else

  #if !defined (_THIN_LIB)

static void    __AccessIOB( void )
/*************************/
{
    __AccessSemaphore( &IOBSemaphore );
}

static void    __ReleaseIOB( void )
/**************************/
{
    __ReleaseSemaphore( &IOBSemaphore );
}

static void __AccessFileH( int handle )
/******************************/
{
    __AccessSemaphore( &FileSemaphores[ (unsigned)handle % MAX_SEMAPHORE ] );
}

static void __ReleaseFileH( int handle )
/*******************************/
{
    __ReleaseSemaphore( &FileSemaphores[ (unsigned)handle % MAX_SEMAPHORE ] );
}

  #endif

  #if !defined( __NETWARE__ )

static void    __AccessNHeap( void )
/***************************/
{
    __AccessSemaphore( &NHeapSemaphore );
}

static void    __ReleaseNHeap( void )
/****************************/
{
    __ReleaseSemaphore( &NHeapSemaphore );
}

static void    __AccessFHeap( void )
/***************************/
{
    __AccessSemaphore( &FHeapSemaphore );
}

static void    __ReleaseFHeap( void )
/****************************/
{
    __ReleaseSemaphore( &FHeapSemaphore );
}

  #endif

void    __AccessTDList( void )
/****************************/
{
    __AccessSemaphore( &TDListSemaphore );
}

void    __ReleaseTDList( void )
/*****************************/
{
    __ReleaseSemaphore( &TDListSemaphore );
}

  #if defined( __NT__ ) || defined( __LINUX__ )

static void    __AccessFList( void )
/***************************/
{
    __AccessSemaphore( &FListSemaphore );
}

static void    __ReleaseFList( void )
/****************************/
{
    __ReleaseSemaphore( &FListSemaphore );
}

  #endif

#endif

thread_data *__MultipleThread( void )
{
#if defined( __NT__ )
    /*
     * Preserve old error code -- important because this code can get
     * called from _STK.
     */
    DWORD old = GetLastError();

    thread_data *tdata;
    tdata = (thread_data *)TlsGetValue( __TlsIndex );
    if( tdata == NULL ) {
        tdata = __GetThreadData();
    } else if( tdata->__resize ) {
        tdata = __ReallocThreadData();
    }
    SetLastError(old);
    return( tdata );
#elif defined (_NETWARE_LIBC)
    /*
     * Preserve old error code -- important because this code can get
     * called from _STK.
     */
    int old = GetLastError();
    int ccode = 0;

    thread_data *tdata = NULL;

    if(0 != (ccode = NXKeyGetValue(__NXSlotID, (void **) &tdata)))
        tdata = NULL;

    if( tdata == NULL ) {
        tdata = __GetThreadData();
    } else if( tdata->__resize ) {
        tdata = __ReallocThreadData();
    }
    SetLastError(old);
    return( tdata );
#elif defined( __WARP__ )
    // 32 bit OS/2
    TID tid;
    thread_data *tdata = NULL;
    tid = GetCurrentThreadId();
    if( tid <= __MaxThreads ) {
        tdata = __ThreadData[tid].data;
    }
    if( tdata == NULL ) {
        tdata = __GetThreadData();
    } else if( tdata->__resize ) {
        tdata = __ReallocThreadData();
    }
    return( tdata );
#elif defined( __OS2_286__ )
    // 16 bit OS/2
    return( __ThreadData[GetCurrentThreadId()] );
#elif defined( __QNX__ )
    void *tdata;
    __getmagicvar( &tdata, _m_thread_data );
    if( tdata == NULL ) {
        tdata = __QNXAddThread( tdata );
    }
    return( tdata );
#elif defined( __LINUX__ )
    // TODO: Init multiple threads for Linux!
    return( NULL );
#elif defined( __RDOS__ )
    thread_data *tdata;
    tdata = (thread_data *)__tls_get_value( __TlsIndex );
    if( tdata == NULL )
        tdata = __GetThreadData();
    return( tdata );
#elif defined( __RDOSDEV__ )
    return( NULL );
#else
    return( __ThreadData[GetCurrentThreadId()].data );
#endif
}

#if !defined( _M_I86 )

thread_data *__AllocInitThreadData( thread_data *tdata )
/******************************************************/
{
    if( tdata == NULL ) {
        tdata = lib_calloc( 1, __ThreadDataSize );
        if( tdata != NULL ) {
            tdata->__allocated = 1;
            tdata->__data_size = __ThreadDataSize;
        }
    }
    __InitThreadData( tdata );
    return( tdata );
}

void __FreeInitThreadData( thread_data *tdata )
/******************************************************/
{
    if( tdata != NULL ) {
        if( tdata->__allocated == 1 ) {
            lib_free( tdata );
        }
    }
}

  #if defined( __NT__ )

int __NTThreadInit( void )
/************************/
{
    if( __TlsIndex == NO_INDEX ) {
        __TlsIndex = TlsAlloc();
        // avoid Win32s bug (0-2 are assumed to be reserved by Win32s)
        if( WIN32_IS_WIN32S ) {
            while( (__TlsIndex != NO_INDEX) && (__TlsIndex <= 2) ) {
                __TlsIndex = TlsAlloc();
            }
        }
    }
    if( __TlsIndex == NO_INDEX ) {
        return( FALSE );
    }
    return( TRUE );
}


static void __NTThreadFini( void )
/********************************/
{
    if( __TlsIndex != NO_INDEX ) {
        TlsFree( __TlsIndex );
        __TlsIndex = NO_INDEX;
    }
}


int __NTAddThread( thread_data *tdata )
/*************************************/
{
    if( __TlsIndex == NO_INDEX ) {
        return( FALSE );
    }

    tdata = __AllocInitThreadData( tdata );
    if( tdata == NULL ) {
        return( FALSE );
    }
    if( !__AddThreadData( tdata->thread_id, tdata ) ) {
        __FreeInitThreadData( tdata );
        return( FALSE );
    }
    TlsSetValue( __TlsIndex, tdata );

    return( TRUE );
}


void __NTRemoveThread( int close_handle )
/***************************************/
{
    thread_data *tdata;
    HANDLE      thread_handle;

    if( __TlsIndex != NO_INDEX ) {
        tdata = TlsGetValue( __TlsIndex );
    #if defined( __RUNTIME_CHECKS__ ) && defined( _M_IX86 )
        if( tdata == (thread_data *)2 )
            return;
    #else
        if( tdata == NULL )
            return;
    #endif
        thread_handle = tdata->thread_handle;
        __RemoveThreadData( tdata->thread_id );
    #if defined( __RUNTIME_CHECKS__ ) && defined( _M_IX86 )
        TlsSetValue( __TlsIndex, (void*)2 );
    #else
        TlsSetValue( __TlsIndex, NULL );
    #endif
        if( thread_handle != 0 && close_handle ) {
            CloseHandle( thread_handle );
        }
    }
}

static void __ThreadExit( void )
/******************************/
{
    __NTRemoveThread( TRUE );
    __NTThreadFini();
}

  #elif defined( __OS2__ )

int __OS2AddThread( TID tid, thread_data *tdata )
/***********************************************/
{
    tdata = __AllocInitThreadData( tdata );
    if( tdata == NULL ) return( 0 );
    if( tid <= __MaxThreads ) {
        if( __initthread( tdata ) ) {
            __FreeInitThreadData( tdata );
            return( 0 );
        } else {
            __ThreadData[tid].data = tdata;
            __ThreadData[tid].allocated_entry = tdata->__allocated;
        }
    } else {
        if( !__AddThreadData( tid, tdata ) ) {
            // unable to setup storage
            __FreeInitThreadData( tdata );
            return( 0 );
        }
    }
    return( 1 );
}

void __OS2RemoveThread( void )
/****************************/
{
    TID tid;
    tid = *_threadid;
    if( tid <= __MaxThreads ) {
        if( __ThreadData[tid].allocated_entry ) {
            lib_free( __ThreadData[tid].data );
        }
        __ThreadData[tid].data = NULL;
    } else {
        __RemoveThreadData( tid );
    }
}

  #elif defined( __QNX__ )

thread_data *__QNXAddThread( thread_data *tdata )
/***********************************************/
{
    void *tmp;
    tdata = __AllocInitThreadData( tdata );
    // if tdata is NULL it doesn't matter what we do with it
    tmp = (void *)tdata;
    __setmagicvar( &tmp, _m_thread_data );
    return( tdata );
}

void __QNXRemoveThread( void )
/****************************/
{
    void *tmp;
    thread_data *tdata;

    __getmagicvar( &tmp, _m_thread_data );
    if( tmp != NULL ) {
        tdata = tmp;
        if( tdata->__allocated ) {
            lib_free( tdata );
        }
        tmp = 0;
        __setmagicvar( &tmp, _m_thread_data );
    }
}

  #elif defined( __LINUX__ )

thread_data *__LinuxAddThread( thread_data *tdata )
/***********************************************/
{
    // TODO: Implement this for Linux!
    return( NULL );
}

void __LinuxRemoveThread( void )
/****************************/
{
    // TODO: Implement this for Linux!
}

  #elif defined( __RDOS__ )

int __RdosThreadInit( void )
/**************************/
{
    if( __TlsIndex == NO_INDEX ) {
        __TlsIndex = __tls_alloc();
    }
    if( __TlsIndex == NO_INDEX ) {
        return( 0 );
    }
    __tls_set_value( __TlsIndex, NULL );
    return( 1 );
}

    #if 0

static void __RdosThreadFini( void )
/**********************************/
{
    if( __TlsIndex != NO_INDEX ) {
        __tls_free( __TlsIndex );
        __TlsIndex = NO_INDEX;
    }
}

    #endif

int __RdosAddThread( thread_data *tdata )
/***************************************/
{
    if( __TlsIndex == NO_INDEX ) {
        return( 0 );
    }

    tdata = __AllocInitThreadData( tdata );
    if( tdata == NULL ) {
        return( 0 );
    }
    if( !__AddThreadData( tdata->thread_id, tdata ) ) {
        __FreeInitThreadData( tdata );
        return( 0 );
    }
    __tls_set_value( __TlsIndex, tdata );

    return( 1 );
}


void __RdosRemoveThread( void )
/*****************************/
{
    thread_data *tdata;
    int thread_handle;

    if( __TlsIndex != NO_INDEX ) {
        tdata = __tls_get_value( __TlsIndex );
        if( tdata == NULL ) return;
        __RemoveThreadData( tdata->thread_id );
        __tls_set_value( __TlsIndex, NULL );
    }
}

  #endif

void __InitMultipleThread( void )
/*******************************/
{
    if( __GetThreadPtr != __MultipleThread ) {
  #if defined( _NETWARE_CLIB )
        {
        /* __ThreadData[ 0 ] is used whenever GetThreadID() returns a pointer
           not in our __ThreadIDs list - ie. whenever it returns NULL, a
           pointer to a thread we didn't create, or an invalid pointer */
            void *ptr;
            ptr = lib_calloc( 1, __ThreadDataSize );
            if( ptr == NULL ) {
                __fatal_runtime_error(
                    "Unable to allocate thread-specific data", 1 );
            }
            __ThreadData[ 0 ].data = ptr;
            __ThreadData[ 0 ].allocated_entry = 1;
            __ThreadData[ 0 ].data->__allocated = 1;
            __ThreadData[ 0 ].data->__randnext = 1;
            __ThreadData[ 0 ].data->__data_size = __ThreadDataSize;
            if( __initthread( ptr ) ) {
                lib_free( ptr );
                __fatal_runtime_error(
                    "Unable to initialize thread-specific data", 1 );
            }
            ptr = lib_calloc( 1, __ThreadDataSize );
            if( ptr == NULL ) {
                __fatal_runtime_error(
                    "Unable to allocate thread-specific data", 1 );
            }
            __FirstThreadData = ptr;
            __FirstThreadData->__allocated = 1;
            __FirstThreadData->__randnext = 1;
            __FirstThreadData->__data_size = __ThreadDataSize;
            __ThreadData[ 1 ].data = __FirstThreadData;
            __ThreadData[ 1 ].allocated_entry = __FirstThreadData->__allocated;
            __ThreadIDs[ 1 ] = GetThreadID();
            if( __initthread( ptr ) ) {
                lib_free( ptr );
                __fatal_runtime_error(
                    "Unable to initialize thread-specific data", 1 );
            }
        }
  #elif defined( _NETWARE_LIBC )
        InitSemaphore.semaphore     = 0;    /* sema4 is mutex in this case */
        InitSemaphore.initialized   = 1;
        //_ThreadExitRtn = &__ThreadExit;   - might need this at some point??
        // Note: __AddThreadData uses the InitSemaphore, _AccessTDList & _ReleaseTDList

        __FirstThreadData->thread_id = GetCurrentThreadId();

        __AddThreadData( __FirstThreadData->thread_id, __FirstThreadData );
        if(0 != NXKeySetValue(__NXSlotID, __FirstThreadData)) {
            __fatal_runtime_error(
                "Unable to initialize thread-specific data", 1 );
        }
  #elif defined( __NT__ )
        InitSemaphore.semaphore = __NTGetCriticalSection();
        InitSemaphore.initialized = 1;
        _ThreadExitRtn = &__ThreadExit;
        // Note: __AddThreadData uses the InitSemaphore, _AccessTDList & _ReleaseTDList
        __AddThreadData( __FirstThreadData->thread_id, __FirstThreadData );
        TlsSetValue( __TlsIndex, __FirstThreadData );
  #elif defined( __QNX__ )
        __posix_sem_init( &InitSemaphore.semaphore, 1, 1 );
        InitSemaphore.initialized = 1;
        // first thread data already in magic memory
  #elif defined( __LINUX__ )
        __posix_sem_init( &InitSemaphore.semaphore, 0, 1 );
        InitSemaphore.initialized = 1;
        __AddThreadData( __FirstThreadData->thread_id, __FirstThreadData );
  #elif defined( __RDOS__ )
        InitSemaphore.semaphore = RdosCreateSection();
        InitSemaphore.initialized = 1;
        __AddThreadData( __FirstThreadData->thread_id, __FirstThreadData );
        __tls_set_value( __TlsIndex, __FirstThreadData );
  #elif defined( __RDOSDEV__ )
        RdosInitKernelSection( &InitSemaphore.semaphore );
        InitSemaphore.initialized = 1;
  #elif defined( __OS2__ )
        DosCreateMutexSem( NULL, &InitSemaphore.semaphore, 0, FALSE );
        InitSemaphore.initialized = 1;
        __ThreadData[1].data = __FirstThreadData;
        __ThreadData[1].allocated_entry = __FirstThreadData->__allocated;
  #else
    #error Multiple thread support is not defined for this platform
  #endif

  #if !defined( _M_I86 )
    // Set these up after we have created the InitSemaphore
    #if !defined (_THIN_LIB)
        _AccessFileH      = &__AccessFileH;
        _ReleaseFileH     = &__ReleaseFileH;
        _AccessIOB        = &__AccessIOB;
        _ReleaseIOB       = &__ReleaseIOB;
    #endif
        _AccessTDList     = &__AccessTDList;
        _ReleaseTDList    = &__ReleaseTDList;
        __AccessSema4     = &__AccessSemaphore;
        __ReleaseSema4    = &__ReleaseSemaphore;
        __CloseSema4      = &__CloseSemaphore;
    #if !defined( __NETWARE__ )
        _AccessNHeap  = &__AccessNHeap;
        _AccessFHeap  = &__AccessFHeap;
        _ReleaseNHeap = &__ReleaseNHeap;
        _ReleaseFHeap = &__ReleaseFHeap;
    #endif
    #if defined( __NT__ )
        _AccessFList  = &__AccessFList;
        _ReleaseFList = &__ReleaseFList;
    #endif
  #endif
        __GetThreadPtr  = __MultipleThread;
    }
}
#endif

static void __FiniSema4s( void )              // called from finalizer
/******************************/
{
    int         i;

    _CloseSemaphore( &IOBSemaphore );
    for( i = 0; i < MAX_SEMAPHORE; i++ ) {
        _CloseSemaphore( &FileSemaphores[ i ] );
    }
#if defined( __NT__ )
    _CloseSemaphore( &FListSemaphore );
    __NTFreeCriticalSection();
#endif
#if !defined( __QNX__ )
    __FiniThreadProcessing();
  #if !defined( __OS2_286__ )
    // All thread data areas freed, including main process thread data
    // so mark first thread data pointer null. Note that OS/2 1.x does
    // not have __FirstThreadData at all.
    __FirstThreadData = NULL;
  #endif
#endif
#if !defined( __NETWARE__ )
    _heapshrink();
    _CloseSemaphore( &NHeapSemaphore );
    _CloseSemaphore( &FHeapSemaphore );
#endif
#if !defined( _M_I86 )
    _CloseSemaphore( &TDListSemaphore );
    _CloseSemaphore( &InitSemaphore );

    // After closing InitSemaphore, we need to reset the sem access routines to
    // the dummy ones; someone may still want semaphore protection during shutdown
    // processing but since threading is gone now, there should be no reentrancy
    // problems
    __AccessSema4  = &nullSema4Rtn;
    __ReleaseSema4 = &nullSema4Rtn;
    __CloseSema4   = &nullSema4Rtn;
  #if !defined( __NETWARE__ )
    _AccessNHeap  = &__NullAccHeapRtn;
    _AccessFHeap  = &__NullAccHeapRtn;
    _ReleaseNHeap = &__NullAccHeapRtn;
    _ReleaseFHeap = &__NullAccHeapRtn;
  #endif

  #if defined( __NT__ )
    __NTDeleteCriticalSection();
    __NTThreadFini();
  #endif
  #if defined (_NETWARE_LIBC)
    __LibCThreadFini();
  #endif
#endif
}

AYI( __FiniSema4s, INIT_PRIORITY_RUNTIME )
