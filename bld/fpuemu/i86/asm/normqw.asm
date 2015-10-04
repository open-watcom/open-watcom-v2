ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc
include shiftmac.inc

        modstart    normqw, word

endif

        xdefp   __qw_normalize

;       normalize 64-bit fraction
;
;       DI              exponent
;       AX:BX:CX:DX     fraction
;

        defp    __qw_normalize

        _loop                   ; loop shift fraction by 16 quantities
          or  AX,AX             ; - check fraction MSB (16-bits)
        _quif nz                ; quit if not zero
          sub DI,16             ; - adjust result exponent
          qw_lshift_16          ; - shift fraction << 16
        _endloop                ; endloop
        or  AH,AH               ; check fraction MSB (8-bits)
        _if z                   ; if zero
          sub DI,8              ; - adjust result exponent
          qw_lshift_8           ; - shift fraction << 8
          or  AH,AH             ; - check fraction MSB (8-bits)
        _endif                  ; endif
        _if ns                  ; if not normalized (implied bit is not set)
          _loop                 ; - loop
            dec DI              ; - - adjust result exponent
            qw_lshift_1         ; - - shift fraction << 1
          _until s              ; - until normalized (implied bit is set)
        _endif                  ; endif
        ret                     ; return

        endproc __qw_normalize


ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
