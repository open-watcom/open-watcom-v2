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
* Description:  POSIX thread scheduler set/get functions
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <pthread.h>
#include <sys/types.h>
#include <sched.h>

#include "_ptint.h"


_WCRTLINK int pthread_getschedparam(pthread_t __thr, int *__policy, struct sched_param *__param)
{
int ret;
pid_t tid;

    ret = 0;

    tid = __get_thread_id(__thr);

    if(__policy != NULL)
        *__policy = sched_getscheduler(tid);
    
    if(__param != NULL)
        ret = sched_getparam(tid, __param);

    return( ret );
}


_WCRTLINK int pthread_setschedparam(pthread_t __thr, int __policy, const struct sched_param *__param)
{
pid_t tid;

    tid = __get_thread_id(__thr);

    return( sched_setscheduler(tid, __policy, __param) );
}
