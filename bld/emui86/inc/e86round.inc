;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<> __frndint - round long double to integer using rounding control
;<>     input:  AX - pointer to operand
;<>             DX - rounding control
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        modstart e86round

        xdefp   __frndint

WPTR    equ     word ptr

Masks:  dw      07FFFh
        dw      03FFFh
        dw      01FFFh
        dw      00FFFh
        dw      007FFh
        dw      003FFh
        dw      001FFh
        dw      000FFh
        dw      0007Fh
        dw      0003Fh
        dw      0001Fh
        dw      0000Fh
        dw      00007h
        dw      00003h
        dw      00001h
        dw      00000h

        defp    __frndint
        push    DI              ; save DI
        mov     DI,AX           ; point to operand
        mov     BX,8[DI]        ; get exponent
        cmp     DX,RC_DOWN      ; if not round down
        _if     ne              ; then
          cmp   DX,RC_UP        ; - check for round up
        _endif                  ; endif
        _if     e               ; if round down or round up
          or    BX,BX           ; - check sign of operand
          _if   s               ; - if operand is negative
            xor   DX,RC_DOWN+RC_UP;- - change up to down and down to up
          _endif                ; - endif
        _endif                  ; endif
        cmp     DX,RC_ZERO      ; if round toward zero
        _if     e               ; then
          mov   DX,RC_DOWN      ; - use round down
        _endif                  ; endif
        and     BX,7FFFh        ; isolate exponent
        sub     BX,3FFFh+64     ; subtract bias + 64
        _if     ge              ; if exponent >= 64
          pop   DI              ; - restore DI
          ret                   ; - return
        _endif                  ; endif
        mov     CX,[DI]         ; get last word of fraction
        add     BX,16           ; if exponent >= 48
        _if     ge              ; then
          _shl  BX,1            ; - double for index
          mov   AX,WPTR Masks[BX]; - get mask
          and   CX,AX           ; - isolate fraction bits
          xor   [DI],CX         ; - get rid of fraction from operand
          inc   AX              ; - get rounding bit mask
          cmp   DX,RC_DOWN      ; - if not round down
          _if   ne              ; - then
            cmp   DX,RC_NEAREST ; - - if round nearest
            _if   e             ; - - then
              _shl  CX,1        ; - - - line up the fraction
              cmp   AX,CX       ; - - - if fraction less than halfway
              ja    done48      ; - - - then done
              _if   e           ; - - - if fraction exactly halfway
                rol   AX,1      ; - - - - the next highest bit must be set to 0
                _if   c         ; - - - - if at word boundary
                  test  2[DI],AX; - - - - - check bottom bit of third word
                _else           ; - - - - else
                  test  [DI],AX ; - - - - - check bit from last word
                _endif          ; - - - - endif
                je      done48  ; - - - - done if value already even
                ror     AX,1    ; - - - - put rounding bit back into place
              _endif            ; - - - endif
            _endif              ; - - endif
            or    CX,CX         ; - - if fraction not zero
            _if   ne            ; - - then
              add   [DI],AX     ; - - - round up
              adc   WPTR 2[DI],0; - - - ...
              adc   WPTR 4[DI],0; - - - ...
              adc   WPTR 6[DI],0; - - - ...
              _if   c           ; - - - if carry
                rcr   WPTR 6[DI],1; - - - set fraction to 8000
                inc   WPTR 8[DI]; - - - - increment exponent
              _endif            ; - - - endif
            _endif              ; - - endif
          _endif                ; - endif
done48:   pop   DI              ; - restore DI
          ret                   ; - return
        _endif                  ; endif
        push    SI              ; save SI
        mov     WPTR [DI],0     ; zero it
        mov     SI,2[DI]        ; get third word of fraction

        add     BX,16           ; if exponent >= 32
        _if     ge              ; then
          _shl  BX,1            ; - double for index
          mov   AX,WPTR Masks[BX]; - get mask
          and   SI,AX           ; - isolate fraction bits
          xor   2[DI],SI        ; - get rid of fraction from operand
          inc   AX              ; - get rounding bit mask
          cmp   DX,RC_DOWN      ; - if not round down
          _if   ne              ; - then
            cmp   DX,RC_NEAREST ; - - if round nearest
            _if   e             ; - - then
              rol   CX,1        ; - - - get top bit of rest of fraction in carry
              adc   SI,SI       ; - - - line up the fraction
              cmp   AX,SI       ; - - - if fraction less than halfway
              ja    done32      ; - - - then done
              _if   e           ; - - - if fraction exactly halfway
                or    CX,CX     ; - - - - check rest of the fraction
                jne   round32   ; - - - - if not zero, then round up
                rol   AX,1      ; - - - - the next highest bit must be set to 0
                _if   c         ; - - - - if at word boundary
                  test  4[DI],AX; - - - - - check bottom bit of second word
                _else           ; - - - - else
                  test  2[DI],AX; - - - - - check bit from third word
                _endif          ; - - - - endif
                je      done32  ; - - - - done if value already even
                ror     AX,1    ; - - - - put rounding bit back into place
              _endif            ; - - - endif
            _endif              ; - - endif
            or    CX,SI         ; - - if fraction not zero
            _if   ne            ; - - then
round32:      add   2[DI],AX    ; - - - round up
              adc   WPTR 4[DI],0; - - - ...
              adc   WPTR 6[DI],0; - - - ...
              _if   c           ; - - - if carry
                rcr   WPTR 6[DI],1; - - - set fraction to 8000
                inc   WPTR 8[DI]; - - - - increment exponent
              _endif            ; - - - endif
            _endif              ; - - endif
          _endif                ; - endif
