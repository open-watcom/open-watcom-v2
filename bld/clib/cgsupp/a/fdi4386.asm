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


;
include mdef.inc
include struct.inc

        modstart        fdi4386

        xdefp   __FDI4
        xdefp   __RDI4

        xdefp   __FDU4
        xdefp   __RDU4

;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]
;[] __FDU4      convert double float EDX:EAX into 32-bit integer EAX
;[]     Input:  EDX:EAX  - double precision floating point number
;[]     Output: EAX      - 32-bit integer
;[]
;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;       convert floating double to 4-byte integer with rounding

        defpe   __RDU4
        mov     AL,80h+20h      ; indicate we are rounding
        jmp     short DtoI      ; do it

        defpe   __RDI4
        mov     AL,80h+1Fh      ; indicate we are rounding
        jmp     short DtoI      ; do it

        defpe   __FDI4
        mov     AL,1Fh          ; indicate we are truncating
        jmp     short DtoI      ; do it

;       convert floating double to 4-byte integer with truncation

        defpe   __FDU4
        mov     AL,20h          ; indicate we are truncating

DtoI:   _shl    EDX,1           ; get sign
        rcr     AH,1            ; AH <-- sign
        shr     AH,1            ; shift sign bit over 1
        or      AH,AL           ; get rounding bit
        shr     EDX,1           ; restore exponent to its place

;       high bit of AH is rounding bit
;       next bit is the sign bit

;<~> Shift real right four places so that exponent occupies an entire
;<~> word and the mantissa occupies the remaining words. We do not need
;<~> AX because we only need 32 sig digits

        push    ECX             ; save ECX
        mov     ECX,EDX         ; get high part
        sar     ECX,20          ; get exponent to the bottom
        and     CX,07FFh        ; isolate exponent
        sub     CX,03FEh        ; remove bias from exponent
        jl      short DIzero    ; if |real| < .5 goto DIzero
        cmp     CX,20h          ; if exponent > 32
        jg      short DIo_f     ; goto DIo_flow
        and     AL,3Fh          ; isolate # of significant bits
        cmp     CL,AL           ; quit if number too big
        jg      short DIo_f     ; goto DIo_flow
        mov     CH,AH           ; save rounding/truncation bit
        and     EDX,000FFFFFh   ; isolate top 20 bits of fraction
        and     EAX,0FFF00000h  ; isolate next 12 bits of fraction
        or      EDX,EAX         ; glue them together
        rol     EDX,12          ; and rotate into position

        stc                     ; set carry and
        rcr     EDX,1           ; restore implied 1/2 bit

        rcr     CH,1            ; save rounding bit
        cmp     CL,32           ; if want 32 bits
        _if     e               ; then
          mov   EAX,EDX         ; - get them
          _shl  CH,1            ; - get rounding bit
        _else                   ; else
          sub   EAX,EAX         ; - zero result register
          shld  EAX,EDX,CL      ; - shift answer into EAX
          shl   EDX,CL          ; - shift rounding bit into position
          _shl  CH,1            ; - get rid of rounding bit from CH
          _shl  EDX,1           ; - get proper rounding bit
        _endif                  ; endif
        mov     CL,0FFh         ; get mask
        rcr     CL,1            ; get rounding bit
        and     CH,CL           ; mask it with rounding control bit
        _shl    CH,1            ; get rounding bit
        adc     EAX,0           ; add it to the integer to round it up
        _shl    CH,1            ; get sign
        _if     c               ; if negative
          neg   EAX             ; - negate integer
        _endif                  ; endif
        pop     ECX             ; restore ECX
        ret                     ; return

DIo_f:
        mov     EAX,80000000h   ; set answer to largest negative number
        pop     ECX             ; restore ECX
        ret                     ; return
;       jmp     I4OverFlow      ; report overflow

DIzero: sub     EAX,EAX         ; set result to zero
        pop     ECX             ; restore ECX
        ret

        endproc __FDU4
        endproc __FDI4
        endproc __RDI4
        endproc __RDU4

        endmod
        end
