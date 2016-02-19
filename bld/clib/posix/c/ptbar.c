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
* Description:  POSIX thread barrier implementation
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

_WCRTLINK int pthread_barrier_init( pthread_barrier_t *__barrier,
                                    const pthread_barrierattr_t *__attr, 
                                    unsigned __count )
{

    if(__barrier == NULL)
        return( EINVAL );

    __barrier->access = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if(__barrier->access == NULL)
        return( ENOMEM );
    pthread_mutex_init(__barrier->access, NULL);
    
    __barrier->cond = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
    if(__barrier->cond == NULL) {
        free(__barrier->access);
        return( ENOMEM );
    }
    pthread_cond_init(__barrier->cond, NULL);

    __barrier->count = 0;
    __barrier->limit = __count;

    return( 0 );
}

_WCRTLINK int pthread_barrier_destroy(pthread_barrier_t *__barrier)
{
int res;

    pthread_mutex_lock(__barrier->access);
    if(__barrier->count > 0) {
        pthread_mutex_unlock(__barrier->access);
        return( EBUSY );
    }
    
    res = pthread_cond_destroy(__barrier->cond);
    if(res != 0) {
        pthread_mutex_unlock(__barrier->access);
        return( res );
    }
    
    pthread_mutex_unlock(__barrier->access);
    pthread_mutex_destroy(__barrier->access);
    
    return( 0 );
}

_WCRTLINK int pthread_barrier_wait(pthread_barrier_t *__barrier)
{
int ret;

    pthread_mutex_lock(__barrier->access);
    
    ret = 0;
    
    __barrier->count++;
    
    if(__barrier->count >= __barrier->limit) {
        pthread_cond_broadcast(__barrier->cond);
        __barrier->count = 0;
        ret = PTHREAD_BARRIER_SERIAL_THREAD;
    } else {
        pthread_cond_wait(__barrier->cond, __barrier->access);
    }
    
    pthread_mutex_unlock(__barrier->access);
    
    return( ret );
}
