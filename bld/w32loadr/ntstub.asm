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
;* Description:  Stub for 32-bit DOS programs.
;*
;*****************************************************************************

        name    ntstub

STACK   segment use16 word stack 'STACK'
        db      128 dup(?)
STACK   ends

_TEXT   segment use16 para public 'CODE'
        assume  cs:_TEXT

ntstub  proc    near
        call    L1
        db      "32-bit DOS programs require the BINW directory "
        db      "to be in your path before BINNT",0Dh,0Ah,7,"$"
L1:     pop     DX              ; get address of message
        push    CS              ; set DS=CS
        pop     DS              ; ...
        mov     AH,9            ; display string
        int     21h             ; ...
        mov     AX,4C01h        ; exit
        int     21h             ; ...
ntstub  endp

_TEXT   ends

        end     ntstub
