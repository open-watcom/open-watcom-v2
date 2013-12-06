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
* Description:  MIPS MAD register definitions.
*
****************************************************************************/


#define REGS_LIST() \
regpickuu( 0, zero ) \
regpickuu( 1, at ) \
regpickuu( 2, v0 ) \
regpickuu( 3, v1 ) \
regpickuu( 4, a0 ) \
regpickuu( 5, a1 ) \
regpickuu( 6, a2 ) \
regpickuu( 7, a3 ) \
regpickuu( 8, t0 ) \
regpickuu( 9, t1 ) \
regpickuu( 10, t2 ) \
regpickuu( 11, t3 ) \
regpickuu( 12, t4 ) \
regpickuu( 13, t5 ) \
regpickuu( 14, t6 ) \
regpickuu( 15, t7 ) \
regpickuu( 16, s0 ) \
regpickuu( 17, s1 ) \
regpickuu( 18, s2 ) \
regpickuu( 19, s3 ) \
regpickuu( 20, s4 ) \
regpickuu( 21, s5 ) \
regpickuu( 22, s6 ) \
regpickuu( 23, s7 ) \
regpickuu( 24, t8 ) \
regpickuu( 25, t9 ) \
regpickuu( 26, k0 ) \
regpickuu( 27, k1 ) \
regpickuu( 28, gp ) \
regpickuu( 29, sp ) \
regpickuu( 30, fp ) \
regpickuu( 31, ra )

#define regpickuu(num,alias) regpick(f##num,DOUBLE,FPU)
REGS_LIST()
#undef regpickuu
#define regpickuu(num,alias) regpick(r##num,DWORD,CPU)
REGS_LIST()
#undef regpickuu
#define regpickuu(num,alias) regpick(alias,DWORD,CPU)
REGS_LIST()
#undef regpickuu

regpick( pc, DWORD, CPU )
regpick( lo, WORD, CPU )
regpick( hi, WORD, CPU )
regpick( fpcsr, FPCSR, FPU )
regpick( fpivr, WORD, FPU )
