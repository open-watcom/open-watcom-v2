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
* Description:  Register names for x86 CPUs.
*
****************************************************************************/


/*
    Ordering is important! Keep all like sized registers together for
    GetRefType.
*/
regpick( ah, "ah" )
regpick( al, "al" )
regpick( ch, "ch" )
regpick( cl, "cl" )
regpick( dh, "dh" )
regpick( dl, "dl" )
regpick( bh, "bh" )
regpick( bl, "bl" )
regpick( ax, "ax" )
regpick( cx, "cx" )
regpick( dx, "dx" )
regpick( bx, "bx" )
regpick( sp, "sp" )
regpick( bp, "bp" )
regpick( si, "si" )
regpick( di, "di" )
regpick( eax, "eax" )
regpick( ecx, "ecx" )
regpick( edx, "edx" )
regpick( ebx, "ebx" )
regpick( esp, "esp" )
regpick( ebp, "ebp" )
regpick( esi, "esi" )
regpick( edi, "edi" )
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
regpick( dr0, "dr0")
regpick( dr1, "dr1")
regpick( dr2, "dr2")
regpick( dr3, "dr3")
regpick( dr6, "dr6")
regpick( dr7, "dr7")
regpick( tr3, "tr3")
regpick( tr4, "tr4")
regpick( tr5, "tr5")
regpick( tr6, "tr6")
regpick( tr7, "tr7")
regpick( es, "es" )
regpick( cs, "cs" )
regpick( ss, "ss" )
regpick( ds, "ds" )
regpick( fs, "fs" )
regpick( gs, "gs" )
