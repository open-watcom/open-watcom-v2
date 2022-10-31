/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  MAD CPU registers structure.
*
****************************************************************************/


#ifndef MADREGS_H_INCLUDED
#define MADREGS_H_INCLUDED

#include "madconf.h"
#include "madtypes.h"

#if MADARCH & (MADARCH_X86 | MADARCH_X64)
    #include "madx86.h"
#endif

#if MADARCH & MADARCH_X64
//    #include "madx64.h"
#endif

#if MADARCH & MADARCH_AXP
    #include "madaxp.h"
#endif

#if MADARCH & MADARCH_PPC
    #include "madppc.h"
#endif

#if MADARCH & MADARCH_MIPS
    #include "madmips.h"
#endif

#if MADARCH & MADARCH_JVM
    #include "madjvm.h"
#endif

#include "digpck.h"

union mad_registers {
    unsigned_8                  nul;
#if MADARCH & (MADARCH_X86 | MADARCH_X64)
    struct x86_mad_registers    x86;
#endif
#if MADARCH & MADARCH_X64
//    struct x64_mad_registers    x64;
#endif
#if MADARCH & MADARCH_AXP
    struct axp_mad_registers    axp;
#endif
#if MADARCH & MADARCH_PPC
    struct ppc_mad_registers    ppc;
#endif
#if MADARCH & MADARCH_MIPS
    struct mips_mad_registers   mips;
#endif
#if MADARCH & MADARCH_JVM
    struct jvm_mad_registers    jvm;
#endif
};

typedef union machine_data_spec {
    unsigned_8                  nul;
#if MADARCH & (MADARCH_X86 | MADARCH_X64)
    x86_addrflags               x86_addr_flags;
#endif
#if MADARCH & MADARCH_X64
//    x64_addrflags               x64_addr_flags;
#endif
#if MADARCH & MADARCH_AXP
    axp_pdata_struct            axp_pdata;
#endif
#if MADARCH & MADARCH_PPC
//    ppc_pdata_struct            ppc_pdata;
#endif
#if MADARCH & MADARCH_MIPS
//    mips_pdata_struct           mips_pdata;
#endif
#if MADARCH & MADARCH_JVM
//    jvm_pdata_struct            jvm_pdata;
#endif
} machine_data_spec;

#include "digunpck.h"

#endif
