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
;* Description:  Alpha AXP division routines. Alpha has no integer divide
;*               instruction, oh joy.
;*
;*****************************************************************************


.set noat
.set noreorder

.text

.globl      _OtsDivide
.globl      _OtsRemainder32Unsigned
.globl      _OtsDivide32Unsigned
.globl      _OtsModulus32
.globl      _OtsRemainder32
.globl      _OtsDivide32Overflow
.globl      _OtsDivide32
.globl      _OtsDivide64Overflow
.globl      _OtsDivide64
.globl      _OtsRemainder64
.globl      _OtsModulus64
.globl      _OtsDivide64Unsigned
.globl      _OtsRemainder64Unsigned
.extern     _OtsDivData

_OtsDivide:

_OtsRemainder32Unsigned:
    ldah          $t12,h^_OtsDivData($zero)
    lda           $t12,l^_OtsDivData($t12)
    lda           $at,-0x200($a1)
    subl          $a1,0x00000001,$t0
    blt           $a1,rui_big
    and           $a1,$t0,$a2
    bgt           $at,rui_lrgdiv
    addq          $a1,$a1,$v0
    beq           $a2,rui_pwr2
    s8addq        $v0,$t12,$t12
    ldq           $t0,($t12)
    cmpult        $a0,$a1,$a2
    zap           $a0,0xf0,$v0
    bne           $a2,rui_lss
    umulh         $v0,$t0,$v0
    mull          $v0,$a1,$v0
    subl          $a0,$v0,$v0
    ret           $zero,($ra)
rui_pwr2:
    beq           $a1,divzer
    and           $a0,$t0,$v0
    ret           $zero,($ra)
rui_lss:
    mov           $a0,$v0
    ret           $zero,($ra)
rui_lrgdiv:
    zap           $a0,0xf0,$a0
    bsr           $at,div32
    sextl         $t0,$v0
    ret           $zero,($ra)
rui_big:
    cmpult        $a0,$a1,$t0
    subl          $a0,$a1,$v0
    cmovne        $t0,$a0,$v0
    ret           $zero,($ra)

_OtsDivide32Unsigned:
    ldah          $t12,h^_OtsDivData($zero)
    lda           $t12,l^_OtsDivData($t12)
    lda           $at,-0x200($a1)
    blt           $a1,dui_big
    addq          $a1,$a1,$a2
    cmpule        $a1,$a0,$v0
    beq           $a1,divzer
    s8addq        $a2,$t12,$t12
    beq           $v0,dui_end
    bgt           $at,dui_lrgdiv
    ldq           $t0,($t12)
    zap           $a0,0xf0,$a0
    blt           $t0,dui_smpwr2
    umulh         $a0,$t0,$v0
dui_end:
    ret           $zero,($ra)
    nop
dui_smpwr2:
    srl           $a0,$t0,$v0
    sextl         $v0,$v0
    ret           $zero,($ra)
dui_lrgdiv:
    zap           $a0,0xf0,$a0
    bsr           $at,div32
    sextl         $v0,$v0
    ret           $zero,($ra)
dui_big:
    cmpule        $a1,$a0,$v0
    ret           $zero,($ra)

_OtsModulus32:
    subq          $zero,$a1,$a2
    cmovge        $a1,$a1,$a2
    subq          $a2,1,$t0
    beq           $a1,divzer
    and           $a2,$t0,$v0
    beq           $v0,mi_p2
    xor           $a0,$a1,$at
    clr           $a3
    cmovlt        $at,$a1,$a3
    and           $a0,$a1,$t12
    mov           $a2,$a1
    negq          $a0,$a2
    cmovlt        $a0,$a2,$a0
    cmplt         $t12,$zero,$v0
    sll           $v0,63,$v0
    bis           $v0,$a3,$a3
    bsr           $at,div32
    cmoveq        $t0,$zero,$a3
    addq          $a3,$a3,$a2
    subq          $a3,1,$at
    negl          $t0,$v0
    cmovlt        $at,$t0,$v0
    cmoveq        $a2,$zero,$a3
    addl          $a3,$v0,$v0
    ret           $zero,($ra)
mi_p2:
    cmovge        $a1,$zero,$a1
    and           $a0,$t0,$t0
    cmoveq        $t0,$zero,$a1
    addl          $a1,$t0,$v0
    ret           $zero,($ra)

