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


;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<>     long double math library
;<>
;<>     inputs: EAX - pointer to long double (op1)
;<>             EDX - pointer to long double (op2)
;<>             EBX - pointer to long double (result)
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        xdefp   __FLDA          ; add real*10 to real*10
        xdefp   __FLDAC         ; add real*10 to real*10 (opnd 2 on stack)
        xdefp   __FLDS          ; subtract real*10 from real*10
        xdefp   ___LDA          ; long double add routine

        defp    __FLDS
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
        call    ___LDS          ; do the subtract
        pop     EBX             ; restore pointer to result
        mov     [EBX],EAX       ; store result
        mov     4[EBX],EDX      ; ...
        mov     8[EBX],SI       ; ...
        pop     ECX             ; restore ECX
        pop     ESI             ; restore ESI
        ret                     ; return
        endproc __FLDS

        defp    __FLDA
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
        call    ___LDA          ; do the add
        pop     EBX             ; restore pointer to result
        mov     [EBX],EAX       ; store result
        mov     4[EBX],EDX      ; ...
        mov     8[EBX],SI       ; ...
        pop     ECX             ; restore ECX
        pop     ESI             ; restore ESI
        ret                     ; return
        endproc __FLDA

        defp    __FLDAC
        push    ESI             ; save ESI
        push    ECX             ; save ECX
        push    EBX             ; save EBX
        mov     SI,16+8[ESP]    ; load op2 from stack
        mov     ECX,16+4[ESP]   ; ...
        mov     EBX,16+0[ESP]   ; ...
        shl     ESI,16          ; shift to the top
        mov     SI,8[EAX]       ; load op1
        mov     EDX,4[EAX]      ; ...
        mov     EAX,[EAX]       ; ...
        call    ___LDA          ; do the add
        pop     EBX             ; restore pointer to result
        mov     [EBX],EAX       ; store result
        mov     4[EBX],EDX      ; ...
        mov     8[EBX],SI       ; ...
        pop     ECX             ; restore ECX
        pop     ESI             ; restore ESI
        ret     12              ; return and clean up stack
        endproc __FLDAC


        defp    ___LDS
        push    EDI             ; save EDI
        mov     EDI,80000000h   ; indicate subtract
        jmp     __add           ; do the add
;
;       op1 is a NaN or infinity
;
addnan1:_guess                  ; guess:
          dec   SI              ; - adjust low exponent
          add   ESI,10000h      ; - increment high word
          _quif c               ; - quit if NaN
          _quif o               ; - ...
          ; op2 is not a NaN or infinity
          pop   EDI             ; - restore EDI
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
          mov   EAX,ESI         ; - get signs and exponents
          shr   EAX,16          ; - shift op2 sign+exponent to bottom
          cmp   SI,AX           ; - compare sign+exponents
          mov   EAX,EBX         ; - restore EAX
          _if   e               ; - if signs are same
            or  EDI,EDI         ; - - if 'subtract'
            jne short addnan3   ; - - then return -NaN
          _else                 ; - else
            or  EDI,EDI         ; - - if 'add'
            _if e               ; - - then
addnan3:      sar  EDX,1        ; - - - answer is -NaN
              mov  SI,0FFFFh    ; - - - ...
            _endif              ; - - endif
          _endif                ; - endif
        _admit                  ; guess: fabs(op1) < fabs(op2)
          cmp   EDX,ECX         ; - compare high words
          _if   e               ; - if high words equal
            cmp   EAX,EBX       ; - - compare low words
;           _if   e             ; - - if low words equal
;             mov   EAX,ESI     ; - - - get signs and exponents
;             shr   EAX,16      ; - - - shift op2 sign+exponent to bottom
;             cmp   SI,AX       ; - - - compare sign+exponents
;             mov   EAX,EBX     ; - - - restore EAX
;           _endif              ; - - endif
          _endif                ; - endif
          _quif a               ; - quit if op1 > op2
          _if   e               ; - if op1 == op2
            or  EDI,EDI         ; - - check for 'add'
          _endif                ; - endif
          _quif e               ; - quit if 'add'
          mov   EDX,ECX         ; - load op2
          mov   EAX,EBX         ; - ...
          shr   ESI,16          ; - ...
        _endguess               ; endguess
        pop     EDI             ; restore EDI
        ret                     ; return
