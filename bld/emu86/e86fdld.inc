
ifdef _BUILDING_MATHLIB
        xdefp   __iFDLD
else
        xdefp   __EmuFDLD
endif

;       convert double to long double
; input:
;ifdef _BUILDING_MATHLIB
;       SS:AX           pointer to double
;       SS:DX           pointer to long double to be filled in
;else
;       AX:BX:CX:DX     double
;       DS:SI           pointer to long double to be filled in
;endif

ifdef _BUILDING_MATHLIB
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
__EmuFDLD  proc    near
        push    DI              ; save DI
endif
        mov     DI,AX           ; get exponent and sign
        shr     DI,1            ; get exponent
        shr     DI,1            ; get exponent
        shr     DI,1            ; get exponent
        and     DI,0FFEh        ; get exponent (still needs >> by 1 )
        _shl    DX,1            ; shift fraction left by one
        _rcl    CX,1            ; ...
        _rcl    BX,1            ; ...
        _rcl    AX,1            ; ...
        rcr     DI,1            ; get sign & put exponent into posn
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
          _shl  DI,1            ; - remove sign bit
          _quif e               ; - quit if a denormal number
          rcr   DI,1            ; - restore sign bit
          cmp   DI,07FFh        ; - if infinity or Nan
          _if   ne              ; - or
            cmp   DI,87FFh      ; - - check for -infinity or -Nan
          _endif                ; - endif
          _if   e               ; - if infinity or Nan
            or    DI,7FFFh      ; - - set infinity or Nan for temp real
            cmp   AX,8000h      ; - - if high word is 8000h (11-jul-92)
            _if   e             ; - - then
              mov   AX,BX       ; - - - check low order words
              or    AX,CX       ; - - - ...
              or    AX,DX       ; - - - ...
              mov   AX,0        ; - - - set AX back to 0
            _endif              ; - - endif
            _if   ne            ; - - if fraction not 0, then its a NaN
              or    AH,40h      ; - - - indicate NaN
              push  AX          ; - - - save AX
              push  DX          ; - - - save DX
              push  BX          ; - - - save BX
              push  CX          ; - - - save CX
              call  F8InvalidOp ; - - - indicate "Invalid" exception
              pop   CX          ; - - - restore CX
              pop   BX          ; - - - restore BX
              pop   DX          ; - - - restore DX
              pop   AX          ; - - - restore AX
            _endif              ; - - endif
          _else                 ; - else
            add   DI,3FFFh-03FFh; - - change bias to temp real format
          _endif                ; - endif
          or    AH,80h          ; - turn on implied one bit
        _admit                  ; admit: denormal number
          rcr   DI,1            ; - restore sign bit
          push  DI              ; - save exponent
          mov   DI,AX           ; - determine if number is 0
          or    DI,BX           ; - ...
          or    DI,CX           ; - ...
          or    DI,DX           ; - ...
          pop   DI              ; - restore exponent
          _quif e               ; - quit if number is 0
          or    DI,3C01h        ; - set exponent
          _loop                 ; - loop (normalize number)
            or    AX,AX         ; - - quit if top word is not 0
            _quif ne            ; - - ...
            xchg  AX,BX         ; - - shift number left 16 bits
            xchg  AX,CX         ; - - ...
            xchg  AX,DX         ; - - ...
            sub   DI,16         ; - - adjust exponent
          _endloop              ; - endloop
          _loop                 ; - loop (normalize number)
            or    AX,AX         ; - - quit if top bit is on
            _quif s             ; - - ...
            _shl  DX,1          ; - - shift number left 1 bit
            _rcl  CX,1          ; - - ...
            _rcl  BX,1          ; - - ...
            _rcl  AX,1          ; - - ...
            dec   DI            ; - - decrement exponent
          _endloop              ; - endloop
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
else
        mov     0[SI],DX        ; store number
        mov     2[SI],CX        ; ...
        mov     4[SI],BX        ; ...
        mov     6[SI],AX        ; ...
        mov     8[SI],DI        ; ...
        pop     DI              ; restore DI
        ret                     ; return
__EmuFDLD  endp
endif
