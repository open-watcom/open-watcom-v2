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
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <malloc.h>
#include "liballoc.h"
#include "ntex.h"
#include "stacklow.h"
#include "sigtab.h"
#include "thread.h"
#include "trdlist.h"
#include "mthread.h"
#include "rtdata.h"
#include "extfunc.h"

extern  void            __InitMultipleThread( void );

extern  DWORD           __TlsIndex;
extern  int             __Is_DLL;

extern  void            (*__sig_init_rtn)(void);
extern  void            (*__sig_fini_rtn)(void);

extern  void            _endthread(void);

static  int             event_ctr = 0;

typedef struct thread_args {
    thread_fn   *rtn;
    void        *argument;
    HANDLE      parent;
    HANDLE      event;
    HANDLE      event_ack;
    HANDLE      *thread_handle;
} thread_args;

static __cdecl begin_thread_helper( thread_args *td )
/***************************************************/
{
    thread_fn           *rtn;
    void                *arg;
    HANDLE              event_ack;
    REGISTRATION_RECORD rr;
    thread_data         *tdata;

    rtn = td->rtn;
    arg = td->argument;

    if( !__Is_DLL ) {                                   /* 15-feb-93 */
        #if defined(__AXP__) || defined(__PPC__)
            tdata = (thread_data *)alloca( __ThreadDataSize );
        #else
            tdata = (thread_data *)__alloca( __ThreadDataSize );
        #endif
        memset( tdata, 0, __ThreadDataSize );
        // tdata->__allocated = 0;
        tdata->__data_size = __ThreadDataSize;
        if( !__NTAddThread( tdata ) ) {
            return;
        }
    }
    // make a copy of the event handle before it disappears from the stack
    event_ack = td->event_ack;
    td->thread_handle = &(__THREADDATAPTR->thread_handle);
    // allow main process to proceed
    SetEvent( td->event );

    __NewExceptionHandler( &rr, 0 );
    __sig_init_rtn();   // fills in a thread-specific copy of signal table
    // wait for main process to have given us the thread handle
    WaitForSingleObject( event_ack, -1 );
    CloseHandle( event_ack );
    (*rtn)( arg );
    _endthread();
}

unsigned long __CBeginThread(thread_fn *start_addr, unsigned stack_size,
                     void *arglist)
{

    DWORD       tid;
    thread_args td;
    HANDLE      th;
    char        *evn;
    char        evname[32];     // we need room for 8+8+8+1 chars

    if( __TlsIndex == NO_INDEX ) {
        if( __NTThreadInit() == FALSE )  return NULL;
        __InitMultipleThread();
    }
    //stack_bottom = stack_bottom;

    stack_size = __Align4K( stack_size );

    td.rtn = start_addr;
    td.argument = arglist;
    td.parent = GetCurrentThread();
    td.thread_handle = 0;

    event_ctr++;
    // make up a unique name like "__bgnthd12def8_2"
    strcpy( evname, "__bgnthd" );
    itoa( GetCurrentThreadId(), &evname[8], 16 );
    evn = &evname[ strlen(evname) ];
    *evn++ = '_';
    itoa( event_ctr, evn, 16 );
    td.event = CreateEvent( NULL, FALSE, FALSE, evname );

    // make up a unique name like "__endthd12def8_2"
    strcpy( evname, "__endthd" );
    itoa( GetCurrentThreadId(), &evname[8], 16 );
    evn = &evname[ strlen(evname) ];
    *evn++ = '_';
    itoa( event_ctr, evn, 16 );
    td.event_ack = CreateEvent( NULL, FALSE, FALSE, evname );

    th = CreateThread( NULL, stack_size, (LPVOID) begin_thread_helper,
                (LPVOID) &td, 0, &tid );
    if( th != NULL ) {
        WaitForSingleObject( td.event, -1 );
        if( td.thread_handle ) {
            (*td.thread_handle) = th;
        } else {
            CloseHandle( th );
        }
        SetEvent( td.event_ack );
    } else {
        tid = -1;
    }
    CloseHandle( td.event );
    return( (unsigned long)th );
}

void __CEndThread( void )
{
    __sig_fini_rtn();
    __DoneExceptionHandler();
    if( ! __Is_DLL ) {
        __NTRemoveThread( TRUE );
    }
    ExitThread( 0 );
}
