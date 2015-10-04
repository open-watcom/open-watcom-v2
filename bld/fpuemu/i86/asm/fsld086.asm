
ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc
include shiftmac.inc

        modstart    fsld086, word

endif

        xrefp   FPInvalidOp
        xrefp   __dw_normalize

ifdef _BUILDING_MATHLIB
        xdefp   __iFSLD
else
        xdefp   __EmuFSLD
endif

;
;       convert float to long double
;
;ifdef _BUILDING_MATHLIB
;       input:  SS:AX   pointer to float
;               SS:DX   pointer to long double to be filled in
;else
;       input:  DX:AX   float
;               DS:BX   pointer to long double to be filled in
;endif

ifdef _BUILDING_MATHLIB

        defp    __iFSLD

        push    BP              ; save registers
        push    CX              ; ...
        push    DX              ; save return pointer
        mov     BP,AX           ; get address for parm
        mov     AX,[BP]         ; get parm
        mov     DX,2[BP]        ; ...
else

        defp    __EmuFSLD

        push    CX              ; save registers
endif
        ; 10-byte temporary result is placed into CX:DX:AX:0:0
        _guess xx1
          mov   CX,DX           ; - get high word to CX
          mov   DX,AX           ; - get low word to DX
          or    AX,AX           ; - check low word for zero
          _quif ne              ; - quit if low word is not zero
          mov   AX,CX           ; - get exponent to AX
          and   AX,7FFFh        ; - check for +/-0.0
          _quif e,xx1           ; - quif if 0.0
          cmp   AX,7F80h        ; - check for +/-infinity
          _quif ne              ; - quit if not infinity
          or    CL,0F0h         ; - set result infinity
          sub   AX,AX           ; - set result mantisa lower part to 0
        _admit                  ; admit
          xor   AL,AL           ; - get fraction to DX:AX
          mov   AH,DL           ; - ...
          mov   DL,DH           ; - ...
          mov   DH,CL           ; - ...
          xchg  CL,CH           ; - adjust entry exponent to result exponent position
          and   CH,80h          ; - ...
          _shl  CH,1            ; - ...
          _rcl  CL,1            ; - ...
          pushf                 ; - save sign (Carry flag)
          cmp   CX,0FFh         ; - check entry exponent is NaN
          _if e                 ; - if entry exponent is NaN
            mov   CX,7FFFh      ; - - adjust result exponent to 7FFFh (NaN)
            test  DH,40h        ; - - check SNaN
            _if e               ; - - then
              call  FPInvalidOp ; - - - indicate "Invalid" exception
            _endif              ; - - endif
            or    DH,0C0h       ; - - indicate QNaN and turn on implied bit
          _else                 ; - else it is non-zero number
            add CX,3FFFh - 7Fh  ; - - adjust result exponent
            cmp CX,3FFFh - 7Fh  ; - - check entry exponent is 0 (denormal number)
            _if e               ; - - if denormal number then normalize it
              inc CX            ; - - - adjust result exponent
              call __dw_normalize; - - - normalize 32-bit fraction
            _endif              ; - - endif
            or    DH,80h        ; - - turn on implied bit
          _endif                ; - endif
          _shl  CX,1            ; - prepare CX to set sign
          popf                  ; - get sign (Carry flag)
          rcr   CX,1            ; - get sign back to DI
        _endguess               ; endguess
ifdef _BUILDING_MATHLIB
        pop     BP              ; fetch pointer to return
        mov     8[BP],CX        ; save exponent
        mov     6[BP],DX        ; save high word of fraction
        mov     4[BP],AX        ; save next word
        sub     AX,AX           ; next 2 words are zero
        mov     2[BP],AX        ; ...
        mov     [BP],AX         ; ...
        pop     CX              ; restore registers
        pop     BP              ; ...
        ret                     ; return

        endproc __iFSLD
else
        mov     8[BX],CX        ; save exponent
        mov     6[BX],DX        ; save high word of fraction
        mov     4[BX],AX        ; save next word
        sub     AX,AX           ; next 2 words are zero
        mov     2[BX],AX        ; ...
        mov     [BX],AX         ; ...
        pop     CX              ; restore registers
        ret                     ; return

        endproc __EmuFSLD
endif

ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
