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

        modstart        log087
.8087
        xdefp   __log87         ; calc log(fac1)


        defp    __log87
        fldln2                  ; load ln(2)
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        push    AX              ; push argument
        push    BX              ; . . .
        push    CX              ; . . .
        push    DX              ; . . .
        fld     qword ptr -8[BP]; load argument
        fyl2x                   ; ln(x) = ln(2) * log2(x)
        fstp    qword ptr -8[BP]; store result
        fwait                   ; wait
        pop     DX              ; load result
        pop     CX              ; . . .
        pop     BX              ; . . .
        pop     AX              ; . . .
        pop     BP              ; restore BP
        ret                     ; return
        endproc __log87

        endmod
        end
