/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023-2026 The Open Watcom Contributors. All Rights Reserved.
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

#ifdef __cplusplus
extern "C" {
#endif

#define U64Byte(a,b)        ((a).u._8[(b)])
#define U64Word(a,b)        ((a).u._16[(b)])

#define U64LowByte(a)       ((a).u._8[I64B0])
#define U64LowWord(a)       ((a).u._16[I64W0])
#define U64Low(a)           ((a).u._32[I64LO32])
#define U64LowLE(a)         ((a).u._32[0])
#define U64HighByte(a)      ((a).u._8[I64B1])
#define U64HighWord(a)      ((a).u._16[I64W1])
#define U64High(a)          ((a).u._32[I64HI32])
#define U64HighLE(a)        ((a).u._32[1])

#define I64Byte(a,b)        ((signed_8)(a).u._8[(b)])
#define I64Word(a,b)        ((signed_16)(a).u._16[(b)])

#define I64LowByte(a)       ((signed_8)(a).u._8[I64B0])
#define I64LowWord(a)       ((signed_16)(a).u._16[I64W0])
#define I64Low(a)           ((signed_32)(a).u._32[I64LO32])
#define I64HighByte(a)      ((signed_32)(a).u._8[I64B1])
#define I64HighWord(a)      ((signed_32)(a).u._16[I64W1])
#define I64High(a)          ((signed_32)(a).u._32[I64HI32])

#define U64CmpU32(a,b)      (((a).u._32[I64HI32])?1:(((a).u._32[I64LO32]<(b))?-1:((a).u._32[I64LO32]!=(b))))
#define I64CmpU32(a,b)      (((a).u.sign.v)?-1:U64CmpU32((a),(b)))

#define I64isNeg(a)         ((a).u.sign.v!=0)
#define U64isZero(a)        (((a).u._32[0]|(a).u._32[1])==0)
#define U64isntZero(a)      (((a).u._32[0]|(a).u._32[1])!=0)
#define U64isEq(a,b)        (((a).u._32[0]==(b).u._32[0])&&((a).u._32[1]==(b).u._32[1]))
#define U64isntEq(a,b)      (((a).u._32[0]!=(b).u._32[0])||((a).u._32[1]!=(b).u._32[1]))

#define U64And(r,a,b)   \
        { (r).u._32[0] = (a).u._32[0] & (b).u._32[0]; \
          (r).u._32[1] = (a).u._32[1] & (b).u._32[1]; }
#define U64AndEq(r,b)   \
        { (r).u._32[0] &= (b).u._32[0]; \
          (r).u._32[1] &= (b).u._32[1]; }
#define U64Or(r,a,b)    \
        { (r).u._32[0] = (a).u._32[0] | (b).u._32[0]; \
          (r).u._32[1] = (a).u._32[1] | (b).u._32[1]; }
#define U64OrEq(r,b)    \
        { (r).u._32[0] |= (b).u._32[0]; \
          (r).u._32[1] |= (b).u._32[1]; }
#define U64Xor(r,a,b)   \
        { (r).u._32[0] = (a).u._32[0] ^ (b).u._32[0]; \
          (r).u._32[1] = (a).u._32[1] ^ (b).u._32[1]; }
#define U64XorEq(r,b)   \
        { (r).u._32[0] ^= (b).u._32[0]; \
          (r).u._32[1] ^= (b).u._32[1]; }
#define U64Not(r,a)     \
        { (r).u._32[0] = ~(a).u._32[0]; \
          (r).u._32[1] = ~(a).u._32[1]; }
#define U64NotEq(r)     \
        { (r).u._32[0] = ~(r).u._32[0]; \
          (r).u._32[1] = ~(r).u._32[1]; }

#define U64NotAnd(r,a,b) \
        { (r).u._32[0] = (a).u._32[0] & ~(b).u._32[0]; \
          (r).u._32[0] = (a).u._32[1] & ~(b).u._32[1]; }
#define U64NotAndEq(r,b) \
        { (r).u._32[0] &= ~(b).u._32[0]; \
          (r).u._32[1] &= ~(b).u._32[1]; }

#define U64Inc(r) if( ++(r).u._32[I64LO32] == 0 ) ++(r).u._32[I64HI32]
#define U64Dec(r) if( (r).u._32[I64LO32]-- == 0 ) --(r).u._32[I64HI32]

/* The FetchTrunc macros grab an 8/16/32-bit value from memory assuming
 * that the value is stored as a 64-bit integer. This is required for
 * big endian systems where the value is at different memory address
 * depending on its size.
 */
#define U32Fetch(x)         (assert((x).u._32[I64HI32]==0),(x).u._32[I64LO32])
#define U32FetchTrunc(x)    ((x).u._32[I64LO32])
#define I32FetchTrunc(x)    ((signed_32)(x).u._32[I64LO32])
#define U16FetchTrunc(x)    ((x).u._16[I64LO16])
#define I16FetchTrunc(x)    ((signed_16)(x).u._16[I64LO16])
#define U8FetchTrunc(x)     ((x).u._8[I64LO8])
#define I8FetchTrunc(x)     ((signed_8)(x).u._8[I64LO8])

#define U64ConvU32(x)       ((x).u._32[I64HI32]=0)
#define U64ConvI32(x)       ((x).u._32[I64HI32]=((signed_32)(x).u._32[I64LO32]<0)?-1:0)
#define U64ConvU16(x)       ((x).u._16[I64W1]=(x).u._32[I64HI32]=0)
#define U64ConvI16(x)       ((x).u._16[I64W1]=(x).u._32[I64HI32]=((signed_16)(x).u._16[I64W0]<0)?-1:0)
#define U64ConvU8(x)        ((x).u._8[I64B1]=(x).u._16[I64W1]=(x).u._32[I64HI32]=0)
#define U64ConvI8(x)        ((x).u._8[I64B1]=(x).u._16[I64W1]=(x).u._32[I64HI32]=((signed_8)(x).u._8[I64B0]<0)?-1:0)

/* Note about the FetchNative macros: These assume that the value is stored
 * in memory as a non-64bit type, starting at the lowest address. That is,
 * it must be accessed as the first entry of the appropriate array (u._16
 * array for 16-bit value etc.) *regardless* of host endianness.
 */
#define U32FetchNative(x)   ((x).u._32[0])
#define I32FetchNative(x)   ((signed_32)(x).u._32[0])
#define U16FetchNative(x)   ((x).u._16[0])
#define I16FetchNative(x)   ((signed_16)(x).u._16[0])
#define U8FetchNative(x)    ((x).u._8[0])
#define I8FetchNative(x)    ((signed_8)(x).u._8[0])

#if defined( __BIG_ENDIAN__ )
    #define Init64Val(h,l)  {(h),(l)}
#else
    #define Init64Val(h,l)  {(l),(h)}
#endif
// set 64-bit from low and high part
#define Set64Val(x,l,h)     ((x).u._32[I64LO32]=(l),(x).u._32[I64HI32]=(h))
#define Set64ValZero(x)     ((x).u._32[0]=0,(x).u._32[1]=0)
#define Set64Val1p(x)       ((x).u._32[I64LO32]=1,(x).u._32[I64HI32]=0)
#define Set64Val1m(x)       ((x).u._32[I64LO32]=(unsigned_32)-1,(x).u._32[I64HI32]=(unsigned_32)-1)
#define Set64ValI32(x,v)    ((x).u._32[I64LO32]=(v),(x).u._32[I64HI32]=((signed_32)(x).u._32[I64LO32]<0)?(unsigned_32)-1:0)
#define Set64ValU32(x,v)    ((x).u._32[I64LO32]=(v),(x).u._32[I64HI32]=0)

// is the U64 a valid U32?
#define U64IsU32(x)         ((x).u._32[I64HI32]==0)
// is the U64 a positive I32?
#define U64IsI32(x)         (((x).u._32[I64HI32]==0)&&((signed_32)(x).u._32[I64LO32]>=0))
// is the U64 a positive I64?
#define U64IsI64(x)         ((signed_32)(x).u._32[I64HI32]>=0)
// is the I64 a I32?
#define I64IsI32(x)         (((x).u._32[I64HI32]==0)&&((signed_32)(x).u._32[I64LO32]>=0) \
                            ||((x).u._32[I64HI32]==-1)&&((signed_32)(x).u._32[I64LO32]<0))

