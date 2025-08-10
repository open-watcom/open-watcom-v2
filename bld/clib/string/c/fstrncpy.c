/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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

_WCRTLINK char _WCFAR *_fstrncpy( char _WCFAR *s, const char _WCFAR *t, size_t len )
{
#ifdef _M_I86
    if( len ) {
        return( fast_strncpy( s, t, len ) );
    }
    return( s );
#else
    char _WCFAR     *p;

    for( p = s; len != 0 && *t != '\0'; --len ) {
        *p++ = *t++;
    }
    while( len-- != 0 ) {
        *p++ = '\0';      /* pad destination string with null chars */
    }
    return( s );
#endif
}
