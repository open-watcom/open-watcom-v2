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
* Description:  MIPS register numbers.
*
****************************************************************************/


typedef enum mips_regn {
    MIPS_REGN_r0,
    MIPS_REGN_r1,
    MIPS_REGN_r2,
    MIPS_REGN_r3,
    MIPS_REGN_r4,
    MIPS_REGN_r5,
    MIPS_REGN_r6,
    MIPS_REGN_r7,
    MIPS_REGN_r8,
    MIPS_REGN_r9,
    MIPS_REGN_r10,
    MIPS_REGN_r11,
    MIPS_REGN_r12,
    MIPS_REGN_r13,
    MIPS_REGN_r14,
    MIPS_REGN_r15,
    MIPS_REGN_r16,
    MIPS_REGN_r17,
    MIPS_REGN_r18,
    MIPS_REGN_r19,
    MIPS_REGN_r20,
    MIPS_REGN_r21,
    MIPS_REGN_r22,
    MIPS_REGN_r23,
    MIPS_REGN_r24,
    MIPS_REGN_r25,
    MIPS_REGN_r26,
    MIPS_REGN_r27,
    MIPS_REGN_r28,
    MIPS_REGN_r29,
    MIPS_REGN_r30,
    MIPS_REGN_r31,
    MIPS_REGN_f0,
    MIPS_REGN_f1,
    MIPS_REGN_f2,
    MIPS_REGN_f3,
    MIPS_REGN_f4,
    MIPS_REGN_f5,
    MIPS_REGN_f6,
    MIPS_REGN_f7,
    MIPS_REGN_f8,
    MIPS_REGN_f9,
    MIPS_REGN_f10,
    MIPS_REGN_f11,
    MIPS_REGN_f12,
    MIPS_REGN_f13,
    MIPS_REGN_f14,
    MIPS_REGN_f15,
    MIPS_REGN_f16,
    MIPS_REGN_f17,
    MIPS_REGN_f18,
    MIPS_REGN_f19,
    MIPS_REGN_f20,
    MIPS_REGN_f21,
    MIPS_REGN_f22,
    MIPS_REGN_f23,
    MIPS_REGN_f24,
    MIPS_REGN_f25,
    MIPS_REGN_f26,
    MIPS_REGN_f27,
    MIPS_REGN_f28,
    MIPS_REGN_f29,
    MIPS_REGN_f30,
    MIPS_REGN_f31,
    MIPS_REGN_END,
} mips_regn;

extern mips_regn MIPSRegN( name *reg_name );
