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


#ifndef _I64_H_INCLUDED
#define _I64_H_INCLUDED

#include "variety.h"
#include "widechar.h"
#include "watcom.h"

#if defined(__AXP__)
    #define USE_INT64
#endif

#if defined(USE_INT64)
    #define INT64_TYPE __int64
    #define UINT64_TYPE unsigned __int64

    #define _clib_I64Negative( a )      (((__int64)a) < 0)
    #define _clib_I64Positive( a )      (((__int64)a) > 0)
    #define _clib_I32ToI64( a, b )      b = (__int64)a
    #define _clib_U32ToU64( a, b )      b = (unsigned __int64)(unsigned_32)a
    #define _clib_I64ToInt( a, b )      b = (signed)a
    #define _clib_U64ToUInt( a, b )     b = (unsigned)a
    #define _clib_I64ToLong( a, b )     b = (signed long)a
    #define _clib_U64ToULong( a, b )    b = (unsigned long)a
    #define _clib_I64Neg( a, b )        b = -a
    #define _clib_U64Neg( a, b )        _clib_I64Neg( a, b )
    #define _clib_U64Zero( a )          (a == 0)
    #define _clib_U64Cmp( a, b )        (a > b ? +1 : a < b ? -1 : 0)

    #define _clib_U64Mul( a, b, c )     c = a * b
    #define _clib_U64Div( a, b, c, d )  (d = a % b, c = a / b)
    #define _clib_U64Add( a, b, c )     c = a + b
#else
    #define INT64_TYPE signed_64
    #define UINT64_TYPE unsigned_64

    #define _clib_I64Negative( a )      (a.u.sign.v)
    #define _clib_I64Positive( a )      ( !_clib_I64Negative(a) && !_clib_U64Zero(a) )
    #define _clib_I32ToI64( a, b )      (b.u._32[I64LO32] = a, b.u._32[I64HI32] = (signed long)a<0 ? -1 : 0)
    #define _clib_U32ToU64( a, b )      (b.u._32[I64LO32] = a, b.u._32[I64HI32] = 0)
    #define _clib_I64ToInt( a, b )      b = (signed)a.u._32[I64LO32]
    #define _clib_U64ToUInt( a, b )     b = (unsigned)a.u._32[I64LO32]
    #define _clib_I64ToLong( a, b )     b = (signed long)a.u._32[I64LO32]
    #define _clib_U64ToULong( a, b )    b = (unsigned long)a.u._32[I64LO32]
    #define _clib_I64Neg( a, b )        (b.u._32[I64LO32] = ~a.u._32[I64LO32], b.u._32[I64HI32] = ~a.u._32[I64HI32], b.u._32[I64HI32] = ( ++b.u._32[I64LO32] == 0 ? b.u._32[I64HI32]+1 : b.u._32[I64HI32]))
    #define _clib_U64Neg( a, b )        _clib_I64Neg( a, b )
    #define _clib_U64Zero( a )          (a.u._32[I64LO32] == 0 && a.u._32[I64HI32] == 0)
    #define _clib_U64Cmp( a, b )        (a.u._32[I64HI32] > b.u._32[I64HI32] ? +1 : a.u._32[I64HI32] < b.u._32[I64HI32] ? -1 : a.u._32[I64LO32] > b.u._32[I64LO32] ? +1 : a.u._32[I64LO32] < b.u._32[I64LO32] ? -1 : 0)

    #if defined(__386__)
        extern void __U8M( void );
        #pragma aux __U8M "*";
        extern void _clib_U8M( void *, void *, void * );
        #pragma aux _clib_U8M = \
                "mov edx,4[eax]" \
                "mov eax,[eax]" \
                "mov ecx,4[ebx]" \
                "mov ebx,[ebx]" \
                "call __U8M" \
                "mov 4[esi],edx" \
                "mov [esi],eax" \
                parm [eax] [ebx] [esi] modify [edx ecx];
        extern void __U8D( void );
        #pragma aux __U8D "*";
        extern void _clib_U8D( void *, void *, void *, void * );
        #pragma aux _clib_U8D = \
                "mov edx,4[eax]" \
                "mov eax,[eax]" \
                "mov ecx,4[ebx]" \
                "mov ebx,[ebx]" \
                "call __U8D" \
                "mov 4[esi],edx" \
                "mov [esi],eax" \
                "mov 4[edi],ecx" \
                "mov [edi],ebx" \
                parm [eax] [ebx] [esi] [edi] modify [edx ecx];
        extern void _clib_U8A( void *, void *, void * );
        #pragma aux _clib_U8A = \
                "mov edx,4[eax]" \
                "mov eax,[eax]" \
                "mov ecx,4[ebx]" \
                "mov ebx,[ebx]" \
                "add eax, ebx" \
                "adc edx, ecx" \
                "mov 4[esi],edx" \
                "mov [esi],eax" \
                parm [eax] [ebx] [esi] modify [edx ecx];
        #define _clib_U64Mul( a, b, c )     _clib_U8M( &a, &b, &c )
        #define _clib_U64Div( a, b, c, d )  _clib_U8D( &a, &b, &c, &d )
        #define _clib_U64Add( a, b, c )     _clib_U8A( &a, &b, &c )
    #else
        #define _clib_U64Shift( a, b, c )   __U64Shift( &a, b, &c )
        #define _clib_U64Mul( a, b, c )     __U64Mul( &a, &b, &c )
        #define _clib_U64Div( a, b, c, d )  __U64Div( &a, &b, &c, &d )
        #define _clib_U64Add( a, b, c )     __U64Add( &a, &b, &c )
        _WCRTLINK void __U64Shift( const UINT64_TYPE *a, int shift,
                                UINT64_TYPE *res );
        _WCRTLINK void __U64Mul( const UINT64_TYPE *a, const UINT64_TYPE *b,
                                UINT64_TYPE *res );
        _WCRTLINK void __U64Div( const UINT64_TYPE *a, const UINT64_TYPE *b,
                                UINT64_TYPE *div, UINT64_TYPE *rem );
        _WCRTLINK void __U64Add( const UINT64_TYPE *a, const UINT64_TYPE *b,
                                UINT64_TYPE *res );
    #endif
#endif

// conversion functions
extern _WCRTLINK CHAR_TYPE *__F_NAME(__clib_ulltoa,__clib_wulltoa)( UINT64_TYPE *, CHAR_TYPE *, unsigned );
extern _WCRTLINK void __F_NAME(__clib_atoll,__clib_watoll)( const CHAR_TYPE *, UINT64_TYPE * );

#endif
