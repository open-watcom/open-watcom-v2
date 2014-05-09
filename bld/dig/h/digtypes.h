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
* Description:  Debugger interface types.
*
****************************************************************************/


#ifndef DIGTYPES_H_INCLUDED
#define DIGTYPES_H_INCLUDED

#include <stddef.h>
#include "machtype.h"

#if defined( __WATCOMC__ ) && defined( _M_I86 )
    #define DIGFAR      __far
#else
    #define DIGFAR
#endif

#if defined( __WINDOWS__ )
    #define DIGREGISTER     DIGFAR __pascal
    #define DIGENTRY        DIGFAR __pascal
    #define DIGCLIENT       __loadds
#else
    #define DIGREGISTER     DIGFAR
    #define DIGENTRY        DIGFAR
    #define DIGCLIENT
#endif

#if defined( __NT__ ) || defined( __OS2__ ) || defined( __RDOS__ )
    #define DIG_DLLEXPORT   __declspec(dllexport)
#else
    #define DIG_DLLEXPORT
#endif

#define DIG_NIL_HANDLE      ( (dig_fhandle) -1 )

typedef unsigned_8 search_result; enum {
    SR_NONE,
    SR_EXACT,
    SR_CLOSEST,
    SR_FAIL,
    SR_LAST
};

typedef unsigned_8 walk_result; enum {
    WR_CONTINUE,
    WR_STOP,
    WR_FAIL,
    WR_LAST
};

