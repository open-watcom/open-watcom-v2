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


include mdef.inc
include struct.inc

        modstart        sqrtf,WORD

        xdefp   sqrtf_          ; calc single precision sqrt(fac1)


        defp    sqrtf_
ifdef __386__
        push    EBX             ; save registers
        push    ECX             ; ...
        push    EDX             ; ...
        push    ESI             ; ...
        _guess                  ; guess: non-zero
          add   EAX,EAX         ; - get rid of sign bit
          _quif e               ; - quit if number is 0
          mov   ECX,EAX         ; - get value
          shl   EAX,7           ; - get rid of exponent
          or    EAX,80000000h   ; - turn on implied 1-bit
          shr   ECX,24          ; - get rid of fraction
          sub   CL,7Fh          ; - remove bias
          sar   CL,1            ; - divide by 2
          _if   nc              ; - if exponent is even
            shr   EAX,1         ; - - divide argument by 2
          _endif                ; - endif
          add   CL,7Fh          ; - add bias
          mov   ESI,EAX         ; - get fraction
          stc                   ; - calculate initial estimate
          rcr   ESI,1           ; - ...
          mov   EBX,EAX         ; - save operand
          mov   EDX,EAX         ; - set up for divide
          _loop                 ; - loop (until estimate is correct)
            sub   EAX,EAX       ; - - zero low word
            div   ESI           ; - - calculate newer estimate
            dec   ESI           ; - - want estimate to be within one
            cmp   ESI,EAX       ; - - ...
            _quif na            ; - - quit if estimate good enough
            inc   ESI           ; - -
            add   ESI,EAX       ; - - calc. new estimate as (old+new)/2
            rcr   ESI,1         ; - - ...
            mov   EDX,EBX       ; - - restore operand
          _endloop              ; - endloop
          shl   EAX,1           ; - get rid of implied one bit
          and   AH,0FEh         ; - isolate fraction bits
          mov   AL,CL           ; - get exponent
          ror   EAX,9           ; - rotate into place
        _endguess               ; endguess
        pop     ESI             ; restore registers
        pop     EDX             ; ...
        pop     ECX             ; ...
        pop     EBX             ; ...
else
        push    BX              ; save registers
        push    CX              ; ...
        push    SI              ; ...
        push    DI              ; ...
        _guess                  ; guess: non-zero
          mov   CX,DX           ; - get high word
          and   CH,7Fh          ; - remove sign bit
          or    CX,AX           ; - or in low order word
          _quif e               ; - quit if number is 0
          mov   CX,DX           ; - get exponent
          shl   CX,1            ; - ...
          mov   CL,CH           ; - get exponent
          mov   DH,DL           ; - shift fraction up
          mov   DL,AH           ; - ...
          mov   AH,AL           ; - ...
          xor   AL,AL           ; - ...
          or    DH,80h          ; - turn on implied 1-bit
          sub   CL,7Fh          ; - remove bias
          sar   CL,1            ; - divide by 2
          _if   nc              ; - if exponent is even
            shr   DX,1          ; - - divide argument by 2
            rcr   AX,1          ; - - ...
          _endif                ; - endif
          add   CL,7Fh          ; - add bias
          mov   BX,AX           ; - save value
          mov   DI,DX           ; - ...
          mov   SI,DX           ; - get high order word of fraction
          stc                   ; - calculate initial estimate
          rcr   SI,1            ; - ...
          inc   DX              ; - check for DX=FFFFh
          _if   e               ; - if high word was -1
            dec   DX            ; - - restore DX
            stc                 ; - - calc. second word of estimate
            rcr   AX,1          ; - - ...
          _else                 ; - else
            dec   DX            ; - - restore DX
            _loop               ; - - loop
              div   SI          ; - - - calculate newer estimate
              dec   SI          ; - - - want estimate to be within one
              cmp   SI,AX       ; - - - ...
              _quif na          ; - - - quit if estimate good enough
              inc   SI          ; - - -
              add   SI,AX       ; - - - calc. new estimate as (old+new)/2
              rcr   SI,1        ; - - - ...
              mov   DX,DI       ; - - - restore operand
              mov   AX,BX       ; - - - ...
            _endloop            ; - - endloop
            inc   SI            ; - - restore divisor
            push  AX            ; - - save word of quotient
            sub   AX,AX         ; - - zero low word
            div   SI            ; - - calc. next word of quotient
            pop   DX            ; - - restore high word of quotient
;
;       SI:0   estimate is too small
;       DX:AX estimate is too large
;       calculate new estimate as (SI:0+DX:AX)/2
;
            add   DX,SI         ; - - ...
            stc                 ; - - divide by 2
            rcr   DX,1          ; - - ...
            rcr   AX,1          ; - - ...
          _endif                ; - endif
          add   AX,AX           ; - get rid of implied one bit
          adc   DX,DX           ; - ...
          mov   AL,AH           ; - right shift by 8
          mov   AH,DL           ; - ...
          mov   DL,DH           ; - ...
          mov   DH,CL           ; - get exponent
          shr   DX,1            ; - set sign to +ve
          rcr   AX,1            ; - ...
        _endguess               ; endguess
        pop     DI              ; restore registers
        pop     SI              ; ...
        pop     CX              ; ...
        pop     BX              ; ...
endif
        ret                     ; return
        endproc sqrtf_

        endmod
        end
