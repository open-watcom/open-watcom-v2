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


#ifndef MADPPC_H
#define MADPPC_H
#include "digpck.h"
enum ppc_cputypes {
    PPC_DUNNO,
    PPC_601,
    PPC_603,
    PPC_604,
    PPC_620
};

typedef union {
    unsigned_64         u64;
    lreal               f;
} ppcfloat;

struct ppc_mad_registers {
    unsigned_64 r0;
    union {
        unsigned_64     r1;
        unsigned_64     sp;
    };
    union {
        unsigned_64     r2;
        unsigned_64     rtoc;
    };
    unsigned_64 r3;
    unsigned_64 r4;
    unsigned_64 r5;
    unsigned_64 r6;
    unsigned_64 r7;
    unsigned_64 r8;
    unsigned_64 r9;
    unsigned_64 r10;
    unsigned_64 r11;
    unsigned_64 r12;
    unsigned_64 r13;
    unsigned_64 r14;
    unsigned_64 r15;
    unsigned_64 r16;
    unsigned_64 r17;
    unsigned_64 r18;
    unsigned_64 r19;
    unsigned_64 r20;
    unsigned_64 r21;
    unsigned_64 r22;
    unsigned_64 r23;
    unsigned_64 r24;
    unsigned_64 r25;
    unsigned_64 r26;
    unsigned_64 r27;
    unsigned_64 r28;
    unsigned_64 r29;
    unsigned_64 r30;
    unsigned_64 r31;

    unsigned_64 lr;
    unsigned_64 ctr;
    unsigned_64 iar;
    unsigned_64 msr;

    unsigned_32 cr;
    unsigned_32 xer;

    unsigned_32 fpscr;
    unsigned_32 unused; /* keep things 64-bit aligned */

    ppcfloat    f0;
    ppcfloat    f1;
    ppcfloat    f2;
    ppcfloat    f3;
    ppcfloat    f4;
    ppcfloat    f5;
    ppcfloat    f6;
    ppcfloat    f7;
    ppcfloat    f8;
    ppcfloat    f9;
    ppcfloat    f10;
    ppcfloat    f11;
    ppcfloat    f12;
    ppcfloat    f13;
    ppcfloat    f14;
    ppcfloat    f15;
    ppcfloat    f16;
    ppcfloat    f17;
    ppcfloat    f18;
    ppcfloat    f19;
    ppcfloat    f20;
    ppcfloat    f21;
    ppcfloat    f22;
    ppcfloat    f23;
    ppcfloat    f24;
    ppcfloat    f25;
    ppcfloat    f26;
    ppcfloat    f27;
    ppcfloat    f28;
    ppcfloat    f29;
    ppcfloat    f30;
    ppcfloat    f31;
};

#define PPC_BF          31 /* bit flipper */
#define MSR_L_le        (PPC_BF-31)
#define MSR_L_ri        (PPC_BF-30)
#define MSR_L_dr        (PPC_BF-27)
#define MSR_L_ir        (PPC_BF-26)
#define MSR_L_ip        (PPC_BF-25)
#define MSR_L_fe1       (PPC_BF-23)
#define MSR_L_be        (PPC_BF-22)
#define MSR_L_se        (PPC_BF-21)
#define MSR_L_fe0       (PPC_BF-20)
#define MSR_L_me        (PPC_BF-19)
#define MSR_L_fp        (PPC_BF-18)
#define MSR_L_pr        (PPC_BF-17)
#define MSR_L_ee        (PPC_BF-16)
#define MSR_L_ile       (PPC_BF-15)
#define MSR_L_pow       (PPC_BF-13)

#define MSR_H_sf        (PPC_BF-0)

#include "digunpck.h"
#endif
