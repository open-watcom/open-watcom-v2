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
* Description:  Flags for controlling code generation, set by compiler
*               front ends into GenSwitches.
*
****************************************************************************/


#ifndef _CG_SWITCH_INCLUDED
#define _CG_SWITCH_INCLUDED

#include "cgtargsw.h"

typedef enum {
    CGSW_OBJ_ENDIAN_BIG           = 0x00000001L,
    CGSW_OBJ_COFF                 = 0x00000002L,
    CGSW_OBJ_ELF                  = 0x00000004L,
    CGSW_DLL_RESIDENT_CODE        = 0x00000008L,
    CGSW_POSITION_INDEPENDANT     = 0x00000010L,
    CGSW_MICROSOFT_COMPATIBLE     = 0x00000020L,
    CGSW_ECHO_API_CALLS           = 0x00000040L,
    CGSW_SUPER_OPTIMAL            = 0x00000080L,
    CGSW_FPU_ROUNDING_OMIT        = 0x00000100L,
    CGSW_FPU_ROUNDING_INLINE      = 0x00000200L,
    CGSW_FLOW_REG_SAVES           = 0x00000400L,
    CGSW_BRANCH_PREDICTION        = 0x00000800L,
    CGSW_DBG_PREDEF               = 0x00001000L,
    CGSW_NULL_DEREF_OK            = 0x00002000L,
    CGSW_FP_UNSTABLE_OPTIMIZATION = 0x00004000L,
    CGSW_MEMORY_LOW_FAILS         = 0x00008000L,
    CGSW_INS_SCHEDULING           = 0x00010000L,
    CGSW_LOOP_OPTIMIZATION        = 0x00020000L,
    CGSW_LOOP_UNROLLING           = 0x00040000L,
    CGSW_DBG_TYPES                = 0x00080000L,
    CGSW_DBG_LOCALS               = 0x00100000L,
    CGSW_RELAX_ALIAS              = 0x00200000L,
    CGSW_DBG_CV                   = 0x00400000L,
    CGSW_DBG_DF                   = 0x00800000L,
    CGSW_FORTRAN_ALIASING         = 0x01000000L,
    CGSW_DBG_NUMBERS              = 0x02000000L,
    CGSW_NO_OPTIMIZATION          = 0x04000000L,
} cg_switches;

#define CGSW_OBJ_OWL        (CGSW_OBJ_COFF | CGSW_OBJ_ELF)

#endif
