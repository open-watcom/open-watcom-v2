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
extern  void    movefwd( char _WCFAR *dst, const char _WCNEAR *src, unsigned len);
#pragma aux     movefwd =  \
        0x06            /* push es   */\
        0x8e 0xc2       /* mov es,dx */\
        0x51            /* push ecx  */\
        0xc1 0xe9 0x02  /* shr ecx,2 */\
        0xf3 0xa5       /* rep movsd */\
        0x59            /* pop ecx   */\
        0x83 0xe1 0x03  /* and ecx,3 */\
        0xf3 0xa4       /* rep movsb */\
        0x07            /* pop es    */\
        parm [dx edi] [esi] [ecx] \
        modify exact [edi esi ecx];
extern  void    movebwd( char _WCFAR *dst, const char _WCNEAR *src, unsigned len);
#pragma aux     movebwd =  \
        0x06            /* push es */\
        0x8e 0xc2       /* mov es,dx */\
        0xfd            /* std */\
        0x4e            /* dec esi */\
        0x4f            /* dec edi */\
        0xd1 0xe9       /* shr ecx,1 */\
        0x66 0xf3 0xa5  /* rep movsw */\
        0x11 0xc9       /* adc ecx,ecx */\
        0x46            /* inc esi */\
        0x47            /* inc edi */\
        0x66 0xf3 0xa4  /* rep movsb */\
        0x07            /* pop es */\
        0xfc            /* cld */\
        parm [dx edi] [esi] [ecx] \
        modify exact [edi esi ecx];

#elif defined(M_I86) && defined(__SMALL_DATA__)
extern  void    movebwd( char _WCFAR *dst, const char _WCNEAR *src, unsigned len);
#pragma aux     movebwd =  \
        0xfd            /* std */\
        0x4e            /* dec si */\
        0x4f            /* dec di */\
        0xd1 0xe9       /* shr cx,1 */\
        0xf3 0xa5       /* rep movsw */\
        0x11 0xc9       /* adc cx,cx */\
        0x46            /* inc si */\
        0x47            /* inc di */\
        0xf3 0xa4       /* rep movsb */\
        0xfc            /* cld */\
        parm [es di] [si] [cx] \
        modify exact [di si cx];

extern  void    movefwd( char _WCFAR *dst, const char _WCNEAR *src, unsigned len);
#pragma aux     movefwd =  \
        0xd1 0xe9       /* shr cx,1 */\
        0xf3 0xa5       /* rep movsw */\
        0x11 0xc9       /* adc cx,cx */\
        0xf3 0xa4       /* rep movsb */\
        parm [es di] [si] [cx] \
        modify exact [di si cx];
#elif defined(M_I86) && defined(__BIG_DATA__)
extern  void    movebwd( char _WCFAR *dst, const char _WCFAR *src, unsigned len);
#pragma aux     movebwd =  \
        0x1e            /* push ds */ \
        0x8e 0xda       /* mov ds,dx */ \
        0xfd            /* std */\
        0x4e            /* dec si */\
        0x4f            /* dec di */\
        0xd1 0xe9       /* shr cx,1 */\
        0xf3 0xa5       /* rep movsw */\
        0x11 0xc9       /* adc cx,cx */\
        0x46            /* inc si */\
        0x47            /* inc di */\
        0xf3 0xa4       /* rep movsb */\
        0xfc            /* cld */\
        0x1f            /* pop ds */ \
        parm [es di] [dx si] [cx] \
        modify exact [di si cx];

extern  void    movefwd( char _WCFAR *dst, const char _WCFAR *src, unsigned len);
#pragma aux     movefwd =  \
        0x1e            /* push ds */ \
        0x8e 0xda       /* mov ds,dx */ \
        0xd1 0xe9       /* shr cx,1 */\
        0xf3 0xa5       /* rep movsw */\
        0x11 0xc9       /* adc cx,cx */\
        0xf3 0xa4       /* rep movsb */\
        0x1f            /* pop ds */ \
        parm [es di] [dx si] [cx] \
        modify exact [di si cx];
#elif defined(__AXP__) || defined(__PPC__)
// no pragma for these ones
#else
#error unrecognized platform
#endif


_WCRTLINK void *memmove( char *to, const char *from, size_t len )
    {
#if defined(__HUGE__) || defined(__AXP__) || defined(__PPC__)
        char *          toStart = to;
#endif

        if( from == to ) {
            return( to );
        }
        if( from < to  &&  from + len > to ) {  /* if buffers are overlapped*/
#if defined(__HUGE__) || defined(__AXP__) || defined(__PPC__)
            to += len;
            from += len;
            while( len != 0 ) {
                *--to = *--from;
                len--;
            }
#else
            movebwd(( to + len ) - 1, ( from + len ) - 1, len );
#endif
        } else {
#if defined(__AXP__) || defined(__PPC__)
            while( len != 0 ) {
                *to++ = *from++;
                len--;
            }
#else
            movefwd( to, from, len );
#endif
        }

#if defined(__HUGE__) || defined(__AXP__) || defined(__PPC__)
        return( toStart );
#else
        return( to );
#endif
    }
