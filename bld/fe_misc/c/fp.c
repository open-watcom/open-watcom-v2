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
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "mp.h"
#include "fp.h"

#define FALSE 0
#define TRUE 1

#define SET_CW_ROUND_TO_NEAREST \
        "push 0000037FH" \
        "fldcw dword ptr [esp]" \
        "add esp,4"

#define SET_CW_ROUND_TO_ZERO \
        "push 00000F7FH" \
        "fldcw dword ptr [esp]" \
        "add esp,4"

void c_dadd( void *dst, void *src1, void *src2 );
void c_dsub( void *dst, void *src1, void *src2 );

#ifndef C_IMPL
void _dadd( void *, void *, void * );
#pragma aux     _dadd = \
        SET_CW_ROUND_TO_NEAREST \
        "fld qword ptr [edx]" \
        "fld qword ptr [ebx]" \
        "fadd" \
        "fstp qword ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
void dadd( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    int diff;

    parseDouble( *(double*)src1, &f1, &e1 );
    parseDouble( *(double*)src2, &f2, &e2 );
    diff = abs( (e1 - e2) );
    if( diff > 11 && diff <= 53 ) {
        /* the FPU will round twice during this operation so use the c version
         * of this function instead */
        c_dadd( dst, src1, src2 );
    } else {
        _dadd( dst, src1, src2 );
    }
}

void _dsub( void *, void *, void * );
#pragma aux     _dsub = \
        SET_CW_ROUND_TO_NEAREST \
        "fld qword ptr [edx]" \
        "fsub qword ptr [ebx]" \
        "fstp qword ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
void dsub( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    int diff;

    parseDouble( *(double*)src1, &f1, &e1 );
    parseDouble( *(double*)src2, &f2, &e2 );
    diff = abs( (e1 - e2) );
    if( diff > 11 && diff <= 53 ) {
        /* the FPU will round twice during this operation so use the c version
         * of this function instead */
        c_dsub( dst, src1, src2 );
    } else {
        _dsub( dst, src1, src2 );
    }
}

void _dmul( void *, void *, void * );
#pragma aux     _dmul = \
        SET_CW_ROUND_TO_NEAREST \
        "fld qword ptr [edx]" \
        "fmul qword ptr [ebx]" \
        "fstp qword ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
void dmul( void *dst, void *src1, void *src2 ) {
    _dmul( dst, src1, src2 );
}

void _ddiv( void *, void *, void * );
#pragma aux     _ddiv = \
        SET_CW_ROUND_TO_NEAREST \
        "fld qword ptr [edx]" \
        "fdiv qword ptr [ebx]" \
        "fstp qword ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
void ddiv( void *dst, void *src1, void *src2 ) {
    _ddiv( dst, src1, src2 );
}

void _drem( void *, void *, void * );
#pragma aux     _drem = \
        SET_CW_ROUND_TO_NEAREST \
        "fld qword ptr [ebx]" \
        "fld qword ptr [edx]" \
        "mov ecx,eax" \
        "remloop:" \
        "fprem" \
        "fstsw ax" \
        "test ax,0400H" \
        "jne remloop" \
        "mov eax,ecx" \
        "fstp qword ptr [eax]" \
        "fstp qword ptr [ebx]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [ecx];
void drem( void *dst, void *src1, void *src2 ) {
    _drem( dst, src1, src2 );
}

void _dneg( void *, void * );
#pragma aux     _dneg = \
        "fld qword ptr [edx]" \
        "fchs" \
        "fstp qword ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [];
void dneg( void *dst, void *src ) {
    _dneg( dst, src );
}

void _fadd( void *, void *, void * );
#pragma aux     _fadd = \
        SET_CW_ROUND_TO_NEAREST \
        "fld dword ptr [edx]" \
        "fadd dword ptr [ebx]" \
        "fstp dword ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
void fadd( void *dst, void *src1, void *src2 ) {
    _fadd( dst, src1, src2 );
}

void _fsub( void *, void *, void * );
#pragma aux     _fsub = \
        SET_CW_ROUND_TO_NEAREST \
        "fld dword ptr [edx]" \
        "fsub dword ptr [ebx]" \
        "fstp dword ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
void fsub( void *dst, void *src1, void *src2 ) {
    _fsub( dst, src1, src2 );
}

void _fmul( void *, void *, void * );
#pragma aux     _fmul = \
        SET_CW_ROUND_TO_NEAREST \
        "fld dword ptr [edx]" \
        "fld dword ptr [ebx]" \
        "fmul" \
        "fstp dword ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
void fmul( void *dst, void *src1, void *src2 ) {
    _fmul( dst, src1, src2 );
}

