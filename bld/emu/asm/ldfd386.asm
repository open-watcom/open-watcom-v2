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
;* Description:  convert long double to double  __iLDFD,  __EmuLDFD
;*
;*****************************************************************************


ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc
include xception.inc

        xref    FPOverFlow

        modstart    ldfd386, dword


        xdefp   __iLDFD
else
        xdefp   __EmuLDFD
endif

;       convert long double to double
; input:
;       EAX - pointer to long double
;ifdef _BUILDING_MATHLIB
;       EDX - pointer to double
;else
; output:
;       EDX:EAX - double
;endif
;
ifdef _BUILDING_MATHLIB
__iLDFD proc    near
        push    ECX                     ; save ECX
        push    EBX                     ; save EBX
        push    ESI                     ; save ESI
        push    EDX                     ; save address of double
else
__EmuLDFD proc  near
        push    ECX                     ; save ECX
        push    EBX                     ; save EBX
        push    ESI                     ; save ESI
endif
        mov     CX,8[EAX]               ; get exponent and sign
        mov     EDX,4[EAX]              ; get fraction
        mov     EAX,[EAX]               ; ...
        mov     ESI,0FFFFF800h          ; get mask of bits to keep
        mov     EBX,EAX                 ; get bottom part
        shl     EBX,22                  ; get rounding bit
        _if     c                       ; if have to round
          _if   e                       ; - if half way between
            _shl  ESI,1                 ; - - adjust mask
          _endif                        ; - endif
          add   EAX,0800h               ; - round up
          adc   EDX,0                   ; - ...
          _if   c                       ; - if exponent needs adjusting
            mov   EDX,80000000h         ; - - set fraction
            inc   CX                    ; - - increment exponent
            ;  check for overflow
          _endif                        ; - endif
        _endif                          ; endif
        and     EAX,ESI                 ; mask off bottom bits
        mov     EBX,ECX                 ; save exponent and sign
        and     CX,7FFFh                ; if number not 0
        add     CX,03FFh-3FFFh          ; change bias to double format
        _guess                          ; guess: number not too large or -ve
          cmp   CX,07FFh                ; - quit if too large or -ve
          _quif ae                      ; - ...
          _guess                        ; - guess: DENORMAL
            or    CX,CX                 ; - - quit if normal
            _quif ne                    ; - - ...
            shrd  EAX,EDX,12            ; - - drop 12 bits off the bottom
            _shl  EDX,1                 ; - - get rid of implied 1 bit
            shr   EDX,12                ; - - shift right 12
          _admit                        ; - admit: NORMAL
            shrd  EAX,EDX,11            ; - - drop 11 bits off the bottom
            _shl  EDX,1                 ; - - get rid of implied 1 bit
            shrd  EDX,ECX,11            ; - - copy exponent into high part
          _endguess                     ; - endguess
          _shl  BX,1                    ; - get sign
          rcr   EDX,1                   ; - shift into result
        _admit                          ; guess: underflow or denormal
          cmp   CX,0C400h               ; - quit if overflow
          _quif b                       ; - ...
          cmp   CX,-52                  ; - if in the denormal range
          _if   ge                      ; - then
            sub   CX,12                 ; - - adjust exponent for 12 more shifts
            neg   CX                    ; - - negate shift count
            cmp   CL,32                 ; - - if at least 32 bits
            _if   ae                    ; - - then
              sub   CL,32               ; - - - adjust shift count
              mov   ESI,EAX             ; - - - shift right 32
              mov   EAX,EDX             ; - - - ...
              sub   EDX,EDX             ; - - - ...
            _endif                      ; - - endif
            shrd  ESI,EAX,CL            ; - - get the bits from the bottom
            shrd  EAX,EDX,CL            ; - - shift right
            shr   EDX,CL                ; - - ...
            add   ESI,ESI               ; - - round up
            adc   EAX,0                 ; - - ...
            adc   EDX,0                 ; - - ...
          _else                         ; - else
            sub   EAX,EAX               ; - - set result to 0
            sub   EDX,EDX               ; - - ...
            shl   EBX,17                ; - - shift sign bit into carry
            rcr   EDX,1                 ; - - set sign bit 21-may-95
          _endif                        ; - endif
        _admit                          ; admit: OVERFLOW
          shrd  EAX,EDX,11              ; - drop 11 bits off the bottom
          _shl  EDX,1                   ; - get rid of implied 1 bit
          shr   EDX,11                  ; - copy exponent into high part
          _shl  BX,1                    ; - get sign
          rcr   EDX,1                   ; - shift into result
          or    EDX,7FF00000h           ; - set exponent
          cmp   CX,43FFh                ; - quit if infinity or NaN
          _quif e                       ; - ...
          call  FPOverFlow              ; - set OVERFLOW exception
        _endguess                       ; endguess
ifdef _BUILDING_MATHLIB
        pop     ESI                     ; restore address of double
        mov     [ESI],EAX               ; store double value
        mov     4[ESI],EDX              ; ...
        pop     ESI                     ; restore ESI
        pop     EBX                     ; restore EBX
        pop     ECX                     ; restore ECX
        ret                             ; return
__iLDFD endp
else
        pop     ESI                     ; restore ESI
        pop     EBX                     ; restore EBX
        pop     ECX                     ; restore ECX
        ret                             ; return
__EmuLDFD endp
endif

ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
