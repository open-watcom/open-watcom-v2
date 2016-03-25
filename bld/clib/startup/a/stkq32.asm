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
;* Description:  Stack checking for 32-bit QNX.
;*
;*****************************************************************************



include mdef.inc
include struct.inc
include exitwmsg.inc

ifdef __MT__
        extrn   "C",__GetThreadPtr : dword
else
        extrn   "C",_STACKLOW : dword
endif
        extrn   "C",__qnx_exit: near

        modstart        stk

_DATA   segment dword public 'DATA'
_DATA   ends

        DGROUP  group   _DATA

        assume  ds:DGROUP

        xdefp   __STK
        xdefp   __CHK
        xdefp   __GRO
        xdefp   "C",__STKOVERFLOW


msg     db      "Stack Overflow at "
msg_end label byte
hextab  db      "0123456789ABCDEF"


        defpe   __GRO
        ret     4
        endproc __GRO

        defpe   __STK
        xchg    eax,[esp]               ; exchange parm with return addr
        push    eax                     ; push return addr
        ; fall into __CHK
        endproc __STK

        defpe   __CHK
        push    eax
        mov     eax,8[esp]
        _guess                  ; guess: no overflow
          cmp   eax,esp         ; - quit if user asking for too much
          _quif ae              ; - . . .
          sub   eax,esp         ; - calculate new low point
          neg   eax             ; - calc what new SP would be
ifdef __MT__
          push  esi             ; - save registers
          push  eax             ; - save eax
          call  __GetThreadPtr  ; - get thread data address
          mov   esi,eax         ; - ...
          pop   eax             ; - restore new ESP value
          cmp   eax,[esi]       ; - quit if too much
          pop   esi             ; - restore registers
else
          cmp   eax,_STACKLOW   ; - quit if too much
endif
          _quif be              ; - . . .
          pop   eax             ; - restore EAX
          ret   4               ; - return
        _endguess               ; endguess

        endproc __CHK

;* modified stack overflow code to print the return address of the
;* caller.

__STKOVERFLOW:

        pop     eax             ; throw away saved eax value
        pop     eax             ; get return addr
        xor     edx,edx
        mov     dx,cs
        add     esp,0100h       ; cream last 256 bytes of stack
        mov     ebp,esp

        push    eax             ; push return address, segment first
        push    edx             ; to make it easier to print it


        push    ss              ; setup string instrs to copy
        pop     es              ; exit message onto stack
        cld
        mov     edi,ebp

        mov     esi,offset msg
        mov     ecx,offset msg_end
        sub     ecx,esi
        push    cs
        pop     ds
        rep     movsb

        pop     ebx             ; get segment
        call    _putw
        mov     al,':'
        stosb
        mov     ebx,[esp]       ; get offset
        shr     ebx,16          ; print high word
        call    _putw
        pop     ebx             ; get offset
        call    _putw           ; print low word
        xor     al,al
        stosb
        push    ss
        pop     ds
ifdef __STACK__
        push    1
        push    ebp
else
        mov     eax,ebp
        mov     edx,1
endif
        call    __fatal_runtime_error
        ; never return

_putw proc near
        mov     edx,ebx         ; save value
        mov     cl,12           ; setup shift count
_lup:
        mov     ebx,edx         ; get value
        shr     ebx,cl          ; put in bottom 4 bits.
        and     ebx,0fh
        mov     al,cs:hextab[ebx]
        stosb
        sub     cl,4
        jns     _lup
        ret
_putw   endp


        endmod
        end