void _fdiv( void *, void *, void * );
#pragma aux     _fdiv = \
        SET_CW_ROUND_TO_NEAREST \
        "fld dword ptr [edx]" \
        "fld dword ptr [ebx]" \
        "fdiv" \
        "fstp dword ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
void fdiv( void *dst, void *src1, void *src2 ) {
    _fdiv( dst, src1, src2 );
}

void _frem( void *, void *, void * );
#pragma aux     _frem = \
        SET_CW_ROUND_TO_NEAREST \
        "fld dword ptr [ebx]" \
        "fld dword ptr [edx]" \
        "mov ecx,eax" \
        "remloop:" \
        "fprem" \
        "fstsw ax" \
        "test ax,0400H" \
        "jne remloop" \
        "mov eax,ecx" \
        "fstp dword ptr [eax]" \
        "fstp dword ptr [ebx]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [ecx];
void frem( void *dst, void *src1, void *src2 ) {
    _frem( dst, src1, src2 );
}

void _fneg( void *, void * );
#pragma aux     _fneg = \
        "fld dword ptr [edx]" \
        "fchs" \
        "fstp dword ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [];
void fneg( void *dst, void *src ) {
    _fneg( dst, src );
}

void _eadd( void *, void *, void * );
#pragma aux     _eadd = \
        SET_CW_ROUND_TO_NEAREST \
        "fld tbyte ptr [edx]" \
        "fld tbyte ptr [ebx]" \
        "fadd" \
        "fstp tbyte ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
void eadd( void *dst, void *src1, void *src2 ) {
    _eadd( dst, src1, src2 );
}

void _esub( void *, void *, void * );
#pragma aux     _esub = \
        SET_CW_ROUND_TO_NEAREST \
        "fld tbyte ptr [edx]" \
        "fld tbyte ptr [edx]" \
        "fsub" \
        "fstp tbyte ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
void esub( void *dst, void *src1, void *src2 ) {
    _esub( dst, src1, src2 );
}

void _emul( void *, void *, void * );
#pragma aux     _emul = \
        SET_CW_ROUND_TO_NEAREST \
        "finit" \
        "fld tbyte ptr [edx]" \
        "fld tbyte ptr [ebx]" \
        "fmul" \
        "fstp tbyte ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
void emul( void *dst, void *src1, void *src2 ) {
    _emul( dst, src1, src2 );
}

void _ediv( void *, void *, void * );
#pragma aux     _ediv = \
        SET_CW_ROUND_TO_NEAREST \
        "fld tbyte ptr [edx]" \
        "fld tbyte ptr [ebx]" \
        "fdiv" \
        "fstp tbyte ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
void ediv( void *dst, void *src1, void *src2 ) {
    _ediv( dst, src1, src2 );
}

void _erem( void *, void *, void * );
#pragma aux     _erem = \
        SET_CW_ROUND_TO_NEAREST \
        "fld tbyte ptr [ebx]" \
        "fld tbyte ptr [edx]" \
        "mov ecx,eax" \
        "remloop:" \
        "fprem" \
        "fstsw ax" \
        "test ax,0400H" \
        "jne remloop" \
        "mov eax,ecx" \
        "fstp tbyte ptr [eax]" \
        "fstp tbyte ptr [ebx]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [ecx];
void erem( void *dst, void *src1, void *src2 ) {
    _erem( dst, src1, src2 );
}

void _eneg( void *, void * );
#pragma aux     _eneg = \
        "fld tbyte ptr [edx]" \
        "fchs" \
        "fstp tbyte ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [];
void eneg( void *dst, void *src ) {
    _eneg( dst, src );
}

/****** conversion functions **********/

void _f2d( void *, void * );
#pragma aux     _f2d = \
        SET_CW_ROUND_TO_NEAREST \
        "fld dword ptr [edx]" \
        "fstp qword ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [ebx];
void f2d( void *dst, void *src ) {
    _f2d( dst, src );
}

void _d2f( void *, void * );
#pragma aux     _d2f = \
        SET_CW_ROUND_TO_NEAREST \
        "fld qword ptr [edx]" \
        "fstp dword ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [ebx];
void d2f( void *dst, void *src ) {
    _d2f( dst, src );
}

void _e2d( void *, void * );
#pragma aux     _e2d = \
        SET_CW_ROUND_TO_NEAREST \
        "fld tbyte ptr [edx]" \
        "fstp qword ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [];
void e2d( void *dst, void *src ) {
    _e2d( dst, src );
}

