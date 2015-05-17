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
* Description:  OS/2 32-bit threading routines.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <process.h>
#include <stddef.h>
#include <string.h>
#include <dos.h>
#include <float.h>
#define INCL_DOSSEMAPHORES
#define INCL_DOSPROCESS
#define INCL_DOSEXCEPTIONS
#include "rtdata.h"
#include "liballoc.h"
#include "osthread.h"
#include "thread.h"
//#include "stacklow.h"
#include "sigtab.h"
#include "trdlist.h"
#include "mthread.h"
#include "initarg.h"
#include "cthread.h"

#pragma aux __threadstksize "*"
extern  unsigned        __threadstksize;

extern  unsigned        __threadstack( void );

typedef struct thread_args {
    thread_fn   *rtn;
    void        *argument;
    HEV         event;
} thread_args;


static void APIENTRY begin_thread_helper( thread_args *td )
/*********************************************************/
{
    thread_fn                   *rtn;
    void                        *arg;
    EXCEPTIONREGISTRATIONRECORD xcpt;
    thread_data                 *tdata;

    rtn = td->rtn;
    arg = td->argument;

    tdata = __alloca( __ThreadDataSize );
    memset( tdata, 0, __ThreadDataSize );
    // tdata->__allocated = 0;
    tdata->__data_size = __ThreadDataSize;
    if( !__Is_DLL ) {
        if( !__OS2AddThread( *_threadid, tdata ) ) return;
    }

    DosPostEventSem( td->event );
    _fpreset();
    __XCPTHANDLER = &xcpt;
    __sig_init_rtn();
    (*rtn)( arg );
    _endthread();
}

int __CBeginThread( thread_fn *start_addr, void *stack_bottom,
                    unsigned stack_size, void *arglist )
/******************************************************/
{
    TID         tid;
    APIRET      rc;
    thread_args td;

    if( __ThreadData == NULL ) {
        if( __InitThreadProcessing() == NULL )  return( -1 );
        __InitMultipleThread();
    }
    stack_bottom = stack_bottom;
    td.rtn = start_addr;
    td.argument = arglist;
    rc = DosCreateEventSem( NULL, &td.event, 0, 0 );
    if( rc != 0 ) return( -1 );
    rc = DosCreateThread( &tid, (PFNTHREAD)begin_thread_helper, (ULONG)&td,
                          0, stack_size + __threadstksize );
    if( rc != 0 ) {
        tid = -1;
    } else {
        /*
           suspend parent thread so that it can't call _beginthread() again
           before new thread extracts data from "td" (no problem if new
           thread calls _beginthread() since it has its own stack)
        */
        DosWaitEventSem( td.event, SEM_INDEFINITE_WAIT );
    }
    DosCloseEventSem( td.event );
    return( tid );
}

void __CEndThread( void )
/***********************/
{
    __sig_fini_rtn();
    __OS2RemoveThread();
    DosExit( EXIT_THREAD, 0 );
}
