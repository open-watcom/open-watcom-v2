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
;* Description:  long double math library - multiply  __FLDM, ___LDM
;*
;*****************************************************************************


ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart    fldm386, dword

endif

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<>     long double math library
;<>
;<>     inputs: EAX - pointer to long double (op1)
;<>             EDX - pointer to long double (op2)
;<>             EBX - pointer to long double (result)
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        xdefp   __FLDM           ; 10-byte real multiply
        xdefp   ___LDM           ; 10-byte real multiply

;=====================================================================

        defp    __FLDM
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
        call    ___LDM          ; do the multiply
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
mulnan1:_guess                  ; guess: op1 is +/- inf and op2 is 0
          or    EAX,EAX         ; - quit if op1 is NaN
          _quif ne              ; - ...
          cmp   EDX,80000000h   ; - ...
          _quif ne              ; - ...
          or    EBX,EBX         ; - quit if op2 not 0
          _quif ne              ; - ...
          or    ECX,ECX         ; - ...
          _quif ne              ; - ...
          mov   ESI,0FFFFFFFFh  ; - answer is -NaN
          sar   EDX,1           ; - ...
          ret                   ; - return
        _endguess               ; endguess
        _guess                  ; guess:
          dec   SI              ; - adjust low exponent
          add   ESI,10000h      ; - increment high exponent
          _quif c               ; - quit if op2 is NaN
          _quif o               ; - ...
          ; op2 is not a NaN or infinity
          ; if op1 is +/- infinity, then flip its sign if op2 is negative
          or    EAX,EAX         ; - check low word
          _if   e               ; - if 0
            cmp   EDX,80000000h ; - - check high word
          _endif                ; - endif
          _if   e               ; - if op1 is +/- infinity
            or  ESI,ESI         ; - - if op2 is negative
            _if s               ; - - then
              xor  SI,8000h     ; - - - flip sign
            _endif              ; - - endif
          _endif                ; - endif
          ret                   ; - return op1
        _endguess               ; endguess
        sub     ESI,10000h      ; readjust high exponent
;
;       both operands are NaNs and/or infinity
;
        _guess                  ; guess: fabs(op1) < fabs(op2)
          cmp   EDX,ECX         ; - compare high words
          _if   e               ; - if high words equal
            cmp   EAX,EBX       ; - - compare low words
          _endif                ; - endif
          _quif a               ; - quit if op1 > op2
          _if   e               ; - if op1 == op2
            ; if op1 is +/- infinity, then flip its sign if op2 is negative
            or  EAX,EAX         ; - - check low word
            _if  e              ; - - if 0
              cmp  EDX,80000000h; - - - check high word
            _endif              ; - - endif
            _if   ne            ; - - if op1 is not +/- infinity
              ret               ; - - - return op1
            _endif              ; - - endif
            or   SI,SI          ; - - if op1 is negative
            _if  s              ; - - then
              xor  ESI,EDX      ; - - - flip sign
            _endif              ; - - endif
          _endif                ; - endif
          mov   EDX,ECX         ; - load op2
          mov   EAX,EBX         ; - ...
          shr   ESI,16          ; - ...
        _endguess               ; endguess
        ret                     ; return
