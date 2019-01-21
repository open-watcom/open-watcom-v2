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
* Description:  Implementation of strncpy() and wcsncpy().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <string.h>
#include "riscstr.h"


#if defined( _M_I86 ) & !defined(__WIDECHAR__)

#if defined(__SMALL_DATA__)

extern char *fast_strncpy( char _WCFAR *, const char *, size_t );
#pragma aux fast_strncpy = \
        "push di"           \
    "L1: lodsb"             \
        "stosb"             \
        "test al,al"        \
        "loopne short L1"   \
        "xor  ax,ax"        \
        "shr  cx,1"         \
        "rep stosw"         \
        "adc  cx,cx"        \
        "rep stosb"         \
        "pop  ax"           \
    __parm __caller     [__es __di] [__si] [__cx] \
    __value             [__ax] \
    __modify __exact    [__ax __cx __si __di]

#else

extern char *fast_strncpy( char _WCFAR *, const char *, size_t );
#pragma aux fast_strncpy = \
        "push ds"           \
        "mov  ds,dx"        \
        "push di"           \
    "L1: lodsb"             \
        "stosb"             \
        "test al,al"        \
        "loopne short L1"   \
        "xor  ax,ax"        \
        "shr  cx,1"         \
        "rep stosw"         \
        "adc  cx,cx"        \
        "rep stosb"         \
        "pop  ax"           \
        "pop  ds"           \
    __parm __caller     [__es __di] [__dx __si] [__cx] \
    __value             [__es __ax] \
    __modify __exact    [__ax __cx __si __di]

#endif

#endif


#if defined(__RISCSTR__) && defined(__WIDECHAR__)
 CHAR_TYPE *__simple_wcsncpy( CHAR_TYPE *dst, const CHAR_TYPE *src, size_t len )
#else
 _WCRTLINK CHAR_TYPE *__F_NAME(strncpy,wcsncpy)( CHAR_TYPE *dst, const CHAR_TYPE *src, size_t len )
#endif
{
#if defined( _M_I86 ) && !defined(__WIDECHAR__)
    if( len )
        return( fast_strncpy( dst, src, len ) );

    return( dst );
#else
    CHAR_TYPE   *ret;

    ret = dst;
    for( ;len; --len ) {
        if( *src == NULLCHAR )
            break;
        *dst++ = *src++;
    }
    while( len != 0 ) {
        *dst++ = NULLCHAR;      /* pad destination string with null chars */
        --len;
    }
    return( ret );
#endif
}
