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


/* This module contains floating point functions that conform to the Java 1.3
 * standard.  This standard is close to the IEEE754 standard but there are
 * some differences.
 */
#ifndef _FP_H
#define _FP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mp.h"

#define float_bias              127
#define double_bias             1023
#define extended_bias           16383

#define float_precision         24
#define double_precision        53
#define extended_precision      64

#define VALUE_TOO_SMALL         -10
#define VALUE_TOO_LARGE         -11

#define FLOAT_POS_ZERO          (0x00000000)
#define FLOAT_NEG_ZERO          (0x80000000)
#define FLOAT_POS_INF           (0x7f800000)
#define FLOAT_NEG_INF           (0xff800000)
#define FLOAT_NAN               (0xffc00000)

#define DOUBLE_POS_ZERO         (0x0000000000000000L)
#define DOUBLE_NEG_ZERO         (0x8000000000000000L)
#define DOUBLE_POS_INF          (0x7ff0000000000000L)
#define DOUBLE_NEG_INF          (0xfff0000000000000L)
#define DOUBLE_NAN              (0xfff8000000000000L)

/* boolean functions */
int f_isNan( void* );
int d_isNan( void* );
int e_isNan( void* );
int f_isInf( void* );
int d_isInf( void* );
int e_isInf( void* );
int f_isPosInf( void* );
int d_isPosInf( void* );
int e_isPosInf( void* );
int f_isNegInf( void* );
int e_isNegInf( void* );
int d_isNegInf( void* );
int f_isZero( void* );
int d_isZero( void* );
int e_isZero( void* );
int f_isPosZero( void* );
int d_isPosZero( void* );
int e_isPosZero( void* );
int f_isNegZero( void* );
int d_isNegZero( void* );
int e_isNegZero( void* );
int f_isNeg( void* );
int d_isNeg( void* );
int e_isNeg( void* );
int f_isSpecialValue( void* );
int d_isSpecialValue( void* );
int e_isSpecialValue( void* );

/* comparison functions */
int fcmp( void*, void* );
int dcmp( void*, void* );
int f_isEqual( void*, void* );
int d_isEqual( void*, void* );
int f_isNotEqual( void*, void* );
int d_isNotEqual( void*, void* );
int f_isGreater( void*, void* );
int d_isGreater( void*, void* );
int f_isGreaterEqual( void*, void* );
int d_isGreaterEqual( void*, void* );
int f_isLess( void*, void* );
int d_isLess( void*, void* );
int f_isLessEqual( void*, void* );
int d_isLessEqual( void*, void* );

/* arithmetic functions */
void fadd( void*, void*, void* );
void fsub( void*, void*, void* );
void fmul( void*, void*, void* );
void fdiv( void*, void*, void* );
void frem( void*, void*, void* );       /* arg1 = arg2 % arg3 */
void fneg( void*, void* );

void dadd( void*, void*, void* );
void dsub( void*, void*, void* );
void dmul( void*, void*, void* );
void ddiv( void*, void*, void* );
void drem( void*, void*, void* );
void dneg( void*, void* );

void eadd( void*, void*, void* );
void esub( void*, void*, void* );
void emul( void*, void*, void* );
void ediv( void*, void*, void* );
void erem( void*, void*, void* );
void eneg( void*, void* );

/* conversion between floating point types */
void f2d( void*, void* );
void d2f( void*, void* );
void e2d( void*, void* );
void d2e( void*, void* );
void e2f( void*, void* );
void f2e( void*, void* );

/* conversion to/from integer types */
void f2i( void*, void* );
void i2f( void*, void* );
void f2l( void*, void* );
void l2f( void*, void* );
void d2i( void*, void* );
void i2d( void*, void* );
void d2l( void*, void* );
void l2d( void*, void* );
void e2i( void*, void* );
void i2e( void*, void* );
void e2l( void*, void* );
void l2e( void*, void* );

/* convert ascii string to floating-point type */
int a2f( float *flt, char *ascii );
int a2d( double *dbl, char *ascii );
int a2e( char *ext, char *ascii );

/* convert floating-point type to ascii string */
int f2a( char *ascii, float *flt, int maxlen );
int d2a( char *ascii, double *dbl, int maxlen );
int e2a( char *ascii, char *ext, int maxlen );

/* break up floating-point into mantissa and exponent */
void parseFloat( float flt, uint64 *f, int *e );
void parseDouble( double db, uint64 *f, int *e );
void parseExtended( char *ext, uint64 *f, int *e );

/* construct floating-point from mantissa and exponent */
void makeFloat( float *dst, uint64 f, int e );
void makeDouble( double *dst, uint64 f, int e );
void makeExtended( char *dst, uint64 f, int e );

/* change just the exponent part of a number */
void changeFloatExponent( float *dst, float *src, int diff );
void changeDoubleExponent( double *dst, double *src, int diff );
void changeExtendedExponent( char *dst, char *src, int diff );

/* create special extended numbers */
void makeExtNan( char *num );
void makeExtPosInf( char *num );
void makeExtNegInf( char *num );
void makeExtPosZero( char *num );
void makeExtNegZero( char *num );

#ifdef __cplusplus
};
#endif

#endif