typedef unsigned context_item; enum {
    /* common */
    CI_FRAME,
    CI_STACK,
    CI_EXECUTION,
    CI_OBJECT,
    CI_DEF_ADDR_SPACE,
    /* x86 architecture */
    CI_EAX,
    CI_EBX,
    CI_ECX,
    CI_EDX,
    CI_ESI,
    CI_EDI,
    CI_EBP,
    CI_ESP,
    CI_EFL,
    CI_EIP,
    CI_DS,
    CI_ES,
    CI_SS,
    CI_CS,
    CI_FS,
    CI_GS,
    CI_ST0,
    CI_ST1,
    CI_ST2,
    CI_ST3,
    CI_ST4,
    CI_ST5,
    CI_ST6,
    CI_ST7,
    CI_SW,
    CI_CW,
    /* Alpha architecture */
    CI_AXP_r0,
    CI_AXP_r1,
    CI_AXP_r2,
    CI_AXP_r3,
    CI_AXP_r4,
    CI_AXP_r5,
    CI_AXP_r6,
    CI_AXP_r7,
    CI_AXP_r8,
    CI_AXP_r9,
    CI_AXP_r10,
    CI_AXP_r11,
    CI_AXP_r12,
    CI_AXP_r13,
    CI_AXP_r14,
    CI_AXP_r15,
    CI_AXP_r16,
    CI_AXP_r17,
    CI_AXP_r18,
    CI_AXP_r19,
    CI_AXP_r20,
    CI_AXP_r21,
    CI_AXP_r22,
    CI_AXP_r23,
    CI_AXP_r24,
    CI_AXP_r25,
    CI_AXP_r26,
    CI_AXP_r27,
    CI_AXP_r28,
    CI_AXP_r29,
    CI_AXP_r30,
    CI_AXP_r31,
    CI_AXP_f0,
    CI_AXP_f1,
    CI_AXP_f2,
    CI_AXP_f3,
    CI_AXP_f4,
    CI_AXP_f5,
    CI_AXP_f6,
    CI_AXP_f7,
    CI_AXP_f8,
    CI_AXP_f9,
    CI_AXP_f10,
    CI_AXP_f11,
    CI_AXP_f12,
    CI_AXP_f13,
    CI_AXP_f14,
    CI_AXP_f15,
    CI_AXP_f16,
    CI_AXP_f17,
    CI_AXP_f18,
    CI_AXP_f19,
    CI_AXP_f20,
    CI_AXP_f21,
    CI_AXP_f22,
    CI_AXP_f23,
    CI_AXP_f24,
    CI_AXP_f25,
    CI_AXP_f26,
    CI_AXP_f27,
    CI_AXP_f28,
    CI_AXP_f29,
    CI_AXP_f30,
    CI_AXP_f31,
    CI_AXP_fir,
    /* PowerPC architecture */
    CI_PPC_r0,
    CI_PPC_r1,
    CI_PPC_r2,
    CI_PPC_r3,
    CI_PPC_r4,
    CI_PPC_r5,
    CI_PPC_r6,
    CI_PPC_r7,
    CI_PPC_r8,
    CI_PPC_r9,
    CI_PPC_r10,
    CI_PPC_r11,
    CI_PPC_r12,
    CI_PPC_r13,
    CI_PPC_r14,
    CI_PPC_r15,
    CI_PPC_r16,
    CI_PPC_r17,
    CI_PPC_r18,
    CI_PPC_r19,
    CI_PPC_r20,
    CI_PPC_r21,
    CI_PPC_r22,
    CI_PPC_r23,
    CI_PPC_r24,
    CI_PPC_r25,
    CI_PPC_r26,
    CI_PPC_r27,
    CI_PPC_r28,
    CI_PPC_r29,
    CI_PPC_r30,
    CI_PPC_r31,
    CI_PPC_f0,
    CI_PPC_f1,
    CI_PPC_f2,
    CI_PPC_f3,
    CI_PPC_f4,
    CI_PPC_f5,
    CI_PPC_f6,
    CI_PPC_f7,
    CI_PPC_f8,
    CI_PPC_f9,
    CI_PPC_f10,
    CI_PPC_f11,
    CI_PPC_f12,
    CI_PPC_f13,
    CI_PPC_f14,
    CI_PPC_f15,
    CI_PPC_f16,
    CI_PPC_f17,
    CI_PPC_f18,
    CI_PPC_f19,
    CI_PPC_f20,
    CI_PPC_f21,
    CI_PPC_f22,
    CI_PPC_f23,
    CI_PPC_f24,
    CI_PPC_f25,
    CI_PPC_f26,
    CI_PPC_f27,
    CI_PPC_f28,
    CI_PPC_f29,
    CI_PPC_f30,
    CI_PPC_f31,
    CI_PPC_lr,
    CI_PPC_ctr,
    CI_PPC_iar,
    CI_PPC_msr,
    CI_PPC_cr,
    CI_PPC_xer,
    CI_PPC_fpscr,
    /* MIPS architecture */
    CI_MIPS_r0,
    CI_MIPS_r1,
    CI_MIPS_r2,
    CI_MIPS_r3,
    CI_MIPS_r4,
    CI_MIPS_r5,
    CI_MIPS_r6,
    CI_MIPS_r7,
    CI_MIPS_r8,
    CI_MIPS_r9,
    CI_MIPS_r10,
    CI_MIPS_r11,
    CI_MIPS_r12,
    CI_MIPS_r13,
    CI_MIPS_r14,
    CI_MIPS_r15,
    CI_MIPS_r16,
    CI_MIPS_r17,
    CI_MIPS_r18,
    CI_MIPS_r19,
    CI_MIPS_r20,
    CI_MIPS_r21,
    CI_MIPS_r22,
    CI_MIPS_r23,
    CI_MIPS_r24,
    CI_MIPS_r25,
    CI_MIPS_r26,
    CI_MIPS_r27,
    CI_MIPS_r28,
    CI_MIPS_r29,
    CI_MIPS_r30,
    CI_MIPS_r31,
    CI_MIPS_f0,
    CI_MIPS_f1,
    CI_MIPS_f2,
    CI_MIPS_f3,
    CI_MIPS_f4,
    CI_MIPS_f5,
    CI_MIPS_f6,
    CI_MIPS_f7,
    CI_MIPS_f8,
    CI_MIPS_f9,
    CI_MIPS_f10,
    CI_MIPS_f11,
    CI_MIPS_f12,
    CI_MIPS_f13,
    CI_MIPS_f14,
    CI_MIPS_f15,
    CI_MIPS_f16,
    CI_MIPS_f17,
    CI_MIPS_f18,
    CI_MIPS_f19,
    CI_MIPS_f20,
    CI_MIPS_f21,
    CI_MIPS_f22,
    CI_MIPS_f23,
    CI_MIPS_f24,
    CI_MIPS_f25,
    CI_MIPS_f26,
    CI_MIPS_f27,
    CI_MIPS_f28,
    CI_MIPS_f29,
    CI_MIPS_f30,
    CI_MIPS_f31,
    CI_MIPS_pc,
    CI_MIPS_lo,
    CI_MIPS_hi,
    /* Java Virtual Machine architecture */
    CI_JVM_pc,
    CI_JVM_vars,
    CI_JVM_optop,
    CI_JVM_frame,
    CI_LAST
};

