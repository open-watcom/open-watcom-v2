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
        _guess                   ; guess: number is 0.0 or -0.0
          mov   EDX,EAX          ; - get float
          and   EAX,7fffffffh    ; - remove sign
          _quif ne               ; - quit if number is not +0.0 or -0.0
        _admit                   ; guess: number is +-infinity
          cmp   EAX,7f800000h    ; - quit if not +-infinity
          _quif ne               ; - ...
          or    EDX,7ff00000h    ; - set double precision infinity
          sub   EAX,EAX          ; - ...
        _admit                   ; admit: not a special number
          test  EAX,7f800000h    ; - if exponent is 0 (denormal number)
          _if   e                ; - then
            or    EDX,7F800000h  ; - - set exponent to 0xFF
            _loop                ; - - loop (normalize the fraction)
              sub  EDX,00800000h ; - - - subtract 1 from exponent adjustment
              _shl EAX,1         ; - - - shift fraction left
              test EAX,00800000h ; - - - check to see if fraction is normalized
            _until ne            ; - - until normalized
            and   EDX,0FF800000h ; - - copy fraction back to EDX
            and   EAX,007FFFFFh  ; - - ...
            or    EDX,EAX        ; - - ...
            sar   EDX,3          ; - shift over 3
            and   EDX,8FFFFFFFh  ; - reset exponent extended bits
            add   EDX,28200000h  ; - adjust exponent by (3FF - FF -7F + 1) shl 20   
          _else                  ; - else 
            sar   EDX,3          ; - shift over 3
            and   EDX,8FFFFFFFh  ; - reset exponent extended bits
            cmp   EAX,7F800000h  ; - if number is not number (NaN)
            _if   a              ;
              or    EDX,7FF00000h; - adjust exponent to NaN 7FF shl 20
            _else
              add   EDX,38000000h; - adjust exponent by (3FF-7F) shl 20
            _endif
          _endif                ; - endif
          and   EAX,7           ; - get bottom 3 bits of fraction
          ror   EAX,3           ; - shift them to the top
        _endguess               ; endguess
        ret                     ; return
        endproc __FSFD

        endmod
        end
