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
;* Description:  C library longjmp support for MIPS.
;*
;*****************************************************************************


.globl      longjmp

.set noreorder

.new_section    .text, "crx4"

longjmp:
    lw      $ra,0x00($a0)
    lw      $sp,0x04($a0)
    lw      $s0,0x08($a0)
    lw      $s1,0x0c($a0)
    lw      $s2,0x10($a0)
    lw      $s3,0x14($a0)
    lw      $s4,0x18($a0)
    lw      $s5,0x1c($a0)
    lw      $s6,0x20($a0)
    lw      $s7,0x24($a0)
    lw      $fp,0x28($a0)
    lw      $v0,0x30($a0)
    lw      $gp,0x2c($a0)
    ctc1    $v0,$31
    lwc1    $f20,0x38($a0)  ; doubles are 8-byte aligned - skip padding
    lwc1    $f21,0x3c($a0)
    lwc1    $f22,0x40($a0)
    lwc1    $f23,0x44($a0)
    lwc1    $f24,0x48($a0)
    lwc1    $f25,0x4c($a0)
    lwc1    $f26,0x50($a0)
    lwc1    $f27,0x54($a0)
    lwc1    $f28,0x58($a0)
    lwc1    $f29,0x5c($a0)
    lwc1    $f30,0x60($a0)
    lwc1    $f31,0x64($a0)
    jr      $ra
    move    $v0,$a1
