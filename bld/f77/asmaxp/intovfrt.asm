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
// intovfrt       : Integer add/sub/mul with overflow check
//

#include "axp_regs.h"

.globl  __Add
// .ent __Add
__Add:
        ldl     t0, (a0)                // load argument 1
        ldl     t1, (a1)                // load argument 2
        addl    t0, t1, t2              // add 32 bit integers
        addq    t0, t1, t3              // add 64 bit integers
        cmpeq   t2, t3, v0              // check if they are the same,
        xor     v0, 1, v0               // ...
        stl     t2, (a0)                // Save result
        ret     zero, (ra)              // Use same routine as to switch in
// .end __Add

.globl  __Sub
// .ent __Sub
__Sub:
        ldl     t0, (a0)                // load argument 1
        ldl     t1, (a1)                // load argument 2
        subl    t0, t1, t2              // multiply 32 bit integers
        subq    t0, t1, t3              // multiply 64 bit integers
        cmpeq   t2, t3, v0              // check if they are the same,
        xor     v0, 1, v0               // ...
        stl     t2, (a0)                // Save result
        ret     zero, (ra)              // Use same routine as to switch in
// .end __Sub

.globl  __Mul
// .ent __Mul
__Mul:
        ldl     t0, (a0)                // load argument 1
        ldl     t1, (a1)                // load argument 2
        mull    t0, t1, t2              // multiply 32 bit integers
        mulq    t0, t1, t3              // multiply 64 bit integers
        cmpeq   t2, t3, v0              // check if they are the same,
        xor     v0, 1, v0               // ...
        stl     t2, (a0)                // Save result
        ret     zero, (ra)              // Use same routine as to switch in
// .end __Mul

