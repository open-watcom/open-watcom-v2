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
;* Description:  80x87 interrupt handler.
;*
;*****************************************************************************


.8087
.286

include struct.inc
include mdef.inc
include stword.inc
include env87.inc
include fstatus.inc

_emu_init_start segment word public 'EMU'
_emu_init_start ends

_emu_init_end segment word public 'EMU'
_emu_init_end ends

DGROUP group _emu_init_start,_emu_init_end

        modstart        fpe87

        datasegment

        extrn   __8087          : byte
ifdef   __MT__
        extrn   "C",_threadid   : dword
        extrn   "C",__ThreadData: word
else
        extrn   "C",_STACKLOW   : word
endif
        extrn   "C",__FPE_handler: dword
ifndef  __OS2__
        extrn   "C",__FPE_int   : byte  ; defined in \clib\math\c\fpeint.c
endif

TInf    db 00h,00h,00h,00h,00h,00h,00h,80h,0ffh,7fh
F8Inf   db 0ffh,0ffh,0ffh,0ffh,0ffh,0ffh,0efh,7fh
F4Inf   db 0ffh,0ffh,7fh,7fh
        enddata

        bss_segment
STACK_SIZE equ 512
        db      (STACK_SIZE) dup(?)
FPEStk  label byte
SaveSS  dw      1 dup(?)
SaveSP  dw      1 dup(?)
        endbss

ifndef  __OS2__
Save87  dw      0
        dw      0
endif

        ifndef  __OS2__
            xdefp       "C",__Init_FPE_handler
            xdefp       "C",__Fini_FPE_handler
        endif

ifndef  __OS2__

defp    __Init_FPE_handler
        _guess                          ; guess initialization required
          cmp   word ptr CS:Save87+2,0  ; - quit if already initialized
          _quif ne                      ; - ...
          cmp   byte ptr __8087,0       ; - quit if no 80x87 present
          _quif e                       ; - ...
          push  BP                      ; - save registers
          push  AX                      ; - save registers
          push  BX                      ; - ...
          push  DX                      ; - ...
          push  DS                      ; - ...
          push  ES                      ; - ...
          mov   AH,35h                  ; - get old interrupt handler
          mov   AL,__FPE_int            ; - for INT 2 (INT 10 on NEC)
          int   21h                     ; - ...
          mov   CS:Save87,BX            ; - save old interrupt handler
          mov   CS:Save87+2,ES          ; - ...
          mov   AH,25h                  ; - set new interrupt handler
          mov   AL,__FPE_int            ; - for INT 2 (INT 10 on NEC)
          push  CS                      ; - set DS:DX to address of new handler
          pop   DS                      ; - ...
          mov   DX,offset __FPEHandler  ; - ...
          int   21h                     ; - ...
          pop   ES                      ; - restore registers
          pop   DS                      ; - ...
          pop   DX                      ; - ...
          pop   BX                      ; - ...
          pop   AX                      ; - ...
          pop   BP                      ; - ...
        _endguess                       ; endguess
        ret
endproc __Init_FPE_handler

defp    __Fini_FPE_handler
        _guess                          ; guess handler was initialized
          cmp   word ptr CS:Save87+2,0  ; - quit if not initialized
          _quif e                       ; - ...
          push  AX                      ; - save registers
          push  DX                      ; - ...
          push  DS                      ; - ...
          push  BP                      ; - save BP
          sub   SP,2                    ; - allocate space for control word
          mov   BP,SP                   ; - point to space for control word
          fstcw word ptr [BP]           ; - get control word
          fwait                         ; - ...
          mov   byte ptr [BP],7Fh       ; - disable exception interrupts
          fldcw word ptr [BP]           ; - ...
          fwait                         ; - ...
          add   SP,2                    ; - remove temporary
          mov   AH,25h                  ; - restore old interrupt handler
          mov   AL,__FPE_int            ; - for INT 2 (INT 10 on NEC)
          mov   DS,CS:Save87+2          ; - get address of old handler
          mov   DX,CS:Save87            ; - ...
          int   21h                     ; - ...
          pop   BP                      ; - restore BP
          pop   DS                      ; - restore registers
          pop   DX                      ; - ...
          pop   AX                      ; - ...
        _endguess                       ; endguess
        ret
