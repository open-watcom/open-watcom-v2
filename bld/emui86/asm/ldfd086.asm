ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart    ldfd086, word
endif

        xref    FPOverFlow

;
;       convert long double to double
;
ifdef _BUILDING_MATHLIB
; input:
;       SS:AX - pointer to long double
;       SS:DX - pointer to double
;
        xdefp   __iLDFD
__iLDFD proc
        push    BX              ; save registers
        push    CX              ; ...
        push    DI              ; ...
        push    BP              ; ...
        push    DX              ; save return pointer
        mov     BP,AX
        mov     DI,8[BP]        ; get exponent and sign
        mov     AX,6[BP]        ; get fraction
        mov     CX,2[BP]        ; get fraction
        mov     DX, [BP]        ; get fraction
        mov     BX,4[BP]        ; get fraction
else
; input:
;       DS:BX - pointer to long double
; output:
;       AX:BX:CX:DX - double
;
        xdefp   __EmuLDFD
__EmuLDFD  proc    near
        push    DI              ; save di
        mov     DI,8[BX]        ; get exponent and sign
        mov     AX,6[BX]        ; get fraction
        mov     CX,2[BX]        ; get fraction
        mov     DX, [BX]        ; get fraction
        mov     BX,4[BX]        ; get fraction
endif
        _guess return
          test      DH,04h      ; if have to round
          _if ne                ; - then
            add     DH,08h      ; - round up
            adc     CX,0        ; - ...
            adc     BX,0        ; - ...
            adc     AX,0        ; - ...
            _if c               ; - if exponent needs adjusting
              mov   AH,80h      ; - - set fraction
              inc   DI          ; - - increment exponent
              ; check for overflow
            _endif              ; - endif
            test    DX,03FFH    ; - if half way between
            _if e               ; - then
              and   DH,0F0h     ; - - mask off bottom bit
            _endif              ; - endif
          _endif                ; endif
          mov       DL,DH       ; shift fraction right by 8
          mov       DH,CL       ; ...
          mov       CL,CH       ; ...
          mov       CH,BL       ; ...
          mov       BL,BH       ; ...
          mov       BH,AL       ; ...
          mov       AL,AH       ; ...
          mov       AH,0        ; ...
          _shl      DI,1        ; get sign
          rcr       AX,1        ; shift number right one more is 9
          rcr       BX,1        ; ...
          rcr       CX,1        ; ...
          rcr       DX,1        ; ...
          shr       AL,1        ; and one more is 10
          rcr       BX,1        ; ...
          rcr       CX,1        ; ...
          rcr       DX,1        ; ...
          shr       AL,1        ; and one more is 11
          rcr       BX,1        ; ...
          rcr       CX,1        ; ...
          rcr       DX,1        ; ...
          add       DI,(03FFh-3FFFh) shl 1 ; change bias to double format
          cmp       DI,07FFh shl 1 ; if exponent is not too large or negative
          _if b                 ; then
            _shl    DI,1        ; - get exponent into position
            _shl    DI,1        ; - ...
            _shl    DI,1        ; - ...
            _if e               ; - if denormal number          14-jul-90
              shr   AL,1        ; - - shift right one more time
              rcr   BX,1        ; - - ...
              rcr   CX,1        ; - - ...
              rcr   DX,1        ; - - ...
            _endif              ; - endif
            and     AL,0FH      ; - get rid of implied one bit
            or      AX,DI       ; - merge in exponent
            _quit return
          _endif                ; endif
          cmp       DI,8800h    ; if exponent >= 8800h (underflow)
          _if ae                ; then
            sar     DI,1        ; - align exponent
            cmp     DI,-52      ; - if in the denormal range  02-jul-90
            _if ge              ; - then
              dec   DI          ; - - adjust exponent for 1 more shift
              _loop             ; - - loop (denormalize number)
                cmp DI,-8       ; - - - quit if less than 8
                _quif g         ; - - - ...
                mov DL,DH       ; - - - shift fraction right by 8
                mov DH,CL       ; - - - ...
                mov CL,CH       ; - - - ...
                mov CH,BL       ; - - - ...
                mov BL,BH       ; - - - ...
                mov BH,AL       ; - - - ...
                mov AL,0        ; - - - ...
                add DI,8        ; - - - adjust exponent
              _endloop          ; - - endloop
              _loop             ; - - loop (denormalize number)
                or  DI,DI       ; - - - quit when exponent is 0
                _quif e, return ; - - - ...
                shr AL,1        ; - - - shift fraction right 1 bit
                rcr BX,1        ; - - - ...
                rcr CX,1        ; - - - ...
                rcr DX,1        ; - - - ...
                inc DI          ; - - - increment exponent
              _endloop          ; - - endloop
            _endif              ; - endif
            sub     AX,AX       ; - set result to zero
            sub     BX,BX       ; - ...
            sub     CX,CX       ; - ...
            sub     DX,DX       ; - ...
            _quit return
          _endif                ; endif
          or        AX,7FF0h    ; set infinity (or NaN)
          cmp       DI,87FEh    ; if not infinity or NaN
          _if ne                ; then (overflow)
            call    FPOverFlow  ; - set OVERFLOW exception
          _endif                ; endif
        _endguess
ifdef _BUILDING_MATHLIB
        pop     BP              ; fetch return pointer
        mov     6[BP],AX        ; store return value
        mov     4[BP],BX        ; ...
        mov     2[BP],CX        ; ...
        mov     [BP],DX         ; ...
        pop     BP              ; restore registers
        pop     DI              ; ...
        pop     CX              ; ...
        pop     BX              ; ...
        ret                     ; return
__iLDFD endp

        endmod

        endf    equ end

else
        pop     DI              ; restore di
        ret                     ; return
__EmuLDFD  endp

        endf    equ <>

endif

endf
