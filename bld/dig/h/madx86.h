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


#ifndef MADX86_H
#define MADX86_H
#include "digpck.h"
enum x86_cputypes {
        X86_86,
        X86_186,
        X86_286,
        X86_386,
        X86_486,
        X86_586,
        X86_686
};

enum x86_fputypes {
        X86_NO,
        X86_87,
        X86_287,
        X86_387,
        X86_487,
        X86_587,
        X86_687,
        X86_EMU = (unsigned_8)-1
};

enum x86_machine_data {
    X86MD_ADDR_CHARACTERISTICS
};

enum x86_addr_characteristics {
    X86AC_BIG   = 0x01,
    X86AC_REAL  = 0x02
};

struct x86_cpu {
    unsigned_32 eax;
    unsigned_32 ebx;
    unsigned_32 ecx;
    unsigned_32 edx;
    unsigned_32 esi;
    unsigned_32 edi;
    unsigned_32 ebp;
    unsigned_32 esp;
    unsigned_32 eip;
    unsigned_32 efl;
    unsigned_32 cr0;
    unsigned_32 cr2;
    unsigned_32 cr3;
    unsigned_16 ds;
    unsigned_16 es;
    unsigned_16 ss;
    unsigned_16 cs;
    unsigned_16 fs;
    unsigned_16 gs;
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

struct x86_fpu {
    unsigned_32         cw;
    unsigned_32         sw;
    unsigned_32         tag;
    fpu_ptr             ip_err;
    fpu_ptr             op_err;
    xreal               reg[8];
};

typedef struct {
    union {
        unsigned_32             ud[2];
        unsigned_16             uw[4];
        unsigned_8              ub[8];
    };
    unsigned_16                 _spacer;
} mmx_reg;

struct x86_mmx {
    unsigned_32         _spacer[7];
    mmx_reg             mm[8];
};

struct x86_mad_registers {
    struct x86_cpu      cpu;
    union {
        struct x86_fpu  fpu;
        struct x86_mmx  mmx;
    };
};

#define BIT( name, shift, len ) SHIFT_##name = shift, LEN_##name = len
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
    BIT( ic,    12, 1 )
};

enum {  TAG_VALID       = 0x0,
        TAG_ZERO        = 0x1,
        TAG_INVALID     = 0x2,
        TAG_EMPTY       = 0x3 };

#include "digunpck.h"
#endif