;
;       op1 is not a NaN or infinity, but op2 is
;
addnan2:mov     EDX,ECX         ; return op2
        mov     EAX,EBX         ; ...
        sub     ESI,10000h      ; readjust high exponent
        _guess                  ; guess: op2 is infinity
          or    EAX,EAX         ; - quit if op2 not infinity
          _quif ne              ; - ...
          cmp   EDX,80000000h   ; - ...
          _quif ne              ; - ...
          xor   ESI,EDI         ; - flip sign if 'subtract'
        _endguess               ; endguess
        shr     ESI,16          ; shift exponent to bottom
        pop     EDI             ; restore EDI
        ret                     ; return
        endproc ___LDS

        defp    ___LDA
        push    EDI             ; save EDI
        sub     EDI,EDI         ; indicate add
__add:  add     SI,1            ; add 1 to exponent
        jc      addnan1         ; quit if NaN
        jo      addnan1         ; ...
        add     ESI,0FFFFh      ; readjust low exponent and inc high word
        jc      short addnan2   ; quit if NaN
        jo      short addnan2   ; ...
        sub     ESI,10000h      ; readjust high exponent
        xor     ESI,EDI         ; flip sign if subtract
        pop     EDI             ; restore EDI
        _guess                  ; guess: op1 is 0
          or    EAX,EAX         ; - quit if op1 is not 0
          _quif ne              ; - ...
          or    EDX,EDX         ; - quit if op1 is not 0
          _quif ne              ; - ...
          _shl  SI,1            ; - place sign in carry
          _if   e               ; - if operand one is 0
            shr   ESI,16        ; - - get sign and exponent of op2
ret_op:
            mov   EAX,EBX       ; - - return op2
            mov   EDX,ECX       ; - - ...
            _shl  ESI,1         ; - - get rid of sign bit
            or    BX,SI         ; - - or in exponent
            or    EBX,EDX       ; - - check for 0
            _if   ne            ; - - if not zero
              shr   ESI,1       ; - - - restore sign bit
            _endif              ; - - endif
            ret                 ; - - return
          _endif                ; - endif
          rcr   SI,1            ; - put back the sign
        _endguess               ; endguess
        _guess                  ; guess: op2 is 0
          or    ECX,ECX         ; - quit if op2 is not 0
          _quif ne              ; - ...
          or    EBX,EBX         ; - quit if op2 is not 0
          _quif ne              ; - ...
          test  ESI,7FFF0000h   ; - quit if op2's exponent is not 0
          _quif ne              ; - ...
          ret                   ; - return op1
        _endguess               ; endguess

        push    EBP             ; save EBP
        push    EDI             ; save EDI

        xchg    ECX,ESI         ; get exponents and signs into ECX
        mov     EDI,ECX         ; get exponent and sign of op1 into EDI
        rol     EDI,16          ; shift to top
        sar     EDI,16          ; shift exponent to bottom, duplicating sign
        sar     ECX,16          ; shift exponent to bottom, duplicating sign
        and     EDI,80007FFFh   ; isolate signs and exponent
        and     ECX,80007FFFh   ; ...
        mov     EBP,ECX         ; assume op1 < op2
        rol     EDI,16          ; rotate signs to bottom
        rol     ECX,16          ; ...
        add     CX,DI           ; calc sign of result
        rol     EDI,16          ; rotate signs to top
        rol     ECX,16          ; ...

        sub     CX,DI           ; calculate difference in exponents
        _if     ne              ; if different
          _if   b               ; - if op2 < op1
            mov   EBP,EDI       ; - - get larger exponent for result
            neg   CX            ; - - negate the shift count
            xchg  EAX,EBX       ; - - flip operands
            xchg  EDX,ESI       ; - - . . .
          _endif                ; - endif
          cmp     CX,64         ; - if shift count too big
          _if     a             ; - then, return operand 1
            _shl  EBP,1         ; - - get sign
            rcr   BP,1          ; - - merge with exponent
            mov   EAX,EBX       ; - - get result
            mov   EDX,ESI       ; - - ...
            mov   ESI,EBP       ; - - ...
            pop   EDI           ; - - restore EDI
            pop   EBP           ; - - restore EBP
            ret                 ; - - return
          _endif                ; - endif
        _endif                  ; endif
        mov     CH,0            ; zero extend op2
        or      ECX,ECX         ; get bit 0 of sign word - value is 0 if
                                ; both operands have same sign, 1 if not
        _if     s               ; if signs are different
          mov   CH,0FFh         ; - set high part to ones
          neg   ESI             ; - negate the fraction of op2
          neg   EBX             ; - . . .
          sbb   ESI,0           ; - . . .
          xor   EBP,80000000h   ; - flip sign
        _endif                  ; endif
        sub     EDI,EDI         ; get a zero for sticky bits
        cmp     CL,0            ; if shifting required
        _if     ne              ; then
          push    EBX           ; - save EBX
          sub     EBX,EBX       ; - for zero fill
          cmp     CL,32         ; - if shift count >= 32
          _if     ae            ; - then
            or    EAX,EAX       ; - - check low order word for 1 bits
            setne BL            ; - - BL=1 if EAX non zero
            mov   EDI,EBX       ; - - save sticky bits
            sub   EBX,EBX       ; - - for zero fill
            cmp   CL,64         ; - - if shift count is 64      (19-nov-89)
            _if   e             ; - - then
              or    EDI,EDX     ; - - - get rest of sticky bits from high part
              sub   EDX,EDX     ; - - - zero high part
            _endif              ; - - endif
            mov   EAX,EDX       ; - - shift right 32
            sub   EDX,EDX       ; - - zero high word
