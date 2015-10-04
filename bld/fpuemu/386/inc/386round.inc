;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************



        xdefp   __frndint

DPTR    equ     dword ptr
WPTR    equ     word ptr

Masks   dd      07FFFFFFFh
        dd      03FFFFFFFh
        dd      01FFFFFFFh
        dd      00FFFFFFFh
        dd      007FFFFFFh
        dd      003FFFFFFh
        dd      001FFFFFFh
        dd      000FFFFFFh
        dd      0007FFFFFh
        dd      0003FFFFFh
        dd      0001FFFFFh
        dd      0000FFFFFh
        dd      00007FFFFh
        dd      00003FFFFh
        dd      00001FFFFh
        dd      00000FFFFh
        dd      000007FFFh
        dd      000003FFFh
        dd      000001FFFh
        dd      000000FFFh
        dd      0000007FFh
        dd      0000003FFh
        dd      0000001FFh
        dd      0000000FFh
        dd      00000007Fh
        dd      00000003Fh
        dd      00000001Fh
        dd      00000000Fh
        dd      000000007h
        dd      000000003h
        dd      000000001h
        dd      000000000h

        defp    __frndint

        push    EDI             ; save EDI
        mov     EDI,EAX         ; point to operand
        sub     EBX,EBX         ; zero index register
        mov     BX,8[EDI]       ; get exponent
        cmp     DX,RC_DOWN      ; if not round down
        _if     ne              ; then
          cmp   DX,RC_UP        ; - check for round up
        _endif                  ; endif
        _if     e               ; if round down or round up
          or    BH,BH           ; - check sign of operand
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
          pop   EDI             ; - restore EDI
          ret                   ; - return
        _endif                  ; endif
        mov     ECX,[EDI]       ; get last word of fraction
        add     BX,32           ; if exponent >= 32
        _if     ge              ; then
          mov   EAX,DPTR Masks[EBX*4]; - get mask
          and   ECX,EAX         ; - isolate fraction bits
          xor   [EDI],ECX       ; - get rid of fraction from operand
          inc   EAX             ; - get rounding bit mask
          cmp   DX,RC_DOWN      ; - if not round down
          _if   ne              ; - then
            cmp   DX,RC_NEAREST ; - - if round nearest
            _if   e             ; - - then
              add   ECX,ECX     ; - - - line up the fraction
              cmp   EAX,ECX     ; - - - if fraction less than halfway
              ja    done32      ; - - - then done
              _if   e           ; - - - if fraction exactly halfway
                rol   EAX,1     ; - - - - the next highest bit must be set to 0
                _if   c         ; - - - - if at word boundary
                  test  4[EDI],EAX; - - - - - check bottom bit of third word
                _else           ; - - - - else
                  test  [EDI],EAX ; - - - - - check bit from last word
                _endif          ; - - - - endif
                je      done32  ; - - - - done if value already even
                ror     EAX,1   ; - - - - put rounding bit back into place
              _endif            ; - - - endif
            _endif              ; - - endif
            or    ECX,ECX       ; - - if fraction not zero
            _if   ne            ; - - then
              add   [EDI],EAX   ; - - - round up
              adc   DPTR 4[EDI],0 ; - - ...
              _if   c           ; - - - if carry
                rcr   DPTR 4[EDI],1; - - - set fraction to 80000000
                inc   WPTR 8[EDI] ; - - - increment exponent
              _endif            ; - - - endif
            _endif              ; - - endif
          _endif                ; - endif
done32:   pop   EDI             ; - restore EDI
          ret                   ; - return
        _endif                  ; endif
        push    ESI             ; save ESI
        mov     DPTR [EDI],0    ; zero it
        mov     ESI,4[EDI]      ; get top word of fraction

        add     BX,32           ; if exponent >= 0
        _if     ge              ; then
          mov   EAX,DPTR Masks[EBX*4]; - get mask
          and   ESI,EAX         ; - isolate fraction bits
          xor   4[EDI],ESI      ; - get rid of fraction from operand
          inc   EAX             ; - get rounding bit mask
          cmp   DX,RC_DOWN      ; - if not round down
          _if   ne              ; - then
            cmp   DX,RC_NEAREST ; - - if round nearest
            _if   e             ; - - then
              rol   ECX,1       ; - - - get top bit of bottom word in carry
              adc   ESI,ESI     ; - - - line up the fraction
              cmp   EAX,ESI     ; - - - if fraction less than halfway
              ja    done00      ; - - - then done
              _if   e           ; - - - if fraction exactly halfway
                or    ECX,ECX   ; - - - - check rest of the fraction
                jne   round00   ; - - - - if not zero, then round up
                _shl  EAX,1     ; - - - - the next highest bit must be set to 0
                _if   nc        ; - - - - if not at word boundary
                  test  4[EDI],EAX; - - - - - check bit from first word
                _endif          ; - - - - endif
                je      done00  ; - - - - done if value already even
                shr     EAX,1   ; - - - - put rounding bit back into place
              _endif            ; - - - endif
            _endif              ; - - endif
            or    ECX,ESI       ; - - if fraction not zero
            _if   ne            ; - - then
round00:      add   4[EDI],EAX  ; - - - round up
              _if   c           ; - - - if carry
                rcr   DPTR 4[EDI],1; - - - set fraction to 80000000
                inc   WPTR 8[EDI] ; - - - increment exponent
              _endif            ; - - - endif
            _endif              ; - - endif
          _endif                ; - endif
done00:   pop   ESI             ; - restore ESI
          pop   EDI             ; - restore EDI
          ret                   ; - return
        _endif                  ; endif
        cmp     DX,RC_DOWN      ; if not round down
        _if     ne              ; then
          cmp   DX,RC_NEAREST   ; - if round nearest
          _if   e               ; - then
            inc   BX            ; - - if exponent was not 0x3ffe
            jne   setzero       ; - - then set result to 0
            cmp   ESI,80000000h ; - - if fraction exactly halfway
            _if   e             ; - - then
              or    ECX,ECX     ; - - - check low order bits to make sure
              je    setzero     ; - - - then set result to 0
            _endif              ; - - endif
          _endif                ; - endif
          mov   DPTR 4[EDI],80000000h; - set result to 1.0 or -1.0
          or    WPTR 8[EDI],3FFFh; - ...
          pop   ESI             ; - restore ESI
          pop   EDI             ; - restore EDI
          ret                   ; - return
        _endif                  ; endif
setzero:sub     EAX,EAX         ; set result to 0
        mov     4[EDI],EAX      ; ...
        mov     8[EDI],AX       ; ...
        pop     ESI             ; restore ESI
        pop     EDI             ; restore EDI
        ret                     ; return

        endproc __frndint