void _d2e( void *, void * );
#pragma aux     _d2e = \
        SET_CW_ROUND_TO_NEAREST \
        "fld qword ptr [edx]" \
        "fstp tbyte ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [];
void d2e( void *dst, void *src ) {
    _d2e( dst, src );
}

void _e2f( void *, void * );
#pragma aux     _e2f = \
        SET_CW_ROUND_TO_NEAREST \
        "fld tbyte ptr [edx]" \
        "fstp dword ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [];
void e2f( void *dst, void *src ) {
    _e2f( dst, src );
}

void _f2e( void *, void * );
#pragma aux     _f2e = \
        SET_CW_ROUND_TO_NEAREST \
        "fld dword ptr [edx]" \
        "fstp tbyte ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [];
void f2e( void *dst, void *src ) {
    _f2e( dst, src );
}

void _f2i( void *, void * );
#pragma aux     _f2i = \
        SET_CW_ROUND_TO_ZERO \
        "fld dword ptr [edx]" \
        "mov ebx,eax" \
        "push 07FFFFFFFH" \
        "fild dword ptr [esp]" \
        "add esp,4" \
        "fcomp" \
        "fstsw ax" \
        "test ax,0400H" \
        "jnz nan" \
        "test ax,0100H" \
        "jnz large" \
        "mov eax,ebx" \
        "fist dword ptr [eax]" \
        "jmp done" \
        "nan:" \
        "mov eax,ebx" \
        "mov dword ptr [eax],0000H" \
        "jmp done" \
        "large:" \
        "mov eax,ebx" \
        "mov dword ptr [eax],7FFFFFFFH" \
        "done:" \
        "fstp dword ptr [edx]" \
        parm caller [eax] [edx] \
        modify exact [];
void f2i( void *dst, void *src ) {
    _f2i( dst, src );
}

void _i2f( void *, void * );
#pragma aux     _i2f = \
        SET_CW_ROUND_TO_NEAREST \
        "fild dword ptr [edx]" \
        "fstp dword ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [];
void i2f( void *dst, void *src ) {
    _i2f( dst, src );
}

void _f2l( void *, void * );
#pragma aux     _f2l = \
        SET_CW_ROUND_TO_ZERO \
        "fld dword ptr [edx]" \
        "mov ebx,eax" \
        "push 07FFFFFFFH" \
        "push 0FFFFFFFFH" \
        "fild qword ptr [esp]" \
        "add esp,8" \
        "fcomp" \
        "fstsw ax" \
        "test ax,0400H" \
        "jnz nan" \
        "test ax,0100H" \
        "jnz large" \
        "mov eax,ebx" \
        "fistp qword ptr [eax]" \
        "jmp done" \
        "nan:" \
        "mov eax,ebx" \
        "mov dword ptr [eax],0000H" \
        "mov dword ptr +4[eax],0000H" \
        "fstp dword ptr [edx]" \
        "jmp done" \
        "large:" \
        "mov eax,ebx" \
        "mov dword ptr [eax],0FFFFFFFFH" \
        "mov dword ptr +4[eax],07FFFFFFFH" \
        "fstp dword ptr [edx]" \
        "done:" \
        parm caller [eax] [edx] \
        modify exact [];
void f2l( void *dst, void *src ) {
    _f2l( dst, src );
}

void _l2f( void *, void * );
#pragma aux     _l2f = \
        SET_CW_ROUND_TO_NEAREST \
        "fild qword ptr [edx]" \
        "fstp dword ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [];
void l2f( void *dst, void *src ) {
    _l2f( dst, src );
}

void _d2i( void *, void * );
#pragma aux     _d2i = \
        SET_CW_ROUND_TO_ZERO \
        "fld qword ptr [edx]" \
        "mov ebx,eax" \
        "push 07FFFFFFFH" \
        "fild dword ptr [esp]" \
        "add esp,4" \
        "fcomp" \
        "fstsw ax" \
        "test ax,0400H" \
        "jnz nan" \
        "test ax,0100H" \
        "jnz large" \
        "mov eax,ebx" \
        "fist dword ptr [eax]" \
        "jmp done" \
        "nan:" \
        "mov eax,ebx" \
        "mov dword ptr [eax],0000H" \
        "jmp done" \
        "large:" \
        "mov eax,ebx" \
        "mov dword ptr [eax],7FFFFFFFH" \
        "done:" \
        "fstp qword ptr [edx]" \
        parm caller [eax] [edx] \
        modify exact [];
void d2i( void *dst, void *src ) {
    _d2i( dst, src );
}

void _i2d( void *, void * );
#pragma aux     _i2d = \
        SET_CW_ROUND_TO_NEAREST \
        "fild dword ptr [edx]" \
        "fstp qword ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [];
