/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
* Description:  Implementation of _fstrncmp() - far strncmp().
*
****************************************************************************/


#include "variety.h"
#include <string.h>


#ifdef  _M_I86

extern int _fast_strncmp( const char _WCFAR *, const char _WCFAR *, size_t );
#pragma aux _fast_strncmp = \
        "push ds"       \
        "xchg si,ax"    \
        "mov  ds,ax"    \
        "mov  dx,di"    \
        "xor  al,al"    \
        "repne scasb"   \
        "mov  cx,di"    \
        "mov  di,dx"    \
        "sub  cx,di"    \
        "repe cmpsb"    \
        "je short L1"   \
        "sbb  cx,cx"    \
        "sbb  cx,-1"    \
    "L1: pop  ds"       \
    __parm __caller     [__si __ax] [__es __di] [__cx] \
    __value             [__cx] \
    __modify __exact    [__ax __cx __dx __di __si]

#endif

/* return <0 if s<t, 0 if s==t, >0 if s>t */

_WCRTLINK int _fstrncmp( const char _WCFAR *s, const char _WCFAR *t, size_t n )
{
#if defined( _M_I86 )
    if( n ) {
        return( _fast_strncmp( s, t, n ) );
    }
    return( 0 );
#else
    for( ;; ) {
        if( n == 0 )
            return( 0 );       /* equal */
        if( *s != *t )
            return( *s - *t ); /* less than or greater than */
        if( *s == '\0' )
            return( 0 );       /* equal */
        ++s;
        ++t;
        --n;
    }
#endif
}
