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
* Description:  MIPS specific register structure for MAD.
*
****************************************************************************/


#ifndef MADMIPS_H
#define MADMIPS_H

#include "digpck.h"
enum mips_cputypes {
    MIPS_DUNNO,
    MIPS_R2000,
    MIPS_R3000,
    MIPS_R4000,
    MIPS_R5000
};

typedef union {
    unsigned_64         u64;
    lreal               f;
    struct {
        sreal           s_lo;
        sreal           s_hi;
    } u;
} mipsfloat;

struct mips_mad_registers {
    union {
        unsigned_64     r0;
        unsigned_64     zero;
    } u0;
    union {
        unsigned_64     r1;
        unsigned_64     at;
    } u1;
    union {
        unsigned_64     r2;
        unsigned_64     v0;
    } u2;
    union {
        unsigned_64     r3;
        unsigned_64     v1;
    } u3;
    union {
        unsigned_64     r4;
        unsigned_64     a0;
    } u4;
    union {
        unsigned_64     r5;
        unsigned_64     a1;
    } u5;
    union {
        unsigned_64     r6;
        unsigned_64     a2;
    } u6;
    union {
        unsigned_64     r7;
        unsigned_64     a3;
    } u7;
    union {
        unsigned_64     r8;
        unsigned_64     t0;
    } u8;
    union {
        unsigned_64     r9;
        unsigned_64     t1;
    } u9;
    union {
        unsigned_64     r10;
        unsigned_64     t2;
    } u10;
    union {
        unsigned_64     r11;
        unsigned_64     t3;
    } u11;
    union {
        unsigned_64     r12;
        unsigned_64     t4;
    } u12;
    union {
        unsigned_64     r13;
        unsigned_64     t5;
    } u13;
    union {
        unsigned_64     r14;
        unsigned_64     t6;
    } u14;
    union {
        unsigned_64     r15;
        unsigned_64     t7;
    } u15;
    union {
        unsigned_64     r16;
        unsigned_64     s0;
    } u16;
    union {
        unsigned_64     r17;
        unsigned_64     s1;
    } u17;
    union {
        unsigned_64     r18;
        unsigned_64     s2;
    } u18;
    union {
        unsigned_64     r19;
        unsigned_64     s3;
    } u19;
    union {
        unsigned_64     r20;
        unsigned_64     s4;
    } u20;
    union {
        unsigned_64     r21;
        unsigned_64     s5;
    } u21;
    union {
        unsigned_64     r22;
        unsigned_64     s6;
    } u22;
    union {
        unsigned_64     r23;
        unsigned_64     s7;
    } u23;
    union {
        unsigned_64     r24;
        unsigned_64     t8;
    } u24;
    union {
        unsigned_64     r25;
        unsigned_64     t9;
    } u25;
    union {
        unsigned_64     r26;
        unsigned_64     k0;
    } u26;
    union {
        unsigned_64     r27;
        unsigned_64     k1;
    } u27;
    union {
        unsigned_64     r28;
        unsigned_64     gp;
    } u28;
    union {
        unsigned_64     r29;
        unsigned_64     sp;
    } u29;
    union {
        unsigned_64     r30;
        unsigned_64     fp;
        unsigned_64     s8;
    } u30;
    union {
        unsigned_64     r31;
        unsigned_64     ra;
    } u31;

    unsigned_64 pc;
    unsigned_32 lo;
    unsigned_32 hi;
    unsigned_32 fpcsr;
    unsigned_32 fpivr;

    mipsfloat   f0;
    mipsfloat   f1;
    mipsfloat   f2;
    mipsfloat   f3;
    mipsfloat   f4;
    mipsfloat   f5;
    mipsfloat   f6;
    mipsfloat   f7;
    mipsfloat   f8;
    mipsfloat   f9;
    mipsfloat   f10;
    mipsfloat   f11;
    mipsfloat   f12;
    mipsfloat   f13;
    mipsfloat   f14;
    mipsfloat   f15;
    mipsfloat   f16;
    mipsfloat   f17;
    mipsfloat   f18;
    mipsfloat   f19;
    mipsfloat   f20;
    mipsfloat   f21;
    mipsfloat   f22;
    mipsfloat   f23;
    mipsfloat   f24;
    mipsfloat   f25;
    mipsfloat   f26;
    mipsfloat   f27;
    mipsfloat   f28;
    mipsfloat   f29;
    mipsfloat   f30;
    mipsfloat   f31;
};
#include "digunpck.h"

#endif
