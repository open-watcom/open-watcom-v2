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
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


.globl          _setjmpex
.globl          __setjmpex_init

.new_section .text, "c4"

__setjmpex_init:
    ldah          $t0,h^_setjmpex($zero)
    lda           $t0,l^_setjmpex($t0)
    ldah          $t1,h^__SetJmpPntr($zero)
    lda           $t1,l^__SetJmpPntr($t1)
    stl           $t0,($t1)
    ret

_setjmpex:
    lda     $sp,-0x250($sp)
    stq     $s0,0x8($sp)
    stq     $ra,($sp)
    lda     $t0,0x20($sp)
    lda     $v0,0x250($sp)
    stt     $f2,0x10($t0)
    stt     $f3,0x18($t0)
    stt     $f4,0x20($t0)
    stt     $f5,0x28($t0)
    stt     $f6,0x30($t0)
    stt     $f7,0x38($t0)
    stt     $f8,0x40($t0)
    stt     $f9,0x48($t0)
    stq     $fp,0x80($t0)
    stq     $v0,0x88($t0)                   /* Save actual SP             */
    stq     $fp,0x18($a0)
    stq     $v0,0x10($a0)
    stq     $s0,0x148($t0)
    stq     $s1,0x150($t0)
    stq     $s2,0x158($t0)
    stq     $s3,0x160($t0)
    stq     $s4,0x168($t0)
    stq     $s5,0x170($t0)
    stq     $fp,0x178($t0)
    stq     $v0,0x188($t0)                  /* Save actual SP             */
    stq     $gp,0x1e8($t0)
    stq     $v0,0x1f0($t0)
    stq     $ra,0x1d0($t0)
    stq     $ra,0x210($t0)
    mov     $a0,$s0
    stl     $ra,0x4($a0)
    addq    $zero,0x00000002,$t1            /* Type == 2                  */
    stl     $t1,0xc($a0)
    subl    $ra,0x00000004,$a0
    bsr     $ra,j^RtlLookupFunctionEntry    /* Get the PDATA              */
    ldq     $a0,($sp)
    mov     $v0,$a1                         /* VirtualUnwind to get       */
    lda     $a2,0x20($sp)                   /* the frame pointer          */
    lda     $a3,0x10($sp)
    lda     $a4,0x18($sp)
    clr     $a5
    subl    $a0,0x00000004,$a0              /* Ensure RA is in func       */
    bsr     $ra,j^RtlVirtualUnwind
    ldl     $t0,0x18($sp)
    stl     $t0,($s0)
    ldq     $ra,($sp)                       /* Restore RA                 */
    ldq     $s0,0x8($sp)
    clr     $v0                             /* Return value == 0          */
    lda     $sp,0x250($sp)
    ret     $zero,($ra),0x00000001


.new_section .rtl$xid, "drw4"
__anon001:
    .byte  0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00
    .long  __setjmpex_init
    .byte  0x00, 0x00, 0x00, 0x00
