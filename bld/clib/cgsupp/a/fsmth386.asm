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
;* Description:  REAL*4 math library.
;*
;*****************************************************************************


;
;       inputs: EAX - operand 1 (high word, low word resp. ) (op1)
;               EDX - operand 2                              (op2)
;
;       operations are performed as op1 (*) op2 where (*) is the selected
;       operation
;
;       output: EAX - result
;
;       F4Add, F4Sub - written  28-apr-84
;                    - modified by A.Kasapi 15-may-84
;                    - to:      Calculate sign of result
;                    -          Guard bit in addition for extra accuracy
;                               Add documentation
;       F4Mul        - written  16-may-84
;                    - by       Athos Kasapi
;       F4DIV        - written  may-84 by "
;
include mdef.inc
include struct.inc

.287
        modstart        fsmth386

        xref            __8087  ; indicate that NDP instructions are present

        xref    __fdiv_m32

        datasegment
        extrn   __real87 : byte         ; cstart
        extrn   __chipbug : byte
fsadd   dd      _chkadd
fsmul   dd      _chkmul
fsdiv   dd      _chkdiv
        enddata


        xref    F4DivZero       ; Fstatus
        xref    F4OverFlow      ; Fstatus
        xref    F4UnderFlow     ; Fstatus

        xdefp   __FSA           ; add real*4 to real*4
        xdefp   __FSS           ; subtract real*4 from real*4
        xdefp   __FSM           ; 4-byte real multiply
        xdefp   __FSD           ; 4-byte real divide


        defpe   __FSS
        or      EDX,EDX         ; if op2 is 0
        je      short ret_op1   ; then return operand 1
        xor     EDX,80000000h   ; flip the sign of op2 and add

        defpe   __FSA
        or      EDX,EDX         ; if op2 is 0
        je      short ret_op1   ; then return operand 1
        or      EAX,EAX         ; if op1 is 0
        _if     e               ; then
          mov   EAX,EDX         ; - return operand 2
ret_op1:  ret                   ; - return
        _endif                  ; endif
        jmp     fsadd

__FSA87:
        push    EBP             ; save EBP
        mov     EBP,ESP         ; get access to stack
        push    EAX             ; push operand 1
        fld     dword ptr -4[EBP]; load operand 1
        push    EDX             ; push operand 2
        fadd    dword ptr -8[EBP]; add operand 2 to operand 1
_ret87:
        fstp    dword ptr -4[EBP]; store result
        add     ESP,4           ; clean up stack
        fwait                   ; wait
        pop     EAX             ; load result into EAX
        cmp     EAX,80000000H   ; is result -0.0
        _if     e               ; if it is then
        xor     EAX,EAX         ; - make it positive
        _endif                  ; endif
        pop     EBP             ; restore EBP
        ret                     ; return

__FSAemu:
        push    ECX             ; save ECX
        push    EBX             ; save EBX
;<> Scheme for calculating sign of result:
;<>   The sign word is built and kept in CL
;<>   Bits 0 and 1 hold the sum of the sign bits
;<>       shifted out of op_1 and op_2
;<>   Bit 2 holds the sign of the larger operand. It is assumed to be
;<>       op_1 until op_2 is found larger

        sub     ECX,ECX         ; clear ECX
        _shl    EAX,1           ; get sign of op1
        _rcl    CL,1            ;
        mov     CH,CL           ;
        _shl    CL,1            ;
        _shl    CL,1            ;
        add     CL,CH           ;
        rol     EAX,8           ; get exponent of op1 into AL
        _shl    EDX,1           ; get sign of op2
        adc     CL,0            ; place in CL
        rol     EDX,8           ; get exponent of op2 in DL
        mov     BL,AL           ; get exponent of op1
        mov     BH,DL           ; get exponent of op2

        mov     AL,0            ; zero rest of fraction
        stc                     ; put implied 1 bit into top bit of
        rcr     EAX,1           ; ... fraction
        mov     DL,0            ; zero rest of fraction
        stc                     ; put implied 1 bit into top bit
        rcr     EDX,1           ; ... of fraction

        mov     CH,BL           ; assume op1 > op2
        sub     BL,BH           ; calculate difference in exponents
        _if     ne              ; if different
          _if   b               ; - if op1 < op2
            mov   CH,BH         ; - - get larger exponent for result
            neg   BL            ; - - negate the shift count
            xchg  EAX,EDX       ; - - flip operands
            xor   CL,4

