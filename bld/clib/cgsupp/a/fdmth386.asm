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
;   real*8 math library
;
;
;     inputs: EDX,EAX - operand 1 (high word, low word resp. ) (op1)
;             ECX,EBX - operand 2                              (op2)
;
;     operations are performed as op1 (*) op2 where (*) is the selected
;     operation
;
;     output: EDX,EAX - result    (high word, low word resp. )
;
;
include mdef.inc
include struct.inc

.8087
        modstart        fdmth386

        xref            __8087  ; indicate that NDP instructions are present

ifndef __PENPOINT__

        datasegment
        extrn   __real87 : byte         ; 8087.asm
        enddata

endif

        xref    F8DivZero       ; Fstatus
        xref    F8OverFlow      ; Fstatus
        xref    F8UnderFlow     ; Fstatus

        xdefp   __FDA           ; add real*8 to real*8
        xdefp   __FDS           ; subtract real*8 from real*8
        xdefp   __FDM           ; 8-byte real multiply


        defpe   __FDS
        xor     ECX,80000000h   ; flip the sign of op2 and add

        defpe   __FDA
        or      EBX,EBX         ; if low word of op2 is 0
        _if     e               ; then
          _shl  ECX,1           ; - place sign in carry
          je    ret_op1         ; - if op2 is 0 then return operand 1
          rcr   ECX,1           ; - put sign back
        _endif                  ; endif
        or      EAX,EAX         ; if op1 is 0
        _if     e               ; then
          _shl  EDX,1           ; - place sign in carry
          _if   e               ; - if op1 really is 0
            mov   EDX,ECX       ; - - return operand 2
            mov   EAX,EBX       ; - - . . .
ret_op1:    ret                 ; - - return
          _endif                ; - endif
          rcr   EDX,1           ; - put sign back
        _endif                  ; endif

ifndef __PENPOINT__
        cmp     byte ptr __real87,0; if 8087 not to be used
        je      short __FDAemu  ; then emulate

__FDA87:
        push    EDX             ; push operand 1
        push    EAX             ; . . .
        fld     qword ptr [ESP] ; load operand 1
        push    ECX             ; push operand 2
        push    EBX             ; . . .
        fadd    qword ptr [ESP] ; add operand 2 to operand 1
_ret87:
        fstp    qword ptr 8[ESP]; store result
        add     ESP,8           ; clean up stack
        fwait                   ; wait
        pop     EAX             ; load result into EDX:EAX
        pop     EDX             ; . . .
        cmp     EDX,80000000H   ; is it a negative zero?
        _if     e               ; if it is then
          sub   EDX,EDX         ; - make it positive 0.0
          mov   EAX,EDX         ; - ...
        _endif                  ; endif
        ret                     ; return
endif

