;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<> __LDI4 - convert long double to 4-byte integer
;<>          it is assumed that the long double has already been rounded
;<>          to an integer by calling __frndint.
;<>   ifdef _BUILDING_MATHLIB
;<>     input:  AX - pointer to operand
;<>   else
;<>     input:  BX - pointer to operand
;<>   endif
;<>     output: DX:AX 4-byte integer
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        xdefp   __LDI4
        xdefp   __LDU4

        defp    __LDI4
        defp    __LDU4
ifdef _BUILDING_MATHLIB
        push    DS              ; save DS
        push    BX              ; save BX
        push    SS              ; fpc code assumes parms are relative to SS
        pop     DS              ; ...
        mov     BX,AX           ; setup parm
endif
        mov     AX,8[BX]        ; get exponent
        and     AX,7FFFh        ; isolate exponent
        cmp     AX,3FFFh        ; if number < 1.0
        _if     b               ; then
          sub   AX,AX           ; - result is 0
          cwd                   ; - ...
ifdef _BUILDING_MATHLIB
          pop   BX              ; - restore BX
          pop   DS              ; - restore DS
endif
          ret                   ; - return
        _endif                  ; endif
        cmp     AX,401Eh        ; if number too large
        _if     ae              ; then
          mov   DX,8000h        ; - return largest number
          sub   AX,AX           ; - ...
ifdef _BUILDING_MATHLIB
          pop   BX              ; - restore BX
          pop   DS              ; - restore DS
endif
          ret                   ; - return
        _endif                  ; endif
        push    CX              ; save CX
        sub     AX,401Eh        ; calculate # of bits to shift
        neg     AX              ; ...
        mov     CL,AL           ; get shift count
        mov     DX,6[BX]        ; get fraction
        mov     AX,4[BX]        ; ...
        _loop                   ; loop
          cmp   CL,16           ; - quit if < 16 bits to shift
          _quif l               ; - ...
          mov   AX,DX           ; - shift right 16
          sub   DX,DX           ; - zero high word
          sub   CL,16           ; - adjust shift count
        _endloop                ; endloop
        cmp     CL,8            ; if >= 8 bits to shift
        _if     ge              ; then
          mov   AL,AH           ; - shift right 8 bits
          mov   AH,DL           ; - ...
          mov   DL,DH           ; - ...
          mov   DH,0            ; - ...
          sub   CL,8            ; - adjust shift count
        _endif                  ; endif
        cmp     CL,0            ; if some bits to shift
        _if     ne              ; then
          _loop                 ; - loop (bit shift)
            shr   DX,1          ; - - shift right 1 bit
            rcr   AX,1          ; - - ...
            dec   CL            ; - - decrement shift count
          _until  e             ; - until done
        _endif                  ; endif
;
        mov     CX,8[BX]        ; get sign of the value
        or      CX,CX           ; if negative
        _if     s               ; then
          not   DX              ; - negate the value
          neg   AX              ; - ...
          sbb   DX,-1           ; - ...
        _endif                  ; endif
        pop     CX              ; restore CX
ifdef _BUILDING_MATHLIB
        pop     BX              ; restore BX
        pop     DS              ; restore DS
endif
        ret                     ; return to caller

        endproc __LDU4
        endproc __LDI4

