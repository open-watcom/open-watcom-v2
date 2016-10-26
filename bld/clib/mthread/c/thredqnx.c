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
* Description:  QNX threading routines.
*
****************************************************************************/


#include "variety.h"
#include <semaphor.h>
#include <string.h>
#include <signal.h>
#include <float.h>
#include <unistd.h>
#include <process.h>
#include "rtstack.h"
#include "stacklow.h"
#include "liballoc.h"
#include "thread.h"
#include "mthread.h"
#include "semapsx.h"
#include "cthread.h"
#include "rterrno.h"

extern  void            __qnx_exit( int __status );

typedef struct thread_args {
    thread_fn   *rtn;
    void        *argument;
    void        *stack_bottom;
    sem_t       event;
} thread_args;


static int begin_thread_helper( void *ptr )
/*****************************************/
{
    thread_args                 *td;
    thread_fn                   *rtn;
    void                        *arg;
    thread_data                 *tdata;

    td = ptr;
    rtn = td->rtn;
    arg = td->argument;

    tdata = __alloca( __ThreadDataSize );
    memset( tdata, 0, __ThreadDataSize );
    // tdata->__allocated = 0;
    tdata->__data_size = __ThreadDataSize;
    __QNXAddThread( tdata );
    _STACKLOW = (unsigned)td->stack_bottom;

    __posix_sem_post( &td->event );
    _fpreset();
    (*rtn)( arg );
    _endthread();
    return( 0 );
}

int __CBeginThread( thread_fn *start_addr, void *stack_bottom,
                    unsigned stack_size, void *arglist )
/******************************************************/
{
    pid_t       pid;
    thread_args td;
    int         rc;

    if( stack_bottom == NULL ) {
        if( stack_size == 0 ) {
            stack_size = 1024*4;
        }
        stack_bottom = lib_calloc( stack_size, 1 );
        if( stack_bottom == NULL ) {
            _RWD_errno = ENOMEM;
            return( -1 );
        }
    }
    __InitMultipleThread();
    td.rtn = start_addr;
    td.argument = arglist;
    td.stack_bottom = stack_bottom;
    rc = __posix_sem_init( &td.event, 1, 0 );
    if( rc == -1 ) return( -1 );
    pid = tfork( stack_bottom, stack_size, begin_thread_helper, &td, 0 );
    if( pid != -1 ) {
        /*
           suspend parent thread so that it can't call _beginthread() again
           before new thread extracts data from "td" (no problem if new
           thread calls _beginthread() since it has its own stack)
        */
        __posix_sem_wait( &td.event );
    }
    __posix_sem_destroy( &td.event );
    return( pid );
}

void __CEndThread( void )
/***********************/
{
    __QNXRemoveThread();
    __qnx_exit( 0 );
}
