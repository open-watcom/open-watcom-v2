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
* Description:  Linux atomic functions
*
****************************************************************************/


#include "variety.h"
#include "atomic.h"


#ifdef __386__

/* Simple wrapper around Intel CMPXCHG */
static unsigned cmpxchg( volatile int *i, int j, int k );
#pragma aux cmpxchg = \
        "lock cmpxchg [edx],ecx"    \
        "jnz short noxchg"          \
        "mov    eax,1"              \
        "jmp short donexchg"        \
    "noxchg:"                       \
        "mov    eax,0"              \
    "donexchg:"                     \
    __parm __caller     [__edx] [__eax] [__ecx] \
    __value             [__eax] \
    __modify __exact    [__eax]

static void increment( volatile int *i );
#pragma aux increment = \
        "lock inc dword ptr [eax]" \
    __parm __caller     [__eax] \
    __value             \
    __modify __exact    []

static void decrement( volatile int *i );
#pragma aux decrement = \
        "lock dec dword ptr [eax]" \
    __parm __caller     [__eax] \
    __value             \
    __modify __exact    []

#endif

int __atomic_compare_and_swap( volatile int *dest, int expected, int source )
{
    unsigned ret;

#ifdef __386__
    ret = cmpxchg( dest, expected, source );
#else
    ret = (unsigned)0;
#endif
    return( ret == (unsigned)1 );
}

int __atomic_add( volatile int *dest, int delta )
{
    int value;

    for( ;; ) {
        value = *dest;
        if( __atomic_compare_and_swap( dest, value, value + delta ) ) {
            return( 1 );
        }
    }
    return( 0 );
}

void __atomic_increment( volatile int *i )
{
#ifdef __386__
    increment(i);
#else
    *i = *i + 1;
#endif
}

void __atomic_decrement( volatile int *i )
{
#ifdef __386__
    decrement(i);
#else
    *i = *i - 1;
#endif
}