endproc __Fini_FPE_handler

endif

; Interrupt handler for 80x87 exceptions.

        xdefp   __FPEHandler
__FPEHandler proc far
        push    BP                      ; allocate room on stack for 80x87
        sub     SP,ENV_SIZE             ; ... environment information
        mov     BP,offset DGROUP:_emu_init_end   ; see if we have an emulator
        cmp     BP,offset DGROUP:_emu_init_start ; ...
        mov     BP,SP                   ; point to buffer for 80x87 environment
        _if     ne                      ; if we have an emulator
        fstenv [BP]                     ; - get 80x87 environment
        _else                           ; else
        fnstenv [BP]                    ; - get 80x87 environment
        _endif                          ; endif
        fwait                           ; wait for 80x87
ifndef  __OS2__
        test    word ptr ENV_SW[BP],ST_EF_ES
                                        ; check if 80x87 interrupted
        _if     e                       ; if interrupt not caused by 80x87
          add   SP,ENV_SIZE             ; - restore stack
          pop   BP                      ; - ...
          push  CS:Save87+2             ; - invoke old interrupt handler
          push  CS:Save87               ; - ...
          ret                           ; - ...
        _endif                          ; endif
endif
        push    AX                      ; save regs
        push    BX                      ; ...
        push    CX                      ; ...
        push    DX                      ; ...
        push    SI                      ; ...
        push    DI                      ; ...
        push    DS                      ; ...
        push    ES                      ; ...
        ;fclex                          ; clear exceptions
        fdisi                           ; disable interrupts
ifndef  __OS2__
        sti                             ; enable CPU interrupts
endif
        mov     AX,seg DGROUP           ; get data segment
        mov     DS,AX                   ; ...
        mov     DX,ENV_CW[BP]           ; get control word
        not     DX                      ; flip bits to get mask
        mov     DH,0FFh                 ; want to keep high bits
ifdef __OS2__
        mov     CX,ENV_SIZE+2[BP]       ; get status word that OS/2 pushed
        mov     ES,ENV_IP+2[BP]         ; get instruction pointer
        mov     DI,ENV_IP+0[BP]         ; ...
        _loop                           ; loop (skip over prefix bytes)
           mov    BX,ES:[DI]            ; - get opcode
           and    BL,0F8h               ; - see if 8087 opcode
           cmp    BL,0D8h               ; - quit if it is
           _quif  e                     ; - ...
           inc    DI                    ; - point to next byte
        _endloop                        ; endloop
        mov     BX,ES:[DI]              ; get opcode
else
        mov     CX,ENV_SW[BP]           ; get status word from fstenv
        mov     BX,ENV_IP+2[BP]         ; get op code
        and     BH,7                    ; ...
        or      BH,0D8h                 ; ...
