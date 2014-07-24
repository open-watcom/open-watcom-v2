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


; This module converts a string to long_double
;       void __ZBuf2LD( char_arg buf, ldbl_arg value );
;
include mdef.inc
include struct.inc

        modstart bufld386

        xdefp   __ZBuf2LD

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>                                                                   <>
;<>   __ZBuf2LD - convert buffer of significant digits into floating  <>
;<>   void __ZBuf2LD( char_arg buf, ldbl_arg value )                  <>
;<>                                                                   <>
;<>   input:  SS:EAX - address of buffer of significant digits        <>
;<>           SS:EDX - place to store value                           <>
;<>   output: SS:[EDX]        - floating-point number                 <>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        defpe   __ZBuf2LD
        push    EBP             ; save EBP
        push    ESI             ; save ESI
        push    EDI             ; save EDI
        push    ECX             ; save ECX
        push    EBX             ; save EBX
        push    EDX             ; save pointer to result
        mov     EBP,EAX         ; get address of buffer
        sub     EDX,EDX         ; set 96-bit integer to 0
        sub     ECX,ECX         ; ...
        sub     ESI,ESI         ; ...
        sub     EAX,EAX         ; zero out EAX
        _loop                   ; loop (convert digits into 64-bit int)
          mov   AL,[EBP]        ; - get next digit
          cmp   AL,0            ; - quit if at end of buffer
          _quif e               ; - . . .

;[]  multiply current value in EDX:ECX:ESI by 10

          mov   EDI,EDX         ; - save current value
          mov   EBX,ECX         ; - ...
          mov   EAX,ESI         ; - ...
          _shl  ESI,1           ; - multiply number by 4
          _rcl  ECX,1           ; -   by shifting left 2 places
          _rcl  EDX,1           ; - ...
          _shl  ESI,1           ; - ...
          _rcl  ECX,1           ; - ...
          _rcl  EDX,1           ; - ...
          add   ESI,EAX         ; - add original value
          adc   ECX,EBX         ; -  (this will make it times 5)
          adc   EDX,EDI         ; - ...
          _shl  ESI,1           ; - shift left to make it times 10
          _rcl  ECX,1           ; - ...
          _rcl  EDX,1           ; - ...
          sub   EAX,EAX         ; - zero eax
          mov   AL,[EBP]        ; - get next digit
          and   AL,0Fh          ; - isolate binary digit
          add   ESI,EAX         ; - add in current digit
          adc   ECX,0           ; - ...
          adc   EDX,0           ; - ...
          inc   EBP             ; - point to next digit in buffer
        _endloop                ; endloop
        mov     EAX,ECX         ; get low order word into EAX

;[] Turn the integer in EDX:EAX:ESI into a real number

        mov     EDI,3FFFh+63+32 ; set exponent
        call    Norm            ; convert the 64 bit integer to a float
        pop     EBP             ; restore pointer to result
        mov     0[EBP],EAX      ; store result
        mov     4[EBP],EDX      ; ...
        mov     8[EBP],SI       ; ...
        pop     EBX             ; restore EBX
        pop     ECX             ; restore ECX
        pop     EDI             ; restore EDI
        pop     ESI             ; restore ESI
        pop     EBP             ; restore EBP
        ret                     ; return to caller
        endproc __ZBuf2LD


;[] Norm normalizes an unsigned real in EDX:EAX:ESI i.e grab top 64 bits
;[] expects the exponent to be in EDI.
;[]     SI contains the new exponent

Norm    proc    near            ; normalize floating point number
        mov     EBP,EAX         ; see if the integer is zero
        or      EBP,EDX         ; ...
        or      EBP,ESI         ; ...
        _if     ne              ; if number is non-zero
          or    EDX,EDX         ; - see if high word is 0
          _if   e               ; - if high word is 0
            mov   EDX,EAX       ; - - shift by 32-bits
            mov   EAX,ESI       ; - - ...
            sub   ESI,ESI       ; - - ...
            sub   EDI,32        ; - - adjust exponent by 32
          _endif                ; - endif
          or    EDX,EDX         ; - see if high word is 0
          _if   e               ; - if high word is 0
            mov   EDX,EAX       ; - - shift by 32-bits
            mov   EAX,ESI       ; - - ...
            sub   ESI,ESI       ; - - ...
            sub   EDI,32        ; - - adjust exponent by 32
          _endif                ; - endif
          _loop                 ; - loop
            or    EDX,EDX       ; - - quit if high bit is on
            _quif s             ; - - ...
            dec   EDI           ; - - decrement exponent
            _shl  ESI,1         ; - - shift integer left by 1 bit
            _rcl  EAX,1         ; - - ...
            _rcl  EDX,1         ; - - ...
          _endloop              ; - endloop
          _shl  ESI,1           ; - get top bit of extra word
          adc   EAX,0           ; - round up
          adc   EDX,0           ; - ...
          _if   c               ; - if carry out the top
            rcr   EDX,1         ; - - shift fraction back 1
            inc   EDI           ; - - increment exponent
          _endif                ; - endif
          mov   ESI,EDI         ; - get exponent
        _endif                  ; endif
        ret                     ; return
        endproc Norm

        endmod
        end
