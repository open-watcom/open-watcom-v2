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
* Description:  Register names for Alpha AXP CPUs.
*
****************************************************************************/


#define REGS_LIST() \
regpickr( 0, v0 ) \
regpickr( 1, t0 ) \
regpickr( 2, t1 ) \
regpickr( 3, t2 ) \
regpickr( 4, t3 ) \
regpickr( 5, t4 ) \
regpickr( 6, t5 ) \
regpickr( 7, t6 ) \
regpickr( 8, t7 ) \
regpickr( 9, s0 ) \
regpickr( 10, s1 ) \
regpickr( 11, s2 ) \
regpickr( 12, s3 ) \
regpickr( 13, s4 ) \
regpickr( 14, s5 ) \
regpickr( 15, fp ) \
regpickr( 16, a0 ) \
regpickr( 17, a1 ) \
regpickr( 18, a2 ) \
regpickr( 19, a3 ) \
regpickr( 20, a4 ) \
regpickr( 21, a5 ) \
regpickr( 22, t8 ) \
regpickr( 23, t9 ) \
regpickr( 24, t10 ) \
regpickr( 25, t11 ) \
regpickr( 26, ra ) \
regpickr( 27, t12 ) \
regpickr( 28, at ) \
regpickr( 29, gp ) \
regpickr( 30, sp ) \
regpickr( 31, zero )

#define strx(x) #x

#define regpickr(num,alias) regpick(f##num,strx(f##num))
REGS_LIST()
#undef regpickr
#define regpickr(num,alias) regpick(r##num,strx(r##num))
REGS_LIST()
#undef regpickr
#define regpickr(num,alias) regpick(alias,strx(alias))
REGS_LIST()
#undef regpickr

#ifdef INCLUDE_ASM_REGS

#define regpickr(num,alias) regpick(af##num,strx($f##num))
REGS_LIST()
#undef regpickr
#define regpickr(num,alias) regpick(ar##num,strx($##num))
REGS_LIST()
#undef regpickr
#define regpickr(num,alias) regpick(a##alias,strx($##alias))
REGS_LIST()
#undef regpickr

#endif

#undef strx

#undef REGS_LIST
