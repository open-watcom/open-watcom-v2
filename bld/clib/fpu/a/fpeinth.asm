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
;* Description:  Generic 80x87 floating-point exception interrupt handler.
;*
;*****************************************************************************


.8087
.386p

include struct.inc
include mdef.inc
include stword.inc
include env387.inc
include fstatus.inc

ifndef  __NETWARE__
        xrefp           __GETDS
endif
        xrefp           __8087  ; indicate that NDP instructions are present

        modstart        fpeinth

        datasegment

        extrn   __FPE_exception_: proc
        extrn   "C",_STACKLOW   : dword

TInf    db 00h,00h,00h,00h,00h,00h,00h,80h,0ffh,7fh
F8Inf   db 0ffh,0ffh,0ffh,0ffh,0ffh,0ffh,0efh,7fh
F4Inf   db 0ffh,0ffh,7fh,7fh
        db      512 dup(0)
FPEStk  label byte
SaveSS  dw      0
SaveESP dd      0
        enddata


; Interrupt handler for 80x87 exceptions.

xdefp   __FPEHandler_
defp    __FPEHandler_
public "C",__FPEHandlerStart_
__FPEHandlerStart_ label byte
        push    EAX                     ; save reg
        mov     AL,20h                  ; issue EOI to 8259 controller
        out     20h,AL                  ; ...
        out     0a0h,AL                 ; issue EOI to slave 8259
        xor     AX,AX                   ; clear busy signal
        out     0f0h,AL                 ; ...
        pop     EAX                     ; restore regs
public  __FPE2Handler_
__FPE2Handler_ label byte
        push    EAX                     ; save regs
        push    EBX                     ; ...
        push    ECX                     ; ...
        push    EDX                     ; ...
        push    ESI                     ; ...
        push    EDI                     ; ...
        push    EBP                     ; ...
        push    DS                      ; ...
        push    ES                      ; ...
        sub     ESP,ENV_SIZE            ; make room for environment information
        mov     EBP,ESP                 ; point to buffer for 80x87 environment
        fnstenv [EBP]                   ; get 80x87 environment
        fwait                           ; wait for 80x87
        fdisi                           ; disable interrupts
        sti                             ; enable CPU interrupts
ifndef  __NETWARE__
        call    __GETDS                 ; load DS
endif
ifdef __NETWARE__
        push    SS                      ; load DS
        pop     DS                      ; ...
endif
        mov     EDX,ENV_CW[EBP]         ; get control word
        not     EDX                     ; flip the mask bits
        mov     DH,0FFh                 ; turn on top byte
        and     EDX,ENV_SW[EBP]         ; get status word
        mov     ES,ENV_IP+4[EBP]        ; get intruction address
        mov     EDI,ENV_IP[EBP]         ; ...
opcode: mov     BX,ES:[EDI]             ; get opcode
        inc     EDI                     ; point to next opcode
        cmp     BL,0d8h                 ; check if its the opcode
        jb      opcode                  ; ...
        cmp     BL,0dfh                 ; ...
        ja      opcode                  ; ...
        mov     ES,ENV_OP+4[EBP]        ; get pointer to operand
        mov     EDI,ENV_OP[EBP]         ; ...
        xchg    BL,BH                   ; get opcode in right position
        mov     CL,FPE_OK               ; assume exception to be ignored
        _guess                          ; guess precision exception
          test  DL,ST_EF_PR             ; - check for precision exception
          _quif e                       ; - quit if not precision exception
          mov   CL,FPE_INEXACT          ; - indicate precision exception
        _admit                          ; guess stack under/overflow
          test  DL,ST_EF_SF             ; - check for stack under/overflow
          _quif e                       ; - quit if not stack under/overflow
          test  DX,ST_C1                ; - check if underflow
          _if   e                       ; - if underflow
            mov CL,FPE_STACKUNDERFLOW   ; - - indicate stack underflow
          _else                         ; - else
            mov CL,FPE_STACKOVERFLOW    ; - - indicate stack overflow
          _endif                        ; - endif
        _admit                          ; guess invalid operation
          test  DL,ST_EF_IO             ; - check for invalid operation
          _quif e                       ; - quit if not invalid operation
          call  InvalidOp               ; - process invalid operation
        _admit                          ; guess denormal operand
          test  DL,ST_EF_DO             ; - check for denormal operand
          _quif e                       ; - quit if not denormal operand
          mov   CL,FPE_DENORMAL         ; - indicate underflow
        _admit                          ; guess overflow
          test  DL,ST_EF_OF             ; - check for overflow
          _quif e                       ; - quit if not overflow
          call  KOOverFlow              ; - process overflow exception
          mov   CL,FPE_OVERFLOW         ; - set floating point error code
        _admit                          ; guess underflow
          test  DL,ST_EF_UF             ; - check for underflow
          _quif e                       ; - quit if not underflow
          mov   CL,FPE_UNDERFLOW        ; - indicate underflow
        _admit                          ; guess divide by 0
          test  DL,ST_EF_ZD             ; - check for divide by zero
          _quif e                       ; - quit if not divide by zero
          call  GetInf                  ; - process divide by zero
          mov   CL,FPE_ZERODIVIDE       ; - indicate divide by zero
        _endguess                       ; endguess
        _guess                          ; guess exception to be handled
          cmp   CL,FPE_OK               ; - check if exception allowed
          _quif e                       ; - quit if exception not allowed
