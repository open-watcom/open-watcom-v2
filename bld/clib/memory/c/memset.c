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
* Description:  Implementation of memset() and wmemset().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <wchar.h>
#include "xstring.h"


#if defined(__386__)
extern  void    __STOSB( void *, int, unsigned );
#pragma aux     __STOSB "*" parm [eax] [edx] [ecx];

extern  void    *__set386( void *, int, unsigned );
#pragma aux     __set386 =  \
        "push   EAX"            /* save return value*/\
        "mov    DH,DL"          /* duplicate byte value thru EDX */\
        "shl    EDX,8"          /* ... */\
        "mov    DL,DH"          /* ... */\
        "shl    EDX,8"          /* ... */\
        "mov    DL,DH"          /* ... */\
        "call   __STOSB"        /* do store */\
        "pop    EAX"            /* restore return value*/\
        parm [eax] [edx] [ecx] \
        value [eax];
#endif

_WCRTLINK VOID_WC_TYPE *__F_NAME(memset,wmemset)( VOID_WC_TYPE *dst, INT_WC_TYPE c, size_t len )
{
#if defined(__INLINE_FUNCTIONS__) && !defined(__WIDECHAR__) && defined(_M_IX86)
    #if defined(__386__) && defined(__SMALL_DATA__)
        return( __set386( dst, c, len ) );
    #else
        return( _inline_memset( dst, c, len ) );
    #endif
#else
    CHAR_TYPE   *p;

    for( p = dst; len; --len ) {
        *p++ = c;
    }
    return( dst );
#endif
}
