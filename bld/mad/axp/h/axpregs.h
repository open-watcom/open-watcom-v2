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


#define REGS_LIST() \
regpickuu( 0, v0 ) \
regpickuu( 1, t0 ) \
regpickuu( 2, t1 ) \
regpickuu( 3, t2 ) \
regpickuu( 4, t3 ) \
regpickuu( 5, t4 ) \
regpickuu( 6, t5 ) \
regpickuu( 7, t6 ) \
regpickuu( 8, t7 ) \
regpickuu( 9, s0 ) \
regpickuu( 10, s1 ) \
regpickuu( 11, s2 ) \
regpickuu( 12, s3 ) \
regpickuu( 13, s4 ) \
regpickuu( 14, s5 ) \
regpickuu( 15, fp ) \
regpickuu( 16, a0 ) \
regpickuu( 17, a1 ) \
regpickuu( 18, a2 ) \
regpickuu( 19, a3 ) \
regpickuu( 20, a4 ) \
regpickuu( 21, a5 ) \
regpickuu( 22, t8 ) \
regpickuu( 23, t9 ) \
regpickuu( 24, t10 ) \
regpickuu( 25, t11 ) \
regpickuu( 26, ra ) \
regpickuu( 27, t12 ) \
regpickuu( 28, at ) \
regpickuu( 29, gp ) \
regpickuu( 30, sp ) \
regpickuu( 31, zero )

#define regpickuu(num,alias) regpick(f##num,FLT,FPU)
REGS_LIST()
#undef regpickuu
#define regpickuu(num,alias) regpicku(u##num,r##num,INT,CPU)
REGS_LIST()
#undef regpickuu
regpick( fpcr, FPCR, FPU )
#define regpickuu(num,alias) regpicku(u##num,alias,INT,CPU)
REGS_LIST()
#undef regpickuu
//NYI: unix, vms pal registers

palpick( nt, fir )
palpick( nt, softfpcr )
palpick( nt, psr )
