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
* Description:  Implementation of strncmp() and wcsncmp().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <string.h>
#include "riscstr.h"

#if defined( _M_I86 ) && !defined(__WIDECHAR__)

#if defined(__SMALL_DATA__)

extern int _fast_strncmp( const char *, const char _WCFAR *, size_t );
#pragma aux _fast_strncmp = \
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
    "L1:"               \
    __parm __caller     [__si] [__es __di] [__cx] \
    __value             [__cx] \
    __modify __exact    [__dx __ax __di __cx __si]

#else

extern int _fast_strncmp( const char *, const char _WCFAR *, size_t );
#pragma aux _fast_strncmp = \
        "push ds"       \
        "mov  ds,dx"    \
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
    __parm __caller     [__dx __si] [__es __di] [__cx] \
    __value             [__cx] \
    __modify __exact    [__dx __ax __di __cx __si]

#endif

#endif

/* return <0 if s<t, 0 if s==t, >0 if s>t */

#if defined(__RISCSTR__) && defined(__WIDECHAR__)
 int __simple_wcsncmp( const CHAR_TYPE *s, const CHAR_TYPE *t, size_t n )
#else
 _WCRTLINK int __F_NAME(strncmp,wcsncmp)( const CHAR_TYPE *s, const CHAR_TYPE *t, size_t n )
#endif
{
#if defined( _M_I86 ) && !defined(__WIDECHAR__)
    if( n )
        return( _fast_strncmp( s, t, n ) );

    return( 0 );
#else
    for( ;; ) {
        if( n == 0 )
            return( 0 );       /* equal */
        if( *s != *t )
            return( *s - *t ); /* less than or greater than */
        if( *s == NULLCHAR )
            return( 0 );       /* equal */
        ++s;
        ++t;
        --n;
    }
#endif
}
