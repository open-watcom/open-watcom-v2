/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef DIGCPU_H_INCLUDED
#define DIGCPU_H_INCLUDED

/*
 * CPU related definitions
 */

typedef enum {
    X86_86,
    X86_186,
    X86_286,
    X86_386,
    X86_486,
    X86_586,
    X86_686,
    X86_P4          = 15,
    X86_CPU_MASK    = 0x0f,
    X86_MMX         = 0x10,
    X86_XMM         = 0x20
} x86_cputypes;

typedef enum {
    X64_CPU1        = 1
} x64_cputypes;

typedef enum {
    AXP_DUNNO,
    AXP_21064,
    AXP_21164
} axp_cputypes;

typedef enum {
    MIPS_DUNNO,
    MIPS_R2000,
    MIPS_R3000,
    MIPS_R4000,
    MIPS_R5000
} mips_cputypes;

typedef enum {
    PPC_DUNNO,
    PPC_601,
    PPC_603,
    PPC_604,
    PPC_620
} ppc_cputypes;

typedef union dig_cputypes {
    unsigned char       byte;
    x86_cputypes        x86;
    x64_cputypes        x64;
    axp_cputypes        axp;
    ppc_cputypes        ppc;
    mips_cputypes       mips;
//    jvm_cputypes        jvm;
} dig_cputypes;

/*
 * FPU related definitions
 */

typedef enum {
    X86_NOFPU,
    X86_87,
    X86_287,
    X86_387,
    X86_487,
    X86_587,
    X86_687,
    X86_P47         = 15,
    X86_EMU         = 255
} x86_fputypes;

typedef enum {
    X64_FPU1        = 1
} x64_fputypes;

typedef union dig_fputypes {
    unsigned char   byte;
    x86_fputypes    x86;
    x64_fputypes    x64;
//    axp_fputypes    axp;
//    ppc_fputypes    ppc;
//    mips_fputypes   mips;
//    jvm_fputypes    jvm;
} dig_fputypes;

#endif
