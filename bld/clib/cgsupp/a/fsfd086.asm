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
        _guess  xx1             ; guess: number is 0.0 or infinity
          mov   bx,ax           ; - move rest of mantissa to correct reg.
          mov   ax,dx           ; - get high word to ax
          sub   cx,cx           ; - ...
          or    bx,bx           ; -
          _quif ne              ; - quit if not 0.0 or infinity (low word not zero)
          and   dx,7fffh        ; - get rid of sign bit
          _quif e,xx1           ; - quit if 0.0
          cmp   dx,7f80h        ; - check if infinity
          _quif ne              ; - quit if not infinity
          or    al,0f0h         ; - set infinity ax = 7ff0h or 0fff0h
        _admit                  ; guess: number is not 0
          and   dh,7fh
          cmp   dx,7f80h
          _if   ae
            mov   dx,( 7ffh - 0ffh ) shl 5 ;- put NaN exponent - ax in dx (ax = 0ffh)
          _else
            mov   dx,( 3ffh - 7fh ) shl 5  ;- put exponent bias in dx
          _endif
          shl   ax,1
          rcr   dx,1
          shr   ax,1
          shr   ax,1            ; - now shift everything three bits
          rcr   bx,1            ; - ...
          rcr   cx,1            ; - ...
          shr   ax,1            ; - ...
          rcr   bx,1            ; - ...
          rcr   cx,1            ; - ...
          shr   ax,1            ; - ...
          rcr   bx,1            ; - ...
          rcr   cx,1            ; - ...
          test  ax,0ff0h        ; - if exponent is 0 (denormal number)
          _if   e               ; - then
            _loop               ; - - loop (normalize the fraction)
              sub  dx,0010h     ; - - - subtract 1 from exponent adjustment
              and  al,0fh       ; - - - clean msb after shift
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
