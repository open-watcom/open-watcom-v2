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


        xdefp   __FLDD
        xdefp   ___LDD

; void __FLDD( long double *op1 , long double *op2, long double *result )
;       EAX - pointer to op1
;       EDX - pointer to op2
;       EBX - pointer for result
;
sign    equ     -8
den     equ     sign-8
quot    equ     den-12
lo      equ     0
hi      equ     4

        defp    __FLDD
        push    ESI             ; save ESI
        push    ECX             ; save ECX
        push    EBX             ; save EBX
        mov     SI,8[EDX]       ; load op2
        mov     ECX,4[EDX]      ; ...
        mov     EBX,[EDX]       ; ...
        shl     ESI,16          ; shift to the top
        mov     SI,8[EAX]       ; load op1
        mov     EDX,4[EAX]      ; ...
        mov     EAX,[EAX]       ; ...
        call    ___LDD          ; do the divide
        pop     EBX             ; restore pointer to result
        mov     [EBX],EAX       ; store result
        mov     4[EBX],EDX      ; ...
        mov     8[EBX],SI       ; ...
        pop     ECX             ; restore ECX
        pop     ESI             ; restore ESI
        ret                     ; return
;
;       op1 is a NaN or infinity
;
divnan1:_guess                  ; guess:
          dec   SI              ; - adjust low exponent
          add   ESI,10000h      ; - increment high exponent
          _quif c               ; - quit if NaN
          _quif o               ; - ...
          ; op2 is not a NaN or infinity
          _if   s               ; - if op2 is negative
            or    EAX,EAX       ; - - check low word of op1
            _if   e             ; - - if could be infinity
              cmp   EDX,80000000h;- - - check high word
            _endif              ; - - endif
            _if   e             ; - - if op1 is infinity
              xor   SI,8000h    ; - - - flip the sign of op1
            _endif              ; - - endif
          _endif                ; - endif
          ret                   ; - return op1
        _endguess               ; endguess
        sub     ESI,10000h      ; readjust high exponent
;
;       both operands are NaNs and/or infinity
;
        _guess                  ; guess: both are infinities
          or    EAX,EAX         ; - quit if not infinity
          _quif ne              ; - ...
          or    EBX,EBX         ; - ...
          _quif ne              ; - ...
          cmp   EDX,80000000h   ; - ...
          _quif ne              ; - ...
          cmp   ECX,EDX         ; - ...
          _quif ne              ; - ...
          sar   EDX,1           ; - return -NaN
          mov   SI,0FFFFh       ; - ...
          ret                   ; - return
        _endguess               ; endguess
        _guess                  ; guess: fabs(op1) < fabs(op2)
          cmp   EDX,ECX         ; - compare high words
          _if   e               ; - if high words equal
            cmp EAX,EBX         ; - - compare low words
;           _if   e             ; - - if low words equal
;             mov   EAX,ESI     ; - - - get signs and exponents
;             shr   EAX,16      ; - - - shift op2 sign+exponent to bottom
;             cmp   SI,AX       ; - - - compare sign+exponents
;             mov   EAX,EBX     ; - - - restore EAX
;           _endif              ; - - endif
          _endif                ; - endif
          _quif a               ; - quit if op1 > op2
          mov   EDX,ECX         ; - load op2
          mov   EAX,EBX         ; - ...
          shr   ESI,16          ; - ...
        _endguess               ; endguess
        ret                     ; return
