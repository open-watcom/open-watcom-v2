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


/* This module contains functions for doing multiply precision integer
 * arithmetic.  Allocations for these numbers increase automatically as
 * required when doing the arithmetic.  The size of a number can grow until
 * there is no more heap space.  This module is used for converting between
 * floating point numbers and ascii strings.
 */
#ifndef _MPNUM_H
#define _MPNUM_H

#ifdef __cplusplus
extern "C" {
#endif

#define NULL 0
#define FALSE 0
#define TRUE 1

#define FLOOR   1
#define CEILING 2

#define MP_NO_ERR               0
#define MP_ERR                  -1
#define MP_OUT_OF_MEMORY        -2
#define MP_DIVIDE_BY_ZERO       -3
#define MP_LOSS_OF_PRECISION    -4
#define MP_NEGATIVE_RESULT      -5
#define MP_TOO_LARGE            -6

#ifndef max
#define max( a, b )     (a > b) ? a : b
#endif
#ifndef min
#define min( a, b )     (a < b) ? a : b
#endif
#ifndef abs
#define abs( a )        a < 0 ? -(a) : a
#endif

typedef signed char             int8;
typedef signed short            int16;
typedef signed long             int32;
typedef signed __int64          int64;
typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned long           uint32;
typedef unsigned __int64        uint64;

typedef struct mpnum {
    uint32 *num;
    uint32 len;
    uint32 allocated;
} mpnum;

int mp_init( mpnum *mp, uint64 value );
int mp_copy( mpnum *dst, mpnum *src );
int mp_free( mpnum *num );
int mp_zero( mpnum *num );

int mp_cmp( mpnum *num1, mpnum *num2 );
int mp_gt( mpnum *num1, mpnum *num2 );
int mp_gte( mpnum *num1, mpnum *num2 );
int mp_lt( mpnum *num1, mpnum *num2 );
int mp_lte( mpnum *num1, mpnum *num2 );
int mp_eq( mpnum *num1, mpnum *num2 );
int mp_ne( mpnum *num1, mpnum *num2 );

int mp_reduce( mpnum *num );

int mp_shiftleft( mpnum *dst, mpnum *src, uint32 bits );
int mp_shiftright( mpnum *dst, mpnum *src, uint32 bits );

int mp_addsc( mpnum *dst, mpnum *src, uint32 scalar );
int mp_add( mpnum *dst, mpnum *src1, mpnum *src2 );
int mp_sub( mpnum *dst, mpnum *src1, mpnum *src2 );
int mp_mulsc( mpnum *dst, mpnum *src, uint32 scalar );
int mp_mul( mpnum *dst, mpnum *src1, mpnum *src2 );
int mp_divsc( mpnum *dst, mpnum *src, uint32 scalar, int mode );
int mp_div( mpnum *qdst, mpnum *rdst, mpnum *src1, mpnum *src2 );
int mp_sqr( mpnum *dst, mpnum *src );
int mp_pow( mpnum *dst, mpnum *src, uint32 exp );

int mp_binround( mpnum *dst, mpnum *src, uint64 bit );

int mp_tofloat( uint8 *dst, mpnum *src );
int mp_todouble( uint8 *dst, mpnum *src );
int mp_toextended( uint8 *dst, mpnum *src );

int mp_touint64( uint64 *dst, mpnum *src );

int mp_bitsize( mpnum *num );

#ifdef __cplusplus
};
#endif

#endif
