;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  C library longjmp support for Alpha.
;*
;*****************************************************************************


.globl          longjmp

.new_section .text, "c4"

longjmp:
    ldl     $t1,0xc($a0)
    addq    $zero,0x00000001,$t0
    cmoveq  $a1,$t0,$a1
    subq    $t1,0x00000001,$t2
    bne     $t2,longjmpex                 /* Type == 2 or 3               */
    mov     $a1,$v0                       /* Type == 1 Traditional setjmp */
    ldt     $f2,0x10($a0)
    ldt     $f3,0x18($a0)
    ldt     $f4,0x20($a0)
    ldt     $f5,0x28($a0)
    ldt     $f6,0x30($a0)
    ldt     $f7,0x38($a0)
    ldt     $f8,0x40($a0)
    ldt     $f9,0x48($a0)
    ldq     $s0,0x50($a0)
    ldq     $s1,0x58($a0)
    ldq     $s2,0x60($a0)
    ldq     $s3,0x68($a0)
    ldq     $s4,0x70($a0)
    ldq     $s5,0x78($a0)
    ldq     $fp,0x80($a0)
    ldq     $sp,0x88($a0)
    ldq     $a1,0x90($a0)
    jmp     $zero,($a1)
