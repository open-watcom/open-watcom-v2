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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include <string.h>

#ifdef M_I86

extern char _WCFAR *fast_strncpy( char _WCFAR *, const char _WCFAR *, size_t );

#pragma aux fast_strncpy = \
        0x1e            /* push ds    */\
        0x96            /* xchg si,ax */\
        0x8e 0xd8       /* mov ds,ax  */\
        0x57            /* push di */\
        0xac            /* L1: lodsb */\
        0xaa            /* stosb */\
        0x84 0xc0       /* test al,al */\
        0xe0 0xfa       /* loopne L1 */\
        0x31 0xc0       /* xor ax,ax */\
        0xd1 0xe9       /* shr cx,1 */\
        0xf3 0xab       /* rep stosw */\
        0x11 0xc9       /* adc cx,cx */\
        0xf3 0xaa       /* rep stosb */\
        0x58            /* pop ax */\
        0x1f            /* pop ds */\
        parm caller     [es di] [si ax] [cx]\
        value           [es ax] \
        modify exact    [ax cx si di];
#endif

_WCRTLINK char _WCFAR *_fstrncpy( char _WCFAR *dst, const char _WCFAR *src, size_t len )
    {
#ifdef M_I86
        if( len ) {
            return( fast_strncpy( dst, src, len ) );
        }
        return( dst );
#else
        char _WCFAR *ret;

        ret = dst;
        for(;len; --len ) {
            if( *src == '\0' ) break;
            *dst++ = *src++;
        }
        while( len != 0 ) {
            *dst++ = '\0';      /* pad destination string with null chars */
            --len;
        }
        return( ret );
#endif
    }
