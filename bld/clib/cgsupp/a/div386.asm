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

.8087
        modstart        div386

        xrefp           __8087  ; indicate that NDP instructions are present

        datasegment
        extrn   __real87 : byte         ; 8087.asm
        extrn   __chipbug: byte
        enddata

        xrefp   F8DivZero       ; Fstatus
        xrefp   F8OverFlow      ; Fstatus
        xrefp   F8UnderFlow     ; Fstatus
        xrefp   __fdiv_m64

        xdefp   __FDD

;  double __FDD( double , double )
sign    equ     -12
den     equ     sign-8
quot    equ     den-12
lo      equ     0
hi      equ     4

        defpe   __FDD
        or      EBX,EBX                 ; if low word of divisor is 0
        _if     e                       ; then
          _shl  ECX,1                   ; - shift sign of divisor into carry
          _if   e                       ; - if divisor is zero
            jmp   F8DivZero             ; - - handle divide by zero
          _endif                        ; - endif
          rcr   ECX,1                   ; - restore sign of divisor
        _endif                          ; endif
        or      EAX,EAX                 ; check dividend for zero
        _if     e                       ; if so then
          _shl  EDX,1                   ; - save sign of dividend
          _if   e                       ; - if dividend is 0
            ret                         ; - - return
          _endif                        ; - endif
          rcr   EDX,1                   ; - restore sign of dividend
        _endif                          ; endif

        cmp     byte ptr __real87,0     ; if no 80x87 is present
        je      short __FDDemu          ; then emulate

__FDD87:
        push    EDX                     ; push operand 1
        push    EAX                     ; . . .
        fld     qword ptr [ESP]         ; load operand 1
        push    ECX                     ; push operand 2
        push    EBX                     ; . . .
        test    byte ptr __chipbug,1    ; have we got a bad divider?
        _if     ne                      ; then
          call  __fdiv_m64              ; - call support rtn for divide
        _else                           ; else
          fdiv  qword ptr [ESP]         ; - divide operand 1 by operand 2
          add   ESP,8                   ; - clean up stack
        _endif                          ; endif
        fstp    qword ptr [ESP]         ; store result
        fwait                           ; wait
        pop     EAX                     ; load result into DX:AX
        pop     EDX                     ; . . .
        cmp     EDX,80000000H           ; is it a negative zero?
        _if     e                       ; if it is then
          sub   EDX,EDX                 ; - make it positive 0.0
          mov   EAX,EDX                 ; - ...
        _endif                          ; endif
        ret                             ; return

