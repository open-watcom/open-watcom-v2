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
;* Description:  convert long double into 8-byte integer
;*
;*****************************************************************************

ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart    ldi8386, dword

endif

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;  
;   __LDI8 - convert long double into 8-byte integer with truncation
;            it is assumed that the long double has already been rounded
;            to an integer by calling __frndint.
;  
;ifdef _BUILDING_MATHLIB
;       input:  EAX - pointer to long double operand 
;               EDX - pointer to 8-byte integer result
;else
;       input:  EAX     - pointer to long double operand 
;       output: EDX:EAX - 8-byte integer
;endif
;  
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        xdefp   __LDI8

        defp    __LDI8
        push    EBX             ; save EBX

;       high bit of BH is sign bit

        push    ECX             ; save ECX
ifdef _BUILDING_MATHLIB
        push    EDX             ; save EDX
endif
        mov     CX,8[EAX]       ; get exponent
        mov     BH,CH           ; get sign
        and     CX,7FFFh        ; isolate exponent
        _guess                  ; guess: 0
          _quif ne              ; - quit if not 0
          sub   EAX,EAX         ; - set result to 0
          sub   EDX,EDX
        _admit                  ; guess: |x| < 1.0
          sub   CX,3FFEh        ; - remove bias from exponent
          _quif g               ; - quit if >= 1.0
          _shl  BH,1            ; - get sign bit
          sbb   EAX,EAX         ; - answer is 0 or -1
          sbb   EDX,EDX         ; - ...
        _admit                  ; guess: number too large
          cmp   CX,3Fh          ; - if exponent > 63
          _quif le              ; - quit it not too large
          mov   EDX,80000000h   ; - set largest integer
          sub   EAX,EAX         ; - ...
        _admit                  ; admit: can fit in integer
          push  ESI
          mov   CH,BH           ; - save rounding/truncation bit
          mov   ESI,[EAX]       ; - get least significant part of mantissa
          mov   EBX,4[EAX]      ; - get most significant part of mantissa
          sub   EAX,EAX         ; - zero result register
          mov   EDX,EAX
          cmp   CL,1Fh          ; - if exponent > 31
          _if g                 ; - shift 32
            mov EAX,EBX
            mov EBX,ESI
            sub ESI,ESI
            sub CL,20h
          _endif
          or    CL,CL
          _if ne
            shld  EDX,EAX,CL    ; - shift answer into EDX:EAX
            shld  EAX,EBX,CL    ; - ...
            shld  EBX,ESI,CL    ; - ...
            shl   ESI,CL        ; - ...
          _endif
          _shl  CH,1            ; - if number is negative
          _if   c               ; - then
            neg EDX             ; - - negate the integer
            neg EAX             ; - - ...
            sbb EDX,0           ; - - ...
          _endif                ; - endif
          pop   ESI
        _endguess               ; endguess
ifdef _BUILDING_MATHLIB
        pop     EBX             ; restore EDX
        mov     4[EBX],EDX
        mov     [EBX],EAX
endif
        pop     ECX             ; restore ECX
        pop     EBX             ; restore EBX
        ret                     ; return

        endproc __LDI8

ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