;
;       op1 is not a NaN or infinity, but op2 is
;
divnan2:sub     ESI,10000h      ; readjust high exponent
        _guess                  ; guess: op2 is +/- inf
          or    EBX,EBX         ; - quit if op2 is not inf
          _quif ne              ; - ...
          cmp   ECX,80000000h   ; - ...
          _quif ne              ; - ...
          mov   EAX,ESI         ; - get signs
          shl   EAX,16          ; - get sign of op1 to top
          xor   ESI,EAX         ; - get xor of signs
          and   ESI,ECX         ; - just keep the sign bit
          sub   ECX,ECX         ; - return +/- 0
        _endguess               ; endguess
        mov     EDX,ECX         ; return op2
        mov     EAX,EBX         ; ...
        shr     ESI,16          ; get exponent of op2
        ret                     ; return
        endproc __FLDD



        defp    ___LDD
        add     SI,1            ; add 1 to exponent
        jc      divnan1         ; quit if NaN
        jo      divnan1         ; ...
        add     ESI,0FFFFh      ; readjust low exponent and inc high word
        jc      short divnan2   ; quit if NaN
        jo      short divnan2   ; ...
        sub     ESI,10000h      ; readjust high exponent
        _guess                          ; guess: divide by 0
          or    ECX,ECX                 ; - quit if op2 is not 0
          _quif ne                      ; - ...
          or    EBX,EBX                 ; - quit if op2 is not 0
          _quif ne                      ; - ...
          test  ESI,7FFF0000h           ; - quit if op2's exponent is not 0
          _quif ne                      ; - ...
          _guess                        ; - guess: 0/0 (invalid operation)
            or    EAX,EAX               ; - - quit if op1 is not 0
            _quif ne                    ; - - ...
            or    EDX,EDX               ; - - quit if op1 is not 0
            _quif ne                    ; - - ...
            mov   EAX,ESI               ; - - get exponent
            _shl  AX,1                  ; - - place sign in carry
            _quif ne                    ; - - quit if op1 not 0
            call  F8InvalidOp           ; - - indicate invalid operation
            mov   EDX,0C0000000h        ; - - return NaN
            sub   EAX,EAX               ; - - ...
            mov   SI,0FFFFh             ; - - ...
          _admit                        ; - admit: zero divide
            call  F8DivZero             ; - - invoke zero divide exception
            mov   EDX,80000000h         ; - - return signed infinity
            sub   EAX,EAX               ; - - ...
            or    SI,7FFFh              ; - - ...
          _endguess                     ; - endguess
          ret                           ; - return
        _endguess                       ; endguess
        _guess                          ; guess: op1 is 0
          or    EAX,EAX                 ; - quit if op1 is not 0
          _quif ne                      ; - ...
          or    EDX,EDX                 ; - quit if op1 is not 0
          _quif ne                      ; - ...
          _shl  SI,1                    ; - place sign in carry
          _if   e                       ; - if operand one is 0
            ret                         ; - - return 0
          _endif                        ; - endif
          rcr   SI,1                    ; - put back the sign
        _endguess                       ; endguess

        push    EBP                     ; save EBP
        mov     EBP,ESP                 ; get access to stack
        push    EDI                     ; save EDI

        xchg    ECX,ESI         ; get exponents and signs into ECX
        mov     EDI,ECX         ; get exponent and sign of op1 into EDI
        rol     EDI,16          ; shift to top
        sar     EDI,16          ; shift exponent to bottom, duplicating sign
        sar     ECX,16          ; shift exponent to bottom, duplicating sign
        and     EDI,80007FFFh           ; isolate signs and exponent
        and     ECX,80007FFFh           ; ...
        rol     EDI,16                  ; rotate signs to bottom
        rol     ECX,16                  ; ...
        add     DI,CX                   ; calc sign of result
        rol     EDI,16                  ; rotate signs to top
        rol     ECX,16                  ; ...

        or      DI,DI                   ; if op1 is a denormal
        _if     e                       ; then
          _loop                         ; - loop (normalize it)
            _shl  EAX,1                 ; - - shift fraction left
            _rcl  EDX,1                 ; - - . . .
            dec   DI                    ; - - decrement exponent
            or    EDX,EDX               ; - - check for implied 1 bit
          _until  s                     ; - until implied 1 bit is on
        _endif                          ; endif
        or      CX,CX                   ; if op2 is a denormal
        _if     e                       ; then
          _loop                         ; - loop (normalize it)
            _shl  EBX,1                 ; - - shift fraction left
            _rcl  ESI,1                 ; - - . . .
            dec   CX                    ; - - decrement exponent
            or    ESI,ESI               ; - - check for implied 1 bit
          _until  s                     ; - until implied 1 bit is on
        _endif                          ; endif

        sub     DI,CX                   ; calculate exponent of result
        add     DI,3FFFh                ; add in removed bias
        _guess                          ; guess: overflow
          _quif s                       ; - quit if exponent is negative
          cmp   DI,7FFFh                ; - quit if not overflow
          _quif b                       ; - . . .
          mov   EDI,ECX                 ; - get sign
          mov   DI,7FFFh                ; - set infinity
          mov   EDX,80000000h           ; - ...
          sub   EAX,EAX                 ; - ...
          jmp   _divret                 ; - return (infinity)
        _endguess                       ; endguess
        cmp     DI,-64                  ; if exponent is too small
        _if     l                       ; then underflow
          sub   EAX,EAX                 ; - set result to 0
          sub   EDX,EDX                 ; - ...
          sub   EDI,EDI                 ; - ...
          jmp   _divret                 ; - return (underflow)
        _endif                          ; endif

        push    EDI                     ; save sign and exponent
        push    ESI                     ; save divisor
        push    EBX                     ; ...
        mov     ECX,ESI                 ; get top part of divisor
        mov     EDI,EDX                 ; get dividend
        mov     ESI,EAX                 ; ...
        sub     EAX,EAX                 ; assume top bit is 0
        cmp     ECX,EDI                 ; check high parts
        _if     be                      ; if hi(divisor) <= hi(dividend)
          sub   EDX,ECX                 ; - subtract divisor from dividend
          inc   EAX                     ; - set quotient to 1
        _endif                          ; endif
        push    EAX                     ; save high word of quotient
        mov     EAX,ESI                 ; get low part of dividend
        div     ECX                     ; estimate next word of quotient
        push    EAX                     ; save estimate of quotient (quot+4[EBP])
        xchg    EAX,EBX                 ; save quot., get low word of divisor
        mul     EBX                     ; calc. estimate * lo(divisor)
        xchg    EAX,ECX                 ; ...
        xchg    EBX,EDX                 ; save partial product
        mul     EDX                     ; calc. estimate * hi(divisor)
        add     EAX,EBX                 ; add to partial product
        adc     EDX,0                   ; ...
        mov     EBX,den+lo[EBP]         ; get low word of divisor
        test    byte ptr quot+8[EBP],1  ; if high word of quot is 1
        _if     ne                      ; then
          add   EAX,EBX                 ; - add in divisor
          adc   EDX,den+hi[EBP]         ; - ...
        _endif                          ; endif
