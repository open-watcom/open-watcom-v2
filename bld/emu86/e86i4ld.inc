;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<> __I4LD - convert 32-bit integer to long double
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        xdefp   __I4LD
        xdefp   __U4LD

;       __I4LD - convert long into long double
;       __U4LD - convert unsigned long into long double
; input:
;       DX:AX - long
;       DS:BX - pointer to long double
;
ifdef _BUILDING_MATHLIB
__I4LD  proc
else
__I4LD  proc    near
endif
        or      DX,DX           ; if number is negative
        _if     s               ; then
          not   DX              ; - negate the value
          neg   AX              ; - ...
          sbb   DX,-1           ; - ...
          push  CX              ; - save CX
          mov   CX,0C01Eh       ; - set exponent
        _else                   ; else
__U4LD:                         ; - convert unsigned long to long double
          push  CX              ; - save CX
          mov   CX,0401Eh       ; - set exponent
        _endif                  ; endif
        or      DX,DX           ; if high order word is 0
        _if     e               ; then
          sub   CX,16           ; - adjust exponent
          xchg  AX,DX           ; - flip low order word to the top
        _endif                  ; endif
        cmp     DH,0            ; if high order byte is 0
        _if     e               ; then
          mov   DH,DL           ; - shift up 8 bits
          mov   DL,AH           ; - ...
          mov   AH,AL           ; - ...
          mov   AL,0            ; - ...
          sub   CX,8            ; - adjust exponent
        _endif                  ; endif
        or      DH,DH           ; if high order byte is 0
        _if     e               ; then
          sub   CX,CX           ; - whole answer is 0
        _else                   ; else
          _if   ns              ; - if not already normalized
            _loop               ; - - loop (normalize result)
              dec   CX          ; - - - decrement exponent
              _shl  AX,1        ; - - - shift left 1 bit
              _rcl  DX,1        ; - - - ...
            _until  s           ; - - until normalized
          _endif                ; - endif
        _endif                  ; endif
ifdef _BUILDING_MATHLIB
        push    DS              ; save DS
        push    SS              ; fpc code assumes parms are relative to SS
        pop     DS              ; ...
endif
        mov     8[BX],CX        ; store exponent
        mov     6[BX],DX        ; fraction
        mov     4[BX],AX        ; ...
        sub     AX,AX           ; rest is 0
        mov     2[BX],AX        ; ...
        mov     [BX],AX         ; ...
ifdef _BUILDING_MATHLIB
        pop     DS              ; restore DS
endif
        pop     CX              ; restore CX
        ret                     ; return
__I4LD  endp