;<> op_2 is larger, so its sign now occupies bit 2 of sign word.  This
;<> information is only correct if the signs of op-1 and op-2 are different.
;<> Since we look past bit 1 for sign only if the signs are different, bit2
;<> will supply the correct information when it is needed. We get the sign of
;<> op_2 by flipping the sign of op_1, already in bit 2

          _endif                ; - endif
          xchg  CL,BL           ; - get shift count
          cmp   CL,32           ; - if count >= 32
          _if   ge              ; - then
            sub   EDX,EDX       ; - - answer is 0
          _else                 ; - else
            shr   EDX,CL        ; - - align fraction
          _endif                ; - endif
          xchg  CL,BL           ; - put back
        _endif                  ; endif
        shr     CL,1            ; get bit 0 of sign word - value is 0 if
                                ; both operands have same sign, 1 if not
        _if     nc              ; if signs are the same
          add   EAX,EDX         ; - add the fractions
          _if   c               ; - if carry
            rcr   EAX,1         ; - - shift fraction right 1 bit
            inc   CH            ; - - increment exponent
            _if   z             ; - - if we overflowed
              ror CL,1          ; - - - set sign of infinity
              rcr EAX,1         ; - - - . . .
              jmp short add_oflow;- - - handle overflow
            _endif              ; - - endif
          _endif                ; - endif
        _else                   ; else (signs are different)
          shr   CL,1            ; - skip junk bit
          sub   EAX,EDX         ; - subtract the fractions
          _guess                ; - guess
            _quif nc            ; - - quit if no borrow
            inc   CL            ; - - sign := sign of op_2
            neg   EAX           ; - - negate the fraction
          _admit                ; - admit
            or    EAX,EAX       ; - - quit if answer is not 0
            _quif ne            ; - - . . .
            pop   EBX           ; - - restore EBX
            pop   ECX           ; - - restore ECX
            ret                 ; - - return (answer is 0)
          _endguess             ; - endguess
        _endif                  ; endif

        ; normalize the fraction
        add     EAX,00000080h   ; round up fraction if required
        mov     AL,0            ; zero bottom 8 bits    10-jul-89
        _guess  underflow       ; guess
          _quif nc              ; - quit if round up didn't overflow frac
          inc   CH              ; - adjust exponent
        _admit                  ; admit
          _loop                 ; - loop (shift until high bit appears)
            _shl  EAX,1         ; - - shift fraction left
            _quif c,underflow   ; - - quit if carry has appeared
            dec   CH            ; - - decrement exponent
          _until  e             ; - until underflow
          jmp   short add_uflow ; - handle underflow
        _endguess               ; endguess
        mov     AL,CH           ; get exponent
        ror     EAX,8           ; rotate into position
        ror     CL,1            ; get sign bit
        rcr     EAX,1           ; shift it into result
        pop     EBX             ; restore EBX
        pop     ECX             ; restore ECX
        ret                     ; return

add_uflow:                      ; handle underflow
        pop     EBX             ; restore EBX
        pop     ECX             ; restore ECX
        jmp     F4UnderFlow     ; goto underflow routine

add_oflow:                      ; handle overflow
        pop     EBX             ; restore EBX
        pop     ECX             ; restore ECX
        jmp     F4OverFlow      ; handle overflow
        endproc __FSA
        endproc __FSS

;=====================================================================

        defpe   __FSM

;<> multiplies X by Y and places result in C.
;<> X2 and X1 represent the high and low words of X. Similarly for Y and C
;<> Special care is taken to use only six registers, so the code is a bit
;<> obscure

        _guess                  ; guess: answer not 0
          or    EAX,EAX         ; - see if first arg is zero
          _quif e               ; - quit if op1 is 0
          or    EDX,EDX         ; - quit if op2 is 0
          _quif e               ; - . . .
          jmp   fsmul           ; - perform multiplication
        _endguess               ; endguess
        sub     EAX,EAX         ; set answer to 0
        ret                     ; return