void i2d( void *dst, void *src ) {
    _i2d( dst, src );
}

void _d2l( void *, void * );
#pragma aux     _d2l = \
        SET_CW_ROUND_TO_ZERO \
        "fld qword ptr [edx]" \
        "mov ebx,eax" \
        "push 07FFFFFFFH" \
        "push 0FFFFFFFFH" \
        "fild qword ptr [esp]" \
        "add esp,8" \
        "fcomp" \
        "fstsw ax" \
        "test ax,0400H" \
        "jnz nan" \
        "test ax,0100H" \
        "jnz large" \
        "mov eax,ebx" \
        "fistp qword ptr [eax]" \
        "jmp done" \
        "nan:" \
        "mov eax,ebx" \
        "mov dword ptr [eax],0000H" \
        "mov dword ptr +4[eax],0000H" \
        "fstp qword ptr [edx]" \
        "jmp done" \
        "large:" \
        "mov eax,ebx" \
        "mov dword ptr [eax],0FFFFFFFFH" \
        "mov dword ptr +4[eax],07FFFFFFFH" \
        "fstp qword ptr [edx]" \
        "done:" \
        parm caller [eax] [edx] \
        modify exact [];
void d2l( void *dst, void *src ) {
    _d2l( dst, src );
}

void _l2d( void *, void * );
#pragma aux     _l2d = \
        SET_CW_ROUND_TO_NEAREST \
        "fild qword ptr [edx]" \
        "fstp qword ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [];
void l2d( void *dst, void *src ) {
    _l2d( dst, src );
}

void _e2i( void *, void * );
#pragma aux     _e2i = \
        SET_CW_ROUND_TO_ZERO \
        "fld tbyte ptr [edx]" \
        "mov ebx,eax" \
        "push 07FFFFFFFH" \
        "fild dword ptr [esp]" \
        "add esp,4" \
        "fcomp" \
        "fstsw ax" \
        "test ax,0400H" \
        "jnz nan" \
        "test ax,0100H" \
        "jnz large" \
        "mov eax,ebx" \
        "fist dword ptr [eax]" \
        "jmp done" \
        "nan:" \
        "mov eax,ebx" \
        "mov dword ptr [eax],0000H" \
        "jmp done" \
        "large:" \
        "mov eax,ebx" \
        "mov dword ptr [eax],7FFFFFFFH" \
        "done:" \
        "fstp tbyte ptr [edx]" \
        parm caller [eax] [edx] \
        modify exact [];
void e2i( void *dst, void *src ) {
    _e2i( dst, src );
}

void _i2e( void *, void * );
#pragma aux     _i2e = \
        SET_CW_ROUND_TO_NEAREST \
        "fild dword ptr [edx]" \
        "fstp tbyte ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [];
void i2e( void *dst, void *src ) {
    _i2e( dst, src );
}

void _e2l( void *, void * );
#pragma aux     _e2l = \
        SET_CW_ROUND_TO_ZERO \
        "fld tbyte ptr [edx]" \
        "mov ebx,eax" \
        "push 07FFFFFFFH" \
        "push 0FFFFFFFFH" \
        "fild qword ptr [esp]" \
        "add esp,8" \
        "fcomp" \
        "fstsw ax" \
        "test ax,0400H" \
        "jnz nan" \
        "test ax,0100H" \
        "jnz large" \
        "mov eax,ebx" \
        "fistp qword ptr [eax]" \
        "jmp done" \
        "nan:" \
        "mov eax,ebx" \
        "mov dword ptr [eax],0000H" \
        "mov dword ptr +4[eax],0000H" \
        "fstp tbyte ptr [edx]" \
        "jmp done" \
        "large:" \
        "mov eax,ebx" \
        "mov dword ptr [eax],0FFFFFFFFH" \
        "mov dword ptr +4[eax],07FFFFFFFH" \
        "fstp tbyte ptr [edx]" \
        "done:" \
        parm caller [eax] [edx] \
        modify exact [];
void e2l( void *dst, void *src ) {
    _e2l( dst, src );
}

void _l2e( void *, void * );
#pragma aux     _l2e = \
        SET_CW_ROUND_TO_NEAREST \
        "fild qword ptr [edx]" \
        "fstp tbyte ptr [eax]" \
        parm caller [eax] [edx] \
        modify exact [];
void l2e( void *dst, void *src ) {
    _l2e( dst, src );
}

/******** comparison functions **********/

