/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  POSIX thread join implementation
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <stdio.h>
#include <pthread.h>
#include <process.h>

#include "_ptint.h"


_WCRTLINK int pthread_join(pthread_t thread, void **value_ptr)
{
int res;

    /* Increment the thread's waiters */
    res = pthread_mutex_lock(__get_thread_waiting_mutex(thread));
    if(res != 0)
        return( res );
        
    __increment_thread_waiters(thread);
    pthread_mutex_unlock(__get_thread_waiting_mutex(thread));

    /* Wait for the thread to release its running lock */
    res = pthread_mutex_lock(__get_thread_running_mutex(thread));
    if(res != 0)
        return( res );
    
    /* Copy the "internal only" payload pointer */
    if(value_ptr != NULL) 
        *value_ptr = __get_thread_return_value(thread);

    /* Decrement the waiting count */
    res = pthread_mutex_lock(__get_thread_waiting_mutex(thread));
    if(res == 0) {
        __decrement_thread_waiters(thread);
        pthread_mutex_unlock(__get_thread_waiting_mutex(thread));
    }
    
    /* Unlock it so others may use it */
    pthread_mutex_unlock(__get_thread_running_mutex(thread));
    
    return( 0 );
}    
