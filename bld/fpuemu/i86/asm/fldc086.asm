
ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart    fldc086, word

endif

        xdefp   __FLDC

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;  
;   __FLDC - long double comparison
;  
;ifdef _BUILDING_MATHLIB
;       input:  SS:AX - pointer to operand 1
;               SS:DX - pointer to operand 2
;else
;       input:  DS:AX - pointer to operand 1
;               DS:DX - pointer to operand 2
;endif
;       output: AX - result
;  
;         if op1 > op2,  1 is returned in AX
;         if op1 < op2, -1 is returned in AX
;         if op1 = op2,  0 is returned in AX
;         if either opnd is NaN, then 2 is returned in AX
;  
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        defp    __FLDC

ifdef _BUILDING_MATHLIB
        push    DS              ; save DS
        push    SS              ; fpc code assumes parms are relative to SS
        pop     DS              ; ...
endif
        push    DI              ; save DI
        push    SI              ; save SI
        mov     SI,AX           ; point to op1
        mov     DI,DX           ; point to op2
        _guess                  ; guess: op1 is not a NaN
          mov   AX,8[SI]        ; - get sign and exponent of op1
          or    AH,80H          ; - turn on sign bit (all 1's if a NaN)
          inc   AX              ; - will be 0 if a NaN
          _quif ne              ; - quit if not a NaN
          mov   AX,4[SI]        ; - or in rest of fraction
          or    AX,2[SI]        ; - ...
          or    AX,[SI]         ; - ...
          jne   cmpNaN          ; - non-zero fraction means its a NaN
          mov   AX,6[SI]        ; - get high order word of fraction
          cmp   AX,8000h        ; - check for infinity
          jne   cmpNaN          ; - 8000h => Inf, otherwise NaN
        _endguess               ; endguess
        _guess                  ; guess: op2 is not a NaN
          mov   AX,8[DI]        ; - get sign and exponent of op2
          or    AH,80H          ; - turn on sign bit (all 1's if a NaN)
          inc   AX              ; - will be 0 if a NaN
          _quif ne              ; - quit if not a NaN
          mov   AX,4[DI]        ; - or in rest of fraction
          or    AX,2[DI]        ; - ...
          or    AX,[DI]         ; - ...
          jne   cmpNaN          ; - non-zero fraction means its a NaN
          mov   AX,6[DI]        ; - get high order word of fraction
          cmp   AX,8000h        ; - check for infinity
          jne   cmpNaN          ; - 8000h => Inf, otherwise NaN
        _endguess               ; endguess
        mov     DX,8[SI]        ; get sign and exponent of op1
        mov     AX,8[DI]        ; get sign and exponent of op2
        xor     AX,DX           ; see about signs of the operands
        mov     AX,0            ; clear result
        js      short chkfor0   ; quif arg1 & arg2 have diff signs
        _guess                  ; guess
          cmp   DX,8[DI]        ; - compare exponents
          _quif ne              ; - quif not equal
          mov   AX,6[SI]        ; - get high part of fraction
          cmp   AX,6[DI]        ; - compare them
          _quif ne              ; - quif not equal
          mov   AX,4[SI]        ; - get next part of fraction
          cmp   AX,4[DI]        ; - compare them
          _quif ne              ; - quif not equal
          mov   AX,2[SI]        ; - get next part of fraction
          cmp   AX,2[DI]        ; - compare them
          _quif ne              ; - quif not equal
          mov   AX,[SI]         ; - get low part of fraction
          cmp   AX,[DI]         ; - compare them
        _endguess               ; endguess
        mov     AX,0            ; clear result
        _if     ne              ; if arg1 <> arg2
          rcr   AX,1            ; - save carry in AX
          xor   DX,AX           ; - sign of DX is sign of result

cmpdone:  _shl  DX,1            ; - get sign of result into carry
          sbb   AX,0            ; - AX gets sign of result
          _shl  AX,1            ; - double AX
          inc   AX              ; - make AX -1 or 1
        _endif                  ; endif
        pop     SI              ; restore SI
        pop     DI              ; restore DI
ifdef _BUILDING_MATHLIB
        pop     DS              ; restore DS
endif
        ret                     ; return to caller

cmpNaN: mov     AX,2            ; indicate NaN
        pop     SI              ; restore SI
        pop     DI              ; restore DI
ifdef _BUILDING_MATHLIB
        pop     DS              ; restore DS
endif
        ret                     ; return to caller

chkfor0:or      AX,[SI]         ; see if both operands are 0
        or      AX,[DI]         ; ...
        or      AX,2[SI]        ; ...
        or      AX,2[DI]        ; ...
        or      AX,4[SI]        ; ...
        or      AX,4[DI]        ; ...
        or      AX,6[SI]        ; ...
        or      AX,6[DI]        ; ...
        and     DX,7FFFh        ; get rid of sign from op1
        or      AX,DX           ; check for zero
        mov     DX,8[DI]        ; get exponent of op2
        and     DX,7FFFh        ; get rid of sign from op2
        or      AX,DX           ; check for zero
        mov     DX,8[SI]        ; get sign of op1
        mov     AX,0            ;
        jne     cmpdone         ; if not zero
        pop     SI              ; restore SI
        pop     DI              ; restore DI
ifdef _BUILDING_MATHLIB
        pop     DS              ; restore DS
endif
        ret                     ; return to caller

        endproc __FLDC


ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
