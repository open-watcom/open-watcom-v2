ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart    fdld086, word
endif

        xref    FPInvalidOp

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
        mov     DI,AX           ; get exponent
        shr     DI,1            ; ...
        shr     DI,1            ; ...
        shr     DI,1            ; ... (still needs >> by 1 )
        _shl    DX,1            ; shift fraction left by one
        _rcl    CX,1            ; ...
        _rcl    BX,1            ; ...
        _rcl    AX,1            ; ...
        pushf                   ; save sign (Carry flag)
        _shl    DX,1            ; and 1 more makes 2
        _rcl    CX,1            ; ...
        _rcl    BX,1            ; ...
        _rcl    AX,1            ; ...
        _shl    DX,1            ; and 1 more makes 3
        _rcl    CX,1            ; ...
        _rcl    BX,1            ; ...
        _rcl    AX,1            ; ...
        mov     AH,AL           ; and 8 more makes 11
        mov     AL,BH           ; ...
        mov     BH,BL           ; ...
        mov     BL,CH           ; ...
        mov     CH,CL           ; ...
        mov     CL,DH           ; ...
        mov     DH,DL           ; ...
        mov     DL,0            ; ...
        _guess                  ; guess: a normal number
          and   DI,7FFh SHL 1   ; - isolate exponent (still needs >> by 1 )
          _quif e               ; - quit if a denormal number or zero
          _guess xx2            ; - guess: normal number
            cmp DI,7FFh SHL 1   ; - - quit if infinity or NaN
            _quif e             ; - - ...
            add DI,(3FFFh-3FFh) SHL 1; - - change bias to temp real format
          _admit                ; - guess: NaN or infinity
            or    DI,7FFFh SHL 1; - - set infinity or Nan for temp real
            and   AX,7FFFh      ; - - if high word is 0
            _if e               ; - - then
              or    AX,BX       ; - - - check low order words
              or    AX,CX       ; - - - ...
              or    AX,DX       ; - - - ...
              mov   AX,0        ; - - - set AX back to 0
              _quif e, xx2      ; - - - quit if fraction 0, then its a infinity
            _endif              ; - - endif
            call  FPInvalidOp   ; - - - indicate "Invalid" exception
            or    AH,40h        ; - - - indicate QNaN
          _endguess             ; - endguess
          or    AH,80h          ; - turn on implied one bit
        _admit                  ; admit: denormal number or zero
          push  AX              ; - determine if number is 0
          or    AX,BX           ; - ...
          or    AX,CX           ; - ...
          or    AX,DX           ; - ...
          pop   AX              ; - ...
          _quif e               ; - quit if number is zero
          or    DI,(3FFFh-3FFh+1) SHL 1; - set exponent
          _loop                 ; - loop (normalize number)
            or    AX,AX         ; - - quit if top word is not 0
            _quif ne            ; - - ...
            xchg  AX,DX         ; - - shift number left 16 bits
            xchg  AX,CX         ; - - ...
            xchg  AX,BX         ; - - ...
            sub   DI,16 SHL 1   ; - - adjust exponent
          _endloop              ; - endloop
          _loop                 ; - loop (normalize number)
            or    AX,AX         ; - - quit if top bit is on
            _quif s             ; - - ...
            _shl  DX,1          ; - - shift number left 1 bit
            _rcl  CX,1          ; - - ...
            _rcl  BX,1          ; - - ...
            _rcl  AX,1          ; - - ...
            sub   DI,1 SHL 1    ; - - decrement exponent
          _endloop              ; - endloop
        _endguess               ; endguess
        popf                    ; get sign (Carry flag)
        rcr     DI,1            ; get sign back to DI
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
