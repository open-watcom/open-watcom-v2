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
#include <stddef.h>
#include <string.h>

#if defined(__386__)

#if defined(__FLAT__)
extern  void    movefwd( char _WCFAR *dst, const char _WCFAR *src, unsigned len);
#pragma aux     movefwd =  \
        0x06            /* push es */\
        0x8e 0xc2       /* mov es,dx */\
        0x1e            /* push ds */\
        0x96            /* xchg esi,eax */\
        0x8e 0xd8       /* mov ds,ax */\
        0xd1 0xe9       /* shr cx,1 */\
        0x66 0xf3 0xa5  /* rep movsw */\
        0x11 0xc9       /* adc cx,cx */\
        0xf3 0xa4       /* rep movsb */\
        0x1f            /* pop ds */\
        0x07            /* pop es */\
        parm [dx edi] [si eax] [ecx] \
        modify exact [edi esi ecx eax];
#else
extern  void    movefwd( char _WCFAR *dst, const char _WCFAR *src, unsigned len);
#pragma aux     movefwd =  \
        0x1e            /* push ds */\
        0x96            /* xchg esi,eax */\
        0x8e 0xd8       /* mov ds,ax */\
        0xd1 0xe9       /* shr cx,1 */\
        0x66 0xf3 0xa5  /* rep movsw */\
        0x11 0xc9       /* adc cx,cx */\
        0xf3 0xa4       /* rep movsb */\
        0x1f            /* pop ds */\
        parm [es edi] [si eax] [ecx] \
        modify exact [edi esi ecx eax];
#endif

#elif defined( _M_I86 )
#if defined(__SMALL_DATA__)
extern  void    movebwd( char _WCFAR *dst, const char _WCFAR *src, unsigned len);
#pragma aux     movebwd =  \
        0xfd            /* std */\
        0x1e            /* push ds */\
        0x96            /* xchg si,ax */\
        0x8e 0xd8       /* mov ds,ax */\
        0x4e            /* dec si */\
        0x4f            /* dec di */\
        0xd1 0xe9       /* shr cx,1 */\
        0xf3 0xa5       /* rep movsw */\
        0x11 0xc9       /* adc cx,cx */\
        0x46            /* inc si */\
        0x47            /* inc di */\
        0xf3 0xa4       /* rep movsb */\
        0xfc            /* cld */\
        0x1f            /* pop ds */\
        parm [es di] [si ax] [cx] \
        modify exact [di si cx ax];

extern  void    movefwd( char _WCFAR *dst, const char _WCFAR *src, unsigned len);
#pragma aux     movefwd =  \
        0x1e            /* push ds */\
        0x96            /* xchg si,ax */\
        0x8e 0xd8       /* mov ds,ax */\
        0xd1 0xe9       /* shr cx,1 */\
        0xf3 0xa5       /* rep movsw */\
        0x11 0xc9       /* adc cx,cx */\
        0xf3 0xa4       /* rep movsb */\
        0x1f            /* pop ds */\
        parm [es di] [si ax] [cx] \
        modify exact [di si cx ax];
#else
extern  void    movebwd( char _WCFAR *dst, const char _WCFAR *src, unsigned len);
#pragma aux     movebwd =  \
        0xfd            /* std */\
        0x1e            /* push ds */\
        0x96            /* xchg si,ax */\
        0x8e 0xd8       /* mov ds,ax */\
        0x4e            /* dec si */\
        0x4f            /* dec di */\
        0xd1 0xe9       /* shr cx,1 */\
        0xf3 0xa5       /* rep movsw */\
        0x11 0xc9       /* adc cx,cx */\
        0x46            /* inc si */\
        0x47            /* inc di */\
        0xf3 0xa4       /* rep movsb */\
        0xfc            /* cld */\
        0x1f            /* pop ds */\
        parm [es di] [si ax] [cx] \
        modify exact [di si cx ax];

extern  void    movefwd( char _WCFAR *dst, const char _WCFAR *src, unsigned len);
#pragma aux     movefwd =  \
        0x1e            /* push ds */\
        0x96            /* xchg si,ax */\
        0x8e 0xd8       /* mov ds,ax */\
        0xd1 0xe9       /* shr cx,1 */\
        0xf3 0xa5       /* rep movsw */\
        0x11 0xc9       /* adc cx,cx */\
        0xf3 0xa4       /* rep movsb */\
        0x1f            /* pop ds */\
        parm [es di] [si ax] [cx] \
        modify exact [di si cx ax];
#endif
#else
#error platform not supported
#endif


_WCRTLINK void _WCFAR *_fmemmove( void _WCFAR *t, const void _WCFAR *f, size_t len )
    {
        char _WCFAR *to = t;
        const char _WCFAR *from = f;
        if( from == to ) {
            return( to );
        }
        if( from < to  &&  from + len > to ) {  /* if buffers are overlapped*/
#if defined(__HUGE__) || defined(__386__)
            to += len;
            from += len;
            while( len != 0 ) {
                to--;
                from--;
                *to = *from;
                len--;
            }
#else
            movebwd(( to + len ) - 1, ( from + len ) - 1, len );
#endif
        } else {
            movefwd( to, from, len );
        }
        return( to );
    }

