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
* Description:  Linux multi-threading functions
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include <signal.h>
#include <float.h>
#include <unistd.h>
#include <sched.h>
#include <process.h>
#include "rtdata.h"
#include "liballoc.h"
#include "extfunc.h"
#include "linuxsys.h"
#include "mthread.h"
#include "cthread.h"


struct __lnx_thread {
    thread_fn   *start_addr;
    void        *args;
};

static void __cloned_lnx_start_fn( void *thrvoiddata )
{
    struct __lnx_thread *thrdata;

    thrdata = (struct __lnx_thread *)thrvoiddata;
    thrdata->start_addr( thrdata->args );
    free( thrvoiddata );

    _sys_exit( 0 );
}

int __CBeginThread( thread_fn *start_addr, void *stack_bottom,
                    unsigned stack_size, void *arglist )
/******************************************************/
{
    pid_t               pid;
    struct __lnx_thread *thrdata;
    unsigned            flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND
                            | CLONE_THREAD | CLONE_SYSVSEM 
                            | CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID | CLONE_DETACHED;
    
    if( stack_size == 0 && stack_bottom == NULL ) {
        stack_size = 4 * 1024;  /* Docs and other platforms suggest this is
                                 * the OpenWatcom default
                                 */
        stack_bottom = malloc( stack_size );
    }

    if( start_addr == NULL || stack_bottom == NULL || stack_size == 0 ) {
        return( -1 );
    }

    thrdata = (struct __lnx_thread *)malloc( sizeof( struct __lnx_thread ) );
    if( thrdata == NULL ) {
        _RWD_errno = ENOMEM;
        return( -1 );
    }
    thrdata->start_addr = start_addr;
    thrdata->args = arglist;

    pid = clone( (int(*)(void *))__cloned_lnx_start_fn,  
                 (void *)( (int)stack_bottom + stack_size ),
                 flags, 
                 thrdata );

    return( (int)pid );
}

void __CEndThread( void )
/***********************/
{
    _sys_exit( 0 );
}