endif
        and     DX,CX                   ; get status word
        mov     ES,ENV_OP+2[BP]         ; get pointer to operand
        mov     DI,ENV_OP[BP]           ; ...
        mov     CX,FPE_OK               ; assume unrecognizeable error
        _guess                          ; guess precision exception
          test  DL,ST_EF_PR             ; - check for precision exception
          _quif e                       ; - quit if not precision exception
          mov   CX,FPE_INEXACT          ; - indicate precision exception
        _admit                          ; guess invalid operation
          test  DL,ST_EF_IO             ; - check for invalid operation
          _quif e                       ; - quit if not invalid operation
          call  InvalidOp               ; - process invalid operation
        _admit                          ; guess denormal operand
          test  DL,ST_EF_DO             ; - check for denormal operand
          _quif e                       ; - quit if not denormal operand
          mov   CX,FPE_DENORMAL         ; - indicate underflow
        _admit                          ; guess overflow
          test  DL,ST_EF_OF             ; - check for overflow
          _quif e                       ; - quit if not overflow
          call  KOOverFlow              ; - process overflow exception
          mov   CX,FPE_OVERFLOW         ; - set floating point error code
        _admit                          ; guess underflow
          test  DL,ST_EF_UF             ; - check for underflow
          _quif e                       ; - quit if not underflow
          mov   CX,FPE_UNDERFLOW        ; - indicate underflow
        _admit                          ; guess divide by 0
          test  DL,ST_EF_ZD             ; - check for divide by zero
          _quif e                       ; - quit if not divide by zero
          mov   CX,FPE_ZERODIVIDE       ; - indicate divide by zero
        _admit                          ; guess stack under/overflow
          test  DL,ST_EF_SF             ; - check for stack under/overflow
          _quif e                       ; - quit if not stack under/overflow
          test  DX,ST_C1                ; - check if underflow
          _if   e                       ; - if underflow
            mov CX,FPE_STACKUNDERFLOW   ; - - indicate stack underflow
          _else                         ; - else
            mov CX,FPE_STACKOVERFLOW    ; - - indicate stack overflow
          _endif                        ; - endif
        _endguess                       ; endguess
        _guess          ok              ; guess exception to be handled
          test  CX,CX                   ; - check if exception allowed
          _quif e                       ; - quit if exception not allowed
          mov   AX,offset DGROUP:FPEStk - STACK_SIZE; - get new stack bottom
          mov   BX,DS                   ; - get new value for SS:SP
          mov   DX,offset DGROUP:FPEStk ; - ...
          mov   SI,SS                   ; - get current SS:SP
          mov   DI,SP                   ; - ...
          cmp   BX,SI                   ; - if might be using the FPEStk
          _if   e                       ; - then
            cmp   DI,DX                 ; - - check stack pointer
            _if   be                    ; - - if <= stack top
              cmp   DI,AX               ; - - - check with stack bottom
              _quif ae ,  ok            ; - - - quit if FPEStk in use
            _endif                      ; - - endif
          _endif                        ; - endif
          mov   SaveSS,SS               ; - save current stack pointer
          mov   SaveSP,SP               ; - ...
          mov   SS,SI                   ; - get new stack pointer
          mov   SP,DX                   ; - ...
ifdef __MT__
          les   si,_threadid            ; - get thread id
          mov   si,es:[si]              ; - ...
          shl   si,1                    ; - turn into index
          shl   si,1                    ; - ...
          add   si,__ThreadData         ; - get pointer to thread data
          mov   es,__ThreadData+2       ; - ...
          les   si,[si]                 ; - ...
          xchg  ax,es:[si]              ; - set new stack low, and get old one
          push  ax                      ; - save current stack low
          mov   ax,cx                   ; - set floating point status
          call  __FPE_handler           ; - call user's handler
          pop   es:[si]                 ; - restore stack low

else
          push  _STACKLOW               ; - save current stack low
          mov   _STACKLOW,AX            ; - set new stack low
          mov   AX,CX                   ; - set floating point status
          call  __FPE_handler           ; - call user's handler
          mov   AX,seg DGROUP           ; - get data segment again because
          mov   DS,AX                   ; - __FPE_handler may have changed DS
          pop   _STACKLOW               ; - restore old stack low value
endif
          mov   SS,SaveSS               ; - restore stack pointer
          mov   SP,SaveSP               ; - ...
        _endguess                       ; endguess
        fclex                           ; clear exceptions that may have
                                        ; occurred as a result of handling the
                                        ; exception
        and     word ptr ENV_CW[BP],0FF72h
        fldcw   word ptr ENV_CW[BP]     ; enable interrupts
        pop     ES                      ; restore registers
        pop     DS                      ; ...
        pop     DI                      ; ...
        pop     SI                      ; ...
        pop     DX                      ; ...
        pop     CX                      ; ...
        pop     BX                      ; ...
        pop     AX                      ; ...
        fwait                           ; make sure 80x87 is ready
        add     SP,ENV_SIZE             ; clean up stack
        pop     BP                      ; ...
ifdef __OS2__
        add     SP,2                    ; OS/2 stored the FP status word on stack!
endif
        iret                            ; return from interrupt handler
endproc __FPEHandler


; Process invalid operation.

