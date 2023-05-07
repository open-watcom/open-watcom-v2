/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2023Open Watcom Contributors. All Rights Reserved.
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
* Description:  POSIX thread public shutdown implementation
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <stddef.h>
#include <pthread.h>
#include <process.h>
#include <stdio.h>
#include <sched.h>

#include "_ptint.h"


_WCRTLINK _WCNORETURN void pthread_exit( void *value_ptr )
{
    pthread_t myself;

    /* unused parameters */ (void)value_ptr;

    /* Call the thread cleanup routines */
    __call_all_pthread_cleaners();

    myself = __get_current_thread();
    if( myself != NULL ) {
        /* Unlock to release any joins */
        pthread_mutex_unlock( __get_thread_running_mutex( myself ) );

        /* If detached, destroy all internal memory for this thread */
        if( __get_thread_detached( myself ) == 1 ) {
            __unregister_thread( myself );
        }
    }
    _endthread();
    // never return
}