int cmp( unsigned short result )
{
    result = result & 0x4500;
    if( result == 0 ) {
        /* arg1 > arg2 */
        return -1;
    } else if( result == 0x0100 ) {
        /* arg1 < arg2 */
        return 1;
    } else if( result == 0x4000 ) {
        /* arg1 = arg2 */
        return 0;
    } else {  /* result == 0x4500 */
        /* unordered (NaN) */
        return -2;
    }
}

void _fcmp( void *, void *, void * );
#pragma aux _fcmp = \
        "fld dword ptr [edx]" \
        "fcomp dword ptr [ebx]" \
        "fstsw word ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
int fcmp( void *arg1, void *arg2 )
{
    unsigned short result;
    _fcmp( &result, arg1, arg2 );
    return cmp( result );
}

void _dcmp( void *, void *, void * );
#pragma aux _dcmp = \
        "fld qword ptr [edx]" \
        "fcomp qword ptr [ebx]" \
        "fstsw word ptr [eax]" \
        parm caller [eax] [edx] [ebx] \
        modify exact [];
int dcmp( void *arg1, void *arg2 )
{
    unsigned short result;
    _dcmp( &result, arg1, arg2 );
    return cmp( result );
}
#endif

/******* boolean functions ***********/

/* a special value is +inf, -inf, or nan */
int f_isSpecialValue( void *val )
{
    char *v = (char*)val;
    if( (v[3] == 127 || v[3] == 255) && v[2] >= 128 ) {
        return TRUE;
    }
    return FALSE;
}

int d_isSpecialValue( void *val )
{
    char *v = (char*)val;
    if( (v[7] == 127 || v[7] == 255) && v[6] >= (128+64+32+16) ) {
        return TRUE;
    }
    return FALSE;
}

int e_isSpecialValue( void *val )
{
    char *v = (char*)val;
    if( (v[9] == 127 || v[9] == 255) && v[8] == 255 ) {
        return TRUE;
    }
    return FALSE;
}

int f_isNan( void *val )
{
    char *v = (char*)val;
    if( f_isSpecialValue( val ) ) {
        // nan or inf
        if( v[0] > 0 || v[1] > 0 || v[2] > 128 ) {
            return TRUE;
        }
    }
    return FALSE;
}

int d_isNan( void *val )
{
    char *v = (char*)val;
    if( d_isSpecialValue( val ) ) {
        // nan or inf
        if( v[0] > 0 || v[1] > 0 || v[2] > 0 || v[3] > 0 ||
            v[4] > 0 || v[5] > 0 || v[6] > (128+64+32+16) ) {
            return TRUE;
        }
    }
    return FALSE;
}

int e_isNan( void *val )
{
    if( e_isSpecialValue( val ) ) {
        // nan or inf
        if( *(uint64*)val > 0 ) {
            return TRUE;
        }
    }
    return FALSE;
}

int f_isInf( void *val )
{
    return( f_isPosInf( val ) || f_isNegInf( val ) );
}

int d_isInf( void *val )
{
    return( d_isPosInf( val ) || d_isNegInf( val ) );
}

int e_isInf( void *val )
{
    return( e_isPosInf( val ) || e_isNegInf( val ) );
}

int f_isPosInf( void *val )
{
    uint32 *v = (uint32 *)val;
    if( *v == FLOAT_POS_INF ) {
        return TRUE;
    }
    return FALSE;
}

int d_isPosInf( void *val )
{
    uint64 *v = (uint64 *)val;
    if( *v == DOUBLE_POS_INF ) {
        return TRUE;
    }
    return FALSE;
}

int e_isPosInf( void *val )
{
    char *v = (char*)val;
    if( *(uint64*)val == 0 && v[8] == 255 && v[9] == 127 ) {
        return TRUE;
    }
    return FALSE;
}

int f_isNegInf( void *val )
{
    uint32 *v = (uint32 *)val;
    if( *v == FLOAT_NEG_INF ) {
        return TRUE;
    }
    return FALSE;
}

int d_isNegInf( void *val )
{
    uint64 *v = (uint64 *)val;
    if( *v == DOUBLE_NEG_INF ) {
        return TRUE;
    }
    return FALSE;
}

int e_isNegInf( void *val )
{
    char *v = (char*)val;
    if( *(uint64*)val == 0 && v[8] == 255 && v[9] == 255 ) {
        return TRUE;
    }
    return FALSE;
}

int f_isZero( void *val )
{
    return( f_isPosZero( val ) || f_isNegZero( val ) );
}

int d_isZero( void *val )
{
    return( d_isPosZero( val ) || d_isNegZero( val ) );
}

int e_isZero( void *val )
{
    return( e_isPosZero( val ) || e_isNegZero( val ) );
}

