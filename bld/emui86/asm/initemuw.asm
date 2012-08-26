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
;* Description:  Init 16-bit Windows FPU emulation
;*
;*****************************************************************************

.8087

include struct.inc
include mdef.inc
include xinit.inc

public  FJSRQQ
FJSRQQ  equ             08000H
public  FISRQQ
FISRQQ  equ             00632H
public  FIERQQ
FIERQQ  equ             01632H
public  FIDRQQ
FIDRQQ  equ             05C32H
public  FIWRQQ
FIWRQQ  equ             0A23DH
public  FJCRQQ
FJCRQQ  equ             0C000H
public  FJARQQ
FJARQQ  equ             04000H
public  FICRQQ
FICRQQ  equ             00E32H
public  FIARQQ
FIARQQ  equ             0FE32H

        xinit   __init_87_emulator,INIT_PRIORITY_FPU
        xfini   __fini_87_emulator,INIT_PRIORITY_FPU

DGROUP  group   _DATA
        assume  ds:DGROUP

_DATA segment word public 'DATA'
        extrn   __8087 : byte
        extrn   __real87 : byte
_DATA ends

_TEXT segment word public 'CODE'

        extrn   __FPMATH        : far
        extrn   GETWINFLAGS     : far
        extrn   __init_8087_emu : near
        extrn   __x87id         : near
        extrn   __raise_fpe_    : near

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _init_87_emulator( void )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

public  __init_87_emulator
__init_87_emulator proc
        push    ds                      ; save ds
        push    dx                      ; save dx
        push    cx                      ; save cx
        push    bx                      ; save bx
        sub     bx,bx                   ; initialize Windows FPMATH support
        mov     ax,offset _userfpe        ; ...
        mov     dx,cs                   ; ...
        call    __FPMATH                ; ...
        mov     ax,offset _userfpe        ; ...
        mov     dx,cs                   ; ...
        mov     bx,3                    ; ...
        call    __FPMATH                ; ...

        call    __x87id                 ; get the 80x87 type
        mov     byte ptr __8087,al      ; 
        call    __init_8087_emu         ; initialize the 80x87
                                        ; at this point we can't tell the real
                                        ; thing from the fake since emulator is
                                        ; hooked in, ask windows if there is
                                        ; a coprocessor
        call    far ptr GETWINFLAGS
        test    ah,04H
        je      no_copro
        mov     al, byte ptr __8087     ; get coprocessor value
        jmp     set_real
no_copro:
        mov     al, 0
set_real:
        mov     byte ptr __real87,al    ; assume we have a real 80x87 present
        pop     bx                      ; restore bx
        pop     cx                      ; restore cx
        pop     dx                      ; restore dx
        pop     ds                      ; restore ds
        ret                             ; return to caller
__init_87_emulator endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _fini_87_emulator( void )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

public __fini_87_emulator
__fini_87_emulator proc
        push    dx                      ; save dx
        push    cx                      ; save cx
        push    bx                      ; save bx
        mov     bx,2                    ; finalize Windows FPMATH support
        call    __FPMATH                ; ...
        pop     bx                      ; restore bx
        pop     cx                      ; restore cx
        pop     dx                      ; restore dx
        ret                             ; return
__fini_87_emulator endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _userfpe(void)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

public _userfpe
_userfpe proc near
        push    bp              ; DS resides at (SP + 4) - Grab a copy
        mov     bp, sp
        mov     ax, [bp + 6]
        pop     bp              ; Clean up BP
        push    ds              ; Save DS
        mov     ds, ax          ; Restore app's DS
        call    __raise_fpe_    ; Call __raise_fpe()
        pop     ds
        retf
_userfpe endp

_TEXT   ends

        end
