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
* Description:  Win32 threading routines.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <dos.h>
#include <malloc.h>
#include <windows.h>
#include "rterrno.h"
#include "stacklow.h"
#include "liballoc.h"
#include "ntext.h"
#include "thread.h"
#include "initsig.h"
#include "trdlist.h"
#include "mthread.h"
#include "initarg.h"
#include "cthread.h"


extern  DWORD           __TlsIndex;

typedef struct thread_args {
    thread_fn   *rtn;
    void        *argument;
    HANDLE      thread_handle;
} thread_args;

static DWORD WINAPI begin_thread_helper( thread_args *td )
/********************************************************/
{
    thread_fn           *rtn;
    void                *arg;
    REGISTRATION_RECORD rr;
    thread_data         *tdata;
    HANDLE              thread_handle;

    rtn = td->rtn;
    arg = td->argument;
    thread_handle = td->thread_handle;
    free( td );

    // For DLLs, __NTAddThread has already been called from _LibMain
    // in DLL_THREAD_ATTACH processing.
    if( !__Is_DLL ) {
        // allocate thread_data structure on stack
        tdata = __alloca( __ThreadDataSize );
        if( tdata != NULL ) {
            memset( tdata, 0, __ThreadDataSize );
            // tdata->__allocated = 0;
            tdata->__data_size = __ThreadDataSize;
            // do further thread_data initialization and registration
            if( !__NTAddThread( tdata ) ) {
                // print runtime error message now ?
                CloseHandle( thread_handle );
                return( 0 );
            }
        }
    }

    // now get the thread_data ptr the 'standard' way -- this may cause
    // a new thread_data structure to be allocated on heap:
    // by __GetThreadPtr() ==> __MultipleThread(){ TlsGetValue(); if NULL ==>
    //    __GetThreadData() ==> __NTAddThread() ==> __AllocInitThreadData() }
    tdata = __THREADDATAPTR;
    if( tdata == NULL ) {
        // this is a library runtime error, should we print an error message ?
        CloseHandle( thread_handle );
        return( 0 );
    }
    tdata->thread_handle = thread_handle;

    __NewExceptionFilter( &rr );
    __sig_init_rtn(); // fills in a thread-specific copy of signal table
    (*rtn)( arg );
    _endthread();
     return( 0 );
}

int __CBeginThread( thread_fn *start_addr, void *stack_bottom,
                    unsigned stack_size, void *arglist )
/************************************************************/
{
    DWORD       tid;
    thread_args *td;
    HANDLE      th;

    stack_bottom = stack_bottom;    /* parameter not used for NT version */

    if( __TlsIndex == NO_INDEX ) {
        if( !__NTThreadInit() )
            return( -1L );
        __InitMultipleThread();
    }

    td = malloc( sizeof( *td ) );
    if( td == NULL ) {
        _RWD_errno = ENOMEM;
        return( -1L );
    }

    stack_size = __ROUND_UP_SIZE_4K( stack_size );

    td->rtn = start_addr;
    td->argument = arglist;

    th = CreateThread( NULL, stack_size, (LPTHREAD_START_ROUTINE)&begin_thread_helper,
                (LPVOID) td, CREATE_SUSPENDED, &tid );
    if( th ) {
        td->thread_handle = th;
        ResumeThread( th );
    } else {
        // we didn't create the thread so it isn't going to free this
        free( td );
        th = (HANDLE)-1L;
    }
    return( (int)th );
}

void __CEndThread( void )
/***********************/
{
    __sig_fini_rtn();
    __DoneExceptionFilter();
    // For DLLs, __NTRemoveThread will be called from _LibMain
    // in DLL_THREAD_DETACH processing.
    if( ! __Is_DLL ) {
        __NTRemoveThread( TRUE );
    }
    ExitThread( 0 );
}
