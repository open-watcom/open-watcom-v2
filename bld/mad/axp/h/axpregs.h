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


#if !defined(regpick)
#define regpick( name, type, s )        IDX_##name,
#define palpick( pal, name )            IDX_##pal##_##name,
#define defining_enums
enum {
#endif
    regpick( f0, FLT, FPU )
    regpick( f1, FLT, FPU )
    regpick( f2, FLT, FPU )
    regpick( f3, FLT, FPU )
    regpick( f4, FLT, FPU )
    regpick( f5, FLT, FPU )
    regpick( f6, FLT, FPU )
    regpick( f7, FLT, FPU )
    regpick( f8, FLT, FPU )
    regpick( f9, FLT, FPU )
    regpick( f10, FLT, FPU )
    regpick( f11, FLT, FPU )
    regpick( f12, FLT, FPU )
    regpick( f13, FLT, FPU )
    regpick( f14, FLT, FPU )
    regpick( f15, FLT, FPU )
    regpick( f16, FLT, FPU )
    regpick( f17, FLT, FPU )
    regpick( f18, FLT, FPU )
    regpick( f19, FLT, FPU )
    regpick( f20, FLT, FPU )
    regpick( f21, FLT, FPU )
    regpick( f22, FLT, FPU )
    regpick( f23, FLT, FPU )
    regpick( f24, FLT, FPU )
    regpick( f25, FLT, FPU )
    regpick( f26, FLT, FPU )
    regpick( f27, FLT, FPU )
    regpick( f28, FLT, FPU )
    regpick( f29, FLT, FPU )
    regpick( f30, FLT, FPU )
    regpick( f31, FLT, FPU )
    regpick( r0, INT, CPU )
    regpick( r1, INT, CPU )
    regpick( r2, INT, CPU )
    regpick( r3, INT, CPU )
    regpick( r4, INT, CPU )
    regpick( r5, INT, CPU )
    regpick( r6, INT, CPU )
    regpick( r7, INT, CPU )
    regpick( r8, INT, CPU )
    regpick( r9, INT, CPU )
    regpick( r10, INT, CPU )
    regpick( r11, INT, CPU )
    regpick( r12, INT, CPU )
    regpick( r13, INT, CPU )
    regpick( r14, INT, CPU )
    regpick( r15, INT, CPU )
    regpick( r16, INT, CPU )
    regpick( r17, INT, CPU )
    regpick( r18, INT, CPU )
    regpick( r19, INT, CPU )
    regpick( r20, INT, CPU )
    regpick( r21, INT, CPU )
    regpick( r22, INT, CPU )
    regpick( r23, INT, CPU )
    regpick( r24, INT, CPU )
    regpick( r25, INT, CPU )
    regpick( r26, INT, CPU )
    regpick( r27, INT, CPU )
    regpick( r28, INT, CPU )
    regpick( r29, INT, CPU )
    regpick( r30, INT, CPU )
    regpick( r31, INT, CPU )
    regpick( fpcr, FPCR, FPU )
    regpick( v0, INT, CPU )
    regpick( t0, INT, CPU )
    regpick( t1, INT, CPU )
    regpick( t2, INT, CPU )
    regpick( t3, INT, CPU )
    regpick( t4, INT, CPU )
    regpick( t5, INT, CPU )
    regpick( t6, INT, CPU )
    regpick( t7, INT, CPU )
    regpick( s0, INT, CPU )
    regpick( s1, INT, CPU )
    regpick( s2, INT, CPU )
    regpick( s3, INT, CPU )
    regpick( s4, INT, CPU )
    regpick( s5, INT, CPU )
    regpick( fp, INT, CPU )
    regpick( a0, INT, CPU )
    regpick( a1, INT, CPU )
    regpick( a2, INT, CPU )
    regpick( a3, INT, CPU )
    regpick( a4, INT, CPU )
    regpick( a5, INT, CPU )
    regpick( t8, INT, CPU )
    regpick( t9, INT, CPU )
    regpick( t10, INT, CPU )
    regpick( t11, INT, CPU )
    regpick( ra, INT, CPU )
    regpick( t12, INT, CPU )
    regpick( at, INT, CPU )
    regpick( gp, INT, CPU )
    regpick( sp, INT, CPU )
    palpick( nt, fir )
    palpick( nt, softfpcr )
    palpick( nt, psr )
    //NYI: unix, vms pal registers

#if defined(defining_enums)
IDX_LAST_ONE };
#undef regpick
#undef palpick
#undef defining_enums
#endif
