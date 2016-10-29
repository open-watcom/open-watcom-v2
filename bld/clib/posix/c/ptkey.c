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
* Description:  POSIX thread-specific key/data pairs
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <sys/types.h>
#include <pthread.h>
#include "rterrno.h"
#include "thread.h"

#include "_ptint.h"


_WCRTLINK int pthread_key_create( pthread_key_t *__key, void (*__destructor)(void*) )
{
int ret;
pthread_key_t res;

    res = __register_pkey(__destructor);
    if(res < 0) 
        ret = _RWD_errno;
    else {
        ret = 0;
        *__key = res;
    }
    return( ret );   
}

_WCRTLINK int pthread_key_delete(pthread_key_t __key)
{
int ret;

    ret = 0;

    if(__valid_pkey_id(__key) == 0) 
        __destroy_pkey(__key);
    else
        ret = EINVAL;
        
    return( ret );
}

_WCRTLINK int pthread_setspecific(pthread_key_t __key, void *__value)
{
int ret;

    if(__valid_pkey_id(__key) == 0) 
        ret = __set_pkey_value(__key, __value);
    else
        ret = EINVAL;
    
    return( ret );
}

_WCRTLINK void *pthread_getspecific(pthread_key_t __key)
{
    return( __get_pkey_value(__key) );
}
