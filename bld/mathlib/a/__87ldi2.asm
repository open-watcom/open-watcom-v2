;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


;
;  Name:        __87LDI2
;  Operation:   convert float in ST(0) to 32-bit integer
;  Inputs:      ST(0)   float
;  Outputs:     EAX     32-bit
;               AX      16-bit
;  Volatile:    none
;

include mdef.inc

        xrefp           __8087  ; indicate that NDP instructions are present

        modstart __87LDI2

        xdefp   __87LDI2

        defp    __87LDI2
ifdef __386__
        push    EAX                     ; allocate temporary
        fstcw   word ptr [ESP]          ; get 8087 control word
        fwait                           ; wait until store complete
        push    dword ptr [ESP]         ; remember old control word
        mov     byte ptr 4+1[ESP],1Fh   ; set control word to truncate
        fldcw   word ptr 4[ESP]         ; load new control word
        fistp   word ptr 4[ESP]         ; store value as integer
        fldcw   word ptr [ESP]          ; restore old control word
        fwait                           ; wait until load complete
        pop     EAX                     ; remove control word
        pop     EAX                     ; retrieve value
        cwde                            ; sign extend value
else
        push    BP                      ; allocate 2 bytes on stack
        mov     BP,SP                   ; get access to stack
        push    AX                      ; allocate temporary
        push    AX                      ; allocate temporary
        fstcw   -4[BP]                  ; get 8087 control word
        fwait                           ; wait until store complete
        mov     AX,-4[BP]               ; remember old control word
        mov     byte ptr -3[BP],1Fh     ; set control word to truncate
        fldcw   -4[BP]                  ; load new control word
        fistp   word ptr -2[BP]         ; store value as 16-bit integer
        mov     -4[BP],AX               ; restore old 8087 control word
        fldcw   -4[BP]                  ; ...
        fwait                           ; wait until load complete
        pop     AX                      ; remove control word
        pop     AX                      ; load 16-bit integer value
        pop     BP                      ; restore BP
endif
        ret                             ; return
        endproc __87LDI2

        endmod
        end
