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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


typedef enum ppc_regn{
     PPC_REGN_r0,
     PPC_REGN_r1,
     PPC_REGN_r2,
     PPC_REGN_r3,
     PPC_REGN_r4,
     PPC_REGN_r5,
     PPC_REGN_r6,
     PPC_REGN_r7,
     PPC_REGN_r8,
     PPC_REGN_r9,
     PPC_REGN_r10,
     PPC_REGN_r11,
     PPC_REGN_r12,
     PPC_REGN_r13,
     PPC_REGN_r14,
     PPC_REGN_r15,
     PPC_REGN_r16,
     PPC_REGN_r17,
     PPC_REGN_r18,
     PPC_REGN_r19,
     PPC_REGN_r20,
     PPC_REGN_r21,
     PPC_REGN_r22,
     PPC_REGN_r23,
     PPC_REGN_r24,
     PPC_REGN_r25,
     PPC_REGN_r26,
     PPC_REGN_r27,
     PPC_REGN_r28,
     PPC_REGN_r29,
     PPC_REGN_r30,
     PPC_REGN_r31,
     PPC_REGN_f0,
     PPC_REGN_f1,
     PPC_REGN_f2,
     PPC_REGN_f3,
     PPC_REGN_f4,
     PPC_REGN_f5,
     PPC_REGN_f6,
     PPC_REGN_f7,
     PPC_REGN_f8,
     PPC_REGN_f9,
     PPC_REGN_f10,
     PPC_REGN_f11,
     PPC_REGN_f12,
     PPC_REGN_f13,
     PPC_REGN_f14,
     PPC_REGN_f15,
     PPC_REGN_f16,
     PPC_REGN_f17,
     PPC_REGN_f18,
     PPC_REGN_f19,
     PPC_REGN_f20,
     PPC_REGN_f21,
     PPC_REGN_f22,
     PPC_REGN_f23,
     PPC_REGN_f24,
     PPC_REGN_f25,
     PPC_REGN_f26,
     PPC_REGN_f27,
     PPC_REGN_f28,
     PPC_REGN_f29,
     PPC_REGN_f30,
     PPC_REGN_f31,
     PPC_REGN_END,
} ppc_regn;

extern  ppc_regn RegTransN( name *reg_name );
