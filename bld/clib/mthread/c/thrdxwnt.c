/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Win32 extended threading routines.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <time.h>
#include <process.h>
#include <string.h>
#include <dos.h>
#include <windows.h>
#include "roundmac.h"
#include "seterrno.h"
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
    thread_fnex *start_addr;
    void        *argument;
} thread_args;

static DWORD WINAPI begin_thread_helper( thread_args *td )
/********************************************************/
{
    thread_fnex         *start_addr;
    void                *arg;
    REGISTRATION_RECORD rr;
    thread_data         *tdata;
    DWORD               rv;

    start_addr = td->start_addr;
    arg = td->argument;
    free( td );

    if( !__Is_DLL ) {
        tdata = __alloca( __ThreadDataSize );
        if( tdata == NULL )
            return( 0 );
        memset( tdata, 0, __ThreadDataSize );
        // tdata->__allocated = 0;
        tdata->__data_size = __ThreadDataSize;
        if( !__NTAddThread( tdata ) ) {
            return( 0 );
        }
    }

    __NewExceptionFilter( &rr );
    __sig_init_rtn();   // fills in a thread-specific copy of signal table
    rv = (*start_addr)( arg );
    _endthreadex( rv );
    return( rv );
}

unsigned long __CBeginThreadEx(
    void *security,
    unsigned stack_size,
    thread_fnex *start_addr,
    void *arglist,
    unsigned initflag,
    unsigned *thrdaddr )
{

    thread_args *td;
    HANDLE      th;

    if( __TlsIndex == NO_INDEX ) {
        if( !__NTThreadInit() )
            return( 0 );
        __InitMultipleThread();
    }

    td = malloc( sizeof( *td ) );
    if( td == NULL ) {
        _RWD_errno = ENOMEM;
        return( 0 );
    }

    stack_size = __ROUND_UP_SIZE_4K( stack_size );

    td->start_addr = start_addr;
    td->argument = arglist;

    th = CreateThread(
        (LPSECURITY_ATTRIBUTES)security,
        (DWORD)stack_size,
        (LPTHREAD_START_ROUTINE)&begin_thread_helper,
        (LPVOID) td,
        (DWORD)initflag,
        (LPDWORD)thrdaddr );

    if( th == NULL )
        free( td );

    return( (unsigned long)th );
}

void __CEndThreadEx( unsigned retval )
{
    __sig_fini_rtn();
    __DoneExceptionFilter();
    if( ! __Is_DLL ) {
        __NTRemoveThread( FALSE );
    }
    ExitThread( retval );
    // never return
}
