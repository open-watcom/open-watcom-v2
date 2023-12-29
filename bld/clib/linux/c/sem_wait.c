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
* Description:  Linux semaphore wait routines
*
****************************************************************************/


#include "variety.h"
#include <semaphore.h>
#include <time.h>
#include <sched.h>
#include "futex.h"
#include "atomic.h"
#include "rterrno.h"
#include "thread.h"
#include "linuxsys.h"


static int __decrement_if_positive( volatile int *dest )
{
int value;

    value = *dest;
    if( value > 0 ) {
        return (__atomic_compare_and_swap(dest, value, value-1));
    }
    return( 0 );
}

_WCRTLINK int sem_wait( sem_t *sem )
{
int res;
struct timespec timer;

    if( sem == NULL ) {
        _RWD_errno = EINVAL;
        return( -1 );
    }

    timer.tv_sec = 0;
    timer.tv_nsec = 1E+5;

    while( !__decrement_if_positive( &sem->value ) ) {

        if(sem->value <= 0)
            res = __futex( &sem->futex, FUTEX_WAIT_PRIVATE, 1, &timer, 0 );
        else
            res = 0;
    }

    if(sem->value == 0) __atomic_compare_and_swap(&sem->futex, 0, 1);

    return( 0 );
}

_WCRTLINK int sem_timedwait( sem_t *sem, const struct timespec *abstime )
{
    int             ret;
    struct timespec reltime;

    if( sem == NULL ) {
        _RWD_errno = EINVAL;
        return( -1 );
    }

    while( !__decrement_if_positive( &sem->value ) ) {

        clock_gettime( CLOCK_MONOTONIC, &reltime );
        reltime.tv_sec = abstime->tv_sec - reltime.tv_sec;
        reltime.tv_nsec = abstime->tv_nsec - reltime.tv_nsec;
        if(reltime.tv_nsec < 0) {
            reltime.tv_sec--;
            reltime.tv_nsec += 1E+9;
        }

        if(sem->value <= 0)
            ret = __futex( &sem->futex, FUTEX_WAIT_PRIVATE, 1, &reltime, 0 );
        else
            ret = 0;

        if(ret == -ETIMEDOUT) {
            _RWD_errno = ETIMEDOUT;
            return( -1 );
        }

    }

    if(sem->value == 0) __atomic_compare_and_swap(&sem->futex, 0, 1);

    return( 0 );
}

_WCRTLINK int sem_trywait( sem_t *sem )
{
    struct timespec timer;
    int             ret;

    if( sem == NULL ) {
        _RWD_errno = EINVAL;
        return( -1 );
    }
    timer.tv_sec = 0;
    timer.tv_nsec = 0;

    ret = 0;

    if( !__decrement_if_positive( &sem->value ) ) {

        if(sem->value <= 0)
            ret = __futex( &sem->futex, FUTEX_WAIT_PRIVATE, 1, &timer, 0 );
        else
            ret = 0;

    } else {

        if(sem->value == 0) __atomic_compare_and_swap(&sem->futex, 0, 1);
        return( 0 );

    }

    if( __decrement_if_positive( &sem->value ) )
        return( 0 );

    _RWD_errno = EAGAIN;
    return( -1 );
}
