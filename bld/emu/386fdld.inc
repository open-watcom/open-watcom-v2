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
        xdefp   __iFDLD
else
        xdefp   __EmuFDLD
endif

;       convert double to long double
; input:
;ifdef _BUILDING_MATHLIB
;       EAX     pointer to double
;       EDX     pointer to long double to be filled in
;else
;       EDX:EAX double
;       EBX     pointer to long double to be filled in
;endif

ifdef _BUILDING_MATHLIB
__iFDLD proc    near
        push    ECX                     ; save ECX
        push    EBX                     ; save EBX
        mov     EBX,EDX                 ; get pointer to long double
        mov     EDX,4[EAX]              ; get double
        mov     EAX,[EAX]               ; ...
else
__EmuFDLD proc  near
        push    ECX                     ; save ECX
endif
        mov     ECX,EDX                 ; get exponent and sign
        shld    EDX,EAX,11              ; shift fraction left 11 bits
        shl     EAX,11                  ; ...
        _guess                          ; guess: a normal number
          sar   ECX,32-12               ; - shift exponent to bottom
          and   CX,07FFh                ; - isolate exponent
          _quif e                       ; - quit if denormal number
          _guess                        ; - guess: normal number
            cmp   CX,07FFh              ; - - quit if infinity or NaN
            _quif e                     ; - - ...
            add   CX,3FFFh-03FFh        ; - - change bias to temp real format
          _admit                        ; - guess: NaN
            mov   CX,7FFFh              ; - - set exponent for infinity or NaN
            test  EDX,7FFFFFFFh         ; - - check for infinity
            _if   e                     ; - - if top part is 0
              or    EAX,EAX             ; - - - check low word
            _endif                      ; - - endif
            _quif e                     ; - - quit if infinity
            push  EDX                   ; - - save EDX
            push  EAX                   ; - - save EAX
            call  F8InvalidOp           ; - - indicate "Invalid" exception
            pop   EAX                   ; - - restore EAX
            pop   EDX                   ; - - restore EDX
            or    EDX,40000000h         ; - - indicate NaN
          _endguess                     ; - endguess
          or    EDX,80000000h           ; - turn on implied 1 bit
        _admit                          ; guess: zero
          or    EDX,EDX                 ; - quit if non-zero
          _quif ne                      ; - ...
          or    EAX,EAX                 ; - ...
          _quif ne                      ; - ...
          sub   ECX,ECX                 ; - set exponent and sign to 0
        _admit                          ; admit: denormal number
          mov   CX,3C01h                ; - set exponent
          or    EDX,EDX                 ; - if high word is zero
          _if   e                       ; - then
            xchg  EAX,EDX               ; - - shift number left 32 bits
            sub   CX,32                 ; - - adjust exponent
          _endif                        ; - endif
          _loop                         ; - loop (normalize number)
            or    EDX,EDX               ; - - quit if top bit is on
            _quif s                     ; - - ...
            _shl  EAX,1                 ; - - shift number left 1 bit
            _rcl  EDX,1                 ; - - ...
            dec   CX                    ; - - decrement exponent
          _endloop                      ; - endloop
        _endguess                       ; endguess
        mov     [EBX],EAX               ; store number
        mov     4[EBX],EDX              ; ...
        _shl    ECX,1                   ; get sign
        rcr     CX,1                    ; place in top bit
        mov     8[EBX],CX               ; ...
ifdef _BUILDING_MATHLIB
        pop     EBX                     ; restore EBX
        pop     ECX                     ; restore ECX
        ret                             ; return
__iFDLD endp
else
        pop     ECX                     ; restore ECX
        ret                             ; return
__EmuFDLD endp
endif