_OtsRemainder32:
    ldah          $t12,h^_OtsDivData($zero)
    lda           $t12,l^_OtsDivData($t12)
    negq          $a1,$a2
    cmovlt        $a1,$a2,$a1
    subq          $a1,1,$t0
    and           $a1,$t0,$v0
    sra           $a0,63,$a3
    negq          $a0,$a2
    cmovlt        $a0,$a2,$a0
    beq           $v0,ri_pwr2
    lda           $at,-0x200($a1)
    bgt           $at,ri_lrgdiv
    addq          $a1,$a1,$v0
    s8addq        $v0,$t12,$t12
    ldq           $t0,($t12)
    umulh         $a0,$t0,$v0
    mull          $v0,$a1,$v0
    subl          $a0,$v0,$v0
    xor           $v0,$a3,$v0
    subl          $v0,$a3,$v0
    ret           $zero,($ra)
ri_pwr2:
    and           $a0,$t0,$v0
    beq           $a1,divzer
    xor           $v0,$a3,$v0
    subl          $v0,$a3,$v0
    ret           $zero,($ra)
    nop
ri_lrgdiv:
    bsr           $at,div32
    xor           $t0,$a3,$v0
    subl          $v0,$a3,$v0
    ret           $zero,($ra)

_OtsDivide32Overflow:
    ldah          $t12,h^_OtsDivData($zero)
    lda           $t12,l^_OtsDivData($t12)
    not           $a1,$t0
    bne           $t0,di_skip
    negl/v        $a0,$v0
    ret           $zero,($ra)
    nop

_OtsDivide32:
    ldah          $t12,h^_OtsDivData($zero)
    lda           $t12,l^_OtsDivData($t12)
di_retry:
di_skip:
    lda           $at,-0x200($a1)
    ble           $a1,di_notpos
    addq          $a1,$a1,$v0
    subq          $zero,$a0,$a2
    bgt           $at,di_lrgdiv
    s8addq        $v0,$t12,$t12
    cmpule        $a1,$a2,$v0
    cmovge        $a0,$a0,$a2
    beq           $v0,di_end
    ldq           $t0,($t12)
    blt           $t0,di_smpwr2
    umulh         $a2,$t0,$v0
    blt           $a0,di_negres
di_end:
    ret           $zero,($ra)
di_negres:
    negl          $v0,$v0
    ret           $zero,($ra)
di_smpwr2:
    srl           $a2,$t0,$a2
    sra           $a0,63,$a0
    xor           $a2,$a0,$a2
    subl          $a2,$a0,$v0
    ret           $zero,($ra)
di_notpos:
    beq           $a1,divzer
    negl          $a1,$a1
    subq          $zero,$a0,$a0
    bgt           $a1,di_retry
    sextl         $a0,$a0
    cmpeq         $a0,$a1,$v0
    ret           $zero,($ra)
    nop
di_lrgdiv:
    sra           $a0,63,$a3
    cmovlt        $a0,$a2,$a0
    bsr           $at,div32
    xor           $v0,$a3,$v0
    subl          $v0,$a3,$v0
    ret           $zero,($ra)
div32:
    cmpule        $a1,$a0,$v0
    sll           $a1,0x00000020,$a2
    sll           $a1,0x00000008,$t0
    beq           $v0,d32end
ediv32:
    mov           5,$a1
    cmpule        $t0,$a0,$v0
    sll           $t0,8,$t0
    beq           $v0,d32ent
    mov           6,$a1
    cmpule        $t0,$a0,$v0
    sll           $t0,8,$t0
    beq           $v0,d32ent
    mov           7,$a1
    cmpule        $t0,$a0,$v0
    nop
    cmovne        $v0,0x00000008,$a1
d32ent:
    extqh         $a0,$a1,$v0
    subq          $a2,1,$t0
    s8subq        $a1,0x22,$a1
    addq          $v0,$v0,$v0
d32loop:
    subq          $v0,$t0,$a2
    cmovge        $a2,$a2,$v0
    addq          $v0,$v0,$v0
    subq          $v0,$t0,$a2
    cmovge        $a2,$a2,$v0
    subq          $a1,2,$a1
    addq          $v0,$v0,$v0
    bgt           $a1,d32loop
    subq          $v0,$t0,$a2
    cmovge        $a2,$a2,$v0
    addq          $v0,$v0,$v0
    subq          $v0,$t0,$a2
    cmovge        $a2,$a2,$v0
    srl           $v0,32,$t0
    zap           $v0,0xf0,$v0
    nop
