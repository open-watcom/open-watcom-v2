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


.globl          ctrl
.globl          foo
.text
ctrl:
    lda            $sp,-0x8($sp)
    stq            $ra,($sp)
    ldah           $v0,h^L8($zero)
    lda            $v0,l^L8($v0)
    sextl          $v0,$a0
    bsr            $ra,j^printf
    jsr            foo
    sextl          $v0,$a0
    bne            $a0,L2
L1:
    ldah           $v0,h^La($zero)
    lda            $v0,l^La($v0)
    sextl          $v0,$a0
    jsr            $ra,printf
    jsr            L3
L2:
    ldah           $v0,h^Lc($zero)
    lda            $v0,l^Lc($v0)
    sextl          $v0,$a0
    bsr            $ra,j^printf
    bsr            $ra,foo
    sextl          $v0,$a0
    bne            $a0,L1
L3:
    ldah           $v0,h^Le($zero)
    lda            $v0,l^Le($v0)
    sextl          $v0,$a0
    bsr            $ra,j^printf
    ldq            $ra,($sp)
    lda            $sp,0x8($sp)
    ret            ($ra),1
foo:
    mov            0x00000001,$v0
    ret

.data
L8:
                .asciiz "First,\n"
La:
                .asciiz "Third,\n"
Lc:
                .asciiz "Second,\n"
Le:
                .asciiz "End.\n"


