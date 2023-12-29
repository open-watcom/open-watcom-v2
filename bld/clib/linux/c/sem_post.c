/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Linux semaphore post routine
*
****************************************************************************/


#include "variety.h"
#include <semaphore.h>
#include <stddef.h>
#include <stdio.h>
#include <sched.h>
#include <limits.h>
#include "futex.h"
#include "atomic.h"
#include "rterrno.h"
#include "thread.h"


_WCRTLINK int sem_post( sem_t *sem )
{
    if( sem == NULL ) {
        _RWD_errno = EINVAL;
        return( -1 );
    }

    while(sem->value < 0) __atomic_increment( &sem->value );

    __atomic_increment( &sem->value );

    __futex( &sem->futex, FUTEX_WAKE_PRIVATE, INT_MAX, NULL, 0 );

    __atomic_compare_and_swap(&sem->futex, 1, 0);

    return( 0 );
}
