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
    CGSW_GEN_OBJ_ENDIAN_BIG           = 0x00000001,
    CGSW_GEN_OBJ_COFF                 = 0x00000002,
    CGSW_GEN_OBJ_ELF                  = 0x00000004,
    CGSW_GEN_DLL_RESIDENT_CODE        = 0x00000008,
    CGSW_GEN_POSITION_INDEPENDANT     = 0x00000010,
    CGSW_GEN_MICROSOFT_COMPATIBLE     = 0x00000020,
    CGSW_GEN_ECHO_API_CALLS           = 0x00000040,
    CGSW_GEN_SUPER_OPTIMAL            = 0x00000080,
    CGSW_GEN_FPU_ROUNDING_OMIT        = 0x00000100,
    CGSW_GEN_FPU_ROUNDING_INLINE      = 0x00000200,
    CGSW_GEN_FLOW_REG_SAVES           = 0x00000400,
    CGSW_GEN_BRANCH_PREDICTION        = 0x00000800,
    CGSW_GEN_DBG_PREDEF               = 0x00001000,
    CGSW_GEN_NULL_DEREF_OK            = 0x00002000,
    CGSW_GEN_FP_UNSTABLE_OPTIMIZATION = 0x00004000,
    CGSW_GEN_MEMORY_LOW_FAILS         = 0x00008000,
    CGSW_GEN_INS_SCHEDULING           = 0x00010000,
    CGSW_GEN_LOOP_OPTIMIZATION        = 0x00020000,
    CGSW_GEN_LOOP_UNROLLING           = 0x00040000,
    CGSW_GEN_DBG_TYPES                = 0x00080000,
    CGSW_GEN_DBG_LOCALS               = 0x00100000,
    CGSW_GEN_RELAX_ALIAS              = 0x00200000,
    CGSW_GEN_DBG_CV                   = 0x00400000,
    CGSW_GEN_DBG_DF                   = 0x00800000,
    CGSW_GEN_FORTRAN_ALIASING         = 0x01000000,
    CGSW_GEN_DBG_NUMBERS              = 0x02000000,
    CGSW_GEN_NO_OPTIMIZATION          = 0x04000000,
    CGSW_GEN_I_MATH_INLINE            = 0x08000000,
    CGSW_GEN_NO_CALL_RET_TRANSFORM    = 0x10000000,
} cg_switches;

#define CGSW_GEN_OBJ_OWL        (CGSW_GEN_OBJ_COFF | CGSW_GEN_OBJ_ELF)

#endif
