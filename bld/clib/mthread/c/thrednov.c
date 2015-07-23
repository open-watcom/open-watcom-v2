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
* Description:  NetWare specific threading support routines.
*
****************************************************************************/


#include "variety.h"
#include <process.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <i86.h>
#include "rtstack.h"
#include "liballoc.h"
#include "exitwmsg.h"
#include "thread.h"
#include "cthread.h"
#include "snglthrd.h"
#include "maxthrds.h"

#define EXIT_THREAD     0

extern void             *GetThreadID( void );
extern long             OpenLocalSemaphore( long );
extern int              CloseLocalSemaphore( long );
extern long             ExamineLocalSemaphore( long );
extern int              SignalLocalSemaphore( long );
extern int              WaitOnLocalSemaphore( long );
extern void             ExitThread( int , int );
extern int              BeginThread( void (*)( void * ), void *, unsigned,
                                     void * );

extern  void            **__ThreadIDs;

static  int             CurrThrdID = 1;

static int gettid( void *netid )
{
    int         j;

    for( j = 1; j <= __MaxThreads; ++j ) {
        if( __ThreadIDs[ j ] == netid ) {
            return( j );
        }
    }
    return( 0 );   /* __ThreadIDs[ 0 ] points to a thread data struct used
                      whenever we can't find a match */
}

_WCRTLINK int *__threadid( void )
{
    void *netid;
    int   id;

    netid = GetThreadID();
    id = gettid( netid );
    if( netid != NULL && id == 0 ) { // handle stray threads
        id = gettid( NULL );
        if( id != 0 ) {
            void *ptr;
            __ThreadIDs[ id ] = netid;
            ptr = lib_calloc( 1, __ThreadDataSize );
            if( ptr == NULL ) {
                __fatal_runtime_error(
                    "Unable to allocate thread-specific data", 1 );
            }
            __ThreadData[ id ].data = ptr;
            __ThreadData[ id ].allocated_entry = 1;
            __ThreadData[ id ].data->__allocated = 1;
            __ThreadData[ id ].data->__randnext = 1;
            __ThreadData[ id ].data->__data_size = __ThreadDataSize;
            if( __initthread( ptr ) ) {
                lib_free( ptr );
                __fatal_runtime_error(
                    "Unable to initialize thread-specific data", 1 );
            }
        }
    }
    CurrThrdID = id;
    return( &CurrThrdID );
}

typedef struct {
    thread_fn           *start_addr;
    void                *arglist;
    void                *stack_bottom;
    int                 tid;
    unsigned            semaphore;
} begin_thread_data;

static void begin_thread_helper( void *the_arg )
{
    thread_fn           *start_addr;
    void                *arglist;
    void                *stack_bottom;

    thread_data         *tdata;
    int                 newtid;
    begin_thread_data   *data = the_arg;

    tdata = alloca( __ThreadDataSize );
    newtid = gettid( NULL );
    if( newtid != 0 ) {
        data->tid = newtid;
        start_addr = data->start_addr;
        arglist = data->arglist;
        stack_bottom = data->stack_bottom;
        SignalLocalSemaphore( data->semaphore );
        __ThreadIDs[ newtid ] = GetThreadID();
        __ThreadData[ newtid ].data = tdata;
        __ThreadData[ newtid ].allocated_entry = 0;
        memset( tdata, 0, __ThreadDataSize );
        // tdata->__allocated = 0;
        tdata->__data_size = __ThreadDataSize;
        tdata->__randnext = 1;
        _RWD_stacklow = FP_OFF( stack_bottom );
        (*start_addr)( arglist );
        _endthread();
    } else {
        data->tid = -1;
        SignalLocalSemaphore( data->semaphore );
    }
}

extern int __CBeginThread( thread_fn *start_addr, void *stack_bottom,
                         unsigned stack_size, void *arglist )
{
    begin_thread_data   data;

    data.start_addr = start_addr;
    data.stack_bottom = stack_bottom;
    data.arglist = arglist;
    data.semaphore = OpenLocalSemaphore( 0 );
    if( BeginThread( &begin_thread_helper,
                     stack_bottom, stack_size, &data ) != -1 ) {
        WaitOnLocalSemaphore( data.semaphore );
    } else {
        data.tid = -1;
    }
    CloseLocalSemaphore( data.semaphore );
    return( data.tid );
}

extern void __CEndThread( void )
{
    int         thrdid;

    thrdid = *_threadid;
    // don't need to check for allocated indication since always on stack
    __ThreadData[ thrdid ].data = NULL;
    __ThreadIDs[ thrdid ] = NULL;
    ExitThread( EXIT_THREAD, 0 );
}
