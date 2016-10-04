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
* Description:  POSIX thread mutex attributes
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include "rterrno.h"
#include "thread.h"


_WCRTLINK int pthread_mutexattr_init(pthread_mutexattr_t *__attr)
{
    if(__attr == NULL)
        return( EINVAL );
        
    __attr->type = PTHREAD_MUTEX_DEFAULT;
    
    return( 0 );
}

_WCRTLINK int pthread_mutexattr_destroy(pthread_mutexattr_t *__attr)
{
    return( 0 );
}

_WCRTLINK int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *__attr, int __prioceiling)
{
    return( ENOSYS );
}

_WCRTLINK int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *__attr, int *__prioceiling)
{
    return( ENOSYS );
}

_WCRTLINK int pthread_mutexattr_setprotocol(pthread_mutexattr_t *__attr, int __protocol)
{
    return( ENOSYS );
}

_WCRTLINK int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *__attr, int *__protocol)
{
    if(__protocol == NULL)
        return( EINVAL );
        
    __protocol = PTHREAD_PRIO_NONE;
    
    return( 0 );
}

_WCRTLINK int pthread_mutexattr_getpshared(const pthread_mutexattr_t *__attr, int *__pshared)
{
    if(__attr == NULL || __pshared == NULL)
        return( EINVAL );
        
    *__pshared = PTHREAD_PROCESS_PRIVATE;
    
    return( 0 );
}

_WCRTLINK int pthread_mutexattr_setpshared(pthread_mutexattr_t *__attr, int __pshared)
{
    if(__attr == NULL)
        return( EINVAL );
    
    if(__pshared != PTHREAD_PROCESS_PRIVATE)
        return( ENOSYS );
        
    return( 0 );
}

_WCRTLINK int pthread_mutexattr_gettype(const pthread_mutexattr_t *__attr, int *__type)
{
    if(__attr == NULL || __type == NULL)
        return( EINVAL );
        
    *__type = __attr->type;
    return( 0 );
}

_WCRTLINK int pthread_mutexattr_settype(pthread_mutexattr_t *__attr, int __type)
{
    if(__attr == NULL)
        return( EINVAL );
        
    if(__type != PTHREAD_MUTEX_DEFAULT &&
       __type != PTHREAD_MUTEX_RECURSIVE &&
       __type != PTHREAD_MUTEX_ERRORCHECK &&
       __type != PTHREAD_MUTEX_NORMAL)
    {
        return( EINVAL );
    }
    
    /* This implementation can't meet the requirements of
     * this type (same # of locks and unlocks...)
     */
    if(__type == PTHREAD_MUTEX_RECURSIVE)
        return( ENOSYS );
        
    __attr->type = __type;
    
    return( 0 );
}   
