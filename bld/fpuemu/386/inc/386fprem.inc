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


        xdefp   __fprem
        xdefp   ___fprem

;                     EDX:EAX    ECX:EBX
;       double fprem( double x, double modulus, int *quot )
;       int fprem( long double *x, long double *modulus )
;                       EAX             EDX
;

        defp    __fprem

        _guess                  ; guess: number is 0
          cmp   word ptr 8[EAX],0; - quit if not 0
          _quif ne              ; - ...
          cmp   dword ptr 4[EAX],0; - quit if not 0
          _quif ne              ; - ...
          cmp   dword ptr [EAX],0; - quit if not 0
          _quif ne              ; - ...
          sub   EAX,EAX         ; - integer quotient is 0
          ret                   ; - return
        _endguess               ; endguess
        _guess                  ; guess: modulus is 0
          cmp   word ptr 8[EDX],0; - quit if not 0
          _quif ne              ; - ...
          cmp   dword ptr 4[EDX],0; - quit if not 0
          _quif ne              ; - ...
          cmp   dword ptr [EDX],0; - quit if not 0
          _quif ne              ; - ...
          sub   EDX,EDX         ; - set result to 0
          mov   [EAX],EDX       ; - ...
          mov   4[EAX],EDX      ; - ...
          mov   8[EAX],DX       ; - ...
          sub   EAX,EAX         ; - integer quotient is 0
          ret                   ; - return
        _endguess               ; endguess

        push    EBP             ; save EBP
        push    ESI             ; save ESI
        push    EDI             ; save EDI
        push    EDX             ; save EDX
        push    ECX             ; save ECX
        push    EBX             ; save EBX
        push    EAX             ; save address of operand

        mov     SI,8[EAX]       ; get exponent of op1
        mov     DI,8[EDX]       ; get exponent of op2
        mov     ECX,4[EDX]      ; get modulus
        mov     EBX,[EDX]       ; ...
        mov     EDX,4[EAX]      ; get operand
        mov     EAX,[EAX]       ; ...
        call    ___fprem        ; calc remainder

        pop     ECX             ; restore address of operand
        mov     [ECX],EAX       ; store the remainder
        mov     4[ECX],EDX      ; ...
        mov     8[ECX],SI       ; ...
        mov     EAX,EDI         ; store quotient
        pop     EBX             ; restore EBX
        pop     ECX             ; restore ECX
        pop     EDX             ; restore EDX
        pop     EDI             ; restore EDI
        pop     ESI             ; restore ESI
        pop     EBP             ; restore EBP
        ret                     ; return

        endproc __fprem


        defp    ___fprem

        push    ESI             ; save sign of operand
        push    EDI             ; save high part of modulus
        and     ESI,00007FFFh   ; isolate exponent
        and     EDI,00007FFFh   ; isolate exponent of modulus
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
            dec   ESI           ; - - adjust difference in exponents
            jl    _done         ; - - quit if done
            _shl  EAX,1         ; - - shift dividend left
            _rcl  EDX,1         ; - - . . .
            jc    try

;   If bit 5 of dividend is set here, we didnt subtract the modulus from the
;   dividend (recall that the divisor has a 1 in the msb -- if we subtracted
;   it from the dividend without a borrow, the dividend would not have a one
;   in its msb to be shifted into bit 5 tested for in the condition above. If
;   we are rotating a bit into bit 5, the dividend is now big enough that we
;   can be sure of subtracting out the divisor without a borrow, as we have
;   shifted it left one digit.
            or    EDX,EDX       ; - - check top bit of dividend
          _until  ns            ; - until
          jmp   short didnt_go  ; - continue
_done:    sub   ESI,ESI         ; - set ESI to 0
;         normalize the remainder in AL:BX:CX:DX
          _guess                ; - guess: number is zero
            or    EAX,EAX       ; - - quit if not zero
            _quif ne            ; - - ...
            or    EDX,EDX       ; - - ...
            _quif ne            ; - - ...
          _admit                ; - admit: not zero
            _loop               ; - - loop
              or    EDX,EDX     ; - - - quit if number is normalized
              _quif s           ; - - - . . .
              _rcl  EAX,1       ; - - - shift result left
              _rcl  EDX,1       ; - - - ...
              dec   ESI         ; - - - decrement exponent
            _endloop            ; - - endloop
            pop   ECX           ; - - get high part of modulus
            push  ECX           ; - - save it again
            and   ECX,00007FFFh ; - - isolate exponent of modulus
            add   ESI,ECX       ; - - adjust exponent of result
          _endguess             ; - endguess
        _else                   ; else
          add   ESI,EDI         ; - restore exponent
          sub   EDI,EDI         ; - set quotient to 0
        _endif                  ; endif
        pop     ECX             ; restore high part of modulus
        pop     EBX             ; restore sign
        and     EBX,00008000h   ; isolate sign bit
        or      ESI,EBX         ; merge sign with exponent
        or      EDX,EDX         ; test high word of remainder
        _if     e               ; if remainder is zero
          sub   ESI,ESI         ; - clear the sign and exponent
        _endif                  ; endif
        xor     BX,CX           ; calc sign of quotient
        _if     s               ; if quotient should be negative
          neg   EDI             ; - negate quotient
        _endif                  ; endif
        ret                     ; return

        endproc ___fprem

