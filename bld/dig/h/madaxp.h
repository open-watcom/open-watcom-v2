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


#ifndef MADAXP_H
#define MADAXP_H
#include "digpck.h"
enum axp_cputypes {
    AXP_DUNNO,
    AXP_21064,
    AXP_21164
};

enum axp_machine_data {
    AXPMD_PDATA
};

typedef struct {
    unsigned_64         beg_addr;
    unsigned_64         end_addr;
    unsigned_64         except_handler;
    unsigned_64         handler_data;
    unsigned_64         pro_end_addr;
} axp_pdata;

typedef lreal   gfloat; //NYI: dummy definition
typedef lreal   dfloat; //NYI: dummy definition

typedef union {
    unsigned_64         u64;
      signed_64         s64;
    lreal               t;
    gfloat              g;
    dfloat              d;
} axpreg;

enum {
    AR_f0,
    AR_f1,
    AR_f2,
    AR_f3,
    AR_f4,
    AR_f5,
    AR_f6,
    AR_f7,
    AR_f8,
    AR_f9,
    AR_f10,
    AR_f11,
    AR_f12,
    AR_f13,
    AR_f14,
    AR_f15,
    AR_f16,
    AR_f17,
    AR_f18,
    AR_f19,
    AR_f20,
    AR_f21,
    AR_f22,
    AR_f23,
    AR_f24,
    AR_f25,
    AR_f26,
    AR_f27,
    AR_f28,
    AR_f29,
    AR_f30,
    AR_f31,

    AR_r0,
    AR_r1,
    AR_r2,
    AR_r3,
    AR_r4,
    AR_r5,
    AR_r6,
    AR_r7,
    AR_r8,
    AR_r9,
    AR_r10,
    AR_r11,
    AR_r12,
    AR_r13,
    AR_r14,
    AR_r15,
    AR_r16,
    AR_r17,
    AR_r18,
    AR_r19,
    AR_r20,
    AR_r21,
    AR_r22,
    AR_r23,
    AR_r24,
    AR_r25,
    AR_r26,
    AR_r27,
    AR_r28,
    AR_r29,
    AR_r30,
    AR_r31,

    AR_fpcr,
    AR_NUM_REGS,

    AR_v0       = AR_r0,
    AR_t0       = AR_r1,
    AR_t1       = AR_r2,
    AR_t2       = AR_r3,
    AR_t3       = AR_r4,
    AR_t4       = AR_r5,
    AR_t5       = AR_r6,
    AR_t6       = AR_r7,
    AR_t7       = AR_r8,
    AR_s0       = AR_r9,
    AR_s1       = AR_r10,
    AR_s2       = AR_r11,
    AR_s3       = AR_r12,
    AR_s4       = AR_r13,
    AR_s5       = AR_r14,
    AR_fp       = AR_r15,
    AR_a0       = AR_r16,
    AR_a1       = AR_r17,
    AR_a2       = AR_r18,
    AR_a3       = AR_r19,
    AR_a4       = AR_r20,
    AR_a5       = AR_r21,
    AR_t8       = AR_r22,
    AR_t9       = AR_r23,
    AR_t10      = AR_r24,
    AR_t11      = AR_r25,
    AR_ra       = AR_r26,
    AR_t12      = AR_r27,
    AR_at       = AR_r28,
    AR_gp       = AR_r29,
    AR_sp       = AR_r30
};


typedef unsigned_8 axp_pal; enum {
    PAL_all,
    PAL_nt,
    PAL_unix,
    PAL_vms
};

struct axp_mad_registers {
    axpreg      r[AR_NUM_REGS];
    /* the PC must be the first item in all of the PAL reg structures */
    union {
        struct {
            unsigned_64         fir;
            unsigned_64         softfpcr;
            unsigned_32         psr;
        }       nt;
        struct {
            unsigned_64         pc;
            unsigned_8          ps;
        }       unix;
        struct {
            unsigned_64         pc;
            unsigned_64         psr;
        }       vms;
    }           pal;
    axp_pal                     active_pal;
};

#include "digunpck.h"
#endif
