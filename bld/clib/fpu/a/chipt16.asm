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


; static char sccs_id[] = "@(#)fptan16.asm      1.3  12/15/94  15:21:28";
; static char sccs_id[] = "@(#)fptan16.asm      1.4  12/20/94  16:51:49";
;
; This code is being published by Intel to users of the Pentium(tm)
; processor.  Recipients are authorized to copy, modify, compile, use and
; distribute the code.
;
; Intel makes no warranty of any kind with regard to this code, including
; but not limited to, implied warranties or merchantability and fitness for
; a particular purpose. Intel assumes no responsibility for any errors that
; may appear in this code.
;
; No patent licenses are granted, express or implied.
;
;
;  The following code is a PRELIMINARY IMPLEMENTATION of a
;  software patch for the floating point divide instructions.
;
;
include mdef.inc

.386
.387

PATCH_CW        EQU     00ch
PREV_CW         EQU     010h
COSINE          EQU     000h            ; These two are overlaid because they
ANGLE           EQU     000h            ; are not live at the same time.


STACK_SIZE      EQU     014h

ONESMASK        EQU     0e00h


_TEXT  SEGMENT   DWORD USE16 PUBLIC 'CODE'

fdiv_risk_table DB      0, 1, 0, 0, 4, 0, 0, 7, 0, 0, 10, 0, 0, 13, 0, 0
fdiv_scale      DD      03f700000h              ; 0.9375
one_shl_63      DD      05f000000h



        assume cs:_TEXT, ds:nothing, es:nothing, ss:nothing
        public  __fptan_chk

;
;  PRELIMINARY VERSION for register-register divides.
;


        defpe   __fptan_chk
        push    bp
        push    ax
        sub     sp, STACK_SIZE
        mov     bp, sp
        fstp    tbyte ptr [bp+ANGLE]
        mov     ax, [bp+ANGLE+8]
        and     ax, 07fffh
        jz      use_hardware            ; denormals, ...
        cmp     ax, 07fffh
        je      use_hardware            ; NaNs, infinities, ...
        mov     ax, [bp+ANGLE+6]
        add     ax, ax
        jnc     use_hardware            ; unnormals (explicit 1 missing)
        fld     tbyte ptr [bp+ANGLE]


;
; Check for proper parameter range ( |<angle>| < 2^63)
;
        fabs
        fcomp   one_shl_63
        fstsw  ax
        sahf
        jae     use_hardware

        fld     tbyte ptr [bp+ANGLE]
        fsincos
        fstp    tbyte ptr [bp+COSINE]
        fld     tbyte ptr [bp+COSINE]   ; load the denominator (cos(x))
        mov     ax, [bp+COSINE+6]       ; get mantissa bits 48-63
        add     ax,ax                   ; shift one's bit into carry
        xor     ax, ONESMASK            ; invert the bits that must be one
        test    ax, ONESMASK            ; test for needed bits
        jz      scale_if_needed         ; if all one, it may need scalling
        fdivp   st(1), st               ; OK to use the hardware
        fld1                            ; push 1.0 onto FPU stack
        add     sp, STACK_SIZE          ; restore stack size
        pop     ax
        pop     bp
        ret

scale_if_needed:
        shr     ax, 12                  ; keep first four bits after point
        push    bx                      ; save bx
        mov     bx,ax                   ; bx = index
        cmp     fdiv_risk_table[bx], ah ; check for (1,4,7,a,d)
        pop     bx                      ; restore bx
        jnz     divide_scaled
        fdivp   st(1), st               ; OK to use the hardware
        fld1                            ; push 1.0 onto FPU stack
        add     sp, STACK_SIZE          ; restore stack size
        pop     ax
        pop     bp
        ret

divide_scaled:
        fwait                           ; catch preceding exceptions
        fstcw   [bp+PREV_CW]            ; save caller's control word
        mov     ax, [bp+PREV_CW]
        or      ax, 033fh               ; mask exceptions, pc=80
        mov     [bp+PATCH_CW], ax
        fldcw   [bp+PATCH_CW]           ; mask exceptions & pc=80
        fmul    fdiv_scale              ; scale denominator by 15/16
        fxch
        fmul    fdiv_scale              ; scale numerator by 15/16
        fxch

; This assures correctly rounded result if pc=64 as well

        fldcw   [bp+PREV_CW]            ; restore caller's control word
        fdivp   st(1), st               ; OK to use the hardware
        fld1                            ; push 1.0 onto FPU stack
        add     sp, STACK_SIZE          ; restore stack size
        pop     ax
        pop     bp
        ret

use_hardware:
        fld     tbyte ptr [bp+ANGLE]
        fptan
        add     sp, STACK_SIZE          ; restore stack size
        pop     ax
        pop     bp
        ret

__fptan_chk      ENDP

_TEXT   ENDS
        end
