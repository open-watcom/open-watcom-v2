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
    OBJ_ENDIAN_BIG           = 0x00000001L,
    OBJ_COFF                 = 0x00000002L,
    OBJ_ELF                  = 0x00000004L,
    DLL_RESIDENT_CODE        = 0x00000008L,
    POSITION_INDEPENDANT     = 0x00000010L,
    MICROSOFT_COMPATIBLE     = 0x00000020L,
    ECHO_API_CALLS           = 0x00000040L,
    SUPER_OPTIMAL            = 0x00000080L,
    FPU_ROUNDING_OMIT        = 0x00000100L,
    FPU_ROUNDING_INLINE      = 0x00000200L,
    FLOW_REG_SAVES           = 0x00000400L,
    BRANCH_PREDICTION        = 0x00000800L,
    DBG_PREDEF               = 0x00001000L,
    NULL_DEREF_OK            = 0x00002000L,
    FP_UNSTABLE_OPTIMIZATION = 0x00004000L,
    MEMORY_LOW_FAILS         = 0x00008000L,
    INS_SCHEDULING           = 0x00010000L,
    LOOP_OPTIMIZATION        = 0x00020000L,
    LOOP_UNROLLING           = 0x00040000L,
    DBG_TYPES                = 0x00080000L,
    DBG_LOCALS               = 0x00100000L,
    RELAX_ALIAS              = 0x00200000L,
    DBG_CV                   = 0x00400000L,
    DBG_DF                   = 0x00800000L,
    FORTRAN_ALIASING         = 0x01000000L,
    DBG_NUMBERS              = 0x02000000L,
    NO_OPTIMIZATION          = 0x04000000L,
} cg_switches;

#define OBJ_OWL             (OBJ_COFF | OBJ_ELF)

#endif
