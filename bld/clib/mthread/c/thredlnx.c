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
#include <errno.h>
#include <process.h>

#include "liballoc.h"
#include "osthread.h"
//#include "thread.h"
#include "rtdata.h"
#include "stacklow.h"
#include "extfunc.h"
#include "mthread.h"
#include "seterrno.h"
#include "cthread.h"
#include "linuxsys.h"

int __CBeginThread( thread_fn *start_addr, void *stack_bottom,
                    unsigned stack_size, void *arglist )
/******************************************************/
{
    pid_t pid;

    if( start_addr == NULL || stack_bottom == NULL || stack_size == 0 ) {
        return( -1L );
    }
    pid = clone( CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD, (void*)((int)stack_bottom + stack_size) );
    if( pid ) {
        return( (int)pid );
    } else {
        start_addr(arglist);
        _endthread();
        return 0;
    }
}

void __CEndThread( void )
/***********************/
{
    sys_exit(0);
}
