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
;==     Inputs:         DX;AX   single precision float                  ==
;==     Outputs:        AX;BX;CX;DX     double precision float          ==
;==     Volatile:       none                                            ==
;=========================================================================
include mdef.inc
include struct.inc

        modstart        fsfd086

        xdefp   __FSFD

        defpe   __FSFD
        _guess                  ; guess: number is infinity
          or    ax,ax           ; - quit if low word not zero
          _quif ne              ; - ...
          mov   cx,dx           ; - get high word
          and   ch,7fh          ; - get rid of sign bit
          cmp   cx,7f80h        ; - quit if not infinity
          _quif ne              ; - ...
          mov   ax,dx           ; - get sign bit
          or    ax,7ff0h        ; - set infinity
          sub   bx,bx           ; - ...
          sub   cx,cx           ; - ...
        _admit                  ; guess: number is not 0
          xchg  ax,dx           ; - get exponent into ax
          mov   bx,dx           ; - move rest of mantissa to correct reg.
          mov   dx,2*16*(1023-127);- put exponent bias *2 in dx
          mov   cx,bx           ; - get low part of mantissa
          _shl  ax,1            ; - get rid of sign bit
          rcr   dx,1            ; - and place it in dx with exponent adjustment
          or    cx,ax           ; - check for zero (0.0)
          mov   cx,0            ; - set rest of mantissa to 0
          _quif e               ; - quit if number is 0.0
          shr   ax,1            ; - restore ax without sign
          shr   ax,1            ; - now shift everything three bits
          rcr   bx,1            ; - ...
          rcr   cx,1            ; - ...
          shr   ax,1            ; - ...
          rcr   bx,1            ; - ...
          rcr   cx,1            ; - ...
          shr   ax,1            ; - ...
          rcr   bx,1            ; - ...
          rcr   cx,1            ; - ...
          test  ax,7ff0h        ; - if exponent is 0 (denormal number)
          _if   e               ; - then
            _loop               ; - - loop (normalize the fraction)
              sub  dx,0010h     ; - - - subtract 1 from exponent adjustment
              _shl cx,1         ; - - - shift fraction left
              _rcl bx,1         ; - - - . . .
              _rcl ax,1         ; - - - . . .
              test al,10h       ; - - - check to see if fraction is normalized
            _until ne           ; - - until normalized
          _endif                ; - endif
          add   ax,dx           ; - add in the exponent adjustment
        _endguess               ; endguess
        sub     dx,dx           ; clear bottom bits of mantissa
        ret                     ; and return
        endproc __FSFD

        endmod
        end
