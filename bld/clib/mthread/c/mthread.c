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


#include "variety.h"
#include "stacklow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liballoc.h"

#include "thread.h"
#include "trdlist.h"
#include "mthread.h"
#include "rtdata.h"
#include "rtinit.h"
#include "exitwmsg.h"
#include "osver.h"

#if defined( __QNX__ )
  #include "semaqnx.h"
  #include <sys/magic.h>
  extern thread_data *__QNXAddThread( thread_data *tdata );
#endif

extern  void            __FiniThreadProcessing(void);
#if defined( __386__ ) || defined( __AXP__ ) || defined( __PPC__ )
  extern void           (*_AccessFileH)(int);
  extern void           (*_ReleaseFileH)(int);
  extern void           (*_AccessIOB)(void);
  extern void           (*_ReleaseIOB)(void);
  extern void           (*_AccessTDList)(void);
  extern void           (*_ReleaseTDList)(void);
  #if !defined( __NETWARE__ )
    extern void         (*_AccessNHeap)(void);
    extern void         (*_AccessFHeap)(void);
    extern void         (*_ReleaseNHeap)(void);
    extern void         (*_ReleaseFHeap)(void);
  #endif
  #if defined( __NT__ )
    extern void         (*_AccessFList)(int);
    extern void         (*_ReleaseFList)(int);
    extern void         (*_ThreadExitRtn)(void);
    static semaphore_object FListSemaphore;
  #endif
#endif

#if defined( __386__ ) || defined( __AXP__ ) || defined( __PPC__ )
    void static nullSema4Rtn( semaphore_object *p ) { p = p; }
    _WCRTLINK void (*__AccessSema4)( semaphore_object *) = &nullSema4Rtn;
    _WCRTLINK void (*__ReleaseSema4)( semaphore_object *) = &nullSema4Rtn;
    _WCRTLINK void (*__CloseSema4)( semaphore_object *) = &nullSema4Rtn;
#endif

extern  int             __Sema4Fini;            // in finalizer segment
#ifdef _M_IX86
 #pragma aux            __Sema4Fini "_*";
#endif
extern  unsigned        __MaxThreads;
extern  thread_data     *__FirstThreadData;
extern  void            **__ThreadIDs;

#define MAX_SEMAPHORE   16

static semaphore_object FileSemaphores[ MAX_SEMAPHORE ];

#if !defined( __NETWARE__ )
  static semaphore_object FHeapSemaphore;
  static semaphore_object NHeapSemaphore;
#endif

static semaphore_object IOBSemaphore;

#if defined( __386__ ) || defined( __AXP__ ) || defined( __PPC__ )
  static semaphore_object InitSemaphore;
  static semaphore_object TDListSemaphore;
#endif

#if defined( __NT__ )
#define MAX_CRITICAL_SECTION 64
static CRITICAL_SECTION critsect_cache[MAX_CRITICAL_SECTION];
static int critsect_next;
static CRITICAL_SECTION **critsect_vector;
static int critsect_vectornext;

