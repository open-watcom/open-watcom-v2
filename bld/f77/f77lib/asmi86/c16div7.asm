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


;
; C16DIV7       : double precision complex divide
;

.8087

include struct.inc
include mdef.inc

        modstart        c16div7

        xref            CplxDivide


        xdefp   RT@C16DIV
defp    RT@C16DIV
        push    BP                      ; save registers
        sub     SP,20                   ; allocate save area for fp registers
        mov     BP,SP                   ; set up base register
        fxch    st(4)                   ; save floating-point registers
        fstp    tbyte ptr 10[BP]        ; ...
        fxch    st(4)                   ; ...
        fstp    tbyte ptr [BP]          ; ...
        call    CplxDivide              ; do divide
        fstp    qword ptr [SI]          ; return complex result
        fstp    qword ptr 4[SI]         ; ...
        fld     tbyte ptr [BP]          ; restore floating-point registers
        fld     tbyte ptr 10[BP]        ; ...
        add     SP,20                   ; clean up stack
        pop     BP                      ; restore registers
        ret
endproc RT@C16DIV

        endmod
        end