__FDAemu:
        push    EBP             ; save EBP
        push    EDI             ; save EDI
        push    ESI             ; save EDI
        mov     EDI,EDX         ; get high part of op1
        mov     ESI,ECX         ; get high part of op2
        sar     EDI,20          ; shift exponent to bottom, duplicating sign
        sar     ECX,20          ; shift exponent to bottom, duplicating sign
        and     EDI,0800007FFh  ; isolate signs and exponent
        and     ECX,0800007FFh  ; ...
        mov     EBP,ECX         ; assume op1 < op2
        rol     EDI,16          ; rotate signs to bottom
        rol     ECX,16          ; ...
        add     CX,DI           ; calc sign of result
        rol     EDI,16          ; rotate signs to top
        rol     ECX,16          ; ...
        and     EDX,000FFFFFh   ; isolate fraction
        and     ESI,000FFFFFh   ; isolate fraction
        or      DI,DI           ; if op1 is not a denormal
        _if     ne              ; then
          or    EDX,00100000h   ; - turn on implied 1 bit
        _endif                  ; endif
        or      CX,CX           ; if op2 is not a denormal
        _if     ne              ; then
          or    ESI,00100000h   ; - turn on implied 1 bit
        _endif                  ; endif
        _shl    EAX,1           ; shift left 1 to make room for guard bit
        _rcl    EDX,1           ; ...
        _shl    EBX,1           ; ...
        _rcl    ESI,1           ; ...
        sub     CX,DI           ; calculate difference in exponents
        _if     ne              ; if different
          _if   b               ; - if op1 < op2
            mov   EBP,EDI       ; - - get larger exponent for result
            neg   CX            ; - - negate the shift count
            xchg  EAX,EBX       ; - - flip operands
            xchg  EDX,ESI       ; - - . . .
          _endif                ; - endif
          cmp     CX,53+1       ; - if shift count too big
          _if     a             ; - then, return operand 1
            mov   EDX,ESI       ; - - get value in correct registers
            mov   EAX,EBX       ; - - . . .
            _shl  EBP,1         ; - - get sign
            rcr   EDX,1         ; - - rebuild operand 1
            rcr   EAX,1         ; - - ...
            and   EDX,800FFFFFh ; - - ...
            ror   EBP,13        ; - - rotate exponent into position
            and   EBP,7FF00000h ; - - ...
            or    EDX,EBP       ; - - put in exponent
            pop   ESI           ; - - restore ESI
            pop   EDI           ; - - restore EDI
            pop   EBP           ; - - restore EBP
            ret                 ; - - return
          _endif                ; - endif
        _endif                  ; endif
        or      ECX,ECX         ; get bit 0 of sign word - value is 0 if
                                ; both operands have same sign, 1 if not
        _if     s               ; if signs are different
          neg   ESI             ; - negate the fraction of op2
          neg   EBX             ; - . . .
          sbb   ESI,0           ; - . . .
          xor   EBP,80000000h   ; - flip sign
        _endif                  ; endif
        sub     EDI,EDI         ; get a zero for sticky bits
        cmp     CL,0            ; if shifting required
        _if     ne              ; then
          push    EBX           ; - save EBX
          sub     EBX,EBX       ; - for zero fill
          cmp     CL,32         ; - if shift count >= 32
          _if     ae            ; - then
            or    EAX,EAX       ; - - check low order word for 1 bits
            setne BL            ; - - BL=1 if EAX non zero
            mov   EDI,EBX       ; - - save sticky bits
            sub   EBX,EBX       ; - - for zero fill
            mov   EAX,EDX       ; - - shift right 32
            sub   EDX,EDX       ; - - zero high word