d32end:
    cmoveq        $v0,$a0,$t0
    ret           $zero,($at)

div64:
    sll           $a1,32,$a2
    cmpule        $a1,$a0,$v0
    srl           $a1,31,$t0
    beq           $v0,d64end
    cmpule        $a2,$a0,$v0
    sll           $a1,8,$t0
    or            $t0,$v0,$v0
    beq           $v0,ediv32
    cmpbge        $zero,$a1,$v0
    sll           $v0,4,$v0
    subq          $t12,$v0,$v0
    ldq           $t0,0x1000($v0)
    subq          $t0,1,$t0
    extbl         $a1,$t0,$v0
    addq          $v0,$v0,$v0
    s8addq        $v0,$t12,$v0
    ldq           $v0,0x10($v0)
    negq          $t0,$t0
    s8subq        $t0,$v0,$v0
    and           $v0,63,$v0
    sll           $a1,$v0,$a1
    srl           $a1,0x36,$t0
    addq          $t0,1,$t0
    bic           $t0,1,$t0
    s8addq        $t0,$t12,$t12
    sll           $t0,0x36,$t0
    ldq           $a2,($t12)
    subq          $t0,$a1,$t0
    beq           $t0,d64_easy
    inswl         $a2,6,$a2
    blt           $t0,d64_sub
    umulh         $t0,$a2,$t0
    ldq           $t12,0x8($t12)
    br            d64_cont
d64_sub:
    umulh         $t0,$a2,$t0
    ldq           $t12,0x8($t12)
    s4addq        $a2,0,$a2
    subq          $t12,$a2,$t12
d64_cont:
    s4addq        $t0,$t12,$a2
    umulh         $a2,$a1,$t0
    addq          $t0,$a1,$t0
    negq          $t0,$t0
    umulh         $a2,$t0,$t12
    cmovlt        $t0,0,$a2
    addq          $a2,$t0,$t0
    addq          $t12,$t0,$a2
    umulh         $a2,$a1,$t0
    addq          $t0,$a1,$t0
    negq          $t0,$t0
    umulh         $a2,$t0,$t12
    cmovlt        $t0,0,$a2
    addq          $a2,$t0,$t0
    addq          $t12,$t0,$t0
    umulh         $t0,$a0,$a2
    srl           $a1,$v0,$a1
    negq          $v0,$v0
    subq          $v0,8,$v0
    addq          $a2,$a0,$a2
    cmpult        $a2,$a0,$t0
    srl           $a2,8,$a2
    sll           $t0,0x38,$t0
    addq          $t0,$a2,$t0
    srl           $t0,$v0,$v0
    mulq          $a1,$v0,$t0
    subq          $a0,$t0,$t0
    cmpule        $a1,$t0,$a2
    subq          $t0,$a1,$t12
    cmovne        $a2,$t12,$t0
    addq          $v0,$a2,$v0
    ret           $zero,($at)
d64_easy:
    ldq           $t0,0x8($t12)
    srl           $a1,$v0,$a1
    blt           $a2,d64_pow2
    umulh         $t0,$a0,$a2
    negq          $v0,$v0
    and           $a0,0xff,$t0
    subq          $v0,8,$v0
    srl           $a0,8,$t12
    addq          $a2,$t0,$t0
    srl           $t0,8,$t0
    addq          $t0,$t12,$t0
    srl           $t0,$v0,$v0
    mulq          $a1,$v0,$t0
    subq          $a0,$t0,$t0
    ret           $zero,($at)
d64_pow2:
    not           $v0,$v0
    subq          $a1,1,$t0
    srl           $a0,$v0,$v0
    and           $a0,$t0,$t0
    ret           $zero,($at)
d64end:
    mov           $a0,$t0
    ret           $zero,($at)

_OtsDivide64Overflow:
    ldah          $t12,h^_OtsDivData($zero)
    lda           $t12,l^_OtsDivData($t12)
    not           $a1,$t0
    bne           $t0,dl_skip
    subq/v        $zero,$a0,$v0
    ret           $zero,($ra)
    nop

