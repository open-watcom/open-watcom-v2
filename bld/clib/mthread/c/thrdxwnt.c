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
#include <errno.h>
#include "liballoc.h"
#include "ntex.h"
#include "stacklow.h"
#include "sigtab.h"
#include "thread.h"
#include "trdlist.h"
#include "mthread.h"
#include "rtdata.h"
#include "extfunc.h"
#include "seterrno.h"

extern  void            __InitMultipleThread( void );

extern  DWORD           __TlsIndex;
extern  int             __Is_DLL;

extern  void            (*__sig_init_rtn)(void);
extern  void            (*__sig_fini_rtn)(void);

extern  void            _endthreadex(unsigned);

typedef struct thread_args {
    thread_fnex *rtn;
    void        *argument;
    HANDLE      parent;
} thread_args;

static unsigned WINAPI begin_thread_helper( thread_args *td )
/***********************************************************/
{
    thread_fnex         *rtn;
    void                *arg;
    REGISTRATION_RECORD rr;
    thread_data         *tdata;

    rtn = td->rtn;
    arg = td->argument;

    if( !__Is_DLL ) {                                   /* 15-feb-93 */
        #if defined(__AXP__) || defined(__PPC__)
            tdata = alloca( __ThreadDataSize );
        #else
            tdata = __alloca( __ThreadDataSize );
        #endif
        memset( tdata, 0, __ThreadDataSize );
        // tdata->__allocated = 0;
        tdata->__data_size = __ThreadDataSize;
        if( !__NTAddThread( tdata ) ) {
            return( 0 );
        }
    }
    free( td );

    __NewExceptionHandler( &rr, 0 );
    __sig_init_rtn();   // fills in a thread-specific copy of signal table
    (*rtn)( arg );
    _endthreadex( 0 );
    return( 0 );
}

int __CBeginThreadEx(
    void *security,
    unsigned stack_size,
    thread_fnex start_addr,
    void *arglist,
    unsigned initflag,
    unsigned *thrdaddr )
{

    thread_args *td;
    HANDLE      th;

    td = malloc( sizeof( *td ) );
    if( td == NULL ) {
        __set_errno( ENOMEM );
        return( 0 );
    }

    if( __TlsIndex == NO_INDEX ) {
        if( __NTThreadInit() == FALSE )  return( 0 );
        __InitMultipleThread();
    }

    stack_size = __Align4K( stack_size );

    td->rtn = start_addr;
    td->argument = arglist;
    td->parent = GetCurrentThread();

    th = CreateThread(
        (LPSECURITY_ATTRIBUTES)security,
        (DWORD)stack_size,
        (LPTHREAD_START_ROUTINE)&begin_thread_helper,
        (LPVOID) td,
        (DWORD)initflag,
        (LPDWORD)thrdaddr );
    return( (unsigned long)th );
}

void __CEndThreadEx( unsigned retval )
{
    __sig_fini_rtn();
    __DoneExceptionHandler();
    if( ! __Is_DLL ) {
        __NTRemoveThread( TRUE );
    }
    ExitThread( retval );
}
