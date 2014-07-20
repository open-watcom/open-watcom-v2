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
; DIVC16        : complex*16 divide
;

.8087

include struct.inc
include mdef.inc

        modstart        divc16

        xrefp   CplxDivide


        xdefp   RT@C16DIV
defp    RT@C16DIV
        sub     ESP,20                  ; allocate save area for fp registers
        fxch    st(4)                   ; save floating-point registers
        fstp    tbyte ptr 10[ESP]       ; ...
        fxch    st(4)                   ; ...
        fstp    tbyte ptr [ESP]         ; ...
        call    CplxDivide              ; do divide
        fstp    qword ptr [ESI]         ; return complex result
        fstp    qword ptr 8[ESI]        ; ...
        fld     tbyte ptr [ESP]         ; restore floating-point registers
        fld     tbyte ptr 10[ESP]       ; ...
        add     ESP,20                  ; clean up stack
        ret
endproc RT@C16DIV

        endmod
        end
