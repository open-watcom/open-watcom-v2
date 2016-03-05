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
* Description:  Linux semaphore wait routines
*
****************************************************************************/

#include <semaphore.h>
#include <time.h>
#include "variety.h"
#include "rterrno.h"
#include "futex.h"
#include "atomic.h"

static int __decrement_if_positive( volatile int *dest )
{
    if(*dest > 0)
        return __atomic_add(dest, -1) <= 0;
        
    return 0;
}

_WCRTLINK int sem_wait( sem_t *sem ) 
{
    if(sem == NULL) {
        _RWD_errno = EINVAL;
        return -1;
    }

    if(__decrement_if_positive(&sem->value))
        return 0;
    
    do {
        __futex(&sem->value, FUTEX_WAIT_PRIVATE, 0, NULL);
    } while(!__decrement_if_positive(&sem->value));
    
    return 0;
}

_WCRTLINK int sem_trywait( sem_t *sem ) 
{
struct timespec timer;
int ret;
    
    if(sem == NULL) {
        _RWD_errno = EINVAL;
        return -1;
    }
    
    timer.tv_sec = 0;
    timer.tv_nsec = 0;

    if(__decrement_if_positive(&sem->value))
        return 0;
        
    do {
        ret = __futex(&sem->value, FUTEX_WAIT_PRIVATE, 0, &timer);
    } while(!__decrement_if_positive(&sem->value));

    if(ret == 0)
        return 0;

    _RWD_errno = EAGAIN;
    return -1;
}
