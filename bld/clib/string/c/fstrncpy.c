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
* Description:  Implementation of _fstrncpy() - far strncpy().
*
****************************************************************************/


#include "variety.h"
#include <string.h>


#ifdef _M_I86

extern char _WCFAR *fast_strncpy( char _WCFAR *, const char _WCFAR *, size_t );
#pragma aux fast_strncpy = \
        "push ds"           \
        "xchg si,ax"        \
        "mov  ds,ax"        \
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
    __parm __caller     [__es __di] [__si __ax] [__cx] \
    __value             [__es __ax] \
    __modify __exact    [__ax __cx __si __di]

#endif

_WCRTLINK char _WCFAR *_fstrncpy( char _WCFAR *dst, const char _WCFAR *src, size_t len )
{
#ifdef _M_I86
    if( len ) {
        return( fast_strncpy( dst, src, len ) );
    }
    return( dst );
#else
    char _WCFAR     *ret;

    ret = dst;
    for( ;len; --len ) {
        if( *src == '\0' )
            break;
        *dst++ = *src++;
    }
    while( len != 0 ) {
        *dst++ = '\0';      /* pad destination string with null chars */
        --len;
    }
    return( ret );
#endif
}
