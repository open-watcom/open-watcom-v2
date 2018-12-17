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
* Description:  Implementation of strncat() and wcsncat().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <string.h>


#if defined( _M_I86 ) && !defined(__WIDECHAR__)

#if defined(__SMALL_DATA__)

extern char *_fast_strncat( char _WCFAR *, const char *, size_t );
#pragma aux _fast_strncat = \
        "push di"           \
        "mov  cx,-1"        \
        "xor  ax,ax"        \
        "repne scasb"       \
        "dec  di"           \
        "mov  cx,dx"        \
    "L1: lodsb"             \
        "stosb"             \
        "test al,al"        \
        "loopne short L1"   \
        "je short L2"       \
        "mov  es:[di],ah"   \
    "L2: pop  ax"           \
    __parm __caller     [__es __di] [__si] [__dx] \
    __value             [__ax] \
    __modify __exact    [__ax __cx __si __di]

#else

extern char *_fast_strncat( char _WCFAR *, const char *, size_t );
#pragma aux _fast_strncat = \
        "push ds"           \
        "mov  ds,cx"        \
        "push di"           \
        "mov  cx,-1"        \
        "xor  ax,ax"        \
        "repne scasb"       \
        "dec  di"           \
        "mov  cx,dx"        \
    "L1: lodsb"             \
        "stosb"             \
        "test al,al"        \
        "loopne short L1"   \
        "je short L2"       \
        "mov  es:[di],ah"   \
    "L2: pop  ax"           \
        "pop  ds"           \
    __parm __caller     [__es __di] [__cx __si] [__dx] \
    __value             [__es __ax] \
    __modify __exact    [__ax __cx __si __di]

#endif

#endif


/* concatenate t to the end of dst */

_WCRTLINK CHAR_TYPE *__F_NAME(strncat,wcsncat) ( CHAR_TYPE *dst, const CHAR_TYPE *t, size_t n )
{
#if defined( _M_I86 ) && !defined(__WIDECHAR__)
    if( n ) {
        return( _fast_strncat( dst, t, n ) );
    }
    return( dst );
#else
    CHAR_TYPE   *s;

#ifdef __WIDECHAR__
    s = dst + wcslen( dst );
#else
    s = memchr( dst, NULLCHAR, ~0u );
#endif
    while( n != 0 ) {
        *s = *t;
        if( *s == NULLCHAR )
            break;
        ++s;
        ++t;
        --n;
    }
    *s = NULLCHAR;
    return( dst );
#endif
}
