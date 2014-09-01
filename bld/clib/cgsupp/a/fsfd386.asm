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


;=========================================================================
;==     Name:           FSFD                                            ==
;==     Operation:      Float single to float double conversion         ==
;==     Inputs:         EAX     single precision float                  ==
;==     Outputs:        EDX:EAX         double precision float          ==
;==     Volatile:       none                                            ==
;=========================================================================
include mdef.inc
include struct.inc

        modstart        fsfd386

        xdefp   __FSFD

        defpe   __FSFD
        _guess                          ; guess: number is 0.0 or -0.0
          mov   EDX,EAX                 ; - get float
          and   EAX,7fffffffh           ; - remove sign
          _quif ne                      ; - quit if number is not +0.0 or -0.0
        _admit                          ; admit: number is +-infinity
          cmp   EAX,7f800000h           ; - quit if not +-infinity
          _quif ne                      ; - ...
          or    EDX,7ff00000h           ; - set double precision infinity
          sub   EAX,EAX                 ; - ...
        _admit                          ; admit: not a special number
          push  ECX                     ; - save register ECX
          mov   ECX,EDX                 ; - set result sign to ECX
          xor   ECX,EAX                 ; - ...
          mov   EDX,EAX                 ; - get value to EDX:EAX for shift
          sub   EAX,EAX                 ; - ...
          shrd  EAX,EDX,3               ; - shift EDX:EAX right 3 times
          shr   EDX,3                   ; - ...
          cmp   EDX,0ffh shl 20         ; - if exponent is NaN
          _if ae                        ; - then
            or    ECX,(7FFh - 0ffh) shl 20; - - set result exponent adjustment for NaN
          _else                         ; - else 
            or    ECX,(3ffh - 7fh) shl 20; - - set result exponent adjustment
            test  EDX,0ffh shl 20       ; - - if exponent is 0 (denormal number)
            _if e                       ; - - then
              add  ECX,1 shl 20         ; - - - add 1 to result exponent adjustment
              _loop                     ; - - - loop (normalize the fraction)
                sub  ECX,1 shl 20       ; - - - - subtract 1 from result exponent adjustment
                _shl EAX,1              ; - - - - shift fraction left
                _rcl EDX,1              ; - - - - ...
                test EDX,1 shl 20       ; - - - - check to see if fraction is normalized
              _until ne                 ; - - - until normalized
              and EDX,not (1 shl 20)    ; - - - reset implied bit after normalization
            _endif                      ; - - endif
          _endif                        ; - endif
          add   EDX,ECX                 ; - adjust result exponent and set result sign
          pop   ECX                     ; - restore register ECX
        _endguess                       ; endguess
        ret                             ; return
        endproc __FSFD

        endmod
        end