__FDDemu:
        push    EBP                     ; save EBP
        mov     EBP,ESP                 ; get access to stack
        push    EDI                     ; save EDI
        push    ESI                     ; and ESI

        mov     EDI,EDX                 ; get high part of op1
        mov     ESI,ECX                 ; get high part of op2
        sar     EDI,20          ; shift exponent to bottom, duplicating sign
        sar     ECX,20          ; shift exponent to bottom, duplicating sign
        and     EDI,0800007FFh          ; isolate signs and exponent
        and     ECX,0800007FFh          ; ...
        rol     EDI,16                  ; rotate signs to bottom
        rol     ECX,16                  ; ...
        add     DI,CX                   ; calc sign of result
        rol     EDI,16                  ; rotate signs to top
        rol     ECX,16                  ; ...
        and     EDX,000FFFFFh           ; isolate fraction
        and     ESI,000FFFFFh           ; isolate fraction
        or      DI,DI                   ; if op1 is not a denormal
        _if     ne                      ; then
          or    EDX,00100000h           ; - turn on implied 1 bit
        _else                           ; else (denormal)
          _loop                         ; - loop (normalize it)
            _shl  EAX,1                 ; - - shift fraction left
            _rcl  EDX,1                 ; - - . . .
            dec   DI                    ; - - decrement exponent
            test  EDX,00100000h         ; - - until implied 1 bit is on
          _until  ne                    ; - until implied 1 bit is on
        _endif                          ; endif
        or      CX,CX                   ; if op2 is not a denormal
        _if     ne                      ; then
          or    ESI,00100000h           ; - turn on implied 1 bit
        _else                           ; else (denormal)
          _loop                         ; - loop (normalize it)
            _shl  EBX,1                 ; - - shift fraction left
            _rcl  ESI,1                 ; - - . . .
            dec   CX                    ; - - decrement exponent
            test  ESI,00100000h         ; - - until implied 1 bit is on
          _until  ne                    ; - until implied 1 bit is on
        _endif                          ; endif

        sub     DI,CX                   ; calculate exponent of result
        add     DI,03ffh                ; add in removed bias
        _guess                          ; guess: overflow
          _quif s                       ; - quit if exponent is negative
          cmp   DI,07FFh                ; - quit if not overflow
          _quif b                       ; - . . .
          mov   EAX,ECX                 ; - put sign into EAX
          pop   ESI                     ; - restore ESI
          pop   EDI                     ; - restore EDI
          pop   EBP                     ; - restore EBP
          jmp   F8OverFlow              ; - handle overflow
        _endguess                       ; endguess
        cmp     DI,-52                  ; if exponent is too small
        _if     l                       ; then underflow
          pop   ESI                     ; - restore ESI
          pop   EDI                     ; - restore EDI
          pop   EBP                     ; - restore EBP
          jmp   F8UnderFlow             ; - handle underflow
        _endif                          ; endif

        push    EDI                     ; save sign and exponent
        mov     CL,11                   ; shift fractions to top of registers
        shld    EDX,EAX,CL              ; ...
        shld    EAX,EBP,CL              ; ...
        and     EAX,0FFFFF800h          ; ...
        shld    ESI,EBX,CL              ; ...
        shld    EBX,EBP,CL              ; ...
        and     EBX,0FFFFF800h          ; ...
        push    ESI                     ; save divisor
        push    EBX                     ; ...
        mov     ECX,ESI                 ; get top part of divisor
        mov     EDI,EDX                 ; get dividend
        mov     ESI,EAX                 ; ...
        sub     EAX,EAX                 ; assume top bit is 0
        cmp     ECX,EDX                 ; check high parts
        _if     be                      ; if hi(divisor) <= hi(dividend)
          sub   EDX,ECX                 ; - subtract divisor from dividend
          inc   EAX                     ; - set quotient to 1
        _endif                          ; endif
        push    EAX                     ; save high word of quotient
        mov     EAX,ESI                 ; get low part of dividend
        div     ECX                     ; estimate next word of quotient
        push    EAX                     ; save estimate of quotient (quot+4[EBP])
        xchg    EAX,EBX                 ; save quot., get low word of divisor
        mul     EBX                     ; calc. estimate * lo(divisor)
        xchg    EAX,ECX                 ; ...
        xchg    EBX,EDX                 ; save partial product
        mul     EDX                     ; calc. estimate * hi(divisor)
        add     EAX,EBX                 ; add to partial product
        adc     EDX,0                   ; ...
        mov     EBX,den+lo[EBP]         ; get low word of divisor
        test    byte ptr quot+8[EBP],1  ; if high word of quot is 1
        _if     ne                      ; then
          add   EAX,EBX                 ; - add in divisor
          adc   EDX,den+hi[EBP]         ; - ...
        _endif                          ; endif