int f_isPosZero( void *val )
{
    uint32 *v = (uint32 *)val;
    if( *v == FLOAT_POS_ZERO ) {
        return TRUE;
    }
    return FALSE;
}

int d_isPosZero( void *val )
{
    uint64 *v = (uint64 *)val;
    if( *v == DOUBLE_POS_ZERO ) {
        return TRUE;
    }
    return FALSE;
}

int e_isPosZero( void *val )
{
    char *v = (char*)val;
    int i;
    for( i = 0; i < 10; i++ ) {
        if( v[i] != 0 ) return FALSE;
    }
    return TRUE;
}

int f_isNegZero( void *val )
{
    uint32 *v = (uint32 *)val;
    if( *v == FLOAT_NEG_ZERO ) {
        return TRUE;
    }
    return FALSE;
}

int d_isNegZero( void *val )
{
    uint64 *v = (uint64 *)val;
    if( *v == DOUBLE_NEG_ZERO ) {
        return TRUE;
    }
    return FALSE;
}

int e_isNegZero( void *val )
{
    char *v = (char*)val;
    int i;
    for( i = 0; i < 9; i++ ) {
        if( v[i] != 0 ) return FALSE;
    }
    if( v[9] != 128 ) return FALSE;
    return TRUE;
}

int f_isNeg( void *val )
{
    char *v = (char*)val;
    if( v[3] >= 128 ) {
        return TRUE;
    }
    return FALSE;
}

int d_isNeg( void *val )
{
    char *v = (char*)val;
    if( v[7] >= 128 ) {
        return TRUE;
    }
    return FALSE;
}

int e_isNeg( void *val )
{
    char *v = (char*)val;
    if( v[9] >= 128 ) {
        return TRUE;
    }
    return FALSE;
}

int f_isEqual( void *val1, void *val2 )
{
    int result = fcmp( val1, val2 );
    if( result == 0 ) return TRUE;
    return FALSE;
}

int f_isNotEqual( void *val1, void *val2 )
{
    int result = fcmp( val1, val2 );
    if( result != 0 ) return TRUE;
    return FALSE;
}

int f_isGreater( void *val1, void *val2 )
{
    int result = fcmp( val1, val2 );
    if( result == -1 ) return TRUE;
    return FALSE;
}

int f_isGreaterEqual( void *val1, void *val2 )
{
    int result = fcmp( val1, val2 );
    if( result == -1 || result == 0 ) return TRUE;
    return FALSE;
}

int f_isLess( void *val1, void *val2 )
{
    int result = fcmp( val1, val2 );
    if( result == 1 ) return TRUE;
    return FALSE;
}

int f_isLessEqual( void *val1, void *val2 )
{
    int result = fcmp( val1, val2 );
    if( result == 1 || result == 0 ) return TRUE;
    return FALSE;
}

int d_isEqual( void *val1, void *val2 )
{
    int result = dcmp( val1, val2 );
    if( result == 0 ) return TRUE;
    return FALSE;
}

int d_isNotEqual( void *val1, void *val2 )
{
    int result = dcmp( val1, val2 );
    if( result != 0 ) return TRUE;
    return FALSE;
}

int d_isGreater( void *val1, void *val2 )
{
    int result = dcmp( val1, val2 );
    if( result == -1 ) return TRUE;
    return FALSE;
}

int d_isGreaterEqual( void *val1, void *val2 )
{
    int result = dcmp( val1, val2 );
    if( result == -1 || result == 0 ) return TRUE;
    return FALSE;
}

int d_isLess( void *val1, void *val2 )
{
    int result = dcmp( val1, val2 );
    if( result == 1 ) return TRUE;
    return FALSE;
}

int d_isLessEqual( void *val1, void *val2 )
{
    int result = dcmp( val1, val2 );
    if( result == 1 || result == 0 ) return TRUE;
    return FALSE;
}

/* construct a float from a mantissa and an exponent */
void makeFloat( float *dst, uint64 f, int e )
{
    char *temp = (char*)dst;
    char *temp2;
    uint32 exp;
    e += float_bias;
    if( e < -23 ) {
        /* zero */
        e = 0;
        f = 0;
    } else if( e < 0 ) {
        /* denormalized */
        e = 0;
    } else if( e >= 255 ) {
        /* infinity */
        e = 255;
        f = 0;
    }
    *(uint32*)dst = (uint32)f;
    temp[2] = temp[2] & 127; /* remove implied bit */
    exp = e;
    temp2 = (char*)&exp;
    temp[3] = temp2[0] >> 1;
    temp[2] |= (temp2[0] << 7);
}

