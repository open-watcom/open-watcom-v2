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
* Description:  Implementation of strrev() and _wcsrev().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <ctype.h>
#include <string.h>

#if defined(__WIDECHAR__)
extern size_t wcslen( const CHAR_TYPE * );
#endif

#if defined( _M_I86 ) && !defined(__WIDECHAR__)

/*
  explanation of algorithm:

  (1) reverse as much of the string as possible as words

  (2) after main loop: reverse residual inner string (0-3 bytes)

  the string falls into one of these forms:

 { prefix_word }* { suffix_word }*
 { prefix_word }* middle_byte { suffix_word }*
 { prefix_word }* pre_middle_byte post_middle_byte { suffix_word }*
 { prefix_word }* pre_middle_byte middle_byte post_middle_byte { suffix_word }*

 we only have to swap two bytes when:

 len & 2 != 0 is true (ie. the carry is set after second shr cx,1)

*****************************************************************************
  WARNING: the code in the L1: ... reverse loop cannot modify the carry flag
*****************************************************************************
*/

extern void fast_rev( char *, char _WCFAR * );

#if defined(__SMALL_DATA__)
#pragma aux     fast_rev = \
        0xb9 0xff 0xff  /* mov cx,ffff */\
        0x30 0xc0       /* xor al,al */\
        0xf2 0xae       /* repne scasb */\
        0xf7 0xd1       /* not cx */\
        0x49            /* dec cx */\
        0xfd            /* std */\
        0x83 0xef 0x03  /* sub di,3 */\
        0xd1 0xe9       /* shr cx,1 */\
        0xd1 0xe9       /* shr cx,1 */\
        0xe3 0x0d       /* jcxz L2 */\
        0x8b 0x05       /* L1:mov ax,[di] */\
        0x86 0xe0       /* xchg ah,al */\
        0x87 0x04       /* xchg ax,[si] */\
        0x86 0xe0       /* xchg ah,al */\
        0xab            /* stosw */\
        0x46            /* inc si */\
        0x46            /* inc si */\
        0xe2 0xf3       /* loop L1 */\
        0x73 0x07       /* L2:jnc L3 */\
        0x47            /* inc di */\
        0x8a 0x05       /* mov al,[di] */\
        0x86 0x04       /* xchg al,[si] */\
        0x88 0x05       /* mov [di],al */\
        0xfc            /* L3:cld */\
        parm caller     [si] [es di] \
        value           [ax] \
        modify          [si cx ax di];
#else
#pragma aux     fast_rev = \
        0x1e            /* push ds */ \
        0x8e 0xda       /* mov ds,dx */ \
        0xb9 0xff 0xff  /* mov cx,ffff */\
        0x30 0xc0       /* xor al,al */\
        0xf2 0xae       /* repne scasb */\
        0xf7 0xd1       /* not cx */\
        0x49            /* dec cx */\
        0xfd            /* std */\
        0x83 0xef 0x03  /* sub di,3 */\
        0xd1 0xe9       /* shr cx,1 */\
        0xd1 0xe9       /* shr cx,1 */\
        0xe3 0x0d       /* jcxz L2 */\
        0x8b 0x05       /* L1:mov ax,[di] */\
        0x86 0xe0       /* xchg ah,al */\
        0x87 0x04       /* xchg ax,[si] */\
        0x86 0xe0       /* xchg ah,al */\
        0xab            /* stosw */\
        0x46            /* inc si */\
        0x46            /* inc si */\
        0xe2 0xf3       /* loop L1 */\
        0x73 0x07       /* L2:jnc L3 */\
        0x47            /* inc di */\
        0x8a 0x05       /* mov al,[di] */\
        0x86 0x04       /* xchg al,[si] */\
        0x88 0x05       /* mov [di],al */\
        0xfc            /* L3:cld */\
        0x1f            /* pop ds */ \
        parm caller     [dx si] [es di] \
        value           [ax] \
        modify          [si cx ax di];
#endif

#endif


_WCRTLINK CHAR_TYPE *__F_NAME(strrev,_wcsrev)( CHAR_TYPE *str ) /* reverse characters in string */
{
#if  defined( _M_I86 ) && !defined(__WIDECHAR__)
    fast_rev( str, (char _WCFAR *) str );
    return( str );
#else
    CHAR_TYPE       *p1;
    CHAR_TYPE       *p2;
    CHAR_TYPE       c1;
    CHAR_TYPE       c2;

    p1 = str;
    p2 = p1 + __F_NAME(strlen,wcslen)( p1 ) - 1;
    while( p1 < p2 ) {
        c1 = *p1;
        c2 = *p2;
        *p1 = c2;
        *p2 = c1;
        ++p1;
        --p2;
    }
    return( str );
#endif
}