done32:   pop   SI              ; - restore SI
          pop   DI              ; - restore DI
          ret                   ; - return
        _endif                  ; endif
        or      CL,CH           ; keep sticky bits in CL
        mov     CH,0            ; ...
        or      CX,SI           ; get next word of fraction
        mov     WPTR 2[DI],0    ; and zero it
        mov     SI,4[DI]        ; get second word of fraction

        add     BX,16           ; if exponent >= 16
        _if     ge              ; then
          _shl  BX,1            ; - double for index
          mov   AX,WPTR Masks[BX]; - get mask
          and   SI,AX           ; - isolate fraction bits
          xor   4[DI],SI        ; - get rid of fraction from operand
          inc   AX              ; - get rounding bit mask
          cmp   DX,RC_DOWN      ; - if not round down
          _if   ne              ; - then
            cmp   DX,RC_NEAREST ; - - if round nearest
            _if   e             ; - - then
              rol   CX,1        ; - - - get top bit of rest of fraction in carry
              adc   SI,SI       ; - - - line up the fraction
              cmp   AX,SI       ; - - - if fraction less than halfway
              ja    done16      ; - - - then done
              _if   e           ; - - - if fraction exactly halfway
                or    CX,CX     ; - - - - check rest of the fraction
                jne   round16   ; - - - - if not zero, then round up
                rol   AX,1      ; - - - - the next highest bit must be set to 0
                _if   c         ; - - - - if at word boundary
                  test  6[DI],AX; - - - - - check bottom bit of first word
                _else           ; - - - - else
                  test  4[DI],AX; - - - - - check bit from second word
                _endif          ; - - - - endif
                je      done16  ; - - - - done if value already even
                ror     AX,1    ; - - - - put rounding bit back into place
              _endif            ; - - - endif
            _endif              ; - - endif
            or    CX,SI         ; - - if fraction not zero
            _if   ne            ; - - then
round16:      add   4[DI],AX    ; - - - round up
              adc   WPTR 6[DI],0; - - - ...
              _if   c           ; - - - if carry
                rcr   WPTR 6[DI],1; - - - set fraction to 8000
                inc   WPTR 8[DI]; - - - - increment exponent
              _endif            ; - - - endif
            _endif              ; - - endif
          _endif                ; - endif
done16:   pop   SI              ; - restore SI
          pop   DI              ; - restore DI
          ret                   ; - return
        _endif                  ; endif
        or      CL,CH           ; keep sticky bits in CL
        mov     CH,0            ; ...
        or      CX,SI           ; get next word of fraction
        mov     WPTR 4[DI],0    ; and zero it
        mov     SI,6[DI]        ; get first word of fraction

        add     BX,16           ; if exponent >= 0
        _if     ge              ; then
          _shl  BX,1            ; - double for index
          mov   AX,WPTR Masks[BX]; - get mask
          and   SI,AX           ; - isolate fraction bits
          xor   6[DI],SI        ; - get rid of fraction from operand
          inc   AX              ; - get rounding bit mask
          cmp   DX,RC_DOWN      ; - if not round down
          _if   ne              ; - then
            cmp   DX,RC_NEAREST ; - - if round nearest
            _if   e             ; - - then
              rol   CX,1        ; - - - get top bit of rest of fraction in carry
              adc   SI,SI       ; - - - line up the fraction
              cmp   AX,SI       ; - - - if fraction less than halfway
              ja    done00      ; - - - then done
              _if   e           ; - - - if fraction exactly halfway
                or    CX,CX     ; - - - - check rest of the fraction
                jne   round00   ; - - - - if not zero, then round up
                _shl  AX,1      ; - - - - the next highest bit must be set to 0
                _if   nc        ; - - - - if not at word boundary
                  test  6[DI],AX; - - - - - check bit from first word
                _endif          ; - - - - endif
                je      done00  ; - - - - done if value already even
                shr     AX,1    ; - - - - put rounding bit back into place
              _endif            ; - - - endif
            _endif              ; - - endif
            or    CX,SI         ; - - if fraction not zero
            _if   ne            ; - - then
round00:      add   6[DI],AX    ; - - - round up
              _if   c           ; - - - if carry
                rcr   WPTR 6[DI],1; - - - set fraction to 8000
                inc   WPTR 8[DI]; - - - - increment exponent
              _endif            ; - - - endif
            _endif              ; - - endif
          _endif                ; - endif
done00:   pop   SI              ; - restore SI
          pop   DI              ; - restore DI
          ret                   ; - return
        _endif                  ; endif
        cmp     DX,RC_DOWN      ; if not round down
        _if     ne              ; then
          cmp   DX,RC_NEAREST   ; - if round nearest
          _if   e               ; - then
            inc   BX            ; - - if exponent was not 0x3ffe
            jne   setzero       ; - - then set result to 0
            cmp   SI,8000h      ; - - if fraction exactly halfway
            _if   e             ; - - then
              or    CX,CX       ; - - - check low order bits to make sure
              je    setzero     ; - - - then set result to 0
            _endif              ; - - endif
          _endif                ; - endif
          mov   WPTR 6[DI],8000h; - set result to 1.0 or -1.0
          or    WPTR 8[DI],3FFFh; - ... 20-aug-90 changed mov to or
          pop   SI              ; - restore SI
          pop   DI              ; - restore DI
          ret                   ; - return
        _endif                  ; endif
setzero:sub     AX,AX           ; set result to 0
        mov     6[DI],AX        ; ...
        mov     8[DI],AX        ; ...
        pop     SI              ; restore SI
        pop     DI              ; restore DI
        ret                     ; return

        endproc __frndint

