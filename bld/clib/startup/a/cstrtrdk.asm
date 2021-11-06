;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
;* Description:  Startup code for RDOS, 32-bit device-driver.
;*
;*****************************************************************************

.387
.386p

include langenv.inc
include xinit.inc

        assume  nothing

        extrn   __RdosMain              : near
        extrn   __FiniRtns              : near

        assume  nothing

DGROUP group _NULL,_AFTERNULL,CONST,_DATA,DATA,XIB,XI,XIE,YIB,YI,YIE

BEGTEXT segment use32 word public 'CODE'
        assume  cs:BEGTEXT
forever label   near
        int     3h
        jmp short forever
        ; NOTE that ___begtext needs to be at offset 3
        ; don't move it.  i.e. don't change any code before here.
        public ___begtext
___begtext label byte
        nop
        nop
        nop
        nop
        assume  cs:nothing
BEGTEXT ends

_NULL   segment para public 'DATA'
__nullarea label word
        db      01h,01h,01h,00h
        public  __nullarea

_NULL   ends

_AFTERNULL segment word public 'DATA'
end_null dw      0                       ; nullchar for string at address 0
_AFTERNULL ends

CONST   segment word public 'DATA'
NullAssign      db      '*** NULL assignment detected',0
CONST   ends

_DATA   segment use32 public 'DATA'

ExitSs          dw 0
ExitEsp         dd 0

_DATA   ends

DATA    segment word public 'DATA'
DATA    ends

_TEXT   segment use32 word public 'CODE'

        assume  cs:_TEXT

public _cexit_

_cexit_ proc near
    mov edx,SEG _DATA
    mov ds,edx
    mov dx,ds:ExitSs
    or dx,dx
    jz _exit_done
;
    mov ss,dx
    mov esp,ds:ExitEsp
    mov ds:ExitSs,0
    or eax,eax
    clc
    jz _exit_far
;
    stc
_exit_far:
    retf

_exit_done:
    ret
_cexit_ endp

public _cstart_

_cstart_ proc  near
    movzx esp,sp
    mov eax,SEG _DATA
    mov ds,eax
    mov ds:ExitSs,ss
    mov ds:ExitEsp,esp

    sub ebp,ebp                 ; ebp=0 indicate end of ebp chain
    call __RdosMain
    jmp _cexit_
_cstart_ endp

    dd ___begtext               ; make sure dead code elimination
                                ; doesn't kill BEGTEXT segment

;
; copyright message
;
include msgcpyrt.inc

_TEXT   ends

        end     _cstart_