#define U64NegEq(a)         U64Neg((a),(a))
#define U64AddEq(a,b)       U64Add((a),(a),(b))
#define U64SubEq(a,b)       U64Sub((a),(a),(b))
#define U64MulEq(a,b)       U64Mul((a),(a),(b))

#define I64ShiftREq(a,b)    I64ShiftR((a),(a),(b))
#define U64ShiftREq(a,b)    U64ShiftR((a),(a),(b))
#define U64ShiftLEq(a,b)    U64ShiftL((a),(a),(b))
#define U64ShiftEq(a,b)     U64Shift((a),(a),(b))

extern int      U64Cnv10( unsigned_64 *res, char c );
extern int      U64Cnv8( unsigned_64 *res, char c );
extern int      U64Cnv2( unsigned_64 *res, char c );
extern int      U64Cnv16( unsigned_64 *res, char c );
#if defined(__386__) && defined( __WATCOMC__ )
#pragma aux U64Cnv10 \
    __parm __caller [__esi] [__eax] = \
        "mov  ecx,4[esi]" \
        "xor  edx,edx" \
        "mov  ebx,[esi]" \
        "shld edx,ecx,3" \
        "shld ecx,ebx,3" \
        "shl  ebx,3" \
        "shl  dword ptr [esi],1" \
        "rcl  dword ptr 4[esi],1" \
        "rcl  edx,1" \
        "add  ebx,eax" \
        "adc  ecx,0" \
        "adc  edx,0" \
        "add  [esi],ebx" \
        "adc  4[esi],ecx" \
        "adc  edx,0" \
    __value [__edx] \
    __modify __exact [__ebx __ecx __edx]