;;;         sub   CL,32         ; - - adjust shift count
          _endif                ; - endif
          shrd    EBX,EAX,CL    ; - get the extra sticky bits
          or      EDI,EBX       ; - save them
          sub     EBX,EBX       ; - for zero fill
          shrd    EAX,EDX,CL    ; - align the fractions
          shrd    EDX,EBX,CL    ; - ...
          pop     EBX           ; - restore EBX
        _endif                  ; endif

        add     EAX,EBX         ; add the fractions
        adc     EDX,ESI         ; . . .
        _if     s               ; if answer is negative
          cmp   CL,53           ; - if shift count >= 53
          _if   ae              ; - then
            test  EDI,7FFFFFFFh ; - - check the sticky bits
            setne BL            ; - - make single sticky bit
            shr   EBX,1         ; - - carry set if sticky=1
            adc   EAX,0         ; - - round up fraction if required
            adc   EDX,0         ; - - . . .
          _endif                ; - endif
          neg   EDX             ; - negate the fraction
          neg   EAX             ; - . . .
          sbb   EDX,0           ; - . . .
          xor   EBP,80000000h   ; - flip the sign
        _endif                  ; endif
        mov     EBX,EAX         ; get result
        or      EBX,EDX         ; if not zero
        _if     ne              ; then
          or    BP,BP           ; - if exponent is 0
          je    short denormal  ; - denormal when exponent hits 0
          _loop                 ; - loop (normalize)
            test  EDX,7FE00000h ; - - stop when bit slides into exponent field
            _quif ne            ; - - ...
            dec   BP            ; - - decrement exponent
            je    short denormal; - - denormal when exponent hits 0
            _shl  EAX,1         ; - - shift fraction left one bit
            _rcl  EDX,1         ; - - ...
          _endloop              ; - endloop
          test  EDX,00400000h   ; - if we got a carry
          _if   ne              ; - then
            shr   EDX,1         ; - - shift fraction right 1
            rcr   EAX,1         ; - - ...
            adc   EDI,0         ; - - keep sticky bit
            inc   BP            ; - - increment exponent
            cmp   BP,07FFh      ; - - quit if overflow
            je    add_oflow     ; - - . . .
          _endif                ; - endif
          ; normalize the fraction
          shr   EDX,1           ; - get guard bit
          rcr   EAX,1           ; - ...
          _if     c             ; - if guard bit is on
            or    EDI,EDI       ; - - check the sticky bits
            setne BL            ; - - make single sticky bit
            or    EBX,EAX       ; - - or sticky bit with bottom bit
            shr   EBX,1         ; - - carry set if sticky=1 or bottom=1
            adc   EAX,0         ; - - round up fraction if required
            adc   EDX,0         ; - - . . .
            test  EDX,00200000h ; - - if we got a carry (02-nov-90)
            _if   ne            ; - - then
              shr   EDX,1       ; - - - shift fraction right 1
              rcr   EAX,1       ; - - - ...
              inc   BP          ; - - - increment exponent
              cmp   BP,07FFh    ; - - - quit if overflow
              je    add_oflow   ; - - - . . .
            _endif              ; - - endif
          _endif                ; - endif
          and   EDX,000FFFFFh   ; - get rid of implied 1 bit
          mov   ECX,EBP         ; - get sign
          shl   EBP,21          ; - shift exponent to top
          _shl  ECX,1           ; - get sign
          rcr   EBP,1           ; - put it in
          or    EDX,EBP         ; - put exponent and sign into result
        _endif                  ; endif
        pop     ESI             ; restore ESI
        pop     EDI             ; restore EDI
        pop     EBP             ; restore EBP
        ret                     ; return

denormal:                       ; handle denormal
        _shl    EBP,1           ; get sign
        rcr     EDX,1           ; put it in result
        rcr     EAX,1           ; ...
        pop     ESI             ; restore ESI
        pop     EDI             ; restore EDI
        pop     EBP             ; restore EBP
        ret                     ; return

add_oflow:                      ; handle overflow
        mov     EAX,EBP         ; get proper sign for infinity
        pop     ESI             ; restore ESI
        pop     EDI             ; restore EDI
        pop     EBP             ; restore EBP
        jmp     F8OverFlow      ; handle overflow
        endproc __FDA
        endproc __FDS
;=====================================================================
;<> multiplies X by Y and places result in C.
;<> X2 and X1 represent the high and low words of X. Similarly for Y and C
;<> Special care is taken to use only six registers, so the code is a bit
;<> obscure

        defpe   __FDM
        _guess                  ; guess: one of the operands is 0
          or    EAX,EAX         ; - see if first arg is zero
          _quif ne              ; - quit if op1 is not 0
          _shl  EDX,1           ; - place sign in carry
          _if   e               ; - if operand one is 0
            ret                 ; - - return
          _endif                ; - endif
          rcr   EDX,1           ; - restore sign
        _endguess               ; endguess
        _guess                  ; guess: op2 is 0
          or    EBX,EBX         ; - quit if op2 is not 0
          _quif ne              ; - . . .
          _shl  ECX,1           ; - place sign in carry
          _if   e               ; - if operand 2 is 0
            sub   EAX,EAX       ; - - set result to 0
            sub   EDX,EDX       ; - - . . .
            ret                 ; - - return
          _endif                ; - endif
          rcr   ECX,1           ; - restore sign of op2
        _endguess               ; endguess

ifndef __PENPOINT__
        cmp     byte ptr __real87,0; if 8087 not to be used
        je      short __FDMemu  ; then emulate

