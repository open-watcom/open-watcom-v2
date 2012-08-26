
ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        xref    FPInvalidOp

        modstart    fsld086, word


        xdefp   __iFSLD
else
        xdefp   __EmuFSLD
endif

;       convert float to long double
; input:
;ifdef _BUILDING_MATHLIB
;       AX      pointer to float
;       DX      pointer to long double to be filled in
;else
;       DX:AX   float
;       BX      pointer to long double to be filled in
;endif

ifdef _BUILDING_MATHLIB
__iFSLD proc
        push    BX              ; save registers
        push    CX              ; ...
        push    BP              ; ...
        push    DX              ; save return pointer
        mov     BP,AX           ; get address for parm
        mov     AX,[BP]         ; get parm
        mov     DX,2[BP]        ; ...
else
__EmuFSLD proc  near
        push    CX              ; save registers
        push    SI              ; ...
        mov     SI,BX           ; get address for result
endif
        mov     BL,DH           ; get exponent
        mov     BH,DL           ; ...
        mov     DH,DL           ; align fraction
        mov     DL,AH           ; ...
        mov     AH,AL           ; ...
        mov     AL,0            ; ...
        _shl    BH,1            ; align exponent
        _rcl    BL,1            ; ...
        pushf                   ; save sign (Carry flag)
        _guess xx1              ; guess: normal number
          and     BX,0FFh       ; if exponent not zero
          _quif e               ; - quit if denormal number
          _guess xx2            ; - guess: normal number
            cmp   BL,0FFh       ; - - quit if NaN or infinity
            _quif e             ; - - ...
            add   BX,3FFFh-7Fh  ; - - change bias to temp real format
          _admit                ; - guess: NaN or infinity
            or    BH,7Fh        ; - - set exponent to all one's
            test  DX,7FFFh      ; - - if high word is 0
            _if   e             ; - - then
              or    AX,AX       ; - - - ...
              _quif e, xx2      ; - - - if fraction is 0, then its a infinity
            _endif              ; - - endif
            call  FPInvalidOp   ; - - - indicate "Invalid" exception
            or    DH,40h        ; - - - indicate QNaN
          _endguess             ; - endguess
          or    DH,80h          ; - turn on implied one bit
        _admit                  ; admit: denormal number or zero
          or    DX,DX           ; - if high word is 0
          _if   e               ; - then
            or    AX,AX         ; - - check low word
            _quif e, xx1        ; - - quit if number is zero
          _endif                ; - endif
          or    BX,3FFFh-7Fh+1  ; - set exponent
          or    DX,DX           ; - - quit if top word is not 0
          _if   e               ; - - then
            xchg  DX,AX         ; - - shift number left 16 bits
            sub   BX,16         ; - - adjust exponent
          _endif                ; - - endif
          _loop                 ; - loop (normalize number)
            or    DX,DX         ; - - quit if top bit is on
            _quif s             ; - - ...
            _shl  AX,1          ; - - shift number left 1 bit
            _rcl  DX,1          ; - - ...
            dec   BX            ; - - decrement exponent
          _endloop              ; - endloop
        _endguess               ; endguess
        _shl    BX,1            ; get result sign
        popf                    ; - get sign (Carry flag)
        rcr     BX,1            ; - get sign back to BX
ifdef _BUILDING_MATHLIB
        pop     BP              ; fetch pointer to return
        mov     8[BP],BX        ; save exponent
        mov     6[BP],DX        ; save high word of fraction
        mov     4[BP],AX        ; save next word
        sub     AX,AX           ; next 2 words are zero
        mov     2[BP],AX        ; ...
        mov     [BP],AX         ; ...
        pop     BP              ; restore registers
        pop     CX              ; ...
        pop     BX              ; ...
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
        pop     CX              ; ...
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
