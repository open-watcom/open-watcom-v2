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
* Description:  Flags for controlling code generation, set by compiler
*               front ends into GenSwitches.
*
****************************************************************************/


#ifndef _CG_SWITCH_INCLUDED

#define NO_OPTIMIZATION         0x80000000L
#define NUMBERS                 0x40000000L
#define FORTRAN_ALIASING        0x20000000L

#define DBG_DF                  0x10000000L
#define DBG_CV                  0x08000000L

#define RELAX_ALIAS             0x04000000L
#define DBG_LOCALS              0x02000000L
#define DBG_TYPES               0x01000000L
#define LOOP_UNROLLING          0x00800000L
#define LOOP_OPTIMIZATION       0x00400000L
#define INS_SCHEDULING          0x00200000L
#define MEMORY_LOW_FAILS        0x00100000L
#define FP_UNSTABLE_OPTIMIZATION 0x0080000L
#define NULL_DEREF_OK           0x00040000L
#define DBG_PREDEF              0x00020000L
#define BRANCH_PREDICTION       0x00010000L
#define FLOW_REG_SAVES          0x00008000L

#define FPU_ROUNDING_INLINE     0x00004000L
#define FPU_ROUNDING_OMIT       0x00002000L

#define SUPER_OPTIMAL           0x00001000L
#define ECHO_API_CALLS          0x00000800L
#define MICROSOFT_COMPATIBLE    0x00000400L
#define POSITION_INDEPENDANT    0x00000200L
#define DLL_RESIDENT_CODE       0x00000100L

#define OBJ_ELF                 0x00000080L
#define OBJ_COFF                0x00000040L
#define OBJ_OWL                 (OBJ_COFF | OBJ_ELF)
#define OBJ_ENDIAN_BIG          0x00000020L

#define _TARG_CGSWITCH_SHIFT     0

#include "cgtargsw.h"

typedef unsigned                cg_switches;
typedef unsigned                cg_target_switches;

#define _CG_SWITCH_INCLUDED
#endif