; subtract estimate * divisor from dividend
        neg     ECX                     ; 0 - ECX
        sbb     ESI,EAX                 ; ...
        sbb     EDI,EDX                 ; ...
        _if     ne                      ; if high word not 0 (quotient too big)
          _loop                         ; - loop (find correct quotient)
            sub   dword ptr quot+4[EBP],1; - - decrement quotient
            sbb   dword ptr quot+8[EBP],0; - - ...
            add   ECX,EBX               ; - - add divisor back to dividend
            adc   ESI,den+hi[EBP]       ; - - ...
            adc   EDI,0                 ; - - ...
          _until  e                     ; - until done
        _endif                          ; endif
        mov     EDI,ESI                 ; get new dividend
        mov     ESI,ECX                 ; ...
        mov     ECX,den+hi[EBP]         ; get divisor
        cmp     ECX,EDI                 ; check high parts      13-aug-90
        _if     be                      ; if hi(divisor) <= hi(dividend)
          sub   EDI,ECX                 ; - subtract divisor from dividend
          add   dword ptr quot+4[EBP],1 ; - increment quotient
          adc   dword ptr quot+8[EBP],0 ; - ...
        _endif                          ; endif
        mov     EDX,EDI                 ; get dividend into EDX:EAX
        mov     EAX,ESI                 ; ...
        div     ECX                     ; estimate next word of quotient
        push    EAX                     ; save estimate of quotient (-4[EBP])
        or      EAX,EAX                 ; if quotient non-zero
        _if     ne                      ; then
          xchg  EAX,EBX                 ; - save quot., get low word of divisor
          mul   EBX                     ; - calc. estimate * lo(divisor)
          xchg  EAX,ECX                 ; - ...
          xchg  EBX,EDX                 ; - save partial product
          mul   EDX                     ; - calc. estimate * hi(divisor)
          add   EAX,EBX                 ; - add to partial product
          adc   EDX,0                   ; - ...
;   - subtract estimate * divisor from dividend
          neg   ECX                     ; - 0 - ECX
          sbb   ESI,EAX                 ; - ...
          sbb   EDI,EDX                 ; - ...
          _if   ne                      ; - if high word not 0 (quotient too big)
            _loop                       ; - - loop (find correct quotient)
              sub   dword ptr quot+0[EBP],1; - - - decrement quotient
              sbb   dword ptr quot+4[EBP],0; - - - ...
              sbb   dword ptr quot+8[EBP],0; - - - ...
              add   ECX,den+lo[EBP]     ; - - - add divisor back to dividend
              adc   ESI,den+hi[EBP]     ; - - - ...
              adc   EDI,0               ; - - - ...
            _until  e                   ; - - until done
          _endif                        ; - endif
        _endif                          ; endif
        pop     EAX                     ; get quotient
        pop     EDX                     ; ...
        pop     EBX                     ; get high bit
        add     ESP,8                   ; remove divisor
        pop     EDI                     ; restore sign and exponent
        dec     DI                      ; decrement exponent
        shr     EBX,1                   ; if non-zero
        _if     c                       ; then
          rcr   EDX,1                   ; - shift answer right
          rcr   EAX,1                   ; - ...
          inc   EDI                     ; - increment exponent
        _endif                          ; endif

;;      sub     ESI,ESI                 ; get zero for zero fill
;;
;;      _shl    ESI,1                   ; get guard bit
;;      adc     EAX,0                   ; round up
;;      adc     EDX,0                   ; ...
;;      adc     EDI,0                   ; increment exponent if required

        or      DI,DI                   ; if exponent <= 0
        _if   le                        ; then (denormal result)
          _if   e                       ; - if exponent = 0
            mov   CL,1                  ; - - set shift count to 1
          _else                         ; - else
            neg   DI                    ; - - negate to get shift count
            mov   CX,DI                 ; - - ...
          _endif                        ; - endif
          sub     EBX,EBX               ; - for zero fill
          shrd    EAX,EDX,CL            ; - align the fraction
          shrd    EDX,EBX,CL            ; - ...
          sub     DI,DI                 ; - set exponent to 0
        _endif                          ; endif
_divret:
        _shl    EDI,1                   ; get sign
        rcr     DI,1                    ; merge it with exponent
        mov     ESI,EDI                 ; get exponent and sign into SI
        pop     EDI                     ; ...
        pop     EBP                     ; ...
        ret                             ; return

        endproc ___LDD

