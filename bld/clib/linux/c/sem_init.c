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


#include "variety.h"
#include <stdlib.h>
#include <semaphore.h>
#include <limits.h>
#include "rtinit.h"
#include "exitwmsg.h"
#include "rterrno.h"
#include "thread.h"


#define SEM_VALUE_MAX   INT_MAX

extern unsigned is386( void );
/* Try and flip the AC bit in EFlags */
#pragma aux is386 =             \
        ".586"                  \
        "mov    dx,sp"          \
        "and    sp,0xfffc"      \
        "pushfd"                \
        "pushfd"                \
        "pop    eax"            \
        "xor    eax,0x40000"    \
        "push   eax"            \
        "popfd"                 \
        "pushfd"                \
        "pop    ebx"            \
        "popfd"                 \
        "xor    eax,ebx"        \
        "shr    eax,18"         \
        "and    eax,1"          \
        "mov    sp,dx"          \
        value [eax] modify [ebx edx]

static int  __cmpxchg = 0;

_WCRTLINK int sem_init( sem_t *sem, int pshared, unsigned int value )
{
    if( __cmpxchg == 0 ) {
        _RWD_errno = ENOSYS;
        return( -1 );
    }
    if( value > SEM_VALUE_MAX ) {
        _RWD_errno = EINVAL;
        return( -1 );
    }
    if( pshared != 0 ) {
        _RWD_errno = ENOSYS;
        return( -1 );
    }
    sem->value = value;
    return( 0 );
}

_WCRTLINK int sem_destroy( sem_t *sem )
{
    if( sem_trywait( sem ) != 0 ) {
        _RWD_errno = EBUSY;
        return( -1 );
    }
    return( 0 );
}

static void __check_cmpxchg( void )
{
    if( !is386() ) {
        __cmpxchg = 1;
    }
}

AXI( __check_cmpxchg, INIT_PRIORITY_RUNTIME )
