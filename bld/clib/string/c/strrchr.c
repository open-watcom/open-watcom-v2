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
* Description:  Implementation of strchr() and wcsrchr().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stddef.h>
#include <string.h>
#include "riscstr.h"

#if defined( _M_I86 ) && !defined(__WIDECHAR__)

extern  char *_fast_strrchr( const char _WCFAR *, char );

#if defined(__SMALL_DATA__)
#pragma aux    _fast_strrchr = \
        "mov  cx,-1"    \
        "xor  al,al"    \
        "repne scasb"   \
        "not  cx"       \
        "dec  di"       \
        "mov  al,dl"    \
        "std"           \
        "repne scasb"   \
        "cld"           \
        "jne short L1"  \
        "mov  cx,di"    \
        "inc  cx"       \
    "L1:"               \
    __parm __caller     [__es __di] [__dl] \
    __value             [__cx] \
    __modify __exact    [__cx __ax __di]
#else
#pragma aux _fast_strrchr = \
        "mov  cx,-1"    \
        "xor  al,al"    \
        "repne scasb"   \
        "not  cx"       \
        "dec  di"       \
        "mov  al,bl"    \
        "std"           \
        "repne scasb"   \
        "cld"           \
        "jne short L1"  \
        "mov  cx,di"    \
        "inc  cx"       \
        "jmp short L2"  \
    "L1: mov  es,cx"    \
    "L2:"               \
    __parm __caller     [__es __di] [__bl] \
    __value             [__es __cx] \
    __modify __exact    [__es __cx __ax __di]
#endif
#endif

/* Locate the last occurrence of c in the string pointed to by s.
   The terminating null character is considered to be part of the string.
   If the character c is not found, NULL is returned.
*/


#if defined(__RISCSTR__) && defined(__WIDECHAR__)
 CHAR_TYPE *__simple_wcsrchr( const CHAR_TYPE *s, INTCHAR_TYPE c )
#else
 _WCRTLINK CHAR_TYPE *__F_NAME(strrchr,wcsrchr)( const CHAR_TYPE *s, INTCHAR_TYPE c )
#endif
{
#if defined( _M_I86 ) && !defined(__WIDECHAR__)
    return( _fast_strrchr( s, c ) );
#else
    CHAR_TYPE   *p;
    CHAR_TYPE   cc = c;

    p = NULL;       /* assume c will not be found */
    do {
        if( *s == cc )
            p = (CHAR_TYPE *)s;
    } while( *s++ != NULLCHAR );
    return( p );
#endif
}
