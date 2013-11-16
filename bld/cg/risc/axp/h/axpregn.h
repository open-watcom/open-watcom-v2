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


typedef enum axp_regn{
     AXP_REGN_r0,
     AXP_REGN_r1,
     AXP_REGN_r2,
     AXP_REGN_r3,
     AXP_REGN_r4,
     AXP_REGN_r5,
     AXP_REGN_r6,
     AXP_REGN_r7,
     AXP_REGN_r8,
     AXP_REGN_r9,
     AXP_REGN_r10,
     AXP_REGN_r11,
     AXP_REGN_r12,
     AXP_REGN_r13,
     AXP_REGN_r14,
     AXP_REGN_r15,
     AXP_REGN_r16,
     AXP_REGN_r17,
     AXP_REGN_r18,
     AXP_REGN_r19,
     AXP_REGN_r20,
     AXP_REGN_r21,
     AXP_REGN_r22,
     AXP_REGN_r23,
     AXP_REGN_r24,
     AXP_REGN_r25,
     AXP_REGN_r26,
     AXP_REGN_r27,
     AXP_REGN_r28,
     AXP_REGN_r29,
     AXP_REGN_r30,
     AXP_REGN_r31,
     AXP_REGN_f0,
     AXP_REGN_f1,
     AXP_REGN_f2,
     AXP_REGN_f3,
     AXP_REGN_f4,
     AXP_REGN_f5,
     AXP_REGN_f6,
     AXP_REGN_f7,
     AXP_REGN_f8,
     AXP_REGN_f9,
     AXP_REGN_f10,
     AXP_REGN_f11,
     AXP_REGN_f12,
     AXP_REGN_f13,
     AXP_REGN_f14,
     AXP_REGN_f15,
     AXP_REGN_f16,
     AXP_REGN_f17,
     AXP_REGN_f18,
     AXP_REGN_f19,
     AXP_REGN_f20,
     AXP_REGN_f21,
     AXP_REGN_f22,
     AXP_REGN_f23,
     AXP_REGN_f24,
     AXP_REGN_f25,
     AXP_REGN_f26,
     AXP_REGN_f27,
     AXP_REGN_f28,
     AXP_REGN_f29,
     AXP_REGN_f30,
     AXP_REGN_f31,
     AXP_REGN_END,
}axp_regn;
extern  axp_regn RegTransN( name *reg_name );
