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

extern char *_fast_strncat( char _WCFAR *, const char *, size_t );

#if defined(__SMALL_DATA__)

#pragma aux _fast_strncat = \
        0x57            /* push di */\
        0xb9 0xff 0xff  /* mov cx,ffffh */\
        0x31 0xc0       /* xor ax,ax */\
        0xf2 0xae       /* repne scasb */\
        0x4f            /* dec di */\
        0x89 0xd1       /* mov cx,dx */\
        0xac            /* L1: lodsb */\
        0xaa            /* stosb */\
        0x84 0xc0       /* test al,al */\
        0xe0 0xfa       /* loopne L1 */\
        0x74 0x03       /* je L2 */\
        0x26 0x88 0x25  /* mov es:[di],ah */\
        0x58            /* L2: pop ax */\
        parm caller     [es di] [si] [dx]\
        value           [ax] \
        modify exact    [ax cx si di];

#else

#pragma aux _fast_strncat = \
        0x1e            /* push ds */ \
        0x8e 0xd9       /* mov ds,cx */ \
        0x57            /* push di */\
        0xb9 0xff 0xff  /* mov cx,ffffh */\
        0x31 0xc0       /* xor ax,ax */\
        0xf2 0xae       /* repne scasb */\
        0x4f            /* dec di */\
        0x89 0xd1       /* mov cx,dx */\
        0xac            /* L1: lodsb */\
        0xaa            /* stosb */\
        0x84 0xc0       /* test al,al */\
        0xe0 0xfa       /* loopne L1 */\
        0x74 0x03       /* je L2 */\
        0x26 0x88 0x25  /* mov es:[di],ah */\
        0x58            /* L2: pop ax */\
        0x1f            /* pop ds */ \
        parm caller     [es di] [cx si] [dx]\
        value           [es ax] \
        modify exact    [ax cx si di];
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
