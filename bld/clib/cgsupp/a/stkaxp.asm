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


        .text

        .globl  _WtcStkCrawl
        .globl  _WtcStkCrawlSize
        .globl  _WtcStkScribble
        .globl  _WtcStkCopy

        .set noat

/*
 * Note that these routines are called from the prolog of functions,
 * and so are able to destroy registers which are volatile across
 * standard routines.
 */

_WtcStkCrawl:
        cmple   $fp,$sp,$t0
        bne     $t0,done
        mov     $fp,$t0
l1:     ldq     $zero,-8($t0)
        lda     $t0,-4096($t0)
        cmplt   $sp,$t0,$t1
        bne     $t1,l1
done:   ret

_WtcStkCrawlSize:
        addl    $t0,$sp,$t0             /* parm passed in - size of frame */
l3:     ldq     $zero,-8($t0)
        lda     $t0,-4096($t0)
        cmplt   $sp,$t0,$t1
        bne     $t1,l3
        ret     $zero,($v0),1

_WtcInitPattern:.long   0xdeadbeef
                .long   0xdeadbeef

_WtcStkScribble:
        ldah    $t1,h^_WtcInitPattern
        lda     $t1,l^_WtcInitPattern($t1)
        ldq     $t2,($t1)
        addl    $t0,$sp,$t0             /* parm passed in - size of frame */
l2:     stq     $t2,-8($t0)
        lda     $t0,-8($t0)
        cmplt   $sp,$t0,$t1
        bne     $t1,l2
fini:   ret     $zero,($v0),1

/*
 * This is a routine to copy the parm-cache area of the stack
 * down after an alloca has sliced off a new hunk of stack-storage.
 * The size of the new area just sliced off the stack is passed in
 * t0, and the size of the parm save area is passed in t1. The routine
 * is allowed to destroy these registers, as well as the scratch reg
 * and t2.
 */
_WtcStkCopy:
        addl    $sp,$t0,$at
        addl    $at,$t1,$at     // make $at point past last quad of data to copy
        addl    $sp,$t0,$t1     // make $t1 point at start of parm-cache block
        mov     $sp,$t0         // $t0 points at start of new parm-cache block
l4:     ldq     $t2,($t1)
        addl    $t1,8,$t1
        stq     $t2,($t0)
        addl    $t0,8,$t0
        cmplt   $t1,$at,$t2
        bne     $t2,l4
        ret
