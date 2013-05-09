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
* Description:  Target processor platform related declarations.
*
****************************************************************************/


#ifndef DISTYPEX_H
#define DISTYPEX_H

#include "watcom.h"
#include "bool.h"

#define DISCPU_none     0x00
#define DISCPU_axp      0x01
#define DISCPU_ppc      0x02
#define DISCPU_x86      0x04
#define DISCPU_jvm      0x08
#define DISCPU_sparc    0x10
#define DISCPU_mips     0x20
#define DISCPU_x64      0x40

#if defined( NDIS_axp )
#define DISCPU DISCPU_axp
#elif defined( NDIS_ppc )
#define DISCPU DISCPU_ppc
#elif defined( NDIS_x86 )
#define DISCPU DISCPU_x86
#elif defined( NDIS_x64 )
#define DISCPU DISCPU_x64
#elif defined( NDIS_jvm )
#define DISCPU DISCPU_jvm
#elif defined( NDIS_sparc )
#define DISCPU DISCPU_sparc
#elif defined( NDIS_mips )
#define DISCPU DISCPU_mips
#elif defined( NDIS_test )
#define DISCPU ( DISCPU_axp | DISCPU_ppc | DISCPU_x86 | DISCPU_jvm | DISCPU_sparc | DISCPU_mips | DISCPU_x64 )
#else
#define DISCPU ( DISCPU_axp | DISCPU_ppc | DISCPU_x86 | DISCPU_sparc | DISCPU_mips | DISCPU_x64 )
#endif

typedef struct dis_range        dis_range;

typedef struct dis_handle       dis_handle;

typedef unsigned_32             dis_opcode;
typedef signed_32               dis_value;
typedef unsigned                dis_cpu;

typedef enum {
    DI_INVALID,
#if DISCPU & DISCPU_axp
    DI_AXP_FIRST,
    DI_AXP_SKIPBACK = DI_AXP_FIRST - 1,
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) DI_AXP_##idx,
    #include "insaxp.h"
#endif
#if DISCPU & DISCPU_ppc
    DI_PPC_FIRST,
    DI_PPC_SKIPBACK = DI_PPC_FIRST - 1,
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) DI_PPC_##idx,
    #include "insppc.h"
#endif
#if DISCPU & DISCPU_x86
    DI_X86_FIRST,
    DI_X86_SKIPBACK = DI_X86_FIRST - 1,
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) DI_X86_##idx,
    #include "insx86.h"
    #include "insx86e1.h"
    #include "insx86e2.h"
    #include "insx86e3.h"
    #include "insx86e4.h"
#endif
#if DISCPU & DISCPU_x64
    DI_X64_FIRST,
    DI_X64_SKIPBACK = DI_X64_FIRST - 1,
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) DI_X64_##idx,
    #include "insx64.h"
#endif
#if DISCPU & DISCPU_jvm
    DI_JVM_FIRST,
    DI_JVM_SKIPBACK = DI_JVM_FIRST - 1,
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) DI_JVM_##idx,
    #include "insjvm.h"
#endif
#if DISCPU & DISCPU_sparc
    DI_SPARC_FIRST,
    DI_SPARC_SKIPBACK = DI_SPARC_FIRST - 1,
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) DI_SPARC_##idx,
    #include "inssparc.h"
#endif
#if DISCPU & DISCPU_mips
    DI_MIPS_FIRST,
    DI_MIPS_SKIPBACK = DI_MIPS_FIRST - 1,
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) DI_MIPS_##idx,
    #include "insmips.h"
#endif
    #undef inspick
    DI_LAST
} dis_inst_type;

typedef enum {
    DR_NONE,
#if DISCPU & DISCPU_axp
    DR_AXP_FIRST,
    DR_AXP_SKIPBACK = DR_AXP_FIRST - 1,
    #undef regpick
    #define regpick( idx, name ) DR_AXP_##idx,
    #include "regaxp.h"
#endif
#if DISCPU & DISCPU_ppc
    DR_PPC_FIRST,
    DR_PPC_SKIPBACK = DR_PPC_FIRST - 1,
    #undef regpick
    #define regpick( idx, name ) DR_PPC_##idx,
    #include "regppc.h"
#endif
#if DISCPU & DISCPU_x86
    DR_X86_FIRST,
    DR_X86_SKIPBACK = DR_X86_FIRST - 1,
    #undef regpick
    #define regpick( idx, name ) DR_X86_##idx,
    #include "regx86.h"
#endif
#if DISCPU & DISCPU_x64
    DR_X64_FIRST,
    DR_X64_SKIPBACK = DR_X64_FIRST - 1,
    #undef regpick
    #define regpick( idx, name ) DR_X64_##idx,
    #include "regx64.h"
#endif
#if DISCPU & DISCPU_jvm
    DR_JVM_FIRST,
    DR_JVM_SKIPBACK = DR_JVM_FIRST - 1,
    #undef regpick
    #define regpick( idx, name ) DR_JVM_##idx,
    #include "regjvm.h"
#endif
#if DISCPU & DISCPU_sparc
    DR_SPARC_FIRST,
    DR_SPARC_SKIPBACK = DR_SPARC_FIRST - 1,
    #undef regpick
    #define regpick( idx, name ) DR_SPARC_##idx,
    #include "regsparc.h"
#endif
#if DISCPU & DISCPU_mips
    DR_MIPS_FIRST,
    DR_MIPS_SKIPBACK = DR_MIPS_FIRST - 1,
    #undef regpick
    #define regpick( idx, name ) DR_MIPS_##idx,
    #include "regmips.h"
#endif
    #undef regpick
    DR_LAST
} dis_register;

