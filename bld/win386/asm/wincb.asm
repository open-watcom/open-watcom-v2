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


;****************************************************************************
;***                                                                      ***
;*** WINCB.ASM - callback handling for 32 bit windows                     ***
;***                                                                      ***
;****************************************************************************
.386p

DGROUP group _DATA

        include winext.inc

extrn   _DataSelector:WORD
extrn   _SaveSP:DWORD
extrn   _CodeEntry:FWORD
extrn   _EntryStackSave:DWORD

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
_DATA ends

_TEXT segment use16
assume cs:_TEXT
;****************************************************************************
;***                                                                      ***
;*** Invoke16BitFunction - invoke a specified function                    ***
;***                                                                      ***
;****************************************************************************

        assume  ds:DGROUP
PUBLIC  Invoke16BitFunction_
Invoke16BitFunction_ proc far
        Start16BitLand
PNum    equ  dword ptr es:[ebp+20]
PProc   equ  dword ptr es:[ebp+24]
PStart  equ  dword ptr es:[ebp+28]
        StubStackSet

        mov     ecx,PNum                ; get number of parms
        lea     ebx,PStart              ; point to start of parm list
;
; push parms on the 16-bit stack
;
        mov     di,sp                   ; save our 16-bit stack pointer
retryx:
        cmp     cl,0                    ; any parms left
        je      short docallx           ; no, go call func.
        mov     eax,dword ptr es:[ebx]  ; get size of parm
        cmp     eax,4                   ; 4 bytes?
        mov     eax,dword ptr es:[ebx+4]; get the 4 bytes
        jne     short push2x            ; no, push 2 bytes
        push    eax                     ; push 'em
        jmp     short nextonex
push2x:
        push    ax                      ; push 'em
nextonex:
        add     ebx,8                   ; next parm info
        dec     cl                      ; decrement parm count
        jmp     retryx
;
; invoke the routine
;
docallx:
        call    dword ptr PProc
;
; restore the stack
;
        mov     sp,di                   ; restore 16-bit stack pointer
        shl     edx,16                  ; set up
        and     eax,0000FFFFh           ;   the return code
        add     eax,edx                 ;     appropriately
        assume  ds:DGROUP
        StubStackRestore
        BackTo32BitLand
Invoke16BitFunction_ endp

_TEXT   ends
        end
