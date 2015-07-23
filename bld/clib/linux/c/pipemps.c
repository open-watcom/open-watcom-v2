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
* Description:  Linux pipe() implementation for MIPS.
*
****************************************************************************/


#include "variety.h"
#include <unistd.h>
#include "linuxsys.h"

/* The MIPS pipe system call note - the two descriptors are returned
 * in $v0 and $v1 instead of being written to wherever __fildes points to.
 * That also means $v0 doesn't hold return value in case of success,
 * we use zero value in this case.
 */

syscall_res sys_pipe( u_long func, u_long r_4 );
#pragma aux sys_pipe =                          \
    "move   $s0,$a0"                            \
    "syscall"                                   \
    "bnez   $a3, L1"                            \
    "sw     $v0,0($s0)"                         \
    "sw     $v1,4($s0)"                         \
    "move   $v0,$zero"                          \
"L1: move   $v1,$a3"                            \
    parm [$v0] [$a0]                            \
    value [$v1 $v0];

_WCRTLINK int pipe( int __fildes[2] )
{
    syscall_res res = sys_pipe( SYS_pipe, (u_long)__fildes );
    __syscall_return( int, res );
}
