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


;                               Rationale:
;                               The const to add on is (double)(LONGLONG_MAX +
;                               1), an exact power of 2. But you can skip
;                               steps i & ii and add (double)(ULONGLONG_MAX +
;                               1). You don't need to 'doctor' the integer.

;                               If the integer is (-1) 0xFFFFFFFFFFFFFFFF, it
;                               needs to wind up as ULONGLONG_MAX, so add
;                               ULONGLONG_MAX+1 If the integer is
;                               (LONGLONG_MIN) 0x8000000000000000, it needs to
;                               wind up as LONGLONG_MAX+1, so add
;                               ULONGLONG_MAX+1.
include mdef.inc
include struct.inc

.8087
        modstart        _7u8f086

        xdefp   __U8FD7
        xdefp   __U8FS7

;;;;two_to_64       dw      0000h, 0000h, 0000h, 43f0h ; removed by JBS

ULLMXP1:
        DB  00H, 00H, 80H, 5FH          ; (float)(ULONGLONG_MAX +1)
                                        ; (only exponent set)

defpe   __U8FS7
        push    bp                      ; establish frame pointer
        mov     bp,sp
        push    ax                      ; save unsigned int64 (hi)
        push    bx
        push    cx                      ; save unsigned int64 (lo)
        push    dx
        fild    qword ptr -8[bp]        ; load as int64
        test    byte ptr -1[bp],80H     ; most significant bit set?
        jns     L1
        fadd    dword ptr cs:ULLMXP1    ; correct int64 to unsigned int64 as
                                        ; as float (because expression is exact
                                        ; in powers of 2, so save 4 bytes)
L1:
        pop     ax                      ; correct stack
        pop     ax                      ; correct stack
        fstp    dword ptr -4[bp]        ; save float and pop coproc stack
        fwait
        pop     ax                      ; return float in ax/dx
        pop     dx
        pop     bp                      ; restore old frame pointer
        ret
endproc __U8FS7

defpe   __U8FD7
        push    bp                      ; establish frame pointer
        mov     bp,sp
        push    ax                      ; save unsigned int64 (hi)
        push    bx
        push    cx                      ; save unsigned int64 (lo)
        push    dx
        fild    qword ptr -8[bp]        ; load as int64
        test    byte ptr -1[bp],80H     ; most significant bit set?
        jns     L$3                     ; no, jump
        fadd    dword ptr cs:ULLMXP1    ; correct int64 to unsigned int64 as
                                        ; as float (because expression is exact
                                        ; in powers of 2, so save 4 bytes)
L$3:
        fstp    qword ptr -8[bp]        ; save double and pop coproc stack
        fwait
        pop     dx                      ; return double in dx/cx/bx/ax
        pop     cx
        pop     bx
        pop     ax
        pop     bp                      ; restore old frame pointer
        ret
endproc __U8FD7

        endmod
        end
