ifdef _BUILDING_MATHLIB
        xdefp   __iLDFS
else
        xdefp   __EmuLDFS
endif


;       convert long double to float
;ifdef _BUILDING_MATHLIB
; input:
;       SS:AX - pointer to long double
;       SS:DX - pointer to float
;else
; input:
;       DS:BX - pointer to long double
; output:
;       DX:AX - float
;endif
;
ifdef _BUILDING_MATHLIB
__iLDFS proc
        push    DS              ; save registers
        push    BX              ; ...
        push    DX              ; save return pointer
        push    SS              ; get addressability to parms
        pop     DS              ; ...
        mov     BX,AX           ; get parm
else
__EmuLDFS proc  near
endif
        mov     DX,8[BX]        ; get exponent+sign
        mov     AX,DX           ; save sign
        and     DH,7Fh          ; isolate exponent
        add     DX,007Fh-3FFFh  ; change bias to float format
        cmp     DX,0FFh         ; if exponent not too large or negative
        _if     b               ; then
          _shl  AX,1            ; - get sign into carry
          mov   AX,6[BX]        ; - load top part of fraction
          mov   BX,4[BX]        ; - ...
          _rcl  BX,1            ; - save carry, and get rid of implied 1 bit
          _rcl  AX,1            ; - ...
          mov   DH,DL           ; - move exponent to top
          mov   DL,AH           ; - move fraction up
          mov   AH,AL           ; - ...
          mov   AL,BH           ; - ...
          cmp   DH,0            ; - if exponent is 0 (denormal number)
          _if   e               ; - then
            shr   DX,1          ; - - shift right 1 more bit
            rcr   AX,1          ; - - ...
            or    DL,80h        ; - - turn on implied 1 bit
          _endif                ; - endif
          shr   BX,1            ; - get sign
          rcr   DX,1            ; - shift it into number
          rcr   AX,1            ; - ...
ifdef _BUILDING_MATHLIB
          jmp   _ret_ldfs       ; - return
else
          ret                   ; - return
endif
        _endif                  ; endif
        cmp     DX,0C080h       ; if exponent >= 0xC080 then underflow
        _if     ae              ; then
          cmp   DX,-23          ; - if in the denormal range  02-jul-90
          _if   ge              ; - then
            mov   DH,DL         ; - - get exponent into DH
            dec   DH            ; - - decrement exponent for 1 more shift
            mov   DL,7[BX]      ; - - load mantissa
            mov   AX,5[BX]      ; - - ...
            _loop               ; - - loop (denormalize number)
              cmp   DH,-8       ; - - - quit if less than 8
              _quif g           ; - - - ...
              mov   AL,AH       ; - - - shift fraction right by 8
              mov   AH,DL       ; - - - ...
              mov   DL,0        ; - - - ...
              add   DH,8        ; - - - adjust exponent
            _endloop            ; - - endloop
            _loop               ; - - loop (denormalize number)
              cmp   DH,0        ; - - - quit when exponent is 0
              _quif e           ; - - - ...
              shr   DL,1        ; - - - shift fraction right 1 bit
              rcr   AX,1        ; - - - ...
              inc   DH          ; - - - increment exponent
            _endloop            ; - - endloop
            mov   BH,9[BX]      ; - - get sign
            and   BH,80h        ; - - ...
            or    DH,BH         ; - - and set it
ifdef _BUILDING_MATHLIB
            jmp   _ret_ldfs     ; - - return
else
            ret                 ; - - return
endif
          _endif                ; - endif
          sub   AX,AX           ; - set result to zero
          sub   DX,DX           ; - ...
ifdef _BUILDING_MATHLIB
          jmp   _ret_ldfs       ; - return
else
          ret                   ; - return
endif
        _endif                  ; endif
        _shl    AX,1            ; get sign ...
        mov     DX,7F80H shl 1  ; set infinity
        rcr     DX,1            ; ... into exponent word
        push    DX              ; save dx
        call    F8OverFlow      ; set OVERFLOW exception
        pop     DX              ; restore dx
        sub     AX,AX           ; set rest of fraction to 0
ifdef _BUILDING_MATHLIB
_ret_ldfs:                      ; function epilogue
        pop     BX              ; fetch return pointer
        mov     2[BX],DX        ; store return value
        mov     [BX],AX         ; ...
        pop     BX              ; restore registers
        pop     DS              ; ...
        ret                     ; return
__iLDFS endp
else
        ret                     ; return
__EmuLDFS endp
endif