InvalidOp proc near
        mov   CX,FPE_INVALID            ; assume invalid operation
        _guess                          ; guess: it's square root
          cmp   BX,0d9fah               ; - ...
          _quif ne                      ; - quit if it's not that instruction
          mov   CX,FPE_SQRTNEG          ; - indicate divide by zero
        _admit                          ; guess: 'fprem' instruction
          cmp   BX,0D9F8h               ; - check for 'fprem'   10-may-90
          _if   ne                      ; - if not 'fprem'
            cmp   BX,0D9F5h             ; - - check for 'fprem1'
          _endif                        ; - endif
          _quif ne                      ; - quit if not 'fprem' or 'fprem1'
        _admit                          ; guess: integer overflow
          mov   DX,BX                   ; - save op code
          and   DX,0310h                ; - check for fist/fistp instruction
          cmp   DX,0310h                ; - ...
          _quif ne                      ; - quit if its not that instruction
          call  KOIntOFlow              ; - process integer overflow exception
        _admit                          ; guess: it's floating point underflow
          mov   DX,BX                   ; - save op code
          and   DX,0110h                ; - check if fst or fstp instruction
          cmp   DX,0110h                ; - ...
          _quif ne                      ; - quit if it's not that instruction
; Destination is short or long real and source register is an unnormal
; with exponent in range.
          fstp  ST(0)                   ; - pop old result
          fldz                          ; - load zero
          mov   DX,BX                   ; - save op code
          and   DX,00C0h                ; - check the MOD bits of instruction
          cmp   DX,00C0h                ; - ...
          _if   ne                      ; - if result to be placed in memory
            call Store                  ; - - store result in memory
          _endif                        ; - endif
          test  BX,0008h                ; - check if result to be popped
          _if   ne                      ; - if result to be popped
            fstp ST(0)                  ; - - pop the result
          _endif                        ; - endif
          mov   CX,FPE_UNDERFLOW        ; - indicate underflow
        _admit                          ; guess: it's divide
          mov   DX,BX                   ; - save op code
          and   DX,0130h                ; - check for fdiv/fidiv instruction
          cmp   DX,0030h                ; - ...
          _quif ne                      ; - quit if it's not that instruction
          mov   DX,ENV_TW[BP]           ; - get tag word
          mov   CL,AH                   ; - get stack pointer
          and   CL,38h                  ; - ...
          shr   CL,1                    ; - ...
          shr   CL,1                    ; - ...
          ror   DX,CL                   ; - make stack top low order bits
          and   DX,0005h                ; - check if top two elements are 0
          cmp   DX,0005h                ; - ...
          _quif ne                      ; - quif if they are not 0
          mov   CX,FPE_ZERODIVIDE       ; - indicate divide by zero
        _endguess                       ; endguess
        ret
endproc InvalidOp


;    Process integer overflow exception.

KOIntOFlow proc near
        push  BX                ; save BX
        push  DI                ; save DI
        push  BP                ; save base pointer
        sub   SP,10             ; allocate temporary
        mov   BP,SP             ; point to temporary
        fld   ST(0)             ; duplicate result
        fstp  tbyte ptr [BP]    ; get value
        fwait                   ; wait for store to complete
        pop   DX                ; get value into registers
        pop   CX                ; ...
        pop   BX                ; ...
        pop   AX                ; ...
        pop   DI                ; ...
        pop   BP                ; restore base pointer
        mov   SI,DI             ; save sign
        and   DI,7fffh          ; clear sign bit
        sub   DI,3ffeh          ; remove bias
        neg   DI                ; compute 64 - exponent
        add   DI,64             ; ...
        _loop                   ; loop
          or    DI,DI           ; - check for 0 shift count
          _quif le              ; - quit if shift count 0
          shr   AX,1            ; - shift mantissa into integer
          rcr   BX,1            ; - ...
          rcr   CX,1            ; - ...
          rcr   DX,1            ; - ...
          dec   DI              ; - decrement shift count
        _endloop                ; endloop
        _loop                   ; loop
          or    DI,DI           ; - check for 0 shift count
          _quif ge              ; - quit if shift count 0
          shl   DX,1            ; - shift mantissa into integer
          rcl   CX,1            ; - ...
          rcl   BX,1            ; - ...
          rcl   AX,1            ; - ...
          inc   DI              ; - decrement shift count
        _endloop                ; endloop
        or    SI,SI             ; check sign
        _if   s                 ; if negative
          not   CX              ; - negate integer
          neg   DX              ; - ...
          sbb   CX,-1           ; - ...
        _endif                  ; endif
        pop   DI                ; restore DI
        mov   ES:[DI],DX        ; store low order 16 bits
        pop   BX                ; restore BX
        test  BX,0400h          ; check if 32 or 16 bit integer
        _if   e                 ; if 32 bit integer
          mov   ES:2[DI],CX     ; - store high order 16 bits
        _endif                  ; endif
        test  BX,0008h          ; check if result to be popped
        _if   ne                ; if 32 bit integer
          fstp ST(0)            ; - pop the result
        _endif                  ; endif
        ret                     ; return
