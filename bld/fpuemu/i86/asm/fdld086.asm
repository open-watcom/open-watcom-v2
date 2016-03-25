ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc
include shiftmac.inc

        modstart    fdld086, word

endif

        xrefp   FPInvalidOp
        xrefp   __qw_normalize

ifdef _BUILDING_MATHLIB
        xdefp   __iFDLD
else
        xdefp   __EmuFDLD
endif

;
;       convert double to long double
;
;ifdef _BUILDING_MATHLIB
;       input:  SS:AX - pointer to double
;               SS:DX - pointer to long double to be filled in
;else
;       input:  AX:BX:CX:DX - double
;               DS:SI       - pointer to long double to be filled in
;endif

ifdef _BUILDING_MATHLIB
        defp    __iFDLD

        push    BX              ; save BX
        push    CX              ; save CX
        push    DI              ; save DI
        push    BP              ; save BP
        push    DX              ; store return pointer
        mov     BP,AX           ; address of parm
        mov     DX,0[BP]        ; load number
        mov     CX,2[BP]        ; ...
        mov     BX,4[BP]        ; ...
        mov     AX,6[BP]        ; ...
else
        defp    __EmuFDLD

        push    DI              ; save DI
endif
        ; 10-byte temporary result is placed into DI:AX:BX:CX:DX
        _guess xx1              ; guess
          mov   DI,DX           ; - check lower words for zero
          or    DI,CX           ; - ...
          or    DI,BX           ; - ...
          mov   DI,AX           ; - get entry exponent to DI
          _quif ne              ; - quit if lower words are not zero
          and   AX,7FFFh        ; - check for +/-0.0
          _quif e,xx1           ; - quif if 0.0
          cmp   AX,7FF0h        ; - check for +/-infinity
          _quif ne              ; - quit if not infinity
          or    DI,7FFFh        ; - set result infinity
          mov   AX,8000h        ; - set result implied bit
        _admit                  ; admit
          mov     AX,DI
          and     DI,7FF0h      ; - adjust entry exponent to result exponent position
          shr     DI,1          ; - exponent >> 3
          shr     DI,1          ; - ...
          shr     DI,1          ; - ... (still needs >> 1 )
          _rcl    AH,1          ; - get sign to carry flag
          pushf                 ; - save sign (Carry flag)
          mov     AH,3 shl 6    ; - set AH for shift count 3
          _loop                 ; - loop shift fraction
            qw_lshift_1         ; - - shift fraction << 1
          _until nc             ; - loop until count is not zero
          qw_lshift_8           ; - and 8 more makes 11
          cmp     DI,7FFh SHL 1 ; - check entry exponent is NaN
          _if e                 ; - if entry exponent is NaN
            mov   DI,7FFFh SHL 1; - - adjust result exponent to 7FFFh (NaN)
            test  AH,40h        ; - - check SNaN
            _if e               ; - - then
              call  FPInvalidOp ; - - - indicate "Invalid Operand" exception
            _endif              ; - - endif
            or    AH,0C0h       ; - - indicate QNaN and turn on implied bit
          _else                 ; - else it is non-zero number
            add   DI,(3FFFh - 3FFh) SHL 1; - - adjust result exponent
            cmp   DI,(3FFFh - 3FFh) SHL 1; - - check entry exponent is 0 (denormal number)
            _if e               ; - - if denormal number then normalize it
              shr   DI,1        ; - - - shift exponent for normalization
              inc   DI          ; - - - adjust result exponent
              call __qw_normalize; - - - normalize 64-bit fraction
              _shl  DI,1        ; - - - shift exponent back
            _endif              ; - - endif
            or    AH,80h        ; - - turn on implied bit
          _endif                ; - endif
          popf                  ; - get sign (Carry flag)
          rcr     DI,1          ; - get sign back to DI
        _endguess               ; endguess
ifdef _BUILDING_MATHLIB
        pop     BP              ; fetch return pointer
        mov     0[BP],DX        ; store number
        mov     2[BP],CX        ; ...
        mov     4[BP],BX        ; ...
        mov     6[BP],AX        ; ...
        mov     8[BP],DI        ; ...
        pop     BP              ; restore BP
        pop     DI              ; restore DI
        pop     CX              ; restore CX
        pop     BX              ; restore BX
        ret                     ; return

        endproc __iFDLD

        endmod

        endf    equ end

else
        mov     0[SI],DX        ; store number
        mov     2[SI],CX        ; ...
        mov     4[SI],BX        ; ...
        mov     6[SI],AX        ; ...
        mov     8[SI],DI        ; ...
        pop     DI              ; restore DI
        ret                     ; return

        endproc __EmuFDLD

        endf    equ <>

endif

endf
