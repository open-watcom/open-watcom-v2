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
* Description:  Linux atomic functions
*
****************************************************************************/

#include "variety.h"
#include "rtdata.h"

#ifdef __386__

/* Simple wrapper around Intel CMPXCHG */
static void cmpxchg( volatile int *i, int j, int k );
#pragma aux cmpxchg = \
    "lock cmpxchg [edx], ecx" \
    parm [edx] [eax] [ecx];

#endif

int __atomic_compare_and_swap( volatile int *dest, int expected, int source )
{

#ifdef __386__
    cmpxchg( dest, expected, source );
#endif

    return( *dest == source );
}

int __atomic_add( volatile int *dest, int delta )
{
    int value;

    for( ;; ) {
        value = *dest;
        if( __atomic_compare_and_swap( dest, value, value + delta ) )
            return( value + delta );
    }
    return( 0 );
}
