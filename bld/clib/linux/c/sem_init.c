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
* Description:  Linux semaphore initialization and destroy routines
*
****************************************************************************/

#include <semaphore.h>
#include <limits.h>
#include "variety.h"
#include "rterrno.h"

_WCRTLINK int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    if(value > SEM_VALUE_MAX) {
        _RWD_errno = EINVAL;
        return -1;
    }
    
    if(pshared != 0) {
        _RWD_errno = ENOSYS;
        return -1;
    }
    
    sem->value = value;
    return 0;
}

_WCRTLINK int sem_destroy(sem_t *sem) 
{
    if (sem_trywait(sem) != 0) {
        _RWD_errno = EBUSY;
        return -1;
    }
    return 0;
}