;;;         sub   CL,32         ; - - adjust shift count
          _endif                ; - endif
          shrd    EBX,EAX,CL    ; - get the extra sticky bits
          or      EDI,EBX       ; - save them
          sub     EBX,EBX       ; - for zero fill
          shrd    EAX,EDX,CL    ; - align the fractions
          shrd    EDX,EBX,CL    ; - ...
          pop     EBX           ; - restore EBX
        _endif                  ; endif

        add     EAX,EBX         ; add the fractions
        adc     EDX,ESI         ; . . .
        adc     CH,0            ; . . .
        _if     s               ; if answer is negative
          cmp   CL,64           ; - if shift count = 64
          _if   e               ; - then
            test  EDI,7FFFFFFFh ; - - check the sticky bits
            setne BL            ; - - make single sticky bit
            shr   EBX,1         ; - - carry set if sticky=1
            adc   EAX,0         ; - - round up fraction if required
            adc   EDX,0         ; - - . . .
            adc   CH,0          ; - - . . .
          _endif                ; - endif
          neg   EDX             ; - negate the fraction
          neg   EAX             ; - ...
          sbb   EDX,0           ; - ...
          mov   CH,0            ; - zero top bits
          xor   EBP,80000000h   ; - flip the sign
        _endif                  ; endif
        mov     EBX,EAX         ; get result
        or      BL,CH           ; check for zero
        or      EBX,EDX         ; if not zero
        _if     ne              ; then
          or    BP,BP           ; - if exponent is 0
          je    short denormal  ; - denormal when exponent hits 0
          cmp   CH,0            ; - if top bits are 0
          _if   e               ; - then
            rol   EDI,1         ; - - set carry from last sticky bit
            ror   EDI,1         ; - - ...
            _loop               ; - - loop (normalize)
              dec   BP          ; - - - decrement exponent
              je    short denormal; - - denormal when exponent hits 0
              _rcl  EAX,1       ; - - - shift fraction left one bit
              _rcl  EDX,1       ; - - - ...
            _until  c           ; - - until carry
          _endif                ; - endif
          inc   BP              ; - increment exponent
          cmp   BP,7FFFh        ; - quit if overflow
          je    add_oflow       ; - . . .
          stc                   ; - set carry
          rcr   EDX,1           ; - shift fraction right 1
          rcr   EAX,1           ; - ...
          _if     c             ; - if guard bit is on
            _shl  EDI,1         ; - - get top sticky bit
            _if   e             ; - - if no more sticky bits
              ror   EAX,1       ; - - - set carry with bottom bit of DX
              rol   EAX,1       ; - - - ...
            _endif              ; - - endif
            adc   EAX,0         ; - - round up fraction if required
            adc   EDX,0         ; - - . . .
            _if   c             ; - - if we got a carry
              rcr   EDX,1       ; - - - shift fraction right 1
              rcr   EAX,1       ; - - - ...
              inc   BP          ; - - - increment exponent
              cmp   BP,7FFFh    ; - - - quit if overflow
              je    add_oflow   ; - - - . . .
            _endif              ; - - endif
          _endif                ; - endif
        _else                   ; else (answer is 0)
          mov   EBP,EBX         ; - set exponent to 0
        _endif                  ; endif

denormal:                       ; handle denormal
_addret:
        _shl    EBP,1           ; get sign
        rcr     BP,1            ; merge with exponent
        mov     ESI,EBP         ; get exponent and sign into SI
        pop     EDI             ; restore EDI
        pop     EBP             ; restore EBP
        ret                     ; return

add_oflow:                      ; handle overflow
        mov     BP,7FFFh        ; get exponent for infinity
        sub     EAX,EAX         ; set fraction
        mov     EDX,80000000h   ; ...
        jmp     short _addret   ; return
;;      jmp     F8OverFlow      ; handle overflow
        endproc ___LDA

