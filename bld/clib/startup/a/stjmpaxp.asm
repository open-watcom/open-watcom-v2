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


.globl          __SetjmpDirect
.globl          _setjmp
.globl          __SetJmpPntr

.new_section .text, "crx4"
_setjmp:
    ldah    $t0,h^__SetJmpPntr($zero)
    lda     $t0,l^__SetJmpPntr($t0)
    ldq     $t0,($t0)
    jmp     $zero,($t0)

__SetjmpDirect:
    addq    $zero,0x00000001,$t0
    stl     $t0,0xc($a0)
    stt     $f3,0x18($a0)
    stt     $f4,0x20($a0)
    stt     $f5,0x28($a0)
    stt     $f6,0x30($a0)
    stt     $f7,0x38($a0)
    stt     $f8,0x40($a0)
    stt     $f9,0x48($a0)
    stt     $f2,0x10($a0)
    stq     $s0,0x50($a0)
    stq     $s1,0x58($a0)
    stq     $s2,0x60($a0)
    stq     $s3,0x68($a0)
    stq     $s4,0x70($a0)
    stq     $s5,0x78($a0)
    stq     $fp,0x80($a0)
    stq     $sp,0x88($a0)
    stq     $ra,0x90($a0)
    clr     $v0
    ret     $zero,($ra),0x00000001

.new_section .data, "drw4"
__SetJmpPntr:
                .long   __SetjmpDirect
