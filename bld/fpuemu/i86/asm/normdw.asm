
ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc
include shiftmac.inc

        modstart    normdw, word

endif

        xdefp   __dw_normalize

;       normalize 32-bit fraction
;
;       CX      exponent
;       DX:AX   fraction
;

        defp    __dw_normalize

        _loop                   ; loop shift fraction by 16 quantities
          or  DX,DX             ; - check fraction MSB (16-bits)
        _quif nz                ; quit if not zero
          sub   CX,16           ; - adjust result exponent
          dw_lshift_16          ; - shift fraction << 16
        _endloop                ; endloop
        or    DH,DH             ; check fraction MSB (8-bits)
        _if z                   ; if MSB zero
          sub   CX,8            ; - adjust result exponent
          dw_lshift_8           ; - shift fraction << 8
          or  DH,DH             ; - check fraction MSB (8-bits)
        _endif                  ; endif
        _if ns                  ; if not normalized (implied bit is not set)
          _loop                 ; - loop
            dec   CX;           ; - - adjust result exponent
            dw_lshift_1         ; - - shift fraction << 1
          _until s              ; - until normalized (implied bit is set)
        _endif                  ; endif
        ret                     ; return

        endproc __dw_normalize

ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