__FSM87:
        push    EBP             ; save EBP
        mov     EBP,ESP         ; get access to stack
        push    EAX             ; push operand 1
        fld     dword ptr -4[EBP]; load operand 1
        push    EDX             ; push operand 2
        fmul    dword ptr -8[EBP]; mulitply operand 1 by operand 2
        jmp     _ret87          ; goto common epilogue

__FSMemu:
        push    ECX             ; save ECX
        _shl    EAX,1           ; get sign of op1
        _rcl    ECX,1           ; save it
        _shl    EDX,1           ; get sign of op2
        adc     ECX,0           ; calc sign of result
        ror     ECX,1           ; move to the top
        rol     EAX,8           ; move exponent of op1 into AL
        rol     EDX,8           ; move exponent of op2 into DL
        sub     AL,7Fh          ; remove bias from exponents
        sub     DL,7Fh          ; . . .
        add     DL,AL           ; add exponents
        _if     o               ; if over or underflow
          js    short mul_oflow ; - report overflow if signed
          jmp   short mul_uflow ; - handle underflow
        _endif                  ; endif
        cmp     DL,81h          ; check for underflow
        jle     short mul_uflow ; quit if underflow
        add     DL,7fh+1        ; bias exponent
        mov     CL,DL           ; save exponent
        mov     AL,0            ; zero rest of fraction
        mov     DL,0            ; ...
        stc                     ; turn on implied 1 bit in fraction
        rcr     EAX,1           ; ...
        stc                     ; turn on implied 1 bit in fraction
        rcr     EDX,1           ; ...
        mul     EDX             ; calc fraction
        or      EDX,EDX         ; check top bit
        _if     ns              ; if not set
          _shl  EDX,1           ; - move left 1
          dec   CL              ; - decrement exponent
        _endif                  ; endif
        sar     EDX,8           ; place fraction in correct location
        adc     EDX,0           ; round up
        adc     CL,0            ; increment exponent if necessary
        jz      short mul_oflow ; report overflow if required
        shl     EDX,9           ; get rid of implied 1 bit
        mov     DL,CL           ; get exponent
        ror     EDX,8           ; rotate into position except for sign
        _shl    ECX,1           ; get sign
        rcr     EDX,1           ; place sign in result
        mov     EAX,EDX         ; place in correct register
        pop     ECX             ; restore ECX
        ret                     ; return

mul_uflow:                      ; underflow
        pop     ECX             ; restore ECX
        jmp     F4UnderFlow     ; . . .

mul_oflow:                      ; overflow
        mov     EAX,ECX         ; get sign
        pop     ECX             ; restore ECX
        jmp     F4OverFlow      ; report overflow
        endproc __FSM

;====================================================================

        defpe   __FSD
        jmp     fsdiv

__FSDbad_div:
        push    EBP             ; save EBP
        mov     EBP,ESP         ; get access to stack
        push    EAX             ; push operand 1
        fld     dword ptr -4[EBP]; load operand 1
        push    EDX             ; push operand 2
        call    __fdiv_m32      ; divide operand 1 by operand 2
        push    EDX             ; __fdiv_m32 popped operand 2, _ret87 wants it
        jmp     _ret87          ; goto common epilogue

__FSD87:
        push    EBP             ; save EBP
        mov     EBP,ESP         ; get access to stack
        push    EAX             ; push operand 1
        fld     dword ptr -4[EBP]; load operand 1
        push    EDX             ; push operand 2
        fdiv    dword ptr -8[EBP]; divide operand 1 by operand 2
        jmp     _ret87          ; goto common epilogue


