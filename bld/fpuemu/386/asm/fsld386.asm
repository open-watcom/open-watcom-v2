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
;* Description:  convert float to long double  __iFSLD and __EmuFSLD
;*
;*****************************************************************************


ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart    fsld386, dword

endif

        xrefp   FPInvalidOp

ifdef _BUILDING_MATHLIB
        xdefp   __iFSLD
else
        xdefp   __EmuFSLD
endif

;
;       convert float to long double
;
;ifdef _BUILDING_MATHLIB
;       input:  EAX     pointer to float
;               EDX     pointer to long double to be filled in
;else
;       input:  EAX     float
;               EDX     pointer to long double to be filled in
;endif

ifdef _BUILDING_MATHLIB

        defp    __iFSLD

        mov     EAX,[EAX]               ; get float value
else

        defp    __EmuFSLD
endif
        push    ECX                     ; save ECX
        mov     ECX,EAX                 ; get exponent and sign
        shl     EAX,8                   ; shift fraction into place
        _guess                          ; guess: normal number
          sar     ECX,32-9              ; - shift to bottom
          sub     CH,CH                 ; - isolate exponent
          or      CL,CL                 ; - isolate exponent
          _quif e                       ; - quit if denormal number
          _guess                        ; - guess: normal number
            cmp   CL,0FFh               ; - - quit if NaN or infinity
            _quif e                     ; - - ...
            add   CX,3FFFh-7Fh          ; - - change bias to temp real format
          _admit                        ; - guess: NaN or infinity
            or    CH,07Fh               ; - - set exponent to all one's
            test  EAX,7FFFFFFFh         ; - - isolate fraction bits
            _quif e                     ; - - quit if infinity
            call  FPInvalidOp           ; - - indicate "Invalid" exception
            or    EAX,40000000h         ; - - indicate QNaN
          _endguess                     ; - endguess
          or    EAX,80000000h           ; - turn on implied 1 bit
        _admit                          ; admit: denormal number or zero
          or    EAX,EAX                 ; - ...
          _quif e                       ; - quit if zero
          or    CX,3FFFh-7Fh+1          ; - set exponent
          _loop                         ; - loop (normalize number)
            or    EAX,EAX               ; - - quit if top bit is on
            _quif s                     ; - - ...
            _shl  EAX,1                 ; - - shift number left 1 bit
            dec   CX                    ; - - decrement exponent
          _endloop                      ; - endloop
        _endguess                       ; endguess
        mov     4[EDX],EAX              ; ...
        sub     EAX,EAX
        mov     [EDX],EAX               ; store number
        _shl    ECX,1                   ; get sign
        rcr     CX,1                    ; place in top bit
        mov     8[EDX],CX               ; ...
        pop     ECX                     ; restore ECX
        ret                             ; return

ifdef _BUILDING_MATHLIB
        endproc __iFSLD
else
        endproc __EmuFSLD
endif

ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
