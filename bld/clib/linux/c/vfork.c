/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of vfork() for Linux.
*
****************************************************************************/


#include "variety.h"
#include <unistd.h>
#include "thread.h"
#include "rterrno.h"


#if defined( __386__ )

/* Since both parent and child will be sharing the same stack, we must
 * take precautions to avoid changing the existing stack frame after
 * the system call returns.  In particular, this means that we cannot do
 * a 'ret' since that pops an address off the stack that the parent would
 * itself later need when it runs. This is not necessary in case of error
 * (because there is no child).
 *
 * Otherwise, the code is equivalent to:
 *
 *  syscall_res res = sys_call0( SYS_vfork );
 *  __syscall_return( pid_t, res );
 *
 * Since no stack is available to save registers before they are used,
 * the caller must not assume their contents will be preserved. This is
 * signalled to the caller with the line:
 *    #pragma aux vfork modify [edx];
 * in the header file.
 */

#pragma aux __set_errno_vfork __parm [__eax] __modify [__edx]
static int __set_errno_vfork( int err )
{
    _RWD_errno = err;
    return( -1 );
}

static int __vfork( void );
#pragma aux __vfork = \
        "pop    edx"        /* Get the return address off the stack */ \
        "mov    eax,190"    /* SYS_vfork */ \
        "int    80h" \
        "push   edx" \
        "cmp    eax,-125" \
        "jnb    __set_errno_vfork" \
    __value [__eax] \
    __modify __exact [__eax __edx]

_WCRTLINK pid_t vfork( void )
{
    return( __vfork() );
}

#endif
