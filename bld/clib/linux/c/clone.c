/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2016 Open Watcom Contributors. 
*    All Rights Reserved.
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
* Description:  An actual, functioning clone implementation
*
****************************************************************************/


#include "variety.h"
#include <unistd.h>
#include <stdarg.h>
#include <process.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <string.h>
#include <stdint.h>
#include "linuxsys.h"

#include <stdio.h>


#define STACK_PTR(x,p) (((void **)x)+p)


/* Our function handling the call to the user-specified function after
 * cloning.  Note that it has been expressly defined by #pragma aux
 * in order to force Watcom to pass arguments on the stack and on
 * function exit properly cleanup stack.
 */

#pragma aux __callfn parm routine [];

static void __callfn( int (*__fn)(void *), void *args, void *tls )
{
    int ret;

    /* If tls has been specified, we need to set it via a
     * system call for the child now.
     */
    if(tls != NULL)
        sys_call1(SYS_set_thread_area, (u_long)tls);
    
    /* Call the user function */
    ret = __fn(args);
    
    /* Kill this cloned process now, using __fn's return value as an
     * exit code
     */
    _sys_exit( ret );
}

_WCRTLINK pid_t clone(int (*__fn)(void *), void *__child_stack, int __flags, 
                      void *args, ...)
{
    syscall_res res;

    /* Optional arguments based on the presence of certain flags */
    pid_t *ppid;
    pid_t *ctid;
    void *tls;    /* Could be a struct user_desc * */
    
    /* The number of optional args expected */
    int n;
    va_list additional;
    
    ppid = NULL;
    ctid = NULL;
    tls = NULL;
    
    /* Determine how many optional arguments we're storing based
     * on flags
     */
    n = 0;
    if(__flags & (CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID))
        n = 3;
    else if(__flags & CLONE_SETTLS)
        n = 2;
    else if(__flags & CLONE_PARENT_SETTID)
        n = 1;
    
    /* Process optional arguments, if any */
    va_start(additional, args);
    if(n > 0)
        ppid = va_arg(additional, pid_t *);
    if(n > 1)
        tls = va_arg(additional, struct user_desc *);
    if(n > 2)
        ctid = va_arg(additional, pid_t *);
    va_end(additional);
    
    /* Store what we need in our stack space.  Once clone occurs, our
     * stack should be positioned just beyond these three arguments.
     */
    __child_stack = STACK_PTR( __child_stack, -3 );
    *STACK_PTR( __child_stack, 0 ) = __fn;
    *STACK_PTR( __child_stack, 1 ) = args;
    *STACK_PTR( __child_stack, 2 ) = tls;

    /* Call the actual clone operation */
    res = sys_call5( SYS_clone, (u_long)__flags, (u_long)__child_stack, (u_long)ppid, (u_long)ctid, (u_long)NULL);

    if(!__syscall_iserror(res)) {
        
        /* If we're the child... */
        if(__syscall_val( pid_t, res ) == 0) {
        
            /* The arguments for __callfn are actually on the stack, but
             * we don't have access to them at this point.  What we can
             * do is trick the compiler.  We'll typecast __callfn into
             * a function that takes no arguments.  When we call this
             * fake function, it will actually call __callfn.
             * __callfn should pull its arguments off the stack.
             * We didn't pass any, but the
             * proper ones are already waiting on our stack.
             */
            ((void (*)(void))__callfn)();
        }
    }
    
    /* We may now be returning to the parent thread with a
     * child thread id depending on how things have gone above
     */
    __syscall_return( pid_t, res );
}
