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
        xdefp   __iFSLD
else
        xdefp   __EmuFSLD
endif

;       convert float to long double
; input:
;ifdef _BUILDING_MATHLIB
;       EAX     pointer to float
;       EDX     pointer to long double to be filled in
;else
;       EAX     float
;       EDX     pointer to long double to be filled in
;endif

ifdef _BUILDING_MATHLIB
__iFSLD proc  near
        mov     EAX,[EAX]               ; get float value
else
__EmuFSLD proc  near
endif
        push    ECX                     ; save ECX
        push    EBX                     ; save EBX
        mov     EBX,EDX                 ; save address of long double
        sub     EDX,EDX                 ; zero rest of fraction
        mov     ECX,EAX                 ; get exponent and sign
        sar     ECX,32-9                ; shift to bottom
        and     CX,00FFh                ; isolate exponent
        _if     ne                      ; if not 0
          shl   EAX,8                   ; - shift fraction into place
          _guess                        ; - guess: normal number
            cmp   CL,0FFh               ; - - quit if NaN or infinity
            _quif e                     ; - - ...
            add   CX,3FFFh-007Fh        ; - - change bias to temp real format
          _admit                        ; - guess: NaN
            mov   CH,0FFh               ; - - set exponent to all one's
            and   EAX,7FFFFFFFh         ; - - isolate fraction bits
            _quif e                     ; - - quit if infinity
            or    EAX,40000000h         ; - - indicate NaN
          _endguess                     ; - endguess
          _shl  ECX,1                   ; - get sign
          rcr   CX,1                    ; - place in top bit
          or    EAX,80000000h           ; - turn on implied 1 bit
        _endif                          ; endif
        mov     [EBX],EDX               ; store number
        mov     4[EBX],EAX              ; ...
        mov     8[EBX],CX               ; ...
        pop     EBX                     ; restore EBX
        pop     ECX                     ; restore ECX
        ret                             ; return
ifdef _BUILDING_MATHLIB
__iFSLD endp
else
__EmuFSLD endp
endif