_OtsDivide64:
    ldah          $t12,h^_OtsDivData($zero)
    lda           $t12,l^_OtsDivData($t12)
dl_skip:
    xor           $a0,$a1,$a3
dl_retry:
    lda           $at,-0x200($a1)
    ble           $a1,dl_notpos
    addq          $a1,$a1,$v0
    negq          $a0,$a2
    bgt           $at,dl_lrgdiv
    s8addq        $v0,$t12,$t12
    srl           $a0,33,$t0
    cmpule        $a1,$a2,$v0
    bne           $t0,dl_64bit
    cmovge        $a0,$a0,$a2
    beq           $v0,dl_end
    ldq           $t12,($t12)
    sra           $a3,63,$a3
    blt           $t12,dl_smpwr2
    umulh         $a2,$t12,$v0
    beq           $a3,dl_end
    negq          $v0,$v0
    ret           $zero,($ra)
dl_64bit:
    cmovge        $a0,$a0,$a2
    beq           $v0,dl_end
    ldq           $t0,0x8($t12)
    sra           $a3,63,$a3
    ldq           $t12,($t12)
    beq           $t0,dl_smpwr2
    umulh         $a2,$t0,$v0
    addq          $v0,$a2,$a2
dl_smpwr2:
    srl           $a2,$t12,$a2
    xor           $a2,$a3,$a2
    subq          $a2,$a3,$v0
dl_end:
    ret           $zero,($ra)
dl_notpos:
    beq           $a1,divzer
    subq          $zero,$a1,$a1
    subq          $zero,$a0,$a0
    bgt           $a1,dl_retry
    cmpeq         $a0,$a1,$v0
    ret           $zero,($ra)
dl_lrgdiv:
    sra           $a3,63,$a3
    cmovlt        $a0,$a2,$a0
    bsr           $at,div64
    xor           $v0,$a3,$v0
    subq          $v0,$a3,$v0
    ret           $zero,($ra)

_OtsRemainder64:
    ldah          $t12,h^_OtsDivData($zero)
    lda           $t12,l^_OtsDivData($t12)
    negq          $a1,$a2
    cmovlt        $a1,$a2,$a1
    subq          $a1,1,$t0
    and           $a1,$t0,$v0
    sra           $a0,63,$a3
    negq          $a0,$a2
    cmovlt        $a0,$a2,$a0
    beq           $v0,rl_pwr2
    lda           $at,-0x200($a1)
    bgt           $at,rl_lrgdiv
    addq          $a1,$a1,$v0
    s8addq        $v0,$t12,$t12
    ldq           $t0,0x8($t12)
    ldq           $a2,($t12)
    umulh         $a0,$t0,$v0
    addq          $v0,$a0,$v0
    srl           $v0,$a2,$v0
    mulq          $v0,$a1,$v0
    subq          $a0,$v0,$v0
    xor           $v0,$a3,$v0
    subq          $v0,$a3,$v0
    ret           $zero,($ra)
rl_pwr2:
    subq          $zero,$a0,$a2
    cmovlt        $a0,$a2,$a0
    and           $a0,$t0,$v0
    beq           $a1,divzer
    xor           $v0,$a3,$v0
    subq          $v0,$a3,$v0
    ret           $zero,($ra)
rl_lrgdiv:
    bsr           $at,div64
    xor           $t0,$a3,$v0
    subq          $v0,$a3,$v0
    ret           $zero,($ra)

_OtsModulus64:
    ldah          $t12,h^_OtsDivData($zero)
    lda           $t12,l^_OtsDivData($t12)
    negq          $a1,$a2
    cmovge        $a1,$a1,$a2
    subq          $a2,1,$t0
    beq           $a1,divzer
    and           $a2,$t0,$v0
    beq           $v0,ml_p2
    xor           $a0,$a1,$at
    clr           $a3
    cmovlt        $at,$a1,$a3
    and           $a0,$a1,$at
    mov           $a2,$a1
    negq          $a0,$a2
    cmovlt        $a0,$a2,$a0
    cmplt         $at,$zero,$v0
    sll           $v0,63,$v0
    bis           $v0,$a3,$a3
    bsr           $at,div64
    cmoveq        $t0,$zero,$a3
    addq          $a3,$a3,$a2
    subq          $a3,1,$at
    negq          $t0,$v0
    cmovlt        $at,$t0,$v0
    cmoveq        $a2,$zero,$a3
    addq          $a3,$v0,$v0
    ret           $zero,($ra)
