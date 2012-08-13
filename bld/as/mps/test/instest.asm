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
;* Description:  Test of MIPS machine instructions. No pseudo-instructions
;*               here, only instructions that have direct machine equivalent.
;*
;*****************************************************************************


.text

; Currently all instructions listed in "MIPS RISC Architecture", Appendix A
; (with the exception of Coprocessor 1 related instructions)

.set noreorder  # don't want those nops

targ:
    add     $v0,$a0,$a1
    addi    $v0,$a0,0x1234
    addiu   $v0,$a0,0x1234
    addu    $v0,$a0,$a1
    and     $v0,$a0,$a1
    andi    $v0,$a0,0xa5a5
    bc0f    dest
    bc0fl   dest
    bc0t    dest
    bc0tl   dest
    beq     $a0,$a1,dest
    beql    $a0,$a1,dest
    bgez    $a0,dest
    bgezal  $a0,dest
    bgezall $a0,dest
    bgezl   $a0,dest
    bgtz    $a0,dest
    bgtzl   $a0,dest
    blez    $a0,dest
    blezl   $a0,dest
    bltz    $a0,dest
    bltzal  $a0,dest
    bltzall $a0,dest
    bltzl   $a0,dest
    bne     $a0,$a1,dest
    bnel    $a0,$a1,dest
    break
;   cache
    div     $a0,$a1
    divu    $a0,$a1
    eret
    j       targ
    jal     targ
    jalr    $ra,$a0
    jr      $a0
    lb      $v0,($a0)
    lbu     $v0,($a0)
    lh      $v0,($a0)
    lhu     $v0,($a0)
    ll      $v0,($a0)
    lui     $v0,0x8765
    lw      $v0,($a0)
    lwl     $v0,($a0)
;   lcache
    lwr     $v0,($a0)
;   flush
    mfc0    $v0,$31
    mfhi    $v0
    mflo    $v0
    mtc0    $a0,$31
    mthi    $v0
    mtlo    $v0
    mult    $a0,$a1
    multu   $a0,$a1
    nor     $v0,$a0,$a1
    or      $v0,$a0,$a1
    ori     $v0,$a0,0xa5a5
    rfe
    sb      $v0,($a0)
    sc      $v0,($a0)
    sh      $v0,($a0)
    sll     $v0,$a0,15
    sllv    $v0,$a0,$a1
    slt     $v0,$a0,$a1
    slti    $v0,$a0,0x1234
    sltiu   $v0,$a0,0x1234
    sltu    $v0,$a0,$a1
    sra     $v0,$a0,15
    srav    $v0,$a0,$a1
    srl     $v0,$a0,15
    srlv    $v0,$a0,$a1
    sub     $v0,$a0,$a1
    subu    $v0,$a0,$a1
    swl     $v0,($a0)
;   scache
    swr     $v0,($a0)
;   invalidate
    sync
    syscall
    teq     $a0,$a1
    teqi    $a0,0x1234
    tge     $a0,$a1
    tgei    $a0,0x1234
    tgeiu   $a0,0x1234
    tgeu    $a0,$a1
    tlbp
    tlbr
    tlbwi
    tlbwr
    tlt     $a0,$a1
    tlti    $a0,0x1234
    tltiu   $a0,0x1234
    tltu    $a0,$a1
    tne     $a0,$a1
    tnei    $a0,0x1234
    xor     $v0,$a0,$a1
    xori    $v0,$a0,0xa5a5
dest:
