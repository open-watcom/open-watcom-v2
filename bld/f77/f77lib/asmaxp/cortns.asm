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


//
// CORTNS       : i/o co-routines (common to READ and WRITE)
//

#include "axp_regs.h"
#include "ptypes.h"

// external symbols used by this module
.extern __ReleaseIOSys

.extern IORslt
.extern __ASTACKSIZ
.extern __ASTACKPTR
.extern __SwitchStkLow

.data
.align 6
SaveRegs: // assembler does not support .quad directive
        .long   0       // sp
        .long   0
        .long   0       // ra
        .long   0

.align 0
.text

// README !!!
// Setting the alternative stack (__ASTACKSIZ) size has been moved to a C file.

#define FRAME_SIZE      0x90            // define stack frame to discard when restoring
                                        // state (8 registers and return address)

.globl  SwitchToGen
// .ent SwitchToGen
        br      zero, j^SwitchToRT      // Use same routine as to switch in
// .end SwitchToGen

.globl  SwitchToRT
// .ent SwitchToRT
SwitchToRT:
        ldah    t0, h^SaveRegs(zero)    // prepare loading register
        lda     t0, l^SaveRegs(t0)      // ...
        lda     sp, -FRAME_SIZE(sp)     // Push current env onto stack
        stq     s0, 0x00(sp)            // ...
        stq     s1, 0x08(sp)            // ...
        stq     s2, 0x10(sp)            // ...
        stq     s3, 0x18(sp)            // ...
        stq     s4, 0x20(sp)            // ...
        stq     s5, 0x28(sp)            // ...
        stq     s6, 0x30(sp)            // ...
        stq     gp, 0x38(sp)            // ...
        stq     ra, 0x40(sp)            // including the return address

        stt     f2, 0x48(sp)            // floating point regs
        stt     f3, 0x50(sp)            // ...
        stt     f4, 0x58(sp)            // ...
        stt     f5, 0x60(sp)            // ...
        stt     f6, 0x68(sp)            // ...
        stt     f7, 0x70(sp)            // ...
        stt     f8, 0x78(sp)            // ...
        stt     f9, 0x80(sp)            // ...

        lda     t1, (sp)                // switch stacks
        ldq     sp, 0x00(t0)            // ...
        stq     t1, 0x00(t0)            // ...

        lda     s0, 0(v0)               // save return value
        ldah    t0, h^__SwitchStkLow(zero) // switch stack low values
        lda     t0, l^__SwitchStkLow(t0)// ...
        ldl     t0, 0(t0)               // ...
        jsr     ra, (t0)                // do switch
        lda     v0, 0(s0)               // restore return value

        ldq     s0, 0x00(sp)            // Load new environment
        ldq     s1, 0x08(sp)            // ...
        ldq     s2, 0x10(sp)            // ...
        ldq     s3, 0x18(sp)            // ...
        ldq     s4, 0x20(sp)            // ...
        ldq     s5, 0x28(sp)            // ...
        ldq     s6, 0x30(sp)            // ...
        ldq     gp, 0x38(sp)            // ...
        ldq     ra, 0x40(sp)            // ...

        ldt     f2, 0x48(sp)            // floating point regs
        ldt     f3, 0x50(sp)            // ...
        ldt     f4, 0x58(sp)            // ...
        ldt     f5, 0x60(sp)            // ...
        ldt     f6, 0x68(sp)            // ...
        ldt     f7, 0x70(sp)            // ...
        ldt     f8, 0x78(sp)            // ...
        ldt     f9, 0x80(sp)            // ...

        lda     sp, FRAME_SIZE(sp)      // destroy frame
        ret     zero, (ra)              // return
// .end SwitchToRT


