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


include mdef.inc
include struct.inc

        modstart        fprem386

        xdefp   __fprem_

;
;                     EDX:EAX    ECX:EBX
;       void fprem( double x, double modulus, int *quot, double *rem )
;
        defpe   __fprem_
        push    EBP             ; save BP
        mov     EBP,ESP         ; get access to parms
        push    EDX             ; save registers
        push    ECX             ; ...
        push    EBX             ; ...
        push    EAX             ; ...
        mov     EAX,8[EBP]      ; get argument x
        mov     EDX,12[EBP]     ; ...
        mov     EBX,16[EBP]     ; get modulus
        mov     ECX,20[EBP]     ; ...
        or      ECX,ECX         ; if modulus is zero
        _if     e               ; then
          sub   EAX,EAX         ; - set result to 0
          mov   EBX,24[EBP]     ; - quot = 0
          mov   [EBX],EAX       ; - ...
          mov   EBX,28[EBP]     ; - remainder = 0
          mov   [EBX],EAX       ; - ...
          mov   4[EBX],EAX      ; - ...
          pop   EAX             ; - restore registers
          pop   EBX             ; - ...
          pop   ECX             ; - ...
          pop   EDX             ; - ...
          pop   EBP             ; - restore EBP
          ret                   ; - return
        _endif                  ; endif
        push    ESI             ; save ESI
        push    EDI             ; save EDI
        push    EDX             ; save sign of operand
        push    ECX             ; save high part of modulus

        mov     ESI,EDX         ; get most sig word of op1
        mov     EDI,ECX         ; get most sig word of op2
        and     ESI,7FF00000h   ; isolate exponent
        and     EDI,7FF00000h   ; isolate exponent of modulus
        and     EDX,000FFFFFh   ; isolate mantissa of op1
        and     ECX,000FFFFFh   ; isolate mantissa of modulus
        or      EDX,00100000h   ; set implied 1 bit
        or      ECX,00100000h   ; ...
        sub     ESI,EDI         ; calculate difference in exponents
        _if     ge              ; if operand >= modulus
          sub   EDI,EDI         ; - set quotient to 0
          _loop                 ; - loop
            _guess              ; - - guess
              cmp    ECX,EDX    ; - - - The purpose of this guess is to
              _quif  ne         ; - - - determine if the divisor will subtract
              cmp  EBX,EAX      ; - - -
              je   try          ; - - -
            _endguess           ; - - endguess
            _if   c             ; - - if the carry is set (ie the modulus will
                                ; - - - definitely subtract from the dividend
                                ; - - - without a borrow
try:
              sub  EAX,EBX      ; - - - subtract divisor from dividend
              sbb  EDX,ECX      ; - - - . . .
              stc               ; - - - set carry to indicate that modulus was
                                ; - - - successfully subtracted from dividend
            _endif              ; - - endif
didnt_go:   _rcl  EDI,1         ; - - rotate 1 (if carry set) into quotient word
            sub   ESI,00100000h ; - - adjust difference in exponents
            jl    _done         ; - - quit if done
            _shl  EAX,1         ; - - shift dividend left
            _rcl  EDX,1         ; - - . . .
            cmp   EDX,00200000h
            jae   try

; If bit 5 of dividend is set here, we didnt subtract the modulus from the
; dividend (recall that the divisor has a 1 in the msb -- if we subtracted
; it from the dividend without a borrow, the dividend would not have a one
; in its msb to be shifted into bit 5 tested for in the condition above. If
; we are rotating a bit into bit 5, the dividend is now big enough that we
; can be sure of subtracting out the divisor without a borrow, as we have
; shifted it left one digit.

            cmp   EDX,00100000h
          _until  b             ; - until

          cmc                   ; - flip the carry bit
          jmp   short didnt_go  ; - continue
_done:    sub   ESI,ESI         ; - set SI to 0
;         normalize the remainder in AL:BX:CX:DX
          _guess                ; - guess: number is zero
            or    EAX,EAX       ; - - quit if not zero
            _quif ne            ; - - ...
            or    EDX,EDX       ; - - ...
            _quif ne            ; - - ...
          _admit                ; - admit: not zero
            _loop               ; - - loop
              test  EDX,00200000h; - - - quit if number is normalized
              _quif ne          ; - - - . . .
              _rcl  EAX,1       ; - - - shift result left
              _rcl  EDX,1
              sub   ESI,00100000h; - - - decrement exponent
            _endloop            ; - - endloop
            shr   EDX,1         ; - - put in correct position
            rcr   EAX,1         ; - - . . .
            add   ESI,00100000h ; - - increment exponent
            pop   ECX           ; - - get high part of modulus
            push  ECX           ; - - save it again
            and   ECX,7FF00000h ; - - isolate exponent of modulus
            add   ESI,ECX       ; - - adjust exponent of result
          _endguess             ; - endguess
        _else                   ; else
          add   ESI,EDI         ; - restore exponent
          sub   EDI,EDI         ; - set quotient to 0
        _endif                  ; endif
        and     EDX,000FFFFFh   ; keep just the fraction
        add     EDX,ESI         ; update high order word
        pop     ECX             ; restore high part of modulus
        pop     ESI             ; restore sign
        and     ESI,080000000h  ; isolate sign bit
        or      EDX,EDX         ; test high word of remainder
        _if     ne              ; if remainder is non-zero
          or    EDX,ESI         ; - make remainder same sign as original opnd
        _endif                  ; endif
        xor     ESI,ECX         ; calc sign of quotient
        _if     s               ; if quotient should be negative
          neg   EDI             ; - negate quotient
        _endif                  ; endif
        mov     ESI,24[EBP]     ; get address of quotient
        mov     [ESI],EDI       ; store quotient
        mov     ESI,28[EBP]     ; get address of remainder
        mov     [ESI],EAX       ; store remainder
        mov     4[ESI],EDX      ; ...
        pop     EDI             ; restore EDI
        pop     ESI             ; restore ESI
        pop     EAX             ; restore registers
        pop     EBX             ; ...
        pop     ECX             ; ...
        pop     EDX             ; ...
        pop     EBP             ; restore EBP
        ret                     ; return
        endproc __fprem_


        endmod
        end
