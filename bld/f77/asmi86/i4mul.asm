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


include struct.inc
include mdef.inc

        modstart        i4mul

        xdefp   ChkI4Mul_
defp    ChkI4Mul_
        push    DI              ; save registers
        push    SI              ; ...
        push    BP              ; ...
        push    DS              ; ...
        mov     DS,DX           ; get address of op1
        mov     SI,AX           ; ...
        mov     AX,[SI]         ; get value of op1
        mov     DX,2[SI]        ; ...
        sub     DI,DI           ; set table index to zero
        mov     BP,DX           ; save high word of arg 1
        cwd                     ; if arg 1 can be represented in 16 bits
        cmp     DX,BP           ; ...
        _if     e               ; ...
          or    DI,0002h        ; - set index to (16*?)
        _endif                  ; endif
        xchg    AX,BX           ; if arg 2 can be represented in 16 bits
        cwd                     ; ...
        cmp     DX,CX           ; ...
        _if     e               ; ...
          or    DI,0004h        ; - merge index to (?*16)
        _endif                  ; endif
        xchg    AX,BX           ; get everything back in the right regs
        mov     DX,BP           ; ...
        mov     DI,word ptr CS:MulRtn[DI]; do multiply
        call    DI              ; ...
        mov     [SI],AX         ; save result over op1
        mov     2[SI],DX        ; ...
        mov     AX,CX           ; get overflow flag
        pop     DS              ; restore registers
        pop     BP              ; ...
        pop     SI              ; ...
        pop     DI              ; ...
        ret                     ; return
MulRtn: dw      i4i4mul
        dw      i2i4mul
        dw      i4i2mul
        dw      i2i2mul
endproc ChkI4Mul_


defn    i2i2mul                 ; 16-bit * 16-bit
        imul    BX              ; arg1 * arg2
        mov     CX,0            ; indicate no overflow
        ret                     ; return
endproc i2i2mul


defn    i4i2mul                 ; 32-bit * 16-bit
        xchg    AX,BX           ; exchange numbers
        xchg    DX,CX           ; ...
;;;;;;;;hop     i2i4mul         ; fall through to i2i4mul
endproc i4i2mul


defn    i2i4mul                 ; 16-bit * 32-bit
        mov     DI,AX           ; save L1
        imul    CX              ; H3 := Low_word_of( L1 * H2 )
        or      DI,DI           ; if( L1 < 0 )then
        _if     s               ; ...
          sub   AX,BX           ; - H3 += Low_word_of( -1 * L2 )
        _endif                  ; endif
        xchg    DI,AX           ; save H3 and restore L1
        mov     CX,DX           ; save overflow part
        mul     BX              ; L3 := Low_word_of( L1 * L2 )
        add     DX,DI           ; H3 += High_word_of( L1 * L2 )
        adc     CX,0            ; ...
        mov     DI,AX           ; save low part of result
        mov     AX,DX           ; get sign of result
        cwd                     ; ...
        cmp     CX,DX           ; check for overflow
        mov     CX,0            ; assume no overflow
        _if     ne              ; if overflow
          inc   CX              ; - indicate overflow
        _endif                  ; endif
        mov     DX,AX           ; return the result
        mov     AX,DI           ; ...
        ret                     ; return
endproc i2i4mul


defn    i4i4mul                 ; I*4 times I*4 (almost always overflows)
        mov     DI,DX           ; check if top  part is 0 of both ints
        or      DI,CX           ; ...
        je      u2u2mul         ; multiply to unsigned shorts into long
        mov     DI,DX           ; save H1
        mov     BP,AX           ; save L1
        imul    CX              ; H3 := Low_word_of( H2 * L1 )
        xchg    AX,DI           ; H1 <-> H3
        imul    BX              ; H3 += Low_word_of( H1 * L2 )
        add     DI,AX           ; ...
        mov     AX,BP           ; get L1 in a register we can multiply
        mul     BX              ; L3 := Low_word_of( L1 * L1 )
        add     DX,DI           ; finish calculating H3
        mov     CX,1            ; indicate overflow
        ret                     ; return
endproc i4i4mul

defn    u2u2mul                 ; u*2 times u*2, ( we assume cx is 0 )
        mul     BX              ; do multiply of low words
        or      DX,DX           ; check sign bit
        _if     s               ; ...
          inc   CX              ; set overflow
        _endif                  ; ...
        ret                     ; return
endproc u2u2mul
        endmod
        end
