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
* Description:  Register names for MIPS CPUs.
*
****************************************************************************/


#define REGS_LIST() \
regpickr( 0, zero ) \
regpickr( 1, at ) \
regpickr( 2, v0 ) \
regpickr( 3, v1 ) \
regpickr( 4, a0 ) \
regpickr( 5, a1 ) \
regpickr( 6, a2 ) \
regpickr( 7, a3 ) \
regpickr( 8, t0 ) \
regpickr( 9, t1 ) \
regpickr( 10, t2 ) \
regpickr( 11, t3 ) \
regpickr( 12, t4 ) \
regpickr( 13, t5 ) \
regpickr( 14, t6 ) \
regpickr( 15, t7 ) \
regpickr( 16, s0 ) \
regpickr( 17, s1 ) \
regpickr( 18, s2 ) \
regpickr( 19, s3 ) \
regpickr( 20, s4 ) \
regpickr( 21, s5 ) \
regpickr( 22, s6 ) \
regpickr( 23, s7 ) \
regpickr( 24, t8 ) \
regpickr( 25, t9 ) \
regpickr( 26, k0 ) \
regpickr( 27, k1 ) \
regpickr( 28, gp ) \
regpickr( 29, sp ) \
regpickr( 30, fp ) \
regpickr( 31, ra )

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
