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
#include "widechar.h"
#include <stddef.h>
#include <string.h>
#include "riscstr.h"

#if defined(M_I86) && !defined(__WIDECHAR__)

extern  char *_fast_strrchr( const char _WCFAR *, char );

#if defined(__SMALL_DATA__)
#pragma aux    _fast_strrchr = \
        0xb9 0xff 0xff  /* mov cx,ffffh */ \
        0x30 0xc0       /* xor al,al */ \
        0xf2 0xae       /* repne scasb */ \
        0xf7 0xd1       /* not cx */ \
        0x4f            /* dec di */ \
        0x88 0xd0       /* mov al,dl */ \
        0xfd            /* std */ \
        0xf2 0xae       /* repne scasb */ \
        0xfc            /* cld */ \
        0x75 0x03       /* jne L1 */ \
        0x89 0xf9       /* mov cx,di */ \
        0x41            /* inc cx */ \
                        /* L1: */ \
        parm caller [es di] [dl] \
        value [cx] \
        modify exact [cx ax di];
#else
#pragma aux    _fast_strrchr = \
        0xb9 0xff 0xff  /* mov cx,ffffh */ \
        0x30 0xc0       /* xor al,al */ \
        0xf2 0xae       /* repne scasb */ \
        0xf7 0xd1       /* not cx */ \
        0x4f            /* dec di */ \
        0x88 0xd8       /* mov al,bl */ \
        0xfd            /* std */ \
        0xf2 0xae       /* repne scasb */ \
        0xfc            /* cld */ \
        0x75 0x04       /* jne L1 */ \
        0x89 0xf9       /* mov cx,di */ \
        0x41            /* inc cx */ \
        0xa9            /* hide 2 bytes */ \
        0x8e 0xc1       /* L1: mov es,cx */ \
        parm caller [es di] [bl] \
        value [es cx] \
        modify exact [es cx ax di];
#endif
#endif

/* Locate the last occurrence of c in the string pointed to by s.
   The terminating null character is considered to be part of the string.
   If the character c is not found, NULL is returned.
*/


#if defined(__RISCSTR__) && defined(__WIDECHAR__)
 _WCRTLINK CHAR_TYPE *__simple_wcsrchr( const CHAR_TYPE *s, int c )
#else
 _WCRTLINK CHAR_TYPE *__F_NAME(strrchr,wcsrchr)( const CHAR_TYPE *s, int c )
#endif
    {
#if defined(M_I86) && !defined(__WIDECHAR__)
        return( _fast_strrchr( s, c ) );
#else
        CHAR_TYPE *p;
        CHAR_TYPE cc = c;

        p = NULL;       /* assume c will not be found */
        do {
            if( *s == cc ) p = (CHAR_TYPE *)s;
        } while( *s++ != NULLCHAR );
        return( p );
#endif
    }
