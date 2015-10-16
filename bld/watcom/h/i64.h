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
* Description:  Support for 64-bit integers (no native 64-bit type required).
*
****************************************************************************/


#ifndef _H__I64_
#define _H__I64_

#include "watcom.h"

void I32ToI64( signed_32, signed_64 * );
void U32ToU64( unsigned_32, unsigned_64 * );

void U64Neg( const unsigned_64 *a, unsigned_64 *res );

void U64Add( const unsigned_64 *a, const unsigned_64 *b, unsigned_64 *res );
void U64Sub( const unsigned_64 *a, const unsigned_64 *b, unsigned_64 *res );
void U64Mul( const unsigned_64 *a, const unsigned_64 *b, unsigned_64 *res );
void U64Div( const unsigned_64 *a, const unsigned_64 *b,
                unsigned_64 *div, unsigned_64 *rem );
void I64Div( const signed_64 *a, const signed_64 *b,
                signed_64 *div,    signed_64 *rem );

void U64IncDec( unsigned_64 *a, signed_32 i );

int U64Cmp( const unsigned_64 *a, const unsigned_64 *b );
int I64Cmp( const signed_64 *a, const signed_64 *b );

int U64Test( const unsigned_64 *a );
int I64Test( const signed_64 *a );

void I64ShiftR( const signed_64 *a, unsigned shift, signed_64 *res );
void U64ShiftR( const unsigned_64 *a, unsigned shift, unsigned_64 *res );
void U64ShiftL( const unsigned_64 *a, unsigned shift, unsigned_64 *res );
void U64Shift( const unsigned_64 *a, int shift, unsigned_64 *res );

#define U64And( a, b, c )                                       \
        { (c)->u._32[I64LO32] = (a)->u._32[I64LO32] & (b)->u._32[I64LO32];      \
          (c)->u._32[I64HI32] = (a)->u._32[I64HI32] & (b)->u._32[I64HI32]; }
#define U64Or( a, b, c )                                        \
        { (c)->u._32[I64LO32] = (a)->u._32[I64LO32] | (b)->u._32[I64LO32];      \
          (c)->u._32[I64HI32] = (a)->u._32[I64HI32] | (b)->u._32[I64HI32]; }
#define U64Xor( a, b, c )                                       \
        { (c)->u._32[I64LO32] = (a)->u._32[I64LO32] ^ (b)->u._32[I64LO32];      \
          (c)->u._32[I64HI32] = (a)->u._32[I64HI32] ^ (b)->u._32[I64HI32]; }
#define U64Not( a, b )                          \
        { (b)->u._32[I64LO32] = ~(a)->u._32[I64LO32];   \
          (b)->u._32[I64HI32] = ~(a)->u._32[I64HI32]; }

int  U64Cnv10( unsigned_64 *res, char c );
int  U64Cnv8( unsigned_64 *res, char c );
int  U64Cnv16( unsigned_64 *res, char c );
#if defined(__386__) && defined( __WATCOMC__ )

#pragma aux U64Cnv10 = \
    "mov ecx,4[esi]" \
    "xor edx,edx" \
    "mov ebx,[esi]" \
    "shld edx,ecx,3" \
    "shld ecx,ebx,3" \
    "shl ebx,3" \
    "shl dword ptr [esi],1" \
    "rcl dword ptr 4[esi],1" \
    "rcl edx,1" \
    "add ebx,eax" \
    "adc ecx,0" \
    "adc edx,0" \
    "add [esi],ebx" \
    "adc 4[esi],ecx" \
    "adc edx,0" \
    parm caller [esi] [eax] \
    value [edx] \
    modify exact [ebx ecx edx];

#pragma aux U64Cnv8 = \
    "mov ecx,4[esi]" \
    "xor edx,edx" \
    "mov ebx,[esi]" \
    "shld edx,ecx,3" \
    "shld ecx,ebx,3" \
    "shl ebx,3" \
    "mov 4[esi],ecx" \
    "or ebx,eax" \
    "mov [esi],ebx" \
    parm caller [esi] [eax] \
    value [edx] \
    modify exact [ebx ecx edx];

#pragma aux U64Cnv16 = \
    "mov ecx,4[esi]" \
    "xor edx,edx" \
    "mov ebx,[esi]" \
    "shld edx,ecx,4" \
    "shld ecx,ebx,4" \
    "shl ebx,4" \
    "mov 4[esi],ecx" \
    "or ebx,eax" \
    "mov [esi],ebx" \
    parm caller [esi] [eax] \
    value [edx] \
    modify exact [ebx ecx edx];

#else
#define _U64_C_ROUTINES
#endif

#define U64Clear( x )   ((x).u._32[0]=0,(x).u._32[1]=0)

/* The FetchTrunc macros grab an 8/16/32-bit value from memory assuming
 * that the value is stored as a 64-bit integer. This is required for
 * big endian systems where the value is at different memory address
 * depending on its size.
 */
#define U32Fetch( x )           (assert((x).u._32[I64HI32]==0),(x).u._32[I64LO32])
#define U32FetchTrunc( x )      ((x).u._32[I64LO32])
#define I32FetchTrunc( x )      ((signed_32)(x).u._32[I64LO32])
#define U16FetchTrunc( x )      ((x).u._16[I64LO16])
#define I16FetchTrunc( x )      ((signed_16)(x).u._16[I64LO16])
#define U8FetchTrunc( x )       ((x).u._8[I64LO8])
#define I8FetchTrunc( x )       ((signed_8)(x).u._8[I64LO8])

/* Note about the FetchNative macros: These assume that the value is stored
 * in memory as a non-64bit type, starting at the lowest address. That is,
 * it must be accessed as the first entry of the appropriate array (u._16
 * array for 16-bit value etc.) *regardless* of host endianness.
 */
#define U32FetchNative( x )     ((x).u._32[0])
#define I32FetchNative( x )     ((signed_32)(x).u._32[0])
#define U16FetchNative( x )     ((x).u._16[0])
#define I16FetchNative( x )     ((signed_16)(x).u._16[0])
#define U8FetchNative( x )      ((x).u._8[0])
#define I8FetchNative( x )      ((signed_8)(x).u._8[0])

#if defined( __BIG_ENDIAN__ )
#   define I64Val( h, l ) { h, l }
#else
#   define I64Val( h, l ) { l, h }
#endif

// set U64 from low, high part
#define U64Set( x, l, h )       ((x)->u._32[I64LO32] = (l), (x)->u._32[I64HI32] = (h))

// is the U64 a valid U32?
#define U64IsU32( x )   ((x).u._32[I64HI32]==0)
// is the U64 a positive I32?
#define U64IsI32( x )   (((x).u._32[I64HI32]==0)&&((int_32)((x).u._32[I64LO32]))>=0)
// is the U64 a positive I64?
#define U64IsI64( x )   (((int_32)((x).u._32[I64HI32]))>=0)
// is the I64 a I32?
#define I64IsI32(x) (((x).u._32[I64HI32]==0)&&(((int_32)((x).u._32[I64LO32]))>=0) \
                   ||((x).u._32[I64HI32]==-1)&&(((int_32)((x).u._32[I64LO32]))<0))

#endif