// on entry a0 contains address of read or write routine
//          ra contains the return address
//
.globl  RdWrCommon
// .ent RdWrCommon
RdWrCommon:
        ldah    t0, h^SaveRegs(zero)    // prepare loading register
        lda     t0, l^SaveRegs(t0)      // ..
        stq     ra, 0x08(t0)            // including the return address

        lda     sp, -FRAME_SIZE(sp)     // store our environment on stack
        stq     s0, 0x00(sp)            // save CG registers
        stq     s1, 0x08(sp)            // ...
        stq     s2, 0x10(sp)            // ...
        stq     s3, 0x18(sp)            // ...
        stq     s4, 0x20(sp)            // ...
        stq     s5, 0x28(sp)            // ...
        stq     s6, 0x30(sp)            // ...
        stq     gp, 0x38(sp)            // ...
        stq     ra, 0x40(sp)            // including the return address

        stt     f2, 0x48(sp)            // floating point regs
        stt     f3, 0x50(sp)            // ...
        stt     f4, 0x58(sp)            // ...
        stt     f5, 0x60(sp)            // ...
        stt     f6, 0x68(sp)            // ...
        stt     f7, 0x70(sp)            // ...
        stt     f8, 0x78(sp)            // ...
        stt     f9, 0x80(sp)            // ...

        stq     sp, (t0)                // save stack pointer of executing code
        ldah    sp, h^__ASTACKPTR(zero) // get start of new stack
        lda     sp, l^__ASTACKPTR(sp)   // ...
        ldl     sp, 0(sp)               // ...


        lda     sp, -0x08(sp)           // make stack space
        stq     a0, 0x00(sp)            // save parameter value
        ldah    t0, h^__SwitchStkLow(zero) // switch stack low values
        lda     t0, l^__SwitchStkLow(t0)// ...
        ldl     t0, 0x00(t0)            // ...
        jsr     ra, (t0)                // do switch
        ldq     a0, 0x00(sp)            // restore return value
        lda     sp, 0x08(sp)            // destroy stack space

        // a0 still contains the io routine address
        jsr_coroutine   ra,(a0)         // start i/o operation
        ldah    t0, h^SaveRegs(zero)    // prepare loading register
        lda     t0, l^SaveRegs(t0)      // ..
        ldq     sp, 0x00(t0)            // return sp
        ldq     ra, 0x08(t0)            // return ra

        bne     v0, no_update           // check if i/o error then do not update
          ldq   s0, 0x00(sp)            // update CG registers
          ldq   s1, 0x08(sp)            // ...
          ldq   s2, 0x10(sp)            // ...
          ldq   s3, 0x18(sp)            // ...
          ldq   s4, 0x20(sp)            // ...
          ldq   s5, 0x28(sp)            // ...
          ldq   s6, 0x30(sp)            // ...
          ldq   gp, 0x38(sp)            // ...
          ldq   ra, 0x40(sp)            // ...

          ldt   f2, 0x48(sp)            // floating point regs
          ldt   f3, 0x50(sp)            // ...
          ldt   f4, 0x58(sp)            // ...
          ldt   f5, 0x60(sp)            // ...
          ldt   f6, 0x68(sp)            // ...
          ldt   f7, 0x70(sp)            // ...
          ldt   f8, 0x78(sp)            // ...
          ldt   f9, 0x80(sp)            // ...

no_update:                              // Do not restore from stack because
                                        // regs are same as initial call
        stq     v0, 0x08(sp)            // save result of io operation
        stq     ra, 0x00(sp)            // release i/o system
        ldah    t0, h^__SwitchStkLow(zero) // switch stack low values
        lda     t0, l^__SwitchStkLow(t0)// ...
        ldl     t0, 0x00(t0)            // ...
        jsr     ra, (t0)                // ...
        bsr     ra, j^__ReleaseIOSys    // ...
        ldq     ra, 0x00(sp)            // ...
        ldq     v0, 0x08(sp)            // ...
        lda     sp, FRAME_SIZE(sp)      // clear stack frame
        ret     zero, (ra)              // return
// .end RdWrCommon


.globl  IOType
// .ent IOType                          // return to generated code
IOType:
        lda     v0, (zero)              // indicate operation succeeded
        br      zero, j^SwitchToRT      // Use same routine as to switch in
// .end IOType


.globl  IOChar
// .ent IOChar
IOChar:
        ldah    t0, h^IORslt(zero)      // load IORslt pointer
        lda     t0, l^IORslt(t0)        // ...
        ldq     a0, (a0)                // Load SCB (its only 8 bytes :)
        stq     a0, (t0)                // Store SCB in IORslt
        lda     v0, PT_CHAR(zero)       // return CHARACTER*n type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end IOChar


.globl  IOStr
// .ent IOStr
IOStr:
        ldah    t0, h^IORslt(zero)      // load IORslt pointer
        lda     t0, l^IORslt(t0)        // ...
        stq     a0, (t0)                // Store SCB in IORslt
        lda     v0, PT_CHAR(zero)       // return CHARACTER*n type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end IOStr


.globl  IOArr
// .ent IOArr                           // put array descriptor in IORslt
IOArr:
        ldah    t0, h^IORslt(zero)      // load IORslt pointer
        lda     t0, l^IORslt(t0)        // ...
        stl     a0, 0x00(t0)            // Store array record in IORslt (address)
        stl     a1, 0x04(t0)            // number of elements
        stl     a2, 0x0c(t0)            // type
        lda     v0, PT_ARRAY(zero)      // return ARRAY type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end IOArr


.globl  IOChArr
// .ent IOChArr                         // put array descriptor in IORslt
IOChArr:
        ldah    t0, h^IORslt(zero)      // load IORslt pointer
        lda     t0, l^IORslt(t0)        // ...
        stl     a0, (t0)                // Store array record in IORslt (address)
        stl     a1, 0x04(t0)            // number of elements
        stl     a2, 0x08(t0)            // element size
        lda     a1, PT_CHAR(zero)       // set type
        stl     a1, 0x0c(t0)            // ...
        lda     v0, PT_ARRAY(zero)      // return ARRAY type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end IOChArr


.globl  __RT_EndIO
// .ent __RT_EndIO
__RT_EndIO:
        lda     v0, PT_NOTYPE           // return "no i/o items remaining"
        bsr     zero, j^SwitchToRT      // return to caller of IOType()
// .end __RT_EndIO
