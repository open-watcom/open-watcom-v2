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
;* Description:  convert long double into integer
;*
;*****************************************************************************



ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart    ldi4386, dword

endif

        xdefp   __LDI4

        xdefp   __LDU4
        xdefp   __RLDU4

;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]
;[] __LDU4      convert long double [EAX] into 32-bit integer EAX
;[]     Input:  EAX  - pointer to long double
;[]     Output: EAX  - 32-bit integer
;[]
;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;       convert floating double to 4-byte integer with rounding

        defp    __RLDU4
        push    EBX             ; save EBX
        mov     BL,80h+20h      ; indicate we are rounding
        jmp     short DtoI      ; do it

        defp    __RLDI4
        push    EBX             ; save EBX
        mov     BH,9[EAX]       ; get sign

;       high bit of BH is sign bit

        push    ECX             ; save ECX
        mov     CX,8[EAX]       ; get exponent
        and     CX,7FFFh        ; isolate exponent
        _guess                  ; guess: 0
          _quif ne              ; - quit if not 0
          sub   EAX,EAX         ; - set result to 0
        _admit                  ; guess: |x| < 1.0
          sub   CX,3FFEh        ; - remove bias from exponent
          _quif g               ; - quit if >= 1.0
          _shl  BH,1            ; - get sign bit
          sbb   EAX,EAX         ; - answer is 0 or -1
        _admit                  ; guess: number too large
          cmp   CX,1Fh          ; - if exponent > 31
          _quif le              ; - quit it not too large
          mov   EAX,80000000h   ; - set largest integer
        _admit                  ; admit: can fit in integer
          mov   CH,BH           ; - save rounding/truncation bit
          mov   EBX,[EAX]       ; - get least significant part of mantissa
          mov   EAX,4[EAX]      ; - get most significant part of mantissa
          push  EDX             ; - save EDX
          sub   EDX,EDX         ; - zero result register
          shld  EDX,EAX,CL      ; - shift answer into EDX
          shl   EAX,CL          ; - shift rounding bit into position
          or    EBX,EAX         ; - EBX is fraction bits
          mov   EAX,EDX         ; - get result
          pop   EDX             ; - restore EDX
          _shl  CH,1            ; - if number is negative
          _if   c               ; - then
            or  EBX,EBX         ; - - if fraction is non zero
            _if ne              ; - - then
              inc   EAX         ; - - - increment number
            _endif              ; - - endif
            neg   EAX           ; - - negate the integer
          _endif                ; - endif
        _endguess               ; endguess
        pop     ECX             ; restore ECX
        pop     EBX             ; restore EBX
        ret                     ; return

        defp    __LDI4
        push    EBX             ; save EBX
        mov     BL,1Fh          ; indicate we are truncating
        jmp     short DtoI      ; do it

;       convert floating double to 4-byte integer with truncation

        defp    __LDU4
        push    EBX             ; save EBX
        mov     BL,20h          ; indicate we are truncating

DtoI:   mov     BH,9[EAX]       ; get sign
        shr     BH,1            ; shift sign bit over 1
        or      BH,BL           ; get rounding bit

;       high bit of BH is rounding bit
;       next bit is the sign bit

        push    ECX             ; save ECX
        mov     CX,8[EAX]       ; get exponent
        and     CX,7FFFh        ; isolate exponent
        sub     CX,3FFEh        ; remove bias from exponent
        jl      short DIzero    ; if |real| < .5 goto DIzero
        cmp     CX,20h          ; if exponent > 32
        jg      short DIo_f     ; goto DIo_flow
        and     BL,3Fh          ; isolate # of significant bits
        cmp     CL,BL           ; quit if number too big
        jg      short DIo_f     ; goto DIo_flow
        mov     CH,BH           ; save rounding/truncation bit
        mov     EBX,[EAX]       ; get least significant part of mantissa
        mov     EAX,4[EAX]      ; get most significant part of mantissa
        _shl    EBX,1           ; get rounding bit
        rcr     CH,1            ; save rounding bit
        cmp     CL,32           ; if want 32 bits
        _if     e               ; then
          _shl  CH,1            ; - get rounding bit
        _else                   ; else
          sub   EBX,EBX         ; - zero result register
          shld  EBX,EAX,CL      ; - shift answer into EAX
          shl   EAX,CL          ; - shift rounding bit into position
          _shl  CH,1            ; - get rid of rounding bit from CH
          _shl  EAX,1           ; - get proper rounding bit
          mov   EAX,EBX         ; - get result
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
        pop     EBX             ; restore EBX
        ret                     ; return

DIo_f:
        mov     EAX,80000000h   ; set answer to largest negative number
        pop     ECX             ; restore ECX
        pop     EBX             ; restore EBX
        ret                     ; return
;       jmp     I4OverFlow      ; report overflow

DIzero: sub     EAX,EAX         ; set result to zero
        pop     ECX             ; restore ECX
        pop     EBX             ; restore EBX
        ret

        endproc __LDU4
        endproc __LDI4
        endproc __RLDI4
        endproc __RLDU4

ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
