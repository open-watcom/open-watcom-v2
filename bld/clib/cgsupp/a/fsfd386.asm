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
        _guess                  ; guess: number is 0.0 or -0.0
          mov   EDX,EAX         ; - get float
          and   EDX,7fffffffh   ; - remove sign
          _quif ne              ; - quit if number is not +0.0 or -0.0
        _admit                  ; guess: number is +-infinity
          cmp   EDX,7f800000h   ; - quit if not +-infinity
          _quif ne              ; - ...
          mov   EDX,EAX         ; - get sign
          or    EDX,7ff00000h   ; - set double precision infinity
          sub   EAX,EAX         ; - ...
        _admit                  ; admit: not a special number
          test  EDX,7f800000h   ; - if exponent is 0 (denormal number)
          _if   e               ; - then
            xor   EAX,EDX       ; - - keep just the sign
            push  ECX           ; - - save ECX
            sub   ECX,ECX       ; - - exponent adjustment
            _loop               ; - - loop (normalize the fraction)
              add  ECX,00800000h; - - - subtract 1 from exponent adjustment
              _shl EDX,1        ; - - - shift fraction left
              test EDX,00800000h; - - - check to see if fraction is normalized
            _until ne           ; - - until normalized
            or    EAX,EDX       ; - - copy fraction back to EAX
            sub   EDX,ECX       ; - - adjust the exponent
            pop   ECX           ; - - restore ECX
          _endif                ; - endif
          sar   EDX,2           ; - shift over 2
          _shl  EAX,1           ; - get sign
          rcr   EDX,1           ; - shift into result
          and   EAX,0eh         ; - get bottom 3 bits of fraction
          ror   EAX,4           ; - shift them to the top
          add   EDX,(1023-127) shl 20   ; - adjust exponent
        _endguess               ; endguess
        ret                     ; return
        endproc __FSFD

        endmod
        end
