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
* Description:  Linux ptrace() implementation.
*
****************************************************************************/


#include "variety.h"
#include <unistd.h>
#include <sys/ptrace.h>
#include "rtdata.h"
#include "linuxsys.h"

_WCRTLINK long ptrace( int request, int pid, void *addr, void *data )
{
    long    res, ret;

    /* Someone thought having ptrace() behave differently for the PEEK
     * requests was a clever idea. Instead of error code, ptrace()
     * returns the actual value and errno must be checked.
     */
    if( (request >= PTRACE_PEEKTEXT) && (request <= PTRACE_PEEKUSER) )
        *((long**)&data) = &ret;
    res = sys_call4( SYS_ptrace, request, pid, (u_long)addr, (u_long)data );
    if( res >= 0 ) {
        if( (request >= PTRACE_PEEKTEXT) && (request <= PTRACE_PEEKUSER) ) {
            return( ret );
        }
    }
    __syscall_return( long, res );
}