typedef enum {
    DRT_NONE,
#if DISCPU & DISCPU_axp
    DRT_AXP_FIRST,
    DRT_AXP_SKIPBACK = DRT_AXP_FIRST - 1,
    #undef refpick
    #define refpick( idx, name ) DRT_AXP_##idx,
    #include "refaxp.h"
#endif
#if DISCPU & DISCPU_ppc
    DRT_PPC_FIRST,
    DRT_PPC_SKIPBACK = DRT_PPC_FIRST - 1,
    #undef refpick
    #define refpick( idx, name ) DRT_PPC_##idx,
    #include "refppc.h"
#endif
#if DISCPU & DISCPU_x86
    DRT_X86_FIRST,
    DRT_X86_SKIPBACK = DRT_X86_FIRST - 1,
    #undef refpick
    #define refpick( idx, name ) DRT_X86_##idx,
    #include "refx86.h"
#endif
#if DISCPU & DISCPU_x64
    DRT_X64_FIRST,
    DRT_X64_SKIPBACK = DRT_X64_FIRST - 1,
    #undef refpick
    #define refpick( idx, name ) DRT_X64_##idx,
    #include "refx64.h"
#endif
#if DISCPU & DISCPU_jvm
    DRT_JVM_FIRST,
    DRT_JVM_SKIPBACK = DRT_JVM_FIRST - 1,
    #undef refpick
    #define refpick( idx, name ) DRT_JVM_##idx,
    #include "refjvm.h"
#endif
#if DISCPU & DISCPU_sparc
    DRT_SPARC_FIRST,
    DRT_SPARC_SKIPBACK = DRT_SPARC_FIRST - 1,
    #undef refpick
    #define refpick( idx, name ) DRT_SPARC_##idx,
    #include "refsparc.h"
#endif
#if DISCPU & DISCPU_mips
    DRT_MIPS_FIRST,
    DRT_MIPS_SKIPBACK = DRT_MIPS_FIRST - 1,
    #undef refpick
    #define refpick( idx, name ) DRT_MIPS_##idx,
    #include "refmips.h"
#endif
    #undef refpick
    DRT_LAST
} dis_ref_type;

#if DISCPU & DISCPU_axp
typedef enum {
    DIF_AXP_C           = 0x01<<0,
    DIF_AXP_D           = 0x01<<1,
    DIF_AXP_I           = 0x01<<2,
    DIF_AXP_M           = 0x01<<3,
    DIF_AXP_S           = 0x01<<4,
    DIF_AXP_U           = 0x01<<5,
    DIF_AXP_V           = 0x01<<6,
} dis_inst_flags_axp;
#endif

#if DISCPU & DISCPU_ppc
typedef enum {
    DIF_PPC_OE          = 0x01<<0,
    DIF_PPC_RC          = 0x01<<1,
    DIF_PPC_AA          = 0x01<<2,
    DIF_PPC_LK          = 0x01<<3,
} dis_inst_flags_ppc;
#endif

#if DISCPU & DISCPU_x86
typedef enum {
    DIF_X86_CS          = 0x0001,
    DIF_X86_DS          = 0x0002,
    DIF_X86_ES          = 0x0004,
    DIF_X86_FS          = 0x0008,
    DIF_X86_GS          = 0x0010,
    DIF_X86_SS          = 0x0020,
    DIF_X86_LOCK        = 0x0040,
    DIF_X86_REPE        = 0x0080,
    DIF_X86_REPNE       = 0x0100,
    DIF_X86_FWAIT       = 0x0200,
    DIF_X86_OPND_SIZE   = 0x0400,
    DIF_X86_ADDR_SIZE   = 0x0800,
    DIF_X86_EMU_INT     = 0x1000,       /* not a prefix */
    // Current Operating Mode
    // Use32 Operands
    DIF_X86_OPND_LONG   = 0x2000,       /* not a prefix */
    // Use32 Address
    DIF_X86_ADDR_LONG   = 0x4000,       /* not a prefix */
    DIF_X86_FP_INS      = 0x8000,       /* not a prefix */
    DIF_X86_USE16_FLAGS = 0,
    DIF_X86_USE32_FLAGS = DIF_X86_OPND_LONG|DIF_X86_ADDR_LONG,
} dis_inst_flags_x86;
#endif

