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
* Description:  Initialize single-threaded mode.
*
****************************************************************************/


#include "variety.h"
#if defined( __NT__ )
#include <windows.h>
#elif defined( __OS2__ )
#include <wos2.h>
#endif
#include "rtdata.h"
#include "thread.h"
#include "snglthrd.h"
#include "mthread.h"


#if defined( __UNIX__ )
thread_data     *__FirstThreadData = (struct thread_data *)&_STACKLOW;
#elif defined( __NETWARE__ )
thread_data     *__FirstThreadData;
#else
thread_data     *__FirstThreadData = NULL;
#endif

static thread_data *__SingleThread( void )
{
    return( __FirstThreadData );
}

_WCRTDATA thread_data *(*__GetThreadPtr)( void ) = __SingleThread;

#if defined( __NETWARE__ )

void __RestoreSingleThreading( void )
{
    __GetThreadPtr = &__SingleThread;
}

#elif defined( __OS2__ )

static thread_data  dummy_stacklow;

void __shutdown_stack_checking( void ) {

    // make sure we are using the single thread data area
    // this is incase there is a DosExitList active
    __GetThreadPtr = &__SingleThread;
    __FirstThreadData = &dummy_stacklow;
}

#endif
