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
#define defining_enums
enum {
#endif
    regpick( f0, DOUBLE, FPU )
    regpick( f1, DOUBLE, FPU )
    regpick( f2, DOUBLE, FPU )
    regpick( f3, DOUBLE, FPU )
    regpick( f4, DOUBLE, FPU )
    regpick( f5, DOUBLE, FPU )
    regpick( f6, DOUBLE, FPU )
    regpick( f7, DOUBLE, FPU )
    regpick( f8, DOUBLE, FPU )
    regpick( f9, DOUBLE, FPU )
    regpick( f10, DOUBLE, FPU )
    regpick( f11, DOUBLE, FPU )
    regpick( f12, DOUBLE, FPU )
    regpick( f13, DOUBLE, FPU )
    regpick( f14, DOUBLE, FPU )
    regpick( f15, DOUBLE, FPU )
    regpick( f16, DOUBLE, FPU )
    regpick( f17, DOUBLE, FPU )
    regpick( f18, DOUBLE, FPU )
    regpick( f19, DOUBLE, FPU )
    regpick( f20, DOUBLE, FPU )
    regpick( f21, DOUBLE, FPU )
    regpick( f22, DOUBLE, FPU )
    regpick( f23, DOUBLE, FPU )
    regpick( f24, DOUBLE, FPU )
    regpick( f25, DOUBLE, FPU )
    regpick( f26, DOUBLE, FPU )
    regpick( f27, DOUBLE, FPU )
    regpick( f28, DOUBLE, FPU )
    regpick( f29, DOUBLE, FPU )
    regpick( f30, DOUBLE, FPU )
    regpick( f31, DOUBLE, FPU )

    regpick( fpscr, WORD, FPU )

    regpick( r0, DWORD, CPU )
    regpick( r1, DWORD, CPU )
    regpick( r2, DWORD, CPU )
    regpick( r3, DWORD, CPU )
    regpick( r4, DWORD, CPU )
    regpick( r5, DWORD, CPU )
    regpick( r6, DWORD, CPU )
    regpick( r7, DWORD, CPU )
    regpick( r8, DWORD, CPU )
    regpick( r9, DWORD, CPU )
    regpick( r10, DWORD, CPU )
    regpick( r11, DWORD, CPU )
    regpick( r12, DWORD, CPU )
    regpick( r13, DWORD, CPU )
    regpick( r14, DWORD, CPU )
    regpick( r15, DWORD, CPU )
    regpick( r16, DWORD, CPU )
    regpick( r17, DWORD, CPU )
    regpick( r18, DWORD, CPU )
    regpick( r19, DWORD, CPU )
    regpick( r20, DWORD, CPU )
    regpick( r21, DWORD, CPU )
    regpick( r22, DWORD, CPU )
    regpick( r23, DWORD, CPU )
    regpick( r24, DWORD, CPU )
    regpick( r25, DWORD, CPU )
    regpick( r26, DWORD, CPU )
    regpick( r27, DWORD, CPU )
    regpick( r28, DWORD, CPU )
    regpick( r29, DWORD, CPU )
    regpick( r30, DWORD, CPU )
    regpick( r31, DWORD, CPU )

    regpick( sp, DWORD, CPU )

    regpick( lr, DWORD, CPU )
    regpick( ctr, DWORD, CPU )
    regpick( iar, DWORD, CPU )
    regpick( msr, DWORD, CPU )

    regpick( cr, WORD, CPU )    //NYI: cr0 .. cr7
    regpick( xer, WORD, CPU )

#if defined(defining_enums)
IDX_LAST_ONE };
#undef regpick
#undef defining_enums
#endif