__FDM87:
        push    EDX             ; push operand 1
        push    EAX             ; . . .
        fld     qword ptr [ESP] ; load operand 1
        push    ECX             ; push operand 2
        push    EBX             ; . . .
        fmul    qword ptr [ESP] ; multiply operand 1 by operand 2
        jmp     _ret87          ; goto common epilogue

endif

__FDMemu:
        push    EBP             ; save EBP
        push    EDI             ; save EDI
        push    ESI             ; save EDI
        mov     EDI,EDX         ; get high part of op1
        mov     ESI,ECX         ; get high part of op2
        sar     EDI,20          ; shift exponent to bottom, duplicating sign
        sar     ECX,20          ; shift exponent to bottom, duplicating sign
        and     EDI,0800007FFh  ; isolate signs and exponent
        and     ECX,0800007FFh  ; ...
        rol     EDI,16          ; rotate signs to bottom
        rol     ECX,16          ; ...
        add     CX,DI           ; calc sign of result
        rol     EDI,16          ; rotate signs to top
        rol     ECX,16          ; ...
        and     EDX,000FFFFFh   ; isolate fraction
        and     ESI,000FFFFFh   ; isolate fraction
        or      DI,DI           ; if op1 is a denormal
        _if     e               ; then
          inc   DI              ; - adjust exponent by 1
          _loop                 ; - loop (normalize it)         27-jul-90
            dec   DI            ; - - decrement exponent
            _shl  EAX,1         ; - - shift left 1
            _rcl  EDX,1         ; - - ...
            test  EDX,00100000h ; - - check for implied 1 bit
          _until  ne            ; - until normalized
        _endif                  ; endif
        or      EDX,00100000h   ; turn on implied 1 bit
        or      CX,CX           ; if op2 is a denormal
        _if     e               ; then
          inc   CX              ; - adjust exponent by 1
          _loop                 ; - loop (normalize it)         27-jul-90
            dec   CX            ; - - decrement exponent
            _shl  EBX,1         ; - - shift left 1
            _rcl  ESI,1         ; - - ...
            test  ESI,00100000h ; - - check for implied 1 bit
          _until  ne            ; - until normalized
        _endif                  ; endif
        or      ESI,00100000h   ; turn on implied 1 bit

        _guess                  ; guess: overflow
          add   CX,DI           ; - determine exponent of result
          sub   CX,03ffh        ; - remove extra bias
          _quif s               ; - quit if exponent is negative
          cmp   CX,07FFh        ; - quit if not overflow
          _quif b               ; - . . .
          mov   EAX,ECX         ; - put sign into EAX
          pop   ESI             ; - restore ESI
          pop   EDI             ; - restore EDI
          pop   EBP             ; - restore EBP
          jmp   F8OverFlow      ; - handle overflow
        _endguess               ; endguess
        cmp     CX,-53          ; if exponent is too small
        _if     l               ; then underflow
          pop   ESI             ; - restore ESI
          pop   EDI             ; - restore EDI
          pop   EBP             ; - restore EBP
          jmp   F8UnderFlow     ; - handle underflow
        _endif                  ; endif
        push    ECX             ; save sign and exponent
        mov     CL,11           ; shift fractions to top of registers
        shld    EDX,EAX,CL      ; ...
        shld    EAX,EBP,CL      ; ...
        and     EAX,0FFFFF800h  ; ...
        shld    ESI,EBX,CL      ; ...
        shld    EBX,EBP,CL      ; ...
        and     EBX,0FFFFF800h  ; ...

        sub     EBP,EBP         ; zero EBP
        push    ESI             ; save high part of op2
        push    EDX             ; save high part of op1
        push    EAX             ; save low part of op1
        mul     EBX             ; low part of op1 * low part of op2
        xchg    EAX,ESI         ; ESI becomes start of the sticky bits
        mov     ECX,EDX         ; save high part of result
        pop     EDX             ; restore low part of op1
        mul     EDX             ; low part of op1 * high part of op2
        mov     EDI,EDX         ; save high part of product
        add     ECX,EAX         ; add partial product
        adc     EDI,EBP         ; ...
        adc     EBP,EBP         ; ...
        pop     EAX             ; restore high part of op1
        xchg    EAX,EBX         ; flip with low part of op2
        mul     EBX             ; high part of op1 * low part of op2
        add     ECX,EAX         ; add partial product
        adc     EDI,EDX         ; ...
        adc     EBP,0           ; ...
        mov     EAX,EBX         ; get high part of op1
        pop     EDX             ; restore high part of op2
        mul     EDX             ; high part of op1 * high part of op2
        add     EAX,EDI         ; add partial product
        adc     EDX,EBP         ; ...
        sub     EBX,EBX         ; get zero for zero fill
        mov     CL,10           ; shift result over
        shrd    EBX,EAX,CL      ; ... get sticky bits 18-feb-91
        shrd    EAX,EDX,CL      ; ...
        shrd    EDX,EBX,CL      ; ...
        pop     ECX             ; restore sign and exponent

        _loop                   ; loop
          test  EDX,00200000h   ; - test to see if bit in exponent field
          _quif e               ; - quit if not
          shr   EDX,1           ; - shift result right
          rcr   EAX,1           ; - . . .
          rcr   EBX,1           ; - save carry
          inc   CX              ; - inc exponent for every shift
          cmp   CX,07FFh        ; - quit if overflow
          je    mul_oflow       ; - . . .
        _endloop                ; endloop
        _shl    EBX,1           ; get guard bit
        _if     c               ; if set
          _if   e               ; - if rest of sticky bits are 0
            or    ESI,ESI       ; - - check the bottom sticky bits
            setne BL            ; - - ...
            shr   EBX,1         ; - - if all sticky bits are zero
            _if   nc            ; - - then
              mov   ESI,EAX     ; - - - get bottom bit of result
              shr   ESI,1       ; - - - as rounding bit
            _endif              ; - - endif
          _endif                ; - endif
          adc   EAX,0           ; - round up
          adc   EDX,0           ; - ...
          test  EDX,00200000h   ; - test to see if bit in exponent field
          _if   ne              ; - if fraction overflowed
            shr   EDX,1         ; - - shift right
            rcr   EAX,1         ; - - ...
            inc   CX            ; - - increment exponent
            cmp   CX,07FFh      ; - - quit if overflow
            je    mul_oflow     ; - - . . .
          _endif                ; - endif
        _endif                  ; endif
        or      CX,CX           ; if exponent <= 0
        _if     le              ; then (denormal result)
          _if   e               ; - if exponent = 0
            mov   CL,1          ; - - set shift count to 1
          _else                 ; - else
            neg   CX            ; - - negate to get shift count
            dec   CX            ; - - adjust
          _endif                ; - endif
          sub   EBX,EBX         ; - for zero fill
          shrd  EAX,EDX,CL      ; - align the fraction
          shrd  EDX,EBX,CL      ; - ...
          sub   CX,CX           ; - set exponent to 0
        _endif                  ; endif
        and     EDX,000FFFFFh   ; isolate fraction
        mov     ESI,ECX         ; get copy of sign
        ror     ECX,11          ; get exponent
        _shl    ESI,1           ; get sign
        rcr     ECX,1           ; put it in
        and     ECX,0FFF00000h  ; isolate sign and exponent
        or      EDX,ECX         ; place it in result
        pop     ESI             ; restore ESI
        pop     EDI             ; restore EDI
        pop     EBP             ; restore EBP
        ret                     ; return

mul_oflow:                      ; overflow
        mov     EAX,ECX         ; get sign of infinity
        pop     ESI             ; restore ESI
        pop     EDI             ; restore EDI
        pop     EBP             ; restore EBP
        jmp     F8OverFlow      ; handle overflow
        endproc __FDM

        endmod
        end
