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
;* Description:  C library setjmp/longjmp support for MIPS processors.
;*
;*****************************************************************************


.globl      __SetjmpDirect
.globl      _setjmp
.globl      __SetJmpPntr

.set noreorder

.new_section    .text, "crx4"
_setjmp:
    la      $t0,__SetJmpPntr
    lw      $t0,($t0)
    jr      $t0
    nop

__SetjmpDirect:
    sw      $ra,0x00($a0)
    sw      $sp,0x04($a0)
    sw      $s0,0x08($a0)
    sw      $s1,0x0c($a0)
    sw      $s2,0x10($a0)
    sw      $s3,0x14($a0)
    sw      $s4,0x18($a0)
    sw      $s5,0x1c($a0)
    sw      $s6,0x20($a0)
    sw      $s7,0x24($a0)
    sw      $fp,0x28($a0)
    cfc1    $v0,$31
    sw      $gp,0x2c($a0)
    sw      $v0,0x30($a0)
    swc1    $f20,0x38($a0)  ; doubles are 8-byte aligned - pad
    swc1    $f21,0x3c($a0)
    swc1    $f22,0x40($a0)
    swc1    $f23,0x44($a0)
    swc1    $f24,0x48($a0)
    swc1    $f25,0x4c($a0)
    swc1    $f26,0x50($a0)
    swc1    $f27,0x54($a0)
    swc1    $f28,0x58($a0)
    swc1    $f29,0x5c($a0)
    swc1    $f30,0x60($a0)
    swc1    $f31,0x64($a0)
    jr      $ra
    li      $v0,0

.new_section    .data, "drw4"
__SetJmpPntr:
    .long   __SetjmpDirect
