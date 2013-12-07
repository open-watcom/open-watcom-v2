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
* Description:  Register names for PowerPC CPUs.
*
****************************************************************************/


#define REGS_LIST() \
regpick1( 0 ) \
regpickr( 1, sp ) \
regpickr( 2, rtoc ) \
regpick1( 3 ) \
regpick1( 4 ) \
regpick1( 5 ) \
regpick1( 6 ) \
regpick1( 7 ) \
regpick1( 8 ) \
regpick1( 9 ) \
regpick1( 10 ) \
regpick1( 11 ) \
regpick1( 12 ) \
regpick1( 13 ) \
regpick1( 14 ) \
regpick1( 15 ) \
regpick1( 16 ) \
regpick1( 17 ) \
regpick1( 18 ) \
regpick1( 19 ) \
regpick1( 20 ) \
regpick1( 21 ) \
regpick1( 22 ) \
regpick1( 23 ) \
regpick1( 24 ) \
regpick1( 25 ) \
regpick1( 26 ) \
regpick1( 27 ) \
regpick1( 28 ) \
regpick1( 29 ) \
regpick1( 30 ) \
regpick1( 31 )

#define strx(x) #x

#define regpick1(num)       regpick(r##num,strx(r##num))
#define regpickr(num,alias) regpick(r##num,strx(r##num))
REGS_LIST()
#undef regpick1
#undef regpickr

#ifdef INCLUDE_ASM_REGS

#define regpick1(num)       regpick(f##num,strx(f##num))
#define regpickr(num,alias) regpick(f##num,strx(f##num))
REGS_LIST()
#undef regpick1
#undef regpickr
regpick( cr0, "cr0" )
regpick( cr1, "cr1" )
regpick( cr2, "cr2" )
regpick( cr3, "cr3" )
regpick( cr4, "cr4" )
regpick( cr5, "cr5" )
regpick( cr6, "cr6" )
regpick( cr7, "cr7" )
#define regpick1(num)       regpick(crb##num,strx(crb##num))
#define regpickr(num,alias) regpick(crb##num,strx(crb##num))
REGS_LIST()
#undef regpick1
#undef regpickr
#define regpick1(num)
#define regpickr(num,alias) regpick(alias,strx(alias))
REGS_LIST()
#undef regpick1
#undef regpickr

#endif

#undef strx

#undef REGS_LIST
