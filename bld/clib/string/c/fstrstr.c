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
#if defined(__PENPOINT__)  ||  defined(__QNX__)
#include <i86.h>
#else
#include <dos.h>
#endif

#ifdef M_I86

extern  int     i86_memeq( const char _WCFAR *, const char _WCFAR *, int );

#define _ZFLAG          (INTR_ZF<<8)

#pragma aux     i86_memeq = \
        0x1e            /* push ds */\
        0x96            /* xchg si,ax */\
        0x8e 0xd8       /* mov ds,ax  */\
        0xf3 0xa6       /* rep cmpsb */\
        0x9f            /* lahf */\
        0x1f            /* pop ds */\
        parm caller     [si ax] [es di] [cx]\
        value           [ax] \
        modify exact    [si di cx ax];

#define memeq( p1, p2, len )    ( i86_memeq((p1),(p2),(len)) & _ZFLAG )

#else

#define memeq( p1, p2, len )    ( _fmemcmp((p1),(p2),(len)) == 0 )

#endif

/* Locate the first occurrence of the string pointed to by s2 in the
   string pointed to by s1.
   The strstr function returns a pointer to the located string, or a
   null pointer if the string is not found.
*/

_WCRTLINK char _WCFAR *_fstrstr( const char _WCFAR *s1, const char _WCFAR *s2 )
    {
        char _WCFAR *end_of_s1;
        size_t s1len, s2len;

        if( s2[0] == '\0' ) {
            return( (char _WCFAR *)s1 );
        } else if( s2[1] == '\0' ) {
            return( _fstrchr( s1, s2[0] ) );
        }
        end_of_s1 = _fmemchr( s1, '\0', ~0u );
        s2len = _fstrlen( s2 );
        for(;;) {
            s1len = end_of_s1 - s1;
            if( s1len < s2len ) break;
            s1 = _fmemchr( s1, *s2, s1len ); /* find start of possible match */
            if( s1 == NULL ) break;
            if( memeq( s1, s2, s2len ) ) return( (char _WCFAR *)s1 );
            ++s1;
        }
        return( NULL );
    }