ml_p2:
    cmovge        $a1,$zero,$a1
    and           $a0,$t0,$t0
    cmoveq        $t0,$zero,$a1
    addq          $a1,$t0,$v0
    ret           $zero,($ra)
    nop

_OtsDivide64Unsigned:
    ldah          $t12,h^_OtsDivData($zero)
    lda           $t12,l^_OtsDivData($t12)
    lda           $at,-0x200($a1)
    blt           $a1,dul_big
    addq          $a1,$a1,$a2
    srl           $a0,33,$a3
    beq           $a1,divzer
    s8addq        $a2,$t12,$a2
    bgt           $at,dul_lrgdiv
    cmpule        $a1,$a0,$v0
    bne           $a3,dul_64bit
    ldq           $t12,($a2)
    beq           $v0,dul_end
    blt           $t12,dul_smpwr2
    umulh         $a0,$t12,$v0
    ret           $zero,($ra)
    nop
dul_64bit:
    ldq           $t0,0x8($a2)
    ldq           $t12,($a2)
    beq           $v0,dul_end
    beq           $t0,dul_smpwr2
    umulh         $a0,$t0,$v0
    zap           $a0,0x0f,$a2
    zapnot        $a0,0x0f,$a0
    srl           $a2,$t12,$a2
    addq          $v0,$a0,$v0
    srl           $v0,$t12,$v0
    addq          $v0,$a2,$v0
    ret           $zero,($ra)
dul_smpwr2:
    srl           $a0,$t12,$v0
dul_end:
    ret           $zero,($ra)
dul_lrgdiv:
    bsr           $at,div64
    ret           $zero,($ra)
dul_big:
    cmpule        $a1,$a0,$v0
    ret           $zero,($ra)

_OtsRemainder64Unsigned:
    ldah          $t12,h^_OtsDivData($zero)
    lda           $t12,l^_OtsDivData($t12)
    lda           $at,-0x200($a1)
    subq          $a1,1,$t0
    blt           $a1,rul_big
    and           $a1,$t0,$a2
    bgt           $at,rul_lrgdiv
    addq          $a1,$a1,$v0
    beq           $a2,rul_pwr2
    s8addq        $v0,$t12,$t12
    ldq           $t0,0x8($t12)
    cmpult        $a0,$a1,$a2
    bne           $a2,rul_lss
    ldq           $a3,($t12)
    umulh         $a0,$t0,$v0
    blt           $a0,rul_carry
    addq          $v0,$a0,$v0
    srl           $v0,$a3,$v0
    mulq          $v0,$a1,$v0
    subq          $a0,$v0,$v0
    ret           $zero,($ra)
rul_carry:
    zap           $a0,0x0f,$a2
    zapnot        $a0,0x0f,$t0
    srl           $a2,$a3,$a2
    addq          $v0,$t0,$v0
    srl           $v0,$a3,$v0
    addq          $v0,$a2,$v0
    mulq          $v0,$a1,$v0
    subq          $a0,$v0,$v0
    ret           $zero,($ra)
rul_pwr2:
    beq           $a1,divzer
    and           $a0,$t0,$v0
    ret           $zero,($ra)
rul_lss:
    mov           $a0,$v0
    ret           $zero,($ra)
rul_big:
    cmpult        $a0,$a1,$t0
    subq          $a0,$a1,$v0
    cmovne        $t0,$a0,$v0
    ret           $zero,($ra)
    nop
rul_lrgdiv:
    bsr           $at,div64
    mov           $t0,$v0
    ret           $zero,($ra)

divzer:
    mov           0xfffffffe,$a0
    clr           $v0
    clr           $t0
    call_pal      0x000000aa    // PAL_gentrap
    ret           $zero,($ra)

.new_section .pdata, "dr4"

//  0000                Procedure descriptor for .text
.long   .text                   //  BeginAddress      : 0
.long   .text+0x7e4             //  EndAddress        : 2020
.long   00000000                //  ExceptionHandler  : 0
.long   00000000                //  HandlerData       : 0
.long   .text+0x724             //  PrologEnd         : 1828