typedef unsigned_8 type_kind; enum {
    TK_NONE,
    TK_DATA,
    TK_CODE,
    TK_ADDRESS,
    TK_VOID,
    TK_BOOL,
    TK_ENUM,
    TK_CHAR,
    TK_INTEGER,
    TK_REAL,
    TK_COMPLEX,
    TK_STRING,
    TK_POINTER,
    TK_STRUCT,
    TK_ARRAY,
    TK_FUNCTION,
    TK_NAMESPACE,
    TK_LAST
};

typedef unsigned_8 type_modifier; enum {
    TM_NONE = 0,

    /* for pointer/address types */
    TM_NEAR = 1,
    TM_FAR,
    TM_HUGE,

    /* for integer types */
    TM_SIGNED = 1,
    TM_UNSIGNED,

    /* for floating point types */
    TM_IEEE = 1,
    TM_VAX1,    /* F, G floating */
    TM_VAX2,    /* D floating */

    /* for string types */
    TM_ASCII = 1,
    TM_EBCIDIC,
    TM_UNICODE,

    TM_MOD_MASK         = 0x0f,
    TM_FLAG_DEREF       = 0x10
};

#include "digpck.h"

typedef struct dip_type_info {
    unsigned long       size;
    type_kind           kind;
    type_modifier       modifier;
} dip_type_info;

typedef         unsigned dig_fhandle;
typedef         unsigned_8 dig_seek; enum {
    DIG_ORG,
    DIG_CUR,
    DIG_END
};

typedef         unsigned_8 dig_open; enum {
    DIG_READ        = 0x01,
    DIG_WRITE       = 0x02,
    DIG_CREATE      = 0x04,
    DIG_TRUNC       = 0x08,
    DIG_APPEND      = 0x10,
    DIG_REMOTE      = 0x20,
    DIG_LOCAL       = 0x40,
    DIG_SEARCH      = 0x80
};

enum archtypes {
    MAD_NIL,
    #define pick_mad(enum,file,desc) enum,
    #include "madarch.h"
    #undef pick_mad
    MAD_MAX
};
typedef unsigned_16             mad_handle;

enum ostypes {                  //NYI: redo these for PIL
    #define pick_mad(enum,desc) enum,
    #include "mados.h"
    #undef pick_mad
    MAD_OS_MAX
};

typedef struct {                //NYI: redo this for PIL
    unsigned_8          cpu;
    unsigned_8          fpu;
    unsigned_8          osmajor;
    unsigned_8          osminor;
    unsigned_8          os;
    unsigned_8          huge_shift;
    mad_handle          mad;
} system_config;

enum {
    MAP_FLAT_CODE_SELECTOR      = (unsigned_16)-1,
    MAP_FLAT_DATA_SELECTOR      = (unsigned_16)-2,
};

#include "digunpck.h"
#endif
