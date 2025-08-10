/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Generic utility functions for cg.
*
****************************************************************************/


#if defined(__WATCOMC__) && defined( _M_IX86 )

#ifdef __FLAT__
#define _CGESREG
#define _CGESPTR
#else
#define _CGESREG        __es
#define _CGESPTR        __far
#endif

extern void *Copy( const void *src, void _CGESPTR *dst, size_t len );
#pragma aux Copy = \
        "mov eax,edi" \
        "rep movsb" \
    __parm __routine    [__esi] [_CGESREG __edi] [__ecx] \
    __value             [__eax]

extern void *Fill( void _CGESPTR *dst, size_t len, int byte );
#pragma aux Fill = \
        "rep stosb" \
    __parm __routine    [_CGESREG __edi] [__ecx] [__al] \
    __value             [__edi]

extern bool     Equal( const void *s1, const void _CGESPTR *s2, size_t len );
#pragma aux Equal = \
        "xor    eax,eax" \
        "repe cmpsb" \
        "jne short L1" \
        "inc    eax" \
    "L1:" \
    __parm __routine    [__esi] [_CGESREG __edi] [__ecx] \
    __value             [__al]

extern size_t   Length( const char _CGESPTR *);
#pragma aux Length = \
        "xor    eax,eax" \
        "xor    ecx,ecx" \
        "dec    ecx" \
        "repne scasb" \
        "not    ecx" \
        "dec    ecx" \
    __parm __routine    [_CGESREG __edi] \
    __value             [__ecx] \
    __modify            [__eax]

extern char *CopyStr( const char *src, char _CGESPTR *dst );
#pragma aux CopyStr = \
        "push   edi" \
        "xor    eax,eax" \
    "L1: lodsb" \
        "stosb" \
        "or     eax,eax" \
        "jnz short L1" \
        "pop    eax" \
    __parm __routine    [__esi] [_CGESREG __edi] \
    __value             [__eax]

extern char *CopyStrEnd( const char *src, char _CGESPTR *dst );
#pragma aux CopyStrEnd = \
        "xor    eax,eax" \
    "L1: lodsb" \
        "stosb" \
        "or     eax,eax" \
        "jnz short L1" \
        "dec    edi" \
        "mov    eax,edi" \
    __parm __routine    [__esi] [_CGESREG __edi] \
    __value             [__eax]

#else

#define Copy(s,d,l)     memcpy((d), (s), (l))
#define Fill(d,l,c)     memset((d), (c), (l))
#define Equal(s1,s2,l)  (memcmp((s1), (s2), (l)) == 0)
#define CopyStr(s,d)    strcpy((d), (s))
#define CopyStrEnd(s,d) strcpy((d), (s)) + strlen((d))
#define Length(s)       strlen((s))

#endif

extern uint_32  CountBits( uint_32 value );
