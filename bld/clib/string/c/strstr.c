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
* Description:  Implementation of strstr() and wcsstr().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stddef.h>
#include <string.h>
#if defined(_M_IX86)
 #include <i86.h>
#endif

#if defined( _M_I86 ) && !defined( __WIDECHAR__ )

extern  int     i86_memeq( const char *, const char _WCFAR *, int );

#define _ZFLAG          (INTR_ZF<<8)

#if defined(__SMALL_DATA__)

#pragma aux     i86_memeq = \
        0xf3 0xa6       /* rep cmpsb */\
        0x9f            /* lahf */\
        parm caller     [si] [es di] [cx]\
        value           [ax] \
        modify exact    [si di cx ax];

#else

#pragma aux     i86_memeq = \
        0x1e            /* push ds */ \
        0x8e 0xda       /* mov ds,dx */ \
        0xf3 0xa6       /* rep cmpsb */\
        0x9f            /* lahf */\
        0x1f            /* pop ds */ \
        parm caller     [dx si] [es di] [cx]\
        value           [ax] \
        modify exact    [si di cx ax];

#endif

#define memeq( p1, p2, len )    ( i86_memeq((p1),(p2),(len)) & _ZFLAG )

#else

#define memeq( p1, p2, len )    ( memcmp((p1),(p2),(len)*CHARSIZE) == 0 )

#endif

/* Locate the first occurrence of the string pointed to by s2 in the
   string pointed to by s1.
   The strstr function returns a pointer to the located string, or a
   null pointer if the string is not found.
*/


_WCRTLINK CHAR_TYPE *__F_NAME(strstr,wcsstr)( const CHAR_TYPE *s1, const CHAR_TYPE *s2 )
{
    CHAR_TYPE       *end_of_s1;
    size_t          s1len, s2len;

    if( s2[0] == NULLCHAR ) {
        return( (CHAR_TYPE *)s1 );
    } else if( s2[1] == NULLCHAR ) {
        return( __F_NAME(strchr,wcschr)( s1, s2[0] ) );
    }
#ifdef __WIDECHAR__
    end_of_s1 = (CHAR_TYPE*)s1 + wcslen( s1 );
#else
    end_of_s1 = memchr( s1, NULLCHAR, ~0u );
#endif
    s2len = __F_NAME(strlen,wcslen)( s2 );
    for( ;; ) {
        s1len = end_of_s1 - s1;
        if( s1len < s2len )
            break;
#ifdef __WIDECHAR__
        s1 = wcschr( s1, *s2 );  /* find start of possible match */
#else
        s1 = memchr( s1, *s2, s1len );  /* find start of possible match */
#endif
        if( s1 == NULL )
            break;
        if( memeq( s1, s2, s2len ) )
            return( (CHAR_TYPE *)s1 );
        ++s1;
    }
    return( NULL );
}
