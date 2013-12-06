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
regpickr( 0 ) \
regpickuu( 1, sp ) \
regpickuu( 2, rtoc ) \
regpickr( 3 ) \
regpickr( 4 ) \
regpickr( 5 ) \
regpickr( 6 ) \
regpickr( 7 ) \
regpickr( 8 ) \
regpickr( 9 ) \
regpickr( 10 ) \
regpickr( 11 ) \
regpickr( 12 ) \
regpickr( 13 ) \
regpickr( 14 ) \
regpickr( 15 ) \
regpickr( 16 ) \
regpickr( 17 ) \
regpickr( 18 ) \
regpickr( 19 ) \
regpickr( 20 ) \
regpickr( 21 ) \
regpickr( 22 ) \
regpickr( 23 ) \
regpickr( 24 ) \
regpickr( 25 ) \
regpickr( 26 ) \
regpickr( 27 ) \
regpickr( 28 ) \
regpickr( 29 ) \
regpickr( 30 ) \
regpickr( 31 )

#define regpickr(num) regpick(f##num,DOUBLE,FPU)
#define regpickuu(num,alias) regpick(f##num,DOUBLE,FPU)
REGS_LIST()
#undef regpickuu
#undef regpickr
regpick( fpscr, WORD, FPU )

#define regpickr(num) regpick(r##num,DWORD,CPU)
#define regpickuu(num,alias) regpick(r##num,DWORD,CPU)
REGS_LIST()
#undef regpickuu
#undef regpickr
#define regpickr(num)
#define regpickuu(num,alias) regpick(alias,DWORD,CPU)
REGS_LIST()
#undef regpickuu
#undef regpickr

regpick( lr, DWORD, CPU )
regpick( ctr, DWORD, CPU )
regpick( iar, DWORD, CPU )
regpick( msr, DWORD, CPU )

regpick( cr, WORD, CPU )    //NYI: cr0 .. cr7
regpick( xer, WORD, CPU )
