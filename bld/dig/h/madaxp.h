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

typedef unsigned_8 axp_pal; enum {
    PAL_all,
    PAL_nt,
    PAL_unix,
    PAL_vms
};

struct axp_mad_registers {
    axpreg          f0;
    axpreg          f1;
    axpreg          f2;
    axpreg          f3;
    axpreg          f4;
    axpreg          f5;
    axpreg          f6;
    axpreg          f7;
    axpreg          f8;
    axpreg          f9;
    axpreg          f10;
    axpreg          f11;
    axpreg          f12;
    axpreg          f13;
    axpreg          f14;
    axpreg          f15;
    axpreg          f16;
    axpreg          f17;
    axpreg          f18;
    axpreg          f19;
    axpreg          f20;
    axpreg          f21;
    axpreg          f22;
    axpreg          f23;
    axpreg          f24;
    axpreg          f25;
    axpreg          f26;
    axpreg          f27;
    axpreg          f28;
    axpreg          f29;
    axpreg          f30;
    axpreg          f31;

    union {
        axpreg      r0;
        axpreg      v0;
    } u0;
    union {
        axpreg      r1;
        axpreg      t0;
    } u1;
    union {
        axpreg      r2;
        axpreg      t1;
    } u2;
    union {
        axpreg      r3;
        axpreg      t2;
    } u3;
    union {
        axpreg      r4;
        axpreg      t3;
    } u4;
    union {
        axpreg      r5;
        axpreg      t4;
    } u5;
    union {
        axpreg      r6;
        axpreg      t5;
    } u6;
    union {
        axpreg      r7;
        axpreg      t6;
    } u7;
    union {
        axpreg      r8;
        axpreg      t7;
    } u8;
    union {
        axpreg      r9;
        axpreg      s0;
    } u9;
    union {
        axpreg      r10;
        axpreg      s1;
    } u10;
    union {
        axpreg      r11;
        axpreg      s2;
    } u11;
    union {
        axpreg      r12;
        axpreg      s3;
    } u12;
    union {
        axpreg      r13;
        axpreg      s4;
    } u13;
    union {
        axpreg      r14;
        axpreg      s5;
    } u14;
    union {
        axpreg      r15;
        axpreg      fp;
    } u15;
    union {
        axpreg      r16;
        axpreg      a0;
    } u16;
    union {
        axpreg      r17;
        axpreg      a1;
    } u17;
    union {
        axpreg      r18;
        axpreg      a2;
    } u18;
    union {
        axpreg      r19;
        axpreg      a3;
    } u19;
    union {
        axpreg      r20;
        axpreg      a4;
    } u20;
    union {
        axpreg      r21;
        axpreg      a5;
    } u21;
    union {
        axpreg      r22;
        axpreg      t8;
    } u22;
    union {
        axpreg      r23;
        axpreg      t9;
    } u23;
    union {
        axpreg      r24;
        axpreg      t10;
    } u24;
    union {
        axpreg      r25;
        axpreg      t11;
    } u25;
    union {
        axpreg      r26;
        axpreg      ra;
    } u26;
    union {
        axpreg      r27;
        axpreg      t12;
    } u27;
    union {
        axpreg      r28;
        axpreg      at;
    } u28;
    union {
        axpreg      r29;
        axpreg      gp;
    } u29;
    union {
        axpreg      r30;
        axpreg      sp;
    } u30;
    union {
        axpreg      r31;
        axpreg      zero;
    } u31;

    unsigned_64     fpcr;

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
        }       nix;
        struct {
            unsigned_64         pc;
            unsigned_64         psr;
        }       vms;
    }           pal;
    axp_pal                     active_pal;
};

#include "digunpck.h"
#endif
