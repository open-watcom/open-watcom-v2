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
* Description:  Implementation of strcpy() and wcscpy().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <string.h>
#include "riscstr.h"

#undef  strcpy

extern CHAR_TYPE *__strcpy( CHAR_TYPE *dst, const CHAR_TYPE *src );
#if defined(__386__)
 #if defined(__SMALL_DATA__)
  #pragma aux __strcpy = \
        "       push    eax"      \
        "L1:    mov     cl,[edx]" \
        "       mov     [eax],cl" \
        "       cmp     cl,0"     \
        "       je      L2"       \
        "       mov     cl,1[edx]"\
        "       add     edx,2"    \
        "       mov     1[eax],cl"\
        "       add     eax,2"    \
        "       cmp     cl,0"     \
        "       jne     L1"       \
        "L2:    pop     eax"      \
        parm [eax] [edx] value [eax] modify exact [eax edx ecx];
 #else  // compact or large
  #pragma aux __strcpy = \
        "       push    ds"       \
        "       push    edi"       \
        "       mov     ds,edx"    \
        "       test    esi,1"     \
        "       je      L1"       \
        "       lodsb"            \
        "       stosb"            \
        "       cmp     al,0"     \
        "       je      L3"       \
        "L1:    mov     ax,[esi]"  \
        "       test    al,al"    \
        "       je      L2"       \
        "       stosw"            \
        "       test    ah,ah"    \
        "       je      L3"       \
        "       mov     ax,2[esi]" \
        "       test    al,al"    \
        "       je      L2"       \
        "       stosw"            \
        "       add     esi,4"     \
        "       test    ah,ah"    \
        "       jne     L1"       \
        "       je      L3"       \
        "L2:    stosb"            \
        "L3:    pop     eax"       \
        "       mov     edx,es"    \
        "       pop     ds"       \
        parm [es edi] [edx esi] value [dx eax] modify exact [esi edi];
 #endif
#elif defined( _M_I86 )
 #if defined(__SMALL_DATA__)
  #pragma aux __strcpy = \
        "       push    di"       \
        "       test    si,1"     \
        "       je      L1"       \
        "       lodsb"            \
        "       mov     [di],al"  \
        "       inc     di"       \
        "       cmp     al,0"     \
        "       je      L3"       \
        "L1:    mov     ax,[si]"  \
        "       test    al,al"    \
        "       je      L2"       \
        "       mov     [di],ax"  \
        "       add     di,2"     \
        "       test    ah,ah"    \
        "       je      L3"       \
        "       mov     ax,2[si]" \
        "       test    al,al"    \
        "       je      L2"       \
        "       mov     [di],ax"  \
        "       add     si,4"     \
        "       add     di,2"     \
        "       test    ah,ah"    \
        "       jne     L1"       \
        "       je      L3"       \
        "L2:    mov     [di],al"  \
        "L3:    pop     ax"       \
        parm [di] [si] value [ax] modify exact [si di];
 #else  // compact, large, or huge
  #pragma aux __strcpy = \
        "       push    ds"       \
        "       push    di"       \
        "       mov     ds,dx"    \
        "       test    si,1"     \
        "       je      L1"       \
        "       lodsb"            \
        "       stosb"            \
        "       cmp     al,0"     \
        "       je      L3"       \
        "L1:    mov     ax,[si]"  \
        "       test    al,al"    \
        "       je      L2"       \
        "       stosw"            \
        "       test    ah,ah"    \
        "       je      L3"       \
        "       mov     ax,2[si]" \
        "       test    al,al"    \
        "       je      L2"       \
        "       stosw"            \
        "       add     si,4"     \
        "       test    ah,ah"    \
        "       jne     L1"       \
        "       je      L3"       \
        "L2:    stosb"            \
        "L3:    pop     ax"       \
        "       mov     dx,es"    \
        "       pop     ds"       \
        parm [es di] [dx si] value [dx ax] modify exact [si di];
 #endif
#else
 /* currently no pragma for non-x86 */
#endif


/* copy string t to string s */

#if defined(__RISCSTR__) && defined(__WIDECHAR__)
 _WCRTLINK CHAR_TYPE *__simple_wcscpy( CHAR_TYPE *s, const CHAR_TYPE *t )
#else
 _WCRTLINK CHAR_TYPE *__F_NAME(strcpy,wcscpy)( CHAR_TYPE *s, const CHAR_TYPE *t )
#endif
{
#if !defined(__WIDECHAR__) && defined(_M_IX86)
    return( __strcpy( s, t ) );
#else
    CHAR_TYPE *dst;

    dst = s;
    while( *dst++ = *t++ )
        ;
    return( s );
#endif
}
