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
* Description:  Register names for x86_64 (AMD64/EM64T) CPUs.
*
****************************************************************************/


/*
    Ordering is important! Keep all like sized registers together for
    GetRefType.
*/

/* Byte size registers */
regpick( al, "al" )
regpick( cl, "cl" )
regpick( dl, "dl" )
regpick( bl, "bl" )
regpick( r8b, "r8b" )
regpick( r9b, "r9b" )
regpick( r10b, "r10b" )
regpick( r11b, "r11b" )
regpick( r12b, "r12b" )
regpick( r13b, "r13b" )
regpick( r14b, "r14b" )
regpick( r15b, "r15b" )

/* special byte size register encoding, depending on the REX prefix */
regpick( spl, "spl" )
regpick( ah, "ah" )
regpick( bpl, "bpl" )
regpick( ch, "ch" )
regpick( sil, "sil" )
regpick( dh, "dh" )
regpick( dil, "dil" )
regpick( bh, "bh" )

/* Word sized registers */
regpick( ax, "ax" )
regpick( cx, "cx" )
regpick( dx, "dx" )
regpick( bx, "bx" )
regpick( sp, "sp" )
regpick( bp, "bp" )
regpick( si, "si" )
regpick( di, "di" )
regpick( r8w, "r8w" )
regpick( r9w, "r9w" )
regpick( r10w, "r10w" )
regpick( r11w, "r11w" )
regpick( r12w, "r12w" )
regpick( r13w, "r13w" )
regpick( r14w, "r14w" )
regpick( r15w, "r15w" )

/* Doubleword sized registers */
regpick( eax, "eax" )
regpick( ecx, "ecx" )
regpick( edx, "edx" )
regpick( ebx, "ebx" )
regpick( esp, "esp" )
regpick( ebp, "ebp" )
regpick( esi, "esi" )
regpick( edi, "edi" )
regpick( r8d, "r8d" )
regpick( r9d, "r9d" )
regpick( r10d, "r10d" )
regpick( r11d, "r11d" )
regpick( r12d, "r12d" )
regpick( r13d, "r13d" )
regpick( r14d, "r14d" )
regpick( r15d, "r15d" )
regpick( eflags, "eflags" )
regpick( eip, "eip" )

/* Quardword sized registers */
regpick( rax, "rax" )
regpick( rcx, "rcx" )
regpick( rdx, "rdx" )
regpick( rbx, "rbx" )
regpick( rsp, "rsp" )
regpick( rbp, "rbp" )
regpick( rsi, "rsi" )
regpick( rdi, "rdi" )
regpick( r8, "r8" )
regpick( r9, "r9" )
regpick( r10, "r10" )
regpick( r11, "r11" )
regpick( r12, "r12" )
regpick( r13, "r13" )
regpick( r14, "r14" )
regpick( r15, "r15" )
regpick( rflags, "rflags" )
regpick( rip, "rip" )

/* not reviewed stuff */
regpick( st, "st" )
regpick( st1, "st(1)")
regpick( st2, "st(2)")
regpick( st3, "st(3)")
regpick( st4, "st(4)")
regpick( st5, "st(5)")
regpick( st6, "st(6)")
regpick( st7, "st(7)")
regpick( mm0, "mm0" )
regpick( mm1, "mm1")
regpick( mm2, "mm2")
regpick( mm3, "mm3")
regpick( mm4, "mm4")
regpick( mm5, "mm5")
regpick( mm6, "mm6")
regpick( mm7, "mm7")
regpick( xmm0, "xmm0" )
regpick( xmm1, "xmm1")
regpick( xmm2, "xmm2")
regpick( xmm3, "xmm3")
regpick( xmm4, "xmm4")
regpick( xmm5, "xmm5")
regpick( xmm6, "xmm6")
regpick( xmm7, "xmm7")
regpick( cr0, "cr0")
regpick( cr2, "cr2")
regpick( cr3, "cr3")
regpick( cr4, "cr4")
regpick( cr8, "cr8")
regpick( dr0, "dr0")
regpick( dr1, "dr1")
regpick( dr2, "dr2")
regpick( dr3, "dr3")
regpick( dr6, "dr6")
regpick( dr7, "dr7")
regpick( es, "es" )
regpick( cs, "cs" )
regpick( ss, "ss" )
regpick( ds, "ds" )
regpick( fs, "fs" )
regpick( gs, "gs" )
