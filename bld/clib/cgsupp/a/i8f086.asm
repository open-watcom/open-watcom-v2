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



;  Note; These routines need to be fixed since they don't support 64-bit
;       integers.  Thee int to float routines copy 64-bit to 32-bit
;       thereby chopping off the high-order 32 bits. The float to int routines
;       convert floats to 32-bit ints and then convert up.

include mdef.inc
include struct.inc

        modstart        i8f086

        xref    __U4FS
        xref    __I4FS
        xref    __U4FD
        xref    __I4FD

        xref    __FSI4
        xref    __FSU4
;       xref    __RSI4
;       xref    __RSU4

        xref    __FDI4
        xref    __FDU4
        xref    __RDI4
        xref    __RDU4

; Convert signed 64-bit integer to single precision float
; Input: [AX BX CX DX] = 64-bit integer
; Output: [DX AX] = 32-bit float

        xdefp   __I8FS
        defp    __I8FS
        mov     ax,dx           ; cheat
        mov     dx,cx
        jmp     __I4FS
        endproc __I8FS

; Convert unsigned 64-bit integer to single precision float
; Input: [AX BX CX DX] = 64-bit integer
; Output: [DX AX] = 32-bit float

        xdefp   __U8FS
        defp    __U8FS
        mov     ax,dx           ; cheat
        mov     dx,cx
        jmp     __U4FS
        endproc __U8FS

; Convert signed 64-bit integer to double precision float
; Input: [AX BX CX DX] = 64-bit integer
; Output: [AX BX CX DX] = 64-bit float

        xdefp   __I8FD
        defp    __I8FD
        mov     ax,dx           ; cheat
        mov     dx,cx
        jmp     __I4FD
        endproc __I8FD

; Convert unsigned 64-bit integer to double precision float
; Input: [AX BX CX DX] = 64-bit integer
; Output: [AX BX CX DX] = 64-bit float

        xdefp   __U8FD
        defp    __U8FD
        mov     ax,dx           ; cheat
        mov     dx,cx
        jmp     __U4FD
        endproc __U8FD

; Convert single precision float to unsigned 64-bit integer with truncation
; Input: [DX AX] = 32-bit float
; Output: [AX BX CX DX] = 64-bit integer

        xdefp   __FSU8
        defp    __FSU8
        call    __FSU4          ; cheat
        mov     cx,dx
        mov     dx,ax
        xor     bx,bx
        xor     ax,ax
        ret
        endproc __FSU8

; Convert single precision float to signed 64-bit integer with truncation
; Input: [DX AX] = 32-bit float
; Output: [AX BX CX DX] = 64-bit integer

        xdefp   __FSI8
        defp    __FSI8
        call    __FSI4          ; cheat
        mov     bx,ax           ; save low-order part of 16-bit result
        mov     ax,dx           ; copy high-order part of 16-bit result
        cwd                     ; sign extend ax into dx
        mov     cx,ax           ; set high-order part of 16-bit result
        mov     ax,dx           ; set extended sign
        mov     dx,bx           ; recover low-order part of 16-bit result
        mov     bx,ax           ; set extended sign
        ret
        endproc __FSI8

; Convert single precision float to unsigned 64-bit integer with rounding
; Input: [DX AX] = 32-bit float
; Output: [AX BX CX DX] = 64-bit integer

;       xdefp   __RSU8
;       defp    __RSU8
;       call    __RSU4
;       mov     cx,dx
;       mov     dx,ax
;       xor     bx,bx
;       xor     ax,ax
;       ret
;       endproc __RSU8

; Convert single precision float to signed 64-bit integer with rounding
; Input: [DX AX] = 32-bit float
; Output: [AX BX CX DX] = 64-bit integer

;       xdefp   __RSI8
;       defp    __RSI8
;       call    __RSI4
;       mov     bx,ax
;       mov     ax,dx
;       cwd
;       mov     cx,ax
;       mov     ax,dx
;       mov     dx,bx
;       mov     bx,ax
;       ret
;       endproc __RSI8

; Convert double precision float to unsigned 64-bit integer with truncation
; Input: [AX BX CX DX] = 64-bit float
; Output: [AX BX CX DX] = 64-bit integer

        xdefp   __FDU8
        defp    __FDU8
        call    __FDU4          ; cheat
        mov     cx,dx
        mov     dx,ax
        xor     bx,bx
        xor     ax,ax
        ret
        endproc __FDU8

; Convert double precision float to signed 64-bit integer with truncation
; Input: [AX BX CX DX] = 64-bit float
; Output: [EDX, EAX] = 64-bit integer

        xdefp   __FDI8
        defp    __FDI8
        call    __FDI4          ; cheat
        mov     bx,ax           ; save low-order part of 16-bit result
        mov     ax,dx           ; copy high-order part of 16-bit result
        cwd                     ; sign extend ax into dx
        mov     cx,ax           ; set high-order part of 16-bit result
        mov     ax,dx           ; set extended sign
        mov     dx,bx           ; recover low-order part of 16-bit result
        mov     bx,ax           ; set extended sign
        ret
        endproc __FDI8

; Convert double precision float to unsigned 64-bit integer with rounding
; Input: [AX BX CX DX] = 64-bit float

; JBS: Who calls this routine?

        xdefp   __RDU8
        defp    __RDU8
        call    __RDU4          ; cheat
        mov     cx,dx
        mov     dx,ax
        xor     bx,bx
        xor     ax,ax
        ret
        endproc __RDU8

; Convert double precision float to signed 64-bit integer with rounding
; Input: [AX BX CX DX] = 64-bit float

; JBS: Who calls this routine?

        xdefp   __RDI8
        defp    __RDI8
        call    __RDI4          ; cheat
        mov     bx,ax           ; save low-order part of 16-bit result
        mov     ax,dx           ; copy high-order part of 16-bit result
        cwd                     ; sign extend ax into dx
        mov     cx,ax           ; set high-order part of 16-bit result
        mov     ax,dx           ; set extended sign
        mov     dx,bx           ; recover low-order part of 16-bit result
        mov     bx,ax           ; set extended sign
        ret
        endproc __RDI8

        endmod
        end