/* construct a double from a mantissa and an exponent */
void makeDouble( double *dst, uint64 f, int e )
{
    char *temp = (char*)dst;
    char *temp2;
    uint32 exp;
    e += double_bias;
    if( e < -52 ) {
        /* zero */
        e = 0;
        f = 0;
    } else if( e < 0 ) {
        /* denormalized */
        e = 0;
    } else if( e >= 2047 ) {
        /* infinity */
        e = 2047;
        f = 0;
    }
    *(uint64*)dst = f;
    temp[6] = temp[6] & 15; /* remove implied bit */
    exp = e;
    temp2 = (char*)&exp;
    temp[7] = temp2[1] << 4;
    temp[7] = temp[7] | ((temp2[0] & 240) >> 4);
    temp[6] = temp[6] | (temp2[0] & 15) << 4;
}

/* construct an extended floating point from a mantissa and an exponent */
void makeExtended( char *dst, uint64 f, int e )
{
    char *temp;
    uint32 exp;
    e += extended_bias;
    if( e < 0 ) e = 0;
    if( e < -63 ) {
        /* zero */
        e = 0;
        f = 0;
    } else if( e < 0 ) {
        /* denormalized */
        e = 0;
    } else if( e >= 32767 ) {
        /* infinity */
        e = 32767;
        f = 0;
    }
    *(uint64*)dst = f;
    exp = e;
    temp = (char*)&exp;
    dst[9] = temp[1] & 127;
    dst[8] = temp[0];
}

/* break up a binary float value into a mantissa and an exponent and
 * normalize so that the result is between 0x800000 and 0x1000000 */
void parseFloat( float flt, uint64 *f, int *e )
{
    char *temp = (char*)&flt;
    temp[3] = temp[3] & 127;    /* remove sign */
    *e = (*(uint32*)temp) >> 23;
    *e -= float_bias;
    temp[3] = 0;                /* remove   */
    temp[2] = temp[2] & 127;    /* exponent */
    if( *e > 0 - float_bias ) {
        temp[2] = temp[2] | 128;  /* add implied leading bit */
    }
    if( *e == 0 - float_bias ) *e = *e + 1;
    *f = (uint64)(*(uint32*)temp);
    if( *f > 0 ) {
        while( *f < 0x800000 ) {
            *f <<= 1;
            (*e)--;
        }
    }
}

/* break up a binary double value into a mantissa and an exponent and
 * normalize so that the result is between 0x10000000000000 and
 * 0x20000000000000 */
void parseDouble( double db, uint64 *f, int *e )
{
    char *temp = (char*)&db;
    temp[7] = temp[7] & 127;    /* remove sign */
    *e = (*(uint64*)temp) >> 52;
    *e -= double_bias;
    temp[7] = 0;                /* remove   */
    temp[6] = temp[6] & 15;    /* exponent */
    if( *e > 0 - double_bias ) {
        temp[6] = temp[6] | 16;  /* add implied leading bit */
    }
    if( *e == 0 - double_bias ) *e = *e + 1;
    *f = *(uint64*)temp;
    if( *f > 0 ) {
        while( *f < 0x10000000000000 ) {
            *f <<= 1;
            (*e)--;
        }
    }
}

/* break up a binary extended float value into a mantissa and an exponent and
 * normalize */
void parseExtended( char *ext, uint64 *f, int *e )
{
    ext[9] = ext[9] & 127;    /* remove sign */
    *e = ext[9] * 256 + ext[8];
    *e -= extended_bias;
    ext[9] = 0;         /* remove   */
    ext[8] = 0;         /* exponent */
    *f = *(uint64*)ext;
    if( *f > 0 ) {
        while( *f < 0x8000000000000000 ) {
            *f <<= 1;
            (*e)--;
        }
    }
}

/* add diff to the the exponent of src */
void changeFloatExponent( float *dst, float *src, int diff )
{
    uint64 f;
    int e;
    parseFloat( *src, &f, &e );
    e += diff;
    makeFloat( dst, f, e );
}

void changeDoubleExponent( double *dst, double *src, int diff )
{
    uint64 f;
    int e;
    parseDouble( *src, &f, &e );
    e += diff;
    makeDouble( dst, f, e );
}

void changeExtendedExponent( char *dst, char *src, int diff )
{
    uint64 f;
    int e;
    parseExtended( src, &f, &e );
    e += diff;
    makeExtended( dst, f, e );
}

/* create special extended numbers */
void makeExtNan( char *num )
{
    memset( num, 0, 10 );
    num[9] = 127;
    num[8] = 255;
    num[7] = 128;
}

