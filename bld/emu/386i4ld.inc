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


        xdefp   __I4LD
        xdefp   __U4LD

;       __I4LD - convert long into long double
;       __U4LD - convert unsigned long into long double
; input:
;       EAX - long
;       EDX - pointer to long double

__I4LD  proc    near
        push    EBX                     ; save EBX
        mov     EBX,EDX                 ; save pointer to long double
        or      EAX,EAX                 ; if number is negative
        _if     s                       ; then
          neg   EAX                     ; - negate number
          mov   EDX,0000BFFFh           ; - set exponent
        _else                           ; else
          pop   EBX                     ; - restore EBX
;       convert unsigned 32-bit integer to long double

__U4LD:
          push  EBX                     ; - save EBX
          mov   EBX,EDX                 ; - save pointer to long double
          mov   EDX,00003FFFh           ; - set exponent

        _endif                          ; endif
        push    ECX                     ; save ECX
        or      EAX,EAX                 ; if number is not zero
        _if     ne                      ; then
          bsr   ECX,EAX                 ; - find most significant non-zero bit
          mov   CH,CL                   ; - save shift count
          mov   CL,31                   ; - calculate # of bits to shift by
          sub   CL,CH                   ; - ...
          shl   EAX,CL                  ; - shift bits into position
          mov   CL,CH                   ; - get shift count
          movzx ECX,CH                  ; - get shift count
          add   ECX,EDX                 ; - calculate exponent
          mov   EDX,EAX                 ; - get the bits
        _else                           ; else
          sub   EDX,EDX                 ; - zero
          sub   ECX,ECX                 ; - zero
        _endif                          ; endif
        sub     EAX,EAX                 ; zero the rest of the fraction bits
        mov     [EBX],EAX               ; store number
        mov     4[EBX],EDX              ; ...
        mov     8[EBX],CX               ; ...
        pop     ECX                     ; restore ECX
        pop     EBX                     ; restore EBX
        ret                             ; return
__I4LD  endp

