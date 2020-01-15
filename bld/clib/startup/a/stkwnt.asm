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
;* Description:  Stack checking for Win32.
;*
;*****************************************************************************


include langenv.inc
include mdef.inc
include struct.inc

include exitwmsg.inc

        modstart        stk

        datasegment
ifdef __MT__
        extrn   "C",__GetThreadPtr : dword
else
        extrn   "C",_STACKLOW : dword
endif
msg     db      "Stack Overflow!", 0
        enddata

        assume  ds:DGROUP

        xdefp   __CHK
        xdefp   __STK
        xdefp   __GRO
        xdefp   "C",__STKOVERFLOW
        xdefp   __alloca_probe

        defp    _init_stk
        ret                             ; return
        endproc _init_stk

        defpe   __CHK                   ; new style stack check
        xchg    eax,4[esp]              ; get parm in eax
        call    __STK                   ; call stack checker
        mov     eax,4[esp]              ; restore eax
        ret     4
        endproc __CHK

        defpe   __STK
        push    eax                     ; save parm for __GRO routine
        _guess                          ; guess: no overflow
          cmp   eax,esp                 ; - check if user asking for too much
        _quif ae                        ; quit if user asking for too much
          sub   eax,esp                 ; - calculate new low point
          neg   eax                     ; - calc what new SP would be
ifdef __MT__
          push  esi                     ; - save registers
          push  eax                     ; - save eax
          call  __GetThreadPtr          ; - get thread data address
          mov   esi,eax                 ; - ...
          pop   eax                     ; - restore new ESP value
          cmp   eax,[esi]               ; - check if too much
          pop   esi                     ; - restore registers
else
          cmp   eax,_STACKLOW           ; - check if too much
endif
        _quif be                        ; quit if too much
          call  __GRO                   ; - grow stack allocation if necessary
          ret                           ; - return
        _endguess                       ; endguess

        pop     eax                     ; pop the stack (parm for __GRO routine)

__STKOVERFLOW:
ifdef __STACK__
        push    1                       ; exit code
        push    offset msg              ; the error message
else
        mov     eax,offset msg          ; the error message
        mov     edx,1                   ; exit code
endif
        jmp     __fatal_runtime_error   ; display msg and exit
        ; never return

        endproc __STK

        defpe   __GRO                   ; dummy function to resolve symbol
        push    eax                     ; save regs
        push    ebx                     ; ...
        mov     eax,12[esp]             ; get size to grow by
        mov     ebx,-4                  ; initialiaze index
        _loop                           ; do {
          mov     [esp+ebx],ebx         ; - touch that stack page
          sub     ebx,1000H             ; - decrement by 4k
          sub     eax,1000H             ; - decrement by 4k
        _loopif g                       ; } while stack left to go
        pop     ebx                     ; restore regs
        pop     eax                     ; ...
        ret     4                       ; return to caller
        endproc __GRO

        ; on entry eax is size of stack to grow
        ; on exit esp has been advanced to new size
        defpe   __alloca_probe          ; ms compatible function
        push    eax                     ; size parm to __GRO
        call    __GRO
        push    eax                     ; save size
        lea     eax,8[esp]              ; get esp value to adjust
        sub     eax,[esp]               ; carve off piece of stack
        xchg    eax,esp                 ; set new esp
        mov     eax,4[eax]              ; snag return address
        jmp     eax                     ; return to caller
        endproc __alloca_probe

        endmod

include xinit.inc

        xinit   _init_stk,DEF_PRIORITY

        end
