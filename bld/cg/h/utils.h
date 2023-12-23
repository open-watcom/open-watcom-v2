/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2023 The Open Watcom Contributors. All Rights Reserved.
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
#define _SAVE_ES
#define _SET_ES
#define _REST_ES
#else
#define _SAVE_ES    "push    es"
#define _SET_ES     "push    ds" \
                    "pop     es"
#define _REST_ES    "pop     es"
#endif

extern void *Copy( const void *src, void *dst, size_t len );
#pragma aux Copy = \
        _SAVE_ES \
        _SET_ES \
        "mov eax,edi" \
        "rep movsb" \
        _REST_ES \
    __parm __routine    [__esi] [__edi] [__ecx] \
    __value             [__eax]

extern void *Fill( void *dst, size_t len, int byte );
#pragma aux Fill = \
        _SAVE_ES \
        _SET_ES \
        "rep stosb" \
        _REST_ES \
    __parm __routine    [__edi] [__ecx] [__al] \
    __value             [__edi]

extern bool     Equal( const void *s1, const void *s2, size_t len );
#pragma aux Equal = \
        _SAVE_ES \
        _SET_ES \
        "xor    eax,eax" \
        "repe cmpsb" \
        "jne short L1" \
        "inc    eax" \
    "L1:" \
        _REST_ES \
    __parm __routine    [__esi] [__edi] [__ecx] \
    __value             [__al]

extern size_t   Length( const char *);
#pragma aux Length = \
        _SAVE_ES \
        _SET_ES \
        "xor    eax,eax" \
        "xor    ecx,ecx" \
        "dec    ecx" \
        "repne scasb" \
        "not    ecx" \
        "dec    ecx" \
        _REST_ES \
    __parm __routine    [__edi] \
    __value             [__ecx] \
    __modify            [__eax]

extern char *CopyStr( const char *src, char *dst );
#pragma aux CopyStr = \
        _SAVE_ES \
        _SET_ES \
        "push   edi" \
        "xor    eax,eax" \
    "L1: lodsb" \
        "stosb" \
        "or     eax,eax" \
        "jnz short L1" \
        "pop    eax" \
        _REST_ES \
    __parm __routine    [__esi] [__edi] \
    __value             [__eax]

extern char *CopyStrEnd( const char *src, char *dst );
#pragma aux CopyStrEnd = \
        _SAVE_ES \
        _SET_ES \
        "xor    eax,eax" \
    "L1: lodsb" \
        "stosb" \
        "or     eax,eax" \
        "jnz short L1" \
        "dec    edi" \
        "mov    eax,edi" \
        _REST_ES \
    __parm __routine    [__esi] [__edi] \
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