#if DISCPU & DISCPU_x64
typedef enum {
    DIF_X64_CS          = 0x0001,
    DIF_X64_DS          = 0x0002,
    DIF_X64_ES          = 0x0004,
    DIF_X64_FS          = 0x0008,
    DIF_X64_GS          = 0x0010,
    DIF_X64_SS          = 0x0020,
    DIF_X64_LOCK        = 0x0040,
    DIF_X64_REPE        = 0x0080,
    DIF_X64_REPNE       = 0x0100,
    DIF_X64_OPND_SIZE   = 0x0200,
    DIF_X64_ADDR_SIZE   = 0x0400,
    DIF_X64_PEX_PR      = 0x0800,
    DIF_X64_REX_B       = 0x1000,      /* REX prefixes */
    DIF_X64_REX_X       = 0x2000,
    DIF_X64_REX_R       = 0x4000,
    DIF_X64_REX_W       = 0x8000,
} dis_inst_flags_x64;
#endif

#if DISCPU & DISCPU_jvm
typedef enum {
    DIF_JVM_WIDE        = 0x01<<0,
    DIF_JVM_QUICK       = 0x01<<1,
} dis_inst_flags_jvm;
#endif

#if DISCPU & DISCPU_sparc
typedef enum {
    DIF_SPARC_ANUL      = 0x01<<0,      /* for branch instructions - next ins anul'd */
} dis_inst_flags_sparc;
#endif

#if DISCPU & DISCPU_mips
typedef enum {
    DIF_MIPS_NULLIFD    = 0x0001,       /* for branch instructions - next ins nullified */
    DIF_MIPS_LINK       = 0x0002,       /* linked jump/branch (ie. a call) */
    DIF_MIPS_LIKELY     = 0x0004,       /* branch instruction likely */
    DIF_MIPS_FF_S       = 0x0008,       /* single precision floating point */
    DIF_MIPS_FF_D       = 0x0010,       /* double precision floating point */
    DIF_MIPS_FF_W       = 0x0020,       /* fixed point, word sized */
    DIF_MIPS_FF_L       = 0x0040,       /* fixed point, doubleword sized */
    DIF_MIPS_FF_PS      = 0x0080,       /* paired single precision floating point */
    DIF_MIPS_FF_FLAGS   = DIF_MIPS_FF_S | DIF_MIPS_FF_D | DIF_MIPS_FF_W | DIF_MIPS_FF_L | DIF_MIPS_FF_PS,
} dis_inst_flags_mips;
#endif

typedef enum {
    DIF_NONE            = 0,
    DIF_ALL             = 0xFFFF
} dis_inst_flags_all;

typedef struct {
    union {
#if DISCPU & DISCPU_axp
        dis_inst_flags_axp  axp;
#endif
#if DISCPU & DISCPU_ppc
        dis_inst_flags_ppc  ppc;
#endif
#if DISCPU & DISCPU_x86
        dis_inst_flags_x86  x86;
#endif
#if DISCPU & DISCPU_x64
        dis_inst_flags_x64  x64;
#endif
#if DISCPU & DISCPU_jvm
        dis_inst_flags_jvm  jvm;
#endif
#if DISCPU & DISCPU_sparc
        dis_inst_flags_sparc    sparc;
#endif
#if DISCPU & DISCPU_mips
        dis_inst_flags_mips mips;
#endif
        dis_inst_flags_all  all;
    } u;
} dis_inst_flags;

typedef enum {
    DO_NONE,
    DO_REG,
    DO_IMMED,
    DO_ABSOLUTE,
    DO_RELATIVE,
    DO_MEMORY_ABS,
    DO_MEMORY_REL,
    DO_MASK             = 0x0f,
    DO_NO_SEG_OVR       = 0x20,
    DO_EXTRA            = 0x40,
    DO_HIDDEN           = 0x80
} dis_operand_type;

typedef enum {
    PE_XFORM    = 0x01
} ppc_extra_flags;

typedef struct {
    dis_value           value;
    dis_register        base;
    dis_register        index;
    unsigned_8          scale;
    unsigned_8          op_position;
    unsigned_16         extra;
    dis_ref_type        ref_type;
    dis_operand_type    type;
} dis_operand;

#define MAX_NUM_OPERANDS 5

typedef struct {
    dis_opcode          opcode;
    dis_inst_flags      flags;
    dis_inst_type       type;
    unsigned_8          size;
    unsigned_8          num_ops;
    dis_operand         op[MAX_NUM_OPERANDS];
} dis_dec_ins;

typedef enum {
    DFF_NONE                    = 0,
    DFF_INS_UP                  = 0x01<<0,
    DFF_REG_UP                  = 0x01<<1,
    DFF_ASM                     = 0x01<<2,
    DFF_PSEUDO                  = 0x01<<3,
    DFF_SYMBOLIC_REG            = 0x01<<4,
    DFF_ALT_INDEXING            = 0x01<<5,
    DFF_UNIX                    = 0x01<<6,
    DFF_DONE
} dis_format_flags;

typedef enum {
    DR_OK,
    DR_FAIL,
    DR_INVALID
} dis_return;

typedef struct  dis_cpu_data    dis_cpu_data;

struct dis_handle {
    unsigned            cpu;
    const dis_cpu_data  *d;
    bool                need_bswap;
};

#endif

