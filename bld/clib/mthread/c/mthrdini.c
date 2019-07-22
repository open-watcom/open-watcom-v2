/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Thread data initialization. Since this module is always
*               linked in, it should contain functions that are required
*               by other parts of the runtime even for non-MT programs.
*               It should also have as few depenencies on other code
*               as possible.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#if defined( __OS2__ )
    #define INCL_DOSSEMAPHORES
    #define INCL_DOSPROCESS
    #include <wos2.h>
#elif defined( __NT__ )
    #include <time.h>
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include "ntext.h"
#elif defined( __UNIX__ )
    #include <sys/types.h>
    #include <unistd.h>
  #if defined( __LINUX__ )
    #include <process.h>
  #endif
#elif defined( __RDOS__ )
    #include <rdos.h>
#elif defined( __RDOSDEV__ )
    #include <rdos.h>
    #include <rdosdev.h>
#elif defined( __NETWARE__ )
    #include "nw_lib.h"
#endif
#include "rtdata.h"
#include "stacklow.h"
#include "liballoc.h"
#include "thread.h"
#include "trdlist.h"
#include "mthread.h"
#include "rtinit.h"
#include "exitwmsg.h"

_WCRTDATA unsigned  __ThreadDataSize = sizeof( thread_data );

void __InitThreadData( thread_data *tdata )
/*****************************************/
{
    if( tdata != NULL ) {
        tdata->__randnext = 1;
#if defined( __OS2__ )
        // We want to detect stack overflow before it actually
        // happens; let's have a page in reserve.
        // Note that this implementation works fine for any thread
        // or DLL as it gets the stack size from the OS.
        tdata->__stklowP = __threadstack() + 4096;
#elif defined( __NT__ )
        __init_stack_limits( &tdata->__stklowP, 0 );
        tdata->thread_id = GetCurrentThreadId();
#elif defined( _NETWARE_LIBC )
        tdata->thread_id = GetCurrentThreadId();
#elif defined( __QNX__ )
        tdata->thread_id = GetCurrentThreadId();
#elif defined( __LINUX__ )
        tdata->thread_id = GetCurrentThreadId();
#elif defined( __RDOS__ )
        tdata->thread_id = GetCurrentThreadId();
#endif
    }
}