#pragma aux U64Cnv8 \
    __parm __caller [__esi] [__eax] = \
        "mov  ecx,4[esi]" \
        "xor  edx,edx" \
        "mov  ebx,[esi]" \
        "shld edx,ecx,3" \
        "shld ecx,ebx,3" \
        "shl  ebx,3" \
        "mov  4[esi],ecx" \
        "or   ebx,eax" \
        "mov  [esi],ebx" \
    __value [__edx] \
    __modify __exact [__ebx __ecx __edx]

#pragma aux U64Cnv2 \
    __parm __caller [__esi] [__eax] = \
        "mov  ecx,4[esi]" \
        "xor  edx,edx" \
        "mov  ebx,[esi]" \
        "shld edx,ecx,1" \
        "shld ecx,ebx,1" \
        "shl  ebx,1" \
        "mov  4[esi],ecx" \
        "or   ebx,eax" \
        "mov  [esi],ebx" \
    __value [__edx] \
    __modify __exact [__ebx __ecx __edx]

#pragma aux U64Cnv16 \
    __parm __caller [__esi] [__eax] = \
        "mov  ecx,4[esi]" \
        "xor  edx,edx" \
        "mov  ebx,[esi]" \
        "shld edx,ecx,4" \
        "shld ecx,ebx,4" \
        "shl  ebx,4" \
        "mov  4[esi],ecx" \
        "or   ebx,eax" \
        "mov  [esi],ebx" \
    __value [__edx] \
    __modify __exact [__ebx __ecx __edx]

#else
#define _U64_C_ROUTINES
#endif

extern void     U64Neg( unsigned_64 *res, const unsigned_64 *a );
extern void     U64Add( unsigned_64 *res, const unsigned_64 *a, const unsigned_64 *b );
extern void     U64Sub( unsigned_64 *res, const unsigned_64 *a, const unsigned_64 *b );
extern void     U64Mul( unsigned_64 *res, const unsigned_64 *a, const unsigned_64 *b );
extern void     U64Div( const unsigned_64 *a, const unsigned_64 *b, unsigned_64 *div, unsigned_64 *rem );
extern void     I64Div( const signed_64 *a, const signed_64 *b, signed_64 *div, signed_64 *rem );
extern void     U64AddI32( unsigned_64 *res, signed_32 i );
extern int      U64Cmp( const unsigned_64 *a, const unsigned_64 *b );
extern int      I64Cmp( const signed_64 *a, const signed_64 *b );
extern void     I64ShiftR( signed_64 *res, const signed_64 *a, unsigned shift );
extern void     U64ShiftR( unsigned_64 *res, const unsigned_64 *a, unsigned shift );
extern void     U64ShiftL( unsigned_64 *res, const unsigned_64 *a, unsigned shift );
extern void     U64Shift( unsigned_64 *res, const unsigned_64 *a, int shift );

#ifdef __cplusplus
}
#endif

#endif
