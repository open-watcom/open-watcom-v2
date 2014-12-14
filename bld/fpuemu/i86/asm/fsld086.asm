
ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        xrefp   FPInvalidOp

        modstart    fsld086, word


        xdefp   __iFSLD
else
        xdefp   __EmuFSLD
endif

;       convert float to long double
; input:
;ifdef _BUILDING_MATHLIB
;       SS:AX   pointer to float
;       SS:DX   pointer to long double to be filled in
;else
;       DX:AX   float
;       DS:BX   pointer to long double to be filled in
;endif

ifdef _BUILDING_MATHLIB
__iFSLD proc
        push    BP              ; save registers
        push    BX              ; ...
        push    DX              ; save return pointer
        mov     BP,AX           ; get address for parm
        mov     AX,[BP]         ; get parm
        mov     DX,2[BP]        ; ...
else
__EmuFSLD proc  near
        push    SI              ; save registers
        mov     SI,BX           ; get address for result
endif
        ; 10-byte temporary result is placed into BX:DX:AX:0:0
        _guess xx1
          mov   BX,DX           ; - get high word to BX
          mov   DX,AX           ; - get low word to DX
          or    AX,AX           ; - check low word for zero
          _quif ne              ; - quit if low word is not zero
          mov   AX,BX           ; - get exponent to AX
          and   AX,7FFFh        ; - check for +/-0.0
          _quif e,xx1           ; - quif if 0.0
          cmp   AX,7F80h        ; - check for +/-infinity
          _quif ne              ; - quit if not infinity
          or    BL,0F0h         ; - set result infinity
          sub   AX,AX           ; - set result mantisa lower part to 0
        _admit                  ; admit
          xor   AL,AL           ; - get fraction to DX:AX
          mov   AH,DL           ; - ...
          mov   DL,DH           ; - ...
          mov   DH,BL           ; - ...
          xchg  BL,BH           ; - adjust entry exponent to result exponent position
          and   BH,80h          ; - ...
          _shl  BH,1            ; - ...
          _rcl  BL,1            ; - ...
          pushf                 ; - save sign (Carry flag)
          cmp   BX,0FFh         ; - check entry exponent is NaN
          _if e                 ; - if entry exponent is NaN
            mov   BX,7FFFh      ; - - adjust result exponent to 7FFFh (NaN)
            test  DH,40h        ; - - check SNaN
            _if e               ; - - then
              call  FPInvalidOp ; - - - indicate "Invalid" exception
            _endif              ; - - endif
            or    DH,0C0h       ; - - indicate QNaN and turn on implied bit
          _else                 ; - else it is non-zero number
            add BX,3FFFh - 7Fh  ; - - adjust result exponent
            cmp BX,3FFFh - 7Fh  ; - - check entry exponent is 0 (denormal number)
            _if e               ; - - if denormal number then normalize it
              inc BX            ; - - - adjust result exponent
              _loop             ; - - - loop shift fraction by 8 quantities
                or  DH,DH       ; - - - - check fraction MSB
                _quif nz        ; - - - - quit if not zero
                mov   DH,DL     ; - - - - shift fraction by 8
                mov   DL,AH     ; - - - - ...
                mov   AH,AL     ; - - - - ...
                xor   AL,AL      ; - - - - ...
                sub   BX,8      ; - - - - adjust result exponent
              _endloop          ; - - - endloop
              _loop             ; - - - loop shift fraction by 1
                _quif s         ; - - - - quit if normalized (implied bit is set)
                dec   BX;       ; - - - - adjust result exponent
                _shl  AX,1      ; - - - - shift fraction << 1
                _rcl  DX,1      ; - - - - ...
              _endloop          ; - - - endloop
            _endif              ; - - endif
            or    DH,80h        ; - - turn on implied bit
          _endif                ; - endif
          _shl  BX,1            ; - prepare BX to set sign
          popf                  ; - get sign (Carry flag)
          rcr   BX,1            ; - get sign back to DI
        _endguess               ; endguess
ifdef _BUILDING_MATHLIB
        pop     BP              ; fetch pointer to return
        mov     8[BP],BX        ; save exponent
        mov     6[BP],DX        ; save high word of fraction
        mov     4[BP],AX        ; save next word
        sub     AX,AX           ; next 2 words are zero
        mov     2[BP],AX        ; ...
        mov     [BP],AX         ; ...
        pop     BX              ; restore registers
        pop     BP              ; ...
        ret                     ; return
__iFSLD endp
else
        mov     8[SI],BX        ; save exponent
        mov     6[SI],DX        ; save high word of fraction
        mov     4[SI],AX        ; save next word
        sub     AX,AX           ; next 2 words are zero
        mov     2[SI],AX        ; ...
        mov     [SI],AX         ; ...
        pop     SI              ; restore registers
        ret                     ; return
__EmuFSLD endp
endif

ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