;         cmp   SaveSS,0                ; - check if already in handler
;         _quif ne                      ; - quit if already in handler
          push  _STACKLOW               ; - save old stack low
          mov   SaveSS,SS               ; - save current stack pointer
          mov   SaveESP,ESP             ; - ...
          push  DS                      ; - get new stack pointer
          pop   SS                      ; - ...
          lea   ESP,FPEStk              ; - ...
          lea   EAX,FPEStk-512          ; - set stack low variable
          mov   _STACKLOW,EAX           ; - set stack low variable
          movzx EAX,CL                  ; - set floating point status
          call  __FPE_exception_        ; - call user's handler
          mov   SS,SaveSS               ; - restore stack pointer
          mov   ESP,SaveESP             ; - ...
          pop   _STACKLOW               ; - restore old stacklow
          mov   SaveSS,0                ; - indicate handler can be re-entered
        _endguess                       ; endguess
        fclex                           ; clear exceptions that may have
                                        ; occurred as a result of handling the
                                        ; exception
        and     word ptr ENV_CW[EBP],0FF72h
        fldcw   word ptr ENV_CW[EBP]    ; enable interrupts
        fwait                           ; ...
        add     ESP,ENV_SIZE            ; clean up stack
        pop     ES                      ; restore registers
        pop     DS                      ; ...
        pop     EBP                     ; ...
        pop     EDI                     ; ...
        pop     ESI                     ; ...
        pop     EDX                     ; ...
        pop     ECX                     ; ...
        pop     EBX                     ; ...
        pop     EAX                     ; ...
        iretd                           ; return from interrupt handler
public  "C",__FPEHandlerEnd_
__FPEHandlerEnd_ label byte
endproc __FPEHandler_


; Process invalid operation.

InvalidOp proc near
        mov   CL,FPE_INVALID            ; assume invalid operation
        _guess                          ; guess it's square root
          cmp   BX,0D9FAh               ; - ...
          _quif ne                      ; - quit if it's not that instruction
          mov   CL,FPE_SQRTNEG          ; - indicate sqrt(negative number)
          ret                           ; - return
        _endguess                       ; endguess
        _guess                          ; guess it's square root
          cmp   BX,0D9F1h               ; - ...
          _quif ne                      ; - quit if it's not that instruction
          mov   CL,FPE_LOGERR           ; - indicate sqrt(negative number)
          ret                           ; - return
        _endguess                       ; endguess
        _guess                          ; guess: 'fprem' instruction
          cmp   BX,0D9F8h               ; - check for 'fprem'   10-may-90
          _if   ne                      ; - if not 'fprem'
            cmp   BX,0D9F5h             ; - - check for 'fprem1'
          _endif                        ; - endif
          _quif ne                      ; - quit if not 'fprem' or 'fprem1'
          mov   CL,FPE_MODERR           ; - indicate mod(negative number)
        _admit                          ; guess: integer overflow
          mov   DX,BX                   ; - save op code
          and   DX,0310h                ; - check for fist/fistp instruction
          cmp   DX,0310h                ; - ...
          _quif ne                      ; - quit if its not that instruction
          mov   CL,FPE_IOVERFLOW        ; - indicate integer overflow
        _admit                          ; guess it's floating point underflow
;;        mov   DX,BX                   ; - save op code
          and   DX,0110h                ; - check if fst or fstp instruction
          cmp   DX,0110h                ; - ...
          _quif ne                      ; - quit if it's not that instruction
