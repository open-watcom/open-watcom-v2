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
;* Description:  Floating-point modulo and string to double conversion (386).
;*
;*****************************************************************************


;       double modf( double value, double *iptr );
;
; Description:
;   The modf function breaks the argument value into integral and
;   fractional parts, each of which has the same sign as the argument.
;   It stores the integral part as a double in the object pointed to
;   by iptr.
;
; Returns:
;   The modf function returns the signed fractional part of value.
;
;  18-oct-86    ...             Fraction in Modf not computed right
;                               significant digits
;
include mdef.inc
include struct.inc

        modstart        amodf386

        xdefp   __ModF
        xdefp   __ZBuf2F

;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]
;[] __ModF
;[]
;[]     void __ModF( double near *AX, double near *DX );
;[]     Input:  EAX         - pointer to double precision float
;[]             EDX         - place to store integral part
;[]     Output: [EAX]       - fractional part of value.
;[]             [EDX]       - integral part of value
;[]
;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

        defpe   __ModF
        push    EBP             ; save BP
        push    ESI             ; save SI
        push    EDI             ; save DI
        push    ECX             ; save CX
        push    EBX             ; save BX
        mov     ESI,EDX         ; get address for ipart
        mov     EBP,EAX         ; get address of float
        mov     EAX,0[EBP]      ; get float
        mov     EDX,4[EBP]      ; . . .
        xchg    ESI,EBP         ; flip pointers
        mov     0[EBP],EAX      ; store integral part of value
        mov     4[EBP],EDX      ; . . .
        _guess                  ; guess: fraction is zero
          mov   ECX,EDX         ; - get exponent
          and   ECX,7FF00000h   ; - get exponent part of R into DI
          je    short done      ; - set integer part to 0 if exponent = 0
          cmp ECX,(3FFh+52) shl 20; - check for exponent > 52
          _quif b               ; - quit if fraction not zero
          xchg  EBP,ESI         ; - get address of fractional part
done:     sub   EAX,EAX         ; - set fraction(or integer) to 0
          mov   4[EBP],EAX      ; - . . .
          mov   0[EBP],EAX      ; - . . .
          pop   EBX             ; - restore BX
          pop   ECX             ; - restore CX
          pop   EDI             ; - restore DI
          pop   ESI             ; - restore SI
          pop   EBP             ; - restore BP
          ret                   ; - return
        _endguess               ; endguess
        mov     EDI,ECX         ; save exponent
        shr     ECX,20          ; move exponent to bottom
        sub     CX,03FFh        ; remove bias
        jb      done            ; quit if number < 1.0
        push    EDX             ; save sign
        sub     EAX,EAX         ; initialize mask to 0
        mov     EDX,0FFF00000h  ; ...
        cmp     CL,20           ; if exponent > 20
        _if     a               ; then
          sar   EDX,21          ; - set ESI to all ones
          rcr   EAX,1           ; - set high bit of EBX
          sub   CL,21           ; - get shift count for second part
          sar   EAX,CL          ; - create rest of mask
        _else                   ; else
          sar   EDX,CL          ; - create mask in high part only
        _endif                  ; endif
        and     4[EBP],EDX      ; mask off the remaining fraction bits
        and     0[EBP],EAX      ; . . .
        not     EDX             ; complement the mask to get fractional part
        not     EAX             ; . . .
        mov     EBP,ESI         ; get address of fractional part
        and     EDX,4[EBP]      ; get fraction bits
        and     EAX,0[EBP]      ; . . .
        call    Norm            ; normalize the fraction
        pop     ESI             ; restore sign
        or      EDX,EDX         ; if fraction is not 0
        _if     ne              ; then
          and   ESI,80000000h   ; - isolate sign
          or    EDX,ESI         ; - set sign in fractional part
        _endif                  ; endif
        mov     4[EBP],EDX      ; store fractional part
        mov     0[EBP],EAX      ; . . .
        pop     EBX             ; restore BX
        pop     ECX             ; restore CX
        pop     EDI             ; restore DI
        pop     ESI             ; restore SI
        pop     EBP             ; restore BP
        ret                     ; return
        endproc __ModF


