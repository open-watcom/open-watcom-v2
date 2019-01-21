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
* Description:  Implementation of _fstrncat() - far strncat().
*
****************************************************************************/


#include "variety.h"
#include <string.h>

#ifdef _M_I86

extern char _WCFAR *_fast_strncat( char _WCFAR *, const char _WCFAR *, size_t );
#pragma aux _fast_strncat = \
        "push ds"           \
        "xchg si,ax"        \
        "mov  ds,ax"        \
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
    __parm __caller     [__es __di] [__si __ax] [__dx] \
    __value             [__es __ax] \
    __modify __exact    [__ax __cx __si __di]

#endif

/* concatenate t to the end of dst */

_WCRTLINK char _WCFAR *_fstrncat( char _WCFAR *dst, const char _WCFAR *t, size_t n )
{
#ifdef _M_I86
    if( n ) {
        return( _fast_strncat( dst, t, n ) );
    }
    return( dst );
#else
    char _WCFAR     *s;

    s = _fmemchr( dst, '\0', ~0 );
    while( n != 0 ) {
        *s = *t;
        if( *s == '\0' )
            break;
        ++s;
        ++t;
        --n;
    }
    *s = '\0';
    return( dst );
#endif
}
