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


// Testing semi-colon statement separator

.globl factorial

.text
factorial:
lda $sp,-0x10($sp); stq $ra,0x8($sp); stq $s0,0x0($sp); bis $zero,$a0,$s0; zapnot $s0,0xf,$a0; cmpule $a0,0x1,$a0; bne $a0,L1; br $zero,L2; L1: lda $v0,0x1($zero); br $zero,L3; L2: lda $v0,-0x1($zero); addl $s0,$v0,$v0; zapnot $v0,0xf,$a0; bsr $ra,factorial; mull $s0,$v0,$v0; L3: ldq $s0,0x0($sp); ldq $ra,0x8($sp); lda $sp,0x10($sp); ret $zero,($ra),0x1

null_stmts: ; ;