;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>                                                                   <>
;<>   __ZBuf2F - convert buffer of significant digits into floating   <>
;<>             void __ZBuf2F( char near *buf, double near *value )   <>
;<>                                                                   <>
;<>   input:  EAX - address of buffer of significant digits           <>
;<>           EDX - place to store value                              <>
;<>   output: [EDX]        - floating point number                    <>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        defpe   __ZBuf2F
        push    EBP             ; save BP
        push    ESI             ; save SI
        push    EDI             ; save DI
        push    ECX             ; save CX
        push    EBX             ; save BX
        push    EDX             ; save pointer to result
        mov     ESI,EAX         ; get address of buffer
        sub     EDX,EDX         ; set 54-bit integer to 0
        sub     ECX,ECX         ; . . .
        sub     EAX,EAX         ; zero out EAX
        _loop                   ; loop (convert digits into 54-bit int)
          mov   AL,[ESI]        ; - get next digit
          cmp   AL,0            ; - quit if at end of buffer
          _quif e               ; - . . .

;[]  multiply current value in EDX:ECX by 10

          mov   EDI,EDX         ; - save current value
          mov   EBX,ECX         ; - ...
          _shl  ECX,1           ; - multiply number by 4
          _rcl  EDX,1           ; -   by shifting left 2 places
          _shl  ECX,1           ; - . . .
          _rcl  EDX,1           ; - . . .
          add   ECX,EBX         ; - add original value
          adc   EDX,EDI         ; -  (this will make it times 5)
          _shl  ECX,1           ; - shift left to make it times 10
          _rcl  EDX,1           ; - . . .
          and   AL,0Fh          ; - isolate binary digit
          add   ECX,EAX         ; - add in current digit
          adc   EDX,0           ; - . . .
          inc   ESI             ; - point to next digit in buffer
        _endloop                ; endloop
        mov     EAX,ECX         ; get low order word into EAX

;[] Turn the integer in EDX:EAX into a real number

        mov     EDI,(3FFh+52) shl 20; set exponent
        call    Norm            ; convert the 52 bit integer to a float
        pop     EBP             ; restore pointer to result
        mov     4[EBP],EDX      ; store result
        mov     0[EBP],EAX      ; . . .
        pop     EBX             ; restore BX
        pop     ECX             ; restore CX
        pop     EDI             ; restore DI
        pop     ESI             ; restore SI
        pop     EBP             ; restore BP
        ret                     ; return to caller
        endproc __ZBuf2F


;[] Norm normalizes an unsigned real in EDX:EAX
;[] expects the exponent to be in EDI.  The real returned is in 'packed'
;[] format
;[]     ESI is destroyed

Norm    proc    near            ; normalize floating point number
        sub     ESI,ESI         ; clear out SI
        or      ESI,EAX         ; see if the integer is zero
        or      ESI,EDX         ; . . .
        je      short Z_52ret   ; if integer is zero, return to caller
        test    EDX,0FFF00000h  ; see if we have to shift forward or backward
        _if     e               ; if (we haven't shifted msb into bit 53)
          _loop                 ; - loop
            sub   EDI,00100000h ; - - exp <-- exp - 1
            _shl  EAX,1         ; - - shift integer left by 1 bit
            _rcl  EDX,1         ; - - . . .
            test  EDX,0FFF00000h;
          _until ne             ; - until( msb is shifted into bit 53 )
        _else                   ; else (we must shift to the right)
          test  EDX,0FFE00000h  ; -
          je    short done1     ; - if msb is bit 53, we are done
          _loop                 ; - loop
            add   EDI,00100000h ; - - exp <-- exp + 1
            shr   EDX,1         ; - - shift integer right by 1 bit
            rcr   EAX,1         ; - - . . .
            rcr   ESI,1         ; - - save lsb
            test  EDX,0FFE00000h; -
          _until e              ; - until( msb is bit 53 )
          _rcl  ESI,1           ; - get lsb
          adc   EAX,0           ; - and use it to round off the number
          adc   EDX,0           ; - . . .
        _endif                  ; endif
done1:  and     EDX,000FFFFFh   ; clear out implied bit
        or      EDX,EDI         ; put in exponent
Z_52ret:ret                     ; return
        endproc Norm

        endmod
        end
