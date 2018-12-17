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
* Description:  Implementation of strchr() and wcschr().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stddef.h>
#include <string.h>
#include "riscstr.h"

#if defined( _M_I86 ) && !defined(__WIDECHAR__)

extern  char * _scan1();

/* use scan1 to find the char we are looking for */

#if defined(__SMALL_DATA__)
#pragma aux _scan1 = \
    "L1: lodsw"         \
        "cmp  al,dl"    \
        "je short L3"   \
        "test al,al"    \
        "je short L2"   \
        "cmp  ah,dl"    \
        "je short L4"   \
        "test ah,ah"    \
        "je short L2"   \
        "lodsw"         \
        "cmp  al,dl"    \
        "je short L3"   \
        "test al,al"    \
        "je short L2"   \
        "cmp  ah,dl"    \
        "je short L4"   \
        "test ah,ah"    \
        "jne short L1"  \
    "L2: xor  si,si"    \
        "jmp short L5"  \
    "L3: dec  si"       \
    "L4: dec  si"       \
    "L5:"               \
    __parm __caller [__si] [__dl] \
    __value         [__si] \
    __modify        [__ax __si]
#else
#pragma aux _scan1 = \
        "push ds"       \
        "mov  ds,cx"    \
        "mov  dx,ds"    \
    "L1: lodsw"         \
        "cmp  al,bl"    \
        "je short L3"   \
        "test al,al"    \
        "je short L2"   \
        "cmp  ah,bl"    \
        "je short L4"   \
        "test ah,ah"    \
        "je short L2"   \
        "lodsw"         \
        "cmp  al,bl"    \
        "je short L3"   \
        "test al,al"    \
        "je short L2"   \
        "cmp  ah,bl"    \
        "je short L4"   \
        "test ah,ah"    \
        "jne short L1"  \
    "L2: xor  si,si"    \
        "mov  dx,si"    \
        "jmp short L5"  \
    "L3: dec  si"       \
    "L4: dec  si"       \
    "L5: pop  ds"       \
    __parm __caller [__cx __si] [__bl] \
    __value         [__dx __si] \
    __modify        [__ax __dx __si]
#endif
#endif


/* locate the first occurrence of c in the initial n characters of the
   string pointed to by s. The terminating null character is considered
   to be part of the string.
   If the character c is not found, NULL is returned.
*/
#undef  strchr


#if defined(__RISCSTR__) && defined(__WIDECHAR__)
 CHAR_TYPE *__simple_wcschr( const CHAR_TYPE *s, INTCHAR_TYPE c )
#else
 _WCRTLINK CHAR_TYPE *__F_NAME(strchr,wcschr)( const CHAR_TYPE *s, INTCHAR_TYPE c )
#endif
{
    CHAR_TYPE   cc = c;
    do {
        if( *s == cc )
            return( (CHAR_TYPE *)s );
    } while( *s++ != NULLCHAR );
    return( NULL );
}
