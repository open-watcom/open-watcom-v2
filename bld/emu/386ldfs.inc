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



ifdef _BUILDING_MATHLIB
        xdefp   __iLDFS
else
        xdefp   __EmuLDFS
endif

;       convert long double to float
; input:
;       EAX - pointer to long double
;ifdef _BUILDING_MATHLIB
;       EDX - pointer to float
;else
; output:
;       EAX - float
;endif
;
ifdef _BUILDING_MATHLIB
__iLDFS proc  near
else
__EmuLDFS proc  near
endif
        push    EDX                     ; save EDX
        push    ECX                     ; save ECX
        push    EBX                     ; save EBX
        mov     EBX,EAX                 ; get pointer to long double
        mov     EDX,0FFFFFF00h          ; get mask of bits to keep
        mov     EAX,4[EBX]              ; get top part
        mov     ECX,EAX                 ; duplicate it
        shl     ECX,25                  ; get rounding bit
        mov     CX,8[EBX]               ; get exponent and sign
        _if     c                       ; if have to round
          _if   e                       ; - if half way between
            cmp   dword ptr [EBX],0     ; - - if bottom part is also 0
            _if   e                     ; - - then (half way)
              _shl  EDX,1               ; - - - adjust mask
            _endif                      ; - - endif
          _endif                        ; - endif
          add   EAX,0100h               ; - round up
          _if   c                       ; - if exponent needs adjusting
            mov   EAX,80000000h         ; - - set fraction
            inc   CX                    ; - - increment exponent
            ;  check for overflow
          _endif                        ; - endif
        _endif                          ; endif
        and     EAX,EDX                 ; mask off bottom bits
        mov     EBX,ECX                 ; save exponent and sign
        and     CX,7FFFh                ; if number not 0
        _if     ne                      ; then
          _guess                        ; - guess: infinity or NaN
            cmp   CX,7FFFh              ; - - quit if not infinity or NaN
            _quif ne                    ; - - ...
            _shl  EAX,1                 ; - - get rid of implied 1 bit
            shr   EAX,8                 ; - - shift fraction over
            or    EAX,0FF000000h        ; - - set exponent to all one's
            _shl  BX,1                  ; - - get sign
            rcr   EAX,1                 ; - - shift into result
          _admit                        ; - guess: UNDERFLOW
            add   CX,007Fh-3FFFh        ; - - change bias to float format
            _quif ns                    ; - - quit if not signed
            sub   EAX,EAX               ; - - set result to 0
          _admit                        ; - guess: OVERFLOW
            cmp   CX,00FFh              ; - - quit if not overflow
            _quif l                     ; - - ...
            mov   EAX,7F800000h shl 1   ; - - set infinity
            _shl  BX,1                  ; - - get sign
            rcr   EAX,1                 ; - - shift into result
          _admit                        ; - admit:
            _shl  EAX,1                 ; - - get rid of implied 1 bit
            shrd  EAX,ECX,8             ; - - copy exponent into high part
            _shl  BX,1                  ; - - get sign
            rcr   EAX,1                 ; - - shift into result
          _endguess                     ; - endguess
        _endif                          ; endif
        pop     EBX                     ; restore EBX
        pop     ECX                     ; restore ECX
        pop     EDX                     ; restore EDX
ifdef _BUILDING_MATHLIB
        mov     [EDX],EAX               ; store float value
        ret                             ; return
__iLDFS endp
else
        ret                             ; return
__EmuLDFS endp
endif
