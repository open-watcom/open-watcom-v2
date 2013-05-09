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


#ifndef MADX64_H
#define MADX64_H
#include "digpck.h"
enum x64_cputypes {
        X64_CPU1 = 0x01,
};

enum x64_fputypes {
        X64_FPU1 = 0x01,
};

enum x64_machine_data {
    X64MD_ADDR_CHARACTERISTICS
};

enum x64_addr_characteristics {
    X64AC_BIG   = 0x01
};

typedef struct {
    union {
        unsigned_64     q[2];
        unsigned_32     d[4];
        unsigned_16     w[8];
        unsigned_8      b[16];
    } u;
} xmm_reg;

struct x64_cpu {
    unsigned_16 cs;
    unsigned_16 ds;
    unsigned_16 es;
    unsigned_16 fs;
    unsigned_16 gs;
    unsigned_16 ss;
    unsigned_32 eflags;

    unsigned_64 dr0;
    unsigned_64 dr1;
    unsigned_64 dr2;
    unsigned_64 dr3;
    unsigned_64 dr6;
    unsigned_64 dr7;

    unsigned_64 rax;
    unsigned_64 rcx;
    unsigned_64 rdx;
    unsigned_64 rbx;
    unsigned_64 rsp;
    unsigned_64 rbp;
    unsigned_64 rsi;
    unsigned_64 rdi;
    unsigned_64 r8;
    unsigned_64 r9;
    unsigned_64 r10;
    unsigned_64 r11;
    unsigned_64 r12;
    unsigned_64 r13;
    unsigned_64 r14;
    unsigned_64 r15;

    unsigned_64 rip;

};

typedef struct {
        unsigned_16     low    : 16;
        unsigned_16     __fill1: 16;
        unsigned_16     opcode : 11;    /* not really there for data ptr */
        unsigned_16     __fill2: 1;
        unsigned_16     hi     : 4;     /* really should be 16 bits */
} fpu_ptr_rm;

typedef struct {
        unsigned_32     offset;
        unsigned_32     segment;
} fpu_ptr_pm;

typedef union {
        fpu_ptr_pm      p;
        fpu_ptr_rm      r;
} fpu_ptr;

struct x64_fpu {
    unsigned_32         cw;
    unsigned_32         sw;
    unsigned_32         tag;
    fpu_ptr             ip_err;
    fpu_ptr             op_err;
    xreal               reg[8];
};

typedef struct {
    union {
        unsigned_64     q[1];
        unsigned_32     d[2];
        unsigned_16     w[4];
        unsigned_8      b[8];
    } u;
    unsigned_16         _spacer;
} mmx_reg;

struct x64_mmx {
    unsigned_32         _spacer[7];
    mmx_reg             mm[8];
};

struct x64_xmm {
    xmm_reg             xmm[8];
    unsigned_32         mxcsr;
};

struct x64_mad_registers {
    struct x64_cpu      cpu;
    union {
        struct x64_fpu  fpu;
        struct x64_mmx  mmx;
    } u;
    struct x64_xmm      xmm;
};

#define BIT( name, shift, len ) SHIFT_##name = shift, LEN_##name = len
#define BIT_MXCSR( name, shift, len ) SHIFT_mxcsr_##name = shift, LEN_mxcsr_##name = len
enum {
    /* [E]FL flag bit definitions */
    BIT( c,     0,  1 ),
    BIT( p,     2,  1 ),
    BIT( a,     4,  1 ),
    BIT( z,     6,  1 ),
    BIT( s,     7,  1 ),
    BIT( t,     8,  1 ),
    BIT( i,     9,  1 ),
    BIT( d,     10, 1 ),
    BIT( o,     11, 1 ),
    BIT( iopl,  13, 2 ),
    BIT( nt,    14, 1 ),
    BIT( rf,    16, 1 ),
    BIT( vm,    17, 1 ),

    FLG_C       = 1 << SHIFT_c,
    FLG_P       = 1 << SHIFT_p,
    FLG_A       = 1 << SHIFT_a,
    FLG_Z       = 1 << SHIFT_z,
    FLG_S       = 1 << SHIFT_s,
    FLG_T       = 1 << SHIFT_t,
    FLG_I       = 1 << SHIFT_i,
    FLG_D       = 1 << SHIFT_d,
    FLG_O       = 1 << SHIFT_o,

    /* SW flag bit definitions */
    BIT( ie,    0,  1 ),
    BIT( de,    1,  1 ),
    BIT( ze,    2,  1 ),
    BIT( oe,    3,  1 ),
    BIT( ue,    4,  1 ),
    BIT( pe,    5,  1 ),
    BIT( sf,    6,  1 ),
    BIT( es,    7,  1 ),
    BIT( c0,    8,  1 ),
    BIT( c1,    9,  1 ),
    BIT( c2,    10, 1 ),
    BIT( st,    11, 3 ),
    BIT( c3,    14, 1 ),
    BIT( b,     15, 1 ),

    /* CW flag bit definitions */
    BIT( im,    0,  1 ),
    BIT( dm,    1,  1 ),
    BIT( zm,    2,  1 ),
    BIT( om,    3,  1 ),
    BIT( um,    4,  1 ),
    BIT( pm,    5,  1 ),
    BIT( iem,   7,  1 ),
    BIT( pc,    8,  2 ),
    BIT( rc,    10, 2 ),
    BIT( ic,    12, 1 ),

    /* MXCSR flag bit definitions */
    BIT_MXCSR( ie,    0,  1 ),
    BIT_MXCSR( de,    1,  1 ),
    BIT_MXCSR( ze,    2,  1 ),
    BIT_MXCSR( oe,    3,  1 ),
    BIT_MXCSR( ue,    4,  1 ),
    BIT_MXCSR( pe,    5,  1 ),
    BIT_MXCSR( daz,   6,  1 ),
    BIT_MXCSR( im,    7,  1 ),
    BIT_MXCSR( dm,    8,  1 ),
    BIT_MXCSR( zm,    9,  1 ),
    BIT_MXCSR( om,    10, 1 ),
    BIT_MXCSR( um,    11, 1 ),
    BIT_MXCSR( pm,    12, 1 ),
    BIT_MXCSR( rc,    13, 2 ),
    BIT_MXCSR( fz,    15, 1 )
};

enum {  TAG_VALID       = 0x0,
        TAG_ZERO        = 0x1,
        TAG_INVALID     = 0x2,
        TAG_EMPTY       = 0x3 };

#include "digunpck.h"
#endif
