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


;
include mdef.inc
include struct.inc

        modstart        i4fd386

        xdefp   __I4FD
        xdefp   __U4FD

;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]
;[] __I4FD      convert signed 32-bit integer in EAX into double float
;[] __U4FD      convert unsigned 32-bit integer in EAX into double float
;[]     Input:  EAX         - 32-bit integer
;[]     Output: EDX:EAX     - double precision representation of integer
;[]
;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

        defpe   __I4FD
        or      EAX,EAX         ; if number is negative
        _if     s               ; then
          neg   EAX             ; - negate number
          mov   EDX,00000BFFh   ; - set exponent
        _else                   ; else

;       convert unsigned 32-bit integer to double

        defpe   __U4FD
          mov   EDX,000003FFh   ; - set exponent
        _endif                  ; endif
        or      EAX,EAX         ; if number is not zero
        _if     ne              ; then
          push  ECX             ; - save ECX
          bsr   ECX,EAX         ; - find most significant non-zero bit
          mov   CH,CL           ; - save shift count
          mov   CL,31           ; - calculate # of bits to shift by
          sub   CL,CH           ; - ...
          shl   EAX,CL          ; - shift bits into position
          _shl  EAX,1           ; - one more to get rid of implied 1 bit
          mov   CL,CH           ; - get shift count
          movzx ECX,CH          ; - get shift count
          add   ECX,EDX         ; - calculate exponent
          mov   EDX,EAX         ; - get the bits
          and   EDX,0FFFFF000h  ; - keep 20 bits
          or    EDX,ECX         ; - get exponent in there
          ror   EDX,12          ; - rotate into position
          shl   EAX,20          ; - get last 12 bits into place
          pop   ECX             ; - restore ECX
          ret                   ; - return
        _endif                  ; endif
        sub     EDX,EDX         ; zero EDX
        ret                     ; return

        endproc __U4FD
        endproc __I4FD

        endmod
        end
