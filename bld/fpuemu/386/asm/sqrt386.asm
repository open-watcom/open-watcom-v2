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
;* Description:  sqrt function for long double argument
;*
;*****************************************************************************

ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart  sqrt386

        xrefp   __iFDLD
        xrefp   __iLDFD
endif

        xdefp   __sqrt

ifdef _BUILDING_MATHLIB

;
;       double __sqrtd( double );
;
;       input:  EDX:EAX - double
;       output: EDX:EAX - double
;
        xdefp   __sqrtd

        defp    __sqrtd

        push    EDX                     ; make double parm addressable
        push    EAX                     ; ...
        mov     EAX,ESP                 ; get address of first parm
        sub     ESP,12                  ; allocate space for long double
        mov     EDX,ESP                 ; point EDX to long double
        call    __iFDLD                 ; convert double to long double
        mov     EAX,ESP                 ; point to long double
        call    __sqrt                  ; calculate square root
        mov     EAX,ESP                 ; point to long double
        mov     EDX,ESP                 ; get address of double
        add     EDX,12                  ; ...
        call    __iLDFD                 ; convert it to double
        add     ESP,12                  ; remove long double from stack
        pop     EAX                     ; pop double into return regs
        pop     EDX                     ; ...
        ret                             ; return

        endproc __sqrtd

include fstatus.inc

endif

;
;       void __sqrt( long double * );
;
;       input:  EAX - pointer to long double operand
;

        defp    __sqrt

        push    EDI             ; save EDI
        push    ESI             ; save ESI
        push    EDX             ; save EDX
        push    ECX             ; save ECX
        push    EBX             ; save EBX
        mov     CX,8[EAX]       ; get exponent
        mov     EDX,4[EAX]      ; get operand
        mov     EBX,[EAX]       ; ...
        _guess                  ; guess: special number
          or    EBX,EBX         ; - quit if not zero
          _quif ne              ; - ...
          or    EDX,EDX         ; - if fraction all zero
          _if   e               ; - then
            _shl  CX,1          ; - - get rid of sign bit
            je    sqrt9         ; - - answer is 0 if exponent is 0
indefinite: mov   word ptr 8[EAX],0FFFFh ; - - set result to indefinite
            mov   dword ptr 4[EAX],0    ; - - ...
            mov   dword ptr [EAX],0     ; - - ...
sq_NaN:     or    byte ptr 7[EAX],0C0h  ; - - set result to NaN
            jmp   sqrt9         ; - - return
          _endif                ; - endif
          cmp   CX,7FFFh        ; - if +ve infinity
          je    sqrt9           ; - answer is +ve infinity if arg is +vf inf.
        _endguess               ; endguess
        _guess                  ; guess: NaN
          mov   EDX,ECX         ; - get exponent
          and   DH,07Fh         ; - get rid of sign bit
          cmp   DX,07FFFh       ; - check for NaN
          je    sq_NaN          ; - result is a NaN
          _shl  CX,1            ; - get rid of sign bit
          jc    sq_NaN          ; - sqrt(-ve) = -ve
          shr   CX,1            ; - restore sign
          _quif e               ; - quit if number is denormal
          mov   EDX,4[EAX]      ; - get top part of fraction
          _shl  EDX,1           ; - top bit should be on
          jnc   indefinite      ; - number is an unnormal
        _endguess               ; endguess
        mov     EDX,4[EAX]      ; get operand
        sub     EDI,EDI         ; zero guard bits
        push    EAX             ; save address of operand
        sub     CX,3FFFh        ; remove bias
        sar     CX,1            ; divide by 2
        _if     nc              ; if exponent is even
          shr   EDX,1           ; - divide argument by 2
          rcr   EBX,1           ; - ...
          rcr   EDI,1           ; - save guard bit
        _endif                  ; endif
        add     CX,3FFFh        ; add bias back in
        mov     8[EAX],CX       ; store exponent
        mov     ECX,EDX         ; save operand
        mov     EAX,EBX         ; ...
        mov     ESI,EDX         ; get high order word
        stc                     ; calculate initial estimate
        rcr     ESI,1           ; ...
        inc     EDX             ; check for EDX=FFFFFFFFh
        _if     ne              ; if not -1, then
          dec   EDX             ; - restore EDX
          _loop                 ; - loop
            div   ESI           ; - - calculate newer estimate
            dec   ESI           ; - - want estimate to be within one
            cmp   ESI,EAX       ; - -
            _quif na            ; - - quit if estimate good enough
            inc   ESI           ; - -
            add   ESI,EAX       ; - - calculate new estimate as (old+new)/2
            rcr   ESI,1         ; - - ...
            mov   EDX,ECX       ; - - restore operand
            mov   EAX,EBX       ; - - ...
          _endloop              ; - endloop
          inc   ESI             ; - restore divisor
          mov   ECX,EAX         ; - save word of quotient
          mov   EAX,EDI         ; - get guard bit
          div   ESI             ; - calculate next word of quotient
;
;       ESI:0   estimate is too small
;       ECX:EAX estimate is too large
;       calculate new estimate as (ESI:0+ECX:EAX)/2
;
          add   ESI,ECX         ; - ...
        _else                   ; else (high word was -1)
          cmp   EAX,ESI         ; - if low word not -1
          je    short sqrt8     ; - then
          xchg  EAX,EDX         ; - flip around
          mov   EAX,EDI         ; - get guard bit
          div   ESI             ; - calculate value for last 32 bits
        _endif                  ; endif
        sub     EDX,EDX         ; zero EDX
        stc                     ; divide by 2
        rcr     ESI,1           ; ...
        rcr     EAX,1           ; ...
        adc     EAX,EDX         ; round up (EDX=0)
sqrt8:  adc     EDX,ESI         ; ...

        pop     ESI             ; restore address of operand
        mov     [ESI],EAX       ; store result
        mov     4[ESI],EDX      ; ...
sqrt9:  pop     EBX             ; restore EBX
        pop     ECX             ; restore ECX
        pop     EDX             ; restore EDX
        pop     ESI             ; restore ESI
        pop     EDI             ; restore EDI
        ret                     ; return to caller

        endproc __sqrt

ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