__FSDemu:
        _shl    EDX,1           ; shift sign of divisor into carry
        _if     e               ; if divisor is zero
          jmp   F4DivZero       ; - handle divide by zero
        _endif                  ; endif
        push    ECX             ; save ECX
        _rcl    ECX,1           ; save sign in ECX
        _shl    EAX,1           ; shift sign of dividend into carry
        _if     e               ; if dividend is 0, then
          pop   ECX             ; - restore ECX
          ret                   ; - return
        _endif                  ; endif
        adc     ECX,0           ; now calculate save sign of result in ECX
        ror     ECX,1           ; rotate sign to top
        rol     EAX,8           ; get exponent into AL
        rol     EDX,8           ; get exponent into DL
        sub     AL,7Fh          ; calculate exponent of result
        sub     DL,7Fh          ; . . .
        sub     AL,DL           ; . . .
        _if     o               ; if over or underflow
          jns   short div_uflow ; - handle underflow
          _shl  ECX,1           ; - get sign of infinity
          rcr   EAX,1           ; - . . .
          jmp   short div_oflow ; - handle overflow
        _endif                  ; endif
        cmp     AL,81h          ; check for underflow
        jle     short div_uflow ; . . .
        add     AL,7Fh          ; restore bias to exponent
        mov     CH,AL           ; save calculated exponent
        mov     AL,0            ; zero bottom of fraction
        mov     DL,0            ; ...
        stc                     ; rotate implied '1'bit back into divisor
        rcr     EDX,1           ; . . .
        stc                     ; rotate implied '1' bit into dividend
        rcr     EAX,1           ; . . .
        push    ECX             ; save sign and exponent
        mov     ECX,EDX         ; save divisor
        mov     EDX,EAX         ; place dividend into EDX
        sub     EAX,EAX         ; set rest to 0
        shr     EDX,1           ; so we don't get a divide overflow trap
        div     ECX             ; do the divide
        pop     ECX             ; restore sign and exponent
        or      EAX,EAX         ; check top bit
        _if     ns              ; if not set
          _shl  EAX,1           ; - move left 1
          dec   CH              ; - decrement exponent
        _endif                  ; endif
        sar     EAX,8           ; place fraction in correct location
        adc     EAX,0           ; round up
        _guess                  ; guess have to inc exponent
          _quif nc              ; - quit if no carry
          inc   CH              ; - increment exponent
          _quif nz              ; - quit if no overflow
          mov   EAX,ECX         ; - get sign of infinity
          jmp   short div_oflow ; - handle overflow
        _endguess               ; endguess
        shl     EAX,9           ; get rid of implied 1 bit
        mov     AL,CH           ; get exponent
        ror     EAX,8           ; rotate into position except for sign
        _shl    ECX,1           ; get sign
        rcr     EAX,1           ; place sign in result
        pop     ECX             ; restore ECX
        ret                     ; return to caller

div_uflow:                      ; handle underflow
        pop     ECX             ; restore ECX
        jmp     F4UnderFlow     ; handle underflow


div_oflow:                      ; handle overflow
        pop     ECX             ; restore ECX
        jmp     F4OverFlow      ; handle overflow
        endproc __FSD


_chkadd: call   _chk8087
        jmp     fsadd

_chkmul: call   _chk8087
        jmp     fsmul

_chkdiv: call   _chk8087
        jmp     fsdiv


_chk8087 proc   near
        push    eax                     ; save AX
        cmp     byte ptr __real87,0     ; if real 80x87 NDP present
        _if     ne                      ; then
          mov   eax,offset __FSA87      ; - get addr of add rtn
          mov   fsadd,eax               ; - ...
          mov   eax,offset __FSM87      ; - get addr of mul rtn
          mov   fsmul,eax               ; - ...
          test  byte ptr __chipbug, 1   ; - if we've got a bad divider
          _if   ne                      ; - then
            mov eax,offset __FSDbad_div ; - - get addr of div rtn
          _else                         ; - else
            mov eax,offset __FSD87      ; - - get addr of div rtn
          _endif                        ; - endif
          mov   fsdiv,eax               ; - ...
        _else                           ; else
          mov   eax,offset __FSAemu     ; - get addr of add rtn
          mov   fsadd,eax               ; - ...
          mov   eax,offset __FSMemu     ; - get addr of mul rtn
          mov   fsmul,eax               ; - ...
          mov   eax,offset __FSDemu     ; - get addr of div rtn
          mov   fsdiv,eax               ; - ...
        _endif                          ; endif
        pop     eax                     ; restore AX
        ret                             ; return
        endproc _chk8087

        endmod
        end