CRITICAL_SECTION *__NTGetCriticalSection( void ) {
    CRITICAL_SECTION *ptr;

    if( critsect_next < MAX_CRITICAL_SECTION ) {
        ptr = &(critsect_cache[critsect_next]);
        critsect_next++;
    } else {
        ptr = lib_calloc( 1, sizeof( *ptr ) );
        if( ptr == NULL ) {
            __fatal_runtime_error(
                "Unable to allocate semaphore data\r\n", 1 );
        }
        critsect_vector = lib_realloc( critsect_vector,
                (critsect_vectornext+1)*sizeof(CRITICAL_SECTION*));
        if( critsect_vector == NULL ) {
            __fatal_runtime_error(
                "Unable to allocate semaphore data\r\n", 1 );
        }
        critsect_vector[critsect_vectornext] = ptr;
        critsect_vectornext++;
    }
    InitializeCriticalSection( ptr );
    return( ptr );
}
__NTDeleteCriticalSection( void ) {
    int i;
    for( i = 0 ; i < critsect_next ; i++ ) {
        DeleteCriticalSection( &(critsect_cache[i]) );
    }
}
__NTFreeCriticalSection( void ) {
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
//      if( obj->count >= 2 ) {
//          __fatal_runtime_error( "Semaphore locked too many times\r\n", 1 );
//      }
        if( obj->count >= 1 ) {
            __fatal_runtime_error( "Semaphore not unlocked\r\n", 1 );
        }
    #endif
    #if !defined( __NT__ )
        #if defined( __386__ ) || defined( __AXP__ ) || defined( __PPC__ )
            if( obj->initialized != 0 ) {
                #if defined( __NETWARE__ )
                    obj->semaphore = 0;
                #elif defined( __QNX__ )
                    __qsem_destroy( &obj->semaphore );
                #else
                    DosCloseMutexSem( obj->semaphore );
                #endif
            }
        #else
            if( obj->count > 0 ) {
                DosSemClear( &obj->semaphore );
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
    #if defined( __NETWARE__ )
        if( tid == 0 ) return;
    #endif
    if( obj->owner != tid ) {
        #if defined( __386__ ) || defined( __AXP__ ) || defined( __PPC__ )
            #if !defined( __NETWARE__ )
                if( obj->initialized == 0 ) {
                    #if defined( __RUNTIME_CHECKS__ ) && defined( _M_IX86 )
                        if( obj == &InitSemaphore ) {
                            __fatal_runtime_error( "Bad semaphore lock\r\n", 1 );
                        }
                    #endif
                    __AccessSemaphore( &InitSemaphore );
                    if( obj->initialized == 0 ) {
                        #if defined( __NT__ )
                            obj->semaphore = __NTGetCriticalSection();
                        #elif defined( __QNX__ )
                            __qsem_init( &obj->semaphore, 1, 1 );
                        #else
                            DosCreateMutexSem( NULL, &obj->semaphore, 0, FALSE );
                        #endif
                        obj->initialized = 1;
                    }
                    __ReleaseSemaphore( &InitSemaphore );

                }
            #endif
            #if defined( __NETWARE__ )
                while( obj->semaphore != 0 ) ThreadSwitch();
                obj->semaphore = 1;
                obj->initialized = 1;
            #elif defined( __NT__ )
                EnterCriticalSection( obj->semaphore );
            #elif defined( __QNX__ )
                __qsem_wait( &obj->semaphore );
            #else
                DosRequestMutexSem( obj->semaphore, SEM_INDEFINITE_WAIT );
            #endif
        #else
            DosSemRequest( &obj->semaphore, -1L );
        #endif
        obj->owner = tid;
    }
    obj->count++;
}

_WCRTLINK void __ReleaseSemaphore( semaphore_object *obj )
{
    TID tid;

    tid = GetCurrentThreadId();
    #if defined( __NETWARE__ )
        if( tid == 0 ) return;
    #endif
    if( obj->count > 0 ) {
        if( obj->owner != tid ) {
            __fatal_runtime_error( "Semaphore unlocked by wrong owner\r\n", 1 );
        }
        if( --obj->count == 0 ) {
            obj->owner = 0;
            #if defined( __386__ ) || defined( __AXP__ ) || defined( __PPC__ )
                #if defined( __NETWARE__ )
                    obj->semaphore = 0;
                #elif defined( __NT__ )
                    LeaveCriticalSection( obj->semaphore );
                #elif defined( __QNX__ )
                    __qsem_post( &obj->semaphore );
                #else
                    DosReleaseMutexSem( obj->semaphore );
                #endif
            #else
                DosSemClear( &obj->semaphore );
            #endif
        }
    }
}

void    __AccessIOB()
/*******************/
{
    __AccessSemaphore( &IOBSemaphore );
}

void    __ReleaseIOB()
/********************/
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


#if !defined( __NETWARE__ )
void    __AccessNHeap()
/*********************/
{
    __AccessSemaphore( &NHeapSemaphore );
}

void    __ReleaseNHeap()
/**********************/
{
    __ReleaseSemaphore( &NHeapSemaphore );
}

void    __AccessFHeap()
/*********************/
{
    __AccessSemaphore( &FHeapSemaphore );
}

void    __ReleaseFHeap()
/**********************/
{
    __ReleaseSemaphore( &FHeapSemaphore );
}
#endif

#if defined( __386__ ) || defined( __AXP__ ) || defined( __PPC__ )

void    __AccessTDList()
/*********************/
{
    __AccessSemaphore( &TDListSemaphore );
}

void    __ReleaseTDList()
/**********************/
{
    __ReleaseSemaphore( &TDListSemaphore );
}

#if defined( __NT__ )
void    __AccessFList()
/*********************/
{
    __AccessSemaphore( &FListSemaphore );
}

void    __ReleaseFList()
/**********************/
{
    __ReleaseSemaphore( &FListSemaphore );
}
#endif
#endif

_WCRTLINK void *__MultipleThread()
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
    #else
        return( __ThreadData[GetCurrentThreadId()].data );
    #endif
}

#if defined( __386__ ) || defined( __AXP__ ) || defined( __PPC__ )

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
        memset( tdata, 0xdeaddead, __ThreadDataSize );
        if( tdata->__allocated == 1 ) lib_free( tdata );
    }
}

#if defined( __NT__ )

BOOL __NTThreadInit( void )
/*************************/
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


void __NTThreadFini( void )
/*************************/
{
    if( __TlsIndex != NO_INDEX ) {
        TlsFree( __TlsIndex );
        __TlsIndex = NO_INDEX;
    }
}


