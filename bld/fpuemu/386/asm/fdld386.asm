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
;* Description:  convert double to long double, __iFDLD and __EmuFDLD
;*
;*****************************************************************************

ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart    fdld386, dword

endif

        xrefp   FPInvalidOp

ifdef _BUILDING_MATHLIB
        xdefp   __iFDLD
else
        xdefp   __EmuFDLD
endif

;
;       convert double to long double
;
;ifdef _BUILDING_MATHLIB
;       input:  EAX - pointer to double
;               EDX - pointer to long double to be filled in
;else
;       input:  EDX:EAX - double
;               EBX     - pointer to long double to be filled in
;endif

ifdef _BUILDING_MATHLIB
        defp    __iFDLD

        push    ECX                     ; save ECX
        push    EBX                     ; save EBX
        mov     EBX,EDX                 ; get pointer to long double
        mov     EDX,4[EAX]              ; get double
        mov     EAX,[EAX]               ; ...
else
        defp    __EmuFDLD

        push    ECX                     ; save ECX
endif
        mov     ECX,EDX                 ; get exponent and sign
        shld    EDX,EAX,11              ; shift fraction left 11 bits
        shl     EAX,11                  ; ...
        _guess xx1                      ; guess: a normal number
          sar   ECX,32-12               ; - shift exponent to bottom
          and   CX,07FFh                ; - isolate exponent
          _quif e                       ; - quit if denormal number
          _guess xx2                    ; - guess: normal number
            cmp   CX,07FFh              ; - - quit if infinity or NaN
            _quif e                     ; - - ...
            add   CX,3FFFh-03FFh        ; - - change bias to temp real format
          _admit                        ; - guess: NaN or infinity
            or    CH,07Fh               ; - - set exponent for infinity or NaN
            test  EDX,7FFFFFFFh         ; - - check for infinity
            _if   e                     ; - - if top part is 0
              or    EAX,EAX             ; - - - check low word
              _quif e, xx2              ; - - - quit if infinity
            _endif                      ; - - endif
            call  FPInvalidOp           ; - - indicate "Invalid" exception
            or    EDX,40000000h         ; - - indicate QNaN
          _endguess                     ; - endguess
          or    EDX,80000000h           ; - turn on implied 1 bit
        _admit                          ; admit: denormal number or zero
          or    EDX,EDX                 ; - check for zero
          _if e                         ; - if top part is 0
            or  EAX,EAX                 ; - - check low word
            _quif e, xx1                ; - - quit if zero
          _endif                        ; - endif
          or    CX,3FFFh-3FFh+1         ; - set exponent
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

        endproc __iFDLD
else
        pop     ECX                     ; restore ECX
        ret                             ; return

        endproc __EmuFDLD
endif

ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
