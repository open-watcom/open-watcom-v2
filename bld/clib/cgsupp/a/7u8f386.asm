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


include mdef.inc
include struct.inc

.8087
        modstart        _7u8f386

        xdefp   __U8FD7
        xdefp   __U8FS7

;;;;two_to_64       dw      0000h, 0000h, 0000h, 43f0h ; removed by JBS

ULLMXP1:
        DB  00H, 00H, 80H, 5FH          ; (float)(ULONGLONG_MAX +1)
                                        ; (only exponent set)

defpe   __U8FS7
        push    edx                     ; save unsigned int64 (hi)
        push    eax                     ; save unsigned int64 (lo)
        fild    qword ptr [esp]         ; load as int64
        test    byte ptr 07H[esp],80H   ; most significant bit set?
        jns     L$2                     ; no, jump
        fadd    dword ptr cs:ULLMXP1    ; correct int64 to unsigned int64 as
                                        ; as float (because expression is exact
                                        ; in powers of 2, so save 4 bytes)
L$2:
        pop     eax                     ; correct stack
        fstp    dword ptr [esp]         ; save float and pop coproc stack
        pop     eax                     ; return float in eax
        ret
endproc __U8FS7

defpe   __U8FD7
        push    edx                     ; save unsigned int64 (hi)
        push    eax                     ; save unsigned int64 (lo)
        fild    qword ptr [esp]         ; load as int64
        test    byte ptr 07H[esp],80H   ; most significant bit set?
        jns     L$3                     ; no, jump
        fadd    dword ptr cs:ULLMXP1    ; correct int64 to unsigned int64 as
                                        ; as float (because expression is exact
                                        ; in powers of 2, so save 4 bytes)
L$3:
        fstp    qword ptr [esp]         ; save double and pop coproc stack
        pop     eax                     ; return double (lo)
        pop     edx                     ; return double (hi)
        ret
endproc __U8FD7

        endmod
        end
