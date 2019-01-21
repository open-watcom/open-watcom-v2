/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Thread creation in RDOS device-driver.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <rdos.h>
#include <rdosdev.h>
#include "rtdata.h"
#include "liballoc.h"
#include "thread.h"
#include "trdlist.h"
#include "mthread.h"
#include "initarg.h"
#include "cthread.h"

typedef struct thread_args {
    thread_fn   *start_addr;
    void        *argument;
    int         signal;
    int         tid;
} thread_args;

#pragma aux begin_thread_helper "*" \
    __parm __caller             [__gs __ebx] \
    __value __struct __routine  [__eax] \
    __modify                    [__eax __ebx __ecx __edx __esi __edi]

static void __far begin_thread_helper( void *param )
/********************************************************/
{
    thread_args         *td = (thread_args *)param;
    __thread_fn         *start_addr;
    void                *arg;

    td->tid = RdosGetThreadHandle();
    start_addr = (__thread_fn *)td->start_addr;
    arg = td->argument;
    RdosSignal( td->signal );

    (*start_addr)( arg );
    _endthread();
     return;
}

int __CBeginThread( thread_fn *start_addr, int prio, const char *thread_name,
                         unsigned stack_size, void *arglist )
/************************************************************/
{
    thread_args *td;
    int         th;

    __InitMultipleThread();

    td = malloc( sizeof( *td ) );
    if( td == NULL ) {
        return( -1L );
    }

    td->start_addr = start_addr;
    td->argument = arglist;
    td->signal = RdosGetThreadHandle();
    RdosClearSignal();

    RdosCreateKernelThread( prio, stack_size, begin_thread_helper, thread_name, td );

    RdosWaitForSignal();
    th = td->tid;
    free( td );

    return( th );
}

void __CEndThread( void )
/***********************/
{
}