endproc KOIntOFlow


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
          mov   DX,BX           ; - save op code
          and   DX,00C0h        ; - check the MOD bits of instruction
          cmp   DX,00C0h        ; - ...
          _if   ne              ; - if result to be placed in memory
            call Store          ; - - store infinity
          _endif                ; - endif
          test  BX,0008h        ; - check if result to be popped
          _if   ne              ; - if result to be popped
            fstp ST(0)          ; - - pop result
          _endif                ; - endif
        _admit                  ; admit arithmetic operation
          mov   DX,BX           ; - save op code
          and   DX,00C0h        ; - check if both operands on stack
          cmp   DX,00C0h        ; - ...
          _quif ne              ; - quif both operands not on stack
;
; This code handles overflow on the following intructions:
;    fxxx   ST,ST(i)
;    fxxx   ST(i),ST    where xxx is one of mul,div,sub or add
;    fxxxp  ST(i),ST
;
          mov   SI,offset DGROUP:TInf ; - load internal infinity
          call Load             ; - ...
        _admit                  ; admit
;
; This admit block is to handle overflow on the following intructions:
;    fxxx   short real
;    fxxx   long real   where xxx is one of mul,div,sub or add
;
          call GetInf           ; - load infinity
        _endguess               ; endguess
        ret                     ; return
endproc KOOverFlow


; Replace the top element of the stack with the appropriate signed
; infinity.

GetInf  proc    near
        ftst                    ; get sign of result
        fstsw word ptr ENV_OP[BP]
        fstp  ST(0)             ; pop argument off stack (does fwait)
        test  BX,0400h          ; check if single or double
        _if   ne                ; if double
          fld qword ptr F8Inf   ; - load single precision infinity
        _else                   ; else
          fld dword ptr F4Inf   ; - load single precision infinity
        _endif                  ; endif
        test  word ptr ENV_OP[BP],ST_C0
        _if   ne                ; if argument is negative
          fchs                  ; - return negative infinity
        _endif                  ; endif
        ret                     ; return
endproc GetInf


; Replace an element on the stack with internal zero or infinity.

Load    proc    near
        test  BX,0400h          ; check if result is top element
        _if   e                 ; if result is not top element
          sub   DX,DX           ; - indicate we are at the top
        _else                   ; else
          mov   DX,BX           ; - get st(i)
          and   DX,0007h        ; - . . .
        _endif                  ; endif
        push  DX                ; save st(i)
        _loop                   ; loop
          dec DX                ; - decrement counter
          or  DX,DX             ; - are we at st(i)?
          _quif e               ; - quit if we are at st(i)
          fincstp               ; - increment stack pointer
        _endloop                ; endloop
        ffree ST(0)             ; free the stack element
        fld   tbyte ptr [SI]    ; load internal zero
        pop   DX                ; get st(i)
        _loop                   ; loop
          dec DX                ; - decrement counter
          or  DX,DX             ; - are we at st(i)?
          _quif e               ; - quit if we are at st(i)
          fdecstp               ; - decrement stack pointer
        _endloop                ; endloop
        ret                     ; return
endproc Load


; Store the top element of the stack at ES:DI.

Store   proc    near
        test  BX,0400h
        _if   ne                ; if double
          fst  qword ptr ES:[DI]; - store zero
        _else                   ; else
          fst  dword ptr ES:[DI]; - store zero
        _endif                  ; endif
        ret                     ; return
endproc Store

        endmod
        end