;
;       op1 is not a NaN or infinity, but op2 is
;
mulnan2:sub     ESI,10000h      ; readjust high exponent
        _guess                  ; guess: op2 is infinity
          or    EBX,EBX         ; - quit if op2 not infinity
          _quif ne              ; - ...
          cmp   ECX,80000000h   ; - ...
          _quif ne              ; - ...
          or    EAX,EAX         ; - if op1 is 0
          _if   e               ; - ...
            or  EDX,EDX         ; - - check high word
          _endif                ; - endif
          _if   e               ; - if op1 is 0
            sar   ECX,1         ; - - return -NaN
            mov   ESI,0FFFFFFFFh; - - ...
          _else                 ; - else
            or    SI,SI         ; - - if op1 is negative
            _if   s             ; - - then
              xor   ESI,ECX     ; - - - flip sign bit
            _endif              ; - - endif
          _endif                ; - endif
        _endguess               ; endguess
        mov     EDX,ECX         ; return op2
        mov     EAX,EBX         ; ...
        shr     ESI,16          ; shift exponent to bottom
        ret                     ; return
        endproc __FLDM



        defp    ___LDM
        add     SI,1            ; add 1 to exponent
        jc      mulnan1         ; quit if NaN
        jo      mulnan1         ; ...
        add     ESI,0FFFFh      ; readjust low exponent and inc high word
        jc      short mulnan2   ; quit if NaN
        jo      short mulnan2   ; ...
        sub     ESI,10000h      ; readjust high exponent
        _guess                  ; guess: op1 is 0
          or    EAX,EAX         ; - quit if op1 is not 0
          _quif ne              ; - ...
          or    EDX,EDX         ; - quit if op1 is not 0
          _quif ne              ; - ...
          _shl  SI,1            ; - place sign in carry
          _if   e               ; - if operand one is 0
            ret                 ; - - return 0
          _endif                ; - endif
          rcr   SI,1            ; - restore sign of op1
        _endguess               ; endguess
        _guess                  ; guess: op2 is 0
          or    ECX,ECX         ; - quit if op2 is not 0
          _quif ne              ; - ...
          or    EBX,EBX         ; - quit if op2 is not 0
          _quif ne              ; - ...
          test  ESI,7FFF0000h   ; - quit if op2's exponent is not 0
          _quif ne              ; - ...
          sub   EAX,EAX         ; - ...
          sub   EDX,EDX         ; - ...
          sub   ESI,ESI         ; - ...
          ret                   ; - return 0
        _endguess               ; endguess

        push    EBP             ; save EBP
        push    EDI             ; save EDI
        xchg    ECX,ESI         ; get exponents and signs into ECX
        mov     EDI,ECX         ; get exponent and sign of op1 into EDI
        rol     EDI,16          ; shift to top
        sar     EDI,16          ; duplicate sign
        sar     ECX,16          ; ...
        and     EDI,080007FFFh  ; isolate signs and exponent
        and     ECX,080007FFFh  ; ...
        _guess                  ; guess: overflow
          add   ECX,EDI         ; - determine exponent of result and sign
          sub   CX,3FFEh        ; - remove extra bias
          _quif c               ; - quit if exponent is negative
          cmp   CX,7FFFh        ; - quit if not overflow
          _quif b               ; - . . .
          jmp   mul_oflow       ; - handle overflow
        _endguess               ; endguess
        cmp     CX,-64          ; if exponent is too small
        _if     l               ; then underflow
          sub   EAX,EAX         ; - set result to 0
          sub   EDX,EDX         ; - ...
          sub   ECX,ECX         ; - ...
          jmp   _mulret         ; - return (underflow)
        _endif                  ; endif

        push    ECX             ; save sign and exponent

        sub     EBP,EBP         ; zero EBP
        push    ESI             ; save high part of op2
        push    EDX             ; save high part of op1
        push    EAX             ; save low part of op1
        mul     EBX             ; low part of op1 * low part of op2
        xchg    EAX,ESI         ; ESI becomes start of the sticky bits
        mov     ECX,EDX         ; save high part of result
        pop     EDX             ; restore low part of op1
        mul     EDX             ; low part of op1 * high part of op2
        mov     EDI,EDX         ; save high part of product
        add     ECX,EAX         ; add partial product
        adc     EDI,EBP         ; ...
        adc     EBP,EBP         ; ...
        pop     EAX             ; restore high part of op1
        xchg    EAX,EBX         ; flip with low part of op2
        mul     EBX             ; high part of op1 * low part of op2
        add     ECX,EAX         ; add partial product
        adc     EDI,EDX         ; ...
        adc     EBP,0           ; ...
        mov     EAX,EBX         ; get high part of op1
        pop     EDX             ; restore high part of op2
        mul     EDX             ; high part of op1 * high part of op2
        add     EAX,EDI         ; add partial product
        adc     EDX,EBP         ; ...
        mov     EBX,ECX         ; get guard bits
        pop     ECX             ; restore sign and exponent
        _if     ns              ; if not normalized
          _shl  EBX,1           ; - shift left 1
          _rcl  EAX,1           ; - ...
          _rcl  EDX,1           ; - ...
          dec   CX              ; - decrement exponent
        _endif                  ; endif

        _shl    EBX,1           ; get guard bit
        _if     c               ; if set
          _if   e               ; - if rest of sticky bits are 0
            or    ESI,ESI       ; - - check the bottom sticky bits
            setne BL            ; - - ...
            shr   EBX,1         ; - - if all sticky bits are zero
            _if   nc            ; - - then
              mov   ESI,EAX     ; - - - get bottom bit of result
              shr   ESI,1       ; - - - as rounding bit
            _endif              ; - - endif
          _endif                ; - endif
          adc   EAX,0           ; - round up
          adc   EDX,0           ; - ...
          _if   c               ; - if fraction overflowed
            rcr   EDX,1         ; - - shift right
            rcr   EAX,1         ; - - ...
            inc   CX            ; - - increment exponent
            cmp   CX,7FFFh      ; - - quit if overflow
            je    mul_oflow     ; - - . . .
          _endif                ; - endif
        _endif                  ; endif
        or      CX,CX           ; if exponent <= 0
        _if     le              ; then (denormal result)
          _if   e               ; - if exponent = 0
            mov   CL,1          ; - - set shift count to 1
          _else                 ; - else
            neg   CX            ; - - negate to get shift count
          _endif                ; - endif
          sub   EBX,EBX         ; - for zero fill
          shrd  EAX,EDX,CL      ; - align the fraction
          shrd  EDX,EBX,CL      ; - ...
          sub   CX,CX           ; - set exponent to 0
        _endif                  ; endif
_mulret:
        _shl    ECX,1           ; get sign
        rcr     CX,1            ; shift into exponent
        mov     ESI,ECX         ; get exponent and sign of result into SI
        pop     EDI             ; restore EDI
        pop     EBP             ; restore EBP
        ret                     ; return

mul_oflow:                      ; overflow
        mov     CX,7FFFh        ; set exponent for infinity
        mov     EDX,80000000h   ; set fraction
        sub     EAX,EAX         ; ...
        jmp     short _mulret   ; return
        endproc ___LDM

ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
