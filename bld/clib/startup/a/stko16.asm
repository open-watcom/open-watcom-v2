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
;* Description:  Stack checking for 16-bit OS/2.
;*
;*****************************************************************************


include mdef.inc
include struct.inc
include exitwmsg.inc

        .286C

ifdef __MT__
        extrn   "C",_threadid       : dword
        extrn   "C",__ThreadData    : word
else
        extrn   "C",_STACKLOW       : word
endif

        modstart        stk

CONST   segment word public 'DATA'
msg     db      "Stack Overflow!", 0
CONST   ends

_DATA   segment word public 'DATA'
_DATA   ends

        DGROUP  group   CONST, _DATA
        assume  ds:DGROUP

        xdefp   __STK
        xdefp   "C",__STKOVERFLOW

dgroupp dw      DGROUP

        defpe   __STK
        _guess                          ; guess: no overflow
        cmp     ax,sp                   ; - quit if user asking for too much
        _quif   ae                      ; - . . .
        sub     ax,sp                   ; - calculate new low point
        neg     ax                      ; - calc what new SP would be
ifdef __MT__
        push    ds                      ; - save registers
        push    si                      ; - ...
        mov     ds,cs:dgroupp           ; - . . .
        lds     si,_threadid            ; - get thread id
        mov     si,[si]                 ; - ...
        shl     si,1                    ; - turn into index
        shl     si,1                    ; - ...
        mov     ds,cs:dgroupp           ; - . . .
        add     si,__ThreadData         ; - get pointer to thread data
        mov     ds,__ThreadData+2       ; - ...
        lds     si,[si]                 ; - ...
        cmp     ax,[si]                 ; - quit if too much
        pop     si                      ; - restore registers
        pop     ds                      ; - ...
else
    if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    ds                      ; - save ds
        mov     ds,cs:dgroupp           ; - load ds from DGROUP
    endif
        cmp     ax,ds:_STACKLOW         ; - quit if too much
    if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     ds                      ; - restore ds
    endif
endif
        _quif   be                      ; - . . .
        ret                             ; - return
        _endguess                       ; endguess

__STKOVERFLOW:
        mov     dx,cs:dgroupp           ; set stk overflow msg
        mov     ax, offset DGROUP:msg
        mov     bx, 01h                 ; return error
        jmp     __fatal_runtime_error   ; arrrggg....
        endproc __STK

        endmod
        end
