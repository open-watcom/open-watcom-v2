/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#if defined(__WATCOMC__) && defined( _M_IX86 )

#if defined(__SMALL__)
    #define _SAVE_ES    "push    es"
    #define _REST_ES    "pop     es"
    #define _SET_ES     "push    ds" \
                        "pop     es"
#else
    #define _SAVE_ES
    #define _REST_ES
    #define _SET_ES
#endif

extern void     *CypCopy( const void *src, void *dst, size_t len );
#pragma aux CypCopy = \
        _SAVE_ES \
        _SET_ES \
        "rep movsb" \
        _REST_ES \
    __parm __routine    [__esi] [__edi] [__ecx] \
    __value             [__edi]

extern void     *CypFill( void *start, size_t len, unsigned char byte );
#pragma aux CypFill = \
        _SAVE_ES \
        _SET_ES \
        "rep stosb" \
        _REST_ES \
    __parm __routine    [__edi] [__ecx] [__al] \
    __value             [__edi]

extern size_t   CypLength( const char *string );
#pragma aux CypLength = \
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

extern bool     CypEqual( const void *src1, const void *src2, size_t len );
#pragma aux CypEqual = \
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

#endif
