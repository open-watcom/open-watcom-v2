
ifdef _BUILDING_MATHLIB
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
        mov     BX,DX           ; get exponent
        shl     BX,1            ; BH has exponent
        cmp     BH,0            ; if exponent not zero
        _if     ne              ; then
          mov   BL,BH           ; - move exponent to bottom
          mov   BH,0            ; - zero high part
          add   BX,3FFFh-007Fh  ; - change bias to temp real format
          shl   BX,1            ; - move left to accept the sign bit
          shl   DX,1            ; - get sign bit
          rcr   BX,1            ; - move it in with exponent
          mov   DH,DL           ; - align fraction
          mov   DL,AH           ; - ...
          mov   AH,AL           ; - ...
          mov   AL,0            ; - ...
          stc                   ; - set implied one bit
          rcr   DH,1            ; - ...
        _else                   ; else (number is 0)
          sub   AX,AX           ; - zero registers
          mov   DX,AX           ; - ...
          mov   DX,BX           ; - ...
        _endif                  ; endif
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