; subtract estimate * divisor from dividend
        neg     ECX                     ; 0 - ECX
        sbb     ESI,EAX                 ; ...
        sbb     EDI,EDX                 ; ...
        _if     ne                      ; if high word not 0 (quotient too big)
          _loop                         ; - loop (find correct quotient)
            sub   dword ptr quot+4[EBP],1; - - decrement quotient
            sbb   dword ptr quot+8[EBP],0; - - ...
            add   ECX,EBX               ; - - add divisor back to dividend
            adc   ESI,den+hi[EBP]       ; - - ...
            adc   EDI,0                 ; - - ...
          _until  e                     ; - until done
        _endif                          ; endif
        mov     EDI,ESI                 ; get new dividend
        mov     ESI,ECX                 ; ...
        mov     ECX,den+hi[EBP]         ; get divisor
        cmp     ECX,EDI                 ; check high parts      13-aug-90
        _if     be                      ; if hi(divisor) <= hi(dividend)
          sub   EDI,ECX                 ; - subtract divisor from dividend
          add   dword ptr quot+4[EBP],1 ; - increment quotient
          adc   dword ptr quot+8[EBP],0 ; - ...
        _endif                          ; endif
        mov     EDX,EDI                 ; get dividend into EDX:EAX
        mov     EAX,ESI                 ; ...
        div     ECX                     ; estimate next word of quotient
        push    EAX                     ; save estimate of quotient (-4[EBP])
        or      EAX,EAX                 ; if quotient non-zero
        _if     ne                      ; then
          xchg  EAX,EBX                 ; - save quot., get low word of divisor
          mul   EBX                     ; - calc. estimate * lo(divisor)
          xchg  EAX,ECX                 ; - ...
          xchg  EBX,EDX                 ; - save partial product
          mul   EDX                     ; - calc. estimate * hi(divisor)
          add   EAX,EBX                 ; - add to partial product
          adc   EDX,0                   ; - ...
;   - subtract estimate * divisor from dividend
          neg   ECX                     ; - 0 - ECX
          sbb   ESI,EAX                 ; - ...
          sbb   EDI,EDX                 ; - ...
          _if   ne                      ; - if high word not 0 (quotient too big)
            _loop                       ; - - loop (find correct quotient)
              sub   dword ptr quot+0[EBP],1; - - - decrement quotient
              sbb   dword ptr quot+4[EBP],0; - - - ...
              sbb   dword ptr quot+8[EBP],0; - - - ...
              add   ECX,den+lo[EBP]     ; - - - add divisor back to dividend
              adc   ESI,den+hi[EBP]     ; - - - ...
              adc   EDI,0               ; - - - ...
            _until  e                   ; - - until done
          _endif                        ; - endif
        _endif                          ; endif
        pop     EAX                     ; get quotient
        pop     EDX                     ; ...
        pop     EBX                     ; get high bit
        add     ESP,8                   ; remove divisor
        pop     EDI                     ; restore sign and exponent
        dec     DI                      ; decrement exponent
        shr     EBX,1                   ; if non-zero
        _if     c                       ; then
          rcr   EDX,1                   ; - shift answer right
          rcr   EAX,1                   ; - ...
          inc   EDI                     ; - increment exponent
        _endif                          ; endif
        sub     ESI,ESI                 ; get zero for zero fill
        mov     CL,11                   ; shift result over
        shrd    EAX,EDX,CL              ; ...
        rcr     ESI,1                   ; save carry
        shrd    EDX,ESI,CL              ; ...

        or      EDX,0FFF00000h          ; turn top bits all on
        _shl    ESI,1                   ; get guard bit
        adc     EAX,0                   ; round up
        adc     EDX,0                   ; ...
        adc     EDI,0                   ; increment exponent if required
        or      DI,DI                   ; if exponent <= 0
        _if     le                      ; then (denormal result)
          _if   e                       ; - if exponent = 0
            mov   CL,1                  ; - - set shift count to 1
          _else                         ; - else
            neg   DI                    ; - - negate to get shift count
            mov   CX,DI                 ; - - ...
          _endif                        ; - endif
          and     EDX,001FFFFFh         ; - isolate fraction
          sub     EBX,EBX               ; - for zero fill
          shrd    EAX,EDX,CL            ; - align the fraction
          shrd    EDX,EBX,CL            ; - ...
          sub     DI,DI                 ; - set exponent to 0
        _endif                          ; endif

        and     EDX,000FFFFFh           ; isolate fraction
        mov     ESI,EDI                 ; get copy of sign
        ror     EDI,11                  ; get exponent
        _shl    ESI,1                   ; get sign
        rcr     EDI,1                   ; put it in
        and     EDI,0FFF00000h          ; isolate sign and exponent
        or      EDX,EDI                 ; place it in result

        pop     ESI                     ; restore registers
        pop     EDI                     ; ...
        pop     EBP                     ; ...
        ret                             ; return
        endproc __FDD

        endmod
        end
