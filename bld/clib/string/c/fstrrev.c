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
* Description:  Implementation of _fstrrev() - far strrev().
*
****************************************************************************/


#include "variety.h"
#include <ctype.h>
#include <string.h>


#ifdef _M_I86

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

extern void fast_rev( char _WCFAR * );
#pragma aux fast_rev = \
        "push ds"       \
        "mov  es,cx"    \
        "mov  ds,cx"    \
        "mov  si,di"    \
        "mov  cx,-1"    \
        "xor  al,al"    \
        "repne scasb"   \
        "not  cx"       \
        "dec  cx"       \
        "std"           \
        "sub  di,3"     \
        "shr  cx,1"     \
        "shr  cx,1"     \
        "jcxz short L2" \
    "L1: mov  ax,[di]"  \
        "xchg ah,al"    \
        "xchg ax,[si]"  \
        "xchg ah,al"    \
        "stosw"         \
        "inc  si"       \
        "inc  si"       \
        "loop short L1" \
    "L2: jnc short L3"  \
        "inc  di"       \
        "mov  al,[di]"  \
        "xchg al,[si]"  \
        "mov  [di],al"  \
    "L3: cld"           \
        "pop  ds"       \
    __parm __caller [__cx __di] \
    __value         \
    __modify        [__si __cx __ax __di __es]

#endif

_WCRTLINK char _WCFAR *_fstrrev( char _WCFAR *str )  /* reverse characters in string */
{
#if  defined( _M_I86 )
    fast_rev( str );
    return( str );
#else
    char _WCFAR     *p1;
    char _WCFAR     *p2;
    char            c1;
    char            c2;

    p1 = str;
    p2 = p1 + _fstrlen( p1 ) - 1;
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