void makeExtPosInf( char *num )
{
    memset( num, 0, 10 );
    num[9] = 127;
    num[8] = 255;
}

void makeExtNegInf( char *num )
{
    memset( num, 0, 10 );
    num[9] = 255;
    num[8] = 255;
}

void makeExtPosZero( char *num )
{
    memset( num, 0, 10 );
}

void makeExtNegZero( char *num )
{
    memset( num, 0, 10 );
    num[9] = 128;
}

#ifdef TEST
/***************************  Testing   *********************************/

void OutputFloatInHex( void *fl )
{
    char *flt = (char*)fl;
    int i;
    printf( "0x" );
    for( i = 3; i >= 0; i-- ) {
        printf( "%02x", flt[i] );
    }
    printf( "\n" );
}

void OutputDoubleInHex( void *db )
{
    char *dbl = (char*)db;
    int i;
    printf( "0x" );
    for( i = 7; i >= 0; i-- ) {
        printf( "%02x", dbl[i] );
    }
    printf( "\n" );
}

void OutputFloatBits( void * fl )
{
    char buf[40];
    int i, j;
    int k = 0;
    char *flt = (char *)fl;
    for( j = 0; j < 4; j++ ) {
        int pow = 128;
        for( i = 0; i < 8; i++ ) {
            if( flt[3-j] & pow ) {
                buf[k] = '1';
            } else {
                buf[k] = '0';
            }
            pow = pow / 2;
            k++;
            if( k == 1 || k == 10 ) {
                buf[k] = ' ';
                k++;
            }
        }
    }
    buf[k] = '\0';
    printf( "%s\n", buf );
}

void OutputDoubleBits( void * db )
{
    char buf[70];
    int i, j;
    int k = 0;
    char *dbl = (char *)db;
    for( j = 0; j < 8; j++ ) {
        int pow = 128;
        for( i = 0; i < 8; i++ ) {
            if( dbl[7-j] & pow ) {
                buf[k] = '1';
            } else {
                buf[k] = '0';
            }
            pow = pow / 2;
            k++;
            if( k == 1 || k == 13 ) {
                buf[k] = ' ';
                k++;
            }
        }
    }
    buf[k] = '\0';
    printf( "%s\n", buf );
}

void floatTest( uint32 l )
{
    float f;
    uint32 r;
    char buf[21];
    int i, temp;

    f = *(float*)&l;
    f2a( buf, &f, 20 );
    a2f( (void*)&r, buf );
    if( l != r ) {
        printf( "0x%08x     %s", l, buf );
        temp = strlen( buf );
        for( i = 0; i < 15 - temp; i++ ) {
            printf( " " );
        }
        printf( "  0x%08x\n", r );
    }
}

void doubleTest( uint64 l )
{
    double d;
    uint64 r;
    char buf[25];
    int i, temp;

    d = *(double*)&l;
    d2a( buf, &d, 24 );
    a2d( (void*)&r, buf );
    if( l != r ) {
        OutputDoubleInHex( &l );
        printf( "    %s", buf );
        temp = strlen( buf );
        for( i = 0; i < 28 - temp; i++ ) {
            printf( " " );
        }
        OutputDoubleInHex( &r );
        printf( "\n" );
    }
}

void doQuickFloatTest()
{
    uint32 l = 0x00000000;

    for(;;) {
        floatTest( l );
        if( l >= 0xffffffff - 0x01010101 ) break;
        l += 0x01010101;
    }
    /* check denormalized numbers */
    l = 0x00000001;
    for(;;) {
        floatTest( l );
        if( l == 0x00800000 ) break;
        l = l<<1;
    }
}

void doFullFloatTest()
{
    uint32 l = 0x00000000;

    for(;;) {
        floatTest( l );
        if( l == 0xffffffff ) break;
        l += 0x00000001;
    }
}

void doQuickDoubleTest()
{
    uint64 ll = 0x0000000000000000;

    for(;;) {
        doubleTest( ll );
        if( ll >= 0xffffffffffffffff - 0x0101010101010101 ) break;
        ll += 0x0101010101010101;
    }
    /* check denormalized numbers */
    ll = 0x0000000000000001;
    for(;;) {
        doubleTest( ll );
        if( ll == 0x0001000000000000 ) break;
        ll = ll<<1;
    }
}

void doFullDoubleTest()
{
    uint64 ll = 0x0000000000000000;

    for(;;) {
        doubleTest( ll );
        if( ll == 0xffffffffffffffff ) break;
        ll += 0x0000000000000001;
    }
}

void main()
{
//    doFullFloatTest();
    doQuickFloatTest();
    doQuickDoubleTest();
}

#endif /* ifdef TEST */
