ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc
include shiftmac.inc

        modstart    fdld086, word
endif

        xrefp   FPInvalidOp

;
;       convert double to long double
;
ifdef _BUILDING_MATHLIB
; input:
;       SS:AX           pointer to double
;       SS:DX           pointer to long double to be filled in
;

        xdefp   __iFDLD
__iFDLD  proc
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
; input:
;       AX:BX:CX:DX     double
;       DS:SI           pointer to long double to be filled in
;
        xdefp   __EmuFDLD
__EmuFDLD  proc    near
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
              add   DI,1 SHL 1  ; - - - adjust result exponent
              _loop             ; - - - loop shift fraction by 16 quantities
                or  AX,AX       ; - - - - check fraction MSB
              _quif nz          ; - - - quit if not zero
                sub DI,16 SHL 1 ; - - - - adjust result exponent
                qw_lshift_16    ; - - - - shift fraction << 16
              _endloop          ; - - - endloop
              _loop             ; - - - loop shift fraction by 8 quantities
                or  AH,AH       ; - - - - check fraction MSB
              _quif nz          ; - - - quit if not zero
                sub DI,8 SHL 1  ; - - - - adjust result exponent
                qw_lshift_8     ; - - - - shift fraction << 8
              _endloop          ; - - - endloop
              _loop             ; - - - loop shift fraction by 1
              _quif s           ; - - - quit if normalized (implied bit is set)
                sub DI,1 SHL 1  ; - - - - adjust result exponent
                qw_lshift_1     ; - - - - shift fraction << 1
              _endloop          ; - - - endloop
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
__iFDLD  endp

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
__EmuFDLD  endp

        endf    equ <>

endif

endf
