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


                NAME    NPXType

no87    ; get the fp emulation bit off
.386
.387

_text segment byte public 'CODE' use16
assume cs:_text

        public  Read8087_
Read8087_ proc   near
        push    ds
        push    bx
        mov     ds,dx
        mov     bx,ax
        fsave   [bx]
        frstor  [bx]
        wait
        pop     bx
        pop     ds
        ret
Read8087_ endp

        public  Write8087_
Write8087_ proc  near
        push    ds
        push    bx
        mov     ds,dx
        mov     bx,ax
        frstor  [bx]
        wait
        pop     bx
        pop     ds
        ret
Write8087_ endp

comment ~
    These routines read/write the FPU or emulator state when we're on a 386
    or better. They're a bit wierd so here's a explanation. There are operand
    size overrides on the fnsave and fnrstor so that a real FPU will save the
    full 32-bit state (this code is in a 16-bit segment). The emulator does
    not respect the operand size, but since it always stores the full state
    anyway that's OK. However, the 32-bit emulator does not know how to decode
    a 16-bit addressing mode - it always interprets things as if they were in
    the 32-bit form. That means that the [bx] addressing mode gets interpreted
    as [edi], which is why there are "movzx edi,bx" instructions. before the
    coprocessor instructions.
~

        public  Read387_
Read387_ proc   near
        push    ds
        push    bx
        push    edi
        mov     ds,dx
        mov     bx,ax
        movzx   edi,bx
        wait
        db      66H
        fnsave   [bx]
        wait
        db      66H
        ;fnrstor  [bx]
        db      0ddH, 027H      ; till wasm gets fixed up
        wait
        pop     edi
        pop     bx
        pop     ds
        ret
Read387_ endp

        public  Write387_
Write387_ proc  near
        push    ds
        push    bx
        push    edi
        mov     ds,dx
        mov     bx,ax
        movzx   edi,bx
        wait
        db      66h
        ;fnrstor  [bx]
        db      0ddH, 027H      ; till wasm gets fixed up
        wait
        pop     edi
        pop     bx
        pop     ds
        ret
Write387_ endp

_text           ENDS

                END
