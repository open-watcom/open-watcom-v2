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


include mdef.inc
include struct.inc
include math87.inc

        modstart        atanl87

        xrefp           IF@DATAN
        xrefp           IF@DATAN2

        xdefp   atanl_  ; long double atanl( long double x )
        xdefp   atan2l_ ; long double atan2l( long double y, long double x )

;  input:       x - on the stack
;  output:      arctan of x in st(0)
;
        defp    atanl_
ifdef __386__
        fld     tbyte ptr argx[ESP]; load x
        call    IF@DATAN        ; calculate atan(x)
        loadres                 ; load result
else
        push    BP              ; save BP
        mov     BP,SP           ; get access to parms
        fld     tbyte ptr argx[BP]; load argument x
        call    IF@DATAN        ; calculate atan(x)
        pop     BP              ; restore BP
endif
        ret_pop _POP_LD_        ; return
        endproc atanl_

        defp    atan2l_
ifdef __386__
        fld     tbyte ptr argx+_POP_LD_[ESP]; load x
        fld     tbyte ptr argx[ESP]; load y
        call    IF@DATAN2       ; calculate atan2(y,x)
        loadres                 ; load result
else
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        fld     tbyte ptr argx+_POP_LD_[BP]; load x
        fld     tbyte ptr argx[BP]; load y
        call    IF@DATAN2       ; calculate atan2(y,x)
        pop     BP              ; restore BP
endif
        ret_pop _POP_LD_*2      ; return
        endproc atan2l_

        endmod
        end