; Destination is short or long real and source register is an unnormal
; with exponent in range.
          fstp  st(0)                   ; - pop old result
          fldz                          ; - load zero
          mov   DL,BL                   ; - save op code
          and   DL,0C0h                 ; - check the MOD bits of instruction
          cmp   DL,0C0h                 ; - ...
          _if   ne                      ; - if result to be placed in memory
            call  Store                 ; - - store result in memory
          _endif                        ; - endif
          test  BL,08h                  ; - check if result to be popped
          _if   ne                      ; - if result to be popped
            fstp  st(0)                 ; - - pop the result
          _endif                        ; - endif
          mov   CL,FPE_UNDERFLOW        ; - indicate underflow
        _admit                          ; guess it's divide
          mov   DX,BX                   ; - save op code
          and   DX,0130h                ; - check for fdiv/fidiv instruction
          cmp   DX,0030h                ; - ...
          _quif ne                      ; - quit if it's not that instruction
          mov   DX,ENV_TW[EBP]          ; - get tag word
          mov   CL,AH                   ; - get stack pointer
          and   CL,38h                  ; - ...
          shr   CL,2                    ; - ...
          ror   DX,CL                   ; - make stack top low order bits
          and   DL,05h                  ; - check if top two elements are 0
          cmp   DL,05h                  ; - ...
          _quif ne                      ; - quif if they are not 0
          mov   CL,FPE_ZERODIVIDE       ; - indicate divide by zero
        _endguess                       ; endguess
        ret
endproc InvalidOp


; Process overflow exception (note that only floating point overflows
; are handled - integer overflows are invalid operations).

KOOverFlow proc near
        _guess                  ; guess: fscale instruction     10-may-90
          cmp   BX,0D9FDh       ; - quit if not 'fscale' instruction
          _quif ne              ; - ...
        _admit                  ; guess: fst/fstp instruction
          mov   DX,BX           ; - save op code
          and   DX,0110h        ; - check if fst or fstp instruction
          cmp   DX,0110h        ; - ...
          _quif ne              ; - quit if not an fst/fstp instr.
          call  GetInf          ; - load infinity
          mov   DL,BL           ; - save op code
          and   DL,0C0h         ; - check the MOD bits of instruction
          cmp   DL,0C0h         ; - ...
          _if   ne              ; - if result to be placed in memory
            call Store          ; - - store infinity
          _endif                ; - endif
          test  BL,08h          ; - check if result to be popped
          _if   ne              ; - if result to be popped
            fstp  st(0)         ; - - pop result
          _endif                ; - endif
        _admit                  ; admit arithmetic operation
          mov   DL,BL           ; - save op code
          and   DL,0C0h         ; - check if both operands on stack
          cmp   DL,0C0h         ; - ...
          _quif ne              ; - quif both operands not on stack
;
; This code handles overflow on the following intructions:
;    fxxx   ST,ST(i)
;    fxxx   ST(i),ST    where xxx is one of mul,div,sub or add
;    fxxxp  ST(i),ST
;
          lea   ESI,TInf        ; - load internal infinity
          call  Load            ; - ...
        _admit                  ; admit
;
; This admit block is to handle overflow on the following intructions:
;    fxxx   short real
;    fxxx   long real   where xxx is one of mul,div,sub or add
;
          call  GetInf          ; - load infinity
        _endguess               ; endguess
        ret                     ; return
endproc KOOverFlow


; Replace the top element of the stack with the appropriate signed
; infinity.

GetInf  proc    near
        ftst                    ; get sign of result
        fstsw   word ptr ENV_OP[EBP]
        fstp    st(0)           ; pop argument off stack (does fwait)
        test    BH,04h          ; check if single or double
        _if     ne              ; if double
          fld   qword ptr F8Inf ; - load double precision infinity
        _else                   ; else
          fld   dword ptr F4Inf ; - load single precision infinity
        _endif                  ; endif
        test    word ptr ENV_OP[EBP],ST_C0
        _if     ne              ; if argument is negative
          fchs                  ; - return negative infinity
        _endif                  ; endif
        ret                     ; return
endproc GetInf


; Replace an element on the stack with internal zero or infinity.

Load    proc    near
        test    BH,04h          ; check if result is top element
        _if     e               ; if result is not top element
          xor   DL,DL           ; - indicate we are at the top
        _else                   ; else
          mov   DL,BL           ; - get st(i)
          and   DL,07h          ; - . . .
        _endif                  ; endif
        push    EDX             ; save st(i)
        _loop                   ; loop
          dec   DL              ; - decrement counter
          _quif l               ; - quit if we are at st(i)
          fincstp               ; - increment stack pointer
        _endloop                ; endloop
        fstp    st(0)           ; free the stack element
        fld     tbyte ptr [ESI] ; load internal zero
        pop     EDX             ; get st(i)
        _loop                   ; loop
          dec   DL              ; - decrement counter
          _quif l               ; - quit if we are at st(i)
          fdecstp               ; - decrement stack pointer
        _endloop                ; endloop
        ret                     ; return
endproc Load


; Store the top element of the stack at ES:EDI.

Store   proc    near
        test    BH,04h
        _if     ne                      ; if double
          fst   qword ptr ES:[EDI]      ; - store as double precision result
        _else                           ; else
          fst   dword ptr ES:[EDI]      ; - store as single precision result
        _endif                          ; endif
        ret                             ; return
endproc Store

        endmod
        end