BOOL __NTAddThread( thread_data *tdata )
/**************************************/
{
    if( __TlsIndex == NO_INDEX ) {
        return( FALSE );
    }

    tdata = __AllocInitThreadData( tdata );
    if( tdata == NULL ) {
        return( FALSE );
    }
    if( !__AddThreadData( tdata->thread_id, tdata ) ) {
        lib_free( tdata );
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
            if( tdata == (thread_data *)2 ) return;
        #else
            if( tdata == NULL ) return;
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


static void __ThreadExit()
/************************/
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
            lib_free( tdata );
            return( 0 );
        } else {
            __ThreadData[tid].data = tdata;
            __ThreadData[tid].allocated_entry = tdata->__allocated;
        }
    } else {
        if( !__AddThreadData( tid, tdata ) ) {
            // unable to setup storage
            lib_free( tdata );
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

#endif

void __InitMultipleThread()
/*************************/
{
    if( __GetThreadPtr != &__MultipleThread ) {
        _AccessFileH   = &__AccessFileH;
        _ReleaseFileH  = &__ReleaseFileH;
        _AccessIOB     = &__AccessIOB;
        _ReleaseIOB    = &__ReleaseIOB;
        _AccessTDList  = &__AccessTDList;
        _ReleaseTDList = &__ReleaseTDList;
        __AccessSema4  = &__AccessSemaphore;
        __ReleaseSema4 = &__ReleaseSemaphore;
        __CloseSema4   = &__CloseSemaphore;
        #if !defined( __NETWARE__ )
            _AccessNHeap  = &__AccessNHeap;
            _AccessFHeap  = &__AccessFHeap;
            _ReleaseNHeap = &__ReleaseNHeap;
            _ReleaseFHeap = &__ReleaseFHeap;
        #endif
        #if defined( __NETWARE__ )
        {
        /* __ThreadData[ 0 ] is used whenever GetThreadID() returns a pointer
           not in our __ThreadIDs list - ie. whenever it returns NULL, a
           pointer to a thread we didn't create, or an invalid pointer */
            void *ptr;
            ptr = lib_calloc( 1, __ThreadDataSize );
            if( ptr == NULL ) {
                __fatal_runtime_error(
                    "Unable to allocate thread-specific data\r\n", 1 );
            }
            __ThreadData[ 0 ].data = ptr;
            __ThreadData[ 0 ].allocated_entry = 1;
            __ThreadData[ 0 ].data->__allocated = 1;
            __ThreadData[ 0 ].data->__randnext = 1;
            __ThreadData[ 0 ].data->__data_size = __ThreadDataSize;
            if( __initthread( ptr ) ) {
                lib_free( ptr );
                __fatal_runtime_error(
                    "Unable to initialize thread-specific data\r\n", 1 );
            }
            ptr = lib_calloc( 1, __ThreadDataSize );
            if( ptr == NULL ) {
                __fatal_runtime_error(
                    "Unable to allocate thread-specific data\r\n", 1 );
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
                    "Unable to initialize thread-specific data\r\n", 1 );
            }
        }
        #elif defined( __NT__ )
            InitSemaphore.semaphore = __NTGetCriticalSection();
            InitSemaphore.initialized = 1;
            _AccessFList  = &__AccessFList;
            _ReleaseFList = &__ReleaseFList;
            _ThreadExitRtn= &__ThreadExit;
            // note that __AddThreadData uses the InitSemaphore
            __AddThreadData( __FirstThreadData->thread_id, __FirstThreadData );
            TlsSetValue( __TlsIndex, __FirstThreadData );
        #elif defined( __QNX__ )
            __qsem_init( &InitSemaphore.semaphore, 1, 1 );
            InitSemaphore.initialized = 1;
            // first thread data already in magic memory
        #else
            DosCreateMutexSem( NULL, &InitSemaphore.semaphore, 0, FALSE );
            InitSemaphore.initialized = 1;
            __ThreadData[1].data = __FirstThreadData;
            __ThreadData[1].allocated_entry = __FirstThreadData->__allocated;
        #endif
        __GetThreadPtr  = &__MultipleThread;
    }
}
#endif

void static __FiniSema4s()              // called from finalizer
/************************/
{
    int         i;

    _CloseSemaphore( &IOBSemaphore );
    for( i = 0; i < MAX_SEMAPHORE; i++ ) {              /* 17-feb-93 */
        _CloseSemaphore( &FileSemaphores[ i ] );
    }
    #if defined( __NT__ )
        _CloseSemaphore( &FListSemaphore );
        __NTFreeCriticalSection();
    #endif
    #if !defined( __QNX__ )
        __FiniThreadProcessing();
    #endif
    #if !defined( __NETWARE__ )
        _heapshrink();
        _CloseSemaphore( &NHeapSemaphore );
        _CloseSemaphore( &FHeapSemaphore );
    #endif
    #if defined( __386__ ) || defined( __AXP__ ) || defined( __PPC__ )
        _CloseSemaphore( &TDListSemaphore );
        _CloseSemaphore( &InitSemaphore );
        #if defined( __NT__ )
            __NTDeleteCriticalSection();
            __NTThreadFini();
        #endif
    #endif
}

AYI( __FiniSema4s, INIT_PRIORITY_RUNTIME )
