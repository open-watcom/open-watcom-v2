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
* Description:  MAD CPU registers structure.
*
****************************************************************************/


#ifndef MADREGS_H_INCLUDED
#define MADREGS_H_INCLUDED

#include "madtypes.h"

#ifdef MD_x86
#include "madx86.h"
#endif

#ifdef MD_axp
#include "madaxp.h"
#endif

#ifdef MD_ppc
#include "madppc.h"
#endif

#ifdef MD_mips
#include "madmips.h"
#endif

#ifdef MD_jvm
#include "madjvm.h"
#endif

#include "digpck.h"

union mad_registers {
    unsigned_8                  nul;
#ifdef MD_x86
    struct x86_mad_registers    x86;
#endif
#ifdef MD_axp
    struct axp_mad_registers    axp;
#endif
#ifdef MD_ppc
    struct ppc_mad_registers    ppc;
#endif
#ifdef MD_mips
    struct mips_mad_registers   mips;
#endif
#ifdef MD_jvm
    struct jvm_mad_registers    jvm;
#endif
};

#include "digunpck.h"

#endif
