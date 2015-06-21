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
* Description:  Thread creation under RDOS.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <dos.h>
#include <malloc.h>
#include <rdos.h>
#include "rterrno.h"
#include "stacklow.h"
#include "liballoc.h"
#include "thread.h"
#include "initsig.h"
#include "trdlist.h"
#include "mthread.h"
#include "initarg.h"
#include "cthread.h"


extern  int             __TlsIndex;

typedef struct thread_args {
    thread_fn   *rtn;
    void        *argument;
    int         thread_handle;
    int         signal;
    int         tid;
} thread_args;

#pragma aux begin_thread_helper "*" parm [esi] modify [eax ebx ecx edx esi edi];

static void begin_thread_helper( void *param )
/********************************************************/
{
    thread_args         *td = (thread_args *)param;
    thread_fn           *rtn;
    void                *arg;
    thread_data         *tdata;
    int                 thread_handle;
    REGISTRATION_RECORD rr;

    td->tid = RdosGetThreadHandle();    
    rtn = td->rtn;
    arg = td->argument;
    thread_handle = td->thread_handle;
    RdosSetSignal( td->signal );

    tdata = ( thread_data * )RdosAllocateMem( __ThreadDataSize );

    if ( tdata ) {
        memset( tdata, 0, __ThreadDataSize );
        tdata->__data_size = __ThreadDataSize;

        if( !__RdosAddThread( tdata ) ) {
                // print runtime error message now ?
            return;
        }
    }

    // now get the thread_data ptr the 'standard' way -- this may cause
    // a new thread_data structure to be allocated on heap:
    tdata = __THREADDATAPTR;
    if ( !tdata ) {
        // this is a library runtime error, should we print an error message ?
        return;
    }

    __NewExceptionFilter( &rr );
    __sig_init_rtn(); // fills in a thread-specific copy of signal table
    (*rtn)( arg );
    _endthread();
    RdosFreeMem(tdata);
     return;
}

int __CBeginThread( thread_fn *start_addr, int prio, const char *thread_name,
                         unsigned stack_size, void *arglist )
/************************************************************/
{
    thread_args *td;
    int         th;
    int         wait_handle;

    if( __TlsIndex == NO_INDEX ) {
        if( !__RdosThreadInit() ) return( -1L );
        __InitMultipleThread();
    }

    td = malloc( sizeof( *td ) );
    if( td == NULL ) {
        _RWD_errno = ENOMEM;
        return( -1L );
    }

    stack_size = __Align4K( stack_size );

    wait_handle = RdosCreateWait();

    td->rtn = start_addr;
    td->argument = arglist;
    td->signal = RdosCreateSignal();
    RdosResetSignal( td->signal );
    RdosAddWaitForSignal( wait_handle, td->signal, 0 );

    __create_thread(begin_thread_helper, prio, thread_name, td, stack_size);

    RdosWaitForever( wait_handle );
    RdosFreeSignal( td->signal );
    RdosCloseWait( wait_handle );
    th = td->tid;
    free( td );

    return( th );
}

void __CEndThread( void )
/***********************/
{
    __sig_fini_rtn();
    __DoneExceptionFilter();
    __RdosRemoveThread();
}
